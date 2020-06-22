#include "lid.h"

void
*lid(void *data)
{
        const char *filepath = data;
        char *confirm = "state:      closed";
        char *file = "/proc/acpi/button/lid/LID/state";

        while (1) {
                size_t size = strlen(confirm);
                FILE *fp = fopen(file, "r");
                char *state = (char *) malloc(size + 2);
                state = fgets(state, size + 2, fp);
                state[strlen(state) - 1] = '\0'; /* Deleting the '\n' character. */
                fclose(fp);

                if (strncmp(state, confirm, size) == 0) {
                        emit_signal(SUSPEND, filepath);
                        /* It's to allow to the operating system change */
                        /* the file content with the new state if the lid has been opened. */
                        sleep(5);
                }

                free(state);
                sleep(1);
        }
}
