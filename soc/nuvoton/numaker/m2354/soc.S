/****************************************************************************//**
 * @file     startup_M2354.S
 * @version  V1.00
 * @brief    CMSIS Device Startup File
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>

_ASM_FILE_PROLOGUE

GTEXT(soc_reset_hook)
GTEXT(__PC)
SECTION_SUBSEC_FUNC(TEXT,_reset_section,soc_reset_hook)

/*
    .syntax unified
    .arch   armv8 - m.base


    .align  2
    .thumb_func
    .global __PC
    .type   __PC, % function
__PC:

    MOV     r0, lr
    BLX     lr
    .size   __PC, . - __PC
*/
.soc_reset_hook:
    /* Check SecureWorld */
    /* MOV     R0, R15
    LSLS    R0, R0, #3
    BMI.N   GotoSystemInit*/

    /* Unlock Register. TODO: Add loop! */
    LDR     R0, =0x40000100
unlock.loop:
    LDR     R1, =0x59
    STR     R1, [R0]
    LDR     R1, =0x16
    STR     R1, [R0]
    LDR     R1, =0x88
    STR     R1, [R0]

    LDR     R1, [R0]
    CMP     R1, #0
    BEQ.N   unlock.loop


    /* power gating */
    /* M32(0x400001f4) = 0xfffffffful; */
    LDR     R0, =0x400001f4
    LDR     R1, =0xffffffff
    STR     R1, [R0]

    /* M32(0x400000dC) = 0ul; */
    LDR     R0, =0x400000dC
    LDR     R1, =0x0
    STR     R1, [R0]

    /* Enable GPIO clks, SRAM clks, Trace clk */
    /* CLK->AHBCLK |= (0xffful << 20) | (1ul << 14); */

    LDR     R0, =0x40000200
    LDR     R1, [R0,#0x4]

    LDR     R2, =0xfff02000

    ORRS    R1, R1, R2
    STR     R1, [R0,#0x4]

    BX  LR

/*SECTION_SUBSEC_FUNC(TEXT,_reset_section,__PC)*/

__PC:

    MOV     r0, lr
    BLX     lr
    .size   __PC, . - __PC


    .end

