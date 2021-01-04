#include "battery_monitor.h"

/* Files to monitorize */
/* If your battery was not found at BAT0 directory, change it. */
#define CAPACITY        "/sys/class/power_supply/BAT0/capacity"
#define STATUS          "/sys/class/power_supply/BAT0/status"
#define CH              "Charging"
#define DISCH           "Discharging"
#define BAT_WARN        25 /* For showing MAX_WARNS warnings */
#define BAT_LIMIT       15 /* For going to the suspension */
#define MAX_WARNS       3
#define TIME            20 /* Time to wait after each verification. */
/* Colours for showing log in verbose mode to stdout. */
#define RED             "\e[1;31m"
#define GREEN           "\e[1;32m"
#define ORANGE          "\e[1;33m"
#define WHITE           "\e[1;37m"
#define NO_COLOR        "\e[0m"

/* This function waits for a pointer with the file where
 * the log will be saved in. Whether it needs to be verbose
 * or not, and whether or not it is running as a daemon.
 */
void *
battery_monitor(void *data)
{
        char **parameters = (char **) data;
        char *file        = parameters[0];
        bool verbose      = strncmp(parameters[1], "1", 1) ? false : true;
        bool daemon       = strncmp(parameters[2], "1", 1) ? false : true;

        /* This variable's gist stands for showing 3 warnings
        * about the low charge percentage. */
        short warnings = 0;

        while (true) {
                FILE *fp_capacity = fopen(CAPACITY, "r");
                FILE *fp_status   = fopen(STATUS, "r");

                /* This variable will be used more times in this function
                 * with the same purpose, but not with the sames variables. */
                size_t size   = 20;
                char *percent = (char *) malloc(size * sizeof(char));
                char *status  = (char *) malloc(size * sizeof(char));

                status        = fgets(status, size, fp_status);
                percent       = fgets(percent, size, fp_capacity);
                short ptg     = atoi(percent);

                free(percent);
                fclose(fp_capacity);
                fclose(fp_status);

                if (verbose) {
                        char *time    = get_time();
                        char *fmt     = "[%s] Actual percentage: %i%%";
                        size          = strlen(time) + strlen(fmt);
                        char *msg     = format(size, fmt, time, ptg);

                        logger(msg, file);
                        if (!daemon) {
                                printf("%s%s%s\n", WHITE, msg, NO_COLOR);
                        }

                        free(time);
                        free(msg);
                }

                if (ptg <= BAT_WARN && warnings < MAX_WARNS &&
                        strncmp(status, DISCH, strlen(DISCH)) == 0) {

                        warnings++;
                        char *time      = get_time();
                        char *tmp       = "Battery's charge is arriving to the allowed limit.";

                        /* 7 stands for the brackets, whitespace, and the warning counter. */
                        size            = strlen(time) + strlen(tmp) + 7;
                        char *message   = format(size, "[%s] %s (%i)",
                                        time, tmp, (MAX_WARNS - warnings));

                        logger(message, file);
                        if (daemon) {
                                notify_send(tmp, file, NOTIFY_URGENCY_NORMAL);
                        } else {
                                printf("%s%s%s\n", ORANGE, message, NO_COLOR);
                        }

                        free(time);
                        free(message);

                } else if (ptg <= BAT_LIMIT &&
                        strncmp(status, DISCH, strlen(DISCH)) == 0) {

                        char *time = get_time();
                        char *tmp  = "Battery's charge is very low, so your computer is going to sleep.";

                        /* 3 stands for the brackets, whitespace. */
                        size            = strlen(time) + strlen(tmp) + 3;
                        char *message   = format(size, "[%s] %s", time, tmp);

                        logger(message, file);
                        if (daemon) {
                                notify_send(tmp, file, NOTIFY_URGENCY_CRITICAL);
                        } else {
                                printf("%s%s%s\n", RED, message, NO_COLOR);
                        }

                        free(time);
                        free(message);

                        emit_signal(SUSPEND, file);
                } else if (strncmp(status, CH, strlen(CH)) == 0) {
                        warnings = 0;
                }

                free(status);
                sleep(TIME);
        }

        return data;
}
