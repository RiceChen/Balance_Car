#ifndef DEVICES_DEV_MOTOR_H_
#define DEVICES_DEV_MOTOR_H_

enum
{
    POSITIVE_DIRECTION,
    NEGATIVE_DIRECTION,
} MOTOR_DIRECTION;

void rt_motor_enbale(rt_bool_t enable);
void rt_left_motor_set_pwm(rt_uint32_t pulse, rt_uint16_t direction);
void rt_right_motor_set_pwm(rt_uint32_t pulse, rt_uint16_t direction);

#endif /* DEVICES_DEV_MOTOR_H_ */
