#include "tle_device.h"
#include "uart_hello_world.h"

volatile unsigned long g_uart_hello_loop_count = 0u;
volatile unsigned long g_uart_hello_chars_sent = 0u;
volatile unsigned long g_uart_hello_tx_timeouts = 0u;

static void uart1_wait_for_tx_done(void)
{
    volatile uint32 timeout;

    for (timeout = 0u; timeout < 100000u; timeout++)
    {
        if (UART1_isByteTransmitted() != false)
        {
            return;
        }
    }

    g_uart_hello_tx_timeouts++;
}

static void uart1_write_char(char c)
{
    UART1_Send_Byte((uint8)c);
    g_uart_hello_chars_sent++;
    uart1_wait_for_tx_done();
}

static void uart1_write_string(const char *text)
{
    while (*text != '\0')
    {
        uart1_write_char(*text);
        text++;
    }
}

static void delay_loop(void)
{
    volatile uint32 count;

    for (count = 0u; count < 800000u; count++)
    {
    }
}

void run_uart_hello_world_demo(void)
{
    while (1)
    {
        g_uart_hello_loop_count++;
        uart1_write_string("hello from TLE9855 UART1\r\n");
        delay_loop();
    }
}
