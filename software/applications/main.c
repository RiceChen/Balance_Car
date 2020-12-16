#include <rtthread.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    int count = 1;

    while (count++)
    {
//        LOG_D("RT-Thread hello");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
