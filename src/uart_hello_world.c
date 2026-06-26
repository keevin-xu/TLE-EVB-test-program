#include "tle_device.h"
#include "uart_hello_world.h"

#define UART_HELLO_USE_UART1 1
#define UART_HELLO_USE_UART2 0

volatile unsigned long g_uart_hello_loop_count = 0u;
volatile unsigned long g_uart_hello_chars_sent = 0u;
volatile unsigned long g_uart_hello_tx_timeouts = 0u;

static void uart_wait_for_tx_done(void)
{
    volatile uint32 timeout;

    for (timeout = 0u; timeout < 100000u; timeout++)
    {
#if UART_HELLO_USE_UART1
        if (UART1_isByteTransmitted() != false)
        {
            return;
        }
#endif

#if UART_HELLO_USE_UART2
        if (UART2_isByteTransmitted() != false)
        {
            return;
        }
#endif
    }

    g_uart_hello_tx_timeouts++;
}

static void uart_write_char(char c)
{
#if UART_HELLO_USE_UART1
    UART1_Send_Byte((uint8)c);
#endif

#if UART_HELLO_USE_UART2
    UART2_Send_Byte((uint8)c);
#endif

    g_uart_hello_chars_sent++;
    uart_wait_for_tx_done();
}

static void uart_write_string(const char *text)
{
    while (*text != '\0')
    {
        uart_write_char(*text);
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
#if UART_HELLO_USE_UART1
        uart_write_string("hello from TLE9855 UART1\r\n");
#endif

#if UART_HELLO_USE_UART2
        uart_write_string("hello from TLE9855 UART2\r\n");
#endif
        delay_loop();
    }
}
