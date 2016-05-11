#include "includes.h"
#include "api.h"
#include <stdint.h>

static vu32 TimingDelay = 0; 
vu32 TimeDisplay = 0;

uc8 Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M

void delayms(int n)
{
    int i,j;

    for(i=0;i<100;i++)
    {
        for(j=0;j<n;j++)
        {
        }
    }
}
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
void delayms(int n)
{
    int i,j;

    for(i=0;i<300;i++)
    {
        for(j=0;j<n;j++)
        {
        }
    }
}

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
void delayms(int n)
{
    int i,j;

    for(i=0;i<600;i++)
    {
        for(j=0;j<n;j++)
        {
        }
    }
}

#endif

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Initialize system clocks
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_RCC_Init(void)
{
	ErrorStatus HSEStartUpStatus;
  	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);

  	/* Wait till HSE is ready */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();

  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* Enable Prefetch Buffer */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    	/* Flash 2 wait state */
    	FLASH_SetLatency(FLASH_Latency_2);
 
    	/* HCLK = SYSCLK */ 
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    	/* PCLK2 = HCLK */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 

    	/* PCLK1 = HCLK/2 */
    	/*����APB1��ʱ��Ƶ�ʣ�����TIMER 2-7��ʱ��Ƶ��*/
    	RCC_PCLK1Config(RCC_HCLK_Div2);

				/*����ADCʱ��Ƶ�ʣ�Ϊ��ʹת�����׼ȷ��һ��ADCʱ��Ƶ�ʲ�����14M*/
#if	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
				
				RCC_ADCCLKConfig(RCC_PCLK2_Div2);
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
				/*ADCʱ��Ƶ��Ϊ12M*/
				RCC_ADCCLKConfig(RCC_PCLK2_Div6);
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
				/*ADCʱ��Ƶ��Ϊ12M*/
				RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#endif

		/* PLLCLK = 8MHz * 2 = 16 MHz */
				/* 
				PLL������ʱ�� = HSEʱ��Ƶ�ʣ�
				���� PLL ʱ��Դ����Ƶϵ��  
				RCC_PLLSource ����	
				RCC_PLLSource_HSI_Div2	PLL������ʱ�� = HSIʱ��Ƶ�ʳ���2  
				RCC_PLLSource_HSE_Div1	PLL������ʱ�� = HSE ʱ��Ƶ��  
				RCC_PLLSource_HSE_Div2	PLL������ʱ�� = HSE ʱ��Ƶ�ʳ���2  
				
				RCC_PLLMul	����  
				RCC_PLLMul_2 PLL����ʱ�� x 2  
				RCC_PLLMul_3 PLL����ʱ�� x 3  
				...
				RCC_PLLMul_15 PLL����ʱ�� x 15 
				 RCC_PLLMul_16 PLL����ʱ�� x 16  
				 ���棺������ȷ���������  ʹPLL���ʱ��Ƶ�ʲ�����72 MHz,��HSI��������ΪPLLʱ�ӵ�����ʱ,ϵͳʱ�ӵ����Ƶ�ʲ��ó���64MHz�� 
				 */
		
#if	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2);
		
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		//Ŀǰʹ���ⲿ���� 20M��ʹ��3��Ƶ�����ʱ��Ƶ��Ϊ20*3=60MHZ
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);
		//RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_24M
		
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);
		
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
#endif

    	/* Enable PLL */
    	RCC_PLLCmd(ENABLE);

    	/* Wait till PLL is ready */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    	{
    	}

    	/* Select PLL as system clock source */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    	/* Wait till PLL is used as system clock source */
    	while(RCC_GetSYSCLKSource() != 0x08)
    	{
    	}
  	}

}


/*******************************************************************************
* Function Name  : NVIC_Init
* Description    : Initialize the NVIC register
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_NVIC_Init(void)
{
//#if UPGRADE_DOME_FUNCTION
//          /* Set the Vector Table base address at 0x08000000 */
//  //  InterruptConfig();
//  /* Deinitializes the NVIC */
//  NVIC_DeInit();
//
//  /* Set the Vector Table base address at 0x08000000 */
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECTOR_TABLE_OFFSET);
//
//#else
//
//
//	#ifdef  VECT_TAB_RAM  
//  		/* Set the Vector Table base location at 0x20000000 */
//  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
//	#else  /* VECT_TAB_FLASH */
//  		/* Set the Vector Table base location at 0x08000000 */
//  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
//	#endif
//#endif
	//Configure one bit for preemption priority 
  	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);


