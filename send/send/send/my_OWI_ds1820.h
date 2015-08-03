#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define SKIP_ROM 0xCC // ���������� ��������������
#define CONVERT_T 0x44 // �������� �����������
#define READ_SCRATCHPAD 0xBE // ��������� ����������
 
//���������� ���� � ��� � �������� ���������� ����������
#define OWI_PORT PORTD
#define OWI_DDR DDRD
#define OWI_PIN PIND
#define OWI_BIT4 4
#define OWI_BIT3 3
#define OWI_BIT2 2

 
#define OWI_LOW4 OWI_DDR |= 1<<OWI_BIT4//����������� ���� � 0
#define OWI_LOW3 OWI_DDR |= 1<<OWI_BIT3//����������� ���� � 0
#define OWI_LOW2 OWI_DDR |= 1<<OWI_BIT2//����������� ���� � 0

#define OWI_HIGH4 OWI_DDR &= ~(1<<OWI_BIT4)//��������� ����
#define OWI_HIGH3 OWI_DDR &= ~(1<<OWI_BIT3)//��������� ����
#define OWI_HIGH2 OWI_DDR &= ~(1<<OWI_BIT2)//��������� ����

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//������� ���������� ���� �� ���������� �� ����
char OWI_find4(void)
{
char SR04=SREG;//��������� �������� ��������
cli(); //��������� ����������
char device4;
OWI_LOW4; //����������� ���� � 0
_delay_us(485);//���� ������� 480���
OWI_HIGH4;//��������� ����
_delay_us(65);//���� ������� 60��� � ������� ��� �� ����
 
if((OWI_PIN & (1<<OWI_BIT4)) ==0x00)//������� ���� �� �����
device4 = 1; //�� ���� ���� ����������
else
device4 = 0; //�� ���� ��� ����������
SREG=SR04;//���������� �������� ���� ��������
_delay_us(420);//���� ���������� ����� �� 480���
return device4;
}
char OWI_find3(void)
{
	char SR0=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char device;
	OWI_LOW3; //����������� ���� � 0
	_delay_us(485);//���� ������� 480���
	OWI_HIGH3;//��������� ����
	_delay_us(65);//���� ������� 60��� � ������� ��� �� ����
	
	if((OWI_PIN & (1<<OWI_BIT3)) ==0x00)//������� ���� �� �����
	device = 1; //�� ���� ���� ����������
	else
	device = 0; //�� ���� ��� ����������
	SREG=SR0;//���������� �������� ���� ��������
	_delay_us(420);//���� ���������� ����� �� 480���
	return device;
}
char OWI_find2(void)
{
	char SR0=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char device;
	OWI_LOW2; //����������� ���� � 0
	_delay_us(485);//���� ������� 480���
	OWI_HIGH2;//��������� ����
	_delay_us(65);//���� ������� 60��� � ������� ��� �� ����
	
	if((OWI_PIN & (1<<OWI_BIT2)) ==0x00)//������� ���� �� �����
	device = 1; //�� ���� ���� ����������
	else
	device = 0; //�� ���� ��� ����������
	SREG=SR0;//���������� �������� ���� ��������
	_delay_us(420);//���� ���������� ����� �� 480���
	return device;
}
 
// ������� ���������� ��� �� ����������
void OWI_write_bit4 (char bit4)
{
char SR14=SREG;//��������� �������� ��������
cli(); //��������� ����������
OWI_LOW4;//���������� "0"
_delay_us(2);
if(bit4) OWI_HIGH4; //��������� ����
_delay_us(65);
OWI_HIGH4; //��������� ����
SREG=SR14;//���������� �������� ���� ��������
}
void OWI_write_bit3 (char bit)
{
	char SR1=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	OWI_LOW3;//���������� "0"
	_delay_us(2);
	if(bit) OWI_HIGH3; //��������� ����
	_delay_us(65);
	OWI_HIGH3; //��������� ����
	SREG=SR1;//���������� �������� ���� ��������
}
void OWI_write_bit2 (char bit)
{
	char SR1=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	OWI_LOW2;//���������� "0"
	_delay_us(2);
	if(bit) OWI_HIGH2; //��������� ����
	_delay_us(65);
	OWI_HIGH2; //��������� ����
	SREG=SR1;//���������� �������� ���� ��������
}
 
