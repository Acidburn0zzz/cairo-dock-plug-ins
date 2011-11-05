/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <libxklavier/xklavier.h>

#include "applet-config.h"
#include "applet-xklavier.h"
#include "applet-notifications.h"
#include "applet-struct.h"
#include "applet-draw.h"
#include "applet-init.h"


CD_APPLET_DEFINITION (N_("keyboard indicator"),
	2, 0, 0,
	CAIRO_DOCK_CATEGORY_APPLET_SYSTEM,
	N_("This applet lets you control the keyboard layout.\n\
	It can also display the current num and caps lock.\n\
	Left-click to switch to the next layout\n\
	Scroll up/down to select the previous/next layout\n\
	Right-click gives you access to the list of available layouts."),
	"Fabounet (Fabrice Rey)")

static void _load_bg_image (void)
{
	if (myData.pBackgroundSurface != NULL)
	{
		cairo_surface_destroy (myData.pBackgroundSurface);
		myData.pBackgroundSurface = NULL;
	}
	if (myData.iBackgroundTexture != 0)
	{
		_cairo_dock_delete_texture (myData.iBackgroundTexture);
		myData.iBackgroundTexture = 0;
	}
	
	if (myConfig.cBackgroundImage != NULL)
	{
		myData.pBackgroundSurface = CD_APPLET_LOAD_SURFACE_FOR_MY_APPLET (myConfig.cBackgroundImage);
		if (g_bUseOpenGL)
			myData.iBackgroundTexture = cairo_dock_create_texture_from_surface (myData.pBackgroundSurface);
	}
}

//\___________ Here is where you initiate your applet. myConfig is already set at this point, and also myIcon, myContainer, myDock, myDesklet (and myDrawContext if you're in dock mode). The macro CD_APPLET_MY_CONF_FILE and CD_APPLET_MY_KEY_FILE can give you access to the applet's conf-file and its corresponding key-file (also available during reload). If you're in desklet mode, myDrawContext is still NULL, and myIcon's buffers has not been filled, because you may not need them then (idem when reloading).
CD_APPLET_INIT_BEGIN
	if (myDesklet)
	{
		CD_APPLET_SET_DESKLET_RENDERER ("Simple");  // set a desklet renderer.
	}
	
	myConfig.textDescription.iSize = (int) myIcon->fHeight * myConfig.fTextRatio;
	
	CD_APPLET_REGISTER_FOR_CLICK_EVENT;
	CD_APPLET_REGISTER_FOR_BUILD_MENU_EVENT;
	CD_APPLET_REGISTER_FOR_SCROLL_EVENT;
	cairo_dock_register_notification_on_object (&myDesktopMgr,
		NOTIFICATION_KBD_STATE_CHANGED,
		(CairoDockNotificationFunc) cd_xkbd_keyboard_state_changed,
		CAIRO_DOCK_RUN_AFTER,
		myApplet);
	
	// shortkey
	myData.pKeyBinding = CD_APPLET_BIND_KEY (myConfig.cShortkey,
		D_("Switch keyboard language"),
		"Configuration", "shortkey",
		(CDBindkeyHandler) cd_xkbd_on_keybinding_pull);
	
	_load_bg_image ();
	
	myData.iCurrentGroup = -1;  // pour forcer le redessin.
	
	/*Display *dsp = (Display*)cairo_dock_get_Xdisplay (); // const
	XklEngine *pEngine = xkl_engine_get_instance (dsp);
	xkl_engine_start_listen (pEngine, XKLL_MANAGE_WINDOW_STATES | XKLL_MANAGE_LAYOUTS);*/
	
	Window Xid = cairo_dock_get_current_active_window ();
	cd_xkbd_keyboard_state_changed (myApplet, &Xid);
CD_APPLET_INIT_END


//\___________ Here is where you stop your applet. myConfig and myData are still valid, but will be reseted to 0 at the end of the function. In the end, your applet will go back to its original state, as if it had never been activated.
CD_APPLET_STOP_BEGIN
	/*Display *dsp = (Display*)cairo_dock_get_Xdisplay (); // const
	XklEngine *pEngine = xkl_engine_get_instance (dsp);
	xkl_engine_stop_listen (pEngine, XKLL_MANAGE_WINDOW_STATES | XKLL_MANAGE_LAYOUTS);*/
	CD_APPLET_UNREGISTER_FOR_CLICK_EVENT;
	CD_APPLET_UNREGISTER_FOR_BUILD_MENU_EVENT;
	CD_APPLET_UNREGISTER_FOR_SCROLL_EVENT;
	cairo_dock_remove_notification_func_on_object (&myDesktopMgr,
                NOTIFICATION_KBD_STATE_CHANGED,
		(CairoDockNotificationFunc) cd_xkbd_keyboard_state_changed,
		myApplet);
	CD_APPLET_REMOVE_TRANSITION_ON_MY_ICON;
	
	cd_keybinder_unbind (myData.pKeyBinding);
CD_APPLET_STOP_END


//\___________ The reload occurs in 2 occasions : when the user changes the applet's config, and when the user reload the cairo-dock's config or modify the desklet's size. The macro CD_APPLET_MY_CONFIG_CHANGED can tell you this. myConfig has already been reloaded at this point if you're in the first case, myData is untouched. You also have the macro CD_APPLET_MY_CONTAINER_TYPE_CHANGED that can tell you if you switched from dock/desklet to desklet/dock mode.
CD_APPLET_RELOAD_BEGIN
	myConfig.textDescription.iSize = (int) myIcon->fHeight * myConfig.fTextRatio;
	
	if (CD_APPLET_MY_CONFIG_CHANGED)
	{
		if (myDesklet && CD_APPLET_MY_CONTAINER_TYPE_CHANGED)  // we are now in a desklet, set a renderer.
		{
			CD_APPLET_SET_DESKLET_RENDERER ("Simple");
		}
		
		CD_APPLET_REMOVE_TRANSITION_ON_MY_ICON;  // prudence.
		_load_bg_image ();
		
		myData.iCurrentGroup = -1;  // pour forcer le redessin.
		
		//\_____________ On declenche le redessin de l'icone.
		Window Xid = cairo_dock_get_current_active_window ();
		cd_xkbd_keyboard_state_changed (myApplet, &Xid);
		
		cd_keybinder_rebind (myData.pKeyBinding, myConfig.cShortkey, NULL);
	}
	else
	{
		myData.iCurrentGroup = -1;
		cd_xkbd_keyboard_state_changed (myApplet, NULL);
	}
CD_APPLET_RELOAD_END
