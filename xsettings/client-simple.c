#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include "xsettings-client.h"

static void 
usage (void)
{
  fprintf (stderr, "Usage: xsettings-client [-display DISPLAY]\n");
  exit (1);
}

static void
dump_setting (XSettingsSetting *setting)
{
  switch (setting->type)
    {
    case XSETTINGS_TYPE_INT:
      printf ("    INT: %d\n", setting->data.v_int);
      break;
    case XSETTINGS_TYPE_STRING:
      printf ("    STRING: %s\n", setting->data.v_string);
      break;
    case XSETTINGS_TYPE_COLOR:
      printf ("    COLOR: (%#4x, %#4x, %#4x, %#4x)\n",
	      setting->data.v_color.red,
	      setting->data.v_color.green,
  	      setting->data.v_color.blue,
	      setting->data.v_color.alpha);
      break;
    }
}

static void
notify_cb (const char       *name,
	   XSettingsAction   action,
	   XSettingsSetting *setting,
	   void             *data)
{
  switch (action)
    {
    case XSETTINGS_ACTION_NEW:
      printf ("%s: new\n", name);
      dump_setting (setting);
      break;
    case XSETTINGS_ACTION_CHANGED:
      printf ("%s: new\n", name);
      dump_setting (setting);
      break;
    case XSETTINGS_ACTION_DELETED:
      printf ("%s: deleted\n", name);
      break;
    }
}

static void 
watch_cb (Window window,
	  Bool   is_start,
	  long   mask,
	  void  *cb_data)
{
  if (is_start)
    printf ("Starting watch on %#lx with mask %#lx\n", window, mask);
  else
    printf ("Stopping watch on %#lx \n", window);
}

int 
main (int argc, char **argv)
{
  Display *display;
  const char *display_str = NULL;
  XSettingsClient *client;
  int i;

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-display") == 0)
	{
	  if (i + 1 == argc)
	    usage();
	  else
	    {
	      display_str = argv[i + 1];
	      i++;
	    }
	}
      else
	usage ();
    }

  display = XOpenDisplay (display_str);
  if (!display)
    {
      fprintf (stderr, "Cannot open display '%s'\n",
	       XDisplayName (display_str));
      exit (1);
    }

  client = xsettings_client_new (display, DefaultScreen (display),
				 notify_cb, watch_cb, NULL);
  if (!client)
    {
      fprintf (stderr, "Cannot create client\n");
      exit (1);
    }

  while (1)
    {
      XEvent xev;

      XNextEvent (display, &xev);

      xsettings_client_process_event (client, &xev);
    }

  xsettings_client_destroy (client);
  XCloseDisplay (display);
  
  return 0;
}
