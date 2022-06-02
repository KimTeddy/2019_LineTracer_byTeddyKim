#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stdio.h>//190713
#include <stm32f10x_adc.h>//190723
#include <stm32f10x.h>////
#include <stm32f10x_dma.h>
#include <stm32f10x_tim.h>
//------------------------------------------------------------------------------
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//LED+모터IN1,2,3,4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ADC6센서
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//모터PWM
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}
//------------------------------------------------------------------------------LED
int led0 = GPIO_Pin_1;//1-0-3-2-4-5
int led1 = GPIO_Pin_0;
int led2 = GPIO_Pin_3;
int led3 = GPIO_Pin_2;
int led4 = GPIO_Pin_4;
int led5 = GPIO_Pin_5;
//---------------------------------------LED,모터IN1,2,3,4
void GPIOC_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5//LED
		| GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;//모터
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//---------------------------------------
static void delay_int_count(volatile unsigned int nTime)
{
	for (; nTime > 0; nTime--);
}
//---------------------------------------
int num = 0x001;
void LED_shift() {
	for (num = 0x02; num < 0x40; num = num << 1)//여기부터
	{
		GPIO_ResetBits(GPIOC, num);
		GPIO_SetBits(GPIOC, num >> 1);
		delay_int_count(806596);
	}
	for (num = 0x20; num > 0x00; num = num >> 1)
	{
		GPIO_ResetBits(GPIOC, num);
		GPIO_SetBits(GPIOC, num << 1);
		delay_int_count(806596);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_0);// delay_int_count(403298);//여기까지 pin0-5-0
}
//------------------------------------------------------------------------------ADC
__IO uint16_t ADCValue[6];//ADCValue변수선언
//---------------------------------------
void ADC_Configuration() {//190723
//---------------------------------------DMA설정
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 6;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	//---------------------------------------ADC설정
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 6;
	ADC_Init(ADC1, &ADC_InitStructure);
	//---------------------------------------

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_7Cycles5);
	//---------------------------------------
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
//---------------------------------------
void GPIOA_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_4 | GPIO_Pin_5;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//---------------------------------------
//---------------------------------------
char A1; char A2; char A3; char A4; char A5; char A6;
char result;
#define b000000 0
#define b000001 1
#define b000011 3
#define b000010 2
#define b000110 6
#define b000100 4
#define b001100 12
#define b001000 8
#define b011000 24
#define b010000 16
#define b110000 48
#define b100000 32
#define b111111 63
#define b000111 7
#define b001110 14
#define b011100 28
#define b111000 56
//------------------------------------------------------------------------------PWM
void TIM_Configuration()
{
	/* TIme base configuration */
	uint16_t TimerPeriod = 1000;

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Prescaler = 72;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM4, ENABLE);
}

//---------------------------------------__IO uint16_t pulseL = 1000;
__IO uint16_t pulseR = 1000;
//---------------------------------------
void PWM_Configuration()
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = pulseL;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = pulseR;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}
//---------------------------------------
void GPIOB_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//PWM
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//---------------------------------------
int EL = GPIO_Pin_8;
int ER = GPIO_Pin_9;

