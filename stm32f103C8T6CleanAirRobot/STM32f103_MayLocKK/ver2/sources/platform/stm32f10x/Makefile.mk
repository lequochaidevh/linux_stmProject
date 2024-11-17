-include sources/platform/stm32f10x/Libraries/CMSIS/Makefile.mk
-include sources/platform/stm32f10x/Libraries/STM32F10x_StdPeriph_Driver/Makefile.mk
-include sources/platform/stm32f10x/arduino/Makefile.mk

LDFILE = sources/platform/stm32f10x/ak.ld

CFLAGS += -I./sources/platform/stm32f10x

VPATH += sources/platform/stm32f10x

# C source files
SOURCES += sources/platform/stm32f10x/stm32.c
SOURCES += sources/platform/stm32f10x/system.c
SOURCES += sources/platform/stm32f10x/sys_cfg.c
SOURCES += sources/platform/stm32f10x/io_cfg.c

# C++ source files
SOURCES_CPP += sources/platform/stm32f10x/mini_cpp.cpp
