#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "my_OWI_ds1820.h"
#include "SPI.h"
#include "nRF24L01.h"
#include "my_i2c_watch.h"
#include "KS0107.h"

#define BUT_1 ((PINB>>7)&1)//кнопка 1
#define BUT_2 ((PINB>>6)&1)// кнопка 2

//unsigned char rec_data[5][3]={0};//принятые данные 5 шт по 1,5 байта
//unsigned int T[3]={0,0,0};// три измеряемые температуры
unsigned char rec_byte=0;//принятый не раскодированый байт
unsigned char screen_st=0;//что на экране 0-температура; 1-время ;
unsigned char mode=0;//экран - часы - температура внутри - температура снаружи...
unsigned char cond_1=0;// состояние кнопки 1
unsigned char cond_2=0;// состояние кнопки 2
unsigned char setmode=0;// режим настройки 1; обычный режим 2;
unsigned char set=0;// выбор что настраивать 1- секунды; 2-минуты;3-часы

unsigned char rec_data0_0=0,rec_data0_1=0,rec_data0_2=0;
unsigned char rec_data1_0=0,rec_data1_1=0,rec_data1_2=0;
unsigned char rec_data2_0=0,rec_data2_1=0,rec_data2_2=0;

unsigned int T_internal=0;
unsigned int T_external=0;
unsigned int T_kitchen=0;
unsigned int T_bathroom=0;

TIME ds_time;

void timer_init(void){
	TCCR1B=(1<<CS12)|(1<<CS10);// clock/1024 
	TIMSK1=(1<<TOIE1);// enable interrupt
	TCNT1=0xffff-1000;
	}
void button_init(void){
	DDRB|=(1>>6)|(1>>7);
	//PORTB|=(1>>4)|(1>>3);
	PORTB|=(1>>6)|(1>>7);
	}
void get_data(void){
	
	rec_byte=nrf24l01_read_data();
	switch((rec_byte>>4)&0x0f){
		// case 1-3 for external sensor
		case 1:
		rec_data0_0=rec_byte&0x0f;
		break;
			
		case 2:
		rec_data0_1=rec_byte&0x0f;
		break;
		
		case 3:
		rec_data0_2=rec_byte&0x0f;
		break;
		
		// case 4-6 for kitchen sensor
		case 4:
		rec_data1_0=rec_byte&0x0f;
		break;
		
		case 5:
		rec_data1_1=rec_byte&0x0f;
		break;
		
		case 6:
		rec_data1_2=rec_byte&0x0f;
		break;
				
		// case 7-9 for bathroom
		case 7:
		rec_data2_0=rec_byte&0x0f;
		break;
		
		case 8:
		rec_data2_1=rec_byte&0x0f;
		break;
		
		case 9:
		rec_data2_2=rec_byte&0x0f;
		break;
	}
T_external=(rec_data0_0<<8)|(rec_data0_1<<4)|(rec_data0_2);
T_kitchen=(rec_data1_0<<8)|(rec_data1_1<<4)|(rec_data1_2);	
T_bathroom=(rec_data2_0<<8)|(rec_data2_1<<4)|(rec_data2_2);	
	}		
