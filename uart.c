#include "uart.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "util.h"

#define LINUXPC 1
static int run = 1;
static int uart_fd = 0;
struct termios extra_termAttr;

int uart_init()
{
	#if LINUXPC == 1
    uart_fd = open("/dev/tty99", O_RDWR | O_NOCTTY | O_NDELAY);
	#else
	uart_fd = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY);
	#endif
	
	run = 1;

    if (uart_fd == -1)
	{
		perror("open_port: Unable to open /dev/ttyO4\n");
		return -1;
	}

    tcgetattr(uart_fd,&extra_termAttr);
	cfsetospeed(&extra_termAttr,B115200);
	extra_termAttr.c_cflag &= ~PARENB;
	extra_termAttr.c_cflag &= ~CSTOPB;
	extra_termAttr.c_cflag &= ~CSIZE;
	extra_termAttr.c_cflag |= CS8;
	extra_termAttr.c_cflag |= (CLOCAL | CREAD);
	extra_termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	extra_termAttr.c_iflag &= ~(IXON | IXOFF | IXANY|INLCR | ICRNL);	//kenny1126d	<~(IXON | IXOFF | IXANY);
	extra_termAttr.c_oflag &= ~OPOST;
	tcsetattr(uart_fd,TCSANOW,&extra_termAttr);

	printf("uart is initialized. \r\n");
}

void* uart_thd_run(void *arg)
{
    int len = 0;
    unsigned char buf[128];
    int i;
        
    while(run)
	{
        if(uart_fd != -1)
        {
            len = read(uart_fd , buf , sizeof(buf));
			if( len != -1 )
			{
            	printf("rx: %x len: %d\r\n", buf[0], len);
				if( buf[0] == 3 )
				break;
			}
        }
		sleep_ms(1);
	}
	close(uart_fd);
	printf("uart thread is closed. \r\n");
}

void uart_close()
{
    run = 0;
    printf("uart is closed. \r\n");
}