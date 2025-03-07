#include <NuMicro.h>

/*
+----------------------------------------+
|        FMC ISP Sample Code             |
+----------------------------------------+
  Company ID ............................ [0x000000da]
  Product ID ............................ [0xa1735410]
  Unique ID 0 ........................... [0x001a0031]
  Unique ID 1 ........................... [0x0458c09a]
  Unique ID 2 ........................... [0x000027a0]
  Unique Customer ID 0 .................. [0xffffffff]
  Unique Customer ID 1 .................. [0xffffffff]
  Unique Customer ID 2 .................. [0xffffffff]
  Unique Customer ID 3 .................. [0xffffffff]
  User Config 0 ......................... [0xffffffff]
  User Config 1 ......................... [0xffffffff]
  User Config 2 ......................... [0xffffffff]
  User Config 3 ......................... [0xffffffff]


LDROM test =>
    Flash test address: 0x100000        Flash test address: 0x100800        Flash test address: 0x101000        Flash test address: 0x101800        Flash test address: 0x102000        Flash test address: 0x102800        Flash test address: 0x103000        Flash test address: 0x103800        Flash Test Passed.          


APROM test =>
    Flash test address: 0x10000        Flash test address: 0x10800        Flash test address: 0x11000        Flash test address: 0x11800        Flash test address: 0x12000        Flash test address: 0x12800        Flash test address: 0x13000        Flash test address: 0x13800        Flash Test Passed.          

FMC Sample Code Completed.
*/

// void SYS_Init_test(void)
// {

//     /*---------------------------------------------------------------------------------------------------------*/
//     /* Init System Clock                                                                                       */
//     /*---------------------------------------------------------------------------------------------------------*/

//     /* Enable HIRC clock */
//     CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

//     /* Wait for HIRC clock ready */
//     CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//     /* Set core clock to 96MHz */
//     CLK_SetCoreClock(96000000);

//     /* Enable UART3 module clock */
//     CLK_EnableModuleClock(UART3_MODULE);

//     /* Select UART3 module clock source as HIRC and UART3 module clock divider as 1 */
//     CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL2_UART3SEL_HIRC, CLK_CLKDIV4_UART3(1));

//     /*---------------------------------------------------------------------------------------------------------*/
//     /* Init I/O Multi-function                                                                                 */
//     /*---------------------------------------------------------------------------------------------------------*/

//     /* Set multi-function pins for UART3 RXD(PC.2) and TXD(PC.3) */
//     SYS->GPC_MFPL = (SYS->GPC_MFPL & (~(UART3_RXD_PC2_Msk | UART3_TXD_PC3_Msk))) | UART3_RXD_PC2 | UART3_TXD_PC3;
//     // SYS->GPA_MFPL = (SYS->GPA_MFPL & (~(UART0_RXD_PA6_Msk | UART0_TXD_PA7_Msk))) | UART0_RXD_PA6 | UART0_TXD_PA7;

// }

// TODO: Works only for UART3/PC.2/PC.3

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

    SendChar_ToUART('\n'); SendChar_ToUART('+'); SendChar_ToUART('\n');
    SendChar_ToUART('\n'); SendChar_ToUART('-'); SendChar_ToUART('\n');
}


void UART3_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART3                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART3 */
    SYS_ResetModule(UART3_RST);

    /* Configure UART3 and set UART3 baud rate */
    UART_Open(UART3, 115200);
}

void SendChar_ToUART_3(int ch)
{
	UART_Write(UART3, (uint8_t *)&ch, 1);
}


int arch_printk_char_out(int c)
{
    if (c == '\n') {
        // esp_rom_uart_tx_one_char('\r');
        SendChar_ToUART_3('\r');
    }
	SendChar_ToUART_3(c);
	return 0;
}


void __print_string(const char *str)
{
    while (*str) {
        SendChar_ToUART_3(*str++);
    }
}

// Print hexadecimal value 0..F
void __prtint_hexc(uint8_t v)
{
    if (v < 10) {
        v += '0';
    } else {
        v += 'A' - 10;
    }
    SendChar_ToUART_3(v);
}

void __prtint_hexb(uint8_t v)
{
    __prtint_hexc(v >> 4);
    __prtint_hexc(v & 0x0F);
}

void __prtint_hexw(uint16_t v)
{
    __prtint_hexb(v >> 8);
    __prtint_hexb(v & 0xFF);
}

void __prtint_hexd(uint32_t v)
{
    __prtint_hexw(v >> 16);
    __prtint_hexw(v & 0xFFFF);
}



static int buf_char_out(int c, void *ctx_p)
{
    SendChar_ToUART_3(c);
    return 0;
}

#include <zephyr/sys/cbprintf.h>

void _vprintk(const char *fmt, va_list ap)
{
    // struct buf_out_context ctx = {0};
    cbvprintf(buf_char_out, NULL, fmt, ap);
}

void _printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_vprintk(fmt, ap);
	va_end(ap);
}

#define TEST_PATTERN                0x5A5A5A5A
#define APROM_TEST_BASE             0x80000
// #define APROM_TEST_END              APROM_TEST_BASE+0x4000
#define APROM_TEST_END              APROM_TEST_BASE+0x1000

// #define APROM_TEST_BASE             0x20000
// #define APROM_TEST_END              APROM_TEST_BASE+0x800


