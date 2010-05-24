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

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include <cairo.h>

#include "rendering-panel.h"

static void cd_compute_size (CairoDock *pDock)
{
	//\_____________ On calcule le nombre de groupes et la place qu'ils occupent.
	int iNbGroups = 1, iCurrentOrder = -1;
	double fCurrentGroupWidth = - myIcons.iIconGap, fGroupsWidth = 0.;
	GList *ic;
	Icon *pIcon;
	for (ic = pDock->icons; ic != NULL; ic = ic->next)
	{
		pIcon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (pIcon))
		{
			if (CAIRO_DOCK_IS_USER_SEPARATOR (pIcon))  // si c'est un separateur automatique, le changement de groupe incrementera le compteur a l'icone suivante.
			{
				if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
				{
					iNbGroups ++;
					fGroupsWidth += MAX (0, fCurrentGroupWidth);
					g_print ("fGroupsWidth += %.2f\n", fCurrentGroupWidth);
					fCurrentGroupWidth = - myIcons.iIconGap;
				}
			}
			continue;
		}
		if (iCurrentOrder != (int)cairo_dock_get_icon_order (pIcon))
		{
			if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
			{
				iNbGroups ++;
				fGroupsWidth += MAX (0, fCurrentGroupWidth);
				g_print ("fGroupsWidth += %.2f\n", fCurrentGroupWidth);
				fCurrentGroupWidth = - myIcons.iIconGap;
			}
		}
		iCurrentOrder = cairo_dock_get_icon_order (pIcon);
		fCurrentGroupWidth += pIcon->fWidth + myIcons.iIconGap;
		g_print ("fCurrentGroupWidth <- %.2f\n", fCurrentGroupWidth);
	}
	if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste un separateur a la fin.
	{
		fGroupsWidth += MAX (0, fCurrentGroupWidth);
		g_print ("fGroupsWidth += %.2f\n", fCurrentGroupWidth);
	}
	if (fGroupsWidth < 0)
		fGroupsWidth = 0;
	
	//\_____________ On en deduit l'ecart entre les groupes d'icones.
	double W = cairo_dock_get_max_authorized_dock_width (pDock);
	double fScreenBorderGap = myBackground.iDockRadius + myBackground.iDockLineWidth;  // on laisse un ecart avec le bord de l'ecran.
	double fGroupGap = (iNbGroups > 1 ? (W - 2*fScreenBorderGap - fGroupsWidth) / (iNbGroups - 1) : W - fScreenBorderGap - fGroupsWidth);
	if (fGroupGap < myIcons.iIconGap)  // les icones depassent en largeur.
		fGroupGap = myIcons.iIconGap;
	g_print (" -> %d groups, %d/%d\nfGroupGap = %.2f\n", iNbGroups, (int)fGroupsWidth, (int)W, fGroupGap);
	
	//\_____________ On calcule la position au repos des icones et la taille du dock.
	double xg = fScreenBorderGap;  // abscisse de l'icone courante, et abscisse du debut du groupe courant.
	double x = xg;
	double fDockWidth = 0.;
	fCurrentGroupWidth = - myIcons.iIconGap;
	iCurrentOrder = -1;
	for (ic = pDock->icons; ic != NULL; ic = ic->next)
	{
		pIcon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (pIcon))
		{
			if (CAIRO_DOCK_IS_USER_SEPARATOR (pIcon))  // si c'est un separateur automatique, le changement de groupe incrementera le compteur a l'icone suivante.
			{
				if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
				{
					xg += fCurrentGroupWidth + fGroupGap;
					x = xg;
					g_print ("jump to %.2f\n", x);
					fCurrentGroupWidth = - myIcons.iIconGap;
				}
			}
			continue;
		}
		if (iCurrentOrder != (int)cairo_dock_get_icon_order (pIcon))
		{
			if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
			{
				xg += fCurrentGroupWidth + fGroupGap;
				x = xg;
				g_print ("jump to %.2f\n", x);
				fCurrentGroupWidth = - myIcons.iIconGap;
			}
		}
		iCurrentOrder = cairo_dock_get_icon_order (pIcon);
		fCurrentGroupWidth += pIcon->fWidth + myIcons.iIconGap;
		
		g_print ("icon at %.2f\n", x);
		pIcon->fXAtRest = x;
		x += pIcon->fWidth + myIcons.iIconGap;
	}
	
	pDock->fMagnitudeMax = 0.;  // pas de vague.
	
	pDock->pFirstDrawnElement = pDock->icons;
	
	pDock->iDecorationsHeight = pDock->iMaxIconHeight * pDock->container.fRatio + 2 * myBackground.iFrameMargin;
	
	pDock->iMaxDockWidth = pDock->fFlatDockWidth = pDock->iMinDockWidth = MAX (W, x);
	g_print ("iMaxDockWidth : %d (%.2f)\n", pDock->iMaxDockWidth, pDock->container.fRatio);
	
	pDock->iMaxDockHeight = myBackground.iDockLineWidth + myBackground.iFrameMargin + pDock->iMaxIconHeight * pDock->container.fRatio + MAX (myLabels.iLabelSize, myBackground.iDockLineWidth + myBackground.iFrameMargin);
	pDock->iMaxDockHeight = MAX (pDock->iMaxDockHeight, pDock->iMaxIconHeight * (1 + myIcons.fAmplitude));  // au moins la taille du FBO.

	pDock->iDecorationsWidth = pDock->iMaxDockWidth;
	pDock->iMinDockHeight = 2 * (myBackground.iDockLineWidth + myBackground.iFrameMargin) + pDock->iMaxIconHeight * pDock->container.fRatio;
}


