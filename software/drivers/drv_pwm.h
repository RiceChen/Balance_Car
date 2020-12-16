#ifndef DRIVERS_DRV_PWM_H_
#define DRIVERS_DRV_PWM_H_

#define RT_USING_PWM1
#define RT_USING_PWM1_CH1
#define RT_USING_PWM1_CH4

struct rt_pwm_config
{
    rt_uint32_t channel; /* 0-n */
    rt_uint32_t period;  /* unit:ns 1ns~4.29s:1Ghz~0.23hz */
    rt_uint32_t pulse;   /* unit:ns (pulse<=period) */
};

struct rt_pwm_drv
{
    TIM_HandleTypeDef tim_handle;
    rt_uint8_t channel;
    char *name;
};

#ifdef RT_USING_PWM1
#define PWM1_CONFIG                                               \
    {                                                             \
        .name = "pwm1",                                           \
        .tim_handle.Instance = TIM1,                              \
        .channel = 0,                                             \
    }
#endif

#ifdef RT_USING_PWM2
#define PWM2_CONFIG                                               \
    {                                                             \
        .name = "pwm2",                                           \
        .tim_handle.Instance = TIM2,                              \
        .channel = 0,                                             \
    }
#endif

struct rt_pwm_drv *rt_find_pwm(char *name);
rt_err_t rt_drv_pwm_enable(TIM_HandleTypeDef *htim, struct rt_pwm_config *config, rt_bool_t enable);
rt_err_t rt_drv_pwm_get(TIM_HandleTypeDef *htim, struct rt_pwm_config *config);
rt_err_t rt_drv_pwm_set(TIM_HandleTypeDef *htim, struct rt_pwm_config *config);


#endif /* DRIVERS_DRV_PWM_H_ */