void show_temp (char Sensor, unsigned int ShowTemp ){
if (screen_st==1){KS0107_Clrscr(0);screen_st=0;send_image(0,24,13,48,4);}// clear screen if there was TIME
send_image(0,24,13,48,4);//4= image - termometr
if (Sensor==0){	send_image(0,0,37,16,Sensor);}//sensor=0=image-INT 
else if (Sensor==1){send_image(0,0,37,16,Sensor);}//sensor=1=image-EXT
else if (Sensor==2){send_image(0,0,37,16,Sensor);}//sensor=2=image-kitchen
else if (Sensor==3){send_image(0,0,37,16,Sensor);}//sensor=3=image-bathroom
if(((ShowTemp>>11)&1)==1)
{
send_image(13,24,26,24,3);//below zero  3=image-minus
ShowTemp = (~ShowTemp)&0x08FF;//change style of temperature
}
else if (((ShowTemp>>11)&1)==0) 
{
send_image(13,24,26,24,2);//above zero 2=image - plus
}
send_number(39,0,26,64,(ShowTemp>>4)/10);
send_number(66,0,26,64,(ShowTemp>>4)%10);
send_point(92,16,1);//point
send_number(102,0,26,64,get_denum(ShowTemp));}  
void show_time(void){
if (screen_st==0){KS0107_Clrscr(0);screen_st=1;}// clean screen if there was TEMPERATURE
send_number(0,0,26,64,ds_time.hour/10);
send_number(27,0,26,64,ds_time.hour%10);
send_number(75,0,26,64,ds_time.minutes/10);
send_number(102,0,26,64,ds_time.minutes%10);
send_number(53,0,10,16,ds_time.second/10);
send_number(64,0,10,16,ds_time.second%10);
switch (mode%2){
case 0: send_point(59,16,0);
break;
case 1: clear_sector(59,16,10,48);
break;
}
}
ISR (TIMER1_OVF_vect){
mode++;
get_data();
if(mode==24)mode=0;
if(mode>40)mode=0;
TCNT1=0xffff-500;
}
void noraml_mode(void){
if (mode<=8){read_ds_data(&ds_time);convert(&ds_time);show_time();}//экран часов
if ((mode>8)&(mode<16)){T_internal=temp_18b20();show_temp(0,T_internal);}//экран температуры внутри
if ((mode>16)&(mode<24)){show_temp(1,T_external);}//экран температуры снаружи
if ((mode>24)&(mode<32)){show_temp(0,T_kitchen);}//экран кухни
if ((mode>32)&(mode<40)){show_temp(1,T_bathroom);}//экран ванной
}
void set_mode(void){
cli();
if(BUT_2==0){if(cond_2==1){cond_2=0;set++;}}//смена места подстройки с>м>ч

//настройка необходимого параметра
if (BUT_1==0){if(cond_1==1){
	cond_1==0;
	switch(set)
	{
		case 0:
		ds_time.second++;
	if(ds_time.second>59){ds_time.second=0;}
	break;
	
	case 1:
	ds_time.minutes++;
if(ds_time.minutes>59){ds_time.minutes=0;}
break;

case 2:
ds_time.hour++;
	if(ds_time.hour>23){ds_time.hour=0;}
	break;
	}	}	}
	
	switch(set){//эффект мерцания =
	case 0:
	clear_sector(53,0,20,16);
	break;

	case 1:
	clear_sector(75,0,52,64);
	break;
	
	case 2:
	clear_sector(0,0,54,64);
	break;
	}
	show_time();//отображение текущих настроек времени
	if (set>=3){
	cond_2=0;//кнопка 2 была нажата
	setmode=0;// переход в нормальный режим
	set=0;//при входе в режим настроек начинать с секунд
	mode=0;// переход в нормальный режим на экран отображения часов
	ds_write(0,for_ds(ds_time.second));//запись измененных данных с 
	ds_write(1,for_ds(ds_time.minutes));//запись измененных данных м
	ds_write(2,for_ds(ds_time.hour));//запись измененных данных ч
	sei();}
}

 //unsigned int a=0,a1=0,a2=0,a3=0;
int main(void)
{
	
	button_init();//инициализация кнопок
	ds_init();//
	KS0107_Init();
	KS0107_Clrscr(0);
	SPI_MasterInit();
	nRF24L01_init(0b00000011);
	nrf24l01_RX_TX_mode(PRX);
	timer_init();//настройка таймера
	if((BUT_1&BUT_2)==0){ds_write(0,0);ds_write(1,0);ds_write(2,0);cond_1=0;cond_2=0;}//full reset 00:00:00
	sei();
	
while(1){

if(BUT_1==1){cond_1=1;}//условие ненажатости
if(BUT_2==1){cond_2=1;}//условие ненажатости	
		    
if (setmode==0){	//обычный режим
if(BUT_1==0){if(cond_1==1){cond_1=0;mode=26;}}	//показ температуры пола
if(BUT_2==0){if(cond_2==1){cond_2=0;mode=100;setmode=1;read_ds_data(&ds_time);convert(&ds_time);}}// переход в режим настройки setmode=1
noraml_mode();
}  //end if (setmode==0) 
else if(setmode==1){//режим настройки часов
set_mode();
	}//end if(setmode==1)
	}//end while(1)
}//end main
