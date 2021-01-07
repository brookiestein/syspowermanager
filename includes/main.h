#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <argp.h>
#include <error.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "format.h"
#include "logger.h"
#include "emit_signal.h"
#include "lid.h"
#include "battery_monitor.h"
#include "gui.h"

const char *argp_program_version = "SPM v1.1.1";

static const char doc[] = "\nSystem Power Manager is a \
software piece that serves \
to manage some power options, like Shutdown or Reboot. \
SPM's gist is to offer you a little interface \
through which you can: Shutdown, reboot, suspend and/or \
hibernate your computer without root permissions. \
Also, you can monitor your battery's charge percentage. \
By way of if SPM detects that it's less than 15% then it \
will put your computer to sleep, and so avoid that you \
lose important data.\v\
This software is distributed under the terms of the GNU GPL v2 license. \
You can find this project at: https://github.com/brookiestein/syspowermanager";

static const char daemon_info[]         = "Work as a daemon.";
static const char file_info[]           = "File where the log will be stored.";
static const char lid_info[]            = "Monitor the laptop's lid to suspend \
the system if detects that it has been closed.";
static const char hbt_info[]            = "Hibernate the system.";
static const char pof_info[]            = "Turn off the system.";
static const char rst_info[]            = "Restart the system.";
static const char ssp_info[]            = "Suspend the system.";
static const char verbose_info[]        = "SPM will be verbose. It depends on the options you use.";
static const char wait_info[]           = "Wait for $seconds to finish before do one of \
either hibernate, poweroff, restart or suspend.";

static const char monitor_info[]        = "SPM will monitor the battery's charge percentage \
and, if it detects that it's less than 15% then SPM will put your \
computer to sleep.";

static struct argp_option options[] = {
        { "daemon",     'd', 0,         0,                      daemon_info,    0 },
        { "file",       'f', "file",    OPTION_ARG_OPTIONAL,    file_info,      1 },
        { "hibernate",  'h', 0,         0,                      hbt_info,       2 },
        { "lid",        'l', 0,         0,                      lid_info,       3 },
        { "monitor",    'm', 0,         0,                      monitor_info,   4 },
        { "poweroff",   'p', 0,         0,                      pof_info,       5 },
        { "restart",    'r', 0,         0,                      rst_info,       6 },
        { "suspend",    's', 0,         0,                      ssp_info,       7 },
        { "verbose",    'v', 0,         0,                      verbose_info,   8 },
        { "wait",       'w', "seconds", OPTION_ARG_OPTIONAL,    wait_info,      9 },
        { 0 }
};

static struct
arguments {
        bool daemonize, gui, hibernate, lid, monitor;
        bool poweroff, restart, suspend, use_file, verbose;
        char *file;
        int wait;
}args;

/* Global here for the signal handler to access them. */
pthread_t lid_id, monitor_id;

const char *HIBERNATE   = "Hibernate";
const char *POWEROFF    = "PowerOff";
const char *RESTART     = "Reboot";
const char *SUSPEND     = "Suspend";
