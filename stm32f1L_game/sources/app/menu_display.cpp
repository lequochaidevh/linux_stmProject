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
#include "menu_display.h"
#include "menu_options_list.h"
#define Game_ID 3
#define LEFT_def 1
#define RIGHT_def 2
uint8_t speed_ball = 3;

#define LCD_WIDTH	124
#define LCD_HEIGHT	60


void scr_startup_handle_wait(ak_msg_t* msg);
void scr_idle_handle_menu(ak_msg_t* msg);
void scr_acc_handle_menu(ak_msg_t* msg);

void view_scr_start_wait(void);
void view_scr_idle_menu();
void view_scr_acc_menu();


scr_mng_t scr_mng_app_menu;

view_dynamic_t dyn_view_startup_wait = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_start_wait
};
view_dynamic_t dyn_view_idle_menu = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_idle_menu
};
view_dynamic_t dyn_view_acc_menu = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_acc_menu
};
/*----------[[[SCREEN]]]------------*/
view_screen_t scr_startup_wait = {
    &dyn_view_startup_wait,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
view_screen_t scr_idle_menu = {
    &dyn_view_idle_menu,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

view_screen_t scr_acc_menu = {
    &dyn_view_acc_menu,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};


class space_tank{
public:
    static int total_t;
    int id, x=(LCD_WIDTH/2)-15, y=LCD_HEIGHT-6, state_mv = RIGHT_def, w=30, h=3;

    space_tank(){


    }
    void moving(){
        if(state_mv == LEFT_def){
            if(x < 0){}
            else {
                x = x - 5;
            }
        }
        else {
            if(x+w > LCD_WIDTH){}
            else {
                x = x + 5;
            }
        }

    }
};
class ball_acc {
    // rand from a to b
    // (rand() % (b - a + 1)) + a
public:
    static int total;
    int id, x, y, slope, axis_x, axis_y, radius;
    int x_tank,y_tank,w_tank;
    ball_acc() {
        axis_x = 1;
        axis_y = 1;
        slope = (rand() % (31)) - 15;
        radius = 5;
        x = rand() % (LCD_WIDTH - radius);
        y = rand() % (LCD_HEIGHT - radius);
    }

    int distance(ball_acc& __ball) {
        uint8_t dx, dy;
        dx = abs(x - __ball.x);
        dy = abs(y - __ball.y);
        return sqrt(dx*dx + dy*dy);
    }

    bool is_hit_to_other(ball_acc& __ball) {
        if ((radius + __ball.radius) <= distance(__ball)) {
            return true;
        }
        else {
            return false;
        }
    }
    void insert_tank(space_tank ptr_tank){
        x_tank = ptr_tank.x;
        y_tank = ptr_tank.y;
        w_tank = ptr_tank.w;
    }
    void moving() {
        if( axis_x > 0) {
            x = x + speed_ball;
        }
        else {
            x = x - speed_ball;
        }

        if (axis_y > 0) {
            y += speed_ball * atan(slope);
        }
        else {
            y -= speed_ball * atan(slope);
        }

        if (x > (LCD_WIDTH - radius) || x < radius) {
            axis_x = -axis_x;
            if (x < radius) {
                x = radius;
            }
        }
        /*
        if (y > (LCD_HEIGHT - radius) || y < radius ) {
            axis_y = -axis_y;
            if (y < radius) {
                y = radius;
            }
        }*/
        if ((y + radius) >= (y_tank-2) && (y + radius) <= (y_tank) &&   \
                (x-radius/2) >= x_tank && (x+radius/2) <= (x_tank+w_tank))
        {
            axis_y = -axis_y;
            if (y < radius) {
                y = radius;
            }
        }
        if (y < radius ) {
            axis_y = -axis_y;
            if (y < radius) {
                y = radius;
            }
        }
        if (y > (LCD_HEIGHT - radius) ) {
            axis_y = -axis_y;
            if (y < radius) {
                y = radius;
            }
            APP_DBG_SIG("\n\n\n   YOU CLOSE   \n\n\n");
        }

    }
};


/*-------------- [[[ IDLE SCREEN]]] ------------*/
vector<ball_acc> v_acc_idle_ball;
vector<space_tank> v_acc_idle_tank;
int ball_acc::total;
int space_tank::total_t;

void view_scr_start_wait(void) {
#if 0

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
    // text display tests
    view_render.clear();
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(20,16);
    view_render.setTextSize(3);
    view_render.println("DEV H");
    view_render.setTextSize(1);
    view_render.println("");
    view_render.setTextColor(WHITE);
    view_render.println(" Press to enter menu!");
    view_render.update();

#endif
}
void view_scr_idle_menu() {
    MenuDisplay(ptr_menu, select_menu);
}
void view_scr_acc_menu(){
    for(ball_acc _ball : v_acc_idle_ball) {
        view_render.drawCircle(_ball.x, _ball.y, _ball.radius,WHITE);
    }
    for(space_tank _tank : v_acc_idle_tank){
    view_render.drawRect(_tank.x,_tank.y,_tank.w,_tank.h,WHITE);
    }
}

void scr_startup_handle_wait(ak_msg_t* msg) {
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
       // timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
        SCREEN_TRAN(scr_idle_handle_menu, &scr_idle_menu);
    }
        break;

    /*case AC_DISPLAY_SHOW_IDLE: {
        APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
        SCREEN_TRAN(scr_idle_handle, &scr_idle);
    }
        break;

    case AC_DISPLAY_SHOW_FW_UPDATE: {
        APP_DBG_SIG("AC_DISPLAY_SHOW_FW_UPDATE\n");
        SCREEN_TRAN(scr_fw_updating_handle, &scr_fw_updating);
    }*/

    default:
        break;
    }
}

