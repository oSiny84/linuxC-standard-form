#include <stdio.h>
#include <stdint.h>
#include <unistd.h> 
#include <pthread.h> 
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "tcp.h"
#include "util.h"

#define BUF_SIZE 1024     
#define TCPPORT 5000
#define NUMTCPWORKERS 100
void* dummyPointer;

static int socketOpenCount = 0;
static int run = 1;

pthread_t tcpworker_p[NUMTCPWORKERS];
int tcpworker_i[NUMTCPWORKERS];
int socketClientList[NUMTCPWORKERS];

void tcp_init()
{

}

void* tcpWorker(void *arg)
{
	int idx = *((int*)arg);
    
	int strLen;
	char message[BUF_SIZE] ={0,};
	char data[BUF_SIZE] = {0,};

    while(run)
    {
    	sleep_ms(1000);

    	if( socketClientList[idx] != 0)
    	{	
            while((strLen = recv(socketClientList[idx], message, BUF_SIZE, 0)) !=0)       
            {   
                if (strLen == -1)
                {
                    socketOpenCount--;
                    printf("disconnect  client \n"); 
                    break;
                    
                }
                memset(data, 0, BUF_SIZE);
                memcpy(data, message, strLen);
				//insert(data);
                printf("data:%s  recv_len = %d \n", data, strLen);
                send(socketClientList[idx], message, strLen, 0);
            }

            close(socketClientList[idx]);
            socketClientList[idx] = 0;

			printf("TCP Count = %d\n", ++socketOpenCount);
    	}
    }
    socketOpenCount--;
    //printf("TCP Client thread is closed. Number of clients : %d", socketOpenCount);
}

int create_thread(pthread_t *thread_handle, void *arg)
{
	int thrid;
	thrid = pthread_create(thread_handle , NULL , tcpWorker , arg);
	sleep_ms(10);
	return thrid;
}

int lSocket;
void* tcp_thd_run(void *arg)
{
    printf("TCP Server is running... \r\n");
    struct sockaddr_in sLocalAddr;
    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen=sizeof(client_addr);
    int optval;
    int optlen = sizeof(optval);
    int status;
    int taskHandle;
    int taskParams[NUMTCPWORKERS];
    
    int i;
    int bSockOpen;
	static int cntRetryBind = 0;	

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));	
    run = 1;

ReBinding:	
    if( !run )
        goto dispose;

    lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (lSocket < 0) {
        printf("tcpHandler: socket failed\n");
        return dummyPointer;
    }

	if ( setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ){	//--0113
		printf("tcpHandler: setsockopt failed\n");

		close(lSocket);
		return dummyPointer;
	}

    sLocalAddr.sin_family = AF_INET;

    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(TCPPORT);


	status = bind(lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr));
	if (status<0){
		printf("########## PORT : %d / BIND : %d\n", TCPPORT, status);
		cntRetryBind++;
		printf("########## tcpHandler : Bind failed [%3d]\n", cntRetryBind);
		if (cntRetryBind >= 100){
			cntRetryBind = 0;

			close(lSocket);
 			printf("########## tcpHandler : TCP task was stopped by binding error\n");
			return dummyPointer;
		}

		close(lSocket);
		printf("########## tcpHandler : Socket will be closed by binding error [%3d]\n", cntRetryBind);
	
		sleep_ms(1000);
		goto ReBinding;
	}
	else{
		printf("PORT : %d / BIND : %d\r\n", TCPPORT, status);
	}

    if (listen(lSocket, NUMTCPWORKERS) != 0)
    {
        printf("tcpHandler: listen failed\n");
	    close(lSocket);
        return dummyPointer;
    }

    for(i =0; i< NUMTCPWORKERS; i++)
    {
    	socketClientList[i] = 0;

		tcpworker_i[i] = create_thread(&tcpworker_p[i], (void *)&i);
		if(tcpworker_i[i] < 0)
		{
			printf("tcpHandler: Failed to create new Task\n");
		}
		else
		{
			pthread_detach(tcpworker_p[i]);
			usleep(800);
		}
    }

    while (run) {
        /* Wait for incoming request */
        clientfd = accept(lSocket, (struct sockaddr*)&client_addr, &addrlen);
        printf("accept client %d \r\n", clientfd);
        if( clientfd == -1 )
        goto dispose;

        bSockOpen = 0;
        for(i = 0; i<NUMTCPWORKERS;i++)
        {
        	if(socketClientList[i] == 0)
        	{
        		socketClientList[i] = clientfd;
        		bSockOpen = 1;
        		break;
        	}
        }

        if( bSockOpen == 0 )
        {
        	if(clientfd != 0)
				close(clientfd);
        }
    }

dispose:
    for(i = 0; i<NUMTCPWORKERS;i++)
    {
        if(socketClientList[i] != 0)
        {
            close(clientfd);
        }
    }

    printf("TCP Server service is closed.\r\n");
}

void tcp_close()
{
    printf("TCP Closing...\r\n");
    run = 0;
    close(lSocket);
    sleep_ms(1000);

    printf("TCP Closed.\r\n");
}