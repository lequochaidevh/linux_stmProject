#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "view_render.h"

#include "qrcode.h"

#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_display.h"
#include "task_aircond_scene.h"

#include <math.h>
#include <vector>

using namespace std;

#define LCD_WIDTH	124
#define LCD_HEIGHT	60

class ball {
	// rand from a to b
	// (rand() % (b - a + 1)) + a
public:
	static int total;
	int id, x, y, slope, axis_x, axis_y, radius;

	ball() {
		axis_x = 1;
		axis_y = 1;
		slope = (rand() % (31)) - 15;
		radius = (rand() % (7)) + 6;
		x = rand() % (LCD_WIDTH - radius);
		y = rand() % (LCD_HEIGHT - radius);
	}

	int distance(ball& __ball) {
		uint8_t dx, dy;
		dx = abs(x - __ball.x);
		dy = abs(y - __ball.y);
		return sqrt(dx*dx + dy*dy);
	}

	bool is_hit_to_other(ball& __ball) {
		if ((radius + __ball.radius) <= distance(__ball)) {
			return true;
		}
		else {
			return false;
		}
	}

	void moving() {
		if( axis_x > 0) {
			x = x + 2;
		}
		else {
			x = x - 2;
		}

		if (axis_y > 0) {
			y += 2 * atan(slope);
		}
		else {
			y -= 2 * atan(slope);
		}

		if (x > (LCD_WIDTH - radius) || x < radius) {
			axis_x = -axis_x;
			if (x < radius) {
				x = radius;
			}
		}

		if (y > (LCD_HEIGHT - radius) || y < radius ) {
			axis_y = -axis_y;
			if (y < radius) {
				y = radius;
			}
		}
	}
};

scr_mng_t scr_mng_app;

/* list of screen handler */
void view_scr_startup();
void scr_startup_handle(ak_msg_t* msg);

void view_scr_idle();
void scr_idle_handle(ak_msg_t* msg);

void view_scr_acc();
void scr_acc_handle(ak_msg_t* msg);

void view_scr_fw_updating();
void scr_fw_updating_handle(ak_msg_t* msg);

/**
 ******************************************************************************
 * objects MAIN SCREEN
 *
 ******************************************************************************
 */

/*----------[[[ITERM]]]------------*/
view_dynamic_t dyn_view_startup = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_startup
};

view_dynamic_t dyn_view_idle = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_idle
};

view_dynamic_t dyn_view_acc = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
    view_scr_acc
};

view_dynamic_t dyn_view_fw_updating = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fw_updating
};

/*----------[[[SCREEN]]]------------*/
view_screen_t scr_startup = {
    &dyn_view_startup,
	ITEM_NULL,
    ITEM_NULL,

	.focus_item = 0,
};

view_screen_t scr_idle = {
    &dyn_view_idle,
	ITEM_NULL,
    ITEM_NULL,

	.focus_item = 0,
};

