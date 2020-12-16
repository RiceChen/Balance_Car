#include <drv_common.h>
#include "drv_pwm.h"

#define MAX_PERIOD      65535
#define MIN_PERIOD      3
#define MIN_PULSE       2

extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

enum
{
#ifdef RT_USING_PWM1
    PWM1_INDEX,
#endif
#ifdef RT_USING_PWM2
    PWM2_INDEX,
#endif
#ifdef RT_USING_PWM3
    PWM3_INDEX,
#endif
#ifdef RT_USING_PWM4
    PWM4_INDEX,
#endif
#ifdef RT_USING_PWM5
    PWM5_INDEX,
#endif
#ifdef RT_USING_PWM6
    PWM6_INDEX,
#endif
#ifdef RT_USING_PWM7
    PWM7_INDEX,
#endif
#ifdef RT_USING_PWM8
    PWM8_INDEX,
#endif
#ifdef RT_USING_PWM9
    PWM9_INDEX,
#endif
#ifdef RT_USING_PWM10
    PWM10_INDEX,
#endif
#ifdef RT_USING_PWM11
    PWM11_INDEX,
#endif
#ifdef RT_USING_PWM12
    PWM12_INDEX,
#endif
#ifdef RT_USING_PWM13
    PWM13_INDEX,
#endif
#ifdef RT_USING_PWM14
    PWM14_INDEX,
#endif
#ifdef RT_USING_PWM15
    PWM15_INDEX,
#endif
#ifdef RT_USING_PWM16
    PWM16_INDEX,
#endif
#ifdef RT_USING_PWM17
    PWM17_INDEX,
#endif
};

struct rt_pwm_drv pwm_obj[] =
{
#ifdef RT_USING_PWM1
    PWM1_CONFIG,
#endif
#ifdef RT_USING_PWM2
    PWM2_CONFIG,
#endif
#ifdef RT_USING_PWM3
    PWM3_CONFIG,
#endif

#ifdef RT_USING_PWM4
    PWM4_CONFIG,
#endif
};

struct rt_pwm_drv *rt_find_pwm(char *name)
{
    rt_size_t obj_num = sizeof(pwm_obj) / sizeof(struct rt_pwm_drv);

    for(int i = 0; i < obj_num; i++)
    {
        if(rt_strncmp(pwm_obj[i].name, name, RT_NAME_MAX) == 0)
        {
            return &pwm_obj[i];
        }
    }
    return RT_NULL;
}

rt_err_t rt_drv_pwm_enable(TIM_HandleTypeDef *htim, struct rt_pwm_config *config, rt_bool_t enable)
{
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (config->channel - 1);

    if (!enable)
    {
        HAL_TIM_PWM_Stop(htim, channel);
    }
    else
    {
        HAL_TIM_PWM_Start(htim, channel);
    }
    return RT_EOK;
}

rt_err_t rt_drv_pwm_get(TIM_HandleTypeDef *htim, struct rt_pwm_config *config)
{
    rt_uint32_t channel = 0x04 * (config->channel - 1);
    rt_uint64_t tim_clock;

    if(0)
    {
        tim_clock = HAL_RCC_GetPCLK2Freq() * 2;
    }
    else
    {
        tim_clock = HAL_RCC_GetPCLK1Freq() * 2;
    }

    if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV2)
    {
        tim_clock = tim_clock / 2;
    }
    else if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV4)
    {
        tim_clock = tim_clock / 4;
    }

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    config->period = (__HAL_TIM_GET_AUTORELOAD(htim) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;
    config->pulse = (__HAL_TIM_GET_COMPARE(htim, channel) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;

    return RT_EOK;
}

rt_err_t rt_drv_pwm_set(TIM_HandleTypeDef *htim, struct rt_pwm_config *config)
{
    rt_uint32_t period, pulse;
    rt_uint64_t tim_clock, psc;
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (config->channel - 1);

    if(0)
    {
        tim_clock = HAL_RCC_GetPCLK2Freq() * 2;
    }
    else
    {
        tim_clock = HAL_RCC_GetPCLK1Freq() * 2;
    }

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    period = (unsigned long long)config->period * tim_clock / 1000ULL ;
    psc = period / MAX_PERIOD + 1;
    period = period / psc;
    __HAL_TIM_SET_PRESCALER(htim, psc - 1);

    if (period < MIN_PERIOD)
    {
        period = MIN_PERIOD;
    }
    __HAL_TIM_SET_AUTORELOAD(htim, period - 1);

    pulse = (unsigned long long)config->pulse * tim_clock / psc / 1000ULL;
    if (pulse < MIN_PULSE)
    {
        pulse = MIN_PULSE;
    }
    else if (pulse > period)
    {
        pulse = period;
    }
    __HAL_TIM_SET_COMPARE(htim, channel, pulse - 1);
    __HAL_TIM_SET_COUNTER(htim, 0);

    /* Update frequency value */
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);

    return RT_EOK;
}

