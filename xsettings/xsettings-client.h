#ifndef XSETTINGS_CLIENT_H
#define XSETTINGS_CLIENT_H

#include <X11/Xlib.h>
#include "xsettings-common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _XSettingsClient XSettingsClient;

typedef enum 
{
  XSETTINGS_ACTION_NEW,
  XSETTINGS_ACTION_CHANGED,
  XSETTINGS_ACTION_DELETED,
} XSettingsAction;

typedef void (*XSettingsNotifyFunc) (const char       *name,
				     XSettingsAction   action,
				     XSettingsSetting *setting,
				     void             *cb_data);
typedef void (*XSettingsWatchFunc)  (Window            window,
				     Bool              is_start,
				     long              mask,
				     void             *cb_data);

XSettingsClient *xsettings_client_new           (Display             *display,
						 int                  screen,
						 XSettingsNotifyFunc  notify,
						 XSettingsWatchFunc   watch,
						 void                *cb_data);
void             xsettings_client_destroy       (XSettingsClient     *client);
Bool             xsettings_client_process_event (XSettingsClient     *client,
						 XEvent              *xev);

XSettingsResult xsettings_client_get_setting (XSettingsClient   *client,
					      const char        *name,
					      XSettingsSetting **setting);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSETTINGS_CLIENT_H */
