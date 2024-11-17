CFLAGS += -I./sources/platform/stm32l/mbmaster_port

CPPFLAGS += -I./sources/platform/stm32l/mbmaster_port

VPATH += sources/platform/stm32l/mbmaster_port

# C source files
SOURCES += sources/platform/stm32l/mbmaster_port/mbportserial.c
SOURCES += sources/platform/stm32l/mbmaster_port/mbporttimer.c
SOURCES += sources/platform/stm32l/mbmaster_port/mbportevent.c
SOURCES += sources/platform/stm32l/mbmaster_port/mbportother.c
