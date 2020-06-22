#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef EMIT_SIGNAL_H
#include "emit_signal.h"
#endif

#ifndef LID_H
#define LID_H 1
#endif

/* Defined on main */
extern const char *SUSPEND;

void
*lid(void *data);