static void cd_render (cairo_t *pCairoContext, CairoDock *pDock)
{
	//\____________________ On trace le cadre.
	double fLineWidth = myBackground.iDockLineWidth;
	double fMargin = myBackground.iFrameMargin;
	double fRadius = (pDock->iDecorationsHeight + fLineWidth - 2 * myBackground.iDockRadius > 0 ? myBackground.iDockRadius : (pDock->iDecorationsHeight + fLineWidth) / 2 - 1);
	double fExtraWidth = 2 * fRadius + fLineWidth;
	double fDockWidth;
	int sens;
	double fDockOffsetX, fDockOffsetY;  // Offset du coin haut gauche du cadre.
	if (cairo_dock_is_extended_dock (pDock))  // mode panel etendu.
	{
		fDockWidth = pDock->container.iWidth - fExtraWidth;
		fDockOffsetX = fExtraWidth / 2;
	}
	else
	{
		fDockWidth = cairo_dock_get_current_dock_width_linear (pDock);
		Icon *pFirstIcon = cairo_dock_get_first_drawn_icon (pDock);
		fDockOffsetX = (pFirstIcon != NULL ? pFirstIcon->fX - fMargin : fExtraWidth / 2);
		if (fDockOffsetX < fExtraWidth / 2)
			fDockOffsetX = fExtraWidth / 2;
		if (fDockOffsetX + fDockWidth + fExtraWidth / 2 > pDock->container.iWidth)
			fDockWidth = pDock->container.iWidth - fDockOffsetX - fExtraWidth / 2;
	}
	if (pDock->container.bDirectionUp)
	{
		sens = 1;
		fDockOffsetY = pDock->container.iHeight - pDock->iDecorationsHeight - 1.5 * fLineWidth;
	}
	else
	{
		sens = -1;
		fDockOffsetY = pDock->iDecorationsHeight + 1.5 * fLineWidth;
	}

	cairo_save (pCairoContext);
	double fDeltaXTrapeze = cairo_dock_draw_frame (pCairoContext, fRadius, fLineWidth, fDockWidth, pDock->iDecorationsHeight, fDockOffsetX, fDockOffsetY, sens, 0., pDock->container.bIsHorizontal, myBackground.bRoundedBottomCorner);

	//\____________________ On dessine les decorations dedans.
	fDockOffsetY = (pDock->container.bDirectionUp ? pDock->container.iHeight - pDock->iDecorationsHeight - fLineWidth : fLineWidth);
	cairo_dock_render_decorations_in_frame (pCairoContext, pDock, fDockOffsetY, fDockOffsetX - fDeltaXTrapeze, fDockWidth + 2*fDeltaXTrapeze);

	//\____________________ On dessine le cadre.
	if (fLineWidth > 0)
	{
		cairo_set_line_width (pCairoContext, fLineWidth);
		cairo_set_source_rgba (pCairoContext, myBackground.fLineColor[0], myBackground.fLineColor[1], myBackground.fLineColor[2], myBackground.fLineColor[3]);
		cairo_stroke (pCairoContext);
	}
	else
		cairo_new_path (pCairoContext);
	cairo_restore (pCairoContext);

	//\____________________ On dessine la ficelle qui les joint.
	if (myIcons.iStringLineWidth > 0)
		cairo_dock_draw_string (pCairoContext, pDock, myIcons.iStringLineWidth, FALSE, FALSE);

	//\____________________ On dessine les icones et les etiquettes, en tenant compte de l'ordre pour dessiner celles en arriere-plan avant celles en avant-plan.
	GList *pFirstDrawnElement = cairo_dock_get_first_drawn_element_linear (pDock->icons);
	if (pFirstDrawnElement == NULL)
		return;
	
	double fDockMagnitude = cairo_dock_calculate_magnitude (pDock->iMagnitudeIndex);  // * pDock->fMagnitudeMax
	Icon *icon;
	GList *ic = pFirstDrawnElement;
	do
	{
		icon = ic->data;

		cairo_save (pCairoContext);
		cairo_dock_render_one_icon (icon, pDock, pCairoContext, fDockMagnitude, icon->bPointed);
		cairo_restore (pCairoContext);

		ic = cairo_dock_get_next_element (ic, pDock->icons);
	} while (ic != pFirstDrawnElement);
}



