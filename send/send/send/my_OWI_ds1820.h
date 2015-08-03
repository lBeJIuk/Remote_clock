#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define SKIP_ROM 0xCC // Пропустить индентификацию
#define CONVERT_T 0x44 // Измерить температуру
#define READ_SCRATCHPAD 0xBE // Прочитать измеренное
 
//определяем порт и бит к которому подключено устройство
#define OWI_PORT PORTD
#define OWI_DDR DDRD
#define OWI_PIN PIND
#define OWI_BIT4 4
#define OWI_BIT3 3
#define OWI_BIT2 2

 
#define OWI_LOW4 OWI_DDR |= 1<<OWI_BIT4//притягиваем шину к 0
#define OWI_LOW3 OWI_DDR |= 1<<OWI_BIT3//притягиваем шину к 0
#define OWI_LOW2 OWI_DDR |= 1<<OWI_BIT2//притягиваем шину к 0

#define OWI_HIGH4 OWI_DDR &= ~(1<<OWI_BIT4)//отпускаем шину
#define OWI_HIGH3 OWI_DDR &= ~(1<<OWI_BIT3)//отпускаем шину
#define OWI_HIGH2 OWI_DDR &= ~(1<<OWI_BIT2)//отпускаем шину

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//функция определяет есть ли устройство на шине
char OWI_find4(void)
{
char SR04=SREG;//сохраняем значение регистра
cli(); //запрещаем прерывания
char device4;
OWI_LOW4; //притягиваем шину к 0
_delay_us(485);//ждем минимум 480мкс
OWI_HIGH4;//отпускаем шину
_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
 
if((OWI_PIN & (1<<OWI_BIT4)) ==0x00)//смотрим есть ли ответ
device4 = 1; //на шине есть устройство
else
device4 = 0; //на шине нет устройства
SREG=SR04;//возвращаем первонач знач регистра
_delay_us(420);//ждем оставшееся время до 480мкс
return device4;
}
char OWI_find3(void)
{
	char SR0=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char device;
	OWI_LOW3; //притягиваем шину к 0
	_delay_us(485);//ждем минимум 480мкс
	OWI_HIGH3;//отпускаем шину
	_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
	
	if((OWI_PIN & (1<<OWI_BIT3)) ==0x00)//смотрим есть ли ответ
	device = 1; //на шине есть устройство
	else
	device = 0; //на шине нет устройства
	SREG=SR0;//возвращаем первонач знач регистра
	_delay_us(420);//ждем оставшееся время до 480мкс
	return device;
}
char OWI_find2(void)
{
	char SR0=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char device;
	OWI_LOW2; //притягиваем шину к 0
	_delay_us(485);//ждем минимум 480мкс
	OWI_HIGH2;//отпускаем шину
	_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
	
	if((OWI_PIN & (1<<OWI_BIT2)) ==0x00)//смотрим есть ли ответ
	device = 1; //на шине есть устройство
	else
	device = 0; //на шине нет устройства
	SREG=SR0;//возвращаем первонач знач регистра
	_delay_us(420);//ждем оставшееся время до 480мкс
	return device;
}
 
// функция отправляет бит на устройство
void OWI_write_bit4 (char bit4)
{
char SR14=SREG;//сохраняем значение регистра
cli(); //запрещаем прерывания
OWI_LOW4;//логический "0"
_delay_us(2);
if(bit4) OWI_HIGH4; //отпускаем шину
_delay_us(65);
OWI_HIGH4; //отпускаем шину
SREG=SR14;//возвращаем первонач знач регистра
}
void OWI_write_bit3 (char bit)
{
	char SR1=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	OWI_LOW3;//логический "0"
	_delay_us(2);
	if(bit) OWI_HIGH3; //отпускаем шину
	_delay_us(65);
	OWI_HIGH3; //отпускаем шину
	SREG=SR1;//возвращаем первонач знач регистра
}
void OWI_write_bit2 (char bit)
{
	char SR1=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	OWI_LOW2;//логический "0"
	_delay_us(2);
	if(bit) OWI_HIGH2; //отпускаем шину
	_delay_us(65);
	OWI_HIGH2; //отпускаем шину
	SREG=SR1;//возвращаем первонач знач регистра
}
 
