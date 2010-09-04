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

#ifndef __APPLET_DRAW__
#define  __APPLET_DRAW__

#include <cairo-dock.h>


void cd_satus_notifier_compute_grid (void);


void cd_satus_notifier_draw_compact_icon (void);


void cd_satus_notifier_reload_compact_mode (void);


CDStatusNotifierItem *cd_satus_notifier_find_item_from_coord (void);


void cd_satus_notifier_update_item_image (CDStatusNotifierItem *pItem);


#endif
