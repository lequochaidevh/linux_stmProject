#ifndef __TASK_AIRCOND_SCENE_H__
#define __TASK_AIRCOND_SCENE_H__

#include "Arduino.h"
#include "WString.h"
#include "ArduinoJson.h"
#include "WString.h"
#include "xprintf.h"

#include <vector>

//#define TABLE_STRING_FUNC_DBG_EN
//#define TASK_AIRCOND_SCENS_DBG_EN

#if defined(TABLE_STRING_FUNC_DBG_EN)
#define TABLE_STRING_FUNC_DBG(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
#define TABLE_STRING_FUNC_DBG(fmt, ...)
#endif

#if defined(TASK_AIRCOND_SCENS_DBG_EN)
#define TASK_AIRCOND_SCENS_DBG(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
#define TASK_AIRCOND_SCENS_DBG(fmt, ...)
#endif

#define STRING_MODE_AUTO			"AUTO"
#define STRING_MODE_WARNING			"WARNING"
#define STRING_MODE_CRITICAL		"CRITICAL"
#define STRING_MODE_MANUAL			"MANUAL"

using namespace std;

class table_string_func {
public:
	enum {
		ARGV_NONE = 0,
		ARGV_ON,
		ARGV_OFF,
		ARGV_E_SIZE,
	} argv_e;

#if defined(TABLE_STRING_FUNC_DBG_EN)
	String argv_e_string[ARGV_E_SIZE] = {
		"ARGV_NONE",
		"ARGV_ON",
		"ARGV_OFF"
	};
#endif

	typedef int (*pf_string_func)(int);

	class pair_string_func {
	public:
		String _string;
		pf_string_func _func;
		int shadow_val;
		int exe_val;
	};

	table_string_func(vector<pair_string_func>& __table_string_func) \
		: _table_string_func(__table_string_func) {}

	int operator () (String str) {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			if (stPairStringFunc._string == str) {
				TABLE_STRING_FUNC_DBG("[shadow][read] %d = %s\n", \
									  stPairStringFunc.shadow_val, \
									  stPairStringFunc._string.c_str());
				return stPairStringFunc.shadow_val;
			}
		}
		return -1;
	}

	int operator () (String str, int i) {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			if (stPairStringFunc._string == str) {
				stPairStringFunc.shadow_val = i;
				TABLE_STRING_FUNC_DBG("[shadow][write] %s = %d\n", \
									  stPairStringFunc._string.c_str(), \
									  stPairStringFunc.shadow_val);
				return stPairStringFunc.shadow_val;
			}
		}
		return -1;
	}

	int get_exe(String str) {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			if (stPairStringFunc._string == str) {
				TABLE_STRING_FUNC_DBG("[exe][read] %d = %s\n", \
									  stPairStringFunc.exe_val, \
									  stPairStringFunc._string.c_str());
				return stPairStringFunc.exe_val;
			}
		}
		return -1;
	}

	int set_real(String str, int i) {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			if (stPairStringFunc._string == str) {
				stPairStringFunc.exe_val = stPairStringFunc._func(i);
				stPairStringFunc.shadow_val = stPairStringFunc.exe_val;
				TABLE_STRING_FUNC_DBG("[exe][write] %d = %s(%d)\n", \
									  stPairStringFunc.exe_val, \
									  stPairStringFunc._string.c_str(), \
									  i);
				return stPairStringFunc.exe_val;
			}
		}
		return -1;
	}

	void exe_shadow() {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			if (stPairStringFunc.shadow_val != stPairStringFunc.exe_val) {
				stPairStringFunc.exe_val = stPairStringFunc._func(stPairStringFunc.shadow_val);
				TABLE_STRING_FUNC_DBG("[exe_shadow] %d = %s(%d)\n", \
									  stPairStringFunc.exe_val, \
									  stPairStringFunc._string.c_str(), \
									  stPairStringFunc.shadow_val);
			}
		}
	}

	void force_exe_shadow() {
		for (pair_string_func& stPairStringFunc : _table_string_func) {
			stPairStringFunc.exe_val = stPairStringFunc._func(stPairStringFunc.shadow_val);
			TABLE_STRING_FUNC_DBG("[force_exe_shadow] %d = %s(%d)\n", \
								  stPairStringFunc.exe_val, \
								  stPairStringFunc._string.c_str(), \
								  stPairStringFunc.shadow_val);
		}
	}

protected:
	vector<pair_string_func>& _table_string_func;
};

#define STATICJSONDOCUMENT_ACC_CAPACITY (7*JSON_ARRAY_SIZE(1) + 6*JSON_ARRAY_SIZE(2) + 4*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(16) + 360)
extern StaticJsonDocument<STATICJSONDOCUMENT_ACC_CAPACITY> acc_doc;

extern table_string_func table_string_relay_pop_indoor;

/***
 * ACctrl
 * Air Condition Controller Json structure
 *
 * tFunc: name function to get temperature for zone control.
 * tBkL: temperature Backup Low.
 * tBkH: temperature Backup High.
 * bMOT: backup Maximum On Time.
 * bMON: backup Maximum On Number.
 * _bNOT: backup Now On Time.
 * _bNON: backup Now On Number.
 * _bNDT: backup Now Duty Time.
 * _stCnt: switch time Counter.
 * stRange: switch time Range.
 * stMutex: switch time Muxtex.
 * stM: switch time Machenism.
 * _stMidx: swithc time Machenism index.
 * group: group air conditioner in a zone.
 *
"_mode": "AUTO"
	   : "WARNING"
	   : "CRITICAL"
	   : "MANUAL"

{
	"ACctrl": [
		{
			"zName": "Zone1",
			"zAttr": {
				"_mode": "AUTO",
				"_modeCnt": 0,

				"tFunc": ["sIPMSt1", "sIPMSt2"],
				"tBkL": 24,
				"tBkH": 27,

				"bMOT": 60,
				"bMON": 3,
				"_bNOT": 0,
				"_bNON": 0,
				"_bNDT": 0,

				"_stCnt": 0,
				"stRange": 120,
				"stMutex": 30,

				"stM": [ [0, 1] , [1, 2], [2, 0] ],
				"_stMidx": 0,

				"group": [
					[ "rIPMS0" , "rIPMS1" ],
					[ "rEACC0" ],
					[ "rEACC1" ]
				]
			}
		}
	]
}
*/

#endif //__TASK_AIRCOND_SCENE_H__
