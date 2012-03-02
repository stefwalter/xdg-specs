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
#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "gtk-utils.h"
#include "xsettings-manager.h"

XSettingsManager *manager;

void
terminate_cb (void *data)
{
  gboolean *terminated = data;
  
  g_print ("Releasing the selection and exiting\n");

  *terminated = TRUE;
  gtk_main_quit ();
}
		    
static GtkWidget *
create_label (const char *text)
{
  GtkWidget *label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);

  return label;
}

typedef struct 
{
  GtkWidget *entry;
  GtkAdjustment *adjustment;
  GtkWidget *colorsel;
  GtkWidget *swatch;
  GtkRcStyle *rc_style;
} GUI;

static void
sync_values (GtkWidget *dummy, GUI *gui)
{
  XSettingsColor color;
  
  xsettings_manager_set_string (manager, "Net/UserName", gtk_entry_get_text (GTK_ENTRY (gui->entry)));
  xsettings_manager_set_int (manager, "Net/DoubleClickTime", gui->adjustment->value);

  color.red = gui->rc_style->bg[GTK_STATE_NORMAL].red;
  color.green = gui->rc_style->bg[GTK_STATE_NORMAL].green;
  color.blue = gui->rc_style->bg[GTK_STATE_NORMAL].blue;
  color.alpha = 0xffff;
  
  xsettings_manager_set_color (manager, "Net/Background/Normal", &color);
  
  xsettings_manager_notify (manager);
}

static void
show_colorsel (GtkWidget *widget, GUI *gui)
{
  gdouble color[4];
  
  color[0] = (gdouble)gui->rc_style->bg[GTK_STATE_NORMAL].red / 0xffff;
  color[1] = (gdouble)gui->rc_style->bg[GTK_STATE_NORMAL].green / 0xffff;
  color[2] = (gdouble)gui->rc_style->bg[GTK_STATE_NORMAL].blue / 0xffff;
  
  gtk_color_selection_set_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (gui->colorsel)->colorsel),
				 color);
  gtk_widget_show (gui->colorsel);
}

static void
colorsel_ok (GtkWidget *widget, GUI *gui)
{
  gdouble color[4];
  
  gtk_color_selection_get_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (gui->colorsel)->colorsel),
				 color);
  
  gtk_rc_style_unref (gui->rc_style);
  
  gui->rc_style = gtk_rc_style_new ();
  gui->rc_style->bg[GTK_STATE_NORMAL].red = color[0] * 0xffff;
  gui->rc_style->bg[GTK_STATE_NORMAL].green = color[1] * 0xffff;
  gui->rc_style->bg[GTK_STATE_NORMAL].blue = color[2] * 0xffff;
  gui->rc_style->bg[GTK_STATE_PRELIGHT] = gui->rc_style->bg[GTK_STATE_NORMAL];
  
  gui->rc_style->color_flags[GTK_STATE_NORMAL] = GTK_RC_BG;
  gui->rc_style->color_flags[GTK_STATE_PRELIGHT] = GTK_RC_BG;

  gtk_widget_modify_style (gui->swatch, gui->rc_style);
  sync_values (NULL, gui);

  gtk_widget_hide (gui->colorsel);
}

static gboolean
close_hides (GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_widget_hide (widget);
  return TRUE;
}

