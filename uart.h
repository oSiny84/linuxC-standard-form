#ifndef UART_H
#define UART_H

int uart_init();
void* uart_thd_run(void *arg);
void uart_close();

#endif