//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  

}

/*******************************************************************************
* Function Name  : SysTick_API_Init
* Description    : Configure a SysTick Base time to 10 ms.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SysTick_Init(void)
{

}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_Init(void)
{
	BSP_RCC_Init();
	BSP_NVIC_Init();
	BSP_SysTick_Init();	
}



/* Initialize the USART1----------------------------------------------*/
void BSP_USART1_Init(void)
{
  	GPIO_InitTypeDef GPIOA_InitStructure;
//	USART_InitTypeDef USART_1_InitStructure;
//	USART_ClockInitTypeDef USART_1_ClockInitStructure;
	NVIC_InitTypeDef NVIC_USART1_InitStructure;
	
	/* open the USART1 RCC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* open the USART1 intterrupt service */
	NVIC_USART1_InitStructure.NVIC_IRQChannel = USART1_IRQn;
				
	NVIC_USART1_InitStructure.NVIC_IRQChannelSubPriority = 0;//7;
	NVIC_USART1_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_USART1_InitStructure);	

	/* configure the rs485 control pin of the receive or send */
	GPIOA_InitStructure.GPIO_Pin = RS485_RX_TX_CTL_PIN;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_RX_TX_CTL_PORT, &GPIOA_InitStructure);

  	/* Configure USART1 Tx (PA9) as alternate function push-pull */ 
  	GPIOA_InitStructure.GPIO_Pin = RS485TXD1;
  	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIOA_InitStructure);
    
  	/* Configure USART1 Rx (PA10) as input floating */ 
  	GPIOA_InitStructure.GPIO_Pin = RS485RXD1;
  	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	/*����RS485Ϊ����ģʽ*/
	GPIO_ResetBits(RS485_RX_TX_CTL_PORT,RS485_RX_TX_CTL_PIN);
}




/* Initialize the USART2----------------------------------------------*/
void BSP_USART2_Init(void)
{
    GPIO_InitTypeDef GPIOA_InitStructure;
//	USART_InitTypeDef USART_2_InitStructure;
//	USART_ClockInitTypeDef USART_2_ClockInitStructure;
	NVIC_InitTypeDef NVIC_USART2_InitStructure;
//	INT16U nCount;

	/* open the USART2 RCC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
	BSP_DelayMS(10);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* open the USART2 intterrupt service */
	NVIC_USART2_InitStructure.NVIC_IRQChannel = USART2_IRQn;
					
	NVIC_USART2_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_USART2_InitStructure.NVIC_IRQChannelSubPriority = 7;//8;
	NVIC_USART2_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_USART2_InitStructure);

  	/* Configure USART2 Tx (PA2) as alternate function push-pull */ 
  	GPIOA_InitStructure.GPIO_Pin = TTLTXD2;
  	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIOA_InitStructure);
    
  	/* Configure USART2 Rx (PA3) as input floating */ 
  	GPIOA_InitStructure.GPIO_Pin = TTLRXD2;
  	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIOA_InitStructure);

}





/* Initialize the Motor-----------------------------------------------*/
void BSP_Motor_Init(void)
{}


/*	��翪�س�ʼ��*/
void BSP_PhotoSwitch_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = 1;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIOD_InitStructure);	

}

void BSP_DAC1_Init(void)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;

	/* Enable peripheral clocks --------------------------------------------------*/
//	/* DMA clock enable */
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	/* AFIO and GPIOA Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
//	/* TIM6 Periph clock enable */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* Configure DAC channe1 output pin */
	GPIOA_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ������ģʽ��Ŀ���ǽ��͹���
	GPIO_Init(GPIOA, &GPIOA_InitStructure);
	
