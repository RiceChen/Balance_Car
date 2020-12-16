#ifndef DRIVERS_DRV_GPIO_H_
#define DRIVERS_DRV_GPIO_H_

#define PIN_LOW                         0x00
#define PIN_HIGH                        0x01

#define PIN_MODE_OUTPUT                 0x00
#define PIN_MODE_INPUT                  0x01
#define PIN_MODE_INPUT_PULLUP           0x02
#define PIN_MODE_INPUT_PULLDOWN         0x03
#define PIN_MODE_OUTPUT_OD              0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1

struct rt_device_pin_mode
{
    rt_uint16_t pin;
    rt_uint16_t mode;
};
struct rt_device_pin_status
{
    rt_uint16_t pin;
    rt_uint16_t status;
};
struct rt_pin_irq_hdr
{
    rt_int16_t        pin;
    rt_uint16_t       mode;
    void (*hdr)(void *args);
    void             *args;
};

void rt_pin_mode(rt_base_t pin, rt_base_t mode);
void rt_pin_write(rt_base_t pin, rt_base_t value);
int rt_pin_read(rt_base_t pin);
rt_err_t rt_pin_attach_irq(rt_int32_t pin, rt_uint32_t mode, void (*hdr)(void *args), void *args);
rt_err_t rt_pin_dettach_irq(rt_int32_t pin);
rt_err_t rt_pin_irq_enable(rt_base_t pin, rt_uint32_t enabled);

#endif /* DRIVERS_DRV_GPIO_H_ */
