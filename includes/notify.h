#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <stdio.h>

#include "logger.h"

#define NOTIFY_H 1

int
notify_send(const char *body, const char *logstream, NotifyUrgency urgency);