//	/* TIM6 Configuration */
//	TIM_PrescalerConfig(TIM6, 0xF, TIM_PSCReloadMode_Update);
//	TIM_SetAutoreload(TIM6, 0xFF);
//	/* TIM6 TRGO selection */
//	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	/* DAC channel1 Configuration */
//	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
//	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	
//	/* DMA2 channel3 configuration */
//	DMA_DeInit(DMA2_Channel3);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&Escalator8bit;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = 6;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	
//	/* Enable DMA2 Channel3 */
//	DMA_Cmd(DMA2_Channel3, ENABLE);
	
	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	DAC_SetChannel1Data(DAC_Align_12b_R, (u16)((0.51/3.30)*4095));
//
//	DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
	
//	/* Enable DMA for DAC Channel1 */
//	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
//	/* TIM6 enable counter */
//	TIM_Cmd(TIM6, ENABLE);
}

void BSP_DAC2_Init(void)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;

	/* Enable peripheral clocks --------------------------------------------------*/
//	/* DMA clock enable */
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	/* AFIO and GPIOA Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
//	/* TIM7 Periph clock enable */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	/* Configure DAC channe1 output pin */
	GPIOA_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);
	
//	/* TIM7 Configuration */
//	TIM_PrescalerConfig(TIM7, 0xF, TIM_PSCReloadMode_Update);
//	TIM_SetAutoreload(TIM7, 0xFF);
//	/* TIM6 TRGO selection */
//	TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
	
	/* DAC channel1 Configuration */
//	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
//	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	
//	/* DMA2 channel4 configuration */
//	DMA_DeInit(DMA2_Channel4);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&Escalator8bit;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = 6;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA2_Channel4, &DMA_InitStructure);
	
//	/* Enable DMA2 Channel4 */
//	DMA_Cmd(DMA2_Channel4, ENABLE);
	
	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_2, ENABLE);

		DAC_SetChannel2Data(DAC_Align_12b_R, (u16)((0.30/3.30)*4095));
//
//	DAC_SoftwareTriggerCmd(DAC_Channel_2, ENABLE);
	
//	/* Enable DMA for DAC Channel1 */
//	DAC_DMACmd(DAC_Channel_2, ENABLE);
//	
//	/* TIM7 enable counter */
//	TIM_Cmd(TIM7, ENABLE);
}



void BSP_I2C2_Init(void)
{

}

/* Initialize the OSD to control the display on the camera------------*/
void BSP_OSD_Init(void)
{
	//BSP_I2C1_Init();
	GPIO_InitTypeDef GPIOB_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOB_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_8);  	
}




void BSP_I2C1_Init(void)
{
	
}



/*
 * Function:       
 * Description:    ���ϵͳ��ʱ����ʱ��2���ж�����Ϊ2ms,
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer2_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM2_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM2_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM2_OCInitStructure;
		
  	/* TIM2 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Enable the TIM2 gloabal Interrupt */
	NVIC_TIM2_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
#if TIMER_INTERRUPT_OPTIMIZE == 1
		
		NVIC_TIM2_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_TIM2_InitStructure.NVIC_IRQChannelSubPriority = 4;
#else
		NVIC_TIM2_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_TIM2_InitStructure.NVIC_IRQChannelSubPriority = 2;
	
#endif
	NVIC_TIM2_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM2_InitStructure);

	TIM_DeInit(TIM2);
		
	/* Time base configuration  2ms */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM2_TimeBaseStructure.TIM_Period = 500;
		TIM2_TimeBaseStructure.TIM_Prescaler = 287;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM2_TimeBaseStructure.TIM_Period = 500;
		TIM2_TimeBaseStructure.TIM_Prescaler = 63;
#elif	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM2_TimeBaseStructure.TIM_Period = 500;
		TIM2_TimeBaseStructure.TIM_Prescaler = 191;
#endif
	TIM2_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM2_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM2_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}

#if MECHANICAL_VERSION == M_QHIR_MINI_QH


