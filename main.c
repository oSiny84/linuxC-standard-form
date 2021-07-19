#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include "uart.h"
#include "util.h"
#include "tcp.h"
#include "util.h"

int software_run = 1;

void signal_handler(int no);
void module_init();
void module_thd_run();
void module_thd_join();
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

    //vs code에서는 ctrl+c 를 누르면 signal handler로 가지 않고 바로 종료되기 때문에 디버깅 모드에서 d_call_sighan 을 1로 변경하여 종료절차를 테스트 합니다.
    int d_call_sighan = 0;
    while(software_run)
    {
        if( d_call_sighan == 1)
            signal_handler(SIGINT);

        sleep_ms(100);
    }

    module_thd_join();
    printf("Software Exit.\n");
    return 0;
}

void signal_handler(int sig)
{
    static int bClosing = 0;

    if(sig == SIGINT)
    {
        printf(" : Ctrl+C \r\n");
    }
    else if(sig == SIGTERM)
    {
        printf(" : kill command\r\n");
    }
    else if(sig == SIGQUIT)
    {
        printf(" : quit command\r\n");
    }
    else
    {
        return;
    }

    if(  bClosing == 1 )
        return;

    bClosing = 1;

    module_close();
    software_run = 0;
}

void module_init()
{
    uart_init();
    tcp_init();
}

int thread_id[10];
pthread_t thread_info[10];
void module_thd_run()
{
    // create thread
    thread_id[0] = pthread_create(&thread_info[0] , NULL , uart_thd_run, NULL);
    thread_id[1] = pthread_create(&thread_info[1] , NULL , tcp_thd_run, NULL);
}

void module_thd_join()
{
    int status[10];
    pthread_join(thread_info[0] , (void *) &status[0]);
    printf("thread join - uart_thd_run() is closed. \r\n");
    pthread_join(thread_info[1] , (void *) &status[1]);
    printf("thread join - tcp_thd_run() is closed. \r\n");
}

void module_close()
{
    printfIt();
    printf("오랜 시간 동안 'Software Exit.' 가 뜨지 않으면 kill -9 pid 명령으로 강제 종료 하세요.\r\n");
    uart_close();
    tcp_close();
}