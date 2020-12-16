#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

#include <rtthread.h>
#include <rthw.h>
#include <drv_common.h>

int rt_flash_read(rt_uint32_t addr, rt_uint8_t *buf, size_t size);
int rt_flash_write(rt_uint32_t addr, const rt_uint8_t *buf, size_t size);
int rt_flash_erase(rt_uint32_t addr, size_t size);

#endif  /* __DRV_FLASH_H__ */