/*
 * Function:       ��ʱ��ʱ��Ƶ��Ϊ72MHZ(δ��Ƶ����£�����ʹ�÷�Ƶ)
 * Description:   ��ֱ�� �ö�ʱ���ж�����������ٶ�Ӱ����ı䣬��ʱ��ʱ��Ƶ��Ϊ50KHZ������ж�����Ϊ20us
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer3_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM3_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM3_OCInitStructure;
		
  	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_TIM3_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	
	NVIC_TIM3_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM3_InitStructure.NVIC_IRQChannelSubPriority = 3;
#else
	NVIC_TIM3_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM3_InitStructure.NVIC_IRQChannelSubPriority = 2;

#endif
	NVIC_TIM3_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM3_InitStructure);

	TIM_DeInit(TIM3);
		
	/* Time base configuration  1us */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
	
		TIM3_TimeBaseStructure.TIM_Period = 10;
		TIM3_TimeBaseStructure.TIM_Prescaler = 999;//1439;//T = 20us

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM3_TimeBaseStructure.TIM_Period = 10;
		TIM3_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM3_TimeBaseStructure.TIM_Period = 10;
		TIM3_TimeBaseStructure.TIM_Prescaler = 191;
	
#endif
	TIM3_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM3, ENABLE);
}

#else
/*
 * Function:       ��ʱ��ʱ��Ƶ��Ϊ72MHZ(δ��Ƶ����£�����ʹ�÷�Ƶ)
 * Description:   ��ֱ�� �ö�ʱ���ж�����������ٶ�Ӱ����ı䣬��ʱ��ʱ��Ƶ��Ϊ250KHZ������ж�����Ϊ4us
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer3_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM3_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM3_OCInitStructure;
		
  	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_TIM3_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	
	NVIC_TIM3_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM3_InitStructure.NVIC_IRQChannelSubPriority = 3;
#else
	NVIC_TIM3_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM3_InitStructure.NVIC_IRQChannelSubPriority = 2;

#endif
	NVIC_TIM3_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM3_InitStructure);

	TIM_DeInit(TIM3);
		
	/* Time base configuration  1us */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
	
		TIM3_TimeBaseStructure.TIM_Period = 10;
#if (MECHANICAL_VERSION == M_QHSP4_QH)
		TIM3_TimeBaseStructure.TIM_Prescaler = 719;//599;
#elif (MECHANICAL_VERSION == M_QHIR_LIANC)
		TIM3_TimeBaseStructure.TIM_Prescaler = 719;//899;//1199;//599;

#elif (MECHANICAL_VERSION == M_QHSP6_QH)
    TIM3_TimeBaseStructure.TIM_Prescaler = 287;//449;
#else
        TIM3_TimeBaseStructure.TIM_Prescaler = 287;
#endif

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM3_TimeBaseStructure.TIM_Period = 10;
		TIM3_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM3_TimeBaseStructure.TIM_Period = 10;
		TIM3_TimeBaseStructure.TIM_Prescaler = 191;
	
#endif
	TIM3_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM3, ENABLE);
}
#endif



/*
 * Function:       
 * Description:    ��ѧϰ���ö�ʱ���ж�����Ϊ16MS����ʱ��ʱ��Ƶ��Ϊ250HZ
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */

void BSP_Timer4_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM4_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM4_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM4_OCInitStructure;
		
  	/* TIM4 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Enable the TIM4 gloabal Interrupt */
	NVIC_TIM4_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
#if TIMER_INTERRUPT_OPTIMIZE == 1
			
			NVIC_TIM4_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_TIM4_InitStructure.NVIC_IRQChannelSubPriority = 6;
#else
			NVIC_TIM4_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
			NVIC_TIM4_InitStructure.NVIC_IRQChannelSubPriority = 2;
		
#endif
	NVIC_TIM4_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM4_InitStructure);

	TIM_DeInit(TIM4);
		
	/* Time base configuration  16us */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM4_TimeBaseStructure.TIM_Period = 4;
		TIM4_TimeBaseStructure.TIM_Prescaler = 287;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM4_TimeBaseStructure.TIM_Period = 4;
		TIM4_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM4_TimeBaseStructure.TIM_Period = 4;
		TIM4_TimeBaseStructure.TIM_Prescaler = 191;
#endif
	TIM4_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM4_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM4_TimeBaseStructure);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM4, ENABLE);
}


#if (MECHANICAL_VERSION == M_QHSP4_QH)
/*
 * Function:       
 * Description:  ˮƽ���ö�ʱ���ж�����������ٶ�Ӱ����ı䣬��ʱ��ʱ��Ƶ��Ϊ500KHZ  
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer5_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM5_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM5_OCInitStructure;
		
  	/* TIM5 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable the TIM5 gloabal Interrupt */
	NVIC_TIM5_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 2;

#else
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 0;

