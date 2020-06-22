#include "show_message.h"

gint
show_message(GtkWidget *parent, GtkMessageType mtype,
GtkButtonsType btype, const gchar *title, const gchar *message)
{
        GtkDialogFlags flag = GTK_DIALOG_DESTROY_WITH_PARENT;

        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                flag, mtype, btype, "%s", message);

        gtk_window_set_title(GTK_WINDOW(dialog), title);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if (response == GTK_RESPONSE_YES) {
                gtk_widget_destroy(parent);
        }

        return response;
}
