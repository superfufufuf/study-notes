// 如何设置DRM模式
// 本篇文档主要描述了关于DRM模式的应用编程接口，在使用之前应该包含 xf86drm.h和xf86drmMode.h
// 这两个头文件，默认在libdrm的每个默认主要发行版都会提供，且依赖性很小。
// 请忽略前面这些后面会用到的函数声明，我重新编排了这些函数，以便于能够从头到尾的顺序阅读本篇
// 文档，若你想要重新实现它的话，可以重新编排一下函数的位置从而去掉前面这些讨厌的函数声明。
// 为了便于阅读，我们忽略关于malloc() 和friends的一些内存分配错误。
// 本篇文章所有的函数和全局变量都用“modest_ *”作为前缀。故可知函数是否是本地helper还是由外部库
// 提供的。

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h> //open
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>	 //usr/include
#include <xf86drmMode.h> //usr/include

struct modeset_dev;
static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn,
							 struct modeset_dev *dev);
static int modeset_create_fb(int fd, struct modeset_dev *dev);
static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn,
							 struct modeset_dev *dev);
static int modeset_open(int *out, const char *node);
static int modeset_prepare(int fd);
static void modeset_draw(void);
static void modeset_cleanup(int fd);

// 当linux内核检测到你的机器上有显卡，它加载正确的设备驱动程序(位于内核树。/drivers/gpu/drm/<xy>) 并提供两个字符设备来控制它。Udev(或者任何热插拔的应用程序)
// 将创建它们为 :
//  dev/dri/card0
//  dev/dri/controlID64
// 我们只需要第一个。您可以将此路径硬编码到应用程序中就像我们在这里做的，但建议 使用libudev与真正的热插拔和多座支持。
// 但是，这超出了本文的范围。 还要注意的是，如果你有多个显卡，也可能有：
// dev/driver/card1，/dev/driver/card2，…

// modeset_open(out, node) : 该函数用于打开DRM设备表示为： @node.成功后，新的fd将存储在 @out中。失败,
// 返回一个负的错误代码。
static int modeset_open(int *out, const char *node)
{
	int fd, ret;
	uint64_t has_dumb;

	fd = open(node, O_RDWR | O_CLOEXEC);
	//  打开node文件，正常返回文件描述符，失败返回 - 1
	if (fd < 0)
	{
		// 失败输出错误信息
		ret = -errno;
		fprintf(stderr, "cannot open '%s': %m\n", node);
		return ret;
	}

	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb)
	{
		fprintf(stderr, "drm device '%s' does not support dumb buffers\n", node);
		close(fd);
		return -EOPNOTSUPP;
	}

	*out = fd;
	return 0;
}

// 下一步我们需要找到可用的显示设备。libdrm提供drmModeRes结构，包含所有需要的信息。
// 我们可以通过drmModeGetResources(fd) 检索它，并通过drmModeFreeResources(res) 释放它。
// 显卡上的物理连接器称为“连接器”（connector）。你可以插入显示器并可控制其显示内容。
// 我们肯定对现在使用的是哪个连接器感兴趣，因此我们简单地刷新连接器列表，并尝试在可用的显
// 示屏上显示测试图片。
// 然而，这并不像听起来那么容易。
// 首先，我们需要检查连接器是否实际使用(显示器插入并打开)。 然后,
// 我们需要找到一个可以控制这个连接器的CRTC（CRTC之后再描述）
// 之后我们创建一个framebuffer对象。如果我们拥有这一切，我们可以mmap() 帧缓冲区，并
// 在其中绘制一个测试图片。然后我们可以告诉DRM设备使用选定的连接器在给定的CRTC上显示framebuffer。
// 当我们想要在framebuffer上绘制移动的图片时，我们必须这样做记住所有这些设置。因此，
// 我们创建一个 "struct modeset_dev"对象的每个成功的connector +
// crtc + framebuffer对初始化 并将其推入全局设备列表。 这种结构的每个字段在第一次使用时都会被描述。
// 作了一个总结 : "struct modeset_dev"包含:
// {
// 	- @next : 指向单链表中的下一个设备
//  - @width : 缓冲区对象的宽度
//  - @height : 缓冲区对象的高度
//  - @stride : 缓冲区对象的stride值
//  - @size : 内存映射缓冲区的大小
//  - @handle : 我们可以绘制的缓冲区对象的DRM句柄
//  - @map : 指向内存映射缓冲区的指针
//  - @mode : 我们想使用的显示模式
//  - @fb : 一个缓冲对象作为扫描缓冲区的framebuffer句柄
//  - @conn : 我们想在这个缓冲区中使用的连接器ID
//  - @crtc : 我们想在这个连接器中使用的crtc ID
//  - @saved_crtc : 我们更改crtc之前的配置。我们使用它这样我们可以在退出时恢复相同的模式。
// }

