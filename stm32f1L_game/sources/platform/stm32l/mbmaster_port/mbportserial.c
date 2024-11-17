/*
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id: mbportserial.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "common/mbtypes.h"
#include "common/mbportlayer.h"
#include "common/mbframe.h"
#include "common/mbutils.h"

/* ----------------------- Defines ------------------------------------------*/
#define IDX_INVALID				( 255 )
#define UART_BAUDRATE_MIN		( 300 )
#define UART_BAUDRATE_MAX		( 115200 )

#define RS_485_UART_2_INIT(  )\
	do { \
	/* not implemented yet */ \
	} while( 0 )

#define RS_485_UART_2_ENABLE_TX(  )	\
	do { \
	/* not implemented yet */ \
	} while( 0 )

#define RS_485_UART_2_DISABLE_TX(  ) \
	do { \
	/* not implemented yet */ \
	} while( 0 )

#define HDL_RESET( x ) do { \
	( x )->ubIdx = IDX_INVALID; \
	( x )->pbMBPTransmitterEmptyFN = NULL; \
	( x )->pvMBPReceiveFN = NULL; \
	( x )->xMBMHdl = MB_HDL_INVALID; \
	} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
	UBYTE           ubIdx;
	pbMBPSerialTransmitterEmptyAPIV1CB pbMBPTransmitterEmptyFN;
	pvMBPSerialReceiverAPIV1CB pvMBPReceiveFN;
	xMBHandle       xMBMHdl;
} xSerialHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xSerialHandle xSerialHdls[1];
STATIC BOOL     bIsInitalized = FALSE;

