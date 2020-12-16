#include "board.h"
#include "drv_flash.h"

static uint32_t GetPage(uint32_t addr)
{
    uint32_t page = 0;
    page = RT_ALIGN_DOWN(addr, FLASH_PAGE_SIZE);
    return page;
}

int rt_flash_read(rt_uint32_t addr, rt_uint8_t *buf, size_t size)
{
    size_t i;

    if ((addr + size) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("read outrange flash size! addr is (0x%p)\n", (void *)(addr + size));
        return -RT_EINVAL;
    }

    for (i = 0; i < size; i++, buf++, addr++)
    {
        *buf = *(rt_uint8_t *) addr;
    }

    return size;
}

int rt_flash_write(rt_uint32_t addr, const rt_uint8_t *buf, size_t size)
{
    rt_err_t result        = RT_EOK;
    rt_uint32_t end_addr   = addr + size;

    if (addr % 4 != 0)
    {
        rt_kprintf("write addr must be 4-byte alignment\n");
        return -RT_EINVAL;
    }

    if ((end_addr) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("write outrange flash size! addr is (0x%p)\n", (void *)(addr + size));
        return -RT_EINVAL;
    }

    HAL_FLASH_Unlock();

    while (addr < end_addr)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *((rt_uint32_t *)buf)) == HAL_OK)
        {
            if (*(rt_uint32_t *)addr != *(rt_uint32_t *)buf)
            {
                result = -RT_ERROR;
                break;
            }
            addr += 4;
            buf  += 4;
        }
        else
        {
            result = -RT_ERROR;
            break;
        }
    }

    HAL_FLASH_Lock();

    if (result != RT_EOK)
    {
        return result;
    }

    return size;
}

static int rt_flash_erase_bank(uint32_t bank, rt_uint32_t addr, size_t size)
{
    rt_err_t result = RT_EOK;
    uint32_t PAGEError = 0;

    /*Variable used for Erase procedure*/
    FLASH_EraseInitTypeDef EraseInitStruct;

    if ((addr + size) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("ERROR: erase outrange flash size! addr is (0x%p)\n", (void *)(addr + size));
        return -RT_EINVAL;
    }

    HAL_FLASH_Unlock();

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = GetPage(addr);
    EraseInitStruct.NbPages     = (size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    EraseInitStruct.Banks       = bank;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        result = -RT_ERROR;
        goto __exit;
    }

__exit:
    HAL_FLASH_Lock();

    if (result != RT_EOK)
    {
        return result;
    }

    return size;
}


int rt_flash_erase(rt_uint32_t addr, size_t size)
{

    return rt_flash_erase_bank(FLASH_BANK_1, addr, size);
}

