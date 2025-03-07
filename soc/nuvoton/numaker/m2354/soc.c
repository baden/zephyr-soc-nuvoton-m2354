/*
 * Copyright (c) 2023 Nuvoton Technology Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/drivers/clock_control/clock_control_numaker.h>
#include <kernel_internal.h>
/* Hardware and starter kit includes. */
#include <NuMicro.h>

#define __HXT         (12000000UL)  /*!< High Speed External Crystal Clock Frequency */

// static uint32_t  g_auPageBuff[FMC_FLASH_PAGE_SIZE / 4];

#if 0
void SYS_Init()
{
    /* If the macros do not exist in your project, please refer to the related clk.h in Header folder of the tool package */
    /* Enable clock source */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for clock source ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk | CLK_STATUS_HIRCSTB_Msk);

    /* Set HCLK clock */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(3));

    /* Enable IP clock */
    // CLK_EnableModuleClock(EADC_MODULE);
    //CLK_EnableModuleClock(FMCIDLE_MODULE);
    // CLK_EnableModuleClock(GPA_MODULE);
    // CLK_EnableModuleClock(GPB_MODULE);
    // CLK_EnableModuleClock(GPC_MODULE);
    // CLK_EnableModuleClock(GPD_MODULE);
    // CLK_EnableModuleClock(GPE_MODULE);
    // CLK_EnableModuleClock(GPF_MODULE);
    CLK_EnableModuleClock(RTC_MODULE);
    CLK_EnableModuleClock(SRAM0_MODULE);
    CLK_EnableModuleClock(SRAM1_MODULE);
    CLK_EnableModuleClock(SRAM2_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    /* Enable EADC module clock */

    // CLK_EnableModuleClock(UART3_MODULE);
    // CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK_DIV2, 0);

    /* Set IP clock */
    // CLK_SetModuleClock(EADC_MODULE, MODULE_NoMsk, CLK_CLKDIV0_EADC(1));

    /* Set EADC clock divider as 12 */
    // CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(12));

    RTC->LXTCTL |= RTC_LXTCTL_RTCCKSEL_Msk;
    // CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_LIRC, MODULE_NoMsk);
    // CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_LIRC, MODULE_NoMsk);
    // CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, MODULE_NoMsk);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, MODULE_NoMsk);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, MODULE_NoMsk);

    // CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL2_UART3SEL_HIRC, CLK_CLKDIV4_UART3(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    // SYS->GPC_MFPL = (SYS->GPC_MFPL & (~(UART3_RXD_PC2_Msk | UART3_TXD_PC3_Msk))) | UART3_RXD_PC2 | UART3_TXD_PC3;

    // SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB14MFP_Msk)) | EADC0_CH14_PB14;
    // SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk)) | EADC0_CH12_PB12;

    /* Disable digital input path of EADC analog pin to prevent leakage */

    // TODO: Move to driver/adc/adc_numaker.c
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT6 | BIT14 /*| BIT13*/ | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT15);
}
#endif

static void SendChar_ToUART_3(int ch)
{
	UART_Write(UART3, (uint8_t *)&ch, 1);
}


void early_debug_uart_init()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    // SYS_Init_test();

    /* Enable UART3 module clock */
    CLK_EnableModuleClock(UART3_MODULE);

    /* Select UART3 module clock source as HIRC and UART3 module clock divider as 1 */
    CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL2_UART3SEL_HIRC, CLK_CLKDIV4_UART3(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set multi-function pins for UART3 RXD(PC.2) and TXD(PC.3) */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~(UART3_RXD_PC2_Msk | UART3_TXD_PC3_Msk))) | UART3_RXD_PC2 | UART3_TXD_PC3;

    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART3, 115200);

    SendChar_ToUART_3('\n'); SendChar_ToUART_3('+'); SendChar_ToUART_3('\n');
}

#if 0
void soc_reset_hook_1(void);

/*
        do
        {
            SYS->REGLCTL = 0x59;
            SYS->REGLCTL = 0x16;
            SYS->REGLCTL = 0x88;
        }
        while(!SYS->REGLCTL);
        SYS->PSWCTL = SYS_PSWCTL_CRPTPWREN_Msk;
        CLK->AHBCLK |= (0xffful << 20) | (1ul << 14);
*/