// #if defined(printf)
// #undef printf
#define printf _printk
// #endif

static int32_t FillDataPattern(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t u32Addr;
    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4) {
        if (FMC_Write(u32Addr, u32Pattern) != 0)  {         /* Program flash */
            printf("FMC_Write address 0x%x failed!\n", u32Addr);
            return -1;
        }
    }
    return 0;
}

static int32_t  VerifyData(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;
    uint32_t    u32Data;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4) {
        u32Data = FMC_Read(u32Addr);

        if(g_FMC_i32ErrCode != 0) {
            printf("FMC_Read address 0x%x failed!\n", u32Addr);
            return -1;
        }
        if(u32Data != u32Pattern)
        {
            printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr, u32Data, u32Pattern);
            return -1;
        }
    }
    return 0;
}

static int32_t  FlashTest(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;
    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += FMC_FLASH_PAGE_SIZE) {
        printf("    Flash test address: 0x%x    \n", u32Addr);
        // Erase page
        if(FMC_Erase(u32Addr) != 0) {
            printf("FMC_Erase address 0x%x failed!\n", u32Addr);
            return -1;
        }
        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0) {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }

        // Write test pattern to fill the whole page
        if(FillDataPattern(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0) {
            printf("Failed to write page 0x%x!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all equal to test pattern
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0) {
            printf("\nData verify failed!\n ");
            return -1;
        }

    #if 0
        // Erase page
        if(FMC_Erase(u32Addr) != 0) {
            printf("FMC_Erase address 0x%x failed!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0) {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }
    #endif
    }
    printf("Flash Test Passed.\n");
    return 0;
}


void TPF_test(const char *title)
{
/* Спробуємо вставити сюди тест роботи з памʼятью */

    uint32_t i, u32Data;

    // Try to make my own print function

    // _printk("Testing Flash...\n");
    printf("Testing Flash...(%s)\n", title);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function */
    FMC_Open();

#if 0
    u32Data = FMC_ReadCID();
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_ReadCID failed!\n");
        goto lexit;
    }
    printf("  Company ID ............................ [0x%08x]\n", u32Data);
#endif
    u32Data = FMC_ReadPID();
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_ReadPID failed!\n");
        goto lexit;
    }
    printf("  Product ID ............................ [0x%08x]\n", u32Data);
#if 0
    for(i = 0; i < 3; i++)
    {
        u32Data = FMC_ReadUID((uint8_t)i);
        if (g_FMC_i32ErrCode != 0)
        {
            printf("FMC_ReadUID %d failed!\n", i);
            goto lexit;
        }
        printf("  Unique ID %d ........................... [0x%08x]\n", i, u32Data);
    }

    for(i = 0; i < 4; i++)
    {
        u32Data = FMC_ReadUCID(i);
        if (g_FMC_i32ErrCode != 0)
        {
            printf("FMC_ReadUCID %d failed!\n", i);
            goto lexit;
        }
        printf("  Unique Customer ID %d .................. [0x%08x]\n", i, u32Data);
    }

    /* Read User Configuration */
    printf("  User Config 0 ......................... [0x%08x]\n", FMC_Read(FMC_USER_CONFIG_0));
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_Read(FMC_CONFIG_BASE) failed!\n");
        goto lexit;
    }

    printf("  User Config 1 ......................... [0x%08x]\n", FMC_Read(FMC_USER_CONFIG_1));
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_Read(FMC_USER_CONFIG_1) failed!\n");
        goto lexit;
    }

    printf("  User Config 2 ......................... [0x%08x]\n", FMC_Read(FMC_USER_CONFIG_2));
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_Read(FMC_USER_CONFIG_2) failed!\n");
        goto lexit;
    }

    printf("  User Config 3 ......................... [0x%08x]\n", FMC_Read(FMC_USER_CONFIG_3));
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_Read(FMC_USER_CONFIG_3) failed!\n");
        goto lexit;
    }

    printf("\n\nLDROM test =>\n");
    FMC_ENABLE_LD_UPDATE();
    if(FlashTest(FMC_LDROM_BASE, FMC_LDROM_BASE + FMC_LDROM_SIZE, TEST_PATTERN) < 0)
    {
        printf("\n\nLDROM test failed!\n");
        goto lexit;
    }
    FMC_DISABLE_LD_UPDATE();
#endif
    printf("\n\nAPROM test =>\n");
    FMC_ENABLE_AP_UPDATE();
    if(FlashTest(APROM_TEST_BASE, APROM_TEST_END, TEST_PATTERN) < 0)
    {
        printf("\n\nAPROM test failed!\n");
        goto lexit;
    }
    FMC_DISABLE_AP_UPDATE();

lexit:
    // while (1){}

}

void TPF_test1(const char *title)
{
    uint32_t u32Addr;
    printf("Testing Flash1...(%s)\n", title);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function */
    FMC_Open();

    printf("\n\nAPROM test =>\n");
    FMC_ENABLE_AP_UPDATE();
    u32Addr = 0x20000;
    if(FMC_Erase( u32Addr ) != 0) {
        printf("FMC_Erase address 0x%x failed!\n", u32Addr);
        return -1;
    }
    FMC_DISABLE_AP_UPDATE();

    printf("Flash Test1 Passed.\n");

lexit:

}
