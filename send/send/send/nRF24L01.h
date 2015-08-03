/*
Библиотека для работы с радиомодулями nRF24L01
согласно nRF24L01_Product_Specification_v2_0.pdf
*/

/* Register Map (регистры) стр 53-58 */
#define CONFIG      0x00 
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17

/* Bit Mnemonics (биты регистров)*/
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1
#define LNA_HCURR   0        
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* Command Команды (стр 46-47) */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF
#define REGISTER_MASK 0x1F

#include <util/delay.h>
#include <avr/interrupt.h>

#define nrf24l01_CSN_PORT PORTB //ножка СSN
#define nrf24l01_CSN_DDR DDRB 
#define nrf24l01_CSN_PIN 2

#define nrf24l01_CE_PORT PORTC //ножка СE
#define nrf24l01_CE_DDR DDRC 
#define nrf24l01_CE_PIN 0

#define nrf24l01_IRQ_PORT PORTB //ножка IRQ
#define nrf24l01_IRQ_DDR DDRB 
#define nrf24l01_IRQ_PIN 0

// макрос уст "1" на ножке порта (x-порт,y-ножка)
#define CLEARBIT(x,y) x&=~(1<<y)
// макрос уст "0" на ножке порта (x-порт,y-ножка)
#define SETBIT(x,y)   x|=1<<y

// удобная замена для управл ножкой CE
#define nrf24l01_CLE_CE CLEARBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
#define nrf24l01_SET_CE SETBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
// удобная замена для управл ножкой CSN
#define nrf24l01_CLE_CSN CLEARBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)
#define nrf24l01_SET_CSN SETBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)
// удобная замена для управл ножкой IRQ
#define nrf24l01_CLE_IRQ CLEARBIT(nrf24l01_IRQ_PORT,nrf24l01_IRQ_PIN)
#define nrf24l01_SET_IRQ SETBIT(nrf24l01_IRQ_PORT,nrf24l01_IRQ_PIN)

/* функция позволяет читать значение регистра
-принимает номер регистра
-возвращает значение в этом регистре
*/
unsigned char nrf24l01_wr_register (char r_n_reg,unsigned char mask1,unsigned char mask2 )
{
  char SR=SREG;//сохраняем значение регистра
  cli(); //запрещаем прерывания
  unsigned char r_regist=0; //переменная для хранения register
  nrf24l01_CLE_CSN;//опускаем линию для начала передачи 
  SPI_MasterTransmit(mask1+r_n_reg);//каманда чтения регистра
  r_regist=SPI_MasterTransmit(mask2);//читаем регистр 
  nrf24l01_SET_CSN;//подымаем линию
  SREG=SR;
  return r_regist; //возвращаем значение Регистра		
}
//макрос чтения значения регистра
#define nrf24l01_readregister(x) nrf24l01_wr_register(x,R_REGISTER,NOP)
//макрос записи значения регистра
#define nrf24l01_writeregister(x,y) nrf24l01_wr_register(x,W_REGISTER,y)

//функция команд без ответа
unsigned char nrf24l01_command (unsigned char M_ask)
{   
	char SR0=SREG;//сохраняем значение регистра
    cli(); //запрещаем прерывания
	unsigned char ST=0;//переменная для хранения STATUS
    nrf24l01_CLE_CSN;//опускаем линию для начала передачи 
	ST=SPI_MasterTransmit(M_ask);//читаем регистр STATUS
	nrf24l01_SET_CSN;//подымаем линию
	SREG=SR0;
	return ST; //возвращаем значение STATUS	
}
//макрос получение регистра статуса
#define nrf24l01_getstatus nrf24l01_command(NOP)
//макрос очистки содержимого регистра передатчика
#define nrf24l01_FLUSH_TX nrf24l01_command(FLUSH_TX)
//макрос очистки содержимого регистра приемника
#define nrf24l01_FLUSH_RX nrf24l01_command(FLUSH_RX)
//макрос повторная отправка отправленного пакета
#define nrf24l01_REUSE_TX_PL nrf24l01_command(REUSE_TX_PL)


/* функция позволяет менять один бит в регистре
-принимает регистр в котором нужно поменять бит
-бит который нужно поменять
-значение на которое нужно поменять
*/
void nrf24l01_sc_bit (unsigned char reg_ister,unsigned char register_bit,char W)
{
unsigned char buf;//переменная временного хранения
buf=nrf24l01_readregister(reg_ister);//читаем значение регистра
if(W)//если нужно установить бит в "1"
buf=buf|(1<<register_bit);//меняем бит в прочитанном регистре
else
buf=buf&(~(1<<register_bit));//меняем бит в прочитанном регистре
nrf24l01_writeregister(reg_ister,buf);//пишем в регистр новое значение	
}