void __attribute__((naked)) soc_reset_hook(void)
{
    __asm__ volatile(
    "loop:\n"
    "movs    r2, #128\n"        // 0x80
    "lsls    r2, r2, #23\n"
    "movw    r3, #256\n"        // 0x100
    "movs    r1, #89\n"         // 0x59"
    "str     r1, [r2, r3]\n"
    "subs    r1, #67\n"         // 0x43
    "str     r1, [r2, r3]\n"
    "adds    r1, #114\n"        // 0x72
    "str     r1, [r2, r3]\n"
    "ldr     r3, [r2, r3]\n"
    "cmp     r3, #0\n"
    "beq.n   loop\n"            // <SystemInit+10>
    "movw    r3, #500\n"        // 0x1f4
    "movw    r1, #4096\n"       // 0x1000
    "str     r1, [r2, r3]\n"
    "movw    r2, #512\n"        // 0x200
    "movt    r2, #16384\n"      // 0x4000
    "ldr     r1, [r2, #4]\n"
    "movw    r3, #16384\n"      // 0x4000
    "movt    r3, #65520\n"      // 0xfff0
    "orrs    r3, r1\n"
    "str     r3, [r2, #4]\n"
    "b      soc_reset_hook_1\n"
    );
    //: "r0", "r1", "r2", "r3", "memory");
}
#endif

void soc_reset_hook0(void)
{
    do
    {
        SYS->REGLCTL = 0x59;
        SYS->REGLCTL = 0x16;
        SYS->REGLCTL = 0x88;
    }
    while(!SYS->REGLCTL);

    /* Enable Crypto power switch */
    SYS->PSWCTL = SYS_PSWCTL_CRPTPWREN_Msk;

    /* Enable all GPIO, SRAM and Trace clock */
    CLK->AHBCLK |= (0xffful << 20) | (1ul << 14);
}

#if 1
// Цей варіант не працює якшо стек вилізає за межі RAM BANK0
// void z_arm_platform_init(void)
void soc_early_init_hook(void)
// void soc_reset_hook1(void)
{
    // SystemInit();
#if 0
    do
    {
        SYS->REGLCTL = 0x59;
        SYS->REGLCTL = 0x16;
        SYS->REGLCTL = 0x88;
    }
    while(!SYS->REGLCTL);

    /* Enable Crypto power switch */
    SYS->PSWCTL = SYS_PSWCTL_CRPTPWREN_Msk;

    /* Enable all GPIO, SRAM and Trace clock */
    CLK->AHBCLK |= (0xffful << 20) | (1ul << 14);
#endif

    #else

void soc_reset_hook_1(void)
{
    #endif

#if 0

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

	return;

	SystemInit();
#endif

	/* Unlock protected registers */
	SYS_UnlockReg();

    /* Release I/O hold status */
    // SYS->IOPDCLTL = 1;

	/*
	 * -------------------
	 * Init System Clock
	 * -------------------
	 */

#if DT_NODE_HAS_PROP(DT_NODELABEL(scc), hxt)
	/* Enable/disable 4~24 MHz external crystal oscillator (HXT) */
	if (DT_ENUM_IDX(DT_NODELABEL(scc), hxt) == NUMAKER_SCC_CLKSW_ENABLE) {
		CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
		/* Wait for HXT clock ready */
		CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
	} else if (DT_ENUM_IDX(DT_NODELABEL(scc), hxt) == NUMAKER_SCC_CLKSW_DISABLE) {
		CLK_DisableXtalRC(CLK_PWRCTL_HXTEN_Msk);
	}
#endif

#if DT_NODE_HAS_PROP(DT_NODELABEL(scc), lxt)
	/* Enable/disable 32.768 kHz low-speed external crystal oscillator (LXT) */
	if (DT_ENUM_IDX(DT_NODELABEL(scc), lxt) == NUMAKER_SCC_CLKSW_ENABLE) {
		CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
		/* Wait for LXT clock ready */
		CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
	} else if (DT_ENUM_IDX(DT_NODELABEL(scc), lxt) == NUMAKER_SCC_CLKSW_DISABLE) {
		CLK_DisableXtalRC(CLK_PWRCTL_LXTEN_Msk);
	}
#endif

	/* Enable 12 MHz high-speed internal RC oscillator (HIRC) */
	CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
	/* Wait for HIRC clock ready */
	CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Select HCLK clock source as HIRC and and HCLK clock divider as 1 */

#if DT_PROP(DT_NODELABEL(scc), core_clock) == 12000000
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
#elif DT_PROP(DT_NODELABEL(scc), core_clock) == 6000000
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(2));
#elif DT_PROP(DT_NODELABEL(scc), core_clock) == 4000000
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(3));
#elif DT_PROP(DT_NODELABEL(scc), core_clock) == 3000000
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(4));
#endif

	/* Enable 10 KHz low-speed internal RC oscillator (LIRC)      (Чи нам він треба?)*/
	CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
	/* Wait for LIRC clock ready */
	CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

