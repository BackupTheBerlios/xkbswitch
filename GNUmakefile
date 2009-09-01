
PREFIX ?=	/usr/local
X11BASE ?=	/usr/X11R6

CC ?=		cc
RM ?=		rm -f

CC_VENDOR = \
	$(shell if $(CC) --version 2>&1 | grep -i gcc > /dev/null; then \
				echo gnu; \
			elif $(CC) -V 2>&1 | grep -i sun > /dev/null; then \
				echo sun; \
			fi)

CFLAGS :=
ifeq ($(CC_VENDOR), sun)
CFLAGS +=	-O2
CFLAGS +=	-Xa
CFLAGS +=	-xc99
else
CFLAGS +=	-Wall
CFLAGS +=	-W
CFLAGS +=	-std=c99
CFLAGS +=	-pedantic
CFLAGS +=	-O2
endif

CLIENT_TARGET :=	xkbswitch
CLIENT_CFLAGS :=
CLIENT_CFLAGS +=	`pkg-config --cflags dbus-1`
CLIENT_LIBS	:=
CLIENT_LIBS	+=	`pkg-config --libs dbus-1`
CLIENT_SRCS :=	xkbswitch.c

MODULE_TARGET :=	xkbswitch.so
MODULE_CFLAGS :=
MODULE_CFLAGS +=	-I$(X11BASE)/include
MODULE_CFLAGS +=	-shared
MODULE_CFLAGS +=	`pkg-config --cflags libxklavier`
MODULE_CFLAGS +=	`python-config --cflags`
MODULE_LIBS :=
MODULE_LIBS +=	-L$(X11BASE)/lib
MODULE_LIBS +=	`pkg-config --libs libxklavier`
MODULE_SRCS :=	xkbswitch_ext.c

EXTRA_CLEAN :=	xkbswitch.pyc

.PHONY: all clean dump-vars realclean

all: $(CLIENT_TARGET) $(MODULE_TARGET)

clean:
	$(RM) $(CLIENT_TARGET) $(MODULE_TARGET)

dump-vars:
	@echo CC = $(CC)
	@echo CC_VENDOR = $(CC_VENDOR)
	@echo CFLAGS = $(CFLAGS)
	@echo CLIENT_CFLAGS = $(CLIENT_CFLAGS)
	@echo CLIENT_LIBS = $(CLIENT_LIBS)
	@echo CLIENT_SRCS = $(CLIENT_SRCS)
	@echo CLIENT_TARGET = $(CLIENT_TARGET)
	@echo EXTRA_CLEAN = $(EXTRA_CLEAN)
	@echo MODULE_CFLAGS = $(MODULE_CFLAGS)
	@echo MODULE_LIBS = $(MODULE_LIBS)
	@echo MODULE_SRCS = $(MODULE_SRCS)
	@echo MODULE_TARGET = $(MODULE_TARGET)
	@echo PREFIX = $(PREFIX)
	@echo RM = $(RM)
	@echo SYS_NAME = $(SYS_NAME)
	@echo X11BASE = $(X11BASE)

realclean: clean
	$(RM) $(EXTRA_CLEAN)

$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $(CLIENT_CFLAGS) $^ -o $@ $(CLIENT_LIBS)

$(MODULE_TARGET): $(MODULE_SRCS)
	$(CC) $(CFLAGS) $(MODULE_CFLAGS) $^ -o $@ $(MODULE_LIBS)
