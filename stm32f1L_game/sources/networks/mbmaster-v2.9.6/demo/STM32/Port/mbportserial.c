/*
 * MODBUS Library: ARM STM32 Port (FWLIB 2.0x)
 * Copyright (c) Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * ARM STM32 Port by Niels Andersen, Elcanic A/S <niels.andersen.elcanic@gmail.com>
 *
 * $Id: mbportserial.c,v 1.2 2008-12-14 20:30:27 cwalter Exp $
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


#define UART_1_ENABLED          ( 1 )   /*!< Set this to 1 to enable USART1 */
#define UART_2_ENABLED          ( 1 )   /*!< Set this to 1 to enable USART2 */

#if ( UART_1_ENABLED == 1 ) && ( UART_2_ENABLED == 1 )
#define UART_1_PORT             ( MB_UART_1 )
#define UART_2_PORT             ( MB_UART_2 )
#define UART_1_IDX              ( 0 )
#define UART_2_IDX              ( 1 )
#define NUARTS                  ( 2 )
#elif ( UART_1_ENABLED == 1 )
#define UART_1_PORT             ( MB_UART_1 )
#define UART_1_IDX              ( 0 )
#define NUARTS                  ( 1 )
#elif ( UART_2_ENABLED == 1 )
#define UART_2_PORT             ( MB_UART_2 )
#define UART_2_IDX              ( 0 )
#define NUARTS                  ( 1 )
#else
#define NUARTS                  ( 0 )
#endif

#define RS_485_UART_1_INIT(  )	\
do { \
} while( 0 )

#define RS_485_UART_1_ENABLE_TX(  )	\
do {\
    /* not implemented yet */\
} while( 0 )

#define RS_485_UART_1_DISABLE_TX(  ) \
do { \
    /* not implemented yet */ \
} while( 0 )

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

/* ----------------------- Defines ------------------------------------------*/
/* ----------------------- Defines (Internal - Don't change) ----------------*/
#define HDL_RESET( x ) do { \
	( x )->ubIdx = IDX_INVALID; \
	( x )->pbMBMTransmitterEmptyFN = NULL; \
	( x )->pvMBMReceiveFN = NULL; \
	( x )->xMBMHdl = MB_HDL_INVALID; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    UBYTE           ubIdx;
    pbMBPSerialTransmitterEmptyAPIV1CB pbMBMTransmitterEmptyFN;
    pvMBPSerialReceiverAPIV1CB pvMBMReceiveFN;
    xMBHandle       xMBMHdl;
} xSerialHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xSerialHandle xSerialHdls[NUARTS];
STATIC BOOL     bIsInitalized = FALSE;

/* ----------------------- Static functions ---------------------------------*/
void            vMBPUSART1ISR( void ) __attribute__ ( ( __interrupt__ ) );
STATIC void     prvvMBPUSART1_TXE_ISR( void );
STATIC void     prvvMBPUSART1_TC_ISR( void );
STATIC void     prvvMBPUSART1_RXNE_ISR( void );

