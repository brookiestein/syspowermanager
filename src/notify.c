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

        GError *err = NULL;
        if (!notify_notification_show(n, &err)) {
                /* logger("An error occurred while showing notification.", logstream); */
                logger(err->message, logstream);
                g_clear_error(&err);
                return 1;
        }

        notify_uninit();
        g_clear_error(&err);
        return 0;
}