//функция посылает байт на устройство
void OWI_write_byte4(unsigned char c){
char i;
for(i = 0; i < 8; i++){//в цикле посылаем побитно
if((c & (1<<i)) == 1<<i)//если бит=1 посылаем 1
OWI_write_bit4(1);
else//иначе посылаем 0
OWI_write_bit4(0);
}}
void OWI_write_byte3(unsigned char c){
	 char i;
	 for(i = 0; i < 8; i++){//в цикле посылаем побитно
	 	 if((c & (1<<i)) == 1<<i)//если бит=1 посылаем 1
		 OWI_write_bit3(1);
		 else//иначе посылаем 0
		 OWI_write_bit3(0);
	 }} 
void OWI_write_byte2(unsigned char c){
		 char i;
		 for(i = 0; i < 8; i++){//в цикле посылаем побитно
		 	 if((c & (1<<i)) == 1<<i)//если бит=1 посылаем 1
			 OWI_write_bit2(1);
			 else//иначе посылаем 0
			 OWI_write_bit2(0);
		 }}

//функция чтения одного бита
char OWI_read_bit4 (void)
{
char SR24=SREG;//сохраняем значение регистра
cli(); //запрещаем прерывания
char OWI_rbit4; //переменная хранения бита
OWI_LOW4;//логический "0"
_delay_us(2);
OWI_HIGH4; //отпускаем шину
_delay_us(13);
OWI_rbit4=(OWI_PIN & (1<<OWI_BIT4))>>OWI_BIT4;//строчка исправлена 05.2014 (исправлена ошибка)
_delay_us(45);//задержка до окончания тайм-слота
SREG=SR24;//возвращаем первонач знач регистра
return OWI_rbit4;
}
char OWI_read_bit3 (void)
 {
	 char SR2=SREG;//сохраняем значение регистра
	 cli(); //запрещаем прерывания
	 char OWI_rbit; //переменная хранения бита
	 OWI_LOW3;//логический "0"
	 _delay_us(2);
	 OWI_HIGH3; //отпускаем шину
	 _delay_us(13);
	 OWI_rbit=(OWI_PIN & (1<<OWI_BIT3))>>OWI_BIT3;//строчка исправлена 05.2014 (исправлена ошибка)
	 _delay_us(45);//задержка до окончания тайм-слота
	 SREG=SR2;//возвращаем первонач знач регистра
	 return OWI_rbit;
 }
char OWI_read_bit2 (void)
 {
	 char SR2=SREG;//сохраняем значение регистра
	 cli(); //запрещаем прерывания
	 char OWI_rbit; //переменная хранения бита
	 OWI_LOW2;//логический "0"
	 _delay_us(2);
	 OWI_HIGH2; //отпускаем шину
	 _delay_us(13);
	 OWI_rbit=(OWI_PIN & (1<<OWI_BIT2))>>OWI_BIT2;//строчка исправлена 05.2014 (исправлена ошибка)
	 _delay_us(45);//задержка до окончания тайм-слота
	 SREG=SR2;//возвращаем первонач знач регистра
	 return OWI_rbit;
 }
 
//функция читает один байт с устройства 1-wire
unsigned char OWI_read_byte4()
{
char data4=0,i;
for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
data4|= OWI_read_bit4()<<i;//читаем очередной бит
return data4;
}
unsigned char OWI_read_byte3()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
	data|= OWI_read_bit3()<<i;//читаем очередной бит
	return data;
}
unsigned char OWI_read_byte2()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
	data|= OWI_read_bit2()<<i;//читаем очередной бит
	return data;
}
 
//функция преобразует полученные с датчика 18b20 данные в температуру

 
 int  temp_18b20_4() {
	 unsigned char B4=0;
	 int ds18_temp4=0;
	 if(OWI_find4()==1)//если есть устройство на шине
	 {
		 OWI_write_byte4(SKIP_ROM);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		 OWI_write_byte4(CONVERT_T);//преобразовать температуру
		 _delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		 OWI_find4();//снова посылаем Presence и Reset
		 OWI_write_byte4(SKIP_ROM);
		 OWI_write_byte4(READ_SCRATCHPAD);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		 
		 B4 = OWI_read_byte4(); //читаем бит LS
		 ds18_temp4 = OWI_read_byte4(); //читаем бит MS
		 ds18_temp4 = (ds18_temp4<<8)|B4;//уладываем биты в последовательности MS потом LS
	 }
	 // else return можно сделать возврат числа что датчика нет
	 return ds18_temp4;//возвращаем int (MS,LS)
 }
 int  temp_18b20_3()
 {
	 unsigned char B;
	 unsigned int ds18_temp=0;
	 if(OWI_find3()==1)//если есть устройство на шине
	 {
		 OWI_write_byte3(SKIP_ROM);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		 OWI_write_byte3(CONVERT_T);//преобразовать температуру
		 _delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		 OWI_find3();//снова посылаем Presence и Reset
		 OWI_write_byte3(SKIP_ROM);
		 OWI_write_byte3(READ_SCRATCHPAD);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		 
		 B = OWI_read_byte3(); //читаем бит LS
		 ds18_temp = OWI_read_byte3(); //читаем бит MS
		 ds18_temp = (ds18_temp<<8)|B;//уладываем биты в последовательности MS потом LS
	 }
	 // else return можно сделать возврат числа что датчика нет
	 return ds18_temp;//возвращаем int (MS,LS)
 }
