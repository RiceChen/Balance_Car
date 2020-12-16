#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include <rtthread.h>
#include <board.h>

#define RT_USING_UART1
#define RT_USING_UART3

struct rt_uart_config
{
    char *name;
    USART_TypeDef *Instance;
};

struct rt_uart_drv
{
    UART_HandleTypeDef handle;
    struct rt_uart_config *config;

    void (*irq_handler)(rt_uint8_t *data);
};

#ifdef RT_USING_UART1
#define UART1_CONFIG                                                \
    {                                                               \
        .name = "uart1",                                            \
        .Instance = USART1,                                         \
    }
#endif

#ifdef RT_USING_UART2
#define UART2_CONFIG                                                \
    {                                                               \
        .name = "uart2",                                            \
        .Instance = USART2,                                         \
    }
#endif

#ifdef RT_USING_UART3
#define UART3_CONFIG                                                \
    {                                                               \
        .name = "uart3",                                            \
        .Instance = USART3,                                         \
    }
#endif

struct rt_uart_drv *rt_find_uart(char *name);
void rt_uart_irq_register_cb(struct rt_uart_drv *obj, void (*irq_handler)(rt_uint8_t *));
void rt_uart_irq_enable(struct rt_uart_drv *obj, rt_bool_t enable);
int rt_uart_send(struct rt_uart_drv *obj, rt_uint8_t *data, rt_size_t len);


#endif /* drv_uart.h */
