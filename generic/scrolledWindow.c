/**
 \brief
  This module implements the gnocl::scrolledWindow widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page40 gnocl::scrolledWindow
  \section sec gnocl::scrolledWindow
  Implementation of gnocl::scrolledWindow
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
  \image html "../pics/scrolledWindow.png"
*/
/*
* $Id: scrolledWindow.c,v 1.9 2005/01/01 15:27:54 baum Exp $
*
* This file implements the scrolledWindow widget
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
   2003-08: added gnoclOptPadding
        12: switched from GnoclWidgetOptions to GnoclOption
   2002-02: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/* static function declarations */
static int optScrollValue ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int optOnChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

/***v* scrolledWindow/windowOptions
 * AUTHOR
 *  WJG
 * SOURCE
 */
static GnoclOption windowOptions[] =
{
	{ "-child", GNOCL_STRING, NULL },
	{ "-scrollbar", GNOCL_OBJ, NULL },
	{ "-borderWidth", GNOCL_OBJ, "border-width", gnoclOptPadding },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ "-onChanged", GNOCL_OBJ, "", optOnChanged },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-xLower", GNOCL_OBJ, "", optScrollValue },
	{ "-xPageSize", GNOCL_OBJ, "", optScrollValue },
	{ "-xUpper", GNOCL_OBJ, "", optScrollValue },
	{ "-xValue", GNOCL_OBJ, "", optScrollValue },
	{ "-yLower", GNOCL_OBJ, "", optScrollValue },
	{ "-yPageSize", GNOCL_OBJ, "", optScrollValue },
	{ "-yUpper", GNOCL_OBJ, "", optScrollValue },
	{ "-yValue", GNOCL_OBJ, "", optScrollValue },
	{ NULL }
};
/*****/

/***d* scrolledWindow/constants
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int childIdx     = 0;
static const int scrollbarIdx = 1;
static const int xValueIdx    = 2;
static const int yValueIdx    = 3;
/****/

/***f* scrolledWindow/staticFuncs/doOnChanged
 * AUTHOR
 *  PGB
 * NOTES
 *  This is function is not cast as a static, but the naming covention
 *  implies that it is. Perhaps some clarificatoin is required.
 *  WJG 12/06/08
 * SOURCE
 */
void doOnChanged ( GtkAdjustment *adj, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( GTK_WIDGET ( cs->data ) );
	/* doOnChanged is called before gnoclRegisterWidget, why? */

	if ( ps[0].val.str == NULL ) return;

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}

/****/

/***f* scrolledWindow/staticFuncs/optOnChanged
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int optOnChanged ( Tcl_Interp *interp, GnoclOption *opt,
						  GObject *obj, Tcl_Obj **ret )
{
	GtkAdjustment *adj = gtk_scrolled_window_get_hadjustment (
							 GTK_SCROLLED_WINDOW ( obj ) );
	/* "changed" is emitted for horizontal, even if
	   only vertical is changed. Is this a GTK bug? */
	gnoclConnectOptCmd ( interp, G_OBJECT ( adj ), "changed",
						 G_CALLBACK ( doOnChanged ), opt, obj, ret );
	gnoclConnectOptCmd ( interp, G_OBJECT ( adj ), "value-changed",
						 G_CALLBACK ( doOnChanged ), opt, obj, ret );
	adj = gtk_scrolled_window_get_vadjustment (
			  GTK_SCROLLED_WINDOW ( obj ) );
	gnoclConnectOptCmd ( interp, G_OBJECT ( adj ), "value-changed",
						 G_CALLBACK ( doOnChanged ), opt, obj, ret );
	return TCL_OK;
}

/****/

