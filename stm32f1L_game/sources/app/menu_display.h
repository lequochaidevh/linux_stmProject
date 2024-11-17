#ifndef MENU_DISPLAY_H
#define MENU_DISPLAY_H

#include "screen_manager.h"


extern scr_mng_t scr_mng_app_menu;

extern void scr_startup_handle_wait(ak_msg_t* msg);

extern view_screen_t scr_startup_wait;

#endif // MENU_DISPLAY_H
