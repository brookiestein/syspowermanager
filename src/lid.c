#include "lid.h"

void
*lid(void *data)
{
        const char *filepath = data;
        char *confirm = "state:      closed";
        size_t size = strlen(confirm);
        char *file = "/proc/acpi/button/lid/LID/state";

        while (1) {
                FILE *fp = fopen(file, "r");
                char *state = (char *) malloc(size + 2);
                state = fgets(state, size + 2, fp);
                state[strlen(state) - 1] = '\0'; /* Deleting the '\n' character. */
                fclose(fp);

                if (strncmp(state, confirm, size) == 0) {
                        emit_signal(SUSPEND, filepath);
                }

                free(state);
                sleep(1);
        }
}
