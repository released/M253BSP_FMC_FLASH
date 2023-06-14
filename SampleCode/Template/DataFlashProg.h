/****************************************************************************//**
 * @file     DataFlashProg.h
 * @version  V0.10
 * @brief    Data flash programming driver header
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __DATA_FLASH_PROG_H__
#define __DATA_FLASH_PROG_H__

#define FILE_DATA_ADDR              (0x10000)
#define MASS_STORAGE_OFFSET         FILE_DATA_ADDR /* To avoid the code to write APROM */
// #define DATA_FLASH_STORAGE_SIZE     (512)  /* Configure the DATA FLASH storage size. To pass USB-IF MSC Test, it needs > 64KB */


/* Windows will consume about 20KB for file system formatting.  Free space will be less than 22KB*/
#define FLASH_PAGE_SIZE             512
#define BUFFER_PAGE_SIZE            512

#endif  /* __DATA_FLASH_PROG_H__ */
