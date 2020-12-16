#include <drv_common.h>
#include <math.h>
#include "controler.h"
#include "show_menu.h"
#include "ble_ctrl.h"

#include "dev_mpu6050.h"
#include "dev_motor.h"
#include "dev_encoder.h"

#define K1                  0.02
#define PI                  3.14159265

#define BALANCE_MID_VALUE   0

struct mpu6050_3axes accel;
struct mpu6050_3axes gyro;

struct angle_y_parm angle_parm;
struct turn_z_parm turn_parm;
struct car_speed_parm speed_parm;
extern struct balance_car_parm car_parm;

static char ctrl_stack[512];
static struct rt_thread ctrl_thread;

void ctrl_get_angle(void)
{
    rt_mpu6050_get_accel_raw(&accel);
    if(accel.x > 32768) accel.x -= 65536;
    if(accel.y > 32768) accel.y -= 65536;
    if(accel.z > 32768) accel.z -= 65536;

    rt_mpu6050_get_gyro_raw(&gyro);
    if(gyro.x > 32768) gyro.x -= 65536;
    if(gyro.y > 32768) gyro.y -= 65536;
    if(gyro.z > 32768) gyro.z -= 65536;

    turn_parm.turn_gyor = gyro.z / 16.384;
    angle_parm.balance_gyor = gyro.y / 16.384;
    angle_parm.angle = atan2(accel.x, accel.z) * 180 / PI;
    angle_parm.angle_increment = angle_parm.out_angle - (gyro.y / 16.384) * 0.005;
    angle_parm.out_angle = K1 * angle_parm.angle + (1 - K1)*(angle_parm.angle_increment);
}

int ctrl_balance_pid(void)
{
    float bias_val = 0.0;

    bias_val = (car_parm.blc_angle - 0.5) - angle_parm.out_angle;

    speed_parm.balance_pwm = car_parm.blc_Kp * bias_val + car_parm.blc_Kd * angle_parm.balance_gyor;

    return speed_parm.balance_pwm;
}

void ctrl_get_speed(void)
{
    rt_int32_t left_speed, right_speed;
    left_speed = rt_left_encoder_get_speed();
    right_speed = -rt_right_encoder_get_speed();

    if(left_speed <=50 && left_speed >= -50)
    {
        speed_parm.get_left_speed = left_speed;
    }

    if(right_speed <= 50 && right_speed >= -50)
    {
        speed_parm.get_right_speed = right_speed;
    }
}

void ctrl_speed_pid(void)
{
    float current_bias = 0;

    current_bias = (speed_parm.get_left_speed + speed_parm.get_right_speed) - car_parm.speed;

    current_bias = speed_parm.last_bias * 0.3 + current_bias * 0.7;


    speed_parm.integral_bias += current_bias;

    speed_parm.speed_pwm = (int)(car_parm.speed_Kp * current_bias +
                         car_parm.speed_Ki * speed_parm.integral_bias);

    speed_parm.last_bias = current_bias;
}

void ctrl_turn_pid(void)
{
    float current_bias = 0;

    current_bias = 0 - turn_parm.turn_gyor;
    speed_parm.turn_pwm = car_parm.turn_Kp * current_bias;

    if(turn_parm.direction == LEFT_DIRECTION)
    {
        speed_parm.turn_pwm += 50;
    }
    else if(turn_parm.direction == RIGHT_DIRECTION)
    {
        speed_parm.turn_pwm -= 50;
    }
    else
    {
        speed_parm.turn_pwm += 0;
    }
}

void ctrl_set_speed()
{
    int direction = POSITIVE_DIRECTION;

    speed_parm.out_pwm = speed_parm.balance_pwm + speed_parm.speed_pwm;

    if(speed_parm.out_pwm < 0)
    {
        direction = NEGATIVE_DIRECTION;
        speed_parm.out_pwm = (0 - speed_parm.out_pwm);
    }

    speed_parm.out_pwm += 450;

    if(speed_parm.out_pwm > 4000)
    {
        speed_parm.out_pwm = 4000;
    }

    rt_left_motor_set_pwm(speed_parm.out_pwm + speed_parm.turn_pwm, direction);
    rt_right_motor_set_pwm(speed_parm.out_pwm + speed_parm.turn_pwm, direction);
}

static void ctrl_thread_entry(void *parameter)
{
    for(;;)
    {
        ctrl_get_angle();
        ctrl_balance_pid();
        ctrl_get_speed();
        ctrl_speed_pid();
        ctrl_turn_pid();
        ctrl_set_speed();

        rt_thread_delay(5);
    }
}

int controler_init()
{
//    show_menu_process();
//
//    rt_thread_init(&ctrl_thread,
//                   "ctrl",
//                   ctrl_thread_entry,
//                   RT_NULL,
//                   &ctrl_stack[0],
//                   sizeof(ctrl_stack),
//                   5, 20);
//    if(rt_thread_startup(&ctrl_thread) != RT_EOK)
//    {
//        rt_kprintf("control thread start fail\n");
//        return RT_ERROR;
//    }

    ble_ctrl_init();
//    show_ctrl_init();

    rt_kprintf("control start finish\n");

    return RT_EOK;
}
INIT_APP_EXPORT(controler_init);
