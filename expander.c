/**
\brief      This module implements the gnocl::expander widget.
\authors    Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation */


/**
\page page17 gnocl::expander
\section sec gnocl::expander
  Implementation of gnocl::expander
\subsection subsection1 Implemented gnocl::expander Options
\subsection subsection2 Implemented gnocl::expander Commands

\subsection subsection3 Sample Tcl Script
\code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
\endcode
\subsection subsection4 Produces
\image html "../pics/expander.png"
**/

/*
 * $Id: expander.c,v 1.2 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements the expander widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-12: added options -spacing -expanderSize -underline
   2008-10: added command, class
   2005-01: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption expanderOptions[] =
{
	{ "-child", GNOCL_OBJ, "", gnoclOptChild },
	{ "-expand", GNOCL_BOOL, "expanded" },
	{ "-label", GNOCL_OBJ, "label", gnoclOptLabelFull },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-spacing", GNOCL_INT, "spacing" },
	{ "-expanderSize", GNOCL_INT, "expander-size"},
	{ "-underline", GNOCL_BOOL, "use-underline" },
	{ "-useMarkup", GNOCL_BOOL, "use-markup" },
	{ NULL },
};

/**
\brief
\author
\date
\note
**/
static int setExpanderSize (
	Tcl_Interp *interp,
	GnoclOption *opt,
	GObject *obj,
	Tcl_Obj **ret )
{

#ifdef DEBUG
	g_print ( "treeLinePattern -this option currently under development.\n" );
#endif

	static const gchar *rc_string =
	{
		"style \"solidTreeLines\"\n"
		"{\n"
		"   GtkTreeView::tree-line-pattern = \"\111\111\"\n"
		"}\n"
		"\n"
		"class \"GtkTreeView\" style \"solidTreeLines\"\n"
	};

	gtk_rc_parse_string ( rc_string );


	return TCL_OK;
}


/**
\brief
\author
\date
\note
**/
static int setExpanderSpacing (
	Tcl_Interp *interp,
	GnoclOption *opt,
	GObject *obj,
	Tcl_Obj **ret )
{
#ifdef DEBUG
	g_print ( "treeLinePattern -this option currently under development.\n" );
#endif
	static const gchar *rc_string =
	{
		"style \"solidTreeLines\"\n"
		"{\n"
		"   GtkTreeView::tree-line-pattern = \"\111\111\"\n"
		"}\n"
		"\n"
		"class \"GtkTreeView\" style \"solidTreeLines\"\n"
	};

	gtk_rc_parse_string ( rc_string );


	return TCL_OK;
}

/**
\brief
\author
\date
\note
**/
static int configure ( Tcl_Interp *interp, GtkExpander *expander,
					   GnoclOption options[] )
{
	return TCL_OK;
}

/**
\brief
\author
\date
\note
**/
int expanderFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	const char *cmds[] = { "delete", "configure", "cget", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ClassIdx };
	int idx;
	GtkExpander *expander = GTK_EXPANDER ( data );

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "expander", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( expander ), objc, objv );
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   expanderOptions, G_OBJECT ( expander ) ) == TCL_OK )
				{
					ret = configure ( interp, expander, expanderOptions );
				}

				gnoclClearOptions ( expanderOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( expander ),
									 expanderOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						assert ( 0 );
				}

				assert ( 0 );
			}
	}

	return TCL_OK;
}

/**
\brief
\author
\date
\note
**/
int gnoclExpanderCmd ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	int         ret;
	GtkExpander *expander;

	if ( gnoclParseOptions ( interp, objc, objv, expanderOptions ) != TCL_OK )
	{
		gnoclClearOptions ( expanderOptions );
		return TCL_ERROR;
	}

	expander = GTK_EXPANDER ( gtk_expander_new ( "" ) );

	gtk_widget_show ( GTK_WIDGET ( expander ) );

	ret = gnoclSetOptions ( interp, expanderOptions, G_OBJECT ( expander ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, expander, expanderOptions );

	gnoclClearOptions ( expanderOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( expander ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( expander ), expanderFunc );
}

/*****/
