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
#include "menu_options_list.h"
#include "stdio.h"
#include "stddef.h"

//#define MENU_NULL (struct LinkerMENU_t*)0

#define MENU_ID_NULL ((char)0)

#define state_On    1
#define state_Off   0
enum    {
    Device_Relay1 = 1,
    Device_Relay2,
    Game_ID
};


uint8_t select_menu = 1;
void MenuDisplay(Menu_t *pt_menu, uint8_t select_mn);
void action_menu();
void ActuatorAction(char state);
void set_action(uint8_t select_mn);
/*********************************************/
Menu_t MainMenu = {
    MENU_ID_NULL,
    "     MAIN MENU     ",
    "   Game            ",&Game, nullptr,
    "   Actuator        ",&ActuatorMenu, nullptr,
    "   Setting         ",&SettingMenu,
};
/*********************************************/
/*********************************************/
Menu_t Game = {
    Game_ID,
    "     GAMEBALL      ",
    "                   ",&SensorsMenu, nullptr,
    "                   ",&ActuatorMenu, nullptr,
    "                   ",&SettingMenu,
};
/*********************************************/
Menu_t SensorsMenu = {
    MENU_ID_NULL,
    "      SENSORS      ",
    "   Temperature     ",&TempMenu, nullptr,
    "   Humidity        ",&HumiMenu, nullptr,
    "   Back            ",&MainMenu,
};
Menu_t TempMenu = {
    MENU_ID_NULL,
    "    TEMPERATURE    ",
    "   On              ",nullptr, nullptr,
    "   Off             ",nullptr, nullptr,
    "   Back            ",&SensorsMenu,
};
Menu_t HumiMenu = {
    MENU_ID_NULL,
    "      HUMIDITY     ",
    "   On              ",nullptr, nullptr,
    "   Off             ",nullptr, nullptr,
    "   Back            ",&SensorsMenu,
};
/*********************************************/
Menu_t ActuatorMenu = {
    Device_Relay1,
    "     ACTUARATOR    ",
    "   Relay1          ",&Relay1Menu, nullptr,
    "   Relay2          ",&Relay2Menu, nullptr,
    "   Back            ",&MainMenu,
};
Menu_t Relay1Menu = {
    Device_Relay1,
    "       RELAY 1     ",
    "   On              ",nullptr, &ActuatorAction,
    "   Off             ",nullptr, &ActuatorAction,
    "   Back            ",&ActuatorMenu,
};
Menu_t Relay2Menu = {
    Device_Relay2,
    "     ACTUARATOR    ",
    "   On              ",nullptr, &ActuatorAction,
    "   Off             ",nullptr, &ActuatorAction,
    "   Back            ",&ActuatorMenu,
};
/*********************************************/
Menu_t SettingMenu = {
    MENU_ID_NULL,
    "      SETTING      ",
    "   Sound           ",&SoundMenu, nullptr,
    "   Volume          ",&VolumeMenu, nullptr,
    "   Back            ",&MainMenu,
};
Menu_t SoundMenu = {
    MENU_ID_NULL,
    "       SOUND       ",
    "   On              ",nullptr, nullptr,
    "   Off             ",nullptr, nullptr,
    "   Back            ",&SettingMenu,
};
Menu_t VolumeMenu = {
    MENU_ID_NULL,
    "       VOLUME      ",
    "   Up              ",nullptr, nullptr,
    "   Down            ",nullptr, nullptr,
    "   Back            ",&SettingMenu,
};
/*********************************************/

Menu_t *ptr_menu = &MainMenu;

void MenuDisplay(Menu_t *pt_menu, uint8_t select_mn)  {
    view_render.setCursor(0,0);
    view_render.setTextSize(1);
    view_render.setTextColor(BLACK, WHITE);
    view_render.println(pt_menu->Title);
    view_render.setTextColor(WHITE);
    view_render.println(pt_menu->List1);
    view_render.println(pt_menu->List2);
    view_render.println(pt_menu->List3);

    view_render.setCursor(0,select_mn*8+1);
    view_render.print("->");

    view_render.update();
}
void action_menu(){
    set_action(select_menu);
    switch (select_menu) {
        case 1:{
            ptr_menu = (ptr_menu->MenuList1==nullptr)?ptr_menu:ptr_menu->MenuList1;
            select_menu = 1;
        }
        break;
        case 2:{
            ptr_menu = (ptr_menu->MenuList2==nullptr)?ptr_menu:ptr_menu->MenuList2;
            select_menu = 1;
        }
        break;
        case 3:{
            ptr_menu = (ptr_menu->MenuList3==nullptr)?ptr_menu:ptr_menu->MenuList3;
            select_menu = 1;
        }
        break;
        default: break;
    }

}

void set_action(uint8_t select_mn){
    switch (select_mn) {
    case 1:
        if (ptr_menu->Activation1 != nullptr) ptr_menu->Activation1(state_On);
        break;

    case 2:
        if (ptr_menu->Activation2 != nullptr) ptr_menu->Activation2(state_Off);
        break;
    default:
        break;

    }
}

void ActuatorAction(char status){
    switch (ptr_menu->menuID) {
        case Device_Relay1: if(status) APP_DBG_SIG("\nRelay1 On\n\n");
        else APP_DBG_SIG("\nRelay1 Off\n\n");
        break;
        case Device_Relay2: if(status) APP_DBG_SIG("\nRelay2 On\n\n");
        else APP_DBG_SIG("\nRelay 2 Off\n\n");
        break;
    }

}
