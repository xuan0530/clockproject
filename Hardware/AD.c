#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[2];

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//分频 72M/6
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//选择规则通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);//68个T
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_55Cycles5);//68个T
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//扫描单次、连续ENABLE
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//左对齐、右
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel=2;//扫描几个通道
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//扫描模式ENABLE
	ADC_Init(ADC1,&ADC_InitStructure);
	

	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;//外设站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord ;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)&AD_Value;//寄存器站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord ;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//传输方向,外设到存储器
	DMA_InitStructure.DMA_BufferSize=2;//缓存区大小，传输计数器,传输几次
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//是否使用自动重装
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable ;//硬件触发、软件(存储器到存储器，enable
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium ;
	
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);//复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//等到复位校准=0,跳出循环
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	
	
}

void AD_GetValue(void)
{
	  DMA_Cmd(DMA1_Channel1,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel1,2);
	  DMA_Cmd(DMA1_Channel1,ENABLE);
	
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件触发

		while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
		DMA_ClearFlag(DMA1_FLAG_TC1);
}
