-include sources/platform/stm32f10x/Libraries/CMSIS/Makefile.mk
-include sources/platform/stm32f10x/Libraries/STM32F10x_StdPeriph_Driver/Makefile.mk

# header path
CPPFLAGS	+= -I./sources/platform/stm32f10x/arduino
CPPFLAGS	+= -I./sources/platform/stm32f10x/arduino/SPI

# source path
VPATH += sources/platform/stm32f10x/arduino
VPATH += sources/platform/stm32f10x/arduino/SPI

# CPP source files
SOURCES_CPP += sources/platform/stm32f10x/arduino/SPI/SPI.cpp

SOURCES_CPP += sources/platform/stm32f10x/arduino/wiring_digital.cpp
SOURCES_CPP += sources/platform/stm32f10x/arduino/wiring_shift.cpp
SOURCES_CPP += sources/platform/stm32f10x/arduino/Print.cpp
SOURCES_CPP += sources/platform/stm32f10x/arduino/Stream.cpp
