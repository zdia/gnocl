/**
\brief      This module implements the gnocl::fontButton widget.
\author     Peter G. Baum, William J Giddings
\date       2001-03:
\note
            'The GtkFontButton is a button which displays the currently selected
            font an allows to open a font selection dialog to change the font.
            It is suitable widget for selecting a font in a preference dialog.'
**/

/* user documentation */

/** \page page20 gnocl::fontButton
  \section sec gnocl::fontButton
  Implementation of gnocl::fontButton
  \subsection subsection1 Implemented gnocl::fontButton Options
  \subsection subsection2 Implemented gnocl::fontButton Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/fontButton.png"
*/


/*
 * $Id: fontButton.c,v 1.2 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the fontButton widget
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

/*
Options:

-fontName   <string>    The name of the currently selected font. Default value: "Sans 12"
-showSize   <boolean>   If this property is set to TRUE, the selected font size will be shown in the label. Default value: TRUE.
-showStyle  <boolean>   If this property is set to TRUE, the name of the selected font style will be shown in the label. Default value: TRUE.
-title      <string>    The title of the font selection dialog. Default value: "Pick a Font"
-useFont    <boolean>   If this property is set to TRUE, the label will be drawn in the selected font. Default value: FALSE
-useSize    <boolean>   If this property is set to TRUE, the label will be drawn with the selected font size. Default value: FALSE
-onFontSet  <script>    Respond to the font-set signal.
    %w widget   name of the widget that generated this command.
    %f font     font desctription seleced by the user.
-data       <string>
-name       <string>
-tooltip    <string>

Commands:

*/
static GnoclOption fontButtonOptions[] =
{

	/* GtkFontButton properties */
	{ "-fontName", GNOCL_STRING, "font-name" },
	{ "-showSize", GNOCL_BOOL, "show-size" },
	{ "-showStyle", GNOCL_BOOL, "show-style" },
	{ "-title", GNOCL_STRING, "title" },
	{ "-useFont", GNOCL_BOOL, "use-font" },
	{ "-useSize", GNOCL_BOOL, "use-size" },

	/* GtkFontButton signals */
	{"-onFontSet", GNOCL_OBJ, "", gnoclOptOnFontSet },

	/* other properties */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ NULL },
};

/**
/brief
/author     William J Giddings
/date
/note       The are no special circumstances for this function to handle.
**/
static int configure (  Tcl_Interp *interp, GtkButton *button,  GnoclOption options[] )
{

#ifdef DEBUG
	printf ( "fontButton configure - no special requirements.\n" );
#endif

	return TCL_OK;

}

/**
\brief      This function is only concerned with the reconfiguration of
            the button, not the dialog.
\author     William J Giddings
\date
**/
int fontButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx };
	GtkFontButton *button = GTK_FONT_BUTTON ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "fontButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( button ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, fontButtonOptions, G_OBJECT ( button ) ) == TCL_OK )
				{
					ret = configure ( interp, button, fontButtonOptions );
				}

				gnoclClearOptions ( fontButtonOptions );

				return ret;
			}

			break;
		case OnClickedIdx:
			{
#ifdef DEBUG
				printf ( "fontButtonFunc - onClicked \n" );
#endif

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( button ) ) )
				{
					gtk_button_clicked ( GTK_BUTTON ( button ) );
				}



			}
			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( button ),
									 fontButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return gnoclCgetNotImplemented ( interp,
														 fontButtonOptions + idx );
				}
			}
	}

	return TCL_OK;
}

/**
/brief
/author     William J Giddings
**/
int gnoclFontButtonCmd ( ClientData data, Tcl_Interp *interp,
						 int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkFontButton *button;

	if ( gnoclParseOptions ( interp, objc, objv, fontButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( fontButtonOptions );
		return TCL_ERROR;
	}

	button = GTK_FONT_BUTTON ( gtk_font_button_new( ) );

	gtk_widget_show ( GTK_WIDGET ( button ) );

	ret = gnoclSetOptions ( interp, fontButtonOptions, G_OBJECT ( button ), -1 );
	gnoclClearOptions ( fontButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( button ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( button ), fontButtonFunc );
}

/*****/
