CFLAGS += -I./sources/platform/stm32f10x/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CFLAGS += -I./sources/platform/stm32f10x/Libraries/CMSIS/CM3/CoreSupport

CPPFLAGS += -I./sources/platform/stm32f10x/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CPPFLAGS += -I./sources/platform/stm32f10x/Libraries/CMSIS/CM3/CoreSupport

CPPFLAGS += -I./sources/platform/stm32f10x/
CFLAGS += -I./sources/platform/stm32f10x/

VPATH += sources/platform/stm32f10x/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x

SOURCES += ./sources/platform/stm32f10x/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