#if DT_NODE_HAS_PROP(DT_NODELABEL(scc), hirc48)
	/* Enable/disable 48 MHz high-speed internal RC oscillator (HIRC48) */
	if (DT_ENUM_IDX(DT_NODELABEL(scc), hirc48) == NUMAKER_SCC_CLKSW_ENABLE) {
		CLK_EnableXtalRC(CLK_PWRCTL_HIRC48EN_Msk);
		/* Wait for HIRC48 clock ready */
		CLK_WaitClockReady(CLK_STATUS_HIRC48STB_Msk);
	} else if (DT_ENUM_IDX(DT_NODELABEL(scc), hirc48) == NUMAKER_SCC_CLKSW_DISABLE) {
		CLK_DisableXtalRC(CLK_PWRCTL_HIRC48EN_Msk);
	}
#endif

#if DT_NODE_HAS_PROP(DT_NODELABEL(scc), clk_pclkdiv)
	/* Set CLK_PCLKDIV register on request */
	CLK->CLKDIV0 = DT_PROP(DT_NODELABEL(scc), clk_pclkdiv);
#endif

    // CLK_SetCoreClock(8000000);

// #if DT_NODE_HAS_PROP(DT_NODELABEL(scc), core_clock)

#if (DT_PROP(DT_NODELABEL(scc), core_clock) > 24000000) && (DT_PROP(DT_NODELABEL(scc), core_clock) <= 96000000)   
    // for(;;) continue;
	/* Set core clock (HCLK) on request */
	CLK_SetCoreClock(DT_PROP(DT_NODELABEL(scc), core_clock));
#endif

    CLK_EnableModuleClock(SRAM0_MODULE);
    CLK_EnableModuleClock(SRAM1_MODULE);
    CLK_EnableModuleClock(SRAM2_MODULE);


	/*
	 * Update System Core Clock
	 * User can use SystemCoreClockUpdate() to calculate SystemCoreClock.
	 */
	SystemCoreClockUpdate();

    // SysTick
    // CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, 0);
    // CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK_DIV2, 0);

	/* Lock protected registers */
	SYS_LockReg();

    early_debug_uart_init();

#if 0
    /*
        This sample code is used to show how to use StdDriver API to implement ISP functions.
    */
    TPF_test("early_boot");
    // for(;;) continue;
#endif
}

static volatile int32_t g_ICE_Conneced = 1;

/**
 * @brief    This function is called by Hardfault handler.
 * @param    None
 * @returns  None
 * @details  This function is called by Hardfault handler and check if it is caused by __BKPT or not.
 *
 */

uint32_t ProcessHardFault(uint32_t lr, uint32_t msp, uint32_t psp)
{
    uint32_t *sp = NULL;
    uint32_t inst = 0;

    /* Check the used stack */
    if(lr & 0x40)
    {
        /* Secure stack used */
        if(lr & 4)
            sp = (uint32_t *)psp;
        else
            sp = (uint32_t *)msp;

    }
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
    else
    {
        /* Non-secure stack used */
        if(lr & 4)
            sp = (uint32_t *)__TZ_get_PSP_NS();
        else
            sp = (uint32_t *)__TZ_get_MSP_NS();

    }
#endif

    /* Get the instruction caused the hardfault */
    if( sp != NULL )
        inst = M16(sp[6]);


    if(inst == 0xBEAB)
    {
        /*
            If the instruction is 0xBEAB, it means it is caused by BKPT without ICE connected.
            We still return for output/input message to UART.
        */
        g_ICE_Conneced = 0; // Set a flag for ICE offline
        sp[6] += 2; // return to next instruction
        return lr;  // Keep lr in R0
    }

    /* It is casued by hardfault (Not semihost). Just process the hard fault here. */
    /* TODO: Implement your hardfault handle code here */

    /*
    printf("  HardFault!\n\n");
    printf("r0  = 0x%x\n", sp[0]);
    printf("r1  = 0x%x\n", sp[1]);
    printf("r2  = 0x%x\n", sp[2]);
    printf("r3  = 0x%x\n", sp[3]);
    printf("r12 = 0x%x\n", sp[4]);
    printf("lr  = 0x%x\n", sp[5]);
    printf("pc  = 0x%x\n", sp[6]);
    printf("psr = 0x%x\n", sp[7]);
    */

    while(1) {}

}


/**
 *
 * @brief      The function to process semihosted command
 * @param[in]  n32In_R0  : semihost register 0
 * @param[in]  n32In_R1  : semihost register 1
 * @param[out] pn32Out_R0: semihost register 0
 * @retval     0: No ICE debug
 * @retval     1: ICE debug
 *
 */

#if 0
int32_t SH_Return(int32_t n32In_R0, int32_t n32In_R1, int32_t *pn32Out_R0)
{
    (void)n32In_R1;

    if(g_ICE_Conneced)
    {
        if(pn32Out_R0)
            *pn32Out_R0 = n32In_R0;

        return 1;
    }
    return 0;
}
#endif

#if 0
static int test_tmc1(void)
{
    TPF_test("post_kernel");
}

SYS_INIT(test_tmc1, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_OBJECTS);
#endif
