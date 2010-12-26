/**
 \brief
  This module implements the gnocl::callBack command.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/** \page page7 gnocl::callBack
  \section sec gnocl::callBack
  Implementation of gnocl::callBack
  \subsection subsection1 Implemented XXX Options
  \subsection subsection2 Implemented XXX Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/button.png"
*/


/****h* command/callback
 * NAME
 * 	callback.c
 * SYNOPSIS
 * 	This file implements the callback command.
 * FUNCTION
 * NOTES
 * BUGS
 * SEE ALSO
 *****/

/*
 * $Id: callback.c,v 1.5 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the callback command
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2003-03: begin of developement
*/

#include "gnocl.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>

/****f* callback/doCommand
 * AUTHOR
 *	PGB
 * SOURCE
 */
static gboolean doCommand ( gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;
	int ret = Tcl_EvalEx ( cs->interp, cs->command, -1,
						   TCL_EVAL_GLOBAL | TCL_EVAL_DIRECT );

	if ( ret == TCL_ERROR )
		Tcl_BackgroundError ( cs->interp );

	if ( ret != TCL_OK )
		return 0;

	return 1;
}

/*****/

/****f* callback/destroyCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
static void destroyCmd ( gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;
	g_free ( ( char * ) cs->command );
	g_free ( cs );
}

/*****/

/****f* callback/create
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int create ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	GnoclOption options[] =
	{
		{ "-interval", GNOCL_OBJ, NULL },    /* 0 */
		{ "-priority", GNOCL_INT, NULL },    /* 1 */
		{ NULL }
	};
	const int intervalIdx = 0;
	const int priorityIdx = 1;

	int interval = -1;
	int priority = 0;
	int id;
	GnoclCommandData *cs;

	if ( objc < 3 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, "script" );
		return TCL_ERROR;
	}

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, options ) != TCL_OK )
		goto errorExit;

	if ( options[priorityIdx].status == GNOCL_STATUS_CHANGED )
		priority = options[priorityIdx].val.i;

	/* TODO? test priority range? */

	if ( options[intervalIdx].status == GNOCL_STATUS_CHANGED )
	{
		Tcl_Obj * const obj = options[intervalIdx].val.obj;

		if ( Tcl_GetIntFromObj ( NULL, obj, &interval ) != TCL_OK )
		{
			if ( strcmp ( Tcl_GetString ( obj ), "idle" ) != 0 )
			{
				Tcl_AppendResult ( interp,
								   "Expected integer or \"idle\", but got \"",
								   Tcl_GetString ( obj ), "\"", NULL );
				goto errorExit;
			}
		}

		else if ( interval <= 0 )
		{
			Tcl_SetResult ( interp, "interval must be greater zero.",
							TCL_STATIC );
			goto errorExit;
		}

	}

	gnoclClearOptions ( options );

	cs = g_new ( GnoclCommandData, 1 );
	cs->command = g_strdup ( Tcl_GetString ( objv[2] ) );
	cs->interp = interp;


	if ( interval <= 0 ) /* idle */
	{
		id = g_idle_add_full ( G_PRIORITY_DEFAULT_IDLE - priority,
							   doCommand, cs, destroyCmd );
	}

	else
	{
		id = g_timeout_add_full ( G_PRIORITY_DEFAULT_IDLE - priority, interval,
								  doCommand, cs, destroyCmd );
	}

	Tcl_SetObjResult ( interp, Tcl_NewIntObj ( id ) );

	return TCL_OK;


errorExit:
	gnoclClearOptions ( options );

	return TCL_ERROR;
}

/*****/

/****f* callback/delete
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int delete ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int      id;
	gboolean removed;

	if ( objc < 3 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, "ID" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIntFromObj ( interp, objv[2], &id ) != TCL_OK )
		return TCL_ERROR;

	removed = g_source_remove ( ( guint ) id );

	Tcl_SetObjResult ( interp, Tcl_NewIntObj ( removed ) );

	return TCL_OK;
}

/*****/

/****f* callback/gnoclCallbackCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclCallbackCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "create", "delete", NULL };
	enum cmdIdx { CreateIdx, DeleteIdx };
	int idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "subcommand" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "subcommand",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case CreateIdx: return create ( interp, objc, objv );
		case DeleteIdx: return delete ( interp, objc, objv );
		default:  assert ( 0 );
	}

	return TCL_ERROR;
}

/*****/