void            vMBPUSART2ISR( void ) __attribute__ ( ( __interrupt__ ) );
STATIC void     prvvMBPUSART2_TXE_ISR( void );
STATIC void     prvvMBPUSART2_TC_ISR( void );
STATIC void     prvvMBPUSART2_RXNE_ISR( void );

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl )
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    eMBErrorCode    eStatus = MB_ENOERR;
    UBYTE           ubIdx;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( !bIsInitalized )
    {
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
        {
            HDL_RESET( &xSerialHdls[ubIdx] );
        }
#if UART_1_ENABLED == 1
        RS_485_UART_1_INIT(  );
        RS_485_UART_1_DISABLE_TX(  );
#endif
#if UART_2_ENABLED == 1
        RS_485_UART_2_INIT(  );
        RS_485_UART_2_DISABLE_TX(  );
#endif
        bIsInitalized = TRUE;
    }

    if( ( MB_HDL_INVALID == xMBMHdl ) || ( NULL == pxSerialHdl ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        eStatus = MB_ENORES;

        /* Setup baudrate */
        if( ( ulBaudRate > UART_BAUDRATE_MIN ) && ( ulBaudRate < UART_BAUDRATE_MAX ) )
        {
            USART_InitStructure.USART_BaudRate = ulBaudRate;
        }
        else
        {
            eStatus = MB_EINVAL;
        }

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

        /* Setup flow control */
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

        /* Enable transmit and receive */
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        /* Default other settings. */
        USART_InitStructure.USART_Clock = USART_Clock_Disable;
        USART_InitStructure.USART_CPOL = USART_CPOL_Low;
        USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
        USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

        if( eStatus != MB_EINVAL )
        {
            switch ( ucPort )
            {
#if UART_1_ENABLED == 1
            case UART_1_PORT:
                if( IDX_INVALID == xSerialHdls[UART_1_IDX].ubIdx )
                {
                    /* Configure  USART1 */
                    USART_Init( USART1, &USART_InitStructure );
                    /* Enable the USART1 */
                    USART_Cmd( USART1, ENABLE );
                    /* Disable receive and transmit interrupts from the beginning */
                    USART_ITConfig( USART1, USART_IT_RXNE, DISABLE );
                    USART_ITConfig( USART1, USART_IT_TXE, DISABLE );

                    /* Setup handle to uart */
                    *pxSerialHdl = &xSerialHdls[UART_1_IDX];
                    xSerialHdls[UART_1_IDX].ubIdx = UART_1_IDX;
                    xSerialHdls[UART_1_IDX].xMBMHdl = xMBMHdl;

                    /* Everything is ok */
                    eStatus = MB_ENOERR;
                }
                else
                {
                    eStatus = MB_ENORES;
                }
                break;
#endif
#if UART_2_ENABLED == 1
            case UART_2_PORT:
                if( IDX_INVALID == xSerialHdls[UART_2_IDX].ubIdx )
                {
                    /* Configure STM32 USART2 */
                    USART_Init( USART2, &USART_InitStructure );

                    /* Enable the USART2 */
                    USART_Cmd( USART2, ENABLE );

                    /* Disable receive and transmit interrupts from the beginning */
                    USART_ITConfig( USART2, USART_IT_RXNE, DISABLE );
                    USART_ITConfig( USART2, USART_IT_TXE, DISABLE );

                    /* Setup handle to uart */
                    *pxSerialHdl = &xSerialHdls[UART_2_IDX];
                    xSerialHdls[UART_2_IDX].ubIdx = UART_2_IDX;
                    xSerialHdls[UART_2_IDX].xMBMHdl = xMBMHdl;

                    /* Every thing is ok */
                    eStatus = MB_ENOERR;
                }
                else
                {
                    eStatus = MB_ENORES;
                }
                break;
#endif
            default:
                break;
            }
        }
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
        switch ( pxSerialIntHdl->ubIdx )
        {
#if UART_1_ENABLED == 1
        case UART_1_IDX:
            if( ( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN ) && ( NULL == pxSerialIntHdl->pvMBMReceiveFN ) )
            {
                /* Close USART 1 */
                USART_Cmd( USART1, DISABLE );
                USART_DeInit( USART1 );
                /* Force RS485 back to receive mode */
                RS_485_UART_1_DISABLE_TX(  );
                /* Reset handle */
                HDL_RESET( pxSerialIntHdl );
                /* No error */
                eStatus = MB_ENOERR;
            }
            else
            {
                eStatus = MB_EIO;
            }
            break;
#endif
#if UART_2_ENABLED == 1
        case UART_2_IDX:
            if( ( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN ) && ( NULL == pxSerialIntHdl->pvMBMReceiveFN ) )
            {
                /* Close USART 1 */
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
                eStatus = MB_EIO;
            }
            break;
#endif
        default:
            MBP_ASSERT( 0 );
            break;
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl, pbMBPSerialTransmitterEmptyCB pbMBMTransmitterEmptyFN )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pbMBMTransmitterEmptyFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN );
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = pbMBMTransmitterEmptyFN;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_1_ENABLED == 1
            case UART_1_IDX:
                /* RS485 transmit mode */
                RS_485_UART_1_ENABLE_TX(  );
                /* Enable USART 1 tx interrupt */
                USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
                break;
#endif
#if UART_2_ENABLED == 1
            case UART_2_IDX:
                /* RS485 transmit mode */
                RS_485_UART_2_ENABLE_TX(  );
                /* Enable USART 2 tx interrupt */
                USART_ITConfig( USART2, USART_IT_TXE, ENABLE );
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }

        }
        else
        {
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = NULL;
            /* The transmitter is disable when the last frame has been sent.
             * This is necessary for RS485 with a half-duplex bus.
             */
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_1_ENABLED == 1
            case UART_1_IDX:
                /* Disable transmit register empty interrupt */
                USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
                /* Enable transmit complete interrupt */
                USART_ITConfig( USART1, USART_IT_TC, ENABLE );
                break;
#endif
#if UART_2_ENABLED == 1
            case UART_2_IDX:
                /* Disable transmit register empty interrupt */
                USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
                /* Enable transmit complete interrupt */
                USART_ITConfig( USART2, USART_IT_TC, ENABLE );
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBMReceiveFN )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pvMBMReceiveFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pvMBMReceiveFN );
            pxSerialIntHdl->pvMBMReceiveFN = pvMBMReceiveFN;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_1_ENABLED == 1
            case UART_1_IDX:
                /* Enable USART 1 receive interrupt */
                USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
                break;
