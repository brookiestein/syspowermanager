#include "format.h"

static void
leave(char *str, const char *filepath, FILE *stream)
{
        if (str)
                free(str);
        remove(filepath);
        fclose(stream);
}

char *
format(size_t size, const char *fmt, ...)
{
        const char file[]       = "/tmp/format";
        FILE *fp                = fopen(file, "w+");
        char *str               = (char *) malloc((++size) * sizeof(char));
        va_list args;

        va_start(args, fmt);
        if (vfprintf(fp, fmt, args) < 0) {
                leave(str, file, fp);
                return NULL;
        }
        va_end(args);

        fseek(fp, SEEK_SET, SEEK_SET);
        str = fgets(str, size, fp);
        leave(NULL, file, fp);
        return str;
}
