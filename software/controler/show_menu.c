#include "board.h"
#include "show_menu.h"
#include "dev_key.h"
#include "drv_flash.h"
#include "dev_oled.h"
#include "controler.h"

#define PARAM_SAVE_ADDR                 ((uint32_t)(ROM_START + 63 * 1024))
#define BALANCE_CAR_PARAM_OFFSET        0
#define KEY_TIMEOUT                     5

struct show_menu_param
{
    rt_uint8_t key_value;
    rt_uint8_t show_line;
    rt_uint8_t show_page;
    rt_uint8_t key_timeout;
    rt_uint8_t key_flag;
};

enum show_menu_line
{
    MENU_LINE_1 = 0x01,
    MENU_LINE_2,
    MENU_LINE_3,
};

enum show_menu_page
{
    MENU_PAGE_1,
    MENU_PAGE_2,
    MENU_PAGE_3,
    MENU_PAGE_4,
    MENU_PAGE_5,
};

static void rt_read_param_from_flash(rt_uint16_t offset, rt_uint8_t *param, rt_size_t len)
{
    if(rt_flash_read(PARAM_SAVE_ADDR + offset, param, len) != len)
    {
        rt_kprintf("read param form flash fail\n");
    }
}

static void rt_write_param_to_flash(rt_uint16_t offset, rt_uint8_t *param, rt_size_t len)
{
    rt_flash_erase(PARAM_SAVE_ADDR + offset, len);
    if(rt_flash_write(PARAM_SAVE_ADDR + offset, param, len) != len)
    {
        rt_kprintf("write param to flash fail\n");
    }
}

struct balance_car_parm car_parm;

struct balance_car_parm *get_balance_param(void)
{
    return &car_parm;
}

