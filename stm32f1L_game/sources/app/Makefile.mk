CFLAGS		+= -I./sources/app
CPPFLAGS	+= -I./sources/app

VPATH += sources/app

# CPP source files
SOURCES_CPP += sources/app/task_system.cpp
SOURCES_CPP += sources/app/app.cpp
SOURCES_CPP += sources/app/app_data.cpp
SOURCES_CPP += sources/app/app_non_clear_ram.cpp
SOURCES_CPP += sources/app/app_bsp.cpp
SOURCES_CPP += sources/app/rf_remote_ctrl.cpp
SOURCES_CPP += sources/app/shell.cpp

SOURCES_CPP += sources/app/task_shell.cpp
SOURCES_CPP += sources/app/task_life.cpp
SOURCES_CPP += sources/app/task_fw.cpp
SOURCES_CPP += sources/app/task_list.cpp
SOURCES_CPP += sources/app/task_display.cpp

#/****************MENU devh******************/

SOURCES_CPP += sources/app/menu_options_list.cpp
SOURCES_CPP += sources/app/menu_display.cpp

ifeq ($(TASK_ZIGBEE_OPTION),-DTASK_ZIGBEE_EN)
SOURCES_CPP += sources/app/task_zigbee.cpp
endif

SOURCES_CPP += sources/app/task_if.cpp
SOURCES_CPP += sources/app/task_rf24_if.cpp
SOURCES_CPP += sources/app/task_uart_if.cpp
SOURCES_CPP += sources/app/task_dbg.cpp
SOURCES_CPP += sources/app/task_aircond_scene.cpp
