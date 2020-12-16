#ifndef DRIVERS_DRV_ADC_H_
#define DRIVERS_DRV_ADC_H_

#define RT_USING_ADC1

struct rt_adc_config
{
    char *name;
    ADC_TypeDef *Instance;
};

struct rt_adc_drv
{
    ADC_HandleTypeDef handle;
    struct rt_adc_config *config;
};

#ifdef RT_USING_ADC1
#define ADC1_CONFIG                                               \
    {                                                             \
        .name = "adc1",                                           \
        .Instance = ADC1,                                         \
    }
#endif

#ifdef RT_USING_ADC2
#define ADC2_CONFIG                                               \
    {                                                             \
        .name = "adc2",                                           \
        .Instance = ADC2,                                         \
    }
#endif

struct rt_adc_drv *rt_find_adc(char *name);
rt_err_t rt_adc_enabled(struct rt_adc_drv *obj, rt_bool_t enabled);
rt_err_t rt_get_adc_value(struct rt_adc_drv *obj, rt_uint32_t channel, rt_uint32_t *value);

#endif /* DRIVERS_DRV_ADC_H_ */
