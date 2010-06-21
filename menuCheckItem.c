/**
 \brief
  This module implements the gnocl::menuCheckItem widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page30 gnocl::menuCheckItem
  \section sec gnocl::menuCheckItem
  Implementation of gnocl::menuCheckItem
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
  \image html "../pics/menuCheckItem.png"
*/

/*
 * $Id: menuCheckItem.c,v 1.7 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the checkItem widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2004-02: added -data
        09: added cget
            removed getValue and setValue
   2003-01: unification with checkButton
        10: switched from GnoclWidgetOptions to GnoclOption
            split from menuItem
        05: updates for gtk 2.0
        02: added "invoke" to label and checkButton
   2002-01: bugfixes concerning -command and -variable handling
        12: added pixmap_menu_item
        11: added STD_OPTIONS
        10: check item
        09: underlined accelerators
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****v* menuCheckItem/checkOptions
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GnoclOption checkOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },           /* 0 */
	{ "-accelerator", GNOCL_OBJ, NULL },    /* 1 */
	{ "-variable", GNOCL_STRING, NULL },    /* 2 */
	{ "-onValue", GNOCL_OBJ, NULL },        /* 3 */
	{ "-offValue", GNOCL_OBJ, NULL },       /* 4 */
	{ "-onToggled", GNOCL_STRING, NULL },   /* 5 */
	{ "-active", GNOCL_BOOL, NULL },        /* 6 */
	{ "-value", GNOCL_OBJ, NULL },          /* 7 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-inconsistent", GNOCL_BOOL, "inconsistent" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};
/*****/

/****v* menuCheckItem/constants
 * AUTHOR
 *	PGB
 * SOURCE
 */
static const int textIdx      = 0;
static const int accelIdx     = 1;
static const int variableIdx  = 2;
static const int onValueIdx   = 3;
static const int offValueIdx  = 4;
static const int onToggledIdx = 5;
static const int activeIdx    = 6;
static const int valueIdx     = 7;
/*****/

/****f* menuCheckItem/staticFuncs/checkOptions
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp,
					   GnoclCheckParams *para, GnoclOption options[] )
{
	if ( options[onValueIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_OBJ ( options[onValueIdx].val.obj, para->onValue );

	if ( options[offValueIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_OBJ ( options[offValueIdx].val.obj, para->offValue );

	gnoclAttacheOptCmdAndVar (
		&options[onToggledIdx], &para->onToggled,
		&options[variableIdx], &para->variable,
		"toggled", G_OBJECT ( para->widget ),
		G_CALLBACK ( gnoclCheckToggledFunc ),
		interp, gnoclCheckTraceFunc, para );

	if ( options[textIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclMenuItemHandleText ( interp, GTK_MENU_ITEM ( para->widget ),
									   options[textIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( options[accelIdx].status == GNOCL_STATUS_CHANGED )
		gnoclMenuItemHandleAccel ( interp, GTK_MENU_ITEM ( para->widget ),
								   options[accelIdx].val.obj );

	if ( gnoclCheckSetActive ( para, &options[activeIdx] ) == 0
			&& ( options[variableIdx].status == GNOCL_STATUS_CHANGED
				 || options[onValueIdx].status == GNOCL_STATUS_CHANGED
				 || options[offValueIdx].status == GNOCL_STATUS_CHANGED ) )
	{
		gnoclCheckVariableValueChanged ( para );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclCheckSetValue ( para, options[valueIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/*****/

/****f* menuCheckItem/staticFuncs/cget
 * AUTHOR
 *	PGM
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, GnoclCheckParams  *para,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
		obj = gnoclCgetMenuItemText ( interp, GTK_MENU_ITEM ( para->widget ) );
	else if ( idx == accelIdx )
		obj = gnoclCgetMenuItemAccel ( interp, GTK_MENU_ITEM ( para->widget ) );
	else if ( idx == onToggledIdx )
		obj = Tcl_NewStringObj ( para->onToggled ? para->onToggled : "", -1 );
	else if ( idx == variableIdx )
		obj = Tcl_NewStringObj ( para->variable, -1 );
	else if ( idx == onValueIdx )
		obj = para->onValue;
	else if ( idx == offValueIdx )
		obj = para->offValue;
	else if ( idx == activeIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
		obj = Tcl_NewBooleanObj ( on );
	}

	else if ( idx == valueIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
		obj = on ? para->onValue : para->offValue;
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/****f* menuCheckItem/staticFuncs/checkItemFunc
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int checkItemFunc ( ClientData data, Tcl_Interp *interp,
						   int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget",
								  "onToggled", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx,
				  OnToggledIdx
				};

	GnoclCheckParams *para = ( GnoclCheckParams * ) data;
	int             idx;

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
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   checkOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, checkOptions );
				}

				gnoclClearOptions ( checkOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 checkOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, checkOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclCheckOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/*****/

/****f* menuCheckItem/gnoclMenuCheckItemCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclMenuCheckItemCmd ( ClientData data, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[] )
{
	GnoclCheckParams *para;
	int             ret;

	if ( gnoclParseOptions ( interp, objc, objv, checkOptions ) != TCL_OK )
	{
		gnoclClearOptions ( checkOptions );
		return TCL_ERROR;
	}

	para = g_new ( GnoclCheckParams, 1 );

	para->name = gnoclGetAutoWidgetId();
	para->interp = interp;
	para->widget = gtk_check_menu_item_new_with_mnemonic ( "" );
	para->onToggled = NULL;
	para->variable = NULL;
	para->onValue = Tcl_NewIntObj ( 1 );
	Tcl_IncrRefCount ( para->onValue );
	para->offValue = Tcl_NewIntObj ( 0 );
	Tcl_IncrRefCount ( para->offValue );
	para->inSetVar = 0;
	gtk_widget_show ( para->widget );

	ret = gnoclSetOptions ( interp, checkOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, checkOptions );

	gnoclClearOptions ( checkOptions );

	if ( ret != TCL_OK )
	{
		Tcl_DecrRefCount ( para->onValue );
		Tcl_DecrRefCount ( para->offValue );
		g_free ( para );
		gtk_widget_destroy ( para->widget );
		return TCL_ERROR;
	}

	g_signal_connect ( G_OBJECT ( para->widget ), "destroy",

					   G_CALLBACK ( gnoclCheckDestroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, para->widget );

	Tcl_CreateObjCommand ( interp, para->name, checkItemFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/*****/
