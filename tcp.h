
#ifndef TCP_H
#define TCP_H

void tcp_init();
void* tcp_thd_run(void *arg);
void tcp_close();

#endif