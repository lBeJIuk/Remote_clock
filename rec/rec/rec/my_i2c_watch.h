#include <avr/io.h>
#include <avr/delay.h>

#define SDA   0  //Пины шины I2C
#define SCL   1  //Пины шины I2C
#define PORT PORTB  //Порт шины I2C
#define DDR DDRB	//Порт шины I2C
#define PIN PINB	//Порт шины I2C
#define DS1307_ADDR  0b01101000 //адрес микросхемы DS1307
#define DS1307_ADDR_R  (DS1307_ADDR<<1)|1 //адрес микросхемы DS1307 + бит чтения
#define DS1307_ADDR_W   DS1307_ADDR<<1  //адрес микросхемы DS1307 + бит записи
#define for_me(x)  ((x>> 4)*10)+(0b00001111&x)
#define for_ds(x) ((x/10)<<4)|(x%10)
unsigned char ack=0;
unsigned char data[5]={};

struct TIME {
	unsigned char second;
	unsigned char minutes;
	unsigned char hour;
	unsigned char day;
	unsigned char date;
	unsigned char month;
	unsigned char year;
	};

/*Условие старта I2C*/
void start_cond (void)
{
	PORT=_BV(SDA)|_BV(SCL);
	asm("nop");
	PORT&=~_BV(SDA);
	asm("nop");
	PORT&=~_BV(SCL);
} 
/*Условие стопа I2C*/
void stop_cond (void)	{
	PORT=_BV(SCL);
	asm("nop");
	PORT&=~_BV(SDA);
	asm("nop");
	PORT|=_BV(SDA);
}
/*Послать байт по I2C*/
void send_byte (unsigned char data)	{
	unsigned char i;
	for (i=0;i<8;i++)
	{
		if ((data&0x80)==0x00) PORT&=~_BV(SDA);	//set SDA
		else PORT|=_BV(SDA);
		asm("nop");
		PORT|=_BV(SCL);				//SCL impulse
		asm("nop");
		PORT&=~_BV(SCL);
		data=data<<1;
	}
	DDRB&=~_BV(SDA);
	PORTB|=_BV(SDA);

	asm("nop");
	PORTB|=_BV(SCL);
	asm("nop");
	if ((PINB&_BV(SDA))==_BV(SDA)) ack=1;	//Reading ACK
	PORTB&=~_BV(SCL);

	PORTB&=~_BV(SDA);
	DDRB|=_BV(SDA);
}
/*Принять байт по I2C*/
unsigned char get_byte (void)	{
	unsigned char i, res=0;

	DDR&=~_BV(SDA);
	PORT|=_BV(SDA);

	for (i=0;i<8;i++)
	{
		res=res<<1;
		PORT|=_BV(SCL);				//SCL impulse
		asm("nop");
		if ((PIN&_BV(SDA))==_BV(SDA)) res=res|0x01;	//Reading SDA
		PORT&=~_BV(SCL);
		asm("nop");
	}

	PORT&=~_BV(SDA);
	DDR|=_BV(SDA);

	asm("nop");
	PORT|=_BV(SCL);
	asm("nop");
	PORT&=~_BV(SCL);

	return res;
}
/*Записать данные в DS1307 по I2C*/
void ds_write(char adr, char data)
{
	start_cond();
	send_byte (DS1307_ADDR_W);
	send_byte (adr);
	send_byte (data);
	stop_cond();
}
/*прочитать данные с DS1307 по I2C*/
unsigned char ds_read(char adr){
	unsigned char res;
	start_cond();
	send_byte(DS1307_ADDR_W);
	send_byte(adr);
	start_cond();
	send_byte(DS1307_ADDR_R);
	res=get_byte();
	stop_cond();
	return res;
}
/*Инициализация DS1307*/
void ds_init(void)	{
	unsigned char a;
	DDR|=(1<<SDA)|(1<<SCL); //PORB=0b00000011;
	a=ds_read(0);
	//a=a&0b01111111;	
	a=a& (~(1<<7)); //запуск часов
	ds_write(0,a);
	a=ds_read(2);
	//a=a&0b10111111;
	a=a& (~(1<<6));// 24 формат
	ds_write(2,a);
}
void read_ds_data(struct TIME* a){
	a->second=ds_read(0);
	a->minutes=ds_read(1);
	a->hour=ds_read(2);
	//TIME.day=ds_read(3);
	//TIME.date=ds_read(4);
	//TIME.month=ds_read(5);
	//TIME.year=ds_read(6);
	}
void convert(struct TIME* a){
	a->second=for_me(a->second);
	a->minutes=for_me(a->minutes);
	a->hour=for_me(a->hour);
}	