#include "includes.h"
#include "stdlib.h"
#include    <math.h>

#include "Protocol.h"
#include "Queue.h"
#include "motorconfig.h"
#include "an41908a.h"


#include "bsp_spi_flash.h"

#define	PD_ZOOM_FOCUS_STOP		0
#define	PD_ZOOM_TELE_CMD		1
#define	PD_ZOOM_WIDE_CMD		2
#define	PD_FOCUS_FAR_CMD		3
#define	PD_FOCUS_NEAR_CMD		4



void timer0_int(void);
void timer0_initial(void);
void timer3_init(void);
u8 zf_key_handle(void);

u8 af_action_cnt = 0;
u8 af_action_onepush_cnt = 0;

#define CAMERA_POWER_PIN    GPIO_Pin_2
#define CAMERA_SET_KEY_PIN    GPIO_Pin_3

#define CAMERA_POWER_PORT    GPIOD
#define CAMERA_SET_KEY_PORT   GPIOB

void camera_power_key_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = CAMERA_SET_KEY_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAMERA_SET_KEY_PORT, &GPIOD_InitStructure);

	GPIOD_InitStructure.GPIO_Pin = CAMERA_POWER_PIN;
    GPIO_Init(CAMERA_POWER_PORT, &GPIOD_InitStructure);


	GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
	//GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	



	
	GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	



	PWR_BackupAccessCmd(ENABLE);//允许修改RTC 和后备寄存器
	RCC_LSEConfig(RCC_LSE_OFF);//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用。
	BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，也就是 PC13，也可以当普通IO 使用
	PWR_BackupAccessCmd(DISABLE);//禁止修改后备寄存器

	//RCC_LSICmd(DISABLE);
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_14);	
	GPIO_SetBits(GPIOC,GPIO_Pin_15);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);


	
	//GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_power_off_on(u8 mode)
{
    if(mode)//on
        GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
    else
        GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_sensor_format_set_with_long_key(void)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);

}

void camera_ntsc_pal_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_14);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_14);
}

void camera_hd_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void camera_cvbs_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_15);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_15);
}



void zoom_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = ZOOM_PIN_A1|ZOOM_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ZOOM_PORT, &GPIOD_InitStructure);	


    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

u8 iii = 2;

void focus_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = FOCUS_PIN_A1|FOCUS_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FOCUS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	


}


void iris_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_PIN_PLUS|IRIS_PIN_SUB;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	


}


void iris_auto_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_AUTO_PIN_A1|IRIS_AUTO_PIN_A2|IRIS_AUTO_PIN_B1|IRIS_AUTO_PIN_B2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_AUTO_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A2);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B2);	

//        md127_keepmode();
}



//设置镜头电机的驱动电流，电压
void motor_voltage_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = VOLTAGE_VL3|VOLTAGE_VL2|VOLTAGE_VL1;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOB,VOLTAGE_VL3);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL2);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL1);	
}

void camera_power_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_2);	

	

}

void camera_power_on_off(u8 mode)
{
    if(mode)
        GPIO_SetBits(GPIOC,GPIO_Pin_2);	
    else
        GPIO_ResetBits(GPIOC,GPIO_Pin_2);	   
}

void iris_ex_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_5);	

	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);	
}


void iris_ex_pin_set(u8 mode)
{
	if(mode > 1)
		return;

	    system_para.system_para.para_iris_ex_mode = mode;

	switch(mode)
	{
	case 1:
		GPIO_SetBits(GPIOC,GPIO_Pin_5);	
		GPIO_SetBits(GPIOB,GPIO_Pin_0);	
		break;
	case 0:
		GPIO_ResetBits(GPIOC,GPIO_Pin_5);	
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
		break;

	}

}

void extern_function_pin_Init(void)
{


	

}

void key_long_osd_emu(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_8);	
    GPIO_SetBits(GPIOB,GPIO_Pin_9);	
    delay_X1ms(4000);   
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}


void extern_function_pin_Init1(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    system_para.system_para.para_ex_io_1_mode = 1;   

    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
    

}

void key_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
		
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	


}