//������� �������� ���� �� ����������
void OWI_write_byte4(unsigned char c){
char i;
for(i = 0; i < 8; i++){//� ����� �������� �������
if((c & (1<<i)) == 1<<i)//���� ���=1 �������� 1
OWI_write_bit4(1);
else//����� �������� 0
OWI_write_bit4(0);
}}
void OWI_write_byte3(unsigned char c){
	 char i;
	 for(i = 0; i < 8; i++){//� ����� �������� �������
	 	 if((c & (1<<i)) == 1<<i)//���� ���=1 �������� 1
		 OWI_write_bit3(1);
		 else//����� �������� 0
		 OWI_write_bit3(0);
	 }} 
void OWI_write_byte2(unsigned char c){
		 char i;
		 for(i = 0; i < 8; i++){//� ����� �������� �������
		 	 if((c & (1<<i)) == 1<<i)//���� ���=1 �������� 1
			 OWI_write_bit2(1);
			 else//����� �������� 0
			 OWI_write_bit2(0);
		 }}

//������� ������ ������ ����
char OWI_read_bit4 (void)
{
char SR24=SREG;//��������� �������� ��������
cli(); //��������� ����������
char OWI_rbit4; //���������� �������� ����
OWI_LOW4;//���������� "0"
_delay_us(2);
OWI_HIGH4; //��������� ����
_delay_us(13);
OWI_rbit4=(OWI_PIN & (1<<OWI_BIT4))>>OWI_BIT4;//������� ���������� 05.2014 (���������� ������)
_delay_us(45);//�������� �� ��������� ����-�����
SREG=SR24;//���������� �������� ���� ��������
return OWI_rbit4;
}
char OWI_read_bit3 (void)
 {
	 char SR2=SREG;//��������� �������� ��������
	 cli(); //��������� ����������
	 char OWI_rbit; //���������� �������� ����
	 OWI_LOW3;//���������� "0"
	 _delay_us(2);
	 OWI_HIGH3; //��������� ����
	 _delay_us(13);
	 OWI_rbit=(OWI_PIN & (1<<OWI_BIT3))>>OWI_BIT3;//������� ���������� 05.2014 (���������� ������)
	 _delay_us(45);//�������� �� ��������� ����-�����
	 SREG=SR2;//���������� �������� ���� ��������
	 return OWI_rbit;
 }
char OWI_read_bit2 (void)
 {
	 char SR2=SREG;//��������� �������� ��������
	 cli(); //��������� ����������
	 char OWI_rbit; //���������� �������� ����
	 OWI_LOW2;//���������� "0"
	 _delay_us(2);
	 OWI_HIGH2; //��������� ����
	 _delay_us(13);
	 OWI_rbit=(OWI_PIN & (1<<OWI_BIT2))>>OWI_BIT2;//������� ���������� 05.2014 (���������� ������)
	 _delay_us(45);//�������� �� ��������� ����-�����
	 SREG=SR2;//���������� �������� ���� ��������
	 return OWI_rbit;
 }
 
//������� ������ ���� ���� � ���������� 1-wire
unsigned char OWI_read_byte4()
{
char data4=0,i;
for(i = 0; i < 8; i++) //� ����� ������� ��� �� ���� � ��������� ��������
data4|= OWI_read_bit4()<<i;//������ ��������� ���
return data4;
}
unsigned char OWI_read_byte3()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //� ����� ������� ��� �� ���� � ��������� ��������
	data|= OWI_read_bit3()<<i;//������ ��������� ���
	return data;
}
unsigned char OWI_read_byte2()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //� ����� ������� ��� �� ���� � ��������� ��������
	data|= OWI_read_bit2()<<i;//������ ��������� ���
	return data;
}
 