view_screen_t scr_fw_updating = {
    &dyn_view_fw_updating,
	ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

view_screen_t scr_acc = {
    &dyn_view_acc,
	ITEM_NULL,
    ITEM_NULL,

	.focus_item = 0,
};


void task_display(ak_msg_t* msg) {
	scr_mng_dispatch(msg);
}

/*-------------- [[[ START UP SCREEN]]] ------------*/
void view_scr_startup() {
#if 0
	/* link github */
	QRCode qrcode;
	uint8_t qrcodeData[qrcode_getBufferSize(3)];
	qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://github.com/thannt92/arm_m3_base.git");

	for (uint8_t y = 0; y < qrcode.size; y++) {
		for (uint8_t x = 0; x < qrcode.size; x++) {
			view_render.drawPixel(x, y, qrcode_getModule(&qrcode, x, y) ? 1 : 0);
		}
	}

	/* ak logo */
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(38, 3);
	view_render.print("   __    _  _ ");
	view_render.setCursor(38, 10);
	view_render.print("  /__\\  ( )/ )");
	view_render.setCursor(38, 20);
	view_render.print(" /(__)\\ (   (");
	view_render.setCursor(38, 30);
	view_render.print("(__)(__)(_)\\_)");
	view_render.setCursor(42, 42);
	view_render.print("Active Kernel");
#else
	/* ak logo */
#define AK_LOGO_AXIS_X	23
#define AK_LOGO_TEXT	(AK_LOGO_AXIS_X + 4)
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(AK_LOGO_AXIS_X, 3);
	view_render.print("   __    _  _ ");
	view_render.setCursor(AK_LOGO_AXIS_X, 10);
	view_render.print("  /__\\  ( )/ )");
	view_render.setCursor(AK_LOGO_AXIS_X, 20);
	view_render.print(" /(__)\\ (   (");
	view_render.setCursor(AK_LOGO_AXIS_X, 30);
	view_render.print("(__)(__)(_)\\_)");
	view_render.setCursor(AK_LOGO_TEXT, 42);
	view_render.print("Active Kernel");
	view_render.update();
	sys_ctrl_delay_ms (2000);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
//static const unsigned char PROGMEM logo16_glcd_bmp[] =
//{ 0B00000000, 0B11000000,
//  0B00000001, 0B11000000,
//  0B00000001, 0B11000000,
//  0B00000011, 0B11100000,
//  0B11110011, 0B11100000,
//  0B11111110, 0B11111000,
//  0B01111110, 0B11111111,
//  0B00110011, 0B10011111,
//  0B00011111, 0B11111100,
//  0B00001101, 0B01110000,
//  0B00011011, 0B10100000,
//  0B00111111, 0B11100000,
//  0B00111111, 0B11110000,
//  0B01111100, 0B11110000,
//  0B01110000, 0B01110000,
//  0B00000000, 0B00110000 };

	// miniature bitmap display
//	view_render.clear();
//	view_render.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
//	view_render.update();
//	sys_ctrl_delay_ms (2000);

	// text display tests
	view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(0,0);
	view_render.println("Hello, world!");
	view_render.setTextColor(BLACK, WHITE); // 'inverted' text
	view_render.println(3.141592);
	view_render.setTextSize(2);
	view_render.setTextColor(WHITE);
	view_render.print("0x"); view_render.println(0xDEADBEEF, HEX);
	view_render.update();

#endif
}

void scr_startup_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_DISPLAY_INITIAL: {
		APP_DBG_SIG("AC_DISPLAY_INITIAL\n");
		view_render.initialize();
		view_render_display_on();
//		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_LOGO_INTERVAL, TIMER_ONE_SHOT);
	}
		break;

	case AC_DISPLAY_BUTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	}
		break;

	case AC_DISPLAY_SHOW_IDLE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	}
		break;

	case AC_DISPLAY_SHOW_FW_UPDATE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_FW_UPDATE\n");
		SCREEN_TRAN(scr_fw_updating_handle, &scr_fw_updating);
	}

	default:
		break;
	}
}

/*-------------- [[[ IDLE SCREEN]]] ------------*/
vector<ball> v_idle_ball;
int ball::total;

void view_scr_idle() {
	for(ball _ball : v_idle_ball) {
		view_render.drawCircle(_ball.x, _ball.y, _ball.radius, 144);
	}
}

void scr_idle_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		if (v_idle_ball.empty()) {
			ball new_ball;
			new_ball.id = ball::total++;
			v_idle_ball.push_back(new_ball);
		}

		timer_set(AC_TASK_DISPLAY_ID, \
				  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, \
				  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL, \
				  TIMER_PERIODIC);
	}
		break;

	case AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE\n");
		for (unsigned int i = 0; i < v_idle_ball.size(); i++) {
			v_idle_ball[i].moving();
		}
	}
		break;

	case AC_DISPLAY_BUTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
		SCREEN_TRAN(scr_acc_handle, &scr_acc);
	}
		break;

	case AC_DISPLAY_BUTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_RELEASED\n");
		ball new_ball;
		new_ball.id = ball::total++;

		if (v_idle_ball.empty()) {
			timer_set(AC_TASK_DISPLAY_ID, \
					  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, \
					  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL, \
					  TIMER_PERIODIC);
		}

		v_idle_ball.push_back(new_ball);
	}
		break;

	case AC_DISPLAY_BUTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_RELEASED\n");
		if (v_idle_ball.size()) {
			ball::total--;
			v_idle_ball.pop_back();
		}

		if (v_idle_ball.empty()) {
			timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
		}
	}
		break;

	default:
		break;
	}
}