#endif
#if UART_2_ENABLED == 1
            case UART_2_IDX:
                /* Enable USART 2 receive interrupt */
                USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
        else
        {
            pxSerialIntHdl->pvMBMReceiveFN = NULL;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_1_ENABLED == 1
            case UART_1_IDX:
                /* Disable USART 1 receive interrupt */
                USART_ITConfig( USART1, USART_IT_RXNE, DISABLE );
                break;
#endif
#if UART_2_ENABLED == 1
            case UART_2_IDX:
                /* Disable USART 2 receive interrupt */
                USART_ITConfig( USART2, USART_IT_RXNE, DISABLE );
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

#if UART_1_ENABLED == 1
void
vMBPUSART1ISR( void )
{
    /* Check for receive interrupt */
    if( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
    {
        /* Handle data incomming data in modbus functions. Interrupt flag is 
         * cleared when byte is read in receive register.
         */
        prvvMBPUSART1_RXNE_ISR(  );
    }
    /* Check for transmit interrupt */
    if( USART_GetITStatus( USART1, USART_IT_TXE ) != RESET )
    {
        /* Handle transmission of data in modbus functions. Interrupt flags are
         * cleared when new byte is written to transmit register.
         */
        prvvMBPUSART1_TXE_ISR(  );
    }
    /* Check for transmit complete */
    if( USART_GetITStatus( USART1, USART_IT_TC ) != RESET )
    {
        /* Handle transmit complete in modbus library */
        prvvMBPUSART1_TC_ISR(  );
    }
}

void
prvvMBPUSART1_TXE_ISR( void )
{
    MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_1_IDX].ubIdx );
    BOOL            bHasMoreData = TRUE;
    UBYTE           ubTxByte;

    if( NULL != xSerialHdls[UART_1_IDX].pbMBMTransmitterEmptyFN )
    {
        bHasMoreData = xSerialHdls[UART_1_IDX].pbMBMTransmitterEmptyFN( xSerialHdls[UART_1_IDX].xMBMHdl, &ubTxByte );
    }
    if( !bHasMoreData )
    {
        xSerialHdls[UART_1_IDX].pbMBMTransmitterEmptyFN = NULL;
        /* The transmitter is disabled when the last frame has been sent.
         * This is necessary for RS485 with a hald-duplex bus.
         */
        USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
        USART_ITConfig( USART1, USART_IT_TC, ENABLE );
    }
    else
    {
        /* Transmit byte on USART */
        USART_SendData( USART1, ubTxByte );
    }
}

/* USART 1 Transmit Complete interrupt */
void
prvvMBPUSART1_TC_ISR( void )
{
    /* Back to receive mode */
    RS_485_UART_1_DISABLE_TX(  );
    /* Transmission complete. Disable interrupt */
    USART_ITConfig( USART1, USART_IT_TC, DISABLE );
}