struct modeset_dev
{
	struct modeset_dev *next;
	// 指向下一个设备指针
	uint32_t width;
	// 缓冲区对象的宽度
	uint32_t height;
	// 缓冲区对象的高度
	uint32_t stride;
	// 缓冲区对象的步幅
	uint32_t size;
	// 缓冲区对象的大小
	uint32_t handle;
	// 句柄
	uint8_t *map;
	// 指向内存映射缓冲区的指针
	drmModeModeInfo mode;
	// 显示模式
	uint32_t fb;
	// framebuffer
	uint32_t conn;
	// 连接器ID
	uint32_t crtc;
	// crtc ID
	drmModeCrtc *saved_crtc;
	// 更改crtc前的配置
};

static struct modeset_dev *modeset_list = NULL;

// 因此，下一步我们需要实际准备所有找到的连接器（下面这个函数）。
// modeset_prepare(fd) : 该函数接受DRM fd作为参数和然后简单地从设备检索资源信息。
// 然后迭代通过所有连接器，并调用其他辅助函数来初始化这个连接器(稍后描述)。 如果初始化成功，我们只需添加这个对象作为新设备进入全局modesset设备列表。
// 资源结构包含一个所有connector- id的列表。我们使用函数drmModeGetConnector() 检索更多信息的每个连接器。在我们用完它之后，我们再次释放它drmModeFreeConnector()。
// modeset_setup_dev() 返回 - ENOENT如果连接器是当前未使用且未插入显示器。可忽略这个连接器。

static int modeset_prepare(int fd)
{
	drmModeRes *res;
	drmModeConnector *conn;
	int i;
	struct modeset_dev *dev;
	int ret;

	/*----------检索资源----------*/
	res = drmModeGetResources(fd);
	if (!res)
	{
		// 检索失败则报错
		fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n", errno);
		return -errno;
	}

	/*----------遍历所有的连接器----------*/
	for (i = 0; i < res->count_connectors; ++i)
	{
		/*----------获取每个连接器的信息----------*/
		conn = drmModeGetConnector(fd, res->connectors[i]);
		if (!conn)
		{
			fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",
					i, res->connectors[i], errno);
			continue;
		}

		/*----------创建设备结构----------*/
		dev = static_cast<modeset_dev *>(malloc(sizeof(*dev)));
		memset(dev, 0, sizeof(*dev));
		dev->conn = conn->connector_id;

		/*----------调用函数来准备这个连接器（连接器的准备）----------*/
		ret = modeset_setup_dev(fd, res, conn, dev);
		if (ret)
		{
			if (ret != -ENOENT)
			{
				errno = -ret;
				fprintf(stderr, "cannot setup device for connector %u:%u (%d): %m\n",
						i, res->connectors[i], errno);
			}
			free(dev);
			drmModeFreeConnector(conn);
			continue;
		}
		/*----------释放 连接器数据和链接设备到全局列表----------*/
		drmModeFreeConnector(conn);
		dev->next = modeset_list;
		modeset_list = dev;
	}
	/*----------再次释放资源----------*/
	drmModeFreeResources(res);
	return 0;
}

// 现在我们深入研究如何设置单个连接器。如前所述,我们首先需要检查几件事:
// 1.若连接器目前未使用，即没有插入监视器，我们可以忽略它。
// 2.我们必须找到一个合适的分辨率(resolution)和刷新率(refresh-rate）
// 这一切都是可在每个crtc的drmModeModeInfo结构保存。我们只是使用第一种模式。
// 这总是与模式最高的分辨率。
// 但是，在实际的应用程序中应该进行更复杂的模式选择。
// 3.我们需要找到一个可以驱动这个连接器的CRTC。CRTC是每个显卡的内部资源。
// crtc的数量控制着可以独立控制的连接器的数量。也就是说，一个显卡可能比crtc有更
// 多的连接器，这意味着不是所有的监视器都可以被独立控制。
// 如果监视器显示相同的内容，实际上可以通过一个CRTC控制多个连接器。但是，我
// 们在这里不使用它。
// 把连接器想象成连接监视器和crtc是管理哪个数据进入哪个管道的控制器。如果
// 管道比crtc多，我们就不能同时控制所有管道。
// 4.我们需要为这个连接器创建一个framebuffer。framebuffer是一个我们可以写入
// XRGB32数据的内存缓冲区。因此，我们使用它来呈现我们的图形，然后CRTC可以将数据
// 从帧缓冲区扫描到监视器上。

