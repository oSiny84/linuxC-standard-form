#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int software_run = 1;

void signal_handler(int no);
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

    while(software_run)
    {

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
    else if(no==SIGTERM)
    {
        printf(" : kill command\r\n");
    }
    else
    {
        return;
    }

    software_run = 0;
}