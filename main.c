#include "can.h"

int main(int argc, char *argv[])
{
	int s, pid, bitrate;
	if(argc <= 1){
		printf("Bitrate unset!\n");
		return -1;
	}
	bitrate = atoi(argv[1]);
	//init can_socket
	s = initCanSocket(bitrate);
	if(s <= 0){
		printf("Init Cansocket failed.\n");
		return -1;
	}
	else{
		printf("Init Cansocket succeed, bitrate: %d\n", bitrate);
	}

	//create can_process
	pid = fork();
	if(pid == 0){
		printf("pid: %d\n", pid);
		//recvData
		struct can_frame frame;
		int recvBytes;
		while(1){
			recvBytes = recvData(s, &frame);
			//usleep(100000);
		}
	}	
	else{
		printf("pid: %d\n", pid);
		//send data
		int sendCnt = 0, i;
		char data[8] = {0};
		while(1){
			for(i = 0; i < 8; i++){
				data[i] = sendCnt % 0xFF;
			}
			int sendBytes = sendData(s, 0x123, data, sizeof(data));
			sendCnt++;
			sleep(1);
		}
	}
	
	return 0;
}