static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn,
							 struct modeset_dev *dev)
{
	int ret;

	/*----------检查是否有显示器连接----------*/
	if (conn->connection != DRM_MODE_CONNECTED)
	{
		fprintf(stderr, "ignoring unused connector %u\n",
				conn->connector_id);
		return -ENOENT;
	}

	/*----------检查是否至少有一个有效的模式----------*/
	if (conn->count_modes == 0)
	{
		fprintf(stderr, "no valid mode for connector %u\n",
				conn->connector_id);
		return -EFAULT;
	}

	/*----------复制模式信息到我们的设备结构----------*/
	memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
	dev->width = conn->modes[0].hdisplay;
	dev->height = conn->modes[0].vdisplay;
	fprintf(stderr, "mode for connector %u is %ux%u\n",
			conn->connector_id, dev->width, dev->height);

	/*----------为这个连接器找到一个crtc----------*/
	ret = modeset_find_crtc(fd, res, conn, dev);
	if (ret)
	{
		fprintf(stderr, "no valid crtc for connector %u\n",
				conn->connector_id);
		return ret;
	}

	/*----------为这个CRTC创建一个framebuffer----------*/
	ret = modeset_create_fb(fd, dev);
	if (ret)
	{
		fprintf(stderr, "cannot create framebuffer for connector %u\n",
				conn->connector_id);
		return ret;
	}

	return 0;
}

// modeset_find_crtc(fd, res, conn, dev) : 该函数试图为给定的连接器找到合适的CRTC。实际上， 我们必须再引入一个DRM对象来让这个问题更加清楚 : 编码器（Encoders）。
// 编码器帮助CRTC将数据从framebuffer转换为可用于所选连接器的正确格式。我们不需要了解更多的
// 这些转换来使用它。但是，您必须知道每个连接器都有一个它可以使用的编码器的有限列表。每个编码器只
// 能与有限的crtc列表一起工作。所以我们要做的是尝试每一个编码器是可用的，并寻找一个CRTC，这个编码
// 器可以工作。如果我们找到了第一个可行的组合，我们会很高兴地把它写入 @dev结构中。 但在迭代所有可用的编码器之前，
// 我们首先在连接器上尝试当前活动的encoder + crtc，以避免完整的模式集。
// 但是，在我们使用CRTC之前，我们必须确保没有其他设备(我们之前设置的设备)已经在使用这个CRTC。
// 请记住，每个CRTC只能驱动一个连接器 !因此，我们只需遍历以前设置设备的“modeset_list”，并检查这个
// CRTC以前是否使用过。否则，我们继续使用下一个CRTC Encoder组合。

static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn,
							 struct modeset_dev *dev)
{
	drmModeEncoder *enc;
	int i, j;
	uint32_t crtc;
	struct modeset_dev *iter;

	/*----------首先尝试当前连接的编码器+crtc----------*/
	if (conn->encoder_id)
		enc = drmModeGetEncoder(fd, conn->encoder_id);
	else
		enc = NULL;

