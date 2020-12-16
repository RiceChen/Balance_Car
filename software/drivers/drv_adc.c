#include <drv_common.h>
#include "drv_adc.h"

enum
{
#ifdef RT_USING_ADC1
    ADC1_INDEX,
#endif
#ifdef RT_USING_ADC2
    ADC2_INDEX,
#endif
};

static struct rt_adc_config adc_config[] =
{
#ifdef RT_USING_ADC1
     ADC1_CONFIG,
#endif
#ifdef RT_USING_ADC2
     ADC2_CONFIG
#endif
};

static struct rt_adc_drv adc_obj[sizeof(adc_config) / sizeof(adc_config[0])] = {0};

static void rt_adc_configure(struct rt_adc_drv *obj)
{
    ADC_HandleTypeDef *handle = &obj->handle;

    handle->Instance = obj->config->Instance;
    handle->Init.ScanConvMode = ADC_SCAN_DISABLE;
    handle->Init.ContinuousConvMode = DISABLE;
    handle->Init.DiscontinuousConvMode = DISABLE;
    handle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    handle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handle->Init.NbrOfConversion = 1;
    handle->Init.NbrOfDiscConversion = 1;
    if (HAL_ADC_Init(handle) != HAL_OK)
    {
        Error_Handler();
    }
}

struct rt_adc_drv *rt_find_adc(char *name)
{
    rt_size_t obj_num = sizeof(adc_obj) / sizeof(struct rt_adc_drv);

    for(int i = 0; i < obj_num; i++)
    {
        if(rt_strncmp(adc_obj[i].config->name, name, RT_NAME_MAX) == 0)
        {
            return &adc_obj[i];
        }
    }
    return RT_NULL;
}

rt_err_t rt_adc_enabled(struct rt_adc_drv *obj, rt_bool_t enabled)
{
    ADC_HandleTypeDef *handle = &obj->handle;

    if(enabled)
    {
        __HAL_ADC_ENABLE(handle);
    }
    else
    {
        __HAL_ADC_DISABLE(handle);
    }
    return RT_EOK;
}

static rt_uint32_t rt_adc_get_channel(rt_uint32_t channel)
{
    rt_uint32_t stm32_channel = 0;

    switch (channel)
    {
    case  0:
        stm32_channel = ADC_CHANNEL_0;
        break;
    case  1:
        stm32_channel = ADC_CHANNEL_1;
        break;
    case  2:
        stm32_channel = ADC_CHANNEL_2;
        break;
    case  3:
        stm32_channel = ADC_CHANNEL_3;
        break;
    case  4:
        stm32_channel = ADC_CHANNEL_4;
        break;
    case  5:
        stm32_channel = ADC_CHANNEL_5;
        break;
    case  6:
        stm32_channel = ADC_CHANNEL_6;
        break;
    case  7:
        stm32_channel = ADC_CHANNEL_7;
        break;
    case  8:
        stm32_channel = ADC_CHANNEL_8;
        break;
    }

    return stm32_channel;
}

rt_err_t rt_get_adc_value(struct rt_adc_drv *obj, rt_uint32_t channel, rt_uint32_t *value)
{
    ADC_ChannelConfTypeDef config;
    ADC_HandleTypeDef *handler = &obj->handle;

    rt_memset(&config, 0, sizeof(config));

    config.Channel =  rt_adc_get_channel(channel);

    config.Rank = 1;

    config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

    HAL_ADC_ConfigChannel(handler, &config);

    /* start ADC */
    HAL_ADC_Start(handler);

    /* Wait for the ADC to convert */
    HAL_ADC_PollForConversion(handler, 100);

    /* get ADC value */
    *value = (rt_uint32_t)HAL_ADC_GetValue(handler);

    return RT_EOK;
}

static int rt_adc_init(void)
{
    rt_size_t obj_num = sizeof(adc_obj) / sizeof(struct rt_adc_drv);

    for(int i = 0; i < obj_num; i++)
    {
        adc_obj[i].config = &adc_config[i];
        rt_adc_configure(&adc_obj[i]);
    }

    return RT_EOK;
}
INIT_PREV_EXPORT(rt_adc_init);


