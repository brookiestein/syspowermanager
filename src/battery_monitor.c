#include "battery_monitor.h"

#define TIME 20 /* Time to wait after each verification. */
/* Files to monitorize */
/* If your battery was not found at BAT0 directory, change it. */
#define CAPACITY        "/sys/class/power_supply/BAT0/capacity"
#define STATUS          "/sys/class/power_supply/BAT0/status"
#define CH              "Charging"
#define DISCH           "Discharging"

static char *RED          = "\e[1;31m";
static char *GREEN        = "\e[1;32m";
static char *ORANGE       = "\e[1;33m";
static char *WHITE        = "\e[1;37m";
static char *NO_COLOR     = "\e[0m";

static char
*get_time(void)
{
        time_t timer            = time(NULL);
        struct tm *ltime        = localtime(&timer);
        char *date              = (char *) malloc(25 + 1);
        strftime(date, 25, "%c", ltime);
        return date;
}

/* This function waits for a pointer with the file where
 * the log will be saved in. Whether it needs to be verbose
 * or not, and whether or not it is running as a daemon.
 */
void
*battery_monitor(void *data)
{
        char **parameters = (char **) data;
        char *file          = parameters[0];
        dbus_bool_t verbose = strncmp(parameters[1], "1", 1) ? FALSE : TRUE;
        dbus_bool_t daemon  = strncmp(parameters[2], "1", 1) ? FALSE : TRUE;

        while (1) {
                unsigned int percentage = 0u;
                /* These variables' gist stands for showing 3 warnings
                * about the low charge percentage. */
                unsigned int warnings      = 0u;
                dbus_bool_t limit_warnings = FALSE;

                FILE *fp_capacity = fopen(CAPACITY, "r");
                FILE *fp_status   = fopen(STATUS, "r");

                /* This variable will be used more times in this function
                 * with the same purpose, but not with the sames variables. */
                size_t size   = 20;
                char *percent = (char *) malloc(size);
                char *status  = (char *) malloc(size);

                percent       = fgets(percent, size, fp_capacity);
                status        = fgets(status, size, fp_status);
                percentage    = atoi(percent);

                fclose(fp_capacity);
                fclose(fp_status);

                if (verbose) {
                        char *time    = get_time();
                        size          = strlen(time) + 25;
                        char *message = format(size,
                                        "[%s] Actual percentage: %i%%",
                                        time, percentage);

                        logger(message, file);

                        if (!daemon) {
                                printf("%s%s%s\n", WHITE, message, NO_COLOR);
                        }

                        free(time);
                        free(message);
                }

                if (percentage <= 25 && warnings < 3 &&
                        strncmp(status, DISCH, strlen(DISCH)) == 0) {

                        warnings++;
                        char *time      = get_time();
                        char *tmp       = "The charge percentage is arriving to the allowed limit.";

                        /* 3 stands for the brackets and whitespace. */
                        size            = strlen(time) + strlen(tmp) + 3;
                        char *message   = format(size, "[%s] %s", time, tmp);

                        logger(message, file);

                        if (daemon) {
                                notify_send(tmp, file, NOTIFY_URGENCY_NORMAL);
                        } else {
                                printf("%s%s%s\n", ORANGE, message, NO_COLOR);
                        }

                        char *color;
                        if (limit_warnings) {
                                color = RED;
                        } else {
                                color = GREEN;
                        }

                        free(message);
                        tmp     = "Please, put your computer to charge before it arrives to the established limit.";
                        /* 3 stands for the brackets and whitespace. */
                        size    = strlen(time) + strlen(tmp) + 3;
                        message = format(size, "[%s] %s", time, tmp);

                        logger(message, file);
                        if (daemon) {
                                notify_send(tmp, file, NOTIFY_URGENCY_NORMAL);
                        } else {
                                printf("%s%s%s\n\n", color, message, NO_COLOR);
                        }

                        free(time);
                        free(message);

                } else if (percentage <= 15 &&
                        strncmp(status, DISCH, strlen(DISCH)) == 0) {

                        char *time = get_time();
                        char *msg0 = "Battery's charge is very low.";
                        char *msg1 = "Your computer is going to sleep.";

                        /* 5 stands for the brackets, whitespace and the '\n' character. */
                        size            = strlen(time) + strlen(msg0) + strlen(msg1) + 5;
                        char *message   = format(size, "[%s] %s\n%s", msg0, msg1);

                        logger(message, file);

                        if (daemon) {
                                char *tmp = format((strlen(msg0) + strlen(msg1) + 1),
                                                "%s\n%s", msg0, msg1);

                                notify_send(tmp, file, NOTIFY_URGENCY_CRITICAL);
                                free(tmp);
                        } else {
                                printf("%s%s%s\n", RED, message, NO_COLOR);
                        }

                        free(time);
                        free(message);

                        emit_signal(SUSPEND, file);
                } else if (strncmp(status, CH, strlen(CH)) == 0) {
                        warnings = 0u;
                        limit_warnings = FALSE;
                }

                free(percent);
                free(status);
                sleep(TIME);
        }

        return data;
}
