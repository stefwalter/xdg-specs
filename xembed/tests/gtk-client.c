#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "gtk-common.h"

static void
remove_buttons (GtkWidget *widget, GtkWidget *other_button)
{
  gtk_widget_destroy (other_button);
  gtk_widget_destroy (widget);
}

static gboolean
blink_cb (gpointer data)
{
  GtkWidget *widget = data;

  gtk_widget_show (widget);
  g_object_set_data (G_OBJECT (widget), "blink", GPOINTER_TO_UINT (0));

  return FALSE;
}

static void
blink (GtkWidget *widget,
       GtkWidget *window)
{
  guint blink_timeout = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (window), "blink"));
  
  if (!blink_timeout)
    {
      blink_timeout = g_timeout_add (1000, blink_cb, window);
      gtk_widget_hide (window);

      g_object_set_data (G_OBJECT (window), "blink", GUINT_TO_POINTER (blink_timeout));
    }
}

static void
remote_destroy (GtkWidget *window)
{
  guint blink_timeout = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (window), "blink"));
  if (blink_timeout)
    g_source_remove (blink_timeout);

  gtk_main_quit ();
}

static void
add_buttons (GtkWidget *widget, GtkWidget *box)
{
  GtkWidget *add_button;
  GtkWidget *remove_button;

  add_button = gtk_button_new_with_mnemonic ("_Add");
  gtk_box_pack_start (GTK_BOX (box), add_button, TRUE, TRUE, 0);
  gtk_widget_show (add_button);

  g_signal_connect (add_button, "clicked",
		    G_CALLBACK (add_buttons), box);

  remove_button = gtk_button_new_with_mnemonic ("_Remove");
  gtk_box_pack_start (GTK_BOX (box), remove_button, TRUE, TRUE, 0);
  gtk_widget_show (remove_button);

  g_signal_connect (remove_button, "clicked",
		    G_CALLBACK (remove_buttons), add_button);
}

void
add_child (GtkWidget *vbox,
	   gboolean   qt)
{
  Socket *socket = socket_new ();

  gtk_box_pack_start (GTK_BOX (vbox), socket->box, TRUE, TRUE, 0);
  gtk_widget_show (socket->box);

  socket_start_child (socket, FALSE, qt);
}

void
add_gtk_child (GtkWidget *widget,
	       GtkWidget *vbox)
{
  add_child (vbox, FALSE);
}

void
add_qt_child (GtkWidget *widget,
	      GtkWidget *vbox)
{
  add_child (vbox, TRUE);
}

guint32
create_child_plug (guint32  xid)
{
  GtkWidget *window;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *entry;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *frame;

  window = gtk_plug_new (xid);

  frame = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (window), frame);
    
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (frame), vbox);

  gtk_container_set_border_width (GTK_CONTAINER (vbox), 3);
  
  g_signal_connect (window, "destroy",
		    G_CALLBACK (remote_destroy), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

  label = gtk_label_new ("GTK+");
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
  
  entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);

  button = gtk_button_new_with_mnemonic ("_Close");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);

  g_signal_connect_swapped (button, "clicked",
			    G_CALLBACK (gtk_widget_destroy), window);

  button = gtk_button_new_with_mnemonic ("_Blink");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (blink), window);

  button = gtk_button_new_with_mnemonic ("Add _GTK+");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (add_gtk_child), vbox);

  button = gtk_button_new_with_mnemonic ("Add _Qt");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (add_qt_child), vbox);

  add_buttons (NULL, hbox);
  
  gtk_widget_show_all (window);

  if (GTK_WIDGET_REALIZED (window))
    return gtk_plug_get_id (GTK_PLUG (window));
  else
    return 0;
}

int
main (int argc, char *argv[])
{
  guint32 xid;
  guint32 plug_xid;

  gtk_init (&argc, &argv);

  if (argc != 1 && argc != 2)
    {
      fprintf (stderr, "Usage: testsocket_child [WINDOW_ID]\n");
      exit (1);
    }

  if (argc == 2)
    {
      xid = strtol (argv[1], NULL, 0);
      if (xid == 0)
	{
	  fprintf (stderr, "Invalid window id '%s'\n", argv[1]);
	  exit (1);
	}
      
      create_child_plug (xid);
    }
  else
    {
      plug_xid = create_child_plug (0);
      printf ("%d\n", plug_xid);
      fflush (stdout);
    }

  gtk_main ();

  wait_for_children ();

  return 0;
}


