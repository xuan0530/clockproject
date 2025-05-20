#include "stm32f10x.h"
#include "Delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "AD.h"
#include "MyDMA.h"
#include "MPU6050.h"
#include "W25Q32.h"

uint8_t MID;
uint16_t DID;
uint8_t ArrayWrite[]={0xA1,0xB2,0xC3,0xD4};
uint8_t ArrayRead[4];
int16_t AX,AY,AZ,GX,GY,GZ;


int main(void)
{
  
    
    Lcd_Init();  
//		MPU6050_Init();
	  W25Q32_Init();
    LCD_LED_SET;  // 打开背光    
    Lcd_Clear(BLACK);
	Gui_DrawFont_GBK16(10, 10,WHITE,BLACK, "MID:  DID:");
	Gui_DrawFont_GBK16(10, 30, WHITE,BLACK,"W:");
	Gui_DrawFont_GBK16(10, 50,WHITE,BLACK, "R:");
	
//	  uint8_t ID = MPU6050_ReadReg(0x75);
//	
//    LCD_ShowHexNum(10, 10, ID, GREEN, GRAY0);
    
//    // 显示标题
//    Gui_DrawFont_GBK16(10, 10, RED, GRAY0, "AD1:");
//    Gui_DrawFont_GBK16(10, 30, GREEN, GRAY0, "AD2:");
		W25Q32_ReadID(&MID,&DID);
	  LCD_ShowHexNum(40, 10, MID, WHITE,BLACK,2);
  	LCD_ShowHexNum(90, 10, DID, WHITE,BLACK,4); 
		
		W25Q32_SectorErase(0x000000);
		W25Q32_PageProgram(0x000000,ArrayWrite,4);
		
		W25Q32_ReadData (0x000000,ArrayRead,4);
		LCD_ShowHexNum(30, 30, ArrayWrite[0], WHITE,BLACK,2); 
		LCD_ShowHexNum(50, 30, ArrayWrite[1], WHITE,BLACK,2); 
		LCD_ShowHexNum(70, 30, ArrayWrite[2], WHITE,BLACK,2); 
		LCD_ShowHexNum(90, 30, ArrayWrite[3], WHITE,BLACK,2); 
		
		LCD_ShowHexNum(30, 50, ArrayRead[0], WHITE,BLACK,2); 
		LCD_ShowHexNum(50, 50, ArrayRead[1], WHITE,BLACK,2); 
		LCD_ShowHexNum(70, 50, ArrayRead[2], WHITE,BLACK,2); 
		LCD_ShowHexNum(90, 50, ArrayRead[3], WHITE,BLACK,2); 
    while (1)
    {
			
//			MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
//			LCD_ShowSignedNum(10, 10, AX, 5, WHITE,BLACK);
//			LCD_ShowSignedNum(10, 30, AY, 5, WHITE,BLACK);
//			LCD_ShowSignedNum(10, 50, AZ, 5, WHITE,BLACK);
//			LCD_ShowSignedNum(70, 10, GX, 5, WHITE,BLACK);
//			LCD_ShowSignedNum(70, 30, GY, 5, WHITE,BLACK);
//			LCD_ShowSignedNum(70, 50, GZ, 5, WHITE,BLACK);
			
//			  AD_GetValue();
//			  LCD_ShowNum(50, 10, AD_Value[0], 4, RED, GRAY0);    // 显示通道1的AD值
//        LCD_ShowNum(50, 30, AD_Value[1], 4, GREEN, GRAY0);  // 显示通道2的AD值

//			Delay_ms(1000);		//延时1s，观察转运前的现象
		}

		
	}	