int show_menu_process(void)
{
    struct show_menu_param menu_param;
    struct balance_car_parm car_parm_temp;

    rt_read_param_from_flash(BALANCE_CAR_PARAM_OFFSET, (rt_uint8_t *)&car_parm_temp, BALANCE_CAR_PARAM_SIZE);

    rt_memset(&menu_param, 0, sizeof(struct show_menu_param));

    menu_param.show_line = MENU_LINE_1;

    while(menu_param.key_value != KEY_FUNC_OUT)
    {
        menu_param.key_value = KEY_NULL;
        menu_param.key_timeout = 0;
        while(menu_param.key_value == KEY_NULL)
        {
            menu_param.key_value = rt_read_key();
            rt_thread_delay(1);
        }

        while(rt_read_key() == KEY_ENTRY_DOWN)
        {
            if(menu_param.key_value == KEY_ENTRY_DOWN)
            {
                rt_thread_delay(50);
                menu_param.key_timeout++;
            }
            if(menu_param.key_timeout > KEY_TIMEOUT)
            {
                menu_param.key_value = KEY_FUNC_OUT;
                break;
            }
        }

        if(menu_param.key_value == KEY_ADD_DOWN || menu_param.key_value == KEY_SUB_DOWN)
        {
            if(menu_param.key_flag == 0)
            {
                rt_thread_delay(100);
                menu_param.key_flag = 1;
            }
        }

        if(menu_param.key_value == KEY_NULL)
        {
            menu_param.key_flag = 0;
        }

        rt_thread_delay(20);
        if(menu_param.key_value == KEY_SWITCH_DOWN)
        {
            switch(menu_param.show_line)
            {
                case MENU_LINE_1:
                {
                    rt_oled_show_str(0, 2, "   ");
                    rt_oled_show_str(0, 4, " * ");
                    rt_oled_show_str(0, 6, "   ");
                    menu_param.show_line = MENU_LINE_2;
                    break;
                }
                case MENU_LINE_2:
                {
                    rt_oled_show_str(0, 2, "   ");
                    rt_oled_show_str(0, 4, "   ");
                    rt_oled_show_str(0, 6, " * ");
                    menu_param.show_line = MENU_LINE_3;
                    break;
                }
                case MENU_LINE_3:
                {
                    rt_oled_show_str(0, 2, " * ");
                    rt_oled_show_str(0, 4, "   ");
                    rt_oled_show_str(0, 6, "   ");
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                default:
                    break;
            }
        }

        if(menu_param.key_value == KEY_ENTRY_DOWN)
        {
            switch(menu_param.show_page)
            {
                case MENU_PAGE_1:
                {
                    rt_oled_fill(OLED_FILL_BLACK);
                    rt_oled_show_str(20,0,"Balance PID");

                    rt_oled_show_digit(80, 2, car_parm_temp.blc_Kp);
                    rt_oled_show_str(0,2," * Bal_P");

                    rt_oled_show_digit(80, 4, car_parm_temp.blc_Ki);
                    rt_oled_show_str(0,4,"   Bal_I");

                    rt_oled_show_digit(80, 6, car_parm_temp.blc_Kd);
                    rt_oled_show_str(0,6,"   Bal_D");

                    menu_param.show_page = MENU_PAGE_2;
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                case MENU_PAGE_2:
                {
                    rt_oled_fill(OLED_FILL_BLACK);
                    rt_oled_show_str(24, 0, " Speed PID");

                    rt_oled_show_digit(80, 2, car_parm_temp.speed_Kp);
                    rt_oled_show_str(0, 2, " * Speed_P");

                    rt_oled_show_digit(80, 4, car_parm_temp.speed_Ki);
                    rt_oled_show_str(0, 4, "   Speed_I");

                    rt_oled_show_digit(80, 6, car_parm_temp.speed_Kd);
                    rt_oled_show_str(0, 6, "   Speed_D");

                    menu_param.show_page = MENU_PAGE_3;
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                case MENU_PAGE_3:
                {
                    rt_oled_fill(OLED_FILL_BLACK);
                    rt_oled_show_str(16, 0, "     speed");

                    rt_oled_show_digit(80, 2, car_parm_temp.speed);
                    rt_oled_show_str(0, 2, " * speed");

                    rt_oled_show_digit(80, 4, car_parm_temp.g_speed);
                    rt_oled_show_str(0, 4, "   Speed_H");

                    rt_oled_show_digit(80, 6, car_parm_temp.t_speed);
                    rt_oled_show_str(0, 6, "   Speed_L");

                    menu_param.show_page = MENU_PAGE_4;
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                case MENU_PAGE_4:
                {
                    rt_oled_fill(OLED_FILL_BLACK);
                    rt_oled_show_str(16, 0, "  Turn PID");

                    rt_oled_show_digit(80, 2, car_parm_temp.turn_Kp);
                    rt_oled_show_str(0, 2, " * Turn_P");

                    rt_oled_show_digit(80, 4, car_parm_temp.turn_Ki);
                    rt_oled_show_str(0, 4, "   Turn_I");

                    rt_oled_show_digit(80, 6, car_parm_temp.turn_Kd);
                    rt_oled_show_str(0, 6, "   Turn_D");

                    menu_param.show_page = MENU_PAGE_5;
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                case MENU_PAGE_5:
                {
                    rt_oled_fill(OLED_FILL_BLACK);
                    rt_oled_show_str(16, 0, "    Angle");

                    rt_oled_show_digit(80, 2, car_parm_temp.blc_angle);
                    rt_oled_show_str(0, 2, " * B_angle");

                    menu_param.show_page = MENU_PAGE_1;
                    menu_param.show_line = MENU_LINE_1;
                    break;
                }
                default:
                    break;
            }
        }
        if(menu_param.key_value == KEY_ADD_DOWN)
        {
            switch(menu_param.show_page)
            {
                case MENU_PAGE_1:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.blc_angle += 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.blc_angle);
                            rt_oled_show_str(0, 2, " * B_angle");
                            break;

                    }
                    break;
                }
                case MENU_PAGE_2:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                        {
                            car_parm_temp.blc_Kp += 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.blc_Kp);
                            rt_oled_show_str(0, 2, " * Bal_P");
                            break;
                        }
                        case MENU_LINE_2:
                        {
                            car_parm_temp.blc_Ki += 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.blc_Ki);
                            rt_oled_show_str(0, 4, " * Bal_I");
                            break;
                        }
                        case MENU_LINE_3:
                        {
                            car_parm_temp.blc_Kd += 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.blc_Kd);
                            rt_oled_show_str(0, 6, " * Bal_D");
                            break;
                        }
                    }
                    break;
                }
                case MENU_PAGE_3:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                        {
                            car_parm_temp.speed_Kp += 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.speed_Kp);
                            rt_oled_show_str(0, 2, " * Speed_P");
                            break;
                        }
                        case MENU_LINE_2:
                        {
                            car_parm_temp.speed_Ki += 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.speed_Ki);
                            rt_oled_show_str(0, 4, " * Speed_I");
                            break;
                        }
                        case MENU_LINE_3:
                        {
                            car_parm_temp.speed_Kd += 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.speed_Kd);
                            rt_oled_show_str(0, 6, " * Speed_D");
                            break;
                        }
                    }
                    break;
                }
                case MENU_PAGE_4:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.speed += 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.speed);
                            rt_oled_show_str(0, 2, " * speed");
                            break;
                        case MENU_LINE_2:
                            car_parm_temp.g_speed += 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.g_speed);
                            rt_oled_show_str(0, 4, " * G-speed");
                            break;
                        case MENU_LINE_3:
                            car_parm_temp.t_speed += 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.t_speed);
                            rt_oled_show_str(0, 6, " * T-speed");
                            break;

                    }
                    break;
                }
                case MENU_PAGE_5:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.turn_Kp += 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.turn_Kp);
                            rt_oled_show_str(0, 2, " * Turn_P");
                            break;
                        case MENU_LINE_2:
                            car_parm_temp.turn_Ki += 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.turn_Ki);
                            rt_oled_show_str(0, 4, " * Turn_I");
                            break;
                        case MENU_LINE_3:
                            car_parm_temp.turn_Kd += 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.turn_Kd);
                            rt_oled_show_str(0, 6, " * Turn_D");
                            break;

                    }
                    break;
                }
            }
        }

        if(menu_param.key_value == KEY_SUB_DOWN)
        {
            switch(menu_param.show_page)
            {
                case MENU_PAGE_1:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.blc_angle -= 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.blc_angle);
                            rt_oled_show_str(0, 2, " * B_angle");
                            break;

                    }
                    break;
                }
                case MENU_PAGE_2:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                        {
                            car_parm_temp.blc_Kp -= 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.blc_Kp);
                            rt_oled_show_str(0, 2, " * Bal_P");
                            break;
                        }
                        case MENU_LINE_2:
                        {
                            car_parm_temp.blc_Ki -= 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.blc_Ki);
                            rt_oled_show_str(0, 4, " * Bal_I");
                            break;
                        }
                        case MENU_LINE_3:
                        {
                            car_parm_temp.blc_Kd -= 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.blc_Kd);
                            rt_oled_show_str(0, 6, " * Bal_D");
                            break;
                        }
                    }
                    break;
                }
                case MENU_PAGE_3:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                        {
                            car_parm_temp.speed_Kp -= 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.speed_Kp);
                            rt_oled_show_str(0, 2, " * Speed_P");
                            break;
                        }
                        case MENU_LINE_2:
                        {
                            car_parm_temp.speed_Ki -= 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.speed_Ki);
                            rt_oled_show_str(0, 4, " * Speed_I");
                            break;
                        }
                        case MENU_LINE_3:
                        {
                            car_parm_temp.speed_Kd -= 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.speed_Kd);
                            rt_oled_show_str(0, 6, " * Speed_D");
                            break;
                        }
                    }
                    break;
                }
                case MENU_PAGE_4:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.speed -= 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.speed);
                            rt_oled_show_str(0, 2, " * speed");
                            break;
                        case MENU_LINE_2:
                            car_parm_temp.g_speed -= 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.g_speed);
                            rt_oled_show_str(0, 4, " * G-speed");
                            break;
                        case MENU_LINE_3:
                            car_parm_temp.t_speed -= 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.t_speed);
                            rt_oled_show_str(0, 6, " * T-speed");
                            break;

                    }
                    break;
                }
                case MENU_PAGE_5:
                {
                    switch(menu_param.show_line)
                    {
                        case MENU_LINE_1:
                            car_parm_temp.turn_Kp -= 1;
                            rt_oled_show_digit(80, 2, car_parm_temp.turn_Kp);
                            rt_oled_show_str(0, 2, " * Turn_P");
                            break;
                        case MENU_LINE_2:
                            car_parm_temp.turn_Ki -= 1;
                            rt_oled_show_digit(80, 4, car_parm_temp.turn_Ki);
                            rt_oled_show_str(0, 4, " * Turn_I");
                            break;
                        case MENU_LINE_3:
                            car_parm_temp.turn_Kd -= 1;
                            rt_oled_show_digit(80, 6, car_parm_temp.turn_Kd);
                            rt_oled_show_str(0, 6, " * Turn_D");
                            break;

                    }
                    break;
                }
            }
        }
    }

    car_parm.blc_Kp = car_parm_temp.blc_Kp;
    car_parm.blc_Ki = car_parm_temp.blc_Ki;
    car_parm.blc_Kd = car_parm_temp.blc_Kd / 10;

    car_parm.speed_Kp = car_parm_temp.speed_Kp;
    car_parm.speed_Ki = car_parm_temp.speed_Ki / 200;
    car_parm.speed_Kd = car_parm_temp.speed_Kd;

    car_parm.turn_Kp = car_parm_temp.turn_Kp / 10;
    car_parm.turn_Ki = car_parm_temp.turn_Ki;
    car_parm.turn_Kd = car_parm_temp.turn_Kd;

    car_parm.speed = car_parm_temp.speed;
    car_parm.g_speed = car_parm_temp.g_speed;
    car_parm.t_speed = car_parm_temp.t_speed;

    car_parm.blc_angle = car_parm_temp.blc_angle;

    rt_kprintf("balance_pid: P- %d  I- %d  D- %d\n", car_parm.blc_Kp,
                                                     car_parm.blc_Ki,
                                                     car_parm.blc_Kd);

    rt_kprintf("speed_pid: P- %d  I- %d  D- %d\n", car_parm.speed_Kp,
                                                   car_parm.speed_Ki,
                                                   car_parm.speed_Kd);

    rt_kprintf("turn_pid: P- %d  I- %d  D- %d\n", car_parm.turn_Kp,
                                                  car_parm.turn_Ki,
                                                  car_parm.turn_Kd);

    rt_kprintf("speed: tg- %d  H- %d  L- %d\n", car_parm.speed,
                                                car_parm.g_speed,
                                                car_parm.t_speed);

    rt_kprintf("angle: angle- %d\n", car_parm.blc_angle);

    rt_write_param_to_flash(BALANCE_CAR_PARAM_OFFSET, (rt_uint8_t *)&car_parm_temp, BALANCE_CAR_PARAM_SIZE);

    rt_oled_fill(OLED_FILL_BLACK);

    return RT_EOK;
}


extern struct angle_y_parm angle_parm;
extern struct turn_z_parm turn_parm;
extern struct car_speed_parm speed_parm;
extern struct balance_car_parm car_parm;

static char show_stack[512];
static struct rt_thread show_thread;

static void show_thread_entry(void *parameter)
{
    while(1)
    {
        rt_oled_show_digit(80, 0, angle_parm.out_angle);
        rt_oled_show_digit(80, 2, speed_parm.out_pwm);
        rt_oled_show_digit(80, 4, turn_parm.turn_gyor);
        rt_oled_show_digit(80, 6, speed_parm.get_right_speed);

        rt_thread_delay(1);
    }
}

int show_ctrl_init(void)
{
    rt_thread_init(&show_thread,
                   "show",
                   show_thread_entry,
                   RT_NULL,
                   &show_stack[0],
                   sizeof(show_stack),
                   6, 20);

    if(rt_thread_startup(&show_thread) != RT_EOK)
    {
        rt_kprintf("show thread start fail\n");
        return RT_ERROR;
    }

    return RT_EOK;
}
