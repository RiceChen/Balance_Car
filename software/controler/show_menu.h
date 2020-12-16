#ifndef CONTROLER_SHOW_MENU_H_
#define CONTROLER_SHOW_MENU_H_

struct balance_car_parm
{
    rt_int16_t blc_Kp;
    rt_int16_t blc_Ki;
    rt_int16_t blc_Kd;

    rt_int16_t speed_Kp;
    rt_int16_t speed_Ki;
    rt_int16_t speed_Kd;

    rt_int16_t turn_Kp;
    rt_int16_t turn_Ki;
    rt_int16_t turn_Kd;

    rt_int16_t speed;
    rt_int16_t g_speed;
    rt_int16_t t_speed;

    rt_int16_t blc_angle;
    rt_int16_t rev;
};
#define BALANCE_CAR_PARAM_SIZE      sizeof(struct balance_car_parm)

int show_ctrl_init(void);
int show_menu_process(void);
struct balance_car_parm *get_balance_param(void);

#endif /* CONTROLER_SHOW_MENU_H_ */
