#include <drv_common.h>
#include "dev_key.h"
#include "drv_gpio.h"

rt_uint16_t rt_read_key(void)
{
    rt_uint16_t key_val = KEY_NULL;
    if(ENTRY_VAL == PIN_LOW || SWITCH_VAL == PIN_LOW || ADD_VAL == PIN_LOW || SUB_VAL == PIN_LOW)
    {
        rt_thread_delay(20);
        if(ENTRY_VAL == PIN_LOW && SWITCH_VAL == PIN_HIGH && ADD_VAL == PIN_HIGH && SUB_VAL == PIN_HIGH)
        {
            key_val = KEY_ENTRY_DOWN;
        }
        if(ENTRY_VAL == PIN_HIGH && SWITCH_VAL == PIN_LOW && ADD_VAL == PIN_HIGH && SUB_VAL == PIN_HIGH)
        {
            key_val = KEY_SWITCH_DOWN;
        }
        if(ENTRY_VAL == PIN_HIGH && SWITCH_VAL == PIN_HIGH && ADD_VAL == PIN_LOW && SUB_VAL == PIN_HIGH)
        {
            key_val = KEY_ADD_DOWN;
        }
        if(ENTRY_VAL == PIN_HIGH && SWITCH_VAL == PIN_HIGH && ADD_VAL == PIN_HIGH && SUB_VAL == PIN_LOW)
        {
            key_val = KEY_SUB_DOWN;
        }
    }
    return key_val;
}

int rt_key_dev_init(void)
{
    rt_pin_mode(ENTRY, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SWITCH, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(ADD, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SUB, PIN_MODE_INPUT_PULLUP);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_key_dev_init);




