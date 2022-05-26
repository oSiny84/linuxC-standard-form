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

int read_with_timeout(int fd, char *buf, int buf_size, int timeout_ms)
{
    int rx_len = 0;
    struct    timeval  timeout;
    fd_set  	readFds;

    // recive time out config
    // Set 1ms timeout counter
    timeout.tv_sec  = 0;
    timeout.tv_usec = timeout_ms*1000;   

    FD_ZERO(&readFds);
    FD_SET(fd, &readFds);
    select(fd+1, &readFds, NULL, NULL, &timeout);

    if(FD_ISSET(fd, &readFds))
    {
        rx_len = read(fd, buf, buf_size);		
    }

	return rx_len;
}

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

	printf("UART is initialized. \r\n");
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
            //len = read(uart_fd , buf , sizeof(buf));
			len = read_with_timeout(uart_fd,buf,sizeof(buf),1000);
			if( len != -1 )
			{
            	printf("rx: %x len: %d\r\n", buf[0], len);
				if( buf[0] == 3 )
				break;
			}
        }
		sleep_ms(1);
	}
	
	printf("UART thread is closed. \r\n");
}

void uart_close()
{
    run = 0;
	close(uart_fd);
    printf("UART is closed. \r\n");
}