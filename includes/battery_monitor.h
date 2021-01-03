#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <time.h>

#include "logger.h"
#include "emit_signal.h"
#include "notify.h"
#include "format.h"

extern const char *SUSPEND;
extern dbus_bool_t is_daemonized;

void
*battery_monitor(void *data);
