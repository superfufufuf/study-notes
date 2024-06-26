#ifndef VIRTKO_H
#define VIRTKO_H

// ioctl
// ioctl的命令具有一定的规则性，一共32位：2bit dir， 14bit size， 8bit type， 8bit nr
// 1. dir（direction），ioctl 命令访问模式（数据传输方向），占据 2 bit，可以为 _IOC_NONE、_IOC_READ、_IOC_WRITE、_IOC_READ | _IOC_WRITE，分别指示了四种访问模式：无数据、读数据、写数据、读写数据；
// 2. size，涉及到 ioctl 函数第三个参数 arg ，占据14bit，指定了 arg 的数据类型及长度；
// 3. type（device type），设备类型，占据 8 bit，可以为任意 char 型字符，例如‘a’、’b’、’c’ 等等，其主要作用是使 ioctl 命令有唯一的设备标识；
// 4. nr（number），命令编号/序数，占据 8 bit，可以为任意 unsigned char 型数据，取值范围 0~255，如果定义了多个 ioctl 命令，通常从 0 开始编号递增
#define  MEM_CLEAR       _IO("virtko", 0x01)

#endif //VIRTKO_H