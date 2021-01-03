#include "battery_monitor.h"

#define TIME 20 /* Time to wait after each verification. */

static char *RED          = "\e[1;31m";
static char *GREEN        = "\e[1;32m";
static char *ORANGE       = "\e[1;33m";
static char *BLUE         = "\e[1;34m";
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

/* This function waits a pointer with the file where */
/* store the possible log and the verbose state. */
void
*battery_monitor(void *data)
{
        char **parameters = (char **) data;
        char *file = parameters[0];
        dbus_bool_t verbose = FALSE;

        if (strncmp(parameters[1], "1", 1) == 0) {
                verbose = TRUE;
        }

        /* If your battery was not found at BAT0 directory, change it. */
        const char *fcapacity           = "/sys/class/power_supply/BAT0/capacity";
        const char *fstatus             = "/sys/class/power_supply/BAT0/status";
        size_t size                     = 20;
        char *percent                   = (char *) malloc(size);
        char *status                    = (char *) malloc(size);
        unsigned int percentage         = 0u;
        unsigned int warnings           = 0u;
        dbus_bool_t limit_warnings      = FALSE;

        while (1) {
                FILE *fp_capacity       = fopen(fcapacity, "r");
                FILE *fp_status         = fopen(fstatus, "r");
                percent                 = fgets(percent, size, fp_capacity);
                status                  = fgets(status, size, fp_status);
                percentage              = atoi(percent);
                fclose(fp_capacity);
                fclose(fp_status);

                if (verbose) {
                        char *color;
                        if (percentage <= 20) {
                                if (warnings >= 3 && limit_warnings != TRUE) {
                                        limit_warnings = TRUE;
                                }
                                color = BLUE;
                        } else {
                                color = WHITE;
                        }

                        size_t init_size        = 25; /* To storage: Actual percentage: */
                        char *message           = (char *) malloc(init_size + 1);
                        char *pp                = (char *) malloc(5 + 1);
                        char *time              = get_time();

                        sprintf(message, "Actual percentage:");
                        sprintf(pp, "%i%%", percentage);

                        printf("%s[%s] %s%s %s%s%s\n", WHITE, time, BLUE, message, color, pp, NO_COLOR);

                        init_size       = strlen(message);
                        char *tmp       = (char *) malloc(init_size + 1);
                        sprintf(tmp, "%s", message);

                        init_size       = 55; /* To storage the log message complete. */
                        message         = realloc(message, init_size + 1);
                        sprintf(message, "[%s] %s %s", time, tmp, pp);
                        logger(message, file);
                        free(tmp);
                        free(message);
                        free(pp);
                }

                if (percentage <= 25 && warnings < 3 && strncmp(status, "Discharging", 11) == 0) {
                        char *color;
                        warnings++;
                        char *message = "The charge percentage is arriving to the allowed limit.";
                        printf("%s%s%s\n", ORANGE, message, NO_COLOR);
                        notify_send(message, file, NOTIFY_URGENCY_NORMAL); /* Useful if it's running as a daemon. */

                        char *time = get_time();

                        /* To storage the message and the time. 4: This to '[] \0' characters. */
                        size_t log_size = strlen(message) + strlen(time) + 4;
                        char *log_message = (char *) malloc(log_size + 1);
                        sprintf(log_message, "[%s] %s", time, message);
                        logger(log_message, file);

                        if (limit_warnings) {
                                color = RED;
                        } else {
                                color = GREEN;
                        }

                        message = "Please, put your computer to charge before it arrives to the established limit.";
                        printf("%s%s%s\n\n", color, message, NO_COLOR);
                        notify_send(message, file, NOTIFY_URGENCY_NORMAL);

                        log_size = strlen(message) + strlen(time) + 4; /* The same */
                        log_message = (char *) realloc(log_message, log_size + 1);
                        sprintf(log_message, "[%s] %s", time, message);
                        logger(log_message, file);
                        free(log_message);
                } else if (percentage <= 15 && strncmp(status, "Discharging", 11) == 0) {
                        char *time = get_time();
                        char *msg0 = "The charge percentage is very low.";
                        char *msg1 = "Your computer is going to sleep to avoid data lose.";

                        /* To storage the messages, the time and '[] \n\0' characters. */
                        size_t log_size = strlen(time) + strlen(msg0) + strlen(msg1) + 5;
                        char *log_message = (char *) malloc(log_size + 1);
                        sprintf(log_message, "[%s] %s\n%s", time, msg0, msg1);
                        printf("%s%s\n%s%s\n", RED, msg0, msg1, NO_COLOR);
                        logger(log_message, file);
                        notify_send(log_message, file, NOTIFY_URGENCY_CRITICAL);
                        free(log_message);

                        emit_signal(SUSPEND, file);
                } else if (strncmp(status, "Charging", 8) == 0) {
                        warnings = 0u;
                        limit_warnings = FALSE;
                }

                sleep(TIME);
        }

        free(percent);
        free(status);
        return data;
}