//������� ����������� ���������� � ������� 18b20 ������ � �����������

 
 int  temp_18b20_4() {
	 unsigned char B4=0;
	 int ds18_temp4=0;
	 if(OWI_find4()==1)//���� ���� ���������� �� ����
	 {
		 OWI_write_byte4(SKIP_ROM);//���������� ROM ���, �� �����, ��� � ��� ���� ���������� ��� �������� ����
		 OWI_write_byte4(CONVERT_T);//������������� �����������
		 _delay_ms(750);//�������������� � 12 ������ ������ �������� 750ms
		 OWI_find4();//����� �������� Presence � Reset
		 OWI_write_byte4(SKIP_ROM);
		 OWI_write_byte4(READ_SCRATCHPAD);//�������� ����� ��������(� 18b20 � ������ ���� ���������� �����������)
		 
		 B4 = OWI_read_byte4(); //������ ��� LS
		 ds18_temp4 = OWI_read_byte4(); //������ ��� MS
		 ds18_temp4 = (ds18_temp4<<8)|B4;//��������� ���� � ������������������ MS ����� LS
	 }
	 // else return ����� ������� ������� ����� ��� ������� ���
	 return ds18_temp4;//���������� int (MS,LS)
 }
 int  temp_18b20_3()
 {
	 unsigned char B;
	 unsigned int ds18_temp=0;
	 if(OWI_find3()==1)//���� ���� ���������� �� ����
	 {
		 OWI_write_byte3(SKIP_ROM);//���������� ROM ���, �� �����, ��� � ��� ���� ���������� ��� �������� ����
		 OWI_write_byte3(CONVERT_T);//������������� �����������
		 _delay_ms(750);//�������������� � 12 ������ ������ �������� 750ms
		 OWI_find3();//����� �������� Presence � Reset
		 OWI_write_byte3(SKIP_ROM);
		 OWI_write_byte3(READ_SCRATCHPAD);//�������� ����� ��������(� 18b20 � ������ ���� ���������� �����������)
		 
		 B = OWI_read_byte3(); //������ ��� LS
		 ds18_temp = OWI_read_byte3(); //������ ��� MS
		 ds18_temp = (ds18_temp<<8)|B;//��������� ���� � ������������������ MS ����� LS
	 }
	 // else return ����� ������� ������� ����� ��� ������� ���
	 return ds18_temp;//���������� int (MS,LS)
 }
int  temp_18b20_2()
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find2()==1)//���� ���� ���������� �� ����
	{
		OWI_write_byte2(SKIP_ROM);//���������� ROM ���, �� �����, ��� � ��� ���� ���������� ��� �������� ����
		OWI_write_byte2(CONVERT_T);//������������� �����������
		_delay_ms(750);//�������������� � 12 ������ ������ �������� 750ms
		OWI_find2();//����� �������� Presence � Reset
		OWI_write_byte2(SKIP_ROM);
		OWI_write_byte2(READ_SCRATCHPAD);//�������� ����� ��������(� 18b20 � ������ ���� ���������� �����������)
		
		B = OWI_read_byte2(); //������ ��� LS
		ds18_temp = OWI_read_byte2(); //������ ��� MS
		ds18_temp = (ds18_temp<<8)|B;//��������� ���� � ������������������ MS ����� LS
	}
	// else return ����� ������� ������� ����� ��� ������� ���
	return ds18_temp;//���������� int (MS,LS)
}



