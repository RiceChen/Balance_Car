#include <drv_common.h>
#include "dev_motor.h"
#include "drv_pwm.h"
#include "drv_gpio.h"

struct rt_pwm_drv *motor_pwm_drv;

#define LEFT_MOTOE_CHANNEL          1
#define RIGHT_MOTOE_CHANNEL         4

#define LEFT_AIN1_PIN               28
#define LEFT_AIN2_PIN               29

#define RIGHT_BIN1_PIN              30
#define RIGHT_BIN2_PIN              31

#define MOTOR_PERIOD                7200

void rt_left_motor_set_pwm(rt_uint32_t pulse, rt_uint16_t direction)
{
    struct rt_pwm_config pwm_cfg = {0};

    if(direction == POSITIVE_DIRECTION)
    {
        rt_pin_write(LEFT_AIN1_PIN, PIN_HIGH);
        rt_pin_write(LEFT_AIN2_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_write(LEFT_AIN1_PIN, PIN_LOW);
        rt_pin_write(LEFT_AIN2_PIN, PIN_HIGH);
    }

    pwm_cfg.channel = LEFT_MOTOE_CHANNEL;
    pwm_cfg.period  = MOTOR_PERIOD;
    pwm_cfg.pulse   = pulse;

    rt_drv_pwm_set(&motor_pwm_drv->tim_handle, &pwm_cfg);
}

void rt_right_motor_set_pwm(rt_uint32_t pulse, rt_uint16_t direction)
{
    struct rt_pwm_config pwm_cfg = {0};

    if(direction == POSITIVE_DIRECTION)
    {
        rt_pin_write(RIGHT_BIN1_PIN, PIN_LOW);
        rt_pin_write(RIGHT_BIN2_PIN, PIN_HIGH);
    }
    else
    {
        rt_pin_write(RIGHT_BIN1_PIN, PIN_HIGH);
        rt_pin_write(RIGHT_BIN2_PIN, PIN_LOW);
    }

    pwm_cfg.channel = RIGHT_MOTOE_CHANNEL;
    pwm_cfg.period  = MOTOR_PERIOD;
    pwm_cfg.pulse   = pulse;

    rt_drv_pwm_set(&motor_pwm_drv->tim_handle, &pwm_cfg);
}

void rt_motor_enbale(rt_bool_t enable)
{
    struct rt_pwm_config pwm_cfg = {0};
    if(enable)
    {
        pwm_cfg.channel = LEFT_MOTOE_CHANNEL;
        rt_drv_pwm_enable(&motor_pwm_drv->tim_handle, &pwm_cfg, RT_TRUE);

        pwm_cfg.channel = RIGHT_MOTOE_CHANNEL;
        rt_drv_pwm_enable(&motor_pwm_drv->tim_handle, &pwm_cfg, RT_TRUE);
    }
    else
    {
        pwm_cfg.channel = LEFT_MOTOE_CHANNEL;
        rt_drv_pwm_enable(&motor_pwm_drv->tim_handle, &pwm_cfg, RT_FALSE);

        pwm_cfg.channel = RIGHT_MOTOE_CHANNEL;
        rt_drv_pwm_enable(&motor_pwm_drv->tim_handle, &pwm_cfg, RT_FALSE);
    }
}

int rt_motor_dev_init(void)
{
    motor_pwm_drv = rt_find_pwm("pwm1");
    if(motor_pwm_drv == RT_NULL)
    {
        rt_kprintf("find pwm1 fail\n");
        return RT_ERROR;
    }

    rt_motor_enbale(RT_TRUE);

    rt_pin_mode(LEFT_AIN1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LEFT_AIN2_PIN, PIN_MODE_OUTPUT);

    rt_pin_mode(RIGHT_BIN1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(RIGHT_BIN2_PIN, PIN_MODE_OUTPUT);

    rt_pin_write(LEFT_AIN1_PIN, PIN_LOW);
    rt_pin_write(LEFT_AIN2_PIN, PIN_LOW);
    rt_pin_write(RIGHT_BIN1_PIN, PIN_LOW);
    rt_pin_write(RIGHT_BIN2_PIN, PIN_LOW);

    rt_kprintf("motor device init finish\n");

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_motor_dev_init);