static rt_err_t rt_hw_pwm_init(struct rt_pwm_drv *device)
{
    rt_err_t result = RT_EOK;
    TIM_HandleTypeDef *tim = RT_NULL;
    TIM_OC_InitTypeDef oc_config = {0};
    TIM_MasterConfigTypeDef master_config = {0};
    TIM_ClockConfigTypeDef clock_config = {0};

    RT_ASSERT(device != RT_NULL);

    tim = (TIM_HandleTypeDef *)&device->tim_handle;

    /* configure the timer to pwm mode */
    tim->Init.Prescaler = 0;
    tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    tim->Init.Period = 0;
    tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(tim) != HAL_OK)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    clock_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(tim, &clock_config) != HAL_OK)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(tim, &master_config) != HAL_OK)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    oc_config.OCMode = TIM_OCMODE_PWM1;
    oc_config.Pulse = 0;
    oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCFastMode = TIM_OCFAST_DISABLE;
    oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    oc_config.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* config pwm channel */
    if (device->channel & 0x01)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_1) != HAL_OK)
        {
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x02)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_2) != HAL_OK)
        {
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x04)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_3) != HAL_OK)
        {
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x08)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_4) != HAL_OK)
        {
            result = -RT_ERROR;
            goto __exit;
        }
    }

    /* pwm pin configuration */
    HAL_TIM_MspPostInit(tim);

    /* enable update request source */
    __HAL_TIM_URS_ENABLE(tim);

__exit:
    return result;
}

static void rt_pwm_get_channel(void)
{
#ifdef RT_USING_PWM1_CH1
    pwm_obj[PWM1_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM1_CH2
    pwm_obj[PWM1_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM1_CH3
    pwm_obj[PWM1_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM1_CH4
    pwm_obj[PWM1_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM2_CH1
    pwm_obj[PWM2_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM2_CH2
    pwm_obj[PWM2_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM2_CH3
    pwm_obj[PWM2_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM2_CH4
    pwm_obj[PWM2_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM3_CH1
    pwm_obj[PWM3_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM3_CH2
    pwm_obj[PWM3_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM3_CH3
    pwm_obj[PWM3_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM3_CH4
    pwm_obj[PWM3_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM4_CH1
    pwm_obj[PWM4_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM4_CH2
    pwm_obj[PWM4_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM4_CH3
    pwm_obj[PWM4_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM4_CH4
    pwm_obj[PWM4_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM5_CH1
    pwm_obj[PWM5_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM5_CH2
    pwm_obj[PWM5_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM5_CH3
    pwm_obj[PWM5_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM5_CH4
    pwm_obj[PWM5_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM6_CH1
    pwm_obj[PWM6_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM6_CH2
    pwm_obj[PWM6_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM6_CH3
    pwm_obj[PWM6_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM6_CH4
    pwm_obj[PWM6_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM7_CH1
    pwm_obj[PWM7_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM7_CH2
    pwm_obj[PWM7_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM7_CH3
    pwm_obj[PWM7_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM7_CH4
    pwm_obj[PWM7_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM8_CH1
    pwm_obj[PWM8_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM8_CH2
    pwm_obj[PWM8_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM8_CH3
    pwm_obj[PWM8_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM8_CH4
    pwm_obj[PWM8_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM9_CH1
    pwm_obj[PWM9_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM9_CH2
    pwm_obj[PWM9_INDEX].channel |= 1 << 1;
#endif
#ifdef RT_USING_PWM9_CH3
    pwm_obj[PWM9_INDEX].channel |= 1 << 2;
#endif
#ifdef RT_USING_PWM9_CH4
    pwm_obj[PWM9_INDEX].channel |= 1 << 3;
#endif
#ifdef RT_USING_PWM12_CH1
    pwm_obj[PWM12_INDEX].channel |= 1 << 0;
#endif
#ifdef RT_USING_PWM12_CH2
    pwm_obj[PWM12_INDEX].channel |= 1 << 1;
#endif
}

int rt_pwm_init()
{
    rt_size_t obj_num = sizeof(pwm_obj) / sizeof(pwm_obj[0]);

    rt_pwm_get_channel();

    for(int i = 0; i < obj_num; i++)
    {
        rt_hw_pwm_init(&pwm_obj[i]);
    }

    return RT_EOK;
}
INIT_PREV_EXPORT(rt_pwm_init);






