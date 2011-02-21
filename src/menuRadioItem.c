/**
 \brief
  This module implements the gnocl::menuRadioItem widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page32 gnocl::menuRadioItem
  \section sec gnocl::menuRadioItem
  Implementation of gnocl::menuRadioItem
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
  \image html "../pics/menuRadioItem.png"
*/

/****h* menu/menuRadioItem
 * NAME
 * 	menuRadioItem.c
 * SYNOPSIS
 * 	This file implements the menu radio item widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/menuRadioItem.png">
 * SEE ALSO
 *****/

/*
 * $Id: menuRadioItem.c,v 1.7 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the menu radio item widget
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
   2004-02: added -data
        09: renamed -value to -onValue
            added cget
            removed getValue and setValue
   2003-01: unification with radioButton
   2002-10: Begin of developement
 */


#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****v* menuRadioItem/radioOptions
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GnoclOption radioOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },           /* 0 */
	{ "-onToggled", GNOCL_STRING, NULL },   /* 1 */
	{ "-variable", GNOCL_STRING, NULL },    /* 2 */
	{ "-onValue", GNOCL_OBJ, NULL },        /* 3 */
	{ "-active", GNOCL_BOOL, NULL },        /* 4 */
	{ "-accelerator", GNOCL_OBJ, NULL },    /* 5 */
	{ "-value", GNOCL_OBJ, NULL },          /* 6 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-inconsistent", GNOCL_BOOL, "inconsistent" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};
/*****/

/****v* menuRadioItem/constants
 * AUTHOR
 *	PGB
 * SOURCE
 */
static const int textIdx      = 0;
static const int onToggledIdx = 1;
static const int variableIdx  = 2;
static const int onValueIdx   = 3;
static const int activeIdx    = 4;
static const int accelIdx     = 5;
static const int valueIdx     = 6;
/*****/

/****v* menuRadioItem/staticFuncs/configure
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp,
					   GnoclRadioParams *para, GnoclOption options[] )
{
	if ( options[textIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclMenuItemHandleText ( interp, GTK_MENU_ITEM ( para->widget ),
									   options[textIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( options[accelIdx].status == GNOCL_STATUS_CHANGED )
		gnoclMenuItemHandleAccel ( interp, GTK_MENU_ITEM ( para->widget ),
								   options[accelIdx].val.obj );

	if ( options[onToggledIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_STRING ( options[onToggledIdx].val.str, para->onToggled );

	if ( gnoclRadioSetValueActive ( para, &options[onValueIdx],
									&options[activeIdx] ) != TCL_OK )
		return TCL_ERROR;

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclRadioSetValue ( para, options[valueIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/*****/

/****v* menuRadioItem/staticFuncs/cget
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, GnoclRadioParams  *para,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
		obj = gnoclCgetMenuItemText ( interp, GTK_MENU_ITEM ( para->widget ) );
	else if ( idx == onToggledIdx )
		obj = Tcl_NewStringObj ( para->onToggled ? para->onToggled : "", -1 );
	else if ( idx == variableIdx )
		obj = Tcl_NewStringObj ( para->group->variable, -1 );
	else if ( idx == onValueIdx )
	{
		GnoclRadioParams *p = gnoclRadioGetActivePara ( para->group );
		obj = p->onValue;
	}

	else if ( idx == activeIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
		obj = Tcl_NewBooleanObj ( on );
	}

	else if ( idx == accelIdx )
		obj = gnoclCgetMenuItemAccel ( interp, GTK_MENU_ITEM ( para->widget ) );
	else if ( idx == valueIdx )
		obj = gnoclRadioGetValue ( para );

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/****v* menuRadioItem/staticFuncs/radioItemFunc
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int radioItemFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onToggled", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnToggledIdx, ClassIdx, };

	GnoclRadioParams *para = ( GnoclRadioParams * ) data;
	int             idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "menuRadioItem", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, radioOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, radioOptions );
				}

				gnoclClearOptions ( radioOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 radioOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, radioOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclRadioOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/*****/

/****v* menuRadioItem/gnoclMenuRadioItemCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclMenuRadioItemCmd ( ClientData data, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[] )
{
	GnoclRadioParams *para;
	int                ret;

	if ( gnoclParseOptions ( interp, objc, objv, radioOptions ) != TCL_OK )
	{
		gnoclClearOptions ( radioOptions );
		return TCL_ERROR;
	}

	if ( radioOptions[onValueIdx].status != GNOCL_STATUS_CHANGED
			|| radioOptions[variableIdx].status != GNOCL_STATUS_CHANGED )
	{
		gnoclClearOptions ( radioOptions );
		Tcl_SetResult ( interp,
						"Option \"-onValue\" and \"-variable\" are required.", TCL_STATIC );
		return TCL_ERROR;
	}

	para = g_new ( GnoclRadioParams, 1 );

	para->name = gnoclGetAutoWidgetId();
	para->widget = gtk_radio_menu_item_new_with_mnemonic ( NULL, "" );
	para->onToggled = NULL;
	para->onValue = NULL;
	gtk_widget_show ( para->widget );

	para->group = gnoclRadioGetGroupFromVariable (
					  radioOptions[variableIdx].val.str );

	if ( para->group == NULL )
	{
		para->group = gnoclRadioGroupNewGroup (
						  radioOptions[variableIdx].val.str, interp );
	}

	else
	{
		GnoclRadioParams *p = gnoclRadioGetParam ( para->group, 0 );
		gtk_radio_menu_item_set_group ( GTK_RADIO_MENU_ITEM ( para->widget ),
										gtk_radio_menu_item_get_group ( GTK_RADIO_MENU_ITEM ( p->widget ) ) );
	}

	gnoclRadioGroupAddWidgetToGroup ( para->group, para );

	ret = gnoclSetOptions ( interp, radioOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, radioOptions );

	gnoclClearOptions ( radioOptions );

	if ( ret != TCL_OK )
	{
		gnoclRadioRemoveWidgetFromGroup ( para->group, para );
		g_free ( para->name );
		g_free ( para );
		gtk_widget_destroy ( para->widget );
		return TCL_ERROR;
	}

	g_signal_connect ( G_OBJECT ( para->widget ), "destroy",

					   G_CALLBACK ( gnoclRadioDestroyFunc ), para );

	g_signal_connect ( G_OBJECT ( para->widget ), "toggled",
					   G_CALLBACK ( gnoclRadioToggledFunc ), para );

	gnoclMemNameAndWidget ( para->name, para->widget );

	Tcl_CreateObjCommand ( interp, para->name, radioItemFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/*****/