static void
create_gui (void)
{
  GtkWidget *dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *vbox, *bbox, *separator;
  GtkWidget *button, *scale;
  GtkWidget *table, *frame, *alignment;
  GUI *gui = g_new (GUI, 1);

  gtk_window_set_title (GTK_WINDOW (dialog), "Sample XSETTINGS Manager");
  gtk_window_set_default_size (GTK_WINDOW (dialog), 500, 300);

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
  
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);


  gtk_table_attach (GTK_TABLE (table), create_label ("Net/DoubleClickTime"),
		    0, 1,                    0, 1,
		    GTK_FILL,                0,
		    0,                       0);

  gui->adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (300., 0., 1000., 10., 100, 0.));
  gtk_signal_connect (GTK_OBJECT (gui->adjustment), "value_changed",
		      GTK_SIGNAL_FUNC (sync_values), gui);
  
  scale = gtk_hscale_new (gui->adjustment);
  gtk_table_attach (GTK_TABLE (table), scale,
		    1, 2,                    0, 1,
		    GTK_FILL | GTK_EXPAND,   0,
		    0,                       0);


  gtk_table_attach (GTK_TABLE (table), create_label ("Net/UserName"),
		    0, 1,                    1, 2,
		    GTK_FILL,                0,
		    0,                       0);
  gui->entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), gui->entry,
		    1, 2,                    1, 2,
		    GTK_FILL | GTK_EXPAND,   0,
		    0,                       0);

  gtk_entry_set_text (GTK_ENTRY (gui->entry), "John Doe");
  gtk_signal_connect (GTK_OBJECT (gui->entry), "changed",
		      GTK_SIGNAL_FUNC (sync_values), gui);

  gtk_table_attach (GTK_TABLE (table), create_label ("Net/Background/Normal"),
		    0, 1,                    2, 3,
		    GTK_FILL,                0,
		    0,                       0);

  alignment = gtk_alignment_new (0.0, 0.5, 0.0, 0.0);
  gtk_table_attach (GTK_TABLE (table), alignment,
		    1, 2,                    2, 3,
		    GTK_FILL | GTK_EXPAND,   0,
		    0,                       0);
  
  button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (alignment), button);
  
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
  gtk_container_add (GTK_CONTAINER (button), frame);
  
  gui->swatch = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (frame), gui->swatch);
  gtk_widget_set_usize (gui->swatch, 80, 15);
  
  gui->rc_style = gtk_rc_style_new ();
  gui->rc_style->bg[GTK_STATE_NORMAL].red = 0xcccc;
  gui->rc_style->bg[GTK_STATE_NORMAL].green = 0xcccc;
  gui->rc_style->bg[GTK_STATE_NORMAL].blue = 0xffff;
  gui->rc_style->bg[GTK_STATE_PRELIGHT] = gui->rc_style->bg[GTK_STATE_NORMAL];
  
  gui->rc_style->color_flags[GTK_STATE_NORMAL] = GTK_RC_BG;
  gui->rc_style->color_flags[GTK_STATE_PRELIGHT] = GTK_RC_BG;

  gtk_widget_modify_style (gui->swatch, gui->rc_style);

  gui->colorsel = gtk_color_selection_dialog_new ("Net/Background/Normal");
  gtk_signal_connect (GTK_OBJECT (gui->colorsel), "delete_event",
		      GTK_SIGNAL_FUNC (close_hides), NULL);

  gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (gui->colorsel)->ok_button),
		      "clicked",
		      GTK_SIGNAL_FUNC (colorsel_ok), gui);
  gtk_signal_connect_object (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (gui->colorsel)->cancel_button),
			     "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_hide),
			     GTK_OBJECT (gui->colorsel));
  gtk_widget_hide (GTK_COLOR_SELECTION_DIALOG (gui->colorsel)->help_button);
  
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (show_colorsel), gui);

  button = gtk_button_new_with_label ("Close");
  gtk_box_pack_end (GTK_BOX (bbox), button, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);

  sync_values (NULL, gui);

  gtk_widget_show_all (dialog);
}

GdkFilterReturn 
manager_event_filter (GdkXEvent *xevent,
		      GdkEvent  *event,
		      gpointer   data)
{
  if (xsettings_manager_process_event (manager, (XEvent *)xevent))
    return GDK_FILTER_REMOVE;
  else
    return GDK_FILTER_CONTINUE;
}

int 
main (int argc, char **argv)
{
  gboolean terminated = FALSE;
  
  gtk_init (&argc, &argv);

  if (xsettings_manager_check_running (gdk_display, DefaultScreen (gdk_display)))
    terminated = !msgbox_run (NULL, 
			      "XSETTINGS manager already detected for screen\n"
			      "Replace existing manager?",
			      "Replace", "Exit", 1);

  if (!terminated)
    {
      manager = xsettings_manager_new (gdk_display, DefaultScreen (gdk_display),
				       terminate_cb, &terminated);
      if (!manager)
	{
	  fprintf (stderr, "Could not create manager!");
	  exit (1);
	}
    }

  gdk_window_add_filter (NULL, manager_event_filter, NULL);
  
  if (!terminated)
    {
      create_gui ();
      gtk_main ();
    }

  xsettings_manager_destroy (manager);

  return 0;
}
