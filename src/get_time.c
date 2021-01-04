#include "get_time.h"

char *
get_time(void)
{
        size_t size      = 25;
        time_t timer     = time(NULL);
        struct tm *ltime = localtime(&timer);
        char *date       = (char *) malloc(size + 1);
        strftime(date, size, "%c", ltime);
        return date;
}
