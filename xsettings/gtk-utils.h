#ifndef GTK_UTILS_H
#define GTK_UTILS_H

#include <gtk/gtkwindow.h>

gboolean msgbox_run (GtkWindow  *parent,
		     const char *message,
		     const char *yes_button,
		     const char *no_button,
		     gint        default_index);

#endif /* GTK_UTILS_H */
