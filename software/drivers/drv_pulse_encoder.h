#ifndef DRIVERS_DRV_PULSE_ENCODER_H_
#define DRIVERS_DRV_PULSE_ENCODER_H_

#define RT_USING_PULSE_ENCODER2
#define RT_USING_PULSE_ENCODER4

struct rt_pulse_encoder_drv
{
    TIM_HandleTypeDef tim_handler;
    IRQn_Type encoder_irqn;
    rt_int32_t over_under_flowcount;
    char *name;
};

#ifdef RT_USING_PULSE_ENCODER2
#ifndef PULSE_ENCODER2_CONFIG
#define PULSE_ENCODER2_CONFIG                     \
    {                                             \
       .tim_handler.Instance     = TIM2,          \
       .encoder_irqn             = TIM2_IRQn,  \
       .name                     = "pulse1"       \
    }
#endif /* PULSE_ENCODER2_CONFIG */
#endif /* RT_USING_PULSE_ENCODER2 */

#ifdef RT_USING_PULSE_ENCODER4
#ifndef PULSE_ENCODER4_CONFIG
#define PULSE_ENCODER4_CONFIG                  \
    {                                          \
       .tim_handler.Instance     = TIM4,       \
       .encoder_irqn             = TIM4_IRQn,  \
       .name                     = "pulse2"    \
    }
#endif /* PULSE_ENCODER4_CONFIG */
#endif /* RT_USING_PULSE_ENCODER4 */

struct rt_pulse_encoder_drv *rt_find_pulse_encoder(char *name);
rt_int32_t rt_pulse_encoder_get_count(struct rt_pulse_encoder_drv *pulse_encoder);
rt_err_t rt_pulse_encoder_clear_count(struct rt_pulse_encoder_drv *pulse_encoder);
void rt_pulse_encoder_enable(struct rt_pulse_encoder_drv *pulse_encoder, rt_bool_t enable);

#endif /* DRIVERS_DRV_PULSE_ENCODER_H_ */
