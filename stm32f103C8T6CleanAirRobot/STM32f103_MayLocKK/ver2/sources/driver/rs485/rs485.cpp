#include "rs485.h"
#include "../ak/fsm.h"
#include "../ak/port.h"
#include "../ak/message.h"

#include "../common/cmd_line.h"
#include "../common/utils.h"
#include "../common/xprintf.h"

#include "../sys/sys_irq.h"
#include "../sys/sys_io.h"
#include "../sys/sys_ctrl.h"
#include "../sys/sys_dbg.h"
#include "../sys/sys_arduino.h"

#include "app.h"
#include "app_dbg.h"

#define RS485_SOP_CHAR					(0xEF)
#define RS485_DATA_SIZE					(128)
#define RS485_ADDR						(0xFF)

#define SOP_STATE						(0x00)
#define ADDR_STATE						(0x01)
#define LEN_STATE						(0x02)
#define DATA_STATE						(0x03)
#define FCS_STATE						(0x04)


static void rs485_frame_parser(uint8_t data);
static uint8_t rs485_frame_state = SOP_STATE;
static uint8_t rs485_cal_cs(uint8_t, uint8_t*);

//static void (*rs485_rev_callback)(uint8_t* data, uint8_t len);
static void (*rs485_write)(uint8_t);
static uint8_t (*rs485_getc)();

void rs485_register_rev_callback(rs485_rev_callback rev_callback) {
	rs485_rev_callback = rev_callback;
}

void rs485_frame_write(uint8_t addr, uint8_t* data, uint8_t len) {
	rs485_write();
}

typedef struct {
	uint8_t start_frame;
	uint8_t dev_addr;
	uint8_t data_index;
	uint8_t data_len;
	uint8_t data_buffer[RS485_DATA_SIZE];
	uint8_t check_sum;
} rs485_data_frame_t;

void rs485_frame_write(uint8_t rs485_dev_addr, uint8_t* data, uint32_t len) {
	rs485_data_frame_t rs485_data_frame_wr;
	rs485_data_frame_wr.start_frame = 0xEF;
	rs485_data_frame_wr.dev_addr = rs485_dev_addr;
	rs485_data_frame_wr.data_len = len;
	mem_cpy(rs485_data_frame_wr.data_buffer, data, len);
	rs485_data_frame_wr.check_sum = rs485_cal_cs(data, len);
	io_uart_interface_transfer(rs485_data_frame_wr.start_frame);
	io_uart_interface_transfer(rs485_data_frame_wr.dev_addr);
	for (uint8_t i = 0; i < len; i++) {
		io_uart_interface_transfer(rs485_data_frame_wr.data_buffer[i]);
	}
	io_uart_interface_transfer(rs485_data_frame_wr.check_sum);
}


uint8_t rs485_cal_cs(uint8_t *data_ptr, uint8_t len) {
	uint8_t xor_result = len;
	for (int i = 0; i < len; i++, data_ptr++) {
		xor_result = xor_result ^ *data_ptr;
	}
	return xor_result;
}



void rs485_frame_parser(uint8_t ch) {
	switch (rs485_frame_state) {
	case SOP_STATE: {
		if (RS485_SOP_CHAR == ch) {
			rs485_frame_state = ADDR_STATE;
		}
	}
		break;

	case ADDR_STATE: {
		if (ch = RS485_ADDR ){
			rs485_frame_state = LEN_STATE;
		}
		else {
			rs485_frame_state = SOP_STATE;
		}
	}

	case LEN_STATE: {
		if (ch > RS485_DATA_SIZE) {
			rs485_frame_state = SOP_STATE;
		}
		else {
			rs485_data_frame_t.data_len = ch;
			rs485_data_frame_t.data_index = 0;
			rs485_frame_state = DATA_STATE;
		}
	}
		break;

	case DATA_STATE: {
		rs485_data_frame_t.data_len[rs485_data_frame_t.data_index++] = ch;

		if (rs485_data_frame_t.data_index == rs485_data_frame_t.len) {
			rs485_frame_state = FCS_STATE;
		}
	}
		break;

	case FCS_STATE: {
		rs485_frame_state = SOP_STATE;

		rs485_data_frame_t.check_sum = ch;

		if (rs485_data_frame_t.frame_fcs \
				== rs485_cal_cs(rs485_data_frame_t.data_buffer, rs485_data_frame_t.data_buffer)) {
				/* post message to rs485 task */
		}
		else {
			/* TODO: handle checksum incorrect */
		}
	}
		break;

	default:
		break;
	}
}


