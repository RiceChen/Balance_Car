#include <drv_common.h>
#include "drv_gpio.h"

#define BUZZER_PIN  GET_PIN(A, 12)

void rt_buzzer_on(void)
{
    rt_pin_write(BUZZER_PIN, PIN_LOW);
}

void rt_buzzer_off(void)
{
    rt_pin_write(BUZZER_PIN, PIN_HIGH);
}

int rt_buzzer_dev_init(void)
{
    rt_pin_mode(BUZZER_PIN, PIN_MODE_OUTPUT_OD);
    rt_pin_write(BUZZER_PIN, PIN_HIGH);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_buzzer_dev_init);