/*
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define SKIP_ROM 0xCC // ���������� ��������������
#define CONVERT_T 0x44 // �������� �����������
#define READ_SCRATCHPAD 0xBE // ��������� ����������

//���������� ���� � ��� � �������� ���������� ����������
#define OWI_PORT PORTD
#define OWI_DDR DDRD
#define OWI_PIN PIND

#define OWI_LOW OWI_DDR |= 1<<OWI_BIT//����������� ���� � 0
#define OWI_HIGH OWI_DDR &= ~(1<<OWI_BIT)//��������� ����

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//������� ���������� ���� �� ���������� �� ����
char OWI_find(char pin)
{
	char SR0=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char device;
	OWI_DDR |= 1<<pin; //����������� ���� � 0
	_delay_us(485);//���� ������� 480���
	OWI_DDR &= ~(1<<pin);//��������� ����
	_delay_us(65);//���� ������� 60��� � ������� ��� �� ����
	
	if((OWI_PIN & (1<<pin)) ==0x00)//������� ���� �� �����
	device = 1; //�� ���� ���� ����������
	else
	device = 0; //�� ���� ��� ����������
	SREG=SR0;//���������� �������� ���� ��������
	_delay_us(420);//���� ���������� ����� �� 480���
	return device;
}

// ������� ���������� ��� �� ����������
void OWI_write_bit (char bit, char pin)
{
	char SR1=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	OWI_DDR |= 1<<pin;//���������� "0"
	_delay_us(2);
	if(bit)	OWI_DDR &= ~(1<<pin); //��������� ����
	_delay_us(65);
	OWI_DDR &= ~(1<<pin); //��������� ����
	SREG=SR1;//���������� �������� ���� ��������
}

//������� �������� ���� �� ����������
void OWI_write_byte(unsigned char c,char pin)
{
	char i;
	for(i = 0; i < 8; i++)//� ����� �������� �������
	{
		if((c & (1<<i)) == 1<<i)//���� ���=1 �������� 1
		OWI_write_bit(1,pin);
		else//����� �������� 0
		OWI_write_bit(0,pin);
	}
}

//������� ������ ������ ����
char OWI_read_bit (char pin)
{
	char SR2=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char OWI_rbit; //���������� �������� ����
	OWI_DDR |= 1<<pin;//���������� "0"
	_delay_us(2);
	OWI_DDR &= ~(1<<pin); //��������� ����
	_delay_us(13);
	OWI_rbit=(OWI_PIN& (1<<pin))>>pin;//������� ���������� 05.2014 (���������� ������)
	_delay_us(45);//�������� �� ��������� ����-�����
	SREG=SR2;//���������� �������� ���� ��������
	return OWI_rbit;
}

//������� ������ ���� ���� � ���������� 1-wire
unsigned char OWI_read_byte(char pin)
{
	char data=0,i;
	for(i = 0; i < 8; i++) //� ����� ������� ��� �� ���� � ��������� ��������
	data|= OWI_read_bit(pin)<<i;//������ ��������� ���
	return data;
}

//������� ����������� ���������� � ������� 18b20 ������ � �����������
int  temp_18b20(char pin)
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find(pin)==1)//���� ���� ���������� �� ����
	{
		OWI_write_byte(SKIP_ROM,pin);//���������� ROM ���, �� �����, ��� � ��� ���� ���������� ��� �������� ����
		OWI_write_byte(CONVERT_T,pin);//������������� �����������
		_delay_ms(750);//�������������� � 12 ������ ������ �������� 750ms
		OWI_find(pin);//����� �������� Presence � Reset
		OWI_write_byte(SKIP_ROM,pin);
		OWI_write_byte(READ_SCRATCHPAD,pin);//�������� ����� ��������(� 18b20 � ������ ���� ���������� �����������)
		
		B = OWI_read_byte(pin); //������ ��� LS
		ds18_temp = OWI_read_byte(pin); //������ ��� MS
		ds18_temp = (ds18_temp<<8)|B;//��������� ���� � ������������������ MS ����� LS
	}
	// else return ����� ������� ������� ����� ��� ������� ���
	return ds18_temp;//���������� int (MS,LS)
}
*/