// recieve datas from keyboard
void uart0_init(void)
{
// UCSR0B = 0x00; //disable while setting baud rate
// UCSR0A = 0x00;
// UCSR0C = 0x06;
//-----------先关闭串口功能
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART1, DISABLE);


	BSP_USART1_Init();

	switch(Baud_rate)
	{
    case DOME_BAUDRATE_1200: 
				APP_USART1_Baudrate(1200);
			   break;    
	case DOME_BAUDRATE_2400: 
				APP_USART1_Baudrate(2400);
			   break;
	case DOME_BAUDRATE_4800: 
				APP_USART1_Baudrate(4800);
			   break;
	case DOME_BAUDRATE_9600: 
				APP_USART1_Baudrate(9600);
			   break;
	case DOME_BAUDRATE_14400:
		APP_USART1_Baudrate(14400);
	       break;
	case DOME_BAUDRATE_19200:
		APP_USART1_Baudrate(19200);
	       break;
	case DOME_BAUDRATE_115200:
		APP_USART1_Baudrate(115200);
	       break;	   
	default:   
		APP_USART1_Baudrate(2400);
	    break;
	}

	/* Enable USART1 receive intterrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  	/* Enable USART1 */
  	USART_Cmd(USART1, ENABLE);
	
	/* set the control pin at receive status */
	//GPIO_ResetBits(GPIOA, RS485Control);

	RS485_RX_ENABLE;
	//RS485_TX_ENABLE;

}

void serial_int1_send(void)	   //send data to USAR1		   
{
    if (cam_counter_send<cam_cmd_lenth)
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send++; 
    }
    else
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send=1;
    	txd1_buff_cFlag=1;
    	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
	if (cam_counter_send>=200)
		cam_counter_send=1; 
}

u8 cam_cmd_recieve_len = 0;
u8 cam_recieve_cnt_public=0;
void serial_int1_receive(uchar udr1)//receive data from USAR1
{
     
    rxd1_buffer[counter_receive] = udr1;

    if(counter_receive == 3)
    {
        cam_recieve_cnt_public=0;
        if(rxd1_buffer[2]==0x20)
        {
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;
        }
        else if(rxd1_buffer[2]==0x28)
        {
            cam_cmd_recieve_len = 7;
        }
        else
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;

		if(rxd1_buffer[2] == 0x10)
			cam_cmd_recieve_len--;
    }
    
   	counter_receive++;

   
    if (counter_receive >= cam_cmd_recieve_len && counter_receive>3)
   //if (counter_receive >= 6 && counter_receive>3)
   {
        cam_recieve_cnt_public = cam_cmd_recieve_len;
    	rxd1_buff_cFlag=1;
    	counter_receive=0;
        cam_cmd_recieve_len=0;
    }

    if (counter_receive >= 200)
    	counter_receive = 0;
}

// control the camera
void uart_to_camera_init(void)
{
    BSP_USART2_Init();
    APP_USART2_Baudrate(19200);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);// Enable USART2 receive intterrupt 
    USART_Cmd(USART2, ENABLE); // Enable USART2

}

//相机泪光片设置
void cam_filt_cutter_int(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	


    GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

}

void cam_filt_cutter_set(u8 mode)
{
    if(mode)
    {// fog
        GPIO_SetBits(GPIOB,GPIO_Pin_13);	

    }
    else
    {// ir

        GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    }

}

void ports_initial(void)
{
	BSP_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能


    //DC Motor driver ic: BA6208
   
	timer0_initial();
	

	SPI_FLASH_Init();
//spi_flash_init();
    an41908a_spi_pin_init();
	key_init();

	
}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;


	#ifdef  VECT_TAB_RAM  
  		/* Set the Vector Table base location at 0x20000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH */
  		/* Set the Vector Table base location at 0x08000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  	
		
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI15_10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  



  /* Enable the TIM1 UP Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line17 Interrupt */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

//////////////////////////////////////////////////////////////////
#if 1//CAMERA_HITACHI_SC110

//return: 1,success;0,overtime
u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff)
{
	u32 overtime=0;
	u16 i;
	RS485_TX_ENABLE;
	for(i=0;i<bytes_size;i++)
	{
		overtime=0;
		USART1->DR = send_buff[i];
		while((USART1->SR&0x40)==0)
		{
			if(overtime>UART_SEND_OVERTIME)
				return 0;
			overtime++;
		}
	}
    RS485_RX_ENABLE;
	return 1;
}

#endif