#endif

	NVIC_TIM5_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM5_InitStructure);

	TIM_DeInit(TIM5);
		
	/* Time base configuration  2ms */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 467;//575;//287
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 191;
#endif

	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM5_TimeBaseStructure);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM5, ENABLE);
}
#elif MECHANICAL_VERSION==M_QHIR_MINI_QH
/*
 * Function:       
 * Description:  ˮƽ���ö�ʱ���ж�����������ٶ�Ӱ����ı䣬��ʱ��ʱ��Ƶ��Ϊ500KHZ  
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer5_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM5_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM5_OCInitStructure;
		
  	/* TIM5 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable the TIM5 gloabal Interrupt */
	NVIC_TIM5_InitStructure.NVIC_IRQChannel = TIM5_IRQChannel;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 2;

#else
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 0;

#endif

	NVIC_TIM5_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM5_InitStructure);

	TIM_DeInit(TIM5);
		
	/* Time base configuration  2ms */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 575;//287
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 191;
#endif

	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM5_TimeBaseStructure);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM5, ENABLE);
}

#elif (MECHANICAL_VERSION == M_QHIR_LIANC)
void BSP_Timer5_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM5_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM5_OCInitStructure;
		
  	/* TIM5 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable the TIM5 gloabal Interrupt */
	NVIC_TIM5_InitStructure.NVIC_IRQChannel = TIM5_IRQChannel;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 2;

#else
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 0;

#endif

	NVIC_TIM5_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM5_InitStructure);

	TIM_DeInit(TIM5);
		
	/* Time base configuration  2ms */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 575;//287
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 191;
#endif

	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM5_TimeBaseStructure);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM5, ENABLE);
}

#else
/*
 * Function:       
 * Description:  ˮƽ���ö�ʱ���ж�����������ٶ�Ӱ����ı䣬��ʱ��ʱ��Ƶ��Ϊ250KHZ  
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer5_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM5_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM5_OCInitStructure;
		
  	/* TIM5 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable the TIM5 gloabal Interrupt */
	NVIC_TIM5_InitStructure.NVIC_IRQChannel = TIM5_IRQChannel;
#if TIMER_INTERRUPT_OPTIMIZE == 1
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 2;

#else
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 0;

#endif

	NVIC_TIM5_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM5_InitStructure);

	TIM_DeInit(TIM5);
		
	/* Time base configuration  2ms */
#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 287;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 63;
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
		TIM5_TimeBaseStructure.TIM_Period = 10;
		TIM5_TimeBaseStructure.TIM_Prescaler = 191;
#endif

	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM5_TimeBaseStructure);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM5, ENABLE);
}
#endif


/*
 * Function:       BSP_Timer6_Init
 * Description:    ���ڴ�����⹦������¼�������3S��ȡһ��ADCֵ(������ǿ��),ÿ��400ms����һ�κ��
 *					��ʱ��6ÿ100MS�ж�һ�� 
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
void BSP_Timer6_Init(void)
{
	NVIC_InitTypeDef NVIC_TIM5_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
		
  	/* TIM5 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* Enable the TIM5 gloabal Interrupt */
	NVIC_TIM5_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
#if TIMER_INTERRUPT_OPTIMIZE == 1
				
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 3;
#else
	NVIC_TIM5_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM5_InitStructure.NVIC_IRQChannelSubPriority = 2;

#endif
	NVIC_TIM5_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_TIM5_InitStructure);

	TIM_DeInit(TIM6);
		
	/* ��ʱ��6ÿ100MS�ж�һ�� */
#if	SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
	TIM5_TimeBaseStructure.TIM_Period = TIMER6_PERIOD_VALUE;
	TIM5_TimeBaseStructure.TIM_Prescaler = TIMER6_PRESCALER_DATA;
	
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
	TIM5_TimeBaseStructure.TIM_Period = 160;
	TIM5_TimeBaseStructure.TIM_Prescaler = 9999;//TIMER6_PRESCALER_DATA;
	
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
		TIM5_TimeBaseStructure.TIM_Period = TIMER6_PERIOD_VALUE;
		TIM5_TimeBaseStructure.TIM_Prescaler = TIMER6_PRESCALER_DATA;//TIMER6_PRESCALER_DATA;

