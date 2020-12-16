#include <drv_common.h>
#include "dev_ble.h"
#include "drv_uart.h"

struct rt_uart_drv *ble_uart_drv;
ble_recv_cb_func recv_cb_func;

rt_uint8_t ble_recv_buff[20] = {0};
rt_uint16_t ble_data_offset = 0;

void rt_ble_uart_recv(rt_uint8_t *ch)
{
    ble_recv_buff[ble_data_offset++] = ch[0];

    if(ble_recv_buff[0] != BLE_HEAD && ble_data_offset == 2)
    {
        ble_data_offset = 0;
    }

    if((ble_recv_buff[ble_data_offset - 1] == BLE_TAIL && ble_data_offset > 0)
       || ble_data_offset >= sizeof(ble_recv_buff))
    {
        ble_recv_buff[ble_data_offset - 1] = '\0';
        ble_data_offset--;
        if(recv_cb_func)
        {
            recv_cb_func(ble_recv_buff, ble_data_offset);
        }
        ble_data_offset = 0;
    }
}

void ble_recv_cb_register(ble_recv_cb_func recv_cb)
{
    recv_cb_func = recv_cb;
}

int rt_ble_uart_send(rt_uint8_t *data, rt_size_t len)
{
    return rt_uart_send(ble_uart_drv, data, len);
}

int rt_ble_dev_init(void)
{
    ble_uart_drv = rt_find_uart("uart3");
    if(ble_uart_drv == RT_NULL)
    {
        rt_kprintf("find uart3 fail\n");
        return RT_ERROR;
    }

    rt_uart_irq_register_cb(ble_uart_drv, rt_ble_uart_recv);
    rt_uart_irq_enable(ble_uart_drv, RT_TRUE);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_ble_dev_init);