static void cd_render_optimized (cairo_t *pCairoContext, CairoDock *pDock, GdkRectangle *pArea)
{
	//g_print ("%s ((%d;%d) x (%d;%d) / (%dx%d))\n", __func__, pArea->x, pArea->y, pArea->width, pArea->height, pDock->container.iWidth, pDock->container.iHeight);
	double fLineWidth = myBackground.iDockLineWidth;
	double fMargin = myBackground.iFrameMargin;
	int iWidth = pDock->container.iWidth;
	int iHeight = pDock->container.iHeight;

	//\____________________ On dessine les decorations du fond sur la portion de fenetre.
	cairo_save (pCairoContext);

	double fDockOffsetX, fDockOffsetY;
	if (pDock->container.bIsHorizontal)
	{
		fDockOffsetX = pArea->x;
		fDockOffsetY = (pDock->container.bDirectionUp ? iHeight - pDock->iDecorationsHeight - fLineWidth : fLineWidth);
	}
	else
	{
		fDockOffsetX = (pDock->container.bDirectionUp ? iHeight - pDock->iDecorationsHeight - fLineWidth : fLineWidth);
		fDockOffsetY = pArea->y;
	}

	if (pDock->container.bIsHorizontal)
		cairo_rectangle (pCairoContext, fDockOffsetX, fDockOffsetY, pArea->width, pDock->iDecorationsHeight);
	else
		cairo_rectangle (pCairoContext, fDockOffsetX, fDockOffsetY, pDock->iDecorationsHeight, pArea->height);

	fDockOffsetY = (pDock->container.bDirectionUp ? pDock->container.iHeight - pDock->iDecorationsHeight - fLineWidth : fLineWidth);
	
	double fRadius = MIN (myBackground.iDockRadius, (pDock->iDecorationsHeight + myBackground.iDockLineWidth) / 2 - 1);
	double fOffsetX;
	if (cairo_dock_is_extended_dock (pDock))  // mode panel etendu.
	{
		fOffsetX = fRadius + fLineWidth / 2;
	}
	else
	{
		Icon *pFirstIcon = cairo_dock_get_first_drawn_icon (pDock);
		fOffsetX = (pFirstIcon != NULL ? pFirstIcon->fX - fMargin : fRadius + fLineWidth / 2);
	}
	double fDockWidth = cairo_dock_get_current_dock_width_linear (pDock);
	double fDeltaXTrapeze = fRadius;
	cairo_dock_render_decorations_in_frame (pCairoContext, pDock, fDockOffsetY, fOffsetX - fDeltaXTrapeze, fDockWidth + 2*fDeltaXTrapeze);
	
	//\____________________ On dessine la partie du cadre qui va bien.
	cairo_new_path (pCairoContext);

	if (pDock->container.bIsHorizontal)
	{
		cairo_move_to (pCairoContext, fDockOffsetX, fDockOffsetY - fLineWidth / 2);
		cairo_rel_line_to (pCairoContext, pArea->width, 0);
		cairo_set_line_width (pCairoContext, fLineWidth);
		cairo_set_source_rgba (pCairoContext, myBackground.fLineColor[0], myBackground.fLineColor[1], myBackground.fLineColor[2], myBackground.fLineColor[3]);
		cairo_stroke (pCairoContext);

		cairo_new_path (pCairoContext);
		cairo_move_to (pCairoContext, fDockOffsetX, (pDock->container.bDirectionUp ? iHeight - fLineWidth / 2 : pDock->iDecorationsHeight + 1.5 * fLineWidth));
		cairo_rel_line_to (pCairoContext, pArea->width, 0);
		cairo_set_line_width (pCairoContext, fLineWidth);
		cairo_set_source_rgba (pCairoContext, myBackground.fLineColor[0], myBackground.fLineColor[1], myBackground.fLineColor[2], myBackground.fLineColor[3]);
	}
	else
	{
		cairo_move_to (pCairoContext, fDockOffsetX - fLineWidth / 2, fDockOffsetY);
		cairo_rel_line_to (pCairoContext, 0, pArea->height);
		cairo_set_line_width (pCairoContext, fLineWidth);
		cairo_set_source_rgba (pCairoContext, myBackground.fLineColor[0], myBackground.fLineColor[1], myBackground.fLineColor[2], myBackground.fLineColor[3]);
		cairo_stroke (pCairoContext);

		cairo_new_path (pCairoContext);
		cairo_move_to (pCairoContext, (pDock->container.bDirectionUp ? iHeight - fLineWidth / 2 : pDock->iDecorationsHeight + 1.5 * fLineWidth), fDockOffsetY);
		cairo_rel_line_to (pCairoContext, 0, pArea->height);
		cairo_set_line_width (pCairoContext, fLineWidth);
		cairo_set_source_rgba (pCairoContext, myBackground.fLineColor[0], myBackground.fLineColor[1], myBackground.fLineColor[2], myBackground.fLineColor[3]);
	}
	cairo_stroke (pCairoContext);

	cairo_restore (pCairoContext);

	//\____________________ On dessine les icones impactees.
	cairo_set_operator (pCairoContext, CAIRO_OPERATOR_OVER);

	GList *pFirstDrawnElement = (pDock->pFirstDrawnElement != NULL ? pDock->pFirstDrawnElement : pDock->icons);
	if (pFirstDrawnElement != NULL)
	{
		double fXMin = (pDock->container.bIsHorizontal ? pArea->x : pArea->y), fXMax = (pDock->container.bIsHorizontal ? pArea->x + pArea->width : pArea->y + pArea->height);
		double fDockMagnitude = cairo_dock_calculate_magnitude (pDock->iMagnitudeIndex);
		double fRatio = pDock->container.fRatio;
		double fXLeft, fXRight;
		
		//g_print ("redraw [%d -> %d]\n", (int) fXMin, (int) fXMax);
		Icon *icon;
		GList *ic = pFirstDrawnElement;
		do
		{
			icon = ic->data;

			fXLeft = icon->fDrawX + icon->fScale + 1;
			fXRight = icon->fDrawX + (icon->fWidth - 1) * icon->fScale * icon->fWidthFactor - 1;

			if (fXLeft < fXMax && fXRight > fXMin)
			{
				cairo_save (pCairoContext);
				//g_print ("dessin optimise de %s [%.2f -> %.2f]\n", icon->cName, fXLeft, fXRight);
				
				icon->fAlpha = 1;
				if (icon->iAnimationState == CAIRO_DOCK_STATE_AVOID_MOUSE)
				{
					icon->fAlpha = 0.7;
				}
				
				cairo_dock_render_one_icon (icon, pDock, pCairoContext, fDockMagnitude, icon->bPointed);
				cairo_restore (pCairoContext);
			}

			ic = cairo_dock_get_next_element (ic, pDock->icons);
		} while (ic != pFirstDrawnElement);
	}
}



