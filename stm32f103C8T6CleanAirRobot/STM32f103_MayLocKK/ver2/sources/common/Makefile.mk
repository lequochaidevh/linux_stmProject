CFLAGS          += -I./sources/common
CPPFLAGS	+= -I./sources/common

VPATH += sources/common

SOURCES += sources/common/utils.c
SOURCES += sources/common/xprintf.c
SOURCES += sources/common/cmd_line.c
SOURCES += sources/common/log_queue.c
SOURCES += sources/common/fifo.c
SOURCES += sources/common/ring_buffer.c
