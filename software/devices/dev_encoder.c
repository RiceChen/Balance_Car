#include <drv_common.h>
#include "dev_encoder.h"
#include "drv_pulse_encoder.h"

struct rt_pulse_encoder_drv *left_encoder;
struct rt_pulse_encoder_drv *right_encoder;

rt_int32_t rt_left_encoder_get_speed(void)
{
    rt_int32_t speed = 0;
    speed = rt_pulse_encoder_get_count(left_encoder);
    rt_pulse_encoder_clear_count(left_encoder);

    return speed;
}

rt_int32_t rt_right_encoder_get_speed(void)
{
    rt_int32_t speed = 0;
    speed = rt_pulse_encoder_get_count(right_encoder);
    rt_pulse_encoder_clear_count(right_encoder);

    return speed;
}

int rt_encoder_dev_init(void)
{
    left_encoder = rt_find_pulse_encoder("pulse1");
    if(left_encoder == RT_NULL)
    {
        rt_kprintf("find pulse encoder 1 fail\n");
        return RT_ERROR;
    }

    right_encoder = rt_find_pulse_encoder("pulse2");
    if(right_encoder == RT_NULL)
    {
        rt_kprintf("find pulse encoder 2 fail\n");
        return RT_ERROR;
    }

    rt_pulse_encoder_enable(left_encoder, RT_TRUE);
    rt_pulse_encoder_enable(right_encoder, RT_TRUE);

    rt_pulse_encoder_clear_count(left_encoder);
    rt_pulse_encoder_clear_count(right_encoder);

    rt_kprintf("encoder device init finish\n");

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_encoder_dev_init);
