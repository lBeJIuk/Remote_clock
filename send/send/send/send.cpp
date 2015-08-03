#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "my_OWI_ds1820.h"
#include "SPI.h"
#include "nRF24L01.h"
/*#define BAUD 4800
#define UBRR_VAL F_CPU/16/BAUD-1

void usart_init (unsigned int speed)
{
	// устанавливаем скорость Baud Rate: 4800
	UBRR0H=(unsigned char)(speed>>8);
	UBRR0L=(unsigned char) speed;
	UCSR0A=0x00;
	UCSR0B|=(1<<TXEN0)|(1<<RXEN0);// Разрешение работы приемника
	UCSR0B|=(1<<RXCIE0);// Разрешение прерываний по приему
	// Установка формата посылки: 8 бит данных, 1 стоп-бит
	UCSR0C=(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
}
void USART_temp (int U_temp){
	UDR0=48+(((U_temp>>4)&0x00ff)/10);
	_delay_ms(5);
	UDR0=48+(((U_temp>>4)&0xff)%10);
	_delay_ms(5);
	UDR0=46;
	_delay_ms(5);
	U_temp=get_denum(U_temp);
	UDR0=48+(U_temp/10);
	_delay_ms(5);
	UDR0=48+(U_temp%10);
	_delay_ms(5);
	UDR0=13;
	_delay_ms(5);
}
*/
void mSend(char pin)
{
	int T=0;//измерить и послать с определенного пина
	//int x;
	char code0,code1,code2;
	char pack[3]={0,0,0};
	
	switch (pin){
	case 4:
	T=temp_18b20_4();
	code0=0x10;
	code1=0x20;
	code2=0x30;
	break;
		
	case 3:
	T=temp_18b20_3();
	code0=0x40;
	code1=0x50;
	code2=0x60;
	break;
		
	case 2:
	T=temp_18b20_2();
	code0=0x70;
	code1=0x80;
	code2=0x90;
	break;
	}
	
 
pack[0]=((T>>8)&0x0F)|code0;
pack[1]=((T>>4)&0x0F)|code1;
pack[2]=(T&0x0F)|code2;
	for(char i=0;i<3;i++){
	nrf24l01_FLUSH_TX;//очищаем буфер передатчика
	nrf24l01_Sent_data_Ret(pack[i]);
	_delay_ms(100);

}
/*	x^=x;
	x=(x|((pack[0]&0x0F) <<8 )|((pack[1]&0x0f)<<4)|(pack[2]&0x0f))&0x0fff;
	UDR0=48+pin;UDR0=35;UDR0=32;
	_delay_ms(5);
	USART_temp(x);*/
}

char i=0;
int main(void)
{ 
	//usart_init (UBRR_VAL);//инициализация модуля
	SPI_MasterInit();//инициализация SPI
	nRF24L01_init(0b00000011);//инициализация модуля
	nrf24l01_RX_TX_mode(PRX);//переходим в режим приемника
	while(1)
	{	
		mSend(4);//внешний
		mSend(3);//кухня
		mSend(2);//ванная		
	
	}
}