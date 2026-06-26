#ifndef UART_HELLO_WORLD_H
#define UART_HELLO_WORLD_H

extern volatile unsigned long g_uart_hello_loop_count;
extern volatile unsigned long g_uart_hello_chars_sent;
extern volatile unsigned long g_uart_hello_tx_timeouts;

void run_uart_hello_world_demo(void);

#endif
