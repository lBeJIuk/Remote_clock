
//ножки интерфейса SPI в Атмега32
#define SS    2
#define MOSI  3
#define MISO  4
#define SCK   5

//порт интерфейса  SPI в Атмега32
#define SPI_DDR DDRB

// Функция инициализации мастера шины SPI
void SPI_MasterInit(void)
{
// Установка выводов SPI на вывод	
SPI_DDR|=(1<<SS)|(1<<MOSI)|(1<<MISO)|(1<<SCK);
SPCR = (1<<SPE)|(1<<MSTR);//Включение SPI, режима ведущего
}

/* Функция передачи байта данных outData. Ожидает окончания
передачи и возвращает принятый по ножке MOSI байт */
unsigned char SPI_MasterTransmit(char outData)
{
SPDR = outData;// Начало передачи
while(!(SPSR & (1<<SPIF)));// Ожидание окончания передачи
return SPDR; //возвращаем принятый байт
}