#endif
	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM5_TimeBaseStructure);

	TIM_ClearFlag(TIM6, TIM_FLAG_Update);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

//    TIM_Cmd(TIM5, ENABLE);
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length (time base 10 ms).
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nCount)
{

}

/*******************************************************************************
* Function Name  : Decrement_TimingDelay
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void Decrement_TimingDelay(void)
{
  	if (TimingDelay != 0x00)
  	{
    	TimingDelay--;
  	}
}

void BSP_DelayTime(INT32U nCount)
{
	while(nCount--);
}
void BSP_DelayMS(INT32U nCount)
{
 	uint i;
	while(nCount) 
	{
		for(i=2670; i>0; i--);
		nCount--;
	}	
}



/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
void APP_Init(void)
{
	BSP_Init();
}


void APP_DelayTime(INT32U nCount)
{
	while(nCount--);
}

void APP_USART1_Baudrate(u32 baudrate)
{
	USART_InitTypeDef USART_1_InitStructure;
//	INT16U BaudrateTemp;

	USART_ClockInitTypeDef  USART_ClockInitStructure;
//	USART_InitTypeDef USART_InitStructure;
	/* Configure the USART1 synchronous paramters */
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART1, &USART_ClockInitStructure);

//	USART_ClockInitTypeDef USART_1_ClockInitStructure;
	/* USART1 configuration */
  	/*USART1 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  	*/
//	BaudrateTemp = baudrate;
  	USART_1_InitStructure.USART_BaudRate = baudrate;
  	USART_1_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_1_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_1_InitStructure.USART_Parity = USART_Parity_No;
  	USART_1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_1_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//ʹ�ܷ��ͺͽ���
	USART_Init(USART1, &USART_1_InitStructure);
	
}


#if 1
void APP_USART1_IRQHandler(void)
{
	INT08U udr0Temp = 0;   

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//�ж��ǲ��ǽ����ж�
	{
		if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
		{	           
			udr0Temp = ((u16)(USART1->DR & (u16)0x01FF)) & 0xFF;
			//uart0_rx_isr(udr0Temp);
		}
	}
}

#else
void APP_USART1_IRQHandler(void)
{
	INT08U udr0Temp = 0;   
	/* if there is data come from the USART1 */
//    if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET) 
//	{	           
//		udr0Temp = USART_ReceiveData(USART1) & 0xFF;
//		uart0_rx_isr(udr0Temp);
////		return;  					
////    } else {
////		return; 
//	}

//    if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET)
	 if ((USART1->SR & USART_IT_RXNE) != (u16)RESET)
	{	           
//		udr0Temp = USART_ReceiveData(USART1) & 0xFF;
		udr0Temp = ((u16)(USART1->DR & (u16)0x01FF)) & 0xFF;
		uart0_rx_isr(udr0Temp);
//		return;  					
//    } else {
//		return; 
	}
	
		
}
#endif


void APP_USART2_Baudrate(INT16U baudrate)
{
	USART_InitTypeDef USART_2_InitStructure;
	INT16U BaudrateTemp;
//	USART_ClockInitTypeDef USART_2_ClockInitStructure;
	/* USART2 configuration */
  	/*USART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  	*/
	BaudrateTemp = baudrate;
  	USART_2_InitStructure.USART_BaudRate = BaudrateTemp;
  	USART_2_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_2_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_2_InitStructure.USART_Parity = USART_Parity_No;
  	USART_2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_2_InitStructure);
		
}

#if 0
void APP_USART2_IRQHandler(void)
{
	INT08U udr1Temp = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//�ж��ǲ��ǽ����ж�
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) 
		//if ((USART2->SR & USART_IT_RXNE) != (u16)RESET)
		{	
			udr1Temp = USART_ReceiveData(USART2) & 0xFF;
			serial_int1_receive(udr1Temp);
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) == SET) 
	{ 	
		if(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET) 
			serial_int1_send();			
    } 
	
	return;		
}
#else
void APP_USART2_IRQHandler(void)
{
	INT08U udr1Temp = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//�ж��ǲ��ǽ����ж�
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) 
		//if ((USART2->SR & USART_IT_RXNE) != (u16)RESET)
		{	
			udr1Temp = USART_ReceiveData(USART2) & 0xFF;
			serial_int1_receive(udr1Temp);
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) == SET) 
	{ 	
		if(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET) 
			serial_int1_send();			
    } 
	
	return;		
}