// функция переключается между режимами
#define  PTX 0
#define  PRX 1
void nrf24l01_RX_TX_mode (char z)
{
 //переходим в режим ожидания-1 из режима приемника 
 nrf24l01_CLE_CE;
 //переключаемся между режимами	меняя PRIM_RX бит
 nrf24l01_sc_bit(CONFIG,PRIM_RX,z);
 //переходим в один из режимов
 nrf24l01_SET_CE;
 _delay_us(15);
 if(!z)  nrf24l01_CLE_CE;
 _delay_us(140);//задержка на переход (10+140)
}

//записываем пакет
void nrf24l01_write_data (unsigned char data) 
{
	nrf24l01_CLE_CSN;//опускаем линию для начала передачи
    SPI_MasterTransmit(W_TX_PAYLOAD);//команда чтения
    SPI_MasterTransmit(data);
    nrf24l01_SET_CSN;//подымаем линию
    //сбрасываем прерывание если они разрешены
}

//Отправляем данные в другой радиомодуль
void nrf24l01_Sent_data_Ret (unsigned char data) 
{
   nrf24l01_write_data(data);//записываем пакет в буфер передатчика
   nrf24l01_RX_TX_mode(PTX);//даем команду на передачу пока буфер не опустеет
   nrf24l01_RX_TX_mode(PRX);//после передачи возвращаемся в режим приемника
}

//читаем данные
unsigned char nrf24l01_read_data (void) 
{
	unsigned char i;
	nrf24l01_CLE_CSN;//опускаем линию для начала передачи
    SPI_MasterTransmit(R_RX_PAYLOAD);//команда чтения
    i=SPI_MasterTransmit(NOP);
    nrf24l01_SET_CSN;//подымаем линию
	return i;
}

//(инициализация) переход врежим Ожидания-1
void nRF24L01_init (unsigned char m)
{
	//устанавливаем ножку CSN на выход
    SETBIT(nrf24l01_CSN_DDR,nrf24l01_CSN_PIN);
    //устанавливаем ножку CE на выход
    SETBIT(nrf24l01_CE_DDR,nrf24l01_CE_PIN);
	 //устанавливаем ножку IRG на вход
    CLEARBIT(nrf24l01_IRQ_DDR,nrf24l01_IRQ_PIN); 
	 //устанавливаем ножку IRG 5v
    //SETBIT(nrf24l01_IRQ_PORT,nrf24l01_IRQ_PIN);
	
  nrf24l01_CLE_CSN;//ноль на ножке
  nrf24l01_CLE_CE;//ноль на ножке
  _delay_ms(15);//время на инициализацию (задержка после подачи питания)
  
  /* 
   Пробежимся по всем регистрам начиная с EN_AA
   в индивидуальной настройке значения этих регистров можно менять либо нет
   
     1) настройка автоматического отклика  Разрешён по умолчанию    EN_AA
     2) настройка каналов модуля                                    EN_RXADDR
	 3) выбор блины адреса                                          SETUP_AW
	 4) настройка переотправки данных                               SETUP_RETER
	 5) настройка частоты                                           RF_CH
     6) Определяем скорость передачи + мощность приемника           RF_SETUP
	 7) Регистр статуса (не настраивается)                          STATUS
	 8) Регистр статистики неудачных передач данных                 OBSERVE_TX
	 9) Регистр используется для анализа частоты                    CD
	 10) Регистры хронения адресов на прием данных                  RX_ADDR_P(0-5)
     11) Регистр для хранения адреса на отправку                    TX_ADDR
     12) Регистры настройки полезной нагрузки для каждого канала    RX_PW_P(0-5)
     13) Регистр статуса буферов приемника и передатчика            FIFO_STATUS  
  */
  //12-ый пункт нуждается в настройке
  nrf24l01_sc_bit(RX_PW_P0,0,1);// устанавливаем 8бит полезной нагрузки
  
   //определяем прерывания (вкл/выключаем) их 3-и по умолчанию включены все!
   nrf24l01_sc_bit(CONFIG,MASK_MAX_RT,m&0b00000001);
   nrf24l01_sc_bit(CONFIG,MASK_TX_DS,m&0b00000010);
   nrf24l01_sc_bit(CONFIG,MASK_RX_DR,m&0b00000100);
  
  // nrf24l01_sc_bit(CONFIG,PRIM_RX,1);//устанавливаем режим приемник
   nrf24l01_sc_bit(CONFIG,PWR_UP,1);//устанавливаем бит вкл в единицу (включаем модуль)
   _delay_ms(2);//задержка на настройку
   

   
   //теперь мы в режиме Ожидание-1
}





