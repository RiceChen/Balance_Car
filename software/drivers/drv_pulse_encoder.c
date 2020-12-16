#include <drv_common.h>
#include "drv_pulse_encoder.h"

#define AUTO_RELOAD_VALUE 0x7FFF

enum
{
#ifdef RT_USING_PULSE_ENCODER1
    PULSE_ENCODER1_INDEX,
#endif
#ifdef RT_USING_PULSE_ENCODER2
    PULSE_ENCODER2_INDEX,
#endif
#ifdef RT_USING_PULSE_ENCODER3
    PULSE_ENCODER3_INDEX,
#endif
#ifdef RT_USING_PULSE_ENCODER4
    PULSE_ENCODER4_INDEX,
#endif
};

static struct rt_pulse_encoder_drv pulse_encoder_obj[] =
{
#ifdef RT_USING_PULSE_ENCODER1
    PULSE_ENCODER1_CONFIG,
#endif
#ifdef RT_USING_PULSE_ENCODER2
    PULSE_ENCODER2_CONFIG,
#endif
#ifdef RT_USING_PULSE_ENCODER3
    PULSE_ENCODER3_CONFIG,
#endif
#ifdef RT_USING_PULSE_ENCODER4
    PULSE_ENCODER4_CONFIG,
#endif
};

rt_err_t rt_hw_pulse_encoder_init(struct rt_pulse_encoder_drv *pulse_encoder)
{
    TIM_Encoder_InitTypeDef sConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    pulse_encoder->tim_handler.Init.Prescaler = 0;
    pulse_encoder->tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    pulse_encoder->tim_handler.Init.Period = AUTO_RELOAD_VALUE;
    pulse_encoder->tim_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    pulse_encoder->tim_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 3;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 3;

    if (HAL_TIM_Encoder_Init(&pulse_encoder->tim_handler, &sConfig) != HAL_OK)
    {
        return -RT_ERROR;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&pulse_encoder->tim_handler, &sMasterConfig))
    {
        return -RT_ERROR;
    }
    else
    {
        HAL_NVIC_SetPriority(pulse_encoder->encoder_irqn, 3, 0);

        /* enable the TIMx global Interrupt */
        HAL_NVIC_EnableIRQ(pulse_encoder->encoder_irqn);

        /* clear update flag */
        __HAL_TIM_CLEAR_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_UPDATE);
        /* enable update request source */
        __HAL_TIM_URS_ENABLE(&pulse_encoder->tim_handler);
    }

    return RT_EOK;
}

struct rt_pulse_encoder_drv *rt_find_pulse_encoder(char *name)
{
    rt_size_t obj_num = sizeof(pulse_encoder_obj) / sizeof(struct rt_pulse_encoder_drv);

    for(int i = 0; i < obj_num; i++)
    {
        if(rt_strncmp(pulse_encoder_obj[i].name, name, RT_NAME_MAX) == 0)
        {
            return &pulse_encoder_obj[i];
        }
    }
    return RT_NULL;
}

rt_err_t rt_pulse_encoder_clear_count(struct rt_pulse_encoder_drv *pulse_encoder)
{
    pulse_encoder->over_under_flowcount = 0;
    __HAL_TIM_SET_COUNTER(&pulse_encoder->tim_handler, 0);
    return RT_EOK;
}

rt_int32_t rt_pulse_encoder_get_count(struct rt_pulse_encoder_drv *pulse_encoder)
{
    return (rt_int32_t)((rt_int16_t)__HAL_TIM_GET_COUNTER(&pulse_encoder->tim_handler) + pulse_encoder->over_under_flowcount * AUTO_RELOAD_VALUE);
}

void rt_pulse_encoder_enable(struct rt_pulse_encoder_drv *pulse_encoder, rt_bool_t enable)
{
    if(enable)
    {
        HAL_TIM_Encoder_Start(&pulse_encoder->tim_handler, TIM_CHANNEL_ALL);
        HAL_TIM_Encoder_Start_IT(&pulse_encoder->tim_handler, TIM_CHANNEL_ALL);
    }
    else
    {
        HAL_TIM_Encoder_Stop(&pulse_encoder->tim_handler, TIM_CHANNEL_ALL);
        HAL_TIM_Encoder_Stop_IT(&pulse_encoder->tim_handler, TIM_CHANNEL_ALL);
    }
}

void rt_pulse_encoder_update_isr(struct rt_pulse_encoder_drv *pulse_encoder)
{
    /* TIM Update event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_UPDATE);
        if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&pulse_encoder->tim_handler))
        {
            pulse_encoder->over_under_flowcount--;
        }
        else
        {
            pulse_encoder->over_under_flowcount++;
        }
    }
    /* Capture compare 1 event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_CC1) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_CC1);
    }
    /* Capture compare 2 event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_CC2) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_CC2);
    }
    /* Capture compare 3 event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_CC3) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_CC3);
    }
    /* Capture compare 4 event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_CC4) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_CC4);
    }
    /* TIM Break input event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_BREAK) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_BREAK);
    }
    /* TIM Trigger detection event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_TRIGGER) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_IT_TRIGGER);
    }
    /* TIM commutation event */
    if (__HAL_TIM_GET_FLAG(&pulse_encoder->tim_handler, TIM_FLAG_COM) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&pulse_encoder->tim_handler, TIM_FLAG_COM);
    }
}

void TIM2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_pulse_encoder_update_isr(&pulse_encoder_obj[PULSE_ENCODER2_INDEX]);
    /* leave interrupt */
    rt_interrupt_leave();
}

void TIM4_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    rt_pulse_encoder_update_isr(&pulse_encoder_obj[PULSE_ENCODER4_INDEX]);
    /* leave interrupt */
    rt_interrupt_leave();
}

int rt_pulse_encoder_init(void)
{
    int i;
    int result;

    result = RT_EOK;
    for (i = 0; i < sizeof(pulse_encoder_obj) / sizeof(pulse_encoder_obj[0]); i++)
    {
        rt_hw_pulse_encoder_init(&pulse_encoder_obj[i]);
    }

    return result;
}
INIT_PREV_EXPORT(rt_pulse_encoder_init);
