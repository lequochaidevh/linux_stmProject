#include "eeprom.h"

#include "../sys/sys_io.h"
#include "../sys/sys_dbg.h"

uint8_t eeprom_read(uint32_t address, uint8_t* pbuf, uint32_t len) {
return 0;
}

uint8_t eeprom_write(uint32_t address, uint8_t* pbuf, uint32_t len) {
	return io_eeprom_write(address, pbuf, len);
}

uint8_t eeprom_erase(uint32_t address, uint32_t len) {
return 0;
}