int  temp_18b20_2()
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find2()==1)//если есть устройство на шине
	{
		OWI_write_byte2(SKIP_ROM);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		OWI_write_byte2(CONVERT_T);//преобразовать температуру
		_delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		OWI_find2();//снова посылаем Presence и Reset
		OWI_write_byte2(SKIP_ROM);
		OWI_write_byte2(READ_SCRATCHPAD);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		
		B = OWI_read_byte2(); //читаем бит LS
		ds18_temp = OWI_read_byte2(); //читаем бит MS
		ds18_temp = (ds18_temp<<8)|B;//уладываем биты в последовательности MS потом LS
	}
	// else return можно сделать возврат числа что датчика нет
	return ds18_temp;//возвращаем int (MS,LS)
}



/*
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define SKIP_ROM 0xCC // Пропустить индентификацию
#define CONVERT_T 0x44 // Измерить температуру
#define READ_SCRATCHPAD 0xBE // Прочитать измеренное

//определяем порт и бит к которому подключено устройство
#define OWI_PORT PORTD
#define OWI_DDR DDRD
#define OWI_PIN PIND

#define OWI_LOW OWI_DDR |= 1<<OWI_BIT//притягиваем шину к 0
#define OWI_HIGH OWI_DDR &= ~(1<<OWI_BIT)//отпускаем шину

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//функция определяет есть ли устройство на шине
char OWI_find(char pin)
{
	char SR0=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char device;
	OWI_DDR |= 1<<pin; //притягиваем шину к 0
	_delay_us(485);//ждем минимум 480мкс
	OWI_DDR &= ~(1<<pin);//отпускаем шину
	_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
	
	if((OWI_PIN & (1<<pin)) ==0x00)//смотрим есть ли ответ
	device = 1; //на шине есть устройство
	else
	device = 0; //на шине нет устройства
	SREG=SR0;//возвращаем первонач знач регистра
	_delay_us(420);//ждем оставшееся время до 480мкс
	return device;
}

// функция отправляет бит на устройство
void OWI_write_bit (char bit, char pin)
{
	char SR1=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	OWI_DDR |= 1<<pin;//логический "0"
	_delay_us(2);
	if(bit)	OWI_DDR &= ~(1<<pin); //отпускаем шину
	_delay_us(65);
	OWI_DDR &= ~(1<<pin); //отпускаем шину
	SREG=SR1;//возвращаем первонач знач регистра
}

//функция посылает байт на устройство
void OWI_write_byte(unsigned char c,char pin)
{
	char i;
	for(i = 0; i < 8; i++)//в цикле посылаем побитно
	{
		if((c & (1<<i)) == 1<<i)//если бит=1 посылаем 1
		OWI_write_bit(1,pin);
		else//иначе посылаем 0
		OWI_write_bit(0,pin);
	}
}

//функция чтения одного бита
char OWI_read_bit (char pin)
{
	char SR2=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char OWI_rbit; //переменная хранения бита
	OWI_DDR |= 1<<pin;//логический "0"
	_delay_us(2);
	OWI_DDR &= ~(1<<pin); //отпускаем шину
	_delay_us(13);
	OWI_rbit=(OWI_PIN& (1<<pin))>>pin;//строчка исправлена 05.2014 (исправлена ошибка)
	_delay_us(45);//задержка до окончания тайм-слота
	SREG=SR2;//возвращаем первонач знач регистра
	return OWI_rbit;
}

//функция читает один байт с устройства 1-wire
unsigned char OWI_read_byte(char pin)
{
	char data=0,i;
	for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
	data|= OWI_read_bit(pin)<<i;//читаем очередной бит
	return data;
}

//функция преобразует полученные с датчика 18b20 данные в температуру
int  temp_18b20(char pin)
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find(pin)==1)//если есть устройство на шине
	{
		OWI_write_byte(SKIP_ROM,pin);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		OWI_write_byte(CONVERT_T,pin);//преобразовать температуру
		_delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		OWI_find(pin);//снова посылаем Presence и Reset
		OWI_write_byte(SKIP_ROM,pin);
		OWI_write_byte(READ_SCRATCHPAD,pin);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		
		B = OWI_read_byte(pin); //читаем бит LS
		ds18_temp = OWI_read_byte(pin); //читаем бит MS
		ds18_temp = (ds18_temp<<8)|B;//уладываем биты в последовательности MS потом LS
	}
	// else return можно сделать возврат числа что датчика нет
	return ds18_temp;//возвращаем int (MS,LS)
}
*/

