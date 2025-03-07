#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include <stdint.h>

#include "../sys/sys_boot.h"

#include "app.h"

/******************************************************************************
* IF Type
*******************************************************************************/
/** RF24 interface for modules
 * IF_TYPE_RF24_GW using to transfer data to gateway.
 * IF_TYPE_RF24_AC using to transfer data to air_condition module.
*/
#define IF_TYPE_RF24_MIN					(0)
#define IF_TYPE_RF24_GW						(0)
#define IF_TYPE_RF24_AC						(1)
#define IF_TYPE_RF24_MAX					(99)

/** APP interface, communication via socket interface
 *
 */
#define IF_TYPE_APP_MIN						(100)
#define IF_TYPE_APP_GW						(100)
#define IF_TYPE_APP_GI						(101)
#define IF_TYPE_APP_MAX						(119)

/** UART interface
 *
 */
#define IF_TYPE_UART_GW_MIN					(120)
#define IF_TYPE_UART_GW						(120)
#define IF_TYPE_UART_AC						(121)
#define IF_TYPE_UART_GW_MAX					(140)


#define RF24_ENCRYPT_DECRYPT_KEY_SIZE		16
extern uint8_t rf24_encrypt_decrypt_key[];

typedef struct {
	uint8_t is_power_on_reset;
} boot_app_share_data_t;

#endif //__APP_DATA_H__
