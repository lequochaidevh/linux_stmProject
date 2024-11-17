#ifndef __DHT11_H
#define __DHT11_H
#if defined (__cplusplus)
extern "C" {
#endif

#include "../sys/sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
#define DHT11_IO_IN()  {GPIOA->CRL&=0X0FFFFFFF;GPIOA->CRL|=8<<28;}
#define DHT11_IO_OUT() {GPIOA->CRL&=0X0FFFFFFF;GPIOA->CRL|=3<<28;}
//IO
#define	DHT11_DQ_OUT PAout(7)
#define	DHT11_DQ_IN  PAin(7)



//#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
//#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

//#define	DHT11_DQ_OUT PBout(9)
//#define	DHT11_DQ_IN  PBin(9)

////////////////////////////////////////////////////////////////////////////////// 
//IO
//#define DHT11_IO_IN()  {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=8<<0;}
//#define DHT11_IO_OUT() {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=3<<0;}

//#define	DHT11_DQ_OUT PAout(8)
//#define	DHT11_DQ_IN  PAin(8)
 

extern u8 DHT11_Init(void);
extern u8 DHT11_Read_Data(u8 *temp,u8 *humi);
extern u8 DHT11_Read_Byte(void);
extern u8 DHT11_Read_Bit(void);
extern u8 DHT11_Check(void);
extern void DHT11_Rst(void);

#ifdef __cplusplus
}
#endif

#endif

