#ifndef DBUS_H
#include <dbus/dbus.h>
#endif

#ifndef LOGGER_H
#include "logger.h"
#endif

#ifndef EMIT_SIGNAL_H
#define EMIT_SIGNAL_H 1
#endif

int
emit_signal(const char *method, const char *filepath);