#endif


void APP_TIM2_IRQHandler(void)
{

	if (((TIM2->SR & TIM_IT_Update) != (u16)RESET) && ((TIM2->DIER & TIM_IT_Update) != (u16)RESET))
	{		
		TIM2->DIER &= (u16)~TIM_IT_Update;
	   	timer0_int();
		TIM2->DIER |= TIM_IT_Update;
		TIM2->SR = (u16)~TIM_IT_Update;
	}
   		
}


void APP_TIM6_IRQHandler(void)
{


}


#if APP_MOTOR_CHANGE_SPEED_MACRO_ENABLE==0

void APP_Motor_ChangeTiltSpeed(INT16U time)
{
  TIM3->ARR = time ;

  /* Generate an update event to reload the Prescaler value immediatly */
  TIM3->EGR = ((u16)0x0001);
    
//  if (((*(u32*)&TIM3) == TIM1_BASE) || ((*(u32*)&TIM3) == TIM8_BASE))  
//  {
//    /* Set the Repetition Counter value */
//    TIM3->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
//  } 
}
#endif

void APP_TIM4_IRQHandler(void)
{
//	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//	{	
//		TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
//	   	timer2_ovf_isr();
//		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//	}

//	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	if (((TIM4->SR & TIM_IT_Update) != (u16)RESET) && ((TIM4->DIER & TIM_IT_Update) != (u16)RESET))
	{	
//		TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
		TIM4->DIER &= (u16)~TIM_IT_Update;
	   	//timer2_ovf_isr();
//		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
		TIM4->DIER |= TIM_IT_Update;
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		TIM4->SR = (u16)~TIM_IT_Update;
	}


}



#if APP_MOTOR_CHANGE_SPEED_MACRO_ENABLE==0

 void APP_Motor_ChangePanSpeed(INT16U time)
{
//	 TIM_TimeBaseInitTypeDef  TIM5_TimeBaseStructure;
//	/* Time base configuration  2ms */
//	TIM5_TimeBaseStructure.TIM_Period = time;
////	TIM5_TimeBaseStructure.TIM_Prescaler = 71;
//	TIM5_TimeBaseStructure.TIM_Prescaler = 63;
//	TIM5_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM5_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM5, &TIM5_TimeBaseStructure);


//	  /* Set the Autoreload value */
//  		TIM5->ARR = time ;

		  /* Select the Counter Mode and set the clock division */
//  TIM5->CR1 &= ((u16)0x00FF) & ((u16)0x038F);
//  TIM5->CR1 |= 0 | ((u16)0x0000);
  /* Set the Autoreload value */
  TIM5->ARR = time ;

//  /* Set the Prescaler value */
//  TIM5->PSC = 63;
																			   
  /* Generate an update event to reload the Prescaler value immediatly */
  TIM5->EGR = ((u16)0x0001);
    
//  if (((*(u32*)&TIM5) == TIM1_BASE) || ((*(u32*)&TIM5) == TIM8_BASE))  
//  {
//    /* Set the Repetition Counter value */
//    TIM5->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
//  }  
}
#endif


#if	DAC_NOT_INTEGER
/* 0V <= voltage <=3.3V */
void APP_DAC_Pan_SetRefVoltage(FP32 voltage)
{
//	FP32 VoltageTemp;
	INT16U VoltageTemp;
	
	DAC_Cmd(DAC_Channel_1, DISABLE);
	VoltageTemp = voltage * 4095 / 3.30;

	*((vu32 *)(DAC_BASE + DHR12R1_Offset + DAC_Align_12b_R)) = (u32)VoltageTemp;//DAC_SetChannel1Data(DAC_Align_12b_R,VoltageTemp);

	DAC->SWTRIGR |= SWTRIGR_SWTRIG_Set << (DAC_Channel_1 >> 4);//DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_1, ENABLE);	

//	BSP_DelayTime(2000);
}