/* USART 1 Receive interrupt */
void
prvvMBPUSART1_RXNE_ISR( void )
{
    UBYTE           ubUDR;
    FlagStatus      fs;

    /* Read current flagstatus */
    fs = RESET;
    if( USART_GetFlagStatus( USART1, USART_FLAG_ORE ) )
    {
        fs |= 1;
    }
    if( USART_GetFlagStatus( USART1, USART_FLAG_NE ) )
    {
        fs |= 2;
    }
    if( USART_GetFlagStatus( USART1, USART_FLAG_FE ) )
    {
        fs |= 4;
    }
    if( USART_GetFlagStatus( USART1, USART_FLAG_PE ) )
    {
        fs |= 8;
    }

    /* Receive byte from USART1 */
    ubUDR = USART_ReceiveData( USART1 );

    /* Send data to modbus functions
     * if no error */
    if( fs == RESET )
    {
        /* Pass received data on to modbuslib */
        MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_1_IDX].ubIdx );
        if( NULL != xSerialHdls[UART_1_IDX].pvMBMReceiveFN )
        {
            xSerialHdls[UART_1_IDX].pvMBMReceiveFN( xSerialHdls[UART_1_IDX].xMBMHdl, ubUDR );
        }
    }
}

#endif

#if UART_2_ENABLED == 1
void
vMBPUSART2ISR( void )
{
    /* Check for receive interrupt */
    if( USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
    {
        /* Handle data incomming data in modbus functions. Interrupt flag is 
         * cleared when byte is read in receive register.
         */
        prvvMBPUSART2_RXNE_ISR(  );
    }
    /* Check for transmit interrupt */
    if( USART_GetITStatus( USART2, USART_IT_TXE ) != RESET )
    {
        /* Handle transmission of data in modbus functions. Interrupt flags are
         * cleared when new byte is written to transmit register.
         */
        prvvMBPUSART2_TXE_ISR(  );
    }
    /* Check for transmit complete */
    if( USART_GetITStatus( USART2, USART_IT_TC ) != RESET )
    {
        /* Handle transmit complete in modbus library */
        prvvMBPUSART2_TC_ISR(  );
    }
}

/* USART 2 transmit register empty interrupt */
void
prvvMBPUSART2_TXE_ISR( void )
{
    MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_2_IDX].ubIdx );
    BOOL            bHasMoreData = TRUE;
    UBYTE           ubTxByte;

    if( NULL != xSerialHdls[UART_2_IDX].pbMBMTransmitterEmptyFN )
    {
        bHasMoreData = xSerialHdls[UART_2_IDX].pbMBMTransmitterEmptyFN( xSerialHdls[UART_2_IDX].xMBMHdl, &ubTxByte );
    }
    if( !bHasMoreData )
    {
        xSerialHdls[UART_2_IDX].pbMBMTransmitterEmptyFN = NULL;
        /* The transmitter is disabled when the last frame has been sent.
         * This is necessary for RS485 with a hald-duplex bus.
         */
        USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
        USART_ITConfig( USART2, USART_IT_TC, ENABLE );
    }
    else
    {
        /* Transmit byte on USART 2 */
        USART_SendData( USART2, ubTxByte );
    }
}

/* USART 2 Transmit Complete interrupt */
void
prvvMBPUSART2_TC_ISR( void )
{
    /* Back to receive mode */
    RS_485_UART_2_DISABLE_TX(  );
    /* Disable TC interrupt */
    USART_ITConfig( USART2, USART_IT_TC, DISABLE );
}

/* USART 2 reveive interrupt */
void
prvvMBPUSART2_RXNE_ISR( void )
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

    /* Send data to modbus functions
     * if no error */
    if( fs == RESET )
    {
        MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_2_IDX].ubIdx );
        if( NULL != xSerialHdls[UART_2_IDX].pvMBMReceiveFN )
        {
            xSerialHdls[UART_2_IDX].pvMBMReceiveFN( xSerialHdls[UART_2_IDX].xMBMHdl, ubUDR );
        }
    }
}
#endif
