#include "sensor_digital.h"

uint8_t	sensor_digital_init(sensor_digital_t* sensor_digital, uint8_t s_active, uint32_t u, uint8_t id, pf_sensor_digital_ctrl init, pf_sensor_digital_read read, pf_sensor_digital_callback callback) {
	sensor_digital->enable			=	SENSOR_DIGITAL_DISABLE;
	sensor_digital->id				=	id;
	sensor_digital->counter			=	0;
	sensor_digital->unit			=	u;
	sensor_digital->state			=	SENSOR_DIGITAL_SW_STATE_RELEASE;
	sensor_digital->state_active	=	s_active;

	sensor_digital->init		=	init;
	sensor_digital->read		=	read;
	sensor_digital->callback	=	callback;

	if (sensor_digital->init) {
		sensor_digital->init();
	}
	else {
		return SENSOR_DIGITAL_DRIVER_NG;
	}

	if (!sensor_digital->read) {
		return SENSOR_DIGITAL_DRIVER_NG;
	}

	if (!sensor_digital->callback) {
		return SENSOR_DIGITAL_DRIVER_NG;
	}

	return SENSOR_DIGITAL_DRIVER_OK;
}

void sensor_digital_enable(sensor_digital_t* sensor_digital) {
	sensor_digital->enable = SENSOR_DIGITAL_ENABLE;
}

void sensor_digital_disable(sensor_digital_t* sensor_digital) {
	sensor_digital->enable = SENSOR_DIGITAL_DISABLE;
}

void sensor_digital_timer_polling(sensor_digital_t* sensor_digital) {
	volatile uint8_t hw_sensor_digital_state;
	if (sensor_digital->enable == SENSOR_DIGITAL_ENABLE) {
		hw_sensor_digital_state = sensor_digital->read();

		/* hard sensor_digital pressed */
		if (hw_sensor_digital_state == sensor_digital->state_active) {

			if (sensor_digital->counter_enable == SENSOR_DIGITAL_ENABLE) {
				/* increase sensor_digital counter */
				sensor_digital->counter += sensor_digital->unit;

				/* check short press */
				if (sensor_digital->counter >= SENSOR_DIGITAL_SHORT_ACTIVE_MAX_TIME &&
						 sensor_digital->state != SENSOR_DIGITAL_SW_STATE_SHORT_HOLD_ACTIVE){
					sensor_digital->enable         = SENSOR_DIGITAL_DISABLE;

					sensor_digital->state          = SENSOR_DIGITAL_SW_STATE_SHORT_HOLD_ACTIVE;
					sensor_digital->callback(sensor_digital);

					sensor_digital->enable         = SENSOR_DIGITAL_ENABLE;
				}
			}
		}
		/* hard sensor_digital released */
		else {

			/* check short press */
			if (sensor_digital->counter < SENSOR_DIGITAL_SHORT_ACTIVE_MAX_TIME &&
					sensor_digital->counter >= SENSOR_DIGITAL_SHORT_ACTIVE_MIN_TIME &&
					sensor_digital->state != SENSOR_DIGITAL_SW_STATE_SHORT_RELEASE_ACTIVE){

				sensor_digital->enable         = SENSOR_DIGITAL_DISABLE;

				sensor_digital->state          = SENSOR_DIGITAL_SW_STATE_SHORT_RELEASE_ACTIVE;
				sensor_digital->callback(sensor_digital);
			}

			/* reset sensor_digital */
			sensor_digital->counter        = 0;
			sensor_digital->state          = SENSOR_DIGITAL_SW_STATE_RELEASE;
			sensor_digital->counter_enable = SENSOR_DIGITAL_ENABLE;
			sensor_digital->enable         = SENSOR_DIGITAL_ENABLE;
		}
	}
}

