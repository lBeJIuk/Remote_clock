
//библиотека для работы с LCD_124x64

#include <util/delay.h>
#include "image.c"
//опр порт шины данных DB0-DB7
#define KS0107_DB_DDR   DDRD
#define KS0107_DB_PORT  PORTD
#define KS0107_DB_PIN   PIND

//опр порт где находятся управл ножки
#define KS0107_DC_DDR   DDRC
#define KS0107_DC_PORT  PORTC

//опр управляющие выводы
#define   KS0107_CS1        0
#define   KS0107_CS2        1
#define   KS0107_RW         2
#define   KS0107_DI         3
#define   KS0107_E          4


//посылка данных дисплею
void KS0107_Sent_Data(unsigned char g)
{
	KS0107_DB_PORT = g;//данные на шину данных
	KS0107_DC_PORT |= (1<<KS0107_DI);//устанавливаем "1" на DI - передаются данные
	_delay_us(1);
	KS0107_DC_PORT |= (1<<KS0107_E);//устанавливаем "1" на E
	_delay_us(1);
	KS0107_DC_PORT &= ~(1<<KS0107_E);// сбрасываем Е
	KS0107_DC_PORT &= ~(1<<KS0107_DI);// сбрасываем DI
	_delay_us(1);
}

//посылка команды дисплею
void KS0107_Sent_Com(unsigned char m)
{
	KS0107_DB_PORT = m;//данные на шину данных
	_delay_us(1);
	KS0107_DC_PORT |= (1<<KS0107_E);//устанавливаем "1" на E
	_delay_us(20);
	KS0107_DC_PORT &= ~(1<<KS0107_E);// сбрасываем Е
	_delay_us(20);
}

//вкл  и выбрать отображение или нет
void KS0107_Screen_ON_OFF (char z)
{
  if(z)//разрешаем отображение
  KS0107_Sent_Com(0b00111111); 
  else//запрещаем отображение
  KS0107_Sent_Com(0b00111110); 
}

//макрос выбора адреса Y
#define  KS0107_Set_Address_Y(y) KS0107_Sent_Com(0x40+y)
//макрос выбора адреса X
#define  KS0107_Set_Address_X(x) KS0107_Sent_Com(0xB8+x)
//макрос для скролинга
#define  KS0107_Skrol(v) KS0107_Sent_Com(0xC0+v)

//установка координаты. По х - страница, по y - координата
void KS0107_GotoXY(unsigned char x, unsigned char y)
{
 if (x<64)//если регистор в первом чипе
  {
  KS0107_DC_PORT &=~(1<<KS0107_CS1);//включаем 1ый чип
 // KS0107_DC_PORT |=(1<<KS0107_CS1);
    KS0107_DC_PORT |=(1<<KS0107_CS2);//выключаем 2ый чип	
   //KS0107_DC_PORT &=~(1<<KS0107_CS2);  
  } 
  else //значит надо включить 2ой чип
  {
  KS0107_DC_PORT |=(1<<KS0107_CS1);//выключаем 1ый чип
  //KS0107_DC_PORT &=~(1<<KS0107_CS1);
  KS0107_DC_PORT &=~(1<<KS0107_CS2);//включаем 2ый чип	
  //KS0107_DC_PORT |=(1<<KS0107_CS2);
  x-=64;//приводим к общему виду
  }  
KS0107_Set_Address_X(y/8);//выбираем страницу  	
KS0107_Set_Address_Y(x);//выбираем адреса Y (или координата X)
}

//функция заливки ЖКИ
void KS0107_Clrscr (unsigned char s)
{
 KS0107_DC_PORT &=~(1<<KS0107_CS1);//включаем 1ый чип
 //KS0107_DC_PORT |=(1<<KS0107_CS1);
 KS0107_DC_PORT &=~(1<<KS0107_CS2);//включаем 2ый чип		
 //KS0107_DC_PORT |=(1<<KS0107_CS2);
char t,i;//переменные для циклов
for(i=0;i<8;i++)
{KS0107_Set_Address_X(i);//переход на след стр
for(t=0;t<64;t++)//записываем всю строку	
KS0107_Sent_Data(s);//записываем байт заливки		
}
}

