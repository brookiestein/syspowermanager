#include "gui.h"

static struct
all_icons {
        GtkWidget *shutdown, *hibernate, *restart;
        GtkWidget *suspend, *leave;
}icons;

static struct
all_buttons {
        GtkWidget *shutdown, *hibernate, *restart;
        GtkWidget *suspend, *leave;
}buttons;

static int
perform(GtkWidget *parent, GtkButton *source)
{
        const gchar *method = gtk_button_get_label(source);
        emit_signal(method, "/dev/null");
        gtk_widget_destroy(parent);
        return 0;
}

static int
activate(GtkApplication *app, gpointer data)
{
        const char *file = (char *) data;
        GtkWidget *window;
        GtkWidget *question, *empty_label;
        GtkWidget *layout[2], *layouts;
        GtkAccelGroup *accel;

        const int WIDTH         = 250;
        const int HEIGHT        = 90;

        accel                   = gtk_accel_group_new();
        window                  = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "System Power Manager");
        gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
        gtk_window_add_accel_group(GTK_WINDOW(window), accel);

        question                = gtk_label_new("What would you like to do?");
        empty_label             = gtk_label_new("");

        /* It will verify only an image. If it hasn't been found there, then the current directory */
        /* will be changed to: /usr/share/spm/. In which the files will be stored when */
        /* the make install order is executed. */
        GdkPixbuf *test         = gdk_pixbuf_new_from_file("resources/icons/shutdown.png", NULL);
        if (test == NULL) {
                if ((chdir("/usr/share/spm")) < 0) {
                        logger("An error occurred while the current directory was being changed.", file);
                }
        }

        icons.shutdown          = gtk_image_new_from_file("resources/icons/shutdown.png");
        icons.hibernate         = gtk_image_new_from_file("resources/icons/hibernate.png");
        icons.restart           = gtk_image_new_from_file("resources/icons/restart.png");
        icons.suspend           = gtk_image_new_from_file("resources/icons/suspend.png");
        icons.leave             = gtk_image_new_from_file("resources/icons/leave.png");

        buttons.shutdown        = gtk_button_new_with_label("PowerOff");
        gtk_button_set_image(GTK_BUTTON(buttons.shutdown), icons.shutdown);
        gtk_widget_add_accelerator(buttons.shutdown, "clicked", accel, GDK_KEY_p, 0, GTK_ACCEL_VISIBLE);
        gtk_widget_set_tooltip_text(buttons.shutdown, "By pressing this button, your computer will shutdown.");

        buttons.hibernate       = gtk_button_new_with_label("Hibernate");
        gtk_button_set_image(GTK_BUTTON(buttons.hibernate), icons.hibernate);
        gtk_widget_add_accelerator(buttons.hibernate, "clicked", accel, GDK_KEY_h, 0, GTK_ACCEL_VISIBLE);
        gtk_widget_set_tooltip_text(buttons.hibernate, "By pressing this button, your computer will hibernate.");

        buttons.restart         = gtk_button_new_with_label("Reboot");
        gtk_button_set_image(GTK_BUTTON(buttons.restart), icons.restart);
        gtk_widget_add_accelerator(buttons.restart, "clicked", accel, GDK_KEY_r, 0, GTK_ACCEL_VISIBLE);
        gtk_widget_set_tooltip_text(buttons.restart, "By pressing this button, your computer will restart.");

        buttons.suspend         = gtk_button_new_with_label("Suspend");
        gtk_button_set_image(GTK_BUTTON(buttons.suspend), icons.suspend);
        gtk_widget_add_accelerator(buttons.suspend, "clicked", accel, GDK_KEY_s, 0, GTK_ACCEL_VISIBLE);
        gtk_widget_set_tooltip_text(buttons.suspend, "By pressing this button, your computer will suspend.");

        buttons.leave           = gtk_button_new_with_label("Leave");
        gtk_button_set_image(GTK_BUTTON(buttons.leave), icons.leave);
        gtk_widget_add_accelerator(buttons.leave, "clicked", accel, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);
        gtk_widget_set_tooltip_text(buttons.leave, "By pressing this button, this interface will be closed.");

        for (gint i = 0; i < 2; i++) {
                layout[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        }

        layouts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        gtk_box_pack_start(GTK_BOX(layout[0]), question, FALSE, FALSE, 150);
        gtk_box_pack_start(GTK_BOX(layout[1]), buttons.shutdown, FALSE, FALSE, 10);
        gtk_box_pack_start(GTK_BOX(layout[1]), buttons.hibernate, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(layout[1]), buttons.restart, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(layout[1]), buttons.suspend, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(layout[1]), buttons.leave, FALSE, FALSE, 10);

        gtk_box_pack_start(GTK_BOX(layouts), layout[0], FALSE, FALSE, 10);
        gtk_box_pack_start(GTK_BOX(layouts), layout[1], FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(layouts), empty_label, FALSE, FALSE, 5);

        g_signal_connect_swapped(buttons.shutdown, "clicked", G_CALLBACK(perform), window);
        g_signal_connect_swapped(buttons.hibernate, "clicked", G_CALLBACK(perform), window);
        g_signal_connect_swapped(buttons.restart, "clicked", G_CALLBACK(perform), window);
        g_signal_connect_swapped(buttons.suspend, "clicked", G_CALLBACK(perform), window);
        g_signal_connect_swapped(buttons.leave, "clicked", G_CALLBACK(gtk_widget_destroy), window);

        gtk_container_add(GTK_CONTAINER(window), layouts);
        gtk_widget_show_all(window);
        return 0;
}

gint
use_gui(char *file)
{
        gpointer data       = file;
        char *APP_ID        = "com.github.brookiestein.SystemPowerManager";
        GtkApplication *app = gtk_application_new(APP_ID, G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(activate), data);
        gint status = g_application_run(G_APPLICATION(app), 0, NULL);
        g_object_unref(app);
        return status;
}