/*-------------- [[[ FW UPDATING SCREEN]]] ------------*/
void view_scr_fw_updating() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(30, 30);
	view_render.print("Updating ...");
}

void scr_fw_updating_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_DISPLAY_SHOW_FW_UPDATE_ERR: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_FW_UPDATE_ERR\n");
        SCREEN_TRAN(scr_startup_handle, &scr_startup);
		view_render_display_off();
		SCREEN_NONE_UPDATE_MASK();
	}
		break;

	default:
		break;
	}
}

/*-------------- [[[ ACC STATUS SCREEN]]] ------------*/
class line_segment {
public:
	int x0, x1;
	int distance() {
		return (x1 - x0);
	}
};

vector<vector<line_segment>> v_v_backup_chart(2);
vector<int> v_last_st_counter(2);

void view_scr_acc() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.drawRect(107, 1, 21, 63, 144);

	JsonArray zone_array = acc_doc["ACctrl"].as<JsonArray>();
	for (int i = 0; i < static_cast<int>(zone_array.size()); i++) {
		JsonObject zoneAttr = zone_array[i]["zAttr"];
		int k = 2400 / zoneAttr["stRange"].as<int>();

		view_render.drawRect(1, 1 + i * 25, 104, 26, 144);

		/* Zone name */
		view_render.setCursor(10, 5 + i * 25);
		view_render.print(zone_array[i]["zName"].as<const char*>());

		/* Temperature */
		view_render.setCursor(88, 13 + i * 25);

		int temperature_value = 0;
		int __temperature_value = 0;
		for (String __temperature_func : zoneAttr["tFunc"].as<JsonArray>()) {
			__temperature_value = table_string_relay_pop_indoor.get_exe(__temperature_func);
			if (__temperature_value > temperature_value) {
				temperature_value = __temperature_value;
			}
		}
		view_render.print(temperature_value);

		/* Mode */
		view_render.drawRect(71, 11 + i * 25, 11, 11, 144);
		view_render.setCursor(74, 13 + i * 25);
		view_render.print(zoneAttr["_mode"].as<const char*>()[0]);

		view_render.drawFastVLine(5, 15 + i * 25, 8, 144);
		view_render.drawFastVLine(46, 15 + i * 25, 8, 144);

		/* Reset backup chart */
		if (zoneAttr["_stCnt"].as<int>() < v_last_st_counter[i]) {
			v_v_backup_chart[i].clear();
		}

		v_last_st_counter[i] = zoneAttr["_stCnt"].as<int>();

		if (zoneAttr["_bNDT"].as<int>() != 0) {
			if (zoneAttr["_bNON"].as<int>() > static_cast<int>(v_v_backup_chart[i].size())) {
				line_segment __line_segment;
				__line_segment.x0 = v_last_st_counter[i];
				__line_segment.x1 = v_last_st_counter[i];
				v_v_backup_chart[i].push_back(__line_segment);
			}
			else {
				line_segment& __line_segment = v_v_backup_chart[i].back();
				__line_segment.x1 = v_last_st_counter[i];
			}
		}

		if (v_v_backup_chart[i].size()) {
			for (line_segment& __line_segment : v_v_backup_chart[i]) {
				view_render.drawFastHLine(6 + __line_segment.x0 * k / 60, \
										  16 + i * 25, \
										  __line_segment.distance() * k / 60, 144);
			}
		}

		view_render.setCursor(50, 6 + i * 25);
		view_render.print(zoneAttr["_bNOT"].as<int>() * 100 / zoneAttr["stRange"].as<int>());

		view_render.drawFastHLine(6, 21 + i * 25, v_last_st_counter[i] * k / 60, 144);

		view_render.setCursor(50, 15 + i * 25);
		view_render.print(v_last_st_counter[i] * 100 / zoneAttr["stRange"].as<int>());
	}

	/* 1 Pha AC status */
	if (table_string_relay_pop_indoor.get_exe("rIPMS0") == table_string_func::ARGV_ON) {
		view_render.fillRect(109, 10 + 0 * 15, 7, 4, 144);
	}
	else {
		view_render.drawRect(109, 10 + 0 * 15, 7, 4, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rIPMS1") == table_string_func::ARGV_ON) {
		view_render.fillRect(109, 10 + 1 * 15, 7, 4, 144);
	}
	else {
		view_render.drawRect(109, 10 + 1 * 15, 7, 4, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rIPMS2") == table_string_func::ARGV_ON) {
		view_render.fillRect(109, 10 + 2 * 15, 7, 4, 144);
	}
	else {
		view_render.drawRect(109, 10 + 2 * 15, 7, 4, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rIPMS3") == table_string_func::ARGV_ON) {
		view_render.fillRect(109, 10 + 3 * 15, 7, 4, 144);
	}
	else {
		view_render.drawRect(109, 10 + 3 * 15, 7, 4, 144);
	}

	/* 3 Pha AC status */
	if (table_string_relay_pop_indoor.get_exe("rEACC0") == table_string_func::ARGV_ON) {
		view_render.fillRect(119, 7 + 0 * 15, 7, 7, 144);
	}
	else {
		view_render.drawRect(119, 7 + 0 * 15, 7, 7, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rEACC1") == table_string_func::ARGV_ON) {
		view_render.fillRect(119, 7 + 1 * 15, 7, 7, 144);
	}
	else {
		view_render.drawRect(119, 7 + 1 * 15, 7, 7, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rEACC2") == table_string_func::ARGV_ON) {
		view_render.fillRect(119, 7 + 2 * 15, 7, 7, 144);
	}
	else {
		view_render.drawRect(119, 7 + 2 * 15, 7, 7, 144);
	}

	if (table_string_relay_pop_indoor.get_exe("rEACC3") == table_string_func::ARGV_ON) {
		view_render.fillRect(119, 7 + 3 * 15, 7, 7, 144);
	}
	else {
		view_render.drawRect(119, 7 + 3 * 15, 7, 7, 144);
	}
}

#define AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL	60000	/* 60s */
#define AC_DISPLAY_UPDATE_REQ_ACC_INTERVAL			500		/* 500ms */

void scr_acc_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_UPDATE_REQ, 1000, TIMER_PERIODIC);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
	}
		break;

	case AC_DISPLAY_TIMEOUT_PROTECT_SCR: {
		APP_DBG_SIG("AC_DISPLAY_TIMEOUT_PROTECT_SCR\n");
		view_render.display_off();
	}
		break;

	case AC_DISPLAY_BUTON_MODE_RELEASED: {
		view_render.display_on();
		task_post_pure_msg(AC_TASK_AIRCOND_SCENE_ID, AC_AIRCOND_SCENE_MODE_AUTO_REQ);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
	}
		break;

	case AC_DISPLAY_BUTON_UP_RELEASED: {
		view_render.display_on();
		int T1 = table_string_relay_pop_indoor("sIPMSt1") + 1;
		int T2 = table_string_relay_pop_indoor("sIPMSt2") + 1;
		table_string_relay_pop_indoor.set_real(String("sIPMSt1"), T1);
		table_string_relay_pop_indoor.set_real(String("sIPMSt2"), T2);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
	}
		break;

	case AC_DISPLAY_BUTON_DOWN_RELEASED: {
		view_render.display_on();
		int T1 = table_string_relay_pop_indoor("sIPMSt1") - 1;
		int T2 = table_string_relay_pop_indoor("sIPMSt2") - 1;
		table_string_relay_pop_indoor.set_real(String("sIPMSt1"), T1);
		table_string_relay_pop_indoor.set_real(String("sIPMSt2"), T2);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
	}
		break;

	default:
		break;
	}
}