static void cd_render_opengl (CairoDock *pDock)
{
	GLsizei w = pDock->container.iWidth;
	GLsizei h = pDock->container.iHeight;
	
	//\_____________ On definit notre rectangle.
	double fLineWidth = myBackground.iDockLineWidth;
	double fMargin = myBackground.iFrameMargin;
	double fRadius = (pDock->iDecorationsHeight + fLineWidth - 2 * myBackground.iDockRadius > 0 ? myBackground.iDockRadius : (pDock->iDecorationsHeight + fLineWidth) / 2 - 1);
	double fExtraWidth = 2 * fRadius + fLineWidth;
	double fDockWidth;
	double fFrameHeight = pDock->iDecorationsHeight + fLineWidth;
	
	int sens;
	double fDockOffsetX, fDockOffsetY;  // Offset du coin haut gauche du cadre.
	GList *pFirstDrawnElement = (pDock->pFirstDrawnElement != NULL ? pDock->pFirstDrawnElement : pDock->icons);
	if (pFirstDrawnElement == NULL)
		return ;
	if (cairo_dock_is_extended_dock (pDock))  // mode panel etendu.
	{
		fDockWidth = pDock->container.iWidth - fExtraWidth;
		fDockOffsetX = fRadius + fLineWidth / 2;
	}
	else
	{
		fDockWidth = cairo_dock_get_current_dock_width_linear (pDock);
		Icon *pFirstIcon = pFirstDrawnElement->data;
		fDockOffsetX = (pFirstIcon != NULL ? pFirstIcon->fX + 0 - fMargin : fRadius + fLineWidth / 2);
		if (fDockOffsetX - (fRadius + fLineWidth / 2) < 0)
			fDockOffsetX = fRadius + fLineWidth / 2;
		if (fDockOffsetX + fDockWidth + (fRadius + fLineWidth / 2) > pDock->container.iWidth)
			fDockWidth = pDock->container.iWidth - fDockOffsetX - (fRadius + fLineWidth / 2);
	}
	
	if ((pDock->container.bIsHorizontal && ! pDock->container.bDirectionUp) || (! pDock->container.bIsHorizontal && pDock->container.bDirectionUp))
		fDockOffsetY = pDock->container.iHeight - .5 * fLineWidth;
	else
		fDockOffsetY = pDock->iDecorationsHeight + 1.5 * fLineWidth;
	
	double fDockMagnitude = cairo_dock_calculate_magnitude (pDock->iMagnitudeIndex);
	
	//\_____________ On genere les coordonnees du contour.
	int iNbVertex;
	const GLfloat *pVertexTab = cairo_dock_generate_rectangle_path (fDockWidth, fFrameHeight, fRadius, myBackground.bRoundedBottomCorner, &iNbVertex);
	
	if (! pDock->container.bIsHorizontal)
		fDockOffsetX = pDock->container.iWidth - fDockOffsetX + fRadius;
	else
		fDockOffsetX = fDockOffsetX - fRadius;
	
	//\_____________ On trace le fond en texturant par des triangles.
	glPushMatrix ();
	cairo_dock_draw_frame_background_opengl (g_pDockBackgroundBuffer.iTexture, fDockWidth+2*fRadius, fFrameHeight, fDockOffsetX, fDockOffsetY, pVertexTab, iNbVertex, pDock->container.bIsHorizontal, pDock->container.bDirectionUp, pDock->fDecorationsOffsetX);
	
	//\_____________ On trace le contour.
	if (fLineWidth != 0)
		cairo_dock_draw_current_path_opengl (fLineWidth, myBackground.fLineColor, iNbVertex);
	glPopMatrix ();
	
	//\_____________ On dessine la ficelle.
	if (myIcons.iStringLineWidth > 0)
		cairo_dock_draw_string_opengl (pDock, myIcons.iStringLineWidth, FALSE, FALSE);
	
	
	//\_____________ On dessine les icones.
	/**glEnable (GL_LIGHTING);  // pour indiquer a OpenGL qu'il devra prendre en compte l'eclairage.
	glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, 1.);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);  // OpenGL doit considerer pour ses calculs d'eclairage que l'oeil est dans la scene (plus realiste).
	GLfloat fGlobalAmbientColor[4] = {.0, .0, .0, 0.};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fGlobalAmbientColor);  // on definit la couleur de la lampe d'ambiance.
	glEnable (GL_LIGHT0);  // on allume la lampe 0.
	GLfloat fAmbientColor[4] = {.1, .1, .1, 1.};
	//glLightfv (GL_LIGHT0, GL_AMBIENT, fAmbientColor);
	GLfloat fDiffuseColor[4] = {.8, .8, .8, 1.};
	glLightfv (GL_LIGHT0, GL_DIFFUSE, fDiffuseColor);
	GLfloat fSpecularColor[4] = {.9, .9, .9, 1.};
	glLightfv (GL_LIGHT0, GL_SPECULAR, fSpecularColor);
	glLightf (GL_LIGHT0, GL_SPOT_EXPONENT, 10.);
	GLfloat fDirection[4] = {.3, .0, -.8, 0.};  // le dernier 0 <=> direction.
	glLightfv(GL_LIGHT0, GL_POSITION, fDirection);*/
	
	pFirstDrawnElement = cairo_dock_get_first_drawn_element_linear (pDock->icons);
	if (pFirstDrawnElement == NULL)
		return;
	
	Icon *icon;
	GList *ic = pFirstDrawnElement;
	do
	{
		icon = ic->data;
		
		glPushMatrix ();
		cairo_dock_render_one_icon_opengl (icon, pDock, fDockMagnitude, icon->bPointed);
		glPopMatrix ();
		
		ic = cairo_dock_get_next_element (ic, pDock->icons);
	} while (ic != pFirstDrawnElement);
	//glDisable (GL_LIGHTING);
}


