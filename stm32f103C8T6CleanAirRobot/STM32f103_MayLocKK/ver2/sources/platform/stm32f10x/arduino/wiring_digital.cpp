#include "Arduino.h"

#include "app_dbg.h"
#include "../io_cfg.h"
#include "../../../sys/sys_dbg.h"

void pinMode(uint8_t pin, uint8_t mode) {
	switch (pin) {
	case Arduino_RS485_DRIECTION_PIN: {

		if (mode == 1) {
			io_RS485_dir_pin_output();
			APP_DBG("io_RS485_dir_pin_output();\n");
		} else if (mode == 0) {
			io_RS485_dir_pin_input();
			APP_DBG("io_RS485_dir_pin_input();\n");
		}
	}
		break;

	default:
		break;
	}
}

void digitalWrite(uint8_t pin, uint8_t pinval) {
	switch (pin) {
	case Arduino_RS485_DRIECTION_PIN: {

		if (pinval == 1) {
			//APP_DBG("io_RS485_dir_pin_set();\n");
			io_RS485_dir_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			//APP_DBG("io_RS485_dir_pin_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_dir_pin_reset();
		}
	}
		break;
	case Arduino_RS485_RELAY1_PIN: {

		if (pinval == 1) {
			APP_DBG("io_RS485_relay1_set();\n");
			io_RS485_relay1_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			APP_DBG("io_RS485_relay1_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_relay1_pin_reset();
		}
	}
		break;
	case Arduino_RS485_RELAY2_PIN: {

		if (pinval == 1) {
			APP_DBG("io_RS485_relay2_set();\n");
			io_RS485_relay2_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			APP_DBG("io_RS485_relay2_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_relay2_pin_reset();
		}
	}
		break;
	case Arduino_RS485_RELAY3_PIN: {

		if (pinval == 1) {
			APP_DBG("io_RS485_relay3_set();\n");
			io_RS485_relay3_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			APP_DBG("io_RS485_relay3_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_relay3_pin_reset();
		}
	}
		break;
	case Arduino_RS485_RELAY4_PIN: {

		if (pinval == 1) {
			APP_DBG("io_RS485_relay4_set();\n");
			io_RS485_relay4_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			APP_DBG("io_RS485_relay4_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_relay4_pin_reset();
		}
	}
		break;
	case Arduino_RS485_RELAY5_PIN: {

		if (pinval == 1) {
			APP_DBG("io_RS485_relay5_set();\n");
			io_RS485_relay5_pin_set();
			//io_RS485_dir_pin_reset();
		} else if (pinval == 0) {
			APP_DBG("io_RS485_relay5_reset(); \n");
			//io_RS485_dir_pin_set();
			io_RS485_relay5_pin_reset();
		}
	}
		break;
	default:
		break;
	}
}

uint8_t digitalRead(uint8_t pin) {
	uint8_t pinval = 0;
	switch (pin) {
	case 1: {
	}
		break;
	default:
		break;
	}
	return pinval;
}