/* 0V <= voltage <=3.3V */
void APP_DAC_Tilt_SetRefVoltage(FP32 voltage)
{
	INT16U VoltageTemp;

	VoltageTemp = voltage * 4095 / 3.30;
//  	DAC_SetChannel2Data(DAC_Align_12b_R,1203);
	DAC_SetChannel2Data(DAC_Align_12b_R,VoltageTemp);
//	DAC_SetChannel2Data(DAC_Align_8b_R,VoltageTemp);

	DAC_SoftwareTriggerCmd(DAC_Channel_2, ENABLE);
//		DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
//	BSP_DelayTime(2000);
}

#else

#define	__DAC_Cmd_Enable(DAC_Channel)		(DAC->CR |= CR_EN_Set << DAC_Channel)
#define	__DAC_Cmd_Disable(DAC_Channel)		(DAC->CR &= ~(CR_EN_Set << DAC_Channel))



void APP_DAC_Pan_SetRefVoltage(short int voltage)
{
//	FP32 VoltageTemp;
	INT16U VoltageTemp;
	
	//__DAC_Cmd_Disable(DAC_Channel_1);//DAC_Cmd(DAC_Channel_1, DISABLE);
	VoltageTemp = voltage * 4095 / 3300;

	DAC_SetChannel1Data(DAC_Align_12b_R,VoltageTemp);

	DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
	//__DAC_Cmd_Enable(DAC_Channel_1);//DAC_Cmd(DAC_Channel_1, ENABLE);	

//	BSP_DelayTime(2000);
}

/* 0mv <= voltage <=3300mV */
void APP_DAC_Tilt_SetRefVoltage(short int voltage)
{
	INT16U VoltageTemp;

	VoltageTemp = voltage * 4095 / 3300 ;
	DAC_SetChannel2Data(DAC_Align_12b_R,VoltageTemp);
	//DAC_SetChannel1Data(DAC_Align_12b_R,VoltageTemp);

	DAC_SoftwareTriggerCmd(DAC_Channel_2, ENABLE);
	//DAC->SWTRIGR |= SWTRIGR_SWTRIG_Set << (DAC_Channel_2 >> 4);//DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
}

#endif





#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M

void delay_half_us(void)//0.37us
{
// asm("nop");
// asm("nop");
// 	uchar time;
//	time = 6;
//	while(time--);
}
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
void delay_half_us(void)//0.37us
{
 	uchar time=3;
	while(time--)
	{
		
	}
}

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
void delay_half_us(void)//0.37us
{
 	uchar time=6;
	while(time--)
	{
		
	}
}


#endif


void delay_2us(uchar time)//2us
{
 while(time--)
     {
      //delay_half_us();
      //delay_half_us();
	  //delay_half_us();
	  delay_half_us();
	  delay_half_us();
	 }
}

void delay_half_ms(uchar time)//0.5 ms delay   
{
	int i;
	for(i=0;i<time;i++)
		delay_2us(1);
}






void delay_half_second(uchar time)//
{
 uchar i;
 while(time--)
	 {
	  i = 5;
	  while(i--)
	      delay_half_ms(200);
	 }
}


//---------------------------������--------------------
/* SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_Pos              16                                             /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFul << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_Pos          16                                             /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_Msk          (0xFFFFul << SCB_AIRCR_VECTKEYSTAT_Pos)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANESS_Pos            15                                             /*!< SCB AIRCR: ENDIANESS Position */
#define SCB_AIRCR_ENDIANESS_Msk            (1ul << SCB_AIRCR_ENDIANESS_Pos)               /*!< SCB AIRCR: ENDIANESS Mask */

#define SCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7ul << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */

#define SCB_AIRCR_SYSRESETREQ_Pos           2                                             /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_Msk          (1ul << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_Pos         1                                             /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_Msk        (1ul << SCB_AIRCR_VECTCLRACTIVE_Pos)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

#define SCB_AIRCR_VECTRESET_Pos             0                                             /*!< SCB AIRCR: VECTRESET Position */
#define SCB_AIRCR_VECTRESET_Msk            (1ul << SCB_AIRCR_VECTRESET_Pos)               /*!< SCB AIRCR: VECTRESET Mask */

#define __DSB()                           __dsb(0)




void Dome_SoftReset(void)
{
	__set_FAULTMASK(1);      
	NVIC_SystemReset();

}

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/