static Icon *cd_calculate_icons (CairoDock *pDock)
{
	//\_____________ On calcule le nombre de groupes et la place qu'ils occupent.
	int iNbGroups = 1, iCurrentOrder = -1;
	double fCurrentGroupWidth = - myIcons.iIconGap, fGroupsWidth = 0.;
	double fSeparatorsPonderation = 0;
	GList *ic;
	Icon *pIcon;
	for (ic = pDock->icons; ic != NULL; ic = ic->next)
	{
		pIcon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (pIcon))
		{
			pIcon->fScale = 1.;
			if (pIcon->fInsertRemoveFactor != 0)
			{
				if (pIcon->fInsertRemoveFactor > 0)
					pIcon->fScale *= pIcon->fInsertRemoveFactor;
				else
					pIcon->fScale *= (1 + pIcon->fInsertRemoveFactor);
			}
			
			if (CAIRO_DOCK_IS_USER_SEPARATOR (pIcon))  // si c'est un separateur automatique, le changement de groupe incrementera le compteur a l'icone suivante.
			{
				if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
				{
					iNbGroups ++;
					fSeparatorsPonderation += pIcon->fScale;
					fGroupsWidth += MAX (0, fCurrentGroupWidth);
					fCurrentGroupWidth = - myIcons.iIconGap;
				}
			}
			continue;
		}
		if (iCurrentOrder != (int)cairo_dock_get_icon_order (pIcon))
		{
			if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
			{
				iNbGroups ++;
				fSeparatorsPonderation ++; // seuls les separateurs utilisateurs peuvent zoomer.
				fGroupsWidth += MAX (0, fCurrentGroupWidth);
				fCurrentGroupWidth = - myIcons.iIconGap;
			}
		}
		
		pIcon->fScale = 1.;
		if (pIcon->fInsertRemoveFactor != 0)
		{
			if (pIcon->fInsertRemoveFactor > 0)
				pIcon->fScale *= pIcon->fInsertRemoveFactor;
			else
				pIcon->fScale *= (1 + pIcon->fInsertRemoveFactor);
		}
		
		iCurrentOrder = cairo_dock_get_icon_order (pIcon);
		fCurrentGroupWidth += pIcon->fWidth * pIcon->fScale + myIcons.iIconGap;
	}
	if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste un separateur a la fin.
	{
		fGroupsWidth += MAX (0, fCurrentGroupWidth);
	}
	if (fGroupsWidth < 0)
		fGroupsWidth = 0;
	
	//\_____________ On en deduit l'ecart entre les groupes d'icones.
	double W = cairo_dock_get_max_authorized_dock_width (pDock);
	double fScreenBorderGap = myBackground.iDockRadius + myBackground.iDockLineWidth;  // on laisse un ecart avec le bord de l'ecran.
	double fGroupGap;
	if (iNbGroups > 1)
	{
		fGroupGap = (W - 2*fScreenBorderGap - fGroupsWidth) / (iNbGroups - 1);
		if (fSeparatorsPonderation != 0 && iNbGroups > 2)
			fGroupGap /= fSeparatorsPonderation / (iNbGroups - 1);
	}
	else
		fGroupGap = W - fScreenBorderGap - fGroupsWidth;
	
	//\_____________ On determine la position de chaque icone.
	Icon *pPointedIcon = NULL;
	double xm = pDock->container.iMouseX;
	double xg = fScreenBorderGap;  // abscisse de l'icone courante, et abscisse du debut du groupe courant.
	double x = xg;
	fCurrentGroupWidth = - myIcons.iIconGap;
	iCurrentOrder = -1;
	for (ic = pDock->icons; ic != NULL; ic = ic->next)
	{
		pIcon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (pIcon))
		{
			pIcon->fX = x;
			pIcon->fDrawX = pIcon->fX;
			if (CAIRO_DOCK_IS_USER_SEPARATOR (pIcon))  // si c'est un separateur automatique, le changement de groupe incrementera le compteur a l'icone suivante.
			{
				if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
				{
					xg += fCurrentGroupWidth + fGroupGap * pIcon->fScale;
					if (pPointedIcon == NULL && xm > x && xm < xg)
					{
						pIcon->bPointed = TRUE;
						pPointedIcon = pIcon;
					}
					else
						pIcon->bPointed = FALSE;
					x = xg;
					fCurrentGroupWidth = - myIcons.iIconGap;
				}
			}
			continue;
		}
		if (iCurrentOrder != (int)cairo_dock_get_icon_order (pIcon))
		{
			if (fCurrentGroupWidth > 0)  // le groupe courant est non vide, sinon c'est juste 2 separateurs cote a cote.
			{
				xg += fCurrentGroupWidth + fGroupGap;
				x = xg;
				fCurrentGroupWidth = - myIcons.iIconGap;
			}
		}
		iCurrentOrder = cairo_dock_get_icon_order (pIcon);
		fCurrentGroupWidth += pIcon->fWidth * pIcon->fScale + myIcons.iIconGap;
		
		pIcon->fX = x;
		if (pPointedIcon == NULL && xm > pIcon->fX - .5*myIcons.iIconGap && xm <= pIcon->fX + pIcon->fWidth * pIcon->fScale + .5*myIcons.iIconGap)
		{
			pIcon->bPointed = TRUE;
			pPointedIcon = pIcon;
		}
		else
			pIcon->bPointed = FALSE;
		pIcon->fDrawX = pIcon->fX;
		
		if (pDock->container.bDirectionUp)
			pIcon->fY = pDock->iMaxDockHeight - (myBackground.iDockLineWidth + myBackground.iFrameMargin + pIcon->fHeight);
		else
			pIcon->fY = myBackground.iDockLineWidth + myBackground.iFrameMargin;
		pIcon->fDrawY = pIcon->fY;
		
		pIcon->fWidthFactor = 1.;
		pIcon->fHeightFactor = 1.;
		pIcon->fOrientation = 0.;
		pIcon->fAlpha = 1.;
		
		x += pIcon->fWidth * pIcon->fScale + myIcons.iIconGap;
	}
	
	cairo_dock_check_if_mouse_inside_linear (pDock);
	
	cairo_dock_check_can_drop_linear (pDock);
	
	return pPointedIcon;
}


void cd_rendering_register_panel_renderer (const gchar *cRendererName)
{
	CairoDockRenderer *pRenderer = g_new0 (CairoDockRenderer, 1);
	pRenderer->cReadmeFilePath = g_strdup_printf ("%s/readme-panel-view", MY_APPLET_SHARE_DATA_DIR);
	pRenderer->cPreviewFilePath = g_strdup_printf ("%s/preview-panel.png", MY_APPLET_SHARE_DATA_DIR);
	pRenderer->compute_size = cd_compute_size;
	pRenderer->calculate_icons = cd_calculate_icons;
	pRenderer->render = cd_render;
	pRenderer->render_optimized = cd_render_optimized;
	pRenderer->render_opengl = cd_render_opengl;
	pRenderer->set_subdock_position = cairo_dock_set_subdock_position_linear;
	pRenderer->bUseReflect = FALSE;
	pRenderer->cDisplayedName = D_ (cRendererName);

	cairo_dock_register_renderer (cRendererName, pRenderer);
}