void scr_idle_handle_menu(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRY\n");

        /*timer_set(AC_TASK_DISPLAY_ID, \
                  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, \
                  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL, \
                  TIMER_PERIODIC);*/
    }
        break;

    case AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE: {
        APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE\n");


    }
        break;

    case AC_DISPLAY_BUTON_MODE_RELEASED: {
        APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_RELEASED\n");
        action_menu();
        if(ptr_menu->menuID == Game_ID){
            SCREEN_TRAN(scr_acc_handle_menu, &scr_acc_menu);
        }
        //timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
        //SCREEN_TRAN(scr_acc_handle_menu, &scr_acc_menu);
    }
        break;

    case AC_DISPLAY_BUTON_UP_RELEASED: {
        APP_DBG_SIG("AC_DISPLAY_BUTON_UP_RELEASED\n");
        select_menu = (select_menu == 1)?3:select_menu-1;
    }
        break;

    case AC_DISPLAY_BUTON_DOWN_RELEASED: {
        APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_RELEASED\n");
        select_menu = (select_menu == 3)?1:select_menu+1;
    }
        break;

    default:
        break;
    }
}
#define AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL	60000	/* 60s */
#define AC_DISPLAY_UPDATE_REQ_ACC_INTERVAL			500		/* 500ms */

void scr_acc_handle_menu(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRYabc\n");

        if (v_acc_idle_ball.empty()) {
            ball_acc new_ball;
            new_ball.id = ball_acc::total++;
            v_acc_idle_ball.push_back(new_ball);
        }
        if (v_acc_idle_tank.empty()) {
            space_tank new_tank;
            new_tank.id = space_tank::total_t++;
            v_acc_idle_tank.push_back(new_tank);
        }
        v_acc_idle_ball[0].insert_tank(v_acc_idle_tank[0]);
        timer_set(AC_TASK_DISPLAY_ID, \
                  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, \
                  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL, \
                  TIMER_PERIODIC);

        //timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_UPDATE_REQ, 1000, TIMER_PERIODIC);
        //timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
    }
        break;
    case AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE: {
        APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE\n");
        v_acc_idle_tank[0].moving();
        v_acc_idle_ball[0].insert_tank(v_acc_idle_tank[0]);
        v_acc_idle_ball[0].moving();
        /*for (unsigned int i = 0; i < v_acc_idle_ball.size(); i++) {
            v_acc_idle_ball[i].moving();
        }*/

    }
        break;
    case AC_DISPLAY_TIMEOUT_PROTECT_SCR: {
        APP_DBG_SIG("AC_DISPLAY_TIMEOUT_PROTECT_SCR\n");
        view_render.display_off();
    }
        break;

    case AC_DISPLAY_BUTON_MODE_RELEASED: {
        view_render.display_on();
        //task_post_pure_msg(AC_TASK_AIRCOND_SCENE_ID, AC_AIRCOND_SCENE_MODE_AUTO_REQ);
        //timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_TIMEOUT_PROTECT_SCR, AC_DISPLAY_TIMEOUT_PROTECT_SCR_ACC_INTERVAL, TIMER_ONE_SHOT);
    }
        break;

    case AC_DISPLAY_BUTON_UP_RELEASED: {
        v_acc_idle_tank[0].state_mv = LEFT_def;
    }
        break;

    case AC_DISPLAY_BUTON_DOWN_RELEASED: {
        v_acc_idle_tank[0].state_mv = RIGHT_def;
    }
        break;

    default:
        break;
    }
}
