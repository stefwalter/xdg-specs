#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include "gtk-utils.h"

static void
msgbox_yes_cb (GtkWidget *widget, gboolean *result)
{
  *result = TRUE;
  gtk_object_destroy (GTK_OBJECT (gtk_widget_get_toplevel (widget)));
}

static gboolean
msgbox_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (event->keyval == GDK_Escape)
    {
      gtk_signal_emit_stop_by_name (GTK_OBJECT (widget), "key_press_event");
      gtk_object_destroy (GTK_OBJECT (widget));
      return TRUE;
    }

  return FALSE;
}

gboolean
msgbox_run (GtkWindow  *parent,
	    const char *message,
	    const char *yes_button,
	    const char *no_button,
	    gint default_index)
{
  gboolean result = FALSE;
  GtkWidget *dialog;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *vbox;
  GtkWidget *button_box;
  GtkWidget *separator;

  g_return_val_if_fail (message != NULL, FALSE);
  g_return_val_if_fail (default_index >= 0 && default_index <= 1, FALSE);
  
  /* Create a dialog
   */
  dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  if (parent)
    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);

  /* Quit our recursive main loop when the dialog is destroyed.
   */
  gtk_signal_connect (GTK_OBJECT (dialog), "destroy",
		      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);

  /* Catch Escape key presses and have them destroy the dialog
   */
  gtk_signal_connect (GTK_OBJECT (dialog), "key_press_event",
		      GTK_SIGNAL_FUNC (msgbox_key_press_cb), NULL);

  /* Fill in the contents of the widget
   */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (dialog), vbox);
  
  label = gtk_label_new (message);
  gtk_misc_set_padding (GTK_MISC (label), 12, 12);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

  button_box = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (vbox), button_box, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 8);
  

  /* When Yes is clicked, call the msgbox_yes_cb
   * This sets the result variable and destroys the dialog
   */
  if (yes_button)
    {
      button = gtk_button_new_with_label (yes_button);
      GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
      gtk_container_add (GTK_CONTAINER (button_box), button);

      if (default_index == 0)
	gtk_widget_grab_default (button);
      
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
			  GTK_SIGNAL_FUNC (msgbox_yes_cb), &result);
    }

  /* When No is clicked, destroy the dialog
   */
  if (no_button)
    {
      button = gtk_button_new_with_label (no_button);
      GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
      gtk_container_add (GTK_CONTAINER (button_box), button);
      
      if (default_index == 1)
	gtk_widget_grab_default (button);
      
      gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
				 GTK_SIGNAL_FUNC (gtk_object_destroy), GTK_OBJECT (dialog));
    }

  gtk_widget_show_all (dialog);

  /* Run a recursive main loop until a button is clicked
   * or the user destroys the dialog through the window mananger */
  gtk_main ();

  return result;
}
