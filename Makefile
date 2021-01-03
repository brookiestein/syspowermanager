CC 		:= cc
CFLAGS 		:= -O2 -pipe -march=native -m64 -std=c99 -lnotify -Iincludes -Wall -Wextra
CFLAGS 		+= $(shell pkg-config --cflags --libs gtk+-3.0 dbus-1)
TARGET 		:= spm
PREFIX 		:= /usr/local/bin
SHARE 		:= /usr/share
CP 		:= cp -frv
MV 		:= mv -fv
RM 		:= rm -frv
MKDIR 		:= mkdir -pv
RESOURCES 	:= resources
SRC_DIR		:= src
SOURCES 	:= ${SRC_DIR}/main.c \
		${SRC_DIR}/emit_signal.c \
		${SRC_DIR}/logger.c \
		${SRC_DIR}/lid.c \
		${SRC_DIR}/battery_monitor.c \
		${SRC_DIR}/notify.c \
		${SRC_DIR}/gui.c
OBJECTS 	:= ${SOURCES:.c=.o}

all : ${OBJECTS}
	${CC} ${SOURCES} ${CFLAGS} -o ${TARGET}

.PHONY : clean
clean :
	${RM} ${OBJECTS} ${TARGET}

.PHONY : install
install :
	${MKDIR} ${SHARE}/${PROGRAM_NAME}
	${CP} ${RESOURCES} ${SHARE}/${TARGET}
	${CP} ${TARGET} ${PREFIX}

.PHONY : uninstall
uninstall :
	${RM} ${PREFIX}/${TARGET}
	${RM} ${SHARE}/${TARGET}