/* ----------------------- Static functions ---------------------------------*/
void            vMBPUSART2ISR( void ) __attribute__ ( ( __interrupt__ ) );
STATIC void     prrvUSARTRxISR( void );
STATIC void     prrvUSARTTxISR( void );
STATIC void     prrvUSARTTcISR( void );

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
				UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	UBYTE           ubIdx;

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( !bIsInitalized )
	{
		for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
		{
			HDL_RESET( &xSerialHdls[ubIdx] );
		}

		RS_485_UART_2_INIT(  );
		RS_485_UART_2_DISABLE_TX(  );

		bIsInitalized = TRUE;
	}

	if( NULL == pxSerialHdl )
	{
		eStatus = MB_EINVAL;
	}
	else if( ( 0 == ucPort ) && ( IDX_INVALID == xSerialHdls[0].ubIdx ) )
	{
		*pxSerialHdl = NULL;

		if( ( ulBaudRate > UART_BAUDRATE_MIN ) && ( ulBaudRate < UART_BAUDRATE_MAX ) && ( MB_HDL_INVALID != xMBMHdl ) )
		{
			/* TODO: Initialize the serial port here. */

			/* Setup stopbits */
			switch ( ucStopBits )
			{
			case 1:
				USART_InitStructure.USART_StopBits = USART_StopBits_1;
				break;
			case 2:
				USART_InitStructure.USART_StopBits = USART_StopBits_1;
				break;
			default:
				eStatus = MB_EINVAL;
				break;
			}

			/* For STM32 parity is placed on bit 9 (MSB)
			 * Select correct number of databits */
			if( ucDataBits == 8 )
			{
				if( eParity != MB_PAR_NONE )
				{
					USART_InitStructure.USART_WordLength = USART_WordLength_9b;
				}
				else
				{
					USART_InitStructure.USART_WordLength = USART_WordLength_8b;
				}
			}
			else
			{
				if( eParity != MB_PAR_NONE )
				{
					USART_InitStructure.USART_WordLength = USART_WordLength_8b;
				}
				else
				{
					eStatus = MB_EINVAL;
				}
			}

			/* Setup parity */
			switch ( eParity )
			{
			case MB_PAR_NONE:
				USART_InitStructure.USART_Parity = USART_Parity_No;
				break;
			case MB_PAR_ODD:
				USART_InitStructure.USART_Parity = USART_Parity_Odd;
				break;
			case MB_PAR_EVEN:
				USART_InitStructure.USART_Parity = USART_Parity_Even;
				break;
			default:
				eStatus = MB_EINVAL;
				break;
			}

			/* Enable GPIO clock */
			RCC_AHBPeriphClockCmd(USART2_TX_GPIO_CLK | USART2_RX_GPIO_CLK, ENABLE);

			/* Enable USART clock */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

			/* Connect PXx to USART2_Tx */
			GPIO_PinAFConfig(USART2_TX_GPIO_PORT, USART2_TX_SOURCE, USART2_TX_AF);

			/* Connect PXx to USART2_Rx */
			GPIO_PinAFConfig(USART2_RX_GPIO_PORT, USART2_RX_SOURCE, USART2_RX_AF);

			/* Configure USART Tx and Rx as alternate function push-pull */
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
			GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
			GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);

			/* USART2 configuration */
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART2, &USART_InitStructure);

			/* Enable the USART2 Interrupt */
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQ_PRIO_UART2_IO;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);

			USART_ClearITPendingBit(USART2, USART_IT_RXNE | USART_IT_TXE);
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);

			/* Enable USART */
			USART_Cmd(USART2, ENABLE);

			/* Setup handle to uart */
			xSerialHdls[0].ubIdx = 0;
			xSerialHdls[0].xMBMHdl = xMBMHdl;
			eStatus = MB_ENOERR;
		}
		else
		{
			eStatus = MB_EINVAL;
		}
	}
	else
	{
		eStatus = MB_ENORES;
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

eMBErrorCode
eMBPSerialClose( xMBPSerialHandle xSerialHdl )
{
	eMBErrorCode    eStatus = MB_EINVAL;
	xSerialHandle  *pxSerialIntHdl = xSerialHdl;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
	{
		if( ( pxSerialIntHdl->pbMBPTransmitterEmptyFN == NULL ) && ( pxSerialIntHdl->pvMBPReceiveFN == NULL ) )
		{
			/* TODO: Close the serial port here. */
			/* Close USART 2 */
			USART_Cmd( USART2, DISABLE );
			USART_DeInit( USART2 );
			/* Force RS485 back to receive mode */
			RS_485_UART_2_DISABLE_TX(  );
			/* Reset handle */
			HDL_RESET( pxSerialIntHdl );
			/* No error */
			eStatus = MB_ENOERR;
		}
		else
		{
			eStatus = MB_EAGAIN;
		}
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

eMBErrorCode
eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl, pbMBPSerialTransmitterEmptyCB pbMBPTransmitterEmptyFN )
{
	eMBErrorCode    eStatus = MB_EINVAL;
	xSerialHandle  *pxSerialIntHdl = xSerialHdl;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
	{
		eStatus = MB_ENOERR;
		if( NULL != pbMBPTransmitterEmptyFN )
		{
			MBP_ASSERT( NULL == pxSerialIntHdl->pbMBPTransmitterEmptyFN );
			pxSerialIntHdl->pbMBPTransmitterEmptyFN = pbMBPTransmitterEmptyFN;

			/* TODO: Enable the transmitter. */
			/* RS485 transmit mode */
			RS_485_UART_2_ENABLE_TX(  );
			/* Enable USART 2 tx interrupt */
			USART_ITConfig( USART2, USART_IT_TXE, ENABLE );
		}
		else
		{
			pxSerialIntHdl->pbMBPTransmitterEmptyFN = NULL;

			/* TODO: Disable the transmitter. Make sure that all characters have been
			 * transmitted in case you do any buffering internally.
			 */
			/* Disable transmit register empty interrupt */
			USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
			/* Enable transmit complete interrupt */
			USART_ITConfig( USART2, USART_IT_TC, ENABLE );
		}
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

eMBErrorCode
eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBPReceiveFN )
{
	eMBErrorCode    eStatus = MB_EINVAL;
	xSerialHandle  *pxSerialIntHdl = xSerialHdl;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
	{
		eStatus = MB_ENOERR;
		if( NULL != pvMBPReceiveFN )
		{
			MBP_ASSERT( NULL == pxSerialIntHdl->pvMBPReceiveFN );
			pxSerialIntHdl->pvMBPReceiveFN = pvMBPReceiveFN;

			/* TODO: Enable the receiver. */
			/* Enable USART 2 receive interrupt */
			USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );
		}
		else
		{
			pxSerialIntHdl->pvMBPReceiveFN = NULL;

			/* TODO: Disable the receiver. */
			/* Disable USART 2 receive interrupt */
			USART_ITConfig( USART2, USART_IT_RXNE, DISABLE );
		}
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

void
vMBPUSART2ISR( void )
{
	/* Check for receive interrupt */
	if( USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
	{
		/* Handle data incomming data in modbus functions. Interrupt flag is
		 * cleared when byte is read in receive register.
		 */
		prrvUSARTRxISR(  );
	}
	/* Check for transmit interrupt */
	if( USART_GetITStatus( USART2, USART_IT_TXE ) != RESET )
	{
		/* Handle transmission of data in modbus functions. Interrupt flags are
		 * cleared when new byte is written to transmit register.
		 */
		prrvUSARTTxISR(  );
	}
	/* Check for transmit complete */
	if( USART_GetITStatus( USART2, USART_IT_TC ) != RESET )
	{
		/* Handle transmit complete in modbus library */
		prrvUSARTTcISR(  );
	}
}

STATIC void
prrvUSARTTxISR( void )
{
	BOOL            bHasMoreData = TRUE;
	UBYTE           ubTxByte;

	if( NULL != xSerialHdls[0].pbMBPTransmitterEmptyFN )
	{
		bHasMoreData = xSerialHdls[0].pbMBPTransmitterEmptyFN( xSerialHdls[0].xMBMHdl, &ubTxByte );
	}
	if( !bHasMoreData )
	{
		xSerialHdls[0].pbMBPTransmitterEmptyFN = NULL;

		/* TODO: Disable the transmitter. */
		/* The transmitter is disabled when the last frame has been sent.
		 * This is necessary for RS485 with a hald-duplex bus.
		 */
		USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
		USART_ITConfig( USART2, USART_IT_TC, ENABLE );
	}
	else
	{
		/* TODO: Place byte ubTxByte in the UART data register. */
		/* Transmit byte on USART 2 */
		USART_SendData( USART2, ubTxByte );
	}
}

STATIC void
prrvUSARTRxISR( void )
{
	UBYTE           ubUDR;
	FlagStatus      fs;

	/* Read current flagstatus */
	fs = RESET;
	if( USART_GetFlagStatus( USART2, USART_FLAG_ORE ) )
		fs |= 1;
	if( USART_GetFlagStatus( USART2, USART_FLAG_NE ) )
		fs |= 2;
	if( USART_GetFlagStatus( USART2, USART_FLAG_FE ) )
		fs |= 4;
	if( USART_GetFlagStatus( USART2, USART_FLAG_PE ) )
		fs |= 8;

	/* Read byte from USART */
	ubUDR = USART_ReceiveData( USART2 );

	MBP_ASSERT( IDX_INVALID != xSerialHdls[0].ubIdx );
	if( NULL != xSerialHdls[0].pvMBPReceiveFN )
	{
		xSerialHdls[0].pvMBPReceiveFN( xSerialHdls[0].xMBMHdl, ubUDR );
	}
}

STATIC void
prrvUSARTTcISR( void )
{
	/* Back to receive mode */
	RS_485_UART_2_DISABLE_TX(  );
	/* Disable TC interrupt */
	USART_ITConfig( USART2, USART_IT_TC, DISABLE );
}
