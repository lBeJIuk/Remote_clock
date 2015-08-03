#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
#define SKIP_ROM 0xCC // Пропустить индентификацию
#define CONVERT_T 0x44 // Измерить температуру
#define READ_SCRATCHPAD 0xBE // Прочитать измеренное

#define SEARCH_ROM 0xF0 // Прочитать КОД всех устройств
#define READ_ROM 0x33 //Прочитать КОД (если одно устройство)
#define MATCH_ROM 0x55 // обращение к конкретному устройству

//определяем порт и бит к которому подключено устройство
#define OWI_PORT PORTC
#define OWI_DDR DDRC
#define OWI_PIN PINC
#define OWI_BIT 5
 
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
}


char  ROM_18b20()
{
	char data[8];
	if(OWI_find()==1)//если есть устройство на шине
	{
		OWI_write_byte(READ_ROM);//ROM код
		OWI_find();//снова посылаем Presence и Reset
		for (char i=0;i<8;i++){
		data[i] = OWI_read_byte();
		UDR0=data[i];
		_delay_ms(10); }
	
	return data[8];//возвращаем int (MS,LS)
}
}