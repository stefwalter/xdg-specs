#include <gtk/gtk.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "gtk-common.h"

GSList *sockets = NULL;

GtkWidget *window;
GtkWidget *vbox;

static void
quit_cb (gpointer        callback_data,
	 guint           callback_action,
	 GtkWidget      *widget)
{
  GtkWidget *message_dialog = gtk_message_dialog_new (GTK_WINDOW (window), 0,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_NONE,
						      "Really Quit?");
  gtk_dialog_add_buttons (GTK_DIALOG (message_dialog),
			  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			  GTK_STOCK_QUIT, GTK_RESPONSE_YES,
			  NULL);
  
  gtk_dialog_set_default_response (GTK_DIALOG (message_dialog), GTK_RESPONSE_YES);

  if (gtk_dialog_run (GTK_DIALOG (message_dialog)) == GTK_RESPONSE_YES)
    gtk_widget_destroy (window);

  gtk_widget_destroy (message_dialog);
}

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_Quit",       "<control>Q", quit_cb,               0 },
};

static void
socket_destroyed (GtkWidget *widget,
		  Socket    *socket)
{
  sockets = g_slist_remove (sockets, socket);
}

static void
remove_child (void)
{
  if (sockets)
    {
      Socket *socket = sockets->data;
      gtk_widget_destroy (socket->box);
    }
}

void
add_child (gboolean   active,
	   gboolean   qt)
{
  Socket *socket = socket_new ();

  gtk_box_pack_start (GTK_BOX (vbox), socket->box, TRUE, TRUE, 0);
  gtk_widget_show (socket->box);

  sockets = g_slist_prepend (sockets, socket);

  socket_start_child (socket, active, qt);

  g_signal_connect (socket->socket, "destroy",
		    G_CALLBACK (socket_destroyed), socket);
}

void
add_active_gtk_child (void)
{
  add_child (TRUE, FALSE);
}

void
add_passive_gtk_child (void)
{
  add_child (FALSE, FALSE);
}

void
add_active_qt_child (void)
{
  add_child (TRUE, TRUE);
}

void
add_passive_qt_child (void)
{
  add_child (FALSE, TRUE);
}

int
main (int argc, char *argv[])
{
  GtkWidget *button;
  GtkWidget *hbox;
  GtkAccelGroup *accel_group;
  GtkItemFactory *item_factory;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);
  
  gtk_window_set_title (GTK_WINDOW (window), "Socket Test");
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

  
  gtk_item_factory_create_items (item_factory,
				 G_N_ELEMENTS (menu_items), menu_items,
				 NULL);
      
  gtk_box_pack_start (GTK_BOX (vbox),
		      gtk_item_factory_get_widget (item_factory, "<main>"),
		      FALSE, FALSE, 0);

  button = gtk_button_new_with_label ("Add Active Qt Child");
  gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (add_active_qt_child), NULL);

  button = gtk_button_new_with_label ("Add Passive Qt Child");
  gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
  g_signal_connect (button, "clicked", GTK_SIGNAL_FUNC (add_passive_qt_child), NULL);

  button = gtk_button_new_with_label ("Add Active GTK+ Child");
  gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (add_active_gtk_child), NULL);

  button = gtk_button_new_with_label ("Add Passive GTK+ Child");
  gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (add_passive_gtk_child), NULL);

  button = gtk_button_new_with_label ("Remove Last Child");
  gtk_box_pack_start (GTK_BOX(vbox), button, FALSE, FALSE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (remove_child), NULL);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  gtk_widget_show_all (window);

  gtk_main ();

  wait_for_children ();

  return 0;
}
