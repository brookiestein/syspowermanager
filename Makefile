CC 		:= gcc
HEADERS 	:= includes
WARNINGS 	:= -Wall -Wextra
CFLAGS 		:= -O2 -pipe -march=native -m64 -std=c99 -pthread
CFLAGS 		+= $(shell pkg-config --cflags glib-2.0 gtk+-3.0 --libs gtk+-3.0 dbus-1)
CFLAGS 		+= $(WARNINGS) -iquote $(HEADERS)
PROGRAM_NAME 	:= spm
PREFIX 		:= /usr/local/bin
SHARE 		:= /usr/share
CP 		:= cp -f -r
MV 		:= mv -f
RM 		:= rm -f -r
MKDIR 		:= mkdir -p
OBJECTS_SRC 	:= objects
RESOURCES 	:= resources

SOURCES 	:= src/main.c \
		src/emit_signal.c \
		src/logger.c \
		src/show_message.c \
		src/lid.c \
		src/battery_monitor.c \
		src/gui.c

OBJECTS 	:= ${SOURCES:.c=.o}

all : mkdirobj $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROGRAM_NAME)
	@$(MV) $(OBJECTS) $(OBJECTS_SRC)

.PHONY : mkdirobj
mkdirobj :
	@$(MKDIR) $(OBJECTS_SRC)

.PHONY : clean
clean :
	@echo "Cleaning..."
	@$(RM) $(OBJECTS_SRC)
	@echo "Cleaning finished."

.PHONY : install
install :
	@echo "Installing..."
	@$(MKDIR) $(SHARE)/$(PROGRAM_NAME)
	@$(CP) $(RESOURCES) $(SHARE)/$(PROGRAM_NAME)
	@$(CP) $(PROGRAM_NAME) $(PREFIX)
	@echo "Installation finished."

.PHONY : uninstall
uninstall :
	@echo "Uninstalling..."
	@$(RM) $(PREFIX)/$(PROGRAM_NAME)
	@$(RM) $(SHARE)/$(PROGRAM_NAME)
	@echo "Uninstallation finished."
