/*
 * Copyright © 2001 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Red Hat not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Red Hat makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL RED HAT
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Owen Taylor, Red Hat, Inc.
 */
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "gtk-utils.h"
#include "xsettings-client.h"

enum {
  NAME = 0,
  TYPE = 1,
  VALUE = 2,
  SERIAL = 3
};

XSettingsClient *client;
GtkWidget *settings_clist;

void
terminate_cb (void *data)
{
  gboolean *terminated = data;
  
  g_print ("Releasing the selection and exiting\n");

  *terminated = TRUE;
  gtk_main_quit ();
}
		    
static void
create_gui (void)
{
  char *titles[] = {
    "Name", "Type", "Value", "Serial"
  };
  
  GtkWidget *scrolled_window;
  GtkWidget *dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *vbox, *bbox, *separator, *table, *button;

  gtk_window_set_title (GTK_WINDOW (dialog), "Sample XSETTINGS Client");
  gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 300);

  gtk_signal_connect (GTK_OBJECT (dialog), "destroy",
		      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (dialog), vbox);

  bbox = gtk_hbutton_box_new ();
  gtk_box_pack_end (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);

  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  
  separator = gtk_hseparator_new ();
  gtk_box_pack_end (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

  table = gtk_table_new (3, 2, FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (table), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  settings_clist = gtk_clist_new_with_titles (4, titles);
  gtk_container_add (GTK_CONTAINER (scrolled_window), settings_clist);
  gtk_clist_set_selection_mode (GTK_CLIST (settings_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_sort_column (GTK_CLIST (settings_clist), 1);
  gtk_clist_set_auto_sort (GTK_CLIST (settings_clist), TRUE);

  gtk_clist_set_column_width (GTK_CLIST (settings_clist), NAME, 200);
  gtk_clist_set_column_width (GTK_CLIST (settings_clist), TYPE, 50);
  gtk_clist_set_column_width (GTK_CLIST (settings_clist), VALUE, 100);
  gtk_clist_set_column_width (GTK_CLIST (settings_clist), SERIAL, 50);

  gtk_table_attach (GTK_TABLE (table), scrolled_window,
		    1, 2,                    1, 2,
		    GTK_EXPAND | GTK_FILL,   GTK_EXPAND | GTK_FILL,
		    0,                       0);
  
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("Close");
  gtk_box_pack_end (GTK_BOX (bbox), button, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);

  gtk_widget_show_all (dialog);
}

static int
find_row (const char *name)
{
  GList *tmp_list = GTK_CLIST (settings_clist)->row_list;
  int index = 0;
  
  while (tmp_list)
    {
      GtkCListRow *row = GTK_CLIST_ROW (tmp_list);

      if (strcmp (row->data, name) == 0)
	return index;
	
      tmp_list = tmp_list->next;
      index++;
    }

  return -1;
}

static void
update_row (int               row,
	    XSettingsSetting *setting)
{
  char buffer[256];
  GtkStyle *style;
  const char *type;
  
  if (setting->type != XSETTINGS_TYPE_COLOR)
    gtk_clist_set_cell_style (GTK_CLIST (settings_clist),
			      row, VALUE, NULL);

  switch (setting->type)
    {
    case XSETTINGS_TYPE_INT:
      type = "INT";
      sprintf (buffer, "%d", setting->data.v_int);
      gtk_clist_set_text (GTK_CLIST (settings_clist), row, VALUE, buffer);      
      break;
    case XSETTINGS_TYPE_STRING:
      type = "STRING";
      gtk_clist_set_text (GTK_CLIST (settings_clist), row, VALUE, setting->data.v_string);
      break;
    case XSETTINGS_TYPE_COLOR:
      type = "COLOR";
      gtk_clist_set_text (GTK_CLIST (settings_clist), row, VALUE, "");
      style = gtk_style_copy (settings_clist->style);
      style->base[GTK_STATE_NORMAL].red = setting->data.v_color.red;
      style->base[GTK_STATE_NORMAL].green = setting->data.v_color.green;
      style->base[GTK_STATE_NORMAL].blue = setting->data.v_color.blue;
      style->bg[GTK_STATE_SELECTED].red = setting->data.v_color.red;
      style->bg[GTK_STATE_SELECTED].green = setting->data.v_color.green;
      style->bg[GTK_STATE_SELECTED].blue = setting->data.v_color.blue;
      gtk_clist_set_cell_style (GTK_CLIST (settings_clist),
				row, VALUE, style);
      gtk_style_unref (style);
      break;
    default:
      g_assert_not_reached ();
    }

  gtk_clist_set_text (GTK_CLIST (settings_clist), row, TYPE, type);

  sprintf (buffer, "%ld", setting->last_change_serial);
  gtk_clist_set_text (GTK_CLIST (settings_clist), row, SERIAL, buffer);
}

static void
notify_cb (const char       *name,
	   XSettingsAction   action,
	   XSettingsSetting *setting,
	   void             *data)
{
  int row;
  char *text[4];
  
  switch (action)
    {
    case XSETTINGS_ACTION_NEW:
      text[NAME] = (char *)name;
      text[TYPE] = text[VALUE] = text[SERIAL] = "";
      row = gtk_clist_insert (GTK_CLIST (settings_clist), 0, text);
      gtk_clist_set_row_data_full (GTK_CLIST (settings_clist), row,
				   g_strdup (name), (GDestroyNotify)g_free);
      update_row (row, setting);
      break;
    case XSETTINGS_ACTION_CHANGED:
      row = find_row (name);
      update_row (row, setting);
      break;
    case XSETTINGS_ACTION_DELETED:
      row = find_row (name);
      gtk_clist_remove (GTK_CLIST (settings_clist), row);
      break;
    }
}

GdkFilterReturn 
client_event_filter (GdkXEvent *xevent,
		     GdkEvent  *event,
		     gpointer   data)
{
  if (xsettings_client_process_event (client, (XEvent *)xevent))
    return GDK_FILTER_REMOVE;
  else
    return GDK_FILTER_CONTINUE;
}

static void 
watch_cb (Window window,
	  Bool   is_start,
	  long   mask,
	  void  *cb_data)
{
  GdkWindow *gdkwin;
  
  if (is_start)
    printf ("Starting watch on %#lx with mask %#lx\n", window, mask);
  else
    printf ("Stopping watch on %#lx \n", window);

  gdkwin = gdk_window_lookup (window);
  if (is_start)
    gdk_window_add_filter (gdkwin, client_event_filter, NULL);
  else
    gdk_window_remove_filter (gdkwin, client_event_filter, NULL);
}

int 
main (int argc, char **argv)
{
  gtk_init (&argc, &argv);

  create_gui ();

  client = xsettings_client_new (gdk_display, DefaultScreen (gdk_display),
				 notify_cb, watch_cb, NULL);
  if (!client)
    {
      fprintf (stderr, "Could not create client!");
      exit (1);
    }

  gtk_main ();

  xsettings_client_destroy (client);

  return 0;
}
