#include <drv_uart.h>
#include "stdlib.h"
#include "drv_common.h"

#define DBG_TAG              "drv.usart"
#ifdef DRV_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#include <rtdbg.h>

#ifdef RT_USING_CONSOLE

enum
{
#ifdef RT_USING_UART1
    UART1_INDEX,
#endif
#ifdef RT_USING_UART2
    UART2_INDEX,
#endif
#ifdef RT_USING_UART3
    UART3_INDEX,
#endif
};

static struct rt_uart_config uart_config[] =
{
#ifdef RT_USING_UART1
     UART1_CONFIG,
#endif
#ifdef RT_USING_UART2
     UART2_CONFIG
#endif
#ifdef RT_USING_UART3
     UART3_CONFIG,
#endif
};

static struct rt_uart_drv uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static void rt_uart_config(struct rt_uart_drv *obj)
{
    UART_HandleTypeDef *handle = &obj->handle;

    handle->Instance = obj->config->Instance;
    handle->Init.BaudRate = 115200;
    handle->Init.WordLength = UART_WORDLENGTH_8B;
    handle->Init.StopBits = UART_STOPBITS_1;
    handle->Init.Parity = UART_PARITY_NONE;
    handle->Init.Mode = UART_MODE_TX_RX;
    handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handle->Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(handle) != HAL_OK)
    {
        Error_Handler();
    }
}

struct rt_uart_drv *rt_find_uart(char *name)
{
    rt_size_t obj_num = sizeof(uart_obj) / sizeof(struct rt_uart_drv);

    for(int i = 0; i < obj_num; i++)
    {
        if(rt_strncmp(uart_obj[i].config->name, name, RT_NAME_MAX) == 0)
        {
            return &uart_obj[i];
        }
    }
    return RT_NULL;
}

int rt_uart_send(struct rt_uart_drv *obj, rt_uint8_t *data, rt_size_t len)
{
    for(int i = 0; i < len; i++)
    {
        HAL_UART_Transmit(&obj->handle, (uint8_t *)(data + i), 1, 1);
    }

    return len;
}


void rt_uart_irq_enable(struct rt_uart_drv *obj, rt_bool_t enable)
{
    if(enable == RT_TRUE)
    {
        __HAL_UART_ENABLE_IT(&obj->handle, UART_IT_RXNE);
    }
}

void rt_uart_irq_register_cb(struct rt_uart_drv *obj, void (*irq_handler)(rt_uint8_t *))
{
    if(obj->irq_handler == RT_NULL)
    {
        obj->irq_handler = irq_handler;
    }
}

static void rt_uart_isr(rt_uint8_t index)
{
    rt_uint8_t ch = 0;

    if(uart_obj[index].irq_handler)
    {
        if(__HAL_UART_GET_FLAG(&uart_obj[index].handle, UART_FLAG_RXNE) != RESET)
        {
            ch = uart_obj[index].config->Instance->DR & 0xff;
            uart_obj[index].irq_handler(&ch);
        }
    }
}

#ifdef RT_USING_UART1
void USART1_IRQHandler(void)
{

    /* enter interrupt */
    rt_interrupt_enter();

    rt_uart_isr(UART1_INDEX);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif

#ifdef RT_USING_UART2
void USART2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_uart_isr(UART2_INDEX);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif


#ifdef RT_USING_UART3
void USART3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_uart_isr(UART3_INDEX);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif

struct rt_uart_drv *drv_console_uart;

static int rt_hw_uart_init(void)
{
    rt_size_t obj_num = sizeof(uart_obj) / sizeof(struct rt_uart_drv);

    for(int i = 0; i < obj_num; i++)
    {
        uart_obj[i].config = &uart_config[i];
        rt_uart_config(&uart_obj[i]);
    }

    drv_console_uart = rt_find_uart("uart1");

    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_uart_init);

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            HAL_UART_Transmit(&drv_console_uart->handle, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&drv_console_uart->handle, (uint8_t *)(str + i), 1, 1);
    }
}

#ifdef RT_USING_FINSH
char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (__HAL_UART_GET_FLAG(&drv_console_uart->handle, UART_FLAG_RXNE) != RESET)
    {
        ch = drv_console_uart->config->Instance->DR & 0xff;
    }
    else
    {
        if(__HAL_UART_GET_FLAG(&drv_console_uart->handle, UART_FLAG_ORE) != RESET)
        {
            __HAL_UART_CLEAR_OREFLAG(&drv_console_uart->handle);
        }
        rt_thread_mdelay(10);
    }
    return ch;
}
#endif /* RT_USING_FINSH */
#endif /* RT_USING_CONSLONE */
