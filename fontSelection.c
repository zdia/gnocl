/**
\brief     This module implements the gnocl::fontSelection widget.
\author     Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation */

/** \page page21 gnocl::fontSelection
  \section sec gnocl::fontSelection
  Implementation of gnocl::fontSelection
  \subsection subsection1 Implemented gnocl::fontSelection fontSelectOptions
  \subsection subsection2 Implemented gnocl::fontSelection Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/fontSelection.png"
*/

/*
 * $Id: fontSelection.c,v 1.5 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the file selection dialog
 *
 * Copyright (c) 2002 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2008-10: added command, class
   2003-02: cleanups with GnoclOption
   2002-07: start of developement
*/

#include "gnocl.h"
#include "string.h"
#include <assert.h>


/*
  "font"                     GdkFont*              : Read
  "font-name"                gchar*                : Read / Write
  "preview-text"             gchar*                : Read / Write
*/


static int setFont ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

#ifdef DEBUG
	printf ( "setFont 1\n" );
#endif

	int retval;

	retval = gtk_font_selection_set_font_name ( GTK_WIDGET ( obj ), Tcl_GetString ( opt->val.obj ) ) ;

#ifdef DEBUG
	printf ( "setFont 2\n" );
#endif

	if ( retval != 1 )
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

static GnoclOption fontSelectOptions[] =
{
	{ "-fontName", GNOCL_STRING, "font-name"},
	{ "-previewText", GNOCL_STRING, "preview-text" },
	{ NULL }
};

static const int commandIdx    = 0;
static const int modalIdx      = 1;
static const int fontSelectIdx = 6; /* first index which relates to the GtkFontSelection and not to the dialog */

/**
\brief
\author     Peter G Baum
\date
\note       No special conditions to handle.
**/
static int configure ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption fontSelectOptions[] )
{

#ifdef DEBUG
	int i;

	for ( i = 0 ; i <= 4 ; i++ )
	{
		g_print ( "fontSelectOptions  %d %s\n", i, fontSelectOptions[i] );
	}

#endif
	return TCL_OK;
}

/**
\brief      Obtain current -option values.
\author     Peter G Baum, William J Giddings
**/
static int cget (   Tcl_Interp *interp, GtkButton *button,  GnoclOption options[],  int idx )
{
	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief
\author
\date
**/
static int fontSelFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "class", "cget", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, ClassIdx, CgetIdx };

	GtkWidget *widget = GTK_WIDGET ( data  );
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
			{
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "fontSelection", -1 ) );
			}
			break;
		case DeleteIdx:
			{
				return gnoclDelete ( interp, widget, objc, objv );
			}
		case ConfigureIdx:
			{

				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, fontSelectOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, widget, fontSelectOptions );
				}

				gnoclClearOptions ( fontSelectOptions );

				return ret;
			}
		case CgetIdx:
			{
				int idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), fontSelectOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, widget, fontSelectOptions, idx );
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
int gnoclFontSelectionCmd ( ClientData data, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[] )
{


	int ret = TCL_OK;
	GtkWidget *widget;


	if ( gnoclParseOptions ( interp, objc, objv, fontSelectOptions ) != TCL_OK )
	{
		gnoclClearOptions ( fontSelectOptions );
		return TCL_ERROR;
	}


	widget = gtk_font_selection_new() ;

	gtk_widget_show ( GTK_WIDGET ( widget ) );



	ret = gnoclSetOptions ( interp, fontSelectOptions, G_OBJECT ( widget ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, G_OBJECT ( widget ), fontSelectOptions );
	}


	gnoclClearOptions ( fontSelectOptions );

	/* STEP 3)  -show the widget */

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( widget ) );
		return TCL_ERROR;
	}


	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), fontSelFunc );


	return ret;
}
