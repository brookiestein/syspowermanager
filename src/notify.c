#include "notify.h"

int
notify_send(const char *body, const char *logstream, NotifyUrgency urgency)
{
        char summary[]  = "SysPowerMananger";

        if (!notify_init(summary)) {
                logger("An error occurred while initialising libnotify.", logstream);
                return 1;
        }

        NotifyNotification *n = notify_notification_new(summary, body, NULL);
        notify_notification_set_urgency(n, urgency);

        if (!notify_notification_show(n, NULL)) {
                logger("An error occurred while showing notification.", logstream);
                return 1;
        }

        notify_uninit();
        return EXIT_SUCCESS;
}
