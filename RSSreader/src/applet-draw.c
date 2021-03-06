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

//\________________ Add your name in the copyright file (and / or modify your name here)

#include <cairo-dock.h>
#include <math.h>
#include "applet-struct.h"
#include "applet-rss.h"
#include "applet-draw.h"


void cd_applet_draw_my_desklet (GldiModuleInstance *myApplet, int iWidth, int iHeight)
{
	if (iWidth < 20 || iHeight < 20)  // inutile de dessiner tant que le desklet n'a pas atteint sa taille definitive.
		return;
	g_return_if_fail (myDrawContext != NULL);
	cd_debug ("%s (%dx%d)", __func__, iWidth, iHeight);
	PangoLayout *pLayout = pango_cairo_create_layout (myDrawContext);
	PangoRectangle ink, log;
	
	CD_APPLET_START_DRAWING_MY_ICON_OR_RETURN_CAIRO ();
	// On efface la surface cairo actuelle
	///cairo_dock_erase_cairo_context (myDrawContext);	
	
	// dessin du fond (optionnel).
	if (myConfig.bDisplayBackground)
	{
		cairo_save (myDrawContext);
		cairo_translate (myDrawContext,
				.5*myConfig.iBorderThickness,
				.5*myConfig.iBorderThickness);		
		cairo_pattern_t *pGradationPattern = cairo_pattern_create_linear (0.,
			0.,
			0.,
			iHeight);
		cairo_pattern_add_color_stop_rgba (pGradationPattern,
			0.,
			myConfig.fBackgroundColor1[0],
			myConfig.fBackgroundColor1[1],
			myConfig.fBackgroundColor1[2],
			myConfig.fBackgroundColor1[3]);
		cairo_pattern_add_color_stop_rgba (pGradationPattern,
			1.,
			myConfig.fBackgroundColor2[0],
			myConfig.fBackgroundColor2[1],
			myConfig.fBackgroundColor2[2],
			myConfig.fBackgroundColor2[3]);
		cairo_set_source (myDrawContext, pGradationPattern);
		
		if (myConfig.iBackgroundRadius != 0)  // On a besoin d'un rayon
		{
			cairo_dock_draw_rounded_rectangle (myDrawContext,
				myConfig.iBackgroundRadius,
				0.,
				iWidth - myConfig.iBorderThickness - 2 * myConfig.iBackgroundRadius,
				iHeight - myConfig.iBorderThickness);
		}
		else  // Il ne faut pas de rayon
		{
			cairo_rectangle (myDrawContext,
				0., 0.,
				iWidth - myConfig.iBorderThickness,
				iHeight - myConfig.iBorderThickness);
		}
		cairo_fill (myDrawContext);
		cairo_pattern_destroy (pGradationPattern);
		cairo_restore (myDrawContext);
	}
	
	int iMargin = .5 * myConfig.iBorderThickness + (1. - sqrt (2) / 2) * myConfig.iBackgroundRadius;  // marge a gauche et au-dessus, pour ne pas mordre sur le coin arrondi.
	double fCurrentY = 0.;  // position de la ligne courante.
	double fTitleWidth = 0., fTitleHeight = 0.;
	int iTitleX = 0;
	
	// On charge le logo si besoin
	PangoFontDescription *fd = pango_font_description_from_string (myConfig.cTitleFont);
	pango_layout_set_font_description (pLayout, fd);
	gboolean bLogoIsOn = FALSE;  // utile pour decaler le texte du titre
	if (myConfig.bDisplayLogo)
	{
		// on recupere la taille du titre.
		int iSize = pango_font_description_get_size (fd);
		if (!pango_font_description_get_size_is_absolute (fd))
			iSize /= PANGO_SCALE;
		if (iSize == 0)
			iSize = 16;
		
		// on en deduit la taille du logo.
		double fLogoSize = MIN (iWidth/2, MIN (iHeight/2, myConfig.fLogoSize * iSize));  // on le limite un peu.
		if (fLogoSize < 1)
			fLogoSize = 1;
		cd_debug ("  fLogoSize  :%.2f", fLogoSize);
		
		// on cree la surface du logo si necessaire.
		if (myData.fLogoSize != iSize || myData.pLogoSurface == NULL)
		{
			cairo_surface_destroy (myData.pLogoSurface);
			myData.fLogoSize = fLogoSize;
			myData.pLogoSurface = cairo_dock_create_surface_for_icon (myConfig.cLogoPath,
				fLogoSize,
				fLogoSize);
		}
		
		// on affiche le logo.
		if (myData.pLogoSurface != NULL)
		{
			bLogoIsOn = TRUE;
			fTitleWidth = myData.fLogoSize + 5;  // 5 pixels d'ecart entre le logo et le texte.
			iTitleX = MAX (0, myConfig.fTitleAlignment * (iWidth - myConfig.iBorderThickness - 2*iMargin - fTitleWidth));
		}
	}
	pango_font_description_free (fd);
	
	// dessin du texte.
	if (myData.pItemList != NULL)
	{
		GList *it = myData.pItemList;
		CDRssItem *pItem = it->data;
		
		// dessin du nom du flux.
		if (pItem->cTitle != NULL)
		{
			pango_layout_set_text (pLayout, pItem->cTitle, -1);
			pango_layout_get_pixel_extents (pLayout, &ink, &log);
			
			fTitleWidth += log.width;
			iTitleX = MAX (0, myConfig.fTitleAlignment * (iWidth - myConfig.iBorderThickness - 2*iMargin - fTitleWidth));
			fTitleHeight = log.height;
			
			cairo_set_source_rgba (myDrawContext, myConfig.fTitleTextColor[0], myConfig.fTitleTextColor[1], myConfig.fTitleTextColor[2], myConfig.fTitleTextColor[3]);
			
			cairo_move_to (myDrawContext,
				iMargin + iTitleX + (bLogoIsOn ? myData.fLogoSize + 5 : 0),
				iMargin + (bLogoIsOn ? MAX (0, (myData.fLogoSize - fTitleHeight)/2) : 0));
			pango_cairo_show_layout (myDrawContext, pLayout);
			
			fCurrentY = iMargin + MAX (log.height, (bLogoIsOn ? myData.fLogoSize : 0)) + myConfig.iSpaceBetweenFeedLines;
		}
		
		// dessin des lignes.
		fd = pango_font_description_from_string (myConfig.cFont);
		pango_layout_set_font_description (pLayout, fd);
		pango_font_description_free (fd);
		
		cairo_set_source_rgba (myDrawContext, myConfig.fTextColor[0], myConfig.fTextColor[1], myConfig.fTextColor[2], myConfig.fTextColor[3]);
		
		if (myData.iFirstDisplayedItem > 0)
		{
			it = g_list_nth (it, myData.iFirstDisplayedItem);
			if (it == NULL)
			{
				it = g_list_last (myData.pItemList);
				if (it->prev)
					it = it->prev;
			}
		}
		
		for (it = it->next; it != NULL; it = it->next)
		{
			pItem = it->data;
			if (pItem->cTitle == NULL)
				continue;
			
			gchar *cLine = g_strdup (pItem->cTitle);
			cd_rssreader_cut_line (cLine, pLayout, iWidth - myConfig.iBorderThickness - 2*iMargin - myConfig.iTextMargin);
			
			pango_layout_set_text (pLayout, cLine, -1);
			pango_layout_get_pixel_extents (pLayout, &ink, &log);
			
			if (fCurrentY + log.height > iHeight - iMargin - .5*myConfig.iBorderThickness)  // on deborde en hauteur.
			{
				//g_print ("fCurrentY : %.1f + %d > %.1f\n", fCurrentY, log.height, iHeight - iMargin - .5*myConfig.iBorderThickness);
				gchar *rc;
				do
				{
					rc = strrchr (cLine, '\n');  // pas tres optimise mais pratique.
					if (!rc)
						break;
					*rc = '\0';
					pango_layout_set_text (pLayout, cLine, -1);
					pango_layout_get_pixel_extents (pLayout, &ink, &log);
					//g_print ("%s -> : %.1f + %d / %.1f\n", cLine, fCurrentY, log.height, iHeight - iMargin - .5*myConfig.iBorderThickness);
				} while (fCurrentY + log.height > iHeight - iMargin - .5*myConfig.iBorderThickness);
				
				if (fCurrentY + log.height > iHeight - iMargin - .5*myConfig.iBorderThickness)  // on deborde toujours => on quitte.
				{
					g_free (cLine);
					break;
				}
				else  // on a reussi a en afficher une partie, on le signale avec des '...'.
				{
					gchar *cCutLine = g_strdup_printf ("%s...", cLine);
					pango_layout_set_text (pLayout, cCutLine, -1);
					pango_layout_get_pixel_extents (pLayout, &ink, &log);
					if (log.width > iWidth - myConfig.iBorderThickness - 2*iMargin - myConfig.iTextMargin)  // les "..." font deborder
					{
						g_free (cCutLine);
						gchar *sp = strrchr (cLine, ' ');
						if (sp)  // on remplace le dernier mot par les "..."
						{
							*sp = '.';
							*(sp+1) = '.';
							*(sp+2) = '.';
							*(sp+3) = '\0';
							cCutLine = NULL;
						}
						else  // on coupe le dernier mot par les "..."
							cCutLine = cairo_dock_cut_string (cLine, -3);
						pango_layout_set_text (pLayout, cCutLine ? cCutLine : cLine, -1);
						pango_layout_get_pixel_extents (pLayout, &ink, &log);
					}
					g_free (cCutLine);
				}
			}
			
			g_free (cLine);
			
			cairo_move_to (myDrawContext,
				iMargin + myConfig.iTextMargin,
				fCurrentY);
			pango_cairo_show_layout (myDrawContext, pLayout);
			
			fCurrentY += log.height + myConfig.iSpaceBetweenFeedLines;
			//g_print ("fCurrentY <- %.2f\n", fCurrentY);
		}
	}
	
	// dessin du logo.
	if (bLogoIsOn)
	{
		cairo_set_source_surface (myDrawContext,
			myData.pLogoSurface,
			iMargin + iTitleX,
			iMargin + MAX (0, (fTitleHeight - myData.fLogoSize)/2));
		cairo_paint (myDrawContext);
	}
	
	// dessin du cadre (optionnel).
	if (myConfig.bDisplayBackground)
	{
		cairo_save (myDrawContext);
		cairo_set_source_rgba (myDrawContext,
			myConfig.fBorderColor[0],
			myConfig.fBorderColor[1],
			myConfig.fBorderColor[2],
			myConfig.fBorderColor[3]);
		cairo_set_line_width (myDrawContext, myConfig.iBorderThickness);
		cairo_translate (myDrawContext,
			.5*myConfig.iBorderThickness,
			.5*myConfig.iBorderThickness);
		cairo_dock_draw_rounded_rectangle (myDrawContext,
			myConfig.iBackgroundRadius,
			0.,
			iWidth - 2*myConfig.iBackgroundRadius - myConfig.iBorderThickness,
			iHeight - myConfig.iBorderThickness);
		cairo_stroke (myDrawContext);
		cairo_restore (myDrawContext);
	}
	
	g_object_unref (pLayout);
	
	CD_APPLET_FINISH_DRAWING_MY_ICON_CAIRO;
	// on met a jour la texture OpenGL.
	/**if (CD_APPLET_MY_CONTAINER_IS_OPENGL)
	{
		cairo_dock_update_icon_texture (myIcon);
	}*/
}


void cd_applet_update_my_icon (GldiModuleInstance *myApplet)
{
	if (myDesklet)
	{
		// taille de la texture.
		int iWidth, iHeight;
		CD_APPLET_GET_MY_ICON_EXTENT (&iWidth, &iHeight);
		
		cd_applet_draw_my_desklet (myApplet, iWidth, iHeight);		
		
		CD_APPLET_REDRAW_MY_ICON;
	}
}
