#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef DBUS_H
#include <dbus/dbus.h>
#endif

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef LOGGER_H
#include "logger.h"
#endif

#ifndef EMIT_SIGNAL_H
#include "emit_signal.h"
#endif

extern const char *SUSPEND;
extern dbus_bool_t is_daemonized;

void
*battery_monitor(void *data);
