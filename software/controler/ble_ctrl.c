#include <drv_common.h>
#include "controler.h"
#include "ble_ctrl.h"
#include "dev_ble.h"
#include "sds.h"

extern struct angle_y_parm angle_parm;
extern struct turn_z_parm turn_parm;
extern struct car_speed_parm speed_parm;
extern struct balance_car_parm car_parm;

static char ble_stack[512];
static struct rt_thread ble_thread;

void ble_recv_data(void *recv_buff, rt_size_t size)
{
    struct ble_data_packet *ble_packet = (struct ble_data_packet *)recv_buff;

    if(ble_packet->type == BLE_DIRECTION_TYPE)
    {
        turn_parm.direction = ble_packet->channel;
    }
}

static void ble_thread_entry(void *parameter)
{
    struct sds_msg msg;
    while(1)
    {
        msg.ch1_data = angle_parm.out_angle * 1000;
        msg.ch2_data = angle_parm.angle * 1000;
        msg.ch3_data = angle_parm.angle_increment * 1000;
        sds_output_data(&msg);

        rt_thread_delay(1);
    }
}

int ble_ctrl_init(void)
{
    rt_thread_init(&ble_thread,
                   "ble",
                   ble_thread_entry,
                   RT_NULL,
                   &ble_stack[0],
                   sizeof(ble_stack),
                   6, 20);

//    if(rt_thread_startup(&ble_thread) != RT_EOK)
//    {
//        rt_kprintf("ble thread start fail\n");
//        return RT_ERROR;
//    }

    ble_recv_cb_register(ble_recv_data);

    return RT_EOK;
}
