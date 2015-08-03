
//���������� ��� ������ � LCD_124x64

#include <util/delay.h>
#include "image.c"
//��� ���� ���� ������ DB0-DB7
#define KS0107_DB_DDR   DDRD
#define KS0107_DB_PORT  PORTD
#define KS0107_DB_PIN   PIND

//��� ���� ��� ��������� ������ �����
#define KS0107_DC_DDR   DDRC
#define KS0107_DC_PORT  PORTC

//��� ����������� ������
#define   KS0107_CS1        0
#define   KS0107_CS2        1
#define   KS0107_RW         2
#define   KS0107_DI         3
#define   KS0107_E          4


//������� ������ �������
void KS0107_Sent_Data(unsigned char g)
{
	KS0107_DB_PORT = g;//������ �� ���� ������
	KS0107_DC_PORT |= (1<<KS0107_DI);//������������� "1" �� DI - ���������� ������
	_delay_us(1);
	KS0107_DC_PORT |= (1<<KS0107_E);//������������� "1" �� E
	_delay_us(1);
	KS0107_DC_PORT &= ~(1<<KS0107_E);// ���������� �
	KS0107_DC_PORT &= ~(1<<KS0107_DI);// ���������� DI
	_delay_us(1);
}

//������� ������� �������
void KS0107_Sent_Com(unsigned char m)
{
	KS0107_DB_PORT = m;//������ �� ���� ������
	_delay_us(1);
	KS0107_DC_PORT |= (1<<KS0107_E);//������������� "1" �� E
	_delay_us(20);
	KS0107_DC_PORT &= ~(1<<KS0107_E);// ���������� �
	_delay_us(20);
}

//���  � ������� ����������� ��� ���
void KS0107_Screen_ON_OFF (char z)
{
  if(z)//��������� �����������
  KS0107_Sent_Com(0b00111111); 
  else//��������� �����������
  KS0107_Sent_Com(0b00111110); 
}

//������ ������ ������ Y
#define  KS0107_Set_Address_Y(y) KS0107_Sent_Com(0x40+y)
//������ ������ ������ X
#define  KS0107_Set_Address_X(x) KS0107_Sent_Com(0xB8+x)
//������ ��� ���������
#define  KS0107_Skrol(v) KS0107_Sent_Com(0xC0+v)

//��������� ����������. �� � - ��������, �� y - ����������
void KS0107_GotoXY(unsigned char x, unsigned char y)
{
 if (x<64)//���� �������� � ������ ����
  {
  KS0107_DC_PORT &=~(1<<KS0107_CS1);//�������� 1�� ���
 // KS0107_DC_PORT |=(1<<KS0107_CS1);
    KS0107_DC_PORT |=(1<<KS0107_CS2);//��������� 2�� ���	
   //KS0107_DC_PORT &=~(1<<KS0107_CS2);  
  } 
  else //������ ���� �������� 2�� ���
  {
  KS0107_DC_PORT |=(1<<KS0107_CS1);//��������� 1�� ���
  //KS0107_DC_PORT &=~(1<<KS0107_CS1);
  KS0107_DC_PORT &=~(1<<KS0107_CS2);//�������� 2�� ���	
  //KS0107_DC_PORT |=(1<<KS0107_CS2);
  x-=64;//�������� � ������ ����
  }  
KS0107_Set_Address_X(y/8);//�������� ��������  	
KS0107_Set_Address_Y(x);//�������� ������ Y (��� ���������� X)
}

//������� ������� ���
void KS0107_Clrscr (unsigned char s)
{
 KS0107_DC_PORT &=~(1<<KS0107_CS1);//�������� 1�� ���
 //KS0107_DC_PORT |=(1<<KS0107_CS1);
 KS0107_DC_PORT &=~(1<<KS0107_CS2);//�������� 2�� ���		
 //KS0107_DC_PORT |=(1<<KS0107_CS2);
char t,i;//���������� ��� ������
for(i=0;i<8;i++)
{KS0107_Set_Address_X(i);//������� �� ���� ���
for(t=0;t<64;t++)//���������� ��� ������	
KS0107_Sent_Data(s);//���������� ���� �������		
}
}

//������� ������������� ���
void KS0107_Init (void)
{
KS0107_DB_DDR=0xFF;//���� ���� ������ �� �����
KS0107_DC_DDR|= (1<<KS0107_DI)|(1<<KS0107_RW)|//����� ���� ���������� �� �����
 
  (1<<KS0107_E)|(1<<KS0107_CS1)|(1<<KS0107_CS2);

KS0107_Clrscr(0);//������� �������
KS0107_Skrol(0);//��������� ��������� ������
KS0107_Screen_ON_OFF(1);//�������� ��� � ���� �����
}

//������ ������ �� ���
unsigned char KS0107_Read_Data(void)
{
  unsigned char t=0;//���������� ��� �������� ����������� ������
 
  KS0107_DB_PORT = 0x00; // Z-���������  
  KS0107_DB_DDR = 0x00; //���� ������ �� ���� 
  
  KS0107_DC_PORT |=(1<<KS0107_RW);//����� � ���  
//  KS0107_DC_PORT &=~(1<<KS0107_RW);
  
  KS0107_DC_PORT |=(1<<KS0107_DI);//������
  _delay_us(1); 
  KS0107_DC_PORT |= (1<<KS0107_E);//������������� "1" �� E
  _delay_us(1);
  KS0107_DC_PORT &=~(1<<KS0107_E);// ���������� �
  _delay_us(1);
  KS0107_DC_PORT |= (1<<KS0107_E);//������������� "1" �� E
   _delay_us(5);
  t=KS0107_DB_PIN;//������ ���� ������
  _delay_us(1);
  KS0107_DC_PORT &=~(1<<KS0107_E);// ���������� �  

 return t;
}

//������� ��������� ����� � ����������� X � Y
// X-�����������, Y-���������
void KS0107_Point (unsigned char x1, unsigned char y1)
{
 unsigned char ddot;//���������� ��� �������� ����������� ������
 KS0107_GotoXY(x1,y1);//��������� � ���� �������
 
 ddot=KS0107_Read_Data();//������ ��������� �������	

 ddot|=(1<<(y1%8));//������������� ����������� ��� ���	

  KS0107_DC_PORT &=~(1<<KS0107_DI);//������ 
  
 KS0107_DC_PORT &=~(1<<KS0107_RW);//���������� ������
 //  KS0107_DC_PORT|=1<<KS0107_RW;
	
  _delay_us(1);
  KS0107_DB_DDR = 0xFF; //���� ������ �� �����  
  
//����� ������ ����� ������ +1 ���� �����
KS0107_GotoXY(x1,y1);//�������� ������ Y�X

KS0107_Sent_Data(ddot);//���������� ���� � ������������� �����
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