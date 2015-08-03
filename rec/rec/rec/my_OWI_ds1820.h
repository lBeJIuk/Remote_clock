#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
#define SKIP_ROM 0xCC // ���������� ��������������
#define CONVERT_T 0x44 // �������� �����������
#define READ_SCRATCHPAD 0xBE // ��������� ����������

#define SEARCH_ROM 0xF0 // ��������� ��� ���� ���������
#define READ_ROM 0x33 //��������� ��� (���� ���� ����������)
#define MATCH_ROM 0x55 // ��������� � ����������� ����������

//���������� ���� � ��� � �������� ���������� ����������
#define OWI_PORT PORTC
#define OWI_DDR DDRC
#define OWI_PIN PINC
#define OWI_BIT 5
 
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
}


char  ROM_18b20()
{
	char data[8];
	if(OWI_find()==1)//���� ���� ���������� �� ����
	{
		OWI_write_byte(READ_ROM);//ROM ���
		OWI_find();//����� �������� Presence � Reset
		for (char i=0;i<8;i++){
		data[i] = OWI_read_byte();
		UDR0=data[i];
		_delay_ms(10); }
	
	return data[8];//���������� int (MS,LS)
}
}