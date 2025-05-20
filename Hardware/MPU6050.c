#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS 0xD0

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t  RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);//先写入要读的寄存器地址，再读，重新指定读写位
	MyI2C_ReceiveAck();					//接收应答
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//最低位为1
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);//想要继续读，就发送应答，不给置1
	MyI2C_Stop();
	
	return Data;
}

void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//设备不复位0，接触睡眠模式0，不循环模式0，无关位0，温度传感器不使能0，陀螺仪时钟001
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);//十分频
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);//110最平滑的滤波
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//最大量程
	
}

void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,
	                     int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)//指针传多返回值
{
	uint8_t DataH,DataL;
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L );
	*AccX = (DataH << 8 ) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L );
	*AccY = (DataH << 8 ) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L );
	*AccZ = (DataH << 8 ) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L );
	*GyroX = (DataH << 8 ) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L );
	*GyroY = (DataH << 8 ) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H );
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L );
	*GyroZ = (DataH << 8 ) | DataL;
	
	
}
