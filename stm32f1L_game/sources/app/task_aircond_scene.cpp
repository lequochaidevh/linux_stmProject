#include "fsm.h"
#include "port.h"
#include "timer.h"
#include "message.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_aircond_scene.h"

#include "HardwareSerial.h"

using namespace std;

static int sIPMSt1(int);
static int sIPMSt2(int);
static int rIPMS0(int);
static int rIPMS1(int);
static int rIPMS2(int);
static int rIPMS3(int);
static int rEACC0(int);
static int rEACC1(int);
static int rEACC2(int);
static int rEACC3(int);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
vector<table_string_func::pair_string_func> relay_pop_indoor_strtbl {
	{ String("sIPMSt1")		,		sIPMSt1		},
	{ String("sIPMSt2")		,		sIPMSt2		},
	{ String("rIPMS0")		,		rIPMS0		},
	{ String("rIPMS1")		,		rIPMS1		},
	{ String("rIPMS2")		,		rIPMS2		},
	{ String("rIPMS3")		,		rIPMS3		},
	{ String("rEACC0")		,		rEACC0		},
	{ String("rEACC1")		,		rEACC1		},
	{ String("rEACC2")		,		rEACC2		},
	{ String("rEACC3")		,		rEACC3		},
};
#pragma GCC diagnostic pop

table_string_func table_string_relay_pop_indoor(relay_pop_indoor_strtbl);

const char* acc_json = "{\"ACctrl\":[{\"zName\":\"Zone1\",\"zAttr\":{\"_mode\":\"AUTO\",\"_modeCnt\":0,\"tFunc\":[\"sIPMSt1\",\"sIPMSt2\"],\"tBkL\":24,\"tBkH\":27,\"bMOT\":60,\"bMON\":3,\"_bNOT\":0,\"_bNON\":0,\"_bNDT\":0,\"_stCnt\":0,\"stRange\":120,\"stMutex\":30,\"stM\":[[0,1],[1,2],[2,0]],\"_stMidx\":0,\"group\":[[\"rIPMS0\",\"rIPMS1\"],[\"rEACC0\"],[\"rEACC1\"]]}}]}";

StaticJsonDocument<STATICJSONDOCUMENT_ACC_CAPACITY> acc_doc;

