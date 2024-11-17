#ifndef MENU_OPTIONS_LIST_H
#define MENU_OPTIONS_LIST_H

#include "screen_manager.h"

typedef struct LinkerMENU_t Menu_t;
struct LinkerMENU_t {
    char menuID;
    char Title[20];
    char List1[20];
    LinkerMENU_t *MenuList1;    void (*Activation1)(char);
    char List2[20];
    LinkerMENU_t *MenuList2;    void (*Activation2)(char);
    char List3[20];
    LinkerMENU_t *MenuList3;
};
extern Menu_t MainMenu, Game, SensorsMenu, TempMenu, HumiMenu, ActuatorMenu, Relay1Menu\
            , Relay2Menu, SettingMenu, SoundMenu, VolumeMenu;
extern Menu_t *ptr_menu;
extern uint8_t select_menu;
extern void action_menu();
extern void MenuDisplay(Menu_t *,uint8_t);

#endif // MENU_OPTIONS_LIST_H



