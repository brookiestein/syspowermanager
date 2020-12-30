#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <argp.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "logger.h"
#include "emit_signal.h"
#include "lid.h"
#include "battery_monitor.h"
#include "gui.h"

const char *argp_program_version = "SPM v1.0.6";

static const char doc[] = "\nSystem Power Manager is a \
software piece that serves\n\
to manage some power options, like Shutdown or Reboot.\n\
The objective of this is offer you a little interface\n\
through which you can: Shutdown, reboot, suspend and/or\n\
hibernate your computer without root permissions.\n\
Also, you can monitor your battery's charge percentage.\n\
By way of if SPM detects that it's less than 15% then it\n\
will put your computer to sleep, and so avoid that you\n\
lose important data.\v\
This software is distributed under the terms of the GNU GPL v2 license.\n\
You can find this project at: https://github.com/brookiestein/syspowermanager";

static const char daemon_info[]         = "SPM will works like a daemon.";
static const char file_info[]           = "File where save the log. (Only for -v)";
static const char lid_info[]            = "SPM will monitors the laptop's lid to suspend\n\
the system if detects that it has been closed.";
static const char hbt_info[]            = "Hibernates the system.";
static const char pof_info[]            = "Turns off the system.";
static const char rst_info[]            = "Restarts the system.";
static const char ssp_info[]            = "Suspends the system.";
static const char time_info[]           = "Does one of either hibernate, poweroff, restart\n\
or suspend after $seconds had finished.";

static const char monitor_info[]        = "SPM will monitors the battery's charge percentage\n\
and, if it detects that it's less than 15% then it will put your \
computer to sleep.";

static const char verbose_info[]        = "Show the charge percentage every 20 seconds.\n\
(Only for -m)";

static struct argp_option options[] = {
        { "daemon",     'd', 0,         0,                      daemon_info,    0 },
        { "file",       'f', "file",    OPTION_ARG_OPTIONAL,    file_info,      1 },
        { "hibernate",  'h', 0,         0,                      hbt_info,       2 },
        { "lid",        'l', 0,         0,                      lid_info,       3 },
        { "monitor",    'm', 0,         0,                      monitor_info,   4 },
        { "poweroff",   'p', 0,         0,                      pof_info,       5 },
        { "restart",    'r', 0,         0,                      rst_info,       6 },
        { "suspend",    's', 0,         0,                      ssp_info,       7 },
        { "time",       't', "seconds", OPTION_ARG_OPTIONAL,    time_info,      8 },
        { "verbose",    'v', 0,         0,                      verbose_info,   9 },
        { 0 }
};

static struct
arguments {
        gboolean daemonize, gui, hibernate, lid, monitor;
        gboolean poweroff, restart, suspend, use_file, verbose;
        gchar *file;
        int time;
}args;

dbus_bool_t is_daemonized;
const char *HIBERNATE   = "Hibernate";
const char *POWEROFF    = "PowerOff";
const char *RESTART     = "Reboot";
const char *SUSPEND     = "Suspend";
