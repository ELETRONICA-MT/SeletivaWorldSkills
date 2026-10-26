#include "DS1307.h"

extern I2C_HandleTypeDef hi2c1;

void DS1307_Set(uint8_t Ano, uint8_t Mes, uint8_t Dia, uint8_t Hora, uint8_t Min, uint8_t Seg){
	uint8_t buffer[8];

	buffer[0]=0x00;
	buffer[1]=Seg; // SEGUNDOS
	buffer[2]=Min;	// MINUTOS
	buffer[3]=Hora;	// HORAS
	buffer[4]=0x00;
	buffer[5]=Dia;	// DIA
	buffer[6]=Mes;	// MES
	buffer[7]=Ano;	// ANO

	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, buffer, 9, 100);
}

void DS1307_Get_Array(uint8_t ArrayDados[6]){
	uint8_t buffer[9];
	buffer[0]=0x00;

	HAL_I2C_Master_Transmit(&hi2c1,0xD0,buffer,1,10);
	HAL_I2C_Master_Receive(&hi2c1,0xD0,&buffer[1],8,10);

	ArrayDados[0]=(buffer[1]>>4)*10+(buffer[1]&0x0F);
	ArrayDados[1]=(buffer[2]>>4)*10+(buffer[2]&0x0F);
	ArrayDados[2]=(buffer[3]>>4)*10+(buffer[3]&0x0F);
	ArrayDados[3]=(buffer[5]>>4)*10+(buffer[5]&0x0F);
	ArrayDados[4]=(buffer[6]>>4)*10+(buffer[6]&0x0F);
	ArrayDados[5]=(buffer[7]>>4)*10+(buffer[7]&0x0F);
}



