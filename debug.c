/**
**/

/*
 * $Id: debug.c,v 1.2 2005/02/25 21:51:53 baum Exp $
 *
 * This file implements a Tcl interface to GTK+ and Gnome
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2005-02: begin of developement
*/

#include "gnocl.h"
#include <string.h>
#include <ctype.h>

/**
\brief
\author     PGB
**/
int gnoclDebugCmd ( ClientData data, Tcl_Interp *interp,
					int objc, Tcl_Obj * const objv[] )
{
	static const char *cmd[] = { "breakpoint", NULL };
	enum optIdx { BreakpointIdx };
	int idx;

	if ( objc != 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT,
							   &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case BreakpointIdx:
			/* this is only for debugging */
			G_BREAKPOINT();
			break;
	}

	return TCL_OK;
}

/**
\brief
\author         William J Giddings
\date           2009-02-04
**/
int gnoclPutsObj ( Tcl_Obj * const objv[] )
{
	if ( 0 )
	{
		g_printf ( "objv[1] %s\n", objv[1] );
	}

	return TCL_OK;
}