//cmd,0,stop; 1,tele,2wide; 3,far,4,near
void pelcod_zf_packet_send(u8 cmd,u8 zfspeed)
{
	u8 cnt;
	
	u8 cmd_buff_private[7];
	cmd_buff_private[0] = 0xff;
	cmd_buff_private[1] = domeNo;

	
	switch(cmd)
	{
	case 1:
		cmd_buff_private[3] = 0x20;
		cmd_buff_private[2] = 0;
		break;
	case 2:
		cmd_buff_private[3] = 0x40;
		cmd_buff_private[2] = 0;
		break;
	case 3:
		cmd_buff_private[3] = 0x00;
		cmd_buff_private[2] = 0x01;//
		break;
	case 4:
		cmd_buff_private[3] = 0x80;
		cmd_buff_private[2] = 0;
		break;
	case 0:
		cmd_buff_private[3] = 0x00;
		cmd_buff_private[2] = 0;
		break;
	}
	
	cmd_buff_private[4] = 0;
	cmd_buff_private[5] = 0;
	
	cmd_buff_private[6] = cmd_buff_private[1] + cmd_buff_private[2] + cmd_buff_private[3] + cmd_buff_private[4] + cmd_buff_private[5];
	RS485_SendBytes(7,cmd_buff_private);
}


void pelcod_call_pre_packet_send(u8 val)
{
	u8 cnt;
	
	u8 cmd_buff_private[7];
	cmd_buff_private[0] = 0xff;
	cmd_buff_private[1] = domeNo;
	cmd_buff_private[2] = 0;
	cmd_buff_private[3] = 0x07;
	cmd_buff_private[4] = 0;
	cmd_buff_private[5] = val;
	
	cmd_buff_private[6] = cmd_buff_private[1] + cmd_buff_private[2] + cmd_buff_private[3] + cmd_buff_private[4] + cmd_buff_private[5];
	RS485_SendBytes(7,cmd_buff_private);

}


void pelcod_set_pre_packet_send(u8 val)
{
	u8 cnt;
	
	u8 cmd_buff_private[7];
	cmd_buff_private[0] = 0xff;
	cmd_buff_private[1] = domeNo;
	cmd_buff_private[2] = 0;
	cmd_buff_private[3] = 0x03;
	cmd_buff_private[4] = 0;
	cmd_buff_private[5] = val;
	
	cmd_buff_private[6] = cmd_buff_private[1] + cmd_buff_private[2] + cmd_buff_private[3] + cmd_buff_private[4] + cmd_buff_private[5];
	RS485_SendBytes(7,cmd_buff_private);
}


void SerialPutString(unsigned char *Sent_value,int lenghtbuf)
{
    int k ;
//	RS485TX_Enable();
	 for (k=0;k<lenghtbuf;k++)   // 发回上位机
	     {
	       USART_SendData(USART1, *(Sent_value+k));
		   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		  // delay_half_ms(12);
		 }
//      delay_half_ms(20);
//	 RS485TX_DisEnable() ;
   
}

void timer0_initial(void)
{
// TCCR0 = 0x00; //stop timer0
// TCNT0 = 0x83; //set count
// TIMSK |= 0x01; //timer interrupt sources
// TCCR0 = 0x06; //start timer0
	BSP_Timer2_Init();
}


//////////////////////////////////////////////////////////////
//TIMER3 initialisation - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 128uSec
// actual value: 128.000uSec (0.0%)
void timer3_init(void)
{	 
 /*TCCR3B = 0x00; //stop
 TCNT3H = 0xFF; //setup
 TCNT3L = 0xa0;
 TCCR3A = 0x00;          //这里不启动定时器
// TCCR3B = 0x02;*/
	BSP_Timer5_Init();
}

u16 key_check_period_cnt = 0;
u32 timer_cnt_x2ms	= 0;

void timer0_int(void)////2 ms timer
{
	if(key_check_period_cnt>0)
		key_check_period_cnt--;

    if(af_action_cnt < 0xff)
        af_action_cnt++;

    if(af_action_onepush_cnt < 0xff)
        af_action_onepush_cnt++;

	if(timer_cnt_x2ms < 0xffffffff)
		timer_cnt_x2ms++;
}

void read_protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{
	       FLASH_Unlock();//不解锁FALSH可能会导致无法正常设置读保护
	       FLASH_ReadOutProtection(ENABLE);
	}
}



u8 key_get(u16 *data)
{
	u16 temp = 0;
	u8 ret = 0;
	temp = (GPIO_ReadInputData(KEY_PORT)>>4&0x000f);
	if(temp)
	{
		delayms(3);
		if(temp == (GPIO_ReadInputData(KEY_PORT)>>4)&0x000f)
		{
			*data = temp;
			ret = 1;
		}
	}
	return ret;
}

u16 KeyPre = 0;//  上次按键的状态
u16 KeyCurrent = 0;//  本次按键的状态
u16 KeyStbLast = 0;// 经滤波后的上次按键的状态
u16 KeyStb = 0;//  经滤波后的本次按键的状态
u16 KeyAvlUp = 0;// 按键抬起有效结果寄存器
u16 KeyAvlDown = 0;//  按键按下有效结果寄存器

