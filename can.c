#include "can.h"

int initCanSocket(int bitrate)
{
	int fd = -1;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_filter filter[2];
	char cmd[100];
	FILE *fp;

    if ((fp = popen("ifconfig can0 down", "r")) == NULL) {
        printf("popen1 error");
        return -1;
    }
    pclose(fp);

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "canconfig can0 bitrate %d ctrlmode triple-sampling on", bitrate);
    if ((fp = popen(cmd, "r")) == NULL) {
        printf("popen2 error");
        return -1;
    }
    pclose(fp);

    if ((fp = popen("ifconfig can0 up", "r")) == NULL) {
        printf("popen3 error");
        return -1;
    }
    pclose(fp);

	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	strcpy(ifr.ifr_name, "can0");
	ioctl(fd, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	
	filter[0].can_id = 0x123;
	filter[0].can_mask = (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_SFF_MASK);
	filter[1].can_id = 0x12345678 | CAN_EFF_FLAG;
	filter[1].can_mask = (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_SFF_MASK);
	setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
	
	return fd;
}

int sendData(int fd, int canid, char *data, int len)
{
	int sendBytes = 0, i;
	struct can_frame frame;

	memset(&frame, 0, sizeof(frame));
	frame.can_id = canid;
	frame.can_dlc = len;
	memcpy(frame.data, data, len);
	sendBytes = write(fd, &frame, sizeof(frame));
	if(sendBytes != sizeof(frame)){
		printf("send error[%d]! sendBytes: %d, framesize: %d\n", errno, sendBytes, sizeof(frame));
	}
	else{
		printf("send: 0x%08x | %d | ", frame.can_id, frame.can_dlc);
		for(i = 0; i < frame.can_dlc; i++)
			printf("0x%02x ", frame.data[i]);
		printf("\n");
	}
	return sendBytes;
}

int recvData(int fd, struct can_frame *frame)
{
	int recvBytes = 0, i, nfds;
	fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 200000;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    nfds = select(fd+1, &rfds, NULL, NULL, &tv);
	if(nfds > 0){
		memset(frame, 0, sizeof(struct can_frame));
		recvBytes = read(fd, frame, sizeof(struct can_frame));
		if(recvBytes > 0){
			printf("recv: 0x%08x | %d | ", frame->can_id, frame->can_dlc);
			for(i = 0; i < frame->can_dlc; i++)
				printf("0x%02x ", frame->data[i]);
			printf("\n");
		}
	}
	return recvBytes;	
}