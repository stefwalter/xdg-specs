#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "gtk-common.h"

static int n_children = 0;

struct _Child
{
  Socket *socket;
};

static void
plug_added (GtkWidget *widget,
	    Socket    *socket)
{
  g_print ("Plug added to socket\n");
  
  gtk_widget_show (socket->socket);
  gtk_widget_hide (socket->frame);
}

static gboolean
plug_removed (GtkWidget *widget,
	      Socket    *socket)
{
  g_print ("Plug removed from socket\n");
  
  gtk_widget_hide (socket->socket);
  gtk_widget_show (socket->frame);
  
  return TRUE;
}

static void
socket_destroy (GtkWidget *widget,
		Socket    *socket)
{
  if (socket->child)
    socket->child->socket = NULL;
  g_free (socket);
}

Socket *
socket_new (void)
{
  GtkWidget *label;
  
  Socket *socket = g_new (Socket, 1);
  
  socket->box = gtk_vbox_new (FALSE, 0);

  socket->socket = gtk_socket_new ();
  
  gtk_box_pack_start (GTK_BOX (socket->box), socket->socket, TRUE, TRUE, 0);
  
  socket->frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (socket->frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (socket->box), socket->frame, TRUE, TRUE, 0);
  gtk_widget_show (socket->frame);
  
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), "<span color=\"red\">Empty</span>");
  gtk_container_add (GTK_CONTAINER (socket->frame), label);
  gtk_widget_show (label);

  g_signal_connect (socket->socket, "plug_added",
		    G_CALLBACK (plug_added), socket);
  g_signal_connect (socket->socket, "plug_removed",
		    G_CALLBACK (plug_removed), socket);
  g_signal_connect_after (socket->socket, "destroy",
			  G_CALLBACK (socket_destroy), socket);

  socket->child = NULL;

  return socket;
}

static gboolean
child_read_watch (GIOChannel *channel, GIOCondition cond, gpointer data)
{
  GIOStatus status;
  GError *error = NULL;
  Child *child = data;
  char *line;
  gsize term;
  int xid;
  
  status = g_io_channel_read_line (channel, &line, NULL, &term, &error);
  switch (status)
    {
    case G_IO_STATUS_NORMAL:
      if (child->socket && !GTK_SOCKET (child->socket->socket)->plug_window)
	{
	  line[term] = '\0';
	  xid = strtol (line, NULL, 0);
	  if (xid == 0)
	    {
	      fprintf (stderr, "Invalid window id '%s'\n", line);
	    }
	  else
	    {
	      gtk_socket_add_id (GTK_SOCKET (child->socket->socket), xid);
	    }
	}
      g_free (line);
      return TRUE;
    case G_IO_STATUS_AGAIN:
      return TRUE;
    case G_IO_STATUS_EOF:
      n_children--;
      if (child->socket)
	child->socket->child = NULL;
      g_free (child);
      g_io_channel_shutdown (channel, FALSE, NULL);
      return FALSE;
    case G_IO_STATUS_ERROR:
      fprintf (stderr, "Error reading fd from child: %s\n", error->message);
      exit (1);
      return FALSE;
    default:
      g_assert_not_reached ();
      return FALSE;
    }
  
}

void
socket_start_child (Socket    *socket,
		    gboolean   active,
		    gboolean   qt)
{
  char *argv[3] = { NULL, NULL, NULL };
  char buffer[20];
  int out_fd;
  GIOChannel *channel;
  GError *error = NULL;

  socket->child = g_new (Child, 1);
  socket->child->socket = socket;

  argv[0] = qt ? "./qt-client" : "./gtk-client";
  
  if (active)
    {
      sprintf(buffer, "%#lx", (gulong) gtk_socket_get_id (GTK_SOCKET (socket->socket)));
      argv[1] = buffer;
    }
  
  if (!g_spawn_async_with_pipes (NULL, argv, NULL, 0, NULL, NULL, NULL, NULL, &out_fd, NULL, &error))
    {
      fprintf (stderr, "Can't exec %s: %s\n", argv[0], error->message);
      exit (1);
    }

  n_children++;
  channel = g_io_channel_unix_new (out_fd);
  g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, &error);
  if (error)
    {
      fprintf (stderr, "Error making channel non-blocking: %s\n", error->message);
      exit (1);
    }
  
  g_io_add_watch (channel, G_IO_IN | G_IO_HUP, child_read_watch, socket->child);
  g_io_channel_unref (channel);
}

void
wait_for_children ()
{
  if (n_children)
    {
      g_print ("Waiting for children to exit\n");

      while (n_children)
	g_main_context_iteration (NULL, TRUE);
    }

}
