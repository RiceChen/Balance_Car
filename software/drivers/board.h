/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     RealThread   first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stm32f1xx.h>
#include <drv_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------- CHIP CONFIG BEGIN --------------------------*/

#define CHIP_FAMILY_STM32
#define CHIP_SERIES_STM32F1
#define CHIP_NAME_STM32F103C8

/*-------------------------- CHIP CONFIG END --------------------------*/

/*-------------------------- ROM/RAM CONFIG BEGIN --------------------------*/

#define ROM_START              ((uint32_t)0x08000000)
#define ROM_SIZE               (64 * 1024)
#define ROM_END                ((uint32_t)(ROM_START + ROM_SIZE))

#define RAM_START              (0x20000000)
#define RAM_SIZE               (20 * 1024)
#define RAM_END                (RAM_START + RAM_SIZE)

/*-------------------------- ROM/RAM CONFIG END --------------------------*/

/*-------------------------- CLOCK CONFIG BEGIN --------------------------*/

#define BSP_CLOCK_SOURCE                  ("HSI")
#define BSP_CLOCK_SOURCE_FREQ_MHZ         ((int32_t)0)
#define BSP_CLOCK_SYSTEM_FREQ_MHZ         ((int32_t)72)

/*-------------------------- CLOCK CONFIG END --------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