	if (enc)
	{
		if (enc->crtc_id)
		{
			crtc = enc->crtc_id;
			for (iter = modeset_list; iter; iter = iter->next)
			{
				if (iter->crtc == crtc)
				{
					crtc = -1;
					break;
				}
			}

			if (crtc >= 0)
			{
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	// 如果连接器目前没有绑定到一个编码器，或者encoder +
	// crtc已经被另一个连接器使用(实际上不太可能，但让我们安全)，迭代所有其他可用的编码器来找到匹配的crtc。
	for (i = 0; i < conn->count_encoders; ++i)
	{
		enc = drmModeGetEncoder(fd, conn->encoders[i]);
		if (!enc)
		{
			fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n",
					i, conn->encoders[i], errno);
			continue;
		}

		/*----------迭代所有全局CRTCs----------*/
		for (j = 0; j < res->count_crtcs; ++j)
		{
			/*----------检查此CRTC是否与编码器一起工作----------*/
			if (!(enc->possible_crtcs & (1 << j)))
				continue;

			/*----------检查是否有其他设备已经使用此CRTC----------*/
			crtc = res->crtcs[j];
			for (iter = modeset_list; iter; iter = iter->next)
			{
				if (iter->crtc == crtc)
				{
					crtc = -1;
					break;
				}
			}
			/*----------我们已经找到一个CRTC，所以保存它并返回----------*/
			if (crtc >= 0)
			{
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}
		drmModeFreeEncoder(enc);
	}

	fprintf(stderr, "cannot find suitable CRTC for connector %u\n",
			conn->connector_id);
	return -ENOENT;
}

// modeset_create_fb(fd, dev) : 在我们找到crtc +连接器 + 模式组合后，我们需要实际创建一个 合适的框架缓冲区，我们可以使用它。实际上有两种方法可以做到这一点 :
// 方法一：我们可以创建一个所谓的“哑缓冲区（dumb buffer）”。这是一个我们可以通过 mmap() 内存映射的缓冲区，每个驱动程序都支持它。我们可以使用它在CPU上进行非加速的软件渲染。
// 方法二：我们可以使用libgbm创建用于硬件加速的缓冲区。libgbm是一个抽象层，它为每个可用的DRM驱动程序创建这些缓冲区。由于没有通用API，每个驱动程序都提供了自己的方式来创建这些缓冲区。
// 然后我们可以使用这样的缓冲区来创建OpenGL上下文与mesa3D库。 我们在这里使用第一种解决方案，因为它简单得多，而且不需要任何外部库。
// 然而，如果你想通 过OpenGL使用硬件加速，实际上用libgbm和libEGL创建缓冲区是相当容易的。但这超出了本文的讨论范围。 所以我们要做的是从驱动程序请求一个新的哑缓冲区（dumb buffer）。
// 我们指定的大小与我们为 连接器选择的当前模式相同。然后我们请求驱动程序为内存映射准备这个缓冲区。之后，我们执行实际 的mmap() 调用。所以我们现在可以通过dev->map内存map直接访问framebuffer内存。

static int modeset_create_fb(int fd, struct modeset_dev *dev)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	/*----------创建dumb buffer----------*/
	memset(&creq, 0, sizeof(creq));
	creq.width = dev->width;
	creq.height = dev->height;
	creq.bpp = 32;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0)
	{
		fprintf(stderr, "cannot create dumb buffer (%d): %m\n",
				errno);
		return -errno;
	}
	dev->stride = creq.pitch;
	dev->size = creq.size;
	dev->handle = creq.handle;

	/*----------为dumb-buffer创建framebuffer对象----------*/
	ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride,
					   dev->handle, &dev->fb);
	if (ret)
	{
		fprintf(stderr, "cannot create framebuffer (%d): %m\n",
				errno);
		ret = -errno;
		goto err_destroy;
	}

	/*----------为内存映射准备缓冲区----------*/
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = dev->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret)
	{
		fprintf(stderr, "cannot map dumb buffer (%d): %m\n",
				errno);
		ret = -errno;
		goto err_fb;
	}

	/*----------执行实际的内存映射----------*/
	dev->map = static_cast<uint8_t *>(mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset));
	if (dev->map == MAP_FAILED)
	{
		fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n",
				errno);
		ret = -errno;
		goto err_fb;
	}

	/*----------将framebuffer清除为0----------*/
	memset(dev->map, 0, dev->size);

	return 0;

err_fb:
	drmModeRmFB(fd, dev->fb);
err_destroy:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = dev->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	return ret;
}

