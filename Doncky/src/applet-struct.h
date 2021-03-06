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


#ifndef __CD_APPLET_STRUCT__
#define  __CD_APPLET_STRUCT__

#include <cairo-dock.h>

#define CD_SYSMONITOR_PROC_FS "/proc"   // REPRIS DE SYSTEM-MONITOR
#define CD_SYSMONITOR_NB_MAX_VALUES 4   // REPRIS DE SYSTEM-MONITOR

typedef struct _CDDiskUsage {
	long long iPrevAvail;
	long long iAvail;
	long long iFree;
	long long iTotal;
	long long iUsed;
	int iType;
	} CDDiskUsage;

//\___________ structure containing the applet's configuration parameters.
struct _AppletConfig {
	gchar *cDefaultFont;
	double fDefaultTextColor[4];
	gint iTextMargin;
	gint iSpaceBetweenLines;
	gboolean bDisplayBackground;
	double fBackgroundColor1[4];
	double fBackgroundColor2[4];
	gint iBackgroundRadius;
	gint iBorderThickness;
	double fBorderColor[4];
	
	gchar *cDeskletSize;
	gchar *cXmlFilePath;
	gint iCheckInterval;
	
	// REPRIS DE SYSTEM-MONITOR :
	gboolean bShowCpu;
	gboolean bShowRam;
	gboolean bShowNvidia;
	gboolean bShowSwap;
	gchar *cSystemMonitorClass;
	gdouble fUserHZ;
	} ;

//\___________ structure containing the applet's data, like surfaces, dialogs, results of calculus, etc.
struct _AppletData {
	//~ // Copy of myConfig :
	//~ gchar *cDefaultFont;
	//~ double fDefaultTextColor[4];
	//~ gint iTextMargin;
	//~ gint iSpaceBetweenLines;
	//~ gboolean bDisplayBackground;
	//~ double fBackgroundColor1[4];
	//~ double fBackgroundColor2[4];
	//~ gint iBackgroundRadius;
	//~ gint iBorderThickness;
	//~ double fBorderColor[4];
	
	gchar *cThemeFolder;
	gchar *cXmlFileName;
	gchar *cPrevAlignWidth;
	gchar *cPrevAlignHeight;
	gchar *cPrevFont;
	double fPrevTextColor[4];
	gint iPrevOverrideH;
	gint iPrevOverrideW;	
	GldiTask *pPeriodicRefreshTask;	
	GList *pTextZoneList;	
	gchar *cCurrentText;	
	double fCurrentX; // Position du curseur dans le desklet
	double fCurrentY;
	double fCurrentYalign;
	gchar *cLastAlignHeight;
	gint iLastLineHeight;
	gboolean bLastWasSameLine;	
	cairo_surface_t *pImgSurface;
	gint iCurrentFileSize;
	
	// REPRIS DE SYSTEM-MONITOR :
	// infos, constantes.
	gint iNbCPU;
	gint iFrequency;
	gchar *cModelName;
	gchar *cGPUName;
	gint iVideoRam;
	gchar *cDriverVersion;
	// shared memory for the main thread.
	gboolean bInitialized;
	gboolean bAcquisitionOK;
	GTimer *pClock;
	long long int cpu_user, cpu_user_nice, cpu_system, cpu_idle;
	unsigned long long ramTotal, ramFree, ramUsed, ramBuffers, ramCached;
	unsigned long long swapTotal, swapFree, swapUsed;
	gint iGPUTemp;
	gdouble fCpuPercent;
	gdouble fPrevCpuPercent;
	gdouble fRamPercent,fSwapPercent;
	gdouble fPrevRamPercent, fPrevSwapPercent;
	gdouble fGpuTempPercent;
	gdouble fPrevGpuTempPercent;
	gboolean bNeedsUpdate;
	gint iTimerCount;
	// end of shared memory.
	} ;

#endif
