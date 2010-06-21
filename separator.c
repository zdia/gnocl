/**
\brief  This module implements the gnocl::separator widget.
\author Peter G. Baum, William J Giddings
\date 2001-03:
*/

/* user documentation */

/**
  \page page41 gnocl::separator
  \section sec gnocl::separator
  Implementation of gnocl::separator
  \subsection subsection1 Implemented Options
  \verbatim

  \endverbatim
  \subsection subsection2 Implemented Commands
  \verbatim

  \endverbatim
  \subsection subsection3 Sample Tcl Script
  \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
  \endcode
  \subsection subsection4 Produces
  \image html "../pics/separator.png"
*/
/*
 * $Id: separator.c,v 1.3 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the separator widget
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
   2003-06: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>


static GnoclOption separatorOptions[] =
{
	{ "-orientation", GNOCL_OBJ, NULL },    /* 0 */
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ NULL }
};

/**
\brief
\author
\date
**/
static const int orientationIdx  = 0;

/**
\brief
\author
\date
**/
int separatorFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, ClassIdx };
	GtkSeparator *separator = GTK_SEPARATOR ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "separator", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( separator ), objc, objv );
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseOptions ( interp, objc - 1, objv + 1,
										 separatorOptions ) == TCL_OK )
				{
					if ( separatorOptions[orientationIdx].status
							== GNOCL_STATUS_CHANGED )
					{
						Tcl_SetResult ( interp,
										"Option \"-orientation\"  can only set on creation.",
										TCL_STATIC );
						ret = TCL_ERROR;
					}

					else
						ret = gnoclSetOptions ( interp, separatorOptions,
												G_OBJECT ( separator ), -1 );
				}

				gnoclClearOptions ( separatorOptions );

				return ret;
			}

			break;
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
int gnoclSeparatorCmd ( ClientData data, Tcl_Interp *interp,
						int objc, Tcl_Obj * const objv[] )
{
	int            ret;
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	GtkSeparator   *separator;

	if ( gnoclParseOptions ( interp, objc, objv, separatorOptions ) != TCL_OK )
	{
		gnoclClearOptions ( separatorOptions );
		return TCL_ERROR;
	}

	if ( separatorOptions[orientationIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGetOrientationType ( interp,
									   separatorOptions[orientationIdx].val.obj, &orient ) != TCL_OK )
		{
			gnoclClearOptions ( separatorOptions );
			return TCL_ERROR;
		}
	}

	if ( orient == GTK_ORIENTATION_HORIZONTAL )
		separator = GTK_SEPARATOR ( gtk_hseparator_new( ) );
	else
		separator = GTK_SEPARATOR ( gtk_vseparator_new( ) );

	gtk_widget_show ( GTK_WIDGET ( separator ) );

	/* gtk_widget_set_sensitive( GTK_WIDGET( para->menuItem ), 0 ); */

	ret = gnoclSetOptions ( interp, separatorOptions, G_OBJECT ( separator ), -1 );

	gnoclClearOptions ( separatorOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( separator ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( separator ), separatorFunc );
}