//функция инициализации ЖКИ
void KS0107_Init (void)
{
KS0107_DB_DDR=0xFF;//порт шины данных на выход
KS0107_DC_DDR|= (1<<KS0107_DI)|(1<<KS0107_RW)|//ножки шины управления на выход
 
  (1<<KS0107_E)|(1<<KS0107_CS1)|(1<<KS0107_CS2);

KS0107_Clrscr(0);//заливка дисплея
KS0107_Skrol(0);//установка начальной строки
KS0107_Screen_ON_OFF(1);//включаем жки и разр отобр
}

//чтение данных из ЖКИ
unsigned char KS0107_Read_Data(void)
{
  unsigned char t=0;//переменная для хранения прочитанных данных
 
  KS0107_DB_PORT = 0x00; // Z-состояние  
  KS0107_DB_DDR = 0x00; //порт данных на ввод 
  
  KS0107_DC_PORT |=(1<<KS0107_RW);//пишем в ЖКИ  
//  KS0107_DC_PORT &=~(1<<KS0107_RW);
  
  KS0107_DC_PORT |=(1<<KS0107_DI);//данные
  _delay_us(1); 
  KS0107_DC_PORT |= (1<<KS0107_E);//устанавливаем "1" на E
  _delay_us(1);
  KS0107_DC_PORT &=~(1<<KS0107_E);// сбрасываем Е
  _delay_us(1);
  KS0107_DC_PORT |= (1<<KS0107_E);//устанавливаем "1" на E
   _delay_us(5);
  t=KS0107_DB_PIN;//читаем шину данных
  _delay_us(1);
  KS0107_DC_PORT &=~(1<<KS0107_E);// сбрасываем Е  

 return t;
}

//функция рисования точки в координатах X и Y
// X-горизонталь, Y-вертикаль
void KS0107_Point (unsigned char x1, unsigned char y1)
{
 unsigned char ddot;//переменная для хранения прочитанных данных
 KS0107_GotoXY(x1,y1);//переходим в нужн регистр
 
 ddot=KS0107_Read_Data();//читаем выбранный регистр	

 ddot|=(1<<(y1%8));//устанавливаем необходимый нам бит	

  KS0107_DC_PORT &=~(1<<KS0107_DI);//данные 
  
 KS0107_DC_PORT &=~(1<<KS0107_RW);//сбрасываем чтение
 //  KS0107_DC_PORT|=1<<KS0107_RW;
	
  _delay_us(1);
  KS0107_DB_DDR = 0xFF; //порт данных на выход  
  
//после чтения адрес увелич +1 поэт снова
KS0107_GotoXY(x1,y1);//выбираем адреса YиX

KS0107_Sent_Data(ddot);//записываем байт с установленным битом
}
void send_point(char x,char y,char mode)
{
int d=0;
char temp;
for (int DY=0;DY<6;DY++){
for(int DX=0;DX<10;DX++){
KS0107_GotoXY(x+DX,y+(DY*8));
temp=pgm_read_byte(&point[mode][1*d]);
KS0107_Sent_Data(temp);
d++;}
}
}
void clear_sector(char x,char y,char width, char height)
{
for (int DY=0;DY<height/8;DY++){
for(int DX=0;DX<width;DX++){
KS0107_GotoXY(x+DX,y+(DY*8));
KS0107_Sent_Data(0);
		}
		}
}
void send_number(char x,char y,char width,char height,char n)
{
char d=0;
char temp;
for (char DY=0;DY<(height/8);DY++){
for(char DX=0;DX<width;DX++){
switch (height){
case 64:
temp=pgm_read_byte(&number_64pt[n][1*d]);
break;

case 16:
temp=pgm_read_byte(&number_16pt[n][1*d]);
break;
}
KS0107_GotoXY(x+DX,y+(DY*8));
KS0107_Sent_Data(temp);
d++;
}
}
}
void send_image(char x,char y,char width,char height,char image)
{
	char d=0;
	char temp;
	for (char DY=0;DY<(height/8);DY++){
		for(char DX=0;DX<width;DX++){
			switch (image){
				case 0://in
				temp=pgm_read_byte(&int_image[1*d]);
				break;
				case 1://out
				temp=pgm_read_byte(&ext_image[1*d]);
				break;
				case 2://plus
				temp=pgm_read_byte(&plus_image[1*d]);
				break;
				case 3://minus
				temp=pgm_read_byte(&minus_image[1*d]);
				break;
				case 4://termometr
				temp=pgm_read_byte(&termometr_image[1*d]);
				break;
			}
			KS0107_GotoXY(x+DX,y+(DY*8));
			KS0107_Sent_Data(temp);
		d++;}
		}
		}