
//����� ���������� SPI � ������32
#define SS    2
#define MOSI  3
#define MISO  4
#define SCK   5

//���� ����������  SPI � ������32
#define SPI_DDR DDRB

// ������� ������������� ������� ���� SPI
void SPI_MasterInit(void)
{
// ��������� ������� SPI �� �����	
SPI_DDR|=(1<<SS)|(1<<MOSI)|(1<<MISO)|(1<<SCK);
SPCR = (1<<SPE)|(1<<MSTR);//��������� SPI, ������ ��������
}

/* ������� �������� ����� ������ outData. ������� ���������
�������� � ���������� �������� �� ����� MOSI ���� */
unsigned char SPI_MasterTransmit(char outData)
{
SPDR = outData;// ������ ��������
while(!(SPSR & (1<<SPIF)));// �������� ��������� ��������
return SPDR; //���������� �������� ����
}