int LF = GPIO_Pin_6;
int LB = GPIO_Pin_7;
int RF = GPIO_Pin_8;
int RB = GPIO_Pin_9;
__IO uint16_t derail = 0;
//------------------------------------------------------------------------------
__IO uint16_t ADC_Low[6];
void ADC_LOW() {
	for (int low = 0; low < 1000; low++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (ADC_Low[i] < ADCValue[i])
				ADC_Low[i] = ADCValue[i];
		}
	}
}
__IO uint16_t ADC_High[6];
void ADC_HIGH() {
	for (int high = 0; high < 1000; high++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (ADC_High[i] < ADCValue[i])
				ADC_High[i] = ADCValue[i];
		}
	}
}
__IO uint16_t ADC_Standard[6];
void ADC_STANDARD() {
	for (int i = 0; i < 6; i++)
	{
		ADC_Standard[i] = (ADC_High[i] + ADC_Low[i]) / 2;
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------LED
void main(void)
{
	RCC_Configuration();
	GPIOC_Configuration();
	//------------------------------------------------------------------------------ADC
	GPIOA_Configuration();
	ADC_Configuration();
	//------------------------------------------------------------------------------PWM
	GPIOB_Configuration();
	TIM_Configuration();
	PWM_Configuration();

	pulseL = 500; pulseR = 500;
	TIM_SetCompare3(TIM4, pulseL);
	TIM_SetCompare4(TIM4, pulseR);
	PWM_Configuration();
	GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
	GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
	//---------------------------------------
	delay_int_count(403298);
	GPIO_SetBits(GPIOC, led0); delay_int_count(403298);//1-0-3-2-4-5
	GPIO_SetBits(GPIOC, led1); delay_int_count(403298);
	GPIO_SetBits(GPIOC, led2); delay_int_count(403298);
	GPIO_SetBits(GPIOC, led3); delay_int_count(403298);
	GPIO_SetBits(GPIOC, led4); delay_int_count(403298);
	GPIO_SetBits(GPIOC, led5); delay_int_count(403298);//---------------------------------------
	ADC_LOW();
	LED_shift(); LED_shift();
	ADC_HIGH();
	ADC_STANDARD();
	//---------------------------------------
	//----------------------------------------------------------------------------
	GPIO_SetBits(GPIOB, EL);
	GPIO_SetBits(GPIOB, ER);
	//---------------------------------------
	GPIO_SetBits(GPIOC, LF); delay_int_count(403298); delay_int_count(403298);
	GPIO_ResetBits(GPIOC, LF);
	GPIO_SetBits(GPIOC, RF); delay_int_count(403298); delay_int_count(403298);
	GPIO_ResetBits(GPIOC, RF);

	GPIO_SetBits(GPIOC, LB); delay_int_count(403298); delay_int_count(403298);
	GPIO_ResetBits(GPIOC, LB);
	GPIO_SetBits(GPIOC, RB); delay_int_count(403298); delay_int_count(403298);
	GPIO_ResetBits(GPIOC, RB); delay_int_count(403298); delay_int_count(403298);


	GPIO_SetBits(GPIOC, LF);
	GPIO_SetBits(GPIOC, RF);
	//------------------------------------------------------------------------------
	int lV = 600;
	int rV = 700;
	while (1)
	{
		TIM_SetCompare3(TIM4, pulseL);
		TIM_SetCompare4(TIM4, pulseR);
		PWM_Configuration();

		int s1 = ADCValue[0];
		int s2 = ADCValue[1];
		int s3 = ADCValue[2];
		int s4 = ADCValue[3];
		int s5 = ADCValue[4];
		int s6 = ADCValue[5];
		//---------------------------------------
		if (s1 > ADC_Standard[0])
		{
			GPIO_ResetBits(GPIOC, led0); A1 = b100000;
		}
		else
		{
			GPIO_SetBits(GPIOC, led0); A1 = 0;
		}

		if (s2 > ADC_Standard[1])
		{
			GPIO_ResetBits(GPIOC, led1); A2 = b010000;
		}
		else
		{
			GPIO_SetBits(GPIOC, led1); A2 = 0;
		}
		if (s3 > ADC_Standard[2])
		{
			GPIO_ResetBits(GPIOC, led2); A3 = b001000;
		}
		else
		{
			GPIO_SetBits(GPIOC, led2); A3 = 0;
		}

		if (s4 > ADC_Standard[3])
		{
			GPIO_ResetBits(GPIOC, led3); A4 = b000100;
		}
		else
		{
			GPIO_SetBits(GPIOC, led3); A4 = 0;
		}

		if (s5 > ADC_Standard[4])
		{
			GPIO_ResetBits(GPIOC, led4); A5 = b000010;
		}
		else
		{
			GPIO_SetBits(GPIOC, led4); A5 = 0;
		}

		if (s6 > ADC_Standard[5])
		{
			GPIO_ResetBits(GPIOC, led5); A6 = b000001;
		}
		else
		{
			GPIO_SetBits(GPIOC, led5); A6 = 0;
		}
		//---------------------------------------

		result = A1 + A2 + A3 + A4 + A5 + A6;
		switch (result)
		{

		case b000000:
			pulseL = 1000; pulseR = 1000;
			GPIO_ResetBits(GPIOC, LF); GPIO_ResetBits(GPIOC, RF);
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			break;

			/*
			case b000000:
			 if(derail == 1)//L에서 탈선
			 {pulseL = 800; pulseR = rV;
			 GPIO_SetBits(GPIOC, LF);GPIO_SetBits(GPIOC, RF);}
			 else if(derail == 0)//중간 탈선
			 {pulseL = V; pulseR = V;
			 GPIO_ResetBits(GPIOC, LB);GPIO_ResetBits(GPIOC, RF);
			 GPIO_SetBits(GPIOC, LF);GPIO_SetBits(GPIOC, RB);
			 LED_shift();
			 }
			 else if(derail == 2)//R에서 탈선
			 {pulseL = lV; pulseR = 800;
			 GPIO_SetBits(GPIOC, LF);GPIO_SetBits(GPIOC, RF);}
			 break;

			*/
		case b000111:
			pulseL = lV - 300; pulseR = 850;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b001110:
			pulseL = lV - 300; pulseR = 800;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b000001:
			pulseL = lV - 300; pulseR = 850;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 2;
			break;
		case b000011:
			pulseL = lV - 250; pulseR = 820;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b000010:
			pulseL = lV - 200; pulseR = 800;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b000110:
			pulseL = lV - 250; pulseR = 780;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b000100:
			pulseL = lV - 100; pulseR = 760;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0; break;
		case b001100:///middle
			pulseL = lV; pulseR = rV;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;

			break;
		case b001000:
			pulseL = 600; pulseR = rV - 50;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b011000:
			pulseL = 660; pulseR = rV - 100;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b010000:
			pulseL = 720; pulseR = rV - 150;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b110000:
			pulseL = 780; pulseR = rV - 200;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b100000:
			pulseL = 800; pulseR = rV - 250;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 1;
			break;
		case b011100:
			pulseL = 750; pulseR = rV - 250;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b111000: pulseL = 800; pulseR = rV - 250;
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			GPIO_SetBits(GPIOC, LF); GPIO_SetBits(GPIOC, RF);
			derail = 0;
			break;
		case b111111:
			pulseL = 1000; pulseR = 1000;
			GPIO_ResetBits(GPIOC, LF); GPIO_ResetBits(GPIOC, RF);
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			break;

		default:
			pulseL = 1000; pulseR = 1000;
			GPIO_ResetBits(GPIOC, LF); GPIO_ResetBits(GPIOC, RF);
			GPIO_ResetBits(GPIOC, LB); GPIO_ResetBits(GPIOC, RB);
			break;
		}
	}//while
}//main