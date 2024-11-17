CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/udp
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/ascii
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/tcp
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/rtu
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include/common
CPPFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include/internal

CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/udp
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/ascii
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/tcp
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/rtu
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include/common
CFLAGS	+= -I./sources/networks/mbmaster-v2.9.6/mbmaster/include/internal


VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/common
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/udp
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/ascii
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/tcp
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/rtu
VPATH += sources/networks/mbmaster-v2.9.6/mbmaster/functions

SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/common/mbutils.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/mbm.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/udp/mbmudp.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/ascii/mbmascii.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/tcp/mbmtcp.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/rtu/mbmcrc.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/rtu/mbmrtu.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncfiles.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncholding.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfunccustom1.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfunccoils.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncraw.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncdisc.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncinput.c
SOURCES  += sources/networks/mbmaster-v2.9.6/mbmaster/functions/mbmfuncslaveid.c
