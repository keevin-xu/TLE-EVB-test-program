#include "tle_device.h"
#include "uart_hello_world.h"

int main(void)
{
    TLE_Init();

    run_uart_hello_world_demo();

    while (1)
    {
    }
}