#define	KEY_DOWN_STATE		0

u16 key_val_table[] = {KEY_LEFT_PIN,KEY_RIGHT_PIN,KEY_UP_PIN,KEY_DOWN_PIN,KEY_SET_PIN};




#define		KEY_DEFAULT		0X1F00


u16 key_pre = 0;
u16 key_pre2 = 0;



#define	KEY_PORT1		GPIOA
#define	KEY_PORT2		GPIOB
#define	KEY_PORT3		GPIOC


enum key_type
{
	KEY_NONE,
		KEY_FOCUS_PLUS, 
		
		KEY_ZOOM_SUB,	

	
	KEY_ZOOM_PLUS,	
		KEY_FOCUS_SUB,	
	
	KEY_MODE,	
};

#define	key_to_long(val)	(val|0x9000)
#define	key_to_release(val)	(val|0x8000)


u32 key_merge(void)
{
	u32 data = 0;

	u32 key_tmp;
	
	data = GPIO_ReadInputData(KEY_PORT3);	
		
	key_tmp = (data>>6)&0x000f;//0-1


	return key_tmp;
}

#define	KEY_NUMS_MAX	4


u32 press_continue_cnt = 0;
u8 continue_motor_flag = 0;
u16 motor_steps_cnt = 0;
u8 press_long_flag = 0;

u8 press_long_flag2 = 0;
u32 press_continue_cnt2 = 0;
u8 continue_motor_flag2 = 0;


static u16 key_check_1(void)
{
	u16 i;
	u32 key_tmp;
	static u32 long_press_cnt = 0;// 50ms
	
	key_tmp = key_merge();
	for(i=0;i<KEY_NUMS_MAX;i++)
	{
		if(((key_tmp>>i)&0x0001)==0)
		{
			delay_X1ms(40);

			key_tmp = key_merge();

			if(((key_tmp>>i)&0x0001)==0)
			{
				if(key_pre == i+1)
				{

					if(key_pre2 != KEY_NONE)
					{
						if(long_press_cnt>80)
						{
							if(press_long_flag)
								return 0;
							
							press_long_flag = 1;
							long_press_cnt=0;
							key_pre = 0;

							
							return ((i+1)|0x9000);
						}
						else
						{
							long_press_cnt++;

						}
					}
					else
					{
						key_pre = i+1;
						return (key_pre);	

					}

				}
				key_pre = i+1;
				//return (i+1);
				break;
			}
		}
	}


	if((key_pre && key_pre!=(i+1))||(key_pre && i>=KEY_NUMS_MAX))
	{
		i = key_pre|0x8000;
		key_pre = 0;

		if(press_continue_cnt>10)
			i = 0;

		if(press_long_flag)
		{
			press_long_flag = 0;
			i =0;
		}
		press_continue_cnt = 0;
		motor_steps_cnt = 0;
			
		continue_motor_flag = 0;
		
		return i;

	}
	return 0;
}	


static u16 key_check_2(void)
{
	u16 i=0;
	u32 key_tmp;
	static u32 long_press_cnt = 0;// 50ms
	
	key_tmp = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15);
	

	if(((key_tmp)&0x0001)==0)
	{
		delay_X1ms(30);

		key_tmp = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15);

		i=0;
		if(((key_tmp)&0x0001)==0)
		{
			if(key_pre2 == KEY_MODE)
			{
				if(long_press_cnt>80)
				{
					if(press_long_flag2)
						return 0;
					
					press_long_flag2 = 1;
					long_press_cnt=0;
					key_pre2 = 0;

					
					return (KEY_MODE|0x9000);
				}
				
				long_press_cnt++;

			}
			key_pre2 = KEY_MODE;
			return (KEY_MODE);
		}
	}


	if(key_pre2==KEY_MODE)
	{
		i = key_pre2|0x8000;
		key_pre2 = 0;

		if(press_continue_cnt2>10)
			i = 0;

		if(press_long_flag2)
		{
			press_long_flag2 = 0;
			i =0;
		}
		press_continue_cnt = 0;
			
		continue_motor_flag2 = 0;
		
		return i;

	}
	return 0;
}



//返回0为无按键，返回非0值，则为对应的按键号
static u16 key_ctl_check(void)
{
	key_check_2();

	
	return (key_check_1());
	
}




#define	IRIS_OPEN_DIR		0
#define	IRIS_CLOSE_DIR		1

