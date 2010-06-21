/****h* widget/plug
 * NAME
 * 	plug.c
 * SYNOPSIS
 * 	This file implements the plug widget.
 * FUNCTION
 * NOTES
 * BUGS
 * SEE ALSO
 *****/
/*
 * $Id: plug.c,v 1.9 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the plug widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2003-06: added onDestroy
   2002-02: switched from GnoclWidgetOptions to GnoclOption
   2001-11: Begin of developement
 */

#include "gnocl.h"
#include <gdk/gdkx.h>
#include <string.h>
#include <assert.h>

/****v* plug/plugOptions
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GnoclOption plugOptions[] =
{
	{ "-socketID", GNOCL_OBJ, NULL },                    /* 0 */
	{ "-defaultWidth", GNOCL_INT, "default-width" },     /* these must be */
	{ "-defaultHeight", GNOCL_INT, "default-height" },   /* before -visible! */
	{ "-visible", GNOCL_BOOL, "visible" },               /* 3 */
	{ "-allowShrink", GNOCL_BOOL, "allow-shrink" },
	{ "-allowGrow", GNOCL_BOOL, "allow-grow" },
	{ "-child", GNOCL_OBJ, "", gnoclOptChild },
	{ "-resizable", GNOCL_BOOL, "resizable" },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ NULL },
};
/*****/

/****v* plug/constants
 * AUTHOR
 *	PGB
 * SOURCE
 */
static const int socketIDIdx = 0;
static const int visibleIdx  = 3;
/*****/

/****f* plug/staticFuncs/configure
 * AUTHOR
 *	PCB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkPlug *plug,
					   GnoclOption options[] )
{

	return TCL_OK;
}

/*****/

/****f* plug/staticFuncs/plugFunc
 * AUTHOR
 *	PCB
 * SOURCE
 */
static int plugFunc ( ClientData data, Tcl_Interp *interp,
					  int objc, Tcl_Obj * const objv[] )
{

	const char *cmds[] = { "delete", "configure", "getID", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, GetIDIdx };
	int idx;
	GtkPlug *plug = GTK_PLUG ( data );

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
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( plug ), objc, objv );
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( plugOptions[socketIDIdx].status == GNOCL_STATUS_CHANGED )
				{
					Tcl_SetResult ( interp,
									"Option -socketID cannot be changed after creation",
									TCL_STATIC );
					return TCL_ERROR;
				}

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   plugOptions, G_OBJECT ( plug ) ) == TCL_OK )
				{
					ret = configure ( interp, plug, plugOptions );
				}

				gnoclClearOptions ( plugOptions );

				return ret;
			}

			break;
		case GetIDIdx:
			{
				long xid;
				Tcl_Obj *val;

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				xid = gtk_plug_get_id ( plug );

				val = Tcl_NewLongObj ( xid );
				Tcl_SetObjResult ( interp, val );
			}

			break;
	}

	return TCL_OK;
}

/*****/

/****f* plug/gnoclPlugCmd
 * AUTHOR
 *	PCB
 * SOURCE
 */
int gnoclPlugCmd ( ClientData data, Tcl_Interp *interp,
				   int objc, Tcl_Obj * const objv[] )
{
	int     ret;
	GtkPlug *plug;
	long    socketId = 0;

	if ( gnoclParseOptions ( interp, objc, objv, plugOptions ) != TCL_OK )
	{
		gnoclClearOptions ( plugOptions );
		return TCL_ERROR;
	}

	if ( plugOptions[socketIDIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( Tcl_GetLongFromObj ( interp, plugOptions[socketIDIdx].val.obj,
								  &socketId ) != TCL_OK )
			return TCL_ERROR;
	}

	plug = GTK_PLUG ( gtk_plug_new ( socketId ) );

	if ( plugOptions[visibleIdx].status == 0 )
		gtk_widget_show ( GTK_WIDGET ( plug ) );

	ret = gnoclSetOptions ( interp, plugOptions, G_OBJECT ( plug ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, plug, plugOptions );

	gnoclClearOptions ( plugOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( plug ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( plug ), plugFunc );
}

/*****/
