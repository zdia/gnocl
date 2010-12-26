/**
 \brief
  This module implements the gnocl::progressBar widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page37 gnocl::progressBar
  \section sec gnocl::progressBar
  Implementation of gnocl::progressBar
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
  \image html "../pics/progressBar.png"
*/


/****h* widget/progressBar
 * NAME
 *  progressBar.c
 * SYNOPSIS
 *   This file implements the progressBar widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/progressBar.png">
 * SEE ALSO
 *****/
/*
 * $Id: progressBar.c,v 1.7 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the progressBar widget
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
   2002-10: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****f* progressBar/staticFuncs/optProgressBarOrientation
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int optProgressBarOrientation ( Tcl_Interp *interp, GnoclOption *opt,
									   GObject *obj, Tcl_Obj **ret )
{
	const char *txt[] = { "leftToRight", "rightToLeft",
						  "bottomToTop", "topToBottom", NULL
						};
	int types[] = { GTK_PROGRESS_LEFT_TO_RIGHT,
					GTK_PROGRESS_RIGHT_TO_LEFT, GTK_PROGRESS_BOTTOM_TO_TOP,
					GTK_PROGRESS_TOP_TO_BOTTOM
				  };

	assert ( sizeof ( GTK_PROGRESS_LEFT_TO_RIGHT ) == sizeof ( int ) );

	return gnoclOptGeneric ( interp, opt, obj, "progressBar style", txt, types, ret );
}

/*****/

/****v* progressBar/progressBarOptions
 * AUTHOR
 *  PGB
 * SOURCE
 */
static GnoclOption progressBarOptions[] =
{
	{ "-activityMode", GNOCL_BOOL, "activity-mode" },
	{ "-fraction", GNOCL_DOUBLE, "fraction" },
	{ "-pulseStep", GNOCL_DOUBLE, "pulse-step" },
	{ "-orientation", GNOCL_OBJ, "orientation", optProgressBarOrientation },
	{ "-text", GNOCL_STRING, "text" },
	{ "-textAlign", GNOCL_OBJ, "text-?align", gnoclOptBothAlign },
	{ "-showText", GNOCL_BOOL, "show-text" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ NULL }
};
/*****/

/*
static int configure( Tcl_Interp *interp, GtkProgressBar *progressBar,
      GnoclOption options[] )
{
   if( options[textIdx].status == GNOCL_STATUS_CHANGED )
   {
   }

   return TCL_OK;
}
*/

/****f* progressBar/staticFuncs/progressBarFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
int progressBarFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	static const char *cmds[] = { "delete", "configure", "pulse", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, PulseIdx, ClassIdx };
	int idx;
	GtkProgressBar *progressBar = ( GtkProgressBar * ) data;

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "progressBar", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( progressBar ), objc, objv );

		case ConfigureIdx:
			{
				int ret = gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
													progressBarOptions, G_OBJECT ( progressBar ) );
				gnoclClearOptions ( progressBarOptions );
				return ret;
			}

			break;
		case PulseIdx:
			gtk_progress_bar_pulse ( progressBar );
			break;
	}

	return TCL_OK;
}

/*****/

/****f* progressBar/gnoclProgressBarCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclProgressBarCmd ( ClientData data, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[] )
{
	GtkProgressBar *progressBar;
	int            ret;

	if ( gnoclParseOptions ( interp, objc, objv, progressBarOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( progressBarOptions );
		return TCL_ERROR;
	}

	progressBar = GTK_PROGRESS_BAR ( gtk_progress_bar_new( ) );

	ret = gnoclSetOptions ( interp, progressBarOptions,
							G_OBJECT ( progressBar ), -1 );
	/*
	if( ret == TCL_OK )
	   ret = configure( interp, progressBar, progressBarOptions );
	*/
	gnoclClearOptions ( progressBarOptions );

	gtk_widget_show ( GTK_WIDGET ( progressBar ) );

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( progressBar ),
								 progressBarFunc );
}

/*****/