/*
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SKIP_ROM 0xCC // ���������� ��������������
#define CONVERT_T 0x44 // �������� �����������
#define READ_SCRATCHPAD 0xBE // ��������� ����������

//���������� ���� � ��� � �������� ���������� ����������
#define OWI_PORT PORTB
#define OWI_DDR DDRB
#define OWI_PIN PINB
#define OWI_BIT 0

#define OWI_LOW OWI_DDR |= 1<<OWI_BIT//����������� ���� � 0
#define OWI_HIGH OWI_DDR &= ~(1<<OWI_BIT)//��������� ����

#define get_denum(x) ( (((x>>0)&1)*0.0625)+ (((x>>1)&1)*0.125) + (((x>>2)&1)*0.25) + (((x>>3)&1)*0.5))*10

//������� ���������� ���� �� ���������� �� ����
char OWI_find(void)
{
	char SR0=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char device;
	OWI_LOW; //����������� ���� � 0
	_delay_us(485);//���� ������� 480���
	OWI_HIGH;//��������� ����
	_delay_us(65);//���� ������� 60��� � ������� ��� �� ����
	
	if((OWI_PIN & (1<<OWI_BIT)) ==0x00)//������� ���� �� �����
	device = 1; //�� ���� ���� ����������
	else
	device = 0; //�� ���� ��� ����������
	SREG=SR0;//���������� �������� ���� ��������
	_delay_us(420);//���� ���������� ����� �� 480���
	return device;
}

// ������� ���������� ��� �� ����������
void OWI_write_bit (char bit)
{
	char SR1=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	OWI_LOW;//���������� "0"
	_delay_us(2);
	if(bit) OWI_HIGH; //��������� ����
	_delay_us(65);
	OWI_HIGH; //��������� ����
	SREG=SR1;//���������� �������� ���� ��������
}

//������� �������� ���� �� ����������
void OWI_write_byte(unsigned char c)
{
	char i;
	for(i = 0; i < 8; i++)//� ����� �������� �������
	{
		if((c & (1<<i)) == 1<<i)//���� ���=1 �������� 1
		OWI_write_bit(1);
		else//����� �������� 0
		OWI_write_bit(0);
	}
}

//������� ������ ������ ����
char OWI_read_bit (void)
{
	char SR2=SREG;//��������� �������� ��������
	cli(); //��������� ����������
	char OWI_rbit; //���������� �������� ����
	OWI_LOW;//���������� "0"
	_delay_us(2);
	OWI_HIGH; //��������� ����
	_delay_us(13);
	OWI_rbit=(OWI_PIN & (1<<OWI_BIT))>>OWI_BIT;//������� ���������� 05.2014 (���������� ������)
	_delay_us(45);//�������� �� ��������� ����-�����
	SREG=SR2;//���������� �������� ���� ��������
	return OWI_rbit;
}

//������� ������ ���� ���� � ���������� 1-wire
unsigned char OWI_read_byte()
{
	char data=0,i;
	for(i = 0; i < 8; i++) //� ����� ������� ��� �� ���� � ��������� ��������
	data|= OWI_read_bit()<<i;//������ ��������� ���
	return data;
}

//������� ����������� ���������� � ������� 18b20 ������ � �����������
int  temp_18b20()
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find()==1)//���� ���� ���������� �� ����
	{
		OWI_write_byte(SKIP_ROM);//���������� ROM ���, �� �����, ��� � ��� ���� ���������� ��� �������� ����
		OWI_write_byte(CONVERT_T);//������������� �����������
		_delay_ms(750);//�������������� � 12 ������ ������ �������� 750ms
		OWI_find();//����� �������� Presence � Reset
		OWI_write_byte(SKIP_ROM);
		OWI_write_byte(READ_SCRATCHPAD);//�������� ����� ��������(� 18b20 � ������ ���� ���������� �����������)
		
		B = OWI_read_byte(); //������ ��� LS
		ds18_temp = OWI_read_byte(); //������ ��� MS
		ds18_temp = (ds18_temp<<8)|B;//��������� ���� � ������������������ MS ����� LS
	}
	// else return ����� ������� ������� ����� ��� ������� ���
	return ds18_temp;//���������� int (MS,LS)
}

//������� ��� �� temp_18b20() � ������� ���
char convert (unsigned int td)
{
	char dat=td>>4;//����� � ��������� �������� �����
	return dat;// (����,2�6,2�5,2�4,2�3,2�2,2�1,2�0)
}*/