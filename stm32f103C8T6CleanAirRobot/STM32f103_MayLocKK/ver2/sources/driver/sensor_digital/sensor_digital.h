#ifndef __SENSOR_DIGITAL_H__
#define __SENSOR_DIGITAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define SENSOR_DIGITAL_DRIVER_OK						(0x00)
#define SENSOR_DIGITAL_DRIVER_NG						(0x01)

#define SENSOR_DIGITAL_SHORT_ACTIVE_MIN_TIME			(20)		/* 20ms */
#define SENSOR_DIGITAL_SHORT_ACTIVE_MAX_TIME			(1000)		/* 1s */

#define SENSOR_DIGITAL_DISABLE							(0x00)
#define SENSOR_DIGITAL_ENABLE							(0x01)

/*
 * This define depend on hardware circuit.
 * Note: please change it for respective.
 */
#define SENSOR_DIGITAL_HW_STATE_ACTIVE_HIGH				(0x00)
#define SENSOR_DIGITAL_HW_STATE_ACTIVE_LOW				(0x01)

#define SENSOR_DIGITAL_SW_STATE_RELEASE					(0x00)
#define SENSOR_DIGITAL_SW_STATE_SHORT_HOLD_ACTIVE		(0x01)
#define SENSOR_DIGITAL_SW_STATE_SHORT_RELEASE_ACTIVE	(0x02)
#define SENSOR_DIGITAL_SW_STATE_LONG_ACTIVE				(0x03)

typedef void (*pf_sensor_digital_ctrl)();
typedef uint8_t (*pf_sensor_digital_read)();
typedef void (*pf_sensor_digital_callback)(void*);

typedef struct {
	uint8_t id;
	uint8_t enable;
	uint8_t state;
	uint8_t state_active;
	uint8_t counter_enable;

	uint32_t counter;
	uint32_t unit;

	pf_sensor_digital_ctrl init;
	pf_sensor_digital_read read;

	pf_sensor_digital_callback callback;
} sensor_digital_t;

uint8_t	sensor_digital_init(sensor_digital_t* sensor_digital, uint8_t s_active, uint32_t u, uint8_t id, pf_sensor_digital_ctrl init, pf_sensor_digital_read read, pf_sensor_digital_callback callback);
void	sensor_digital_enable(sensor_digital_t* sensor_digital);
void	sensor_digital_disable(sensor_digital_t* sensor_digital);
void	sensor_digital_timer_polling(sensor_digital_t* sensor_digital);

#ifdef __cplusplus
}
#endif

#endif //__SENSOR_DIGITAL_H__