/*
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SKIP_ROM 0xCC // Пропустить индентификацию
#define CONVERT_T 0x44 // Измерить температуру
#define READ_SCRATCHPAD 0xBE // Прочитать измеренное

//определяем порт и бит к которому подключено устройство
#define OWI_PORT PORTB
#define OWI_DDR DDRB
#define OWI_PIN PINB
#define OWI_BIT 0

#define OWI_LOW OWI_DDR |= 1<<OWI_BIT//притягиваем шину к 0
#define OWI_HIGH OWI_DDR &= ~(1<<OWI_BIT)//отпускаем шину

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//функция определяет есть ли устройство на шине
char OWI_find(void)
{
	char SR0=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char device;
	OWI_LOW; //притягиваем шину к 0
	_delay_us(485);//ждем минимум 480мкс
	OWI_HIGH;//отпускаем шину
	_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
	
	if((OWI_PIN & (1<<OWI_BIT)) ==0x00)//смотрим есть ли ответ
	device = 1; //на шине есть устройство
	else
	device = 0; //на шине нет устройства
	SREG=SR0;//возвращаем первонач знач регистра
	_delay_us(420);//ждем оставшееся время до 480мкс
	return device;
}

// функция отправляет бит на устройство
void OWI_write_bit (char bit)
{
	char SR1=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	OWI_LOW;//логический "0"
	_delay_us(2);
	if(bit) OWI_HIGH; //отпускаем шину
	_delay_us(65);
	OWI_HIGH; //отпускаем шину
	SREG=SR1;//возвращаем первонач знач регистра
}

//функция посылает байт на устройство
void OWI_write_byte(unsigned char c)
{
	char i;
	for(i = 0; i < 8; i++)//в цикле посылаем побитно
	{
		if((c & (1<<i)) == 1<<i)//если бит=1 посылаем 1
		OWI_write_bit(1);
		else//иначе посылаем 0
		OWI_write_bit(0);
	}
}

//функция чтения одного бита
char OWI_read_bit (void)
{
	char SR2=SREG;//сохраняем значение регистра
	cli(); //запрещаем прерывания
	char OWI_rbit; //переменная хранения бита
	OWI_LOW;//логический "0"
	_delay_us(2);
	OWI_HIGH; //отпускаем шину
	_delay_us(13);
	OWI_rbit=(OWI_PIN & (1<<OWI_BIT))>>OWI_BIT;//строчка исправлена 05.2014 (исправлена ошибка)
	_delay_us(45);//задержка до окончания тайм-слота
	SREG=SR2;//возвращаем первонач знач регистра
	return OWI_rbit;
}

//функция читает один байт с устройства 1-wire
unsigned char OWI_read_byte()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
	data|= OWI_read_bit()<<i;//читаем очередной бит
	return data;
}

//функция преобразует полученные с датчика 18b20 данные в температуру
int  temp_18b20()
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find()==1)//если есть устройство на шине
	{
		OWI_write_byte(SKIP_ROM);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		OWI_write_byte(CONVERT_T);//преобразовать температуру
		_delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		OWI_find();//снова посылаем Presence и Reset
		OWI_write_byte(SKIP_ROM);
		OWI_write_byte(READ_SCRATCHPAD);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		
		B = OWI_read_byte(); //читаем бит LS
		ds18_temp = OWI_read_byte(); //читаем бит MS
		ds18_temp = (ds18_temp<<8)|B;//уладываем биты в последовательности MS потом LS
	}
	// else return можно сделать возврат числа что датчика нет
	return ds18_temp;//возвращаем int (MS,LS)
}

//перевод инф от temp_18b20() в удобный вид
char convert (unsigned int td)
{
	char dat=td>>4;//сдвиг и отсечение старшего байта
	return dat;// (знак,2в6,2в5,2в4,2в3,2в2,2в1,2в0)
}*/