void task_aircond_scene(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_AIRCOND_SCENE_INIT: {
		APP_DBG_SIG("AC_AIRCOND_SCENE_INIT\n");

		deserializeJson(acc_doc, acc_json);

#if 1
		timer_set(AC_TASK_AIRCOND_SCENE_ID, \
				  AC_AIRCOND_SCENE_TIMER_SCAN, \
				  AC_AIRCOND_SCENE_TIMER_SCAN_INTERVAL, \
				  TIMER_PERIODIC);
#else
		timer_set(AC_TASK_AIRCOND_SCENE_ID, \
				  AC_AIRCOND_SCENE_TIMER_SCAN, \
				  3000, \
				  TIMER_PERIODIC);
#endif

		table_string_relay_pop_indoor.set_real(String("sIPMSt1"), 27);
		table_string_relay_pop_indoor.set_real(String("sIPMSt2"), 26);
	}
		break;

	case AC_AIRCOND_SCENE_TIMER_SCAN: {
		APP_DBG_SIG("AC_AIRCOND_SCENE_TIMER_SCAN\n");

		JsonArray zone_array = acc_doc["ACctrl"].as<JsonArray>();

		for (int i = 0; i < static_cast<int>(zone_array.size()); i++) {
			JsonObject js_zoneAttr = zone_array[i]["zAttr"];

			// Start define internal function.
			auto __function_control_all_group_in_acGroup = [&](int _control_req) {
				for (JsonArray __ac_group_array : js_zoneAttr["group"].as<JsonArray>()) {
					for (String __ac_group_item : __ac_group_array) {
						table_string_relay_pop_indoor(__ac_group_item, _control_req);
					}
				}
			};

			auto __function_control_group_in_acGroup = [&](int _group_index, int _control_req) {
				for (int __ac_group_index : js_zoneAttr["stM"][_group_index].as<JsonArray>()) {
					for (String __ac_group_item : js_zoneAttr["group"][__ac_group_index].as<JsonArray>()) {
						table_string_relay_pop_indoor(__ac_group_item, _control_req);
					}
				}
			};

			auto __function_control_next_group_in_acGroup = [&](int _control_req) {
				__function_control_group_in_acGroup((js_zoneAttr["_stMidx"].as<int>() + 1) % static_cast<int>(js_zoneAttr["group"].size()) , \
						_control_req);
			};

			auto __function_control_current_group_in_acGroup = [&](int _control_req) {
				__function_control_group_in_acGroup(js_zoneAttr["_stMidx"].as<int>(),\
						_control_req);
			};

			auto __function_set_js_zoneAttr_mode = [&](const char* __mode) {
				TASK_AIRCOND_SCENS_DBG("\"_mode\" %s -> %s\n", String(js_zoneAttr["_mode"].as<String>()).c_str(), __mode);
				if (__mode != js_zoneAttr["_mode"]) {
					js_zoneAttr["_modeCnt"] = 0;
				}
				js_zoneAttr["_mode"] = __mode;
			};
			// End define internal function.

			/**********************
			 * Temperature control
			 **********************/
			int temperature_value = 0;
			int __temperature_value = 0;
			for (String __temperature_func : js_zoneAttr["tFunc"].as<JsonArray>()) {
				__temperature_value = table_string_relay_pop_indoor.get_exe(__temperature_func);
				if (__temperature_value > temperature_value) {
					temperature_value = __temperature_value;
				}
			}

			if (temperature_value >= js_zoneAttr["tBkH"].as<int>()) {
				if (js_zoneAttr["_bNDT"].as<int>() == 0) {
					/* Turn ON all air condition group */
					__function_control_all_group_in_acGroup(table_string_func::ARGV_ON);

					js_zoneAttr["_bNON"] = js_zoneAttr["_bNON"].as<int>() + 1;

					if (js_zoneAttr["_bNON"].as<int>() >= js_zoneAttr["bMON"].as<int>()) {
						__function_set_js_zoneAttr_mode(STRING_MODE_CRITICAL);
					}
					else if (js_zoneAttr["_mode"] == STRING_MODE_AUTO) {
						__function_set_js_zoneAttr_mode(STRING_MODE_WARNING);
					}
				}
			}
			else if (temperature_value <= js_zoneAttr["tBkL"].as<int>()) {
				if (js_zoneAttr["_mode"] == STRING_MODE_WARNING) {
					js_zoneAttr["_bNDT"] = 0;
					__function_set_js_zoneAttr_mode(STRING_MODE_AUTO);
					__function_control_all_group_in_acGroup(table_string_func::ARGV_OFF);
					__function_control_current_group_in_acGroup(table_string_func::ARGV_ON);
				}
			}

			/***************
			 * Mode control
			 ***************/
			/* AUTO */
			if (js_zoneAttr["_mode"] == STRING_MODE_AUTO) {

				/* Check firt time restart AUTO mode */
				if ( js_zoneAttr["_modeCnt"].as<int>() == 0 && \
					 js_zoneAttr["_bNOT"].as<int>() == 0 && \
					 js_zoneAttr["_bNON"].as<int>() == 0 && \
					 js_zoneAttr["_bNDT"].as<int>() == 0 && \
					 js_zoneAttr["_stCnt"].as<int>() == 0 && \
					 js_zoneAttr["_stMidx"].as<int>() == 0) {
					__function_control_all_group_in_acGroup(table_string_func::ARGV_OFF);
					__function_control_current_group_in_acGroup(table_string_func::ARGV_ON);
				}

				/* Check turn on next group */
				if (js_zoneAttr["_stCnt"].as<int>() >= (js_zoneAttr["stRange"].as<int>() - js_zoneAttr["stMutex"].as<int>())) {
					__function_control_next_group_in_acGroup(table_string_func::ARGV_ON);
				}
			}

			/* WAIRING */
			else if (js_zoneAttr["_mode"] == STRING_MODE_WARNING) {
				js_zoneAttr["_bNDT"] = js_zoneAttr["_bNDT"].as<int>() + 1;
				js_zoneAttr["_bNOT"] = js_zoneAttr["_bNOT"].as<int>() + 1;
			}

			/* CRITICAL */
			else if (js_zoneAttr["_mode"] == STRING_MODE_CRITICAL) {
				js_zoneAttr["_bNDT"] = js_zoneAttr["_bNDT"].as<int>() + 1;
				js_zoneAttr["_bNOT"] = js_zoneAttr["_bNOT"].as<int>() + 1;
			}

			/* check exception */
			else {

			}

			/***************
			 * Time control
			 ***************/
			js_zoneAttr["_stCnt"] = js_zoneAttr["_stCnt"].as<int>() + 1;
			js_zoneAttr["_modeCnt"] = js_zoneAttr["_modeCnt"].as<int>() + 1;

			/* Turn around switch time counter */
			if (js_zoneAttr["_stCnt"].as<int>() >= js_zoneAttr["stRange"].as<int>()) {
				/* Reset switch time counter  */
				js_zoneAttr["_stCnt"] = 0;

				/* Reset total backup time in a cycle */
				js_zoneAttr["_bNOT"] = 0;

				/* Reset number backup time in a cycle */
				js_zoneAttr["_bNON"] = 0;

				/* Reset backup duty time in a cycle */
				js_zoneAttr["_bNDT"] = 0;

				/* Switch to next machenism array index */
				if (js_zoneAttr["_mode"] == STRING_MODE_AUTO) {
					/* Increase machenism index */
					js_zoneAttr["_stMidx"] = (js_zoneAttr["_stMidx"].as<int>() + 1) % static_cast<int>(js_zoneAttr["group"].size());

					__function_control_all_group_in_acGroup(table_string_func::ARGV_OFF);
					__function_control_current_group_in_acGroup(table_string_func::ARGV_ON);
				}
			}

			/* Check backup time */
			if (js_zoneAttr["_bNOT"].as<int>() >= js_zoneAttr["bMOT"].as<int>()) {
				if (js_zoneAttr["_mode"] != STRING_MODE_CRITICAL) {
					__function_set_js_zoneAttr_mode(STRING_MODE_CRITICAL);
				}
			}

			table_string_relay_pop_indoor.exe_shadow();
			/*** Debug Json Struct *********************************
			 * {
			 * 	"zName": "Zone1",
			 * 	"zAttr": {
			 * 		"_mode": "AUTO",
			 *		"_modeCnt": 0,
			 *
			 * 		"tFunc": "sIPMSt1",
			 * 		"tBkL": 24,
			 * 		"tBkH": 27,
			 *
			 * 		"bMOT": 160,
			 * 		"bMON": 3,
			 * 		"_bNOT": 0,
			 * 		"_bNON": 0,
			 *
			 * 		"_stCnt": 0,
			 * 		"stRange": 480,
			 * 		"stMutex": 30,
			 *
			 * 		"stM": [ [0, 1] , [1, 2], [2, 0] ],
			 * 		"_stMidx": 0,
			 *
			 * 		"group": [
			 * 			[ "rIPMS0" , "rIPMS2" ],
			 * 			[ "rEACC0" ],
			 * 			[ "rIPMS1" ]
			 * 		]
			 * 	}
			 * }
			 ***************************************************/
			TASK_AIRCOND_SCENS_DBG("zoneName: %s {\n", zone_array[i]["zName"].as<const char*>());
			TASK_AIRCOND_SCENS_DBG("\t_mode: %s\n", js_zoneAttr["_mode"].as<const char*>());
			TASK_AIRCOND_SCENS_DBG("\t_modeCounter: %d\n", js_zoneAttr["_modeCnt"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\ttFuncCall: %s\n", js_zoneAttr["tFunc"].as<const char*>());
			TASK_AIRCOND_SCENS_DBG("\ttBackupLow: %d\n", js_zoneAttr["tBkL"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\ttBackupHigh: %d\n", js_zoneAttr["tBkH"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\tbMaxOnTime: %d\n", js_zoneAttr["bMOT"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\tbMaxOnNumber: %d\n", js_zoneAttr["bMON"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\t_bNowDutyTime: %d\n", js_zoneAttr["_bNDT"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\t_bNowOnTime: %d\n", js_zoneAttr["_bNOT"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\t_bNowOnNumber: %d\n", js_zoneAttr["_bNON"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\t_stCounter: %d\n", js_zoneAttr["_stCnt"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\tstInterval: %d\n", js_zoneAttr["stRange"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\tstMutex: %d\n", js_zoneAttr["stMutex"].as<int>());
			TASK_AIRCOND_SCENS_DBG("\t_stMachenismIndex: %d\n", js_zoneAttr["_stMidx"].as<int>());
			TASK_AIRCOND_SCENS_DBG("%s\n", "{");
		}
	}
		break;

	case AC_AIRCOND_SCENE_MODE_AUTO_REQ: {
		APP_DBG_SIG("AC_AIRCOND_SCENE_MODE_AUTO_REQ\n");

		JsonArray zone_array = acc_doc["ACctrl"].as<JsonArray>();
		for (JsonObject zone : zone_array) {
			JsonObject js_zoneAttr = zone["zAttr"];
			js_zoneAttr["_mode"] = STRING_MODE_AUTO;
			js_zoneAttr["_modeCnt"] = 0;
			js_zoneAttr["_bNOT"] = 0;
			js_zoneAttr["_bNON"] = 0;
			js_zoneAttr["_bNDT"] = 0;
			js_zoneAttr["_stCnt"] = 0;
			js_zoneAttr["_stMidx"] = 0;
		}
	}
		break;

	case AC_AIRCOND_SCENE_MODE_MANUAL_REQ: {
		APP_DBG_SIG("AC_AIRCOND_SCENE_MODE_MANUAL_REQ\n");
	}
		break;

	default:
		break;
	}
}

int sIPMSt1(int i) {
	return i;
}

int sIPMSt2(int i) {
	return i;
}

int rIPMS0(int i) {
	return i;
}

int rIPMS1(int i) {
	return i;
}

int rIPMS2(int i) {
	return i;
}

int rIPMS3(int i) {
	return i;
}

int rEACC0(int i) {
	return i;
}

int rEACC1(int i) {
	return i;
}

int rEACC2(int i) {
	return i;
}

int rEACC3(int i) {
	return i;
}