// CRTC（Cathode Ray Tube Controller）在图形设备中起着至关重要的作用，它负责控制显示设备的硬件资源以及将扫描线数据转换为屏幕上的可见像素。以下是CRTC的具体作用：
// 显示控制：CRTC负责确定要在显示器上显示的内容以及如何显示。它可以配置显示区域的位置、大小和刷新率，以决定如何将图形数据渲染到屏幕上。
// 时序控制：CRTC负责产生水平和垂直同步信号，以确保屏幕上的图像能够按照正确的顺序进行刷新，避免出现撕裂或闪烁等视觉问题。
// 帧缓冲管理：CRTC与帧缓冲（framebuffer）交互，它选择合适的帧缓冲并将其内容发送到显示器，从而实现在屏幕上显示图像的功能。
// 多路复用：对于支持多个显示输出的图形设备，CRTC还可以对不同的显示输出进行多路复用，使得多个显示器可以同时工作，或者应用分别显示不同内容。
// 总的来说，CRTC在图形设备中扮演着“控制中心”的角色，它负责协调各种硬件资源，确保图像数据以正确的方式渲染到显示器上，从而实现高质量的图形显示。

// 终于 !我们有一个连接器与合适的CRTC。我们知道我们想要使用哪种模式，并且我们有一个
// 大小正确的framebuffer可以写入。没有什么特别的事情要做了。我们只需要对CRTC进行编程，
// 使其将每个新的framebuffer连接到我们保存在全局modeset_list中的每个组合的每个选定连
// 接器。这是通过调用drmModeSetCrtc() 来完成的。
// 现在我们准备好使用main() 函数了。首先，我们检查用户是否在命令行上指定了DRM设备，
// 否则我们使用默认的
// dev/drim/card0。然后通过modeset_open() 打开设备。 modeset_prepare() 准备所有连接器，我们可以循环遍历“modeset_list”，并在每个CRTC
// 连接器组合上调用drmModeSetCrtc()。但是打印空白的黑色页面很无聊，所以我们有另一个函数modeset_draw()，
// 它在framebuffer中绘制一些颜色，持续5秒，然后返回。然后，我们有 所有的清理功能，正确地释放所有设备后，我们使用它们。所有这些函数都在main() 函数下面进行了描述。
// 附注 : drmModeSetCrtc() 实际上接受一个我们想要用这个CRTC控制的连接器列表。但是，我们 只传递了一个连接器。
// 如前所述，如果我们使用多个连接器，那么所有连接器都将具有相同的控制 framebuffer，因此输出将被克隆。
// 这通常不是你想要的，所以我们避免在这里解释这个特性。
// 此外， 所有连接器都必须以相同的模式运行，这通常也不能保证。因此，每个CRTC只使用一个连接器。
// 在调用drmModeSetCrtc() 之前，我们还保存当前的CRTC配置。这是在modeset_cleanup() 中使 用的，用于将CRTC恢复到更改之前的相同模式。
// 如果我们不这样做，在我们退出后，屏幕将保持空白，直到另一个应用程序执行modesetting本身。

int main(int argc, char **argv)
{
	int ret, fd;
	const char *card;
	struct modeset_dev *iter;
	/*----------检查打开哪个DRM设备----------*/
	if (argc > 1)
		card = argv[1];
	else
		card = "/dev/dri/card0";
	fprintf(stderr, "using card '%s'\n", card);
	/*----------打开DRM设备---------*/
	ret = modeset_open(&fd, card);
	if (ret)
		goto out_return;
	/*----------准备好所有连接器和crtcs----------*/
	ret = modeset_prepare(fd);
	if (ret)
		goto out_close;
	/*----------对每个找到的连接器+CRTC执行实际的模式设置（modesset）----------*/
	for (iter = modeset_list; iter; iter = iter->next)
	{
		iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
		ret = drmModeSetCrtc(fd, iter->crtc, iter->fb, 0, 0,
							 &iter->conn, 1, &iter->mode);
		if (ret)
			fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n",
					iter->conn, errno);
	}
	/*----------画一些颜色5秒----------*/
	modeset_draw();
	/*----------清理所有的----------*/
	modeset_cleanup(fd);
	ret = 0;
out_close:
	close(fd);
out_return:
	if (ret)
	{
		errno = -ret;
		fprintf(stderr, "modeset failed with error %d: %m\n", errno);
	}
	else
	{
		fprintf(stderr, "exiting\n");
	}
	return ret;
}

// -------------------------------------------------------
//     一个简短的辅助函数来计算变化的颜色值。没必要去理解它
// -------------------------------------------------------

static uint8_t next_color(bool *up, uint8_t cur, unsigned int mod)
{
	uint8_t next;

	next = cur + (*up ? 1 : -1) * (rand() % mod);
	if ((*up && next < cur) || (!*up && next > cur))
	{
		*up = !*up;
		next = cur;
	}

	return next;
}

