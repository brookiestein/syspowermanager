#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "get_time.h"
#include "logger.h"
#include "emit_signal.h"
#include "notify.h"
#include "format.h"

extern const char *SUSPEND;

void *
battery_monitor(void *data);
