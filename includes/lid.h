#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "emit_signal.h"

#define LID_H 1

/* Defined on main */
extern const char *SUSPEND;

void *
lid(void *data);
