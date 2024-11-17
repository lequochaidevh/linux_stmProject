#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include <stdint.h>
#include "app.h"

/******************************************************************************
* interface type
*******************************************************************************/
/** RF24 interface for modules
 * IF_TYPE_RF24_GW using to transfer data to gateway.
 * IF_TYPE_RF24_AC using to transfer data to air_condition module.
 * IF_TYPE_RF24_WR using to transfer data to warning_sensors module.
 * IF_TYPE_RF24_DM using to transfer data to door_manager module.
*/
#define IF_TYPE_RF24_GW						(0)
#define IF_TYPE_RF24_AC						(1)
#define IF_TYPE_RF24_WR						(2)
#define IF_TYPE_RF24_DM						(3)
#define IF_TYPE_RF24_MAX					(4)

/******************************************************************************
* Data type of RF24Network
*******************************************************************************/
#define RF24_DATA_COMMON_MSG_TYPE			(1)
#define RF24_DATA_PURE_MSG_TYPE				(2)
#define RF24_DATA_REMOTE_CMD_TYPE			(3)

/** APP interface, communication via socket interface
 *
 */
#define IF_TYPE_APP_MIN						(100)
#define IF_TYPE_APP_GW						(100)
#define IF_TYPE_APP_GI						(101)

/******************************************************************************
* Common define
*******************************************************************************/
#define IF_RETRY_COUNTER_MAX		3

/******************************************************************************
* Commom data structure for transceiver data
*******************************************************************************/
#define FIRMWARE_PSK		0x1A2B3C4D
#define FIRMWARE_LOK		0x1234ABCD

#define WR_SENSOR_NOT_CONNECT		0
#define WR_SENSOR_NONE_ACTIVE		1
#define WR_SENSOR_ACTIVE			2
#define WR_SENSOR_ERROR				3

#define RF24_ENCRYPT_DECRYPT_KEY_SIZE		16
extern uint8_t rf24_encrypt_decrypt_key[];

typedef struct {
	uint32_t psk;
	uint32_t bin_len;
	uint16_t checksum;
} firmware_header_t;

typedef struct {
	uint16_t not_connect;
	uint16_t connected_high;
	uint16_t connected_low;
	uint16_t warning_high;
	uint16_t warning_low;
	uint16_t error;
} wr_value_detect_t;

typedef struct {
	wr_value_detect_t door;
	wr_value_detect_t smoke;
	wr_value_detect_t fire;
	wr_value_detect_t water;
} wr_sensor_milestone_t;

typedef struct {
	uint8_t door_opened;
	uint8_t smoke;
	uint8_t fire;
	uint8_t water;
} wr_sensor_packet_t;

typedef struct {
	uint8_t node_chanel;		/* node nrf chanel */
	uint8_t node_addr;			/* node address */
	uint8_t node_server_addr;	/* node server address*/
} network_config_t;

#endif //__APP_DATA_H__