// modeset_draw():在所有已配置的framebuffer中绘制纯色。每100毫秒，颜色就会变化为一种稍微
// 不同的颜色，所以我们会得到某种平滑变化的颜色梯度。
// 颜色计算可以忽略，因为它很无聊。有趣的是遍历"modeset_list"然后遍历所有的行和宽。然后我们
// 将每个像素分别设置为当前的颜色。
// 我们会在每次重划后100毫秒睡觉时这样做50次。这使得50*100ms = 5000ms = 5s，所以需要5秒才
// 能完成这个循环。
// 请注意，我们直接在framebuffer中绘制。这意味着当我们重新绘制屏幕时，当显示器刷新时，您将看
// 到闪烁。为了避免这种情况，您需要使用两个帧缓冲区和对drmModeSetCrtc()的调用来在两个缓冲区之间进行切换。
// 你也可以使用drmModePageFlip()来做垂直同步的页面翻转。但这超出了本文的讨论范围。

static void modeset_draw(void)
{
	uint8_t r, g, b;
	bool r_up, g_up, b_up;
	unsigned int i, j, k, off;
	struct modeset_dev *iter;

	srand(time(NULL));
	r = rand() % 0xff;
	g = rand() % 0xff;
	b = rand() % 0xff;
	r_up = g_up = b_up = true;

	for (i = 0; i < 50; ++i)
	{
		r = next_color(&r_up, r, 20);
		g = next_color(&g_up, g, 10);
		b = next_color(&b_up, b, 5);

		for (iter = modeset_list; iter; iter = iter->next)
		{
			for (j = 0; j < iter->height; ++j)
			{
				for (k = 0; k < iter->width; ++k)
				{
					off = iter->stride * j + k * 4;
					*(uint32_t *)&iter->map[off] =
						(r << 16) | (g << 8) | b;
				}
			}
		}

		usleep(100000);
	}
}

// ------------------------------------------------------------
//   modeset_cleanup(fd): 这将清理我们在此期间创建的所有设备
//   modeset_prepare(). 它将crtc重置为它们保存的状态，并释放所有内存。
//   这一切是如何运作的应该很明显了。
// ------------------------------------------------------------

static void modeset_cleanup(int fd)
{
	struct modeset_dev *iter;
	struct drm_mode_destroy_dumb dreq;

	while (modeset_list)
	{
		/*----------从全局列表中删除----------*/
		iter = modeset_list;
		modeset_list = iter->next;

		/*----------恢复保存的CRTC配置----------*/
		drmModeSetCrtc(fd,
					   iter->saved_crtc->crtc_id,
					   iter->saved_crtc->buffer_id,
					   iter->saved_crtc->x,
					   iter->saved_crtc->y,
					   &iter->conn,
					   1,
					   &iter->saved_crtc->mode);
		drmModeFreeCrtc(iter->saved_crtc);

		/*---------- unmap buffer----------*/
		munmap(iter->map, iter->size);

		/*----------删除framebuffer----------*/
		drmModeRmFB(fd, iter->fb);

		/*----------删除dumb buffer----------*/
		memset(&dreq, 0, sizeof(dreq));
		dreq.handle = iter->handle;
		drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

		/*----------自由分配的内存（释放分配的内存）----------*/
		free(iter);
	}
}

// 我希望这是对DRM modesetting API简短但简单的概述。DRM API提供了更多的功能，包括 : -双缓冲或三缓冲(或任何你想要的) -
// vsync使用翻页
// - 硬件加速渲染(例如通过OpenGL) - 输出克隆
// - 图形客户端 + 认证
// - DRM飞机 / 覆盖 / 精灵
// - ... 如果你对这些主题感兴趣，我目前只能将你重定向到现有的实现，包括 :
// - plymouth(它使用了像这个例子一样的哑缓冲区; 非常容易理解)
// - kmscon(使用libuterm来做这个)
// - wayland(非常复杂的DRM渲染器; 很难完全理解，因为它使用更复杂的技术，如DRM飞机)
// - xserver(很难理解，因为它被分割在很多文件项目中) 但是理解modesset(如本文档所述)
// 是如何工作的，这对于理解所有后续的DRM主题是至关重要的。 欢迎任何反馈。您可以自由地将这些代码用于自己的文档或项目。)
