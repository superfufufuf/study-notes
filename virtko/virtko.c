#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/poll.h>

#include "virtko.h"

static char *test_init_data __initdata = "hello, ";

// dri dependent
static char *ko_info = "world";
const char *device_name = "virtko";

// dev dependent
struct cdev myCdev;
dev_t myDev;

// module param
module_param(ko_info, charp, S_IRUGO);

// private data
#define DataSize 100
struct MyData
{
	char data[DataSize];
	struct mutex mtx;
	wait_queue_head_t waitq;
	int refCount;
};
static struct MyData *memData = NULL;


int virtko_add(int a, int b)
{
	printk("use virtko's add(%d, %d)\n", a, b);
	return a + b;
}
EXPORT_SYMBOL_GPL(virtko_add);

int virtko_open(struct inode *inode, struct file *filp)
{
	mtx.lock();
	filp->private_data = memData;
	memData->refCount++;
	printk("virtko_open, refCount: %d \n", memData->refCount);
	return 0;
}

int virtko_release(struct inode *inode, struct file *filp)
{
	memData->refCount--;
	printk("virtko_release, refCount: %d \n", memData->refCount);
	return 0;
}

loff_t virtko_llseek(struct file *filp, loff_t ppos, int whence)
{
	printk("virtko_llseek, ppos: %d, whence: %d.\n", (int)ppos, (int)whence);
	loff_t new_pos = 0;

	switch (whence)
	{
	case SEEK_SET:
		if (ppos < 0)
		{
			new_pos = -EINVAL;
			break;
		}
		if ((unsigned int)ppos > DataSize)
		{
			new_pos = -EINVAL;
			break;
		}
		filp->f_pos = (unsigned int)ppos;
		new_pos = filp->f_pos;
		break;
	case SEEK_CUR:
		if ((filp->f_pos + ppos) > DataSize)
		{
			new_pos = -EINVAL;
			break;
		}
		if ((filp->f_pos + ppos) < 0)
		{
			new_pos = -EINVAL;
			break;
		}
		filp->f_pos += ppos;
		new_pos = filp->f_pos;
		break;
	case SEEK_END:
		new_pos = DataSize + ppos;
		break;
	default:
		return -EINVAL;
	}

	if (new_pos < 0 || new_pos > DataSize)
		return -EINVAL;

	filp->f_pos = new_pos;
	return new_pos;
}

long virtko_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
	struct MyData *myData = filp->private_data;
    switch(cmd)
    {
        case MEM_CLEAR:
            memset(myData->data, 0, DataSize);
            printk(KERN_INFO "data is set zero");
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

ssize_t virtko_read(struct file *filp, char __user *buffer, size_t count, loff_t *ppos)
{
	// return:
	// < 0: error.
	// == 0: EOF.
	// > 0: the counts whice you read.
	printk("virtko_read, count: %d, ppos: %d\n", (int)count, (int)*ppos);
	struct MyData *myData = filp->private_data;
	int finalPos = *ppos;
	int finalCount = count;

	if (finalPos >= DataSize)
	{
		return 0;
	}

	if (finalCount > DataSize - finalPos)
	{
		finalCount = DataSize - finalPos;
	}
	
	ssize_t retval = 0;
	if (copy_to_user(buffer, myData->data + finalPos, finalCount))
	{
		retval = -EFAULT;
	}
	else
	{
		*ppos += finalCount;
		retval = finalCount;
		printk("read %u bytes(s) from %lu\n", finalCount, finalPos);
	}
	return retval;
}

ssize_t virtko_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos)
{
	printk("virtko_write, count: %d, ppos: %d\n", (int)count, (int)*ppos);
	struct MyData *myData = filp->private_data;
	int finalPos = *ppos;
	int finalCount = count;

	if (finalPos >= DataSize)
	{
		return 0;
	}

	if (finalCount > DataSize - finalPos)
	{
		finalCount = DataSize - finalPos;
	}
	
	ssize_t retval = 0;
	if (copy_from_user(myData->data + finalPos, buffer, finalCount))
	{
		retval = -EFAULT;
	}
	else
	{
		*ppos += finalCount;
		retval = finalCount;
		printk("written %u bytes(s) from %lu\n", finalCount, finalPos);
	}
	return retval;
}

__poll_t virtko_poll(struct file *filp, struct poll_table_struct *wait)
{
	printk("virtko_poll.\n");
	struct MyData *myData = filp->private_data;
	poll_wait(filp, &myData->waitq, wait);

	// todo

	return 0;
}

struct file_operations myFo = {
	.owner = THIS_MODULE,
	.read = virtko_read,
	.write = virtko_write,
	.poll = virtko_poll,
	.open = virtko_open,
	.release = virtko_release,
	.llseek = virtko_llseek,
	.unlocked_ioctl = virtko_ioctl,
};

static int __init ko_init(void)
{
	// request_module(module_name);
	printk(KERN_INFO "ko init, %s%s.\n", test_init_data, ko_info);
	memData = kzalloc(sizeof(struct MyData), GFP_KERNEL);

	if (memData == NULL)
	{
		printk("create data faild.\n");
		return -1;
	}

	int ret = alloc_chrdev_region(&myDev, 0, 1, device_name);
	if (ret < 0)
	{
		printk("%s driver register failed\n", device_name);
	}

	cdev_init(&myCdev, &myFo);
	myCdev.owner = THIS_MODULE;

	ret = cdev_add(&myCdev, myDev, 1);

	if (ret < 0)
	{
		printk("cdev_add fail \n");
		return ret;
	}

	return 0;
}

static void __exit ko_exit(void)
{
	if (memData != NULL)
	{
		kfree(memData);
		memData = NULL;
	}
	cdev_del(&myCdev);
	unregister_chrdev_region(myDev, 1);
	printk(KERN_INFO "ko exit.\n\n");
}

module_init(ko_init);
module_exit(ko_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("v1.0.0");
MODULE_AUTHOR("td");
MODULE_DESCRIPTION("hello, kernel");