#include "emit_signal.h"

static int
leave_with_error(DBusError *error, DBusConnection *con, DBusMessage *message)
{
        if (error != NULL) {
                dbus_error_free(error);
        }

        if (con != NULL) {
                dbus_connection_unref(con);
        }

        if (message != NULL) {
                dbus_message_unref(message);
        }

        return 1;
}

int
emit_signal(const char *method, const char *filepath)
{
        DBusConnection *con;
        DBusMessage *message;
        DBusError error;
        DBusBusType bus_type    = DBUS_BUS_SYSTEM;
        int reply_timeout       = -1;
        const char *DEST        = "org.freedesktop.login1";
        const char *PATH        = "/org/freedesktop/login1";
        const char *NAME        = "org.freedesktop.login1.Manager";
        const char *EXTRA       = "boolean:true";

        dbus_error_init(&error);
        con                     = dbus_bus_get(bus_type, &error);
        if (con == NULL) {
                logger("An error occurred while SPM was contacting to the bus.", filepath);
                return leave_with_error(&error, NULL, NULL);
        }

        message                 = dbus_message_new_method_call(DEST, PATH, NAME, method);
        if (message == NULL) {
                logger("An error occurred while SPM was assigning bus's message.", filepath);
                return leave_with_error(&error, con, NULL);
        }

        dbus_message_set_auto_start(message, TRUE);

        if (dbus_message_append_args(message, DBUS_TYPE_BOOLEAN, &EXTRA, DBUS_TYPE_INVALID) != TRUE) {
                logger("An error occurred while the extra arg was being added.", filepath);
                return leave_with_error(&error, con, message);
        }

        dbus_connection_send_with_reply_and_block(con, message, reply_timeout, &error);

        if (dbus_error_is_set(&error)) {
                logger("An error occurred while SPM was sending bus's message.", filepath);
                return leave_with_error(&error, con, message);
        }

        /* In this case we don't go to leave with error, because */
        /* everything has gone well. For that, the returned value is ignored. */
        leave_with_error(&error, con, message);
        return 0;
}
