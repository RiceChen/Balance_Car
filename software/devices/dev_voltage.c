#include <drv_common.h>
#include "drv_adc.h"

#include "dev_voltage.h"

#define VOLTAGE_ADC_CHANNEL     4
#define REFER_VOLTAGE           330
#define CONVERT_BITS            (1 << 12)

struct rt_adc_drv *voltage_adc_drv;

rt_uint32_t rt_get_voltage(void)
{
    rt_uint32_t vol = 0;
    rt_uint32_t value = 0;
    rt_get_adc_value(voltage_adc_drv, VOLTAGE_ADC_CHANNEL, &value);

    vol = (value * REFER_VOLTAGE / CONVERT_BITS) * 11;

    rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);

    return value;
}
MSH_CMD_EXPORT(rt_get_voltage, rt_get_voltage);

int rt_voltage_dev_init(void)
{
    voltage_adc_drv = rt_find_adc("adc1");
    if(voltage_adc_drv == RT_NULL)
    {
        rt_kprintf("find adc1 fail\n");
        return RT_ERROR;
    }

    rt_adc_enabled(voltage_adc_drv, RT_TRUE);

    return RT_EOK;
}
//INIT_DEVICE_EXPORT(rt_voltage_dev_init);
