#ifndef XSETTINGS_MANAGER_H
#define XSETTINGS_MANAGER_H

#include <X11/Xlib.h>
#include "xsettings-common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _XSettingsManager XSettingsManager;

typedef void (*XSettingsTerminateFunc)  (void *cb_data);

Bool xsettings_manager_check_running (Display *display,
				      int      screen);

XSettingsManager *xsettings_manager_new (Display                *display,
					 int                     screen,
					 XSettingsTerminateFunc  terminate,
					 void                   *cb_data);

void   xsettings_manager_destroy       (XSettingsManager *manager);
Window xsettings_manager_get_window    (XSettingsManager *manager);
Bool   xsettings_manager_process_event (XSettingsManager *manager,
					XEvent           *xev);

XSettingsResult xsettings_manager_set_setting (XSettingsManager *manager,
					       XSettingsSetting *setting);
XSettingsResult xsettings_manager_set_int     (XSettingsManager *manager,
					       const char       *name,
					       int               value);
XSettingsResult xsettings_manager_set_string  (XSettingsManager *manager,
					       const char       *name,
					       const char       *value);
XSettingsResult xsettings_manager_set_color   (XSettingsManager *manager,
					       const char       *name,
					       XSettingsColor   *value);
XSettingsResult xsettings_manager_notify      (XSettingsManager *manager);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSETTINGS_MANAGER_H */
