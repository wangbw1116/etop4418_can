#ifndef __CAN_H__
#define __CAN_H__

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int initCanSocket(int bitrate);
int sendData(int fd, int canid, char *data, int len);
int recvData(int fd, struct can_frame *frame);

#endif // !__CAN_H__