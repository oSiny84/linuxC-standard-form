#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include "uart.h"
#include "util.h"
#include "tcp.h"

int software_run = 1;

void signal_handler(int no);
void module_init();
void module_thd_run();
void module_close();

int main()
{
    printf("Software Initialization....\r\n");

    if(signal(SIGINT, signal_handler) == SIG_ERR) 
    {
        printf("Fail to signal: %m \n");
        return -1;
    }

    if(signal(SIGTERM, signal_handler) == SIG_ERR)
    {
        printf("Fail to signal: %m \r\n");
        return -1;
    }

    module_init();
    module_thd_run();

    while(software_run)
    {
        sleep_ms(100);
    }

    printf("Software Exit.\n");
    return 0;
}

void signal_handler(int no)
{
    if(no == SIGINT)
    {
        printf(" : Ctrl+C \r\n");
    }
    else if(no == SIGTERM)
    {
        printf(" : kill command\r\n");
    }
    else
    {
        return;
    }
    module_close();
    software_run = 0;
}

void module_init()
{
    uart_init();
    tcp_init();
}

int thread_id[10];
void module_thd_run()
{
    // create thread
	pthread_t thread_info[10];
    int status[10];

    thread_id[0] = pthread_create(&thread_info[0] , NULL , uart_thd_run, NULL);
    thread_id[1] = pthread_create(&thread_info[1] , NULL , tcp_thd_run, NULL);
    
    pthread_join(thread_info[0] , (void *) &status[0]);
    printf("thread join - uart_thd_run() is closed. \r\n");
    pthread_join(thread_info[1] , (void *) &status[0]);
    printf("thread join - tcp_thd_run() is closed. \r\n");
}

void module_close()
{
    printf("Call %s at %s::%05d \r\n", __FUNCTION__, __FILE__, __LINE__);
    uart_close();
    tcp_close();
}