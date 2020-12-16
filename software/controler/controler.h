#ifndef CONTROLER_CONTROLER_H_
#define CONTROLER_CONTROLER_H_

struct angle_y_parm
{
    float angle;
    float angle_increment;
    float balance_gyor;
    float out_angle;
};

typedef enum {
    UP_DIRECTION_ = 0x01,
    DOWN_DIRECTION,
    LEFT_DIRECTION,
    RIGHT_DIRECTION,
}DIRECTION;

struct turn_z_parm
{
    rt_uint8_t direction;
    float turn_gyor;
};

struct car_speed_parm
{
    rt_int32_t get_left_speed;
    rt_int32_t get_right_speed;
    rt_int32_t set_left_speed;
    rt_int32_t set_right_speed;
    float integral_bias;
    float last_bias;
    rt_int32_t balance_pwm;
    rt_int32_t speed_pwm;
    rt_int32_t turn_pwm;
    rt_int32_t out_pwm;
};

#endif /* CONTROLER_CONTROLER_H_ */