#define	IRIS_STEPS_MAX		IRIS_LENS_STEPS_MAX

s16 iris_step_cnt = 0;//靠近CLOSE边为0，到OPEN最大时为最大值35

#define	 AN41908_STEPS_ONE_TIME  25//(system_expand_para.system_para.para_stepsmotor_pulse_per_step)


u32 left_motor_steps = 0;
u32 right_motor_steps = 0;

//dir,1,left; 2,right; 0,stop 
//左边电机
void right_motor_run(u8 dir)
{
	if(dir == 1)
	{

				if(continue_motor_flag)
		{
			if(motor_steps_cnt > 100)
				return;
			motor_steps_cnt++;
		}
		LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(10);

	}
	else 
	{

				if(continue_motor_flag)
		{
			if(motor_steps_cnt > 100)
				return;
			motor_steps_cnt++;
		}
		LenDrvZoomMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(10);

	}

	an41908_motor_on_off(0);

	//an41908_brake_set(0);		
}


//dir,1,left; 2,right; 0,stop 
//左边电机
void left_motor_run(u8 dir)
{
	if(dir == 1)
	{
		if(continue_motor_flag)
		{
			if(motor_steps_cnt > 100)
				return;
			motor_steps_cnt++;
		}
		LensDrvFocusMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(10);

	}
	else
	{
				if(continue_motor_flag)
		{
			if(motor_steps_cnt > 100)
				return;
			motor_steps_cnt++;
		}
				
		LensDrvFocusMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(10);

	}


	an41908_motor_on_off(0);
	//an41908_brake_set(0);		

}



u8 en_p0_state = 0;

void key_analyze(u16 val)
{
	static u16 val_pre = 0xffff;

	if(val_pre != 0xffff && val_pre == val)
		{
		
		return;
	}

	val_pre = val;
	
	


	if(key_pre2 == key_to_release(KEY_MODE) || (key_pre2 == key_to_long(KEY_MODE)) ||key_pre2 == (KEY_MODE))
	{
		
		switch(val)
		{
		case key_to_release(KEY_FOCUS_PLUS):
		case key_to_release(KEY_FOCUS_SUB):
		case key_to_release(KEY_ZOOM_PLUS):
		case key_to_release(KEY_ZOOM_SUB):
			pelcod_call_pre_packet_send(200 + (val&0x00ff));
		
			break;
		case key_to_long(KEY_FOCUS_PLUS):
		case key_to_long(KEY_FOCUS_SUB):
		case key_to_long(KEY_ZOOM_PLUS):
		case key_to_long(KEY_ZOOM_SUB):
			pelcod_call_pre_packet_send(204 + (val&0x00ff));
		
			break;
					
		default:
			break;
		}

		return;
	}
	else
	{
		if(val > 0 && val < KEY_MODE)
			{
				switch(val)
				{
				case (KEY_FOCUS_PLUS):
					pelcod_zf_packet_send(PD_FOCUS_FAR_CMD,0);
					break;
				case (KEY_FOCUS_SUB):
					pelcod_zf_packet_send(PD_FOCUS_NEAR_CMD,0);
					break;
		
				case (KEY_ZOOM_PLUS):
					pelcod_zf_packet_send(PD_ZOOM_TELE_CMD,0);
					break;
				case (KEY_ZOOM_SUB):
					pelcod_zf_packet_send(PD_ZOOM_WIDE_CMD,0);
					break;
				default:
					break;
				}
		
				return;
		
			}
		
			switch(val)
			{
			case key_to_release(KEY_FOCUS_PLUS):
			case key_to_release(KEY_FOCUS_SUB):
			case key_to_release(KEY_ZOOM_PLUS):
			case key_to_release(KEY_ZOOM_SUB):
				pelcod_zf_packet_send(PD_ZOOM_FOCUS_STOP,0);
		
				break;	
				
			default:
				break;
			}



	}
}

u8 key_monitor(void)
{
	u16 key_tmp;
	
	key_tmp = key_ctl_check();
	if(key_tmp)
	{
		key_analyze(key_tmp);
		
	}

	return 0;
}




//u8 ttest[2000];
//double myd = 0.00078125;
u32 ttlen,ttlens,FlashID;
//#define VECT_TAB_RAM
extern void delay1Ms(int ms);


int main(void)
{	
    u16 k;
	
	InterruptConfig();
	ports_initial();

	Baud_rate = DOME_BAUDRATE_2400;//2400
	domeNo = 0xff;
	uart0_init();


	while(1)
	{
			
        key_monitor();
		delay_X1ms(10);
		
	}
}


