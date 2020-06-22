#include "logger.h"

int
logger(const char *message, const char *filepath)
{
        FILE *fp = fopen(filepath, "a");
        fprintf(fp, "%s\n", message);
        fclose(fp);
        return 0;
}
