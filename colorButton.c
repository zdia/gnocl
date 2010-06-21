/**
\brief      This module implements the gnocl::colorButton widget.
\authors    Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation */

/** \page page9 gnocl::colorButton
  \section sec DESCRIPTION
  Implementation of...
  \subsection subsection1 Implemented gnocl::colorButton Options
  \subsection subsection2 Implemented gnocl::colorButton Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/colorButton.png"
*/

/*
 * $Id: colorButton.c,v 1.2 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the colorButton widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2008-10: added command, class
   2004-12: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption colorButtonOptions[] =
{
	{ "-alpha", GNOCL_INT, "alpha" },
	{ "-color", GNOCL_OBJ, "color", gnoclOptGdkColor },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onClicked", GNOCL_OBJ, "clicked", gnoclOptCommand },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-title", GNOCL_STRING, "title" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-useAlpha", GNOCL_BOOL, "use-alpha" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ NULL },
};

/**
/brief
/author     William J Giddings
**/
static int configure (  Tcl_Interp *interp, GtkButton *button,  GnoclOption options[] )
{

#ifdef DEBUG
	printf ( "colorButton configure - not fully supported\n" );
#endif

	return TCL_OK;

}

/**
/brief
/author
**/
int clrButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx };
	GtkColorButton *button = GTK_COLOR_BUTTON ( data );
	int idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "colorButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( button ), objc, objv );
		case ConfigureIdx:
			{
#ifdef DEBUG
				printf ( "colorButton clrButtonFunc ConfigureIdx\n" );
#endif
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, colorButtonOptions, G_OBJECT ( button ) ) == TCL_OK )
				{
					ret = configure ( interp, button, colorButtonOptions );
				}

				gnoclClearOptions ( colorButtonOptions );

				return ret;

			} break;
		case OnClickedIdx:

			if ( objc != 2 )
			{
				Tcl_WrongNumArgs ( interp, 2, objv, NULL );
				return TCL_ERROR;
			}

			if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( button ) ) )
				gtk_button_clicked ( GTK_BUTTON ( button ) );

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( button ),
									 colorButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return gnoclCgetNotImplemented ( interp,
														 colorButtonOptions + idx );
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author     William J Giddings
**/
int gnoclColorButtonCmd ( ClientData data, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkColorButton *button;

	if ( gnoclParseOptions ( interp, objc, objv, colorButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( colorButtonOptions );
		return TCL_ERROR;
	}

	button = GTK_COLOR_BUTTON ( gtk_color_button_new( ) );

	gtk_widget_show ( GTK_WIDGET ( button ) );

	ret = gnoclSetOptions ( interp, colorButtonOptions, G_OBJECT ( button ), -1 );
	gnoclClearOptions ( colorButtonOptions );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, button, colorButtonOptions );
	}

	gnoclClearOptions ( colorButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( button ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( button ), clrButtonFunc );
}


