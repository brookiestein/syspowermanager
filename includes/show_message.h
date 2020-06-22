#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

#ifndef SHOW_MESSAGE_H
#define SHOW_MESSAGE_H 1
#endif

gint
show_message(GtkWidget *parent, GtkMessageType mtype,
GtkButtonsType btype, const gchar *title, const gchar *message);
