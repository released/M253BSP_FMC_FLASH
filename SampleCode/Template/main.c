/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "misc_config.h"
#include "DataFlashProg.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_FLASH_WRITE                   		(flag_PROJ_CTL.bit1)
#define FLAG_PROJ_FLASH_READ                 			(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_DATA_CREATE                           (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

uint32_t File_Data_Buf[FMC_FLASH_PAGE_SIZE / 4] = {0};
uint32_t read_Data_Buf[FMC_FLASH_PAGE_SIZE / 4] = {0};


#define STORAGE_DATA_BUF   ((uint32_t)&File_Data_Buf[0])
/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

// void delay_ms(uint16_t ms)
// {
// 	TIMER_Delay(TIMER0, 1000*ms);
// }


void compare_u32buffer(unsigned int *src, unsigned int *des, unsigned int nBytes)
{
    static uint8_t FLAG_MISC_ERROR = 0;
    unsigned int  i = 0;	
	
    #if 1
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			FLAG_MISC_ERROR = 1;//set_flag(flag_error , ENABLE);
        }
    }

	if (!FLAG_MISC_ERROR)//(!is_flag_set(flag_error))
	{
    	printf("%s complete \r\n" , __FUNCTION__);	
	}

    printf("%s end \r\n" , __FUNCTION__);
	FLAG_MISC_ERROR = 0;//set_flag(flag_error , DISABLE);	    
    #else
    if (memcmp(src, des, nBytes))
    {
        printf("\nMismatch!! - %d\n", nBytes);
        for (i = 0; i < nBytes; i++)
            printf("0x%02x    0x%02x\n", src[i], des[i]);
        return -1;
    }
    #endif

}

void dump_u32buffer(unsigned int *pucBuff, int nBytes)
{
    unsigned int  i = 0;
    
    printf("dump_buffer : %2d\r\n" , nBytes);    
    for (i = 0 ; i < nBytes ; i++)
    {
        printf("0x%2X," , pucBuff[i]);
        if ((i+1)%8 ==0)
        {
            printf("\r\n");
        }            
    }
    printf("\r\n");
}

void data_read(uint32_t addr)
{
    uint32_t i = 0;

    printf("%s start\r\n",__FUNCTION__);

    DataFlashRead(0x00000000  , BUFFER_PAGE_SIZE, &read_Data_Buf[0]);
    dump_u32buffer(read_Data_Buf , 128);
    compare_u32buffer(File_Data_Buf , read_Data_Buf , 128);

    printf("%s end\r\n\r\n",__FUNCTION__);
}

void data_write(void)
{
    uint32_t i = 0;

    printf("%s start\r\n",__FUNCTION__);

    DataFlashWrite(0x00000000 , BUFFER_PAGE_SIZE, STORAGE_DATA_BUF);

    printf("%s end\r\n\r\n",__FUNCTION__);
}

void data_creation(void)
{
    uint32_t i = 0;
    static uint16_t counter = 0x10;

    printf("%s start\r\n",__FUNCTION__);    

    /*
        one page = 0x200 , 512 BYTES
        one uint32_t container = 4 word , array size = 512 / 4 = 128 = 0x80

        size 32 in uint32_t container = 32 * 4 = 128 BYTES = 32 words
    */

    for ( i = 0 ; i < 32 ; i++)
    {
        File_Data_Buf[i+32*0] = 0x5A5AAA00 + i + counter;
        File_Data_Buf[i+32*1] = 0x5A5ABB00 + i + counter;
        File_Data_Buf[i+32*2] = 0x5A5ACC00 + i + counter;
        File_Data_Buf[i+32*3] = 0x5A5ADD00 + i + counter;
    }

    dump_u32buffer(&File_Data_Buf[0] , 128);

    printf("%s end (0x%8X)\r\n\r\n",__FUNCTION__ , counter);   
    counter += 0x10; 
}

void FMC_Enable(void)
{
    /* Unlock protected registers for ISP function */
    SYS_UnlockReg();
    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;

    FMC_ENABLE_ISP();
    FMC_ENABLE_AP_UPDATE();

}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_1000MS = 1;//set_flag(flag_timer_period_1000ms ,ENABLE);
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

        // printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
        PB0 ^= 1;        
    }

    if (FLAG_PROJ_FLASH_WRITE)
    {
        FLAG_PROJ_FLASH_WRITE = 0;
        data_write();
    }

    if (FLAG_PROJ_FLASH_READ)
    {
        FLAG_PROJ_FLASH_READ = 0;
        data_read(FILE_DATA_ADDR);
    }

    if (FLAG_PROJ_DATA_CREATE)
    {
        FLAG_PROJ_DATA_CREATE = 0;
        data_creation();
    }
}

void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(UART4);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		printf("press : %c\r\n" , res);
		switch(res)
		{
			case '1':
                FLAG_PROJ_FLASH_WRITE = 1;
				break;
			case '2':
                FLAG_PROJ_FLASH_READ = 1;
				break;
			case '3':
                FLAG_PROJ_DATA_CREATE = 1;
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void UART4_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(UART4, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART4) == 0)
        {
			UARTx_Process();
        }
    }

    if(UART4->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART4, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART4_Init(void)
{
    SYS_ResetModule(UART4_RST);

    /* Configure UART4 and set UART4 baud rate */
    UART_Open(UART4, 115200);
    UART_EnableInt(UART4, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    NVIC_EnableIRQ(UART4_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	

    #if 0
    printf("FLAG_PROJ_TIMER_PERIOD_1000MS : 0x%2X\r\n",FLAG_PROJ_TIMER_PERIOD_1000MS);
    printf("FLAG_PROJ_REVERSE1 : 0x%2X\r\n",FLAG_PROJ_REVERSE1);
    printf("FLAG_PROJ_REVERSE2 : 0x%2X\r\n",FLAG_PROJ_REVERSE2);
    printf("FLAG_PROJ_REVERSE3 : 0x%2X\r\n",FLAG_PROJ_REVERSE3);
    printf("FLAG_PROJ_REVERSE4 : 0x%2X\r\n",FLAG_PROJ_REVERSE4);
    printf("FLAG_PROJ_REVERSE5 : 0x%2X\r\n",FLAG_PROJ_REVERSE5);
    printf("FLAG_PROJ_REVERSE6 : 0x%2X\r\n",FLAG_PROJ_REVERSE6);
    printf("FLAG_PROJ_REVERSE7 : 0x%2X\r\n",FLAG_PROJ_REVERSE7);
    #endif

}

void GPIO_Init (void)
{
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB0MFP_Msk)) | (SYS_GPB_MFPL_PB0MFP_GPIO);

    GPIO_SetMode(PB, BIT0, GPIO_MODE_OUTPUT);

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);
    
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

//    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(GPB_MODULE);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    /* Debug UART clock setting */
    UartDebugCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    UartDebugMFP();


    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M251 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART4 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    SYS_Init();

	GPIO_Init();
	UART4_Init();
	TIMER1_Init();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif

    FMC_Enable();
    data_creation();

    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