/***f* scrolledWindow/staticFuncs/optScrollValue
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int optScrollValue ( Tcl_Interp *interp, GnoclOption *opt,
							GObject *obj, Tcl_Obj **ret )
{
	GtkAdjustment *adj;

	if ( opt->optName[1] == 'x' )
		adj = gtk_scrolled_window_get_hadjustment ( GTK_SCROLLED_WINDOW ( obj ) );
	else
		adj = gtk_scrolled_window_get_vadjustment ( GTK_SCROLLED_WINDOW ( obj ) );

	assert ( opt->optName[1] == 'x' || opt->optName[1] == 'y' );

	if ( ret == NULL ) /* set value */
	{
		if ( opt->optName[2] == 'V' )
		{
			int blocked;
			double val;

			if ( Tcl_GetDoubleFromObj ( interp, opt->val.obj, &val ) != TCL_OK )
				return TCL_ERROR;

			if ( val < adj->lower )
				val = adj->lower;
			else if ( val > adj->upper - adj->page_size )
				val = adj->upper - adj->page_size;

			blocked = g_signal_handlers_block_matched (
						  G_OBJECT ( adj ), G_SIGNAL_MATCH_FUNC,
						  0, 0, NULL, ( gpointer * ) doOnChanged, NULL );

			gtk_adjustment_set_value ( adj, val );

			if ( blocked )
				g_signal_handlers_unblock_matched (
					G_OBJECT ( adj ), G_SIGNAL_MATCH_FUNC,
					0, 0, NULL, ( gpointer * ) doOnChanged, NULL );

		}

		else
		{
			Tcl_AppendResult ( interp, "option \"", opt->optName,
							   "\" is read only.", NULL );
			return TCL_ERROR;
		}
	}

	else /* get value */
	{
		switch ( opt->optName[2] )
		{
			case 'L': *ret = Tcl_NewDoubleObj ( adj->lower ); break;
			case 'P': *ret = Tcl_NewDoubleObj ( adj->page_size ); break;
			case 'U': *ret = Tcl_NewDoubleObj ( adj->upper ); break;
			case 'V': *ret = Tcl_NewDoubleObj ( adj->value ); break;
			default:  assert ( 0 );
		}
	}

	return TCL_OK;
}

/****/

/***f* scrolledWindow/staticFuncs/configure
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkScrolledWindow *window,
					   GnoclOption options[] )
{
	if ( options[childIdx].status == GNOCL_STATUS_CHANGED )
	{
		GtkWidget *childWidget = gnoclChildNotPacked (
									 options[childIdx].val.str, interp );

		if ( childWidget == NULL )
			return TCL_ERROR;

		if ( gtk_bin_get_child ( GTK_BIN ( window ) ) )
			g_object_unref ( gtk_bin_get_child ( GTK_BIN ( window ) ) );

		if ( GTK_IS_LAYOUT ( childWidget ) )
		{
			gtk_container_add ( GTK_CONTAINER ( window ), childWidget );
			gtk_scrolled_window_set_hadjustment ( window,
												  gtk_layout_get_hadjustment ( GTK_LAYOUT ( childWidget ) ) );
			gtk_scrolled_window_set_vadjustment ( window,
												  gtk_layout_get_vadjustment ( GTK_LAYOUT ( childWidget ) ) );
		}

		else
			gtk_scrolled_window_add_with_viewport ( window, childWidget );
	}

	if ( options[scrollbarIdx].status == GNOCL_STATUS_CHANGED )
	{
		GtkPolicyType hor, vert;

		if ( gnoclGetScrollbarPolicy ( interp, options[scrollbarIdx].val.obj,
									   &hor, &vert ) != TCL_OK )
			return TCL_ERROR;

		gtk_scrolled_window_set_policy ( window, hor, vert );
	}

	return TCL_OK;
}

/****/

/***f* scrolledWindow/staticFuncs/cget
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, GtkScrolledWindow *window,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == childIdx )
	{
		GtkWidget *child = gtk_bin_get_child ( GTK_BIN ( window ) );

		if ( child )
			obj = Tcl_NewStringObj ( gnoclGetNameFromWidget ( child ), -1 );
		else
			obj = Tcl_NewStringObj ( "", 0 );
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/****/

/***f* scrolledWindow/staticFuncs/windowFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
int scrlWindowFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ClassIdx };
	GtkScrolledWindow *window = GTK_SCROLLED_WINDOW ( data );
	int idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "scrolledWindow", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( window ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   windowOptions, G_OBJECT ( window ) ) == TCL_OK )
				{
					ret = configure ( interp, window, windowOptions );
				}

				gnoclClearOptions ( windowOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( window ),
									 windowOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, window, windowOptions, idx );
				}
			}

	}

	return TCL_OK;
}

/****/

/***f* scrolledWindow/staticFuncs/gnoclScrolledWindowCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclScrolledWindowCmd ( ClientData data, Tcl_Interp *interp,
							 int objc, Tcl_Obj * const objv[] )
{
	int               ret;
	GtkScrolledWindow *window;

	if ( gnoclParseOptions ( interp, objc, objv, windowOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( windowOptions );
		return TCL_ERROR;
	}

	window = GTK_SCROLLED_WINDOW ( gtk_scrolled_window_new ( NULL, NULL ) );

	gtk_scrolled_window_set_policy ( window, GTK_POLICY_AUTOMATIC,
									 GTK_POLICY_AUTOMATIC );
	gtk_widget_show ( GTK_WIDGET ( window ) );

	/* set default value */
	gtk_container_set_border_width ( GTK_CONTAINER ( window ), GNOCL_PAD );

	ret = gnoclSetOptions ( interp, windowOptions, G_OBJECT ( window ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, window, windowOptions );

	gnoclClearOptions ( windowOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( window ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( window ), scrlWindowFunc );
}

/****/
