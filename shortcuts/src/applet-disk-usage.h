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


#ifndef __APPLET_DISK_USAGE__
#define  __APPLET_DISK_USAGE__


#include <cairo-dock.h>
#include "applet-struct.h"


void cd_shortcuts_get_fs_stat (const gchar *cDiskURI, CDDiskUsage *pDiskUsage);

void cd_shortcuts_display_disk_usage (Icon *pIcon, GldiModuleInstance *myApplet);

void cd_shortcuts_launch_disk_periodic_task (GldiModuleInstance *myApplet);

void cd_shortcuts_free_disk_periodic_task (GldiModuleInstance *myApplet);


gchar *cd_shortcuts_get_disk_info (const gchar *cDiskURI, const gchar *cDiskName);


#endif
