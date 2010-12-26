/**
\brief      This module implements the gnocl::toggleButton widget.
\authors    Peter G. Baum, William J Giddings
\date       2001-03:
\bug        There are problems with the 'configure' command.
**/

/* user documentation */

/**
  \page page47 gnocl::toggleButton
  \section sec gnocl::toggleButton
  Implementation of gnocl::toggleButton
  \subsection subsection1 Implemented Options
  \verbatim
    -active
        type: boolean (default: 0)
        Whether the widget is active.

    -activeBackgroundColor
        type: color
        Background color. DISCLAIMER_COLOR

    -data
        type: string
        User defined data which can be retrieved via the cget subcommand.

    -hasFocus
        type: 1
        This sets the focus to the widget. To unset the focus it must be set to another widet.

    -indicatorOn
        type: boolean (default: true)
        Determines whether or not the toggle button is drawn on screen

    -name
        type: string
        Name of the widget, can be used to set options in an rc file.

    -normalBackgroundColor
        type: color
        Background color. DISCLAIMER_COLOR

    -offValue
        type: string (default: 0)
        Value to store in the button's associated variable whenever this button is not selected

    -onButtonPress
        type: string (default: "")
        Tcl command which is executed if a mouse button is press inside the widget.
        Before evaluation the following percent strings are substituated:
            %w  widget name
            %t  type of event: one of buttonPress, button2Press or button3Press
            %x  x coordinate
            %y  y coordinate
            %b  button number
            %s  state of the buttons and modifiers (bitmask)

    -onButtonRelease
        type: string (default: "")
        Tcl command which is executed if a mouse button is released inside the widget.
        Before evaluation the following percent strings are substituated:
            %w  widget name
            %t  type of event: always buttonRelease
            %x  x coordinate
            %y  y coordinate
            %b  button number
            %s  state of the buttons and modifiers (bitmask)

    -onPopupMenu
        type: string (default: "")
        Tcl command which is executed if the "popup-menu" signal is recieved,
        which is normally the case if the user presses Shift-F10.
        Before evaluation the following percent strings are substituated:
            %w  widget name.

    -onShowHelp
        type: string (default: "")
        Tcl command which is executed in the global scope if the "show-help"
        signal is recieved, which is normally the case if the user presses F1 or Ctrl-F1.
        Before evaluation the following percent strings are substituated
            %w  widget name
            %h | help type: either "whatsThis" or "tooltip"

    -onToggled
        type: string (default: "")
        Tcl command which is executed if the radio button is toggled.
        Before evaluation the following percent strings are substituted:
            %w  widget name
            %v  onValue or offValue depending of the state

    -onValue
        type: string (default: 1)
        Value to store in the button's associated variable whenever this button is selected

    -prelightBackgroundColor
        type: color
        Background color. DISCLAIMER_COLOR

    -relief
        type: ONEOF normal, half, none (default: normal)
        Relief of the button.

    -sensitive
        type: boolean (default: 1)
        Whether or not the item is sensitve to user input.

    -text
        type: percent-string (default: "")
        Text or icon of the button

    -tooltip
        type: string
        Message that appear next to this widget when the mouse pointer is
        held over it for a short amount of time.

    -variable: string
        type: 0
        Name of a (global) variable which is changed when the state of the button changes.

    -visible
        type: boolean (default: 1)
        Whether or not the item is visible.
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
  \image html "../pics/toggleButton.png"



/* FUNCTION
 *  This file implements the toggleButton widget.
 *
 * NOTES
 *  This module is modelled after the checkButton.
 *
 * BUGS
 *
 * HISTORY
 *  *  2009-02: added -widthReuest, -heightRequest
 *  Added 20/06/08
 *
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/toggleButton.png">
 *
 * SEE ALSO
 *
 *****/

/****v* toggleButton/arrays
 * AUTHOR
 *  WJG
 * SOURCE
 ******/

/****f* toggleButton/functions
 * AUTHOR
 *  WJG
 * SOURCE
 ******/

/*
 * $Id: toggleButton.c,v 1.12 2005/02/25 21:33:38 baum Exp $
 *
 * This file implements the toggleButton widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-12: added %g option
   2009-01: added command, geometry
   2008-10: added command, class
   2008-06: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
\brief
\author Willliam J Giddings
\date
\note
**/
static GnoclOption toggleButtonOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },       /* 0 */
	{ "-onToggled", GNOCL_STRING },      /* 1 */
	{ "-variable", GNOCL_STRING },       /* 2 */
	{ "-onValue", GNOCL_OBJ, NULL },    /* 3 */
	{ "-offValue", GNOCL_OBJ, NULL },   /* 4 */
	{ "-active", GNOCL_BOOL, NULL },    /* 5 */
	{ "-value", GNOCL_OBJ, NULL },      /* 6 */
	{ "-icon", GNOCL_OBJ, NULL },       /* 7 */
	{ "-indicatorOn", GNOCL_BOOL, "draw-indicator" },
	{ "-activeBackgroundColor", GNOCL_OBJ, "active", gnoclOptGdkColorBg },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-inconsistent", GNOCL_BOOL, "inconsistent" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-normalBackgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-prelightBackgroundColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorBg },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },

	/* inherited GtkWidget properties */
	{ "-heightRequest", GNOCL_INT, "height-request" },
	{ "-widthRequest", GNOCL_INT, "width-request" },

	/*

	   { "-normalForegroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorFg },
	   { "-activeForegroundColor", GNOCL_OBJ, "active", gnoclOptGdkColorFg },
	   { "-prelightForegroundColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorFg },

	   { "-normalTextColor", GNOCL_OBJ, "normal", gnoclOptGdkColorText },
	   { "-activeTextColor", GNOCL_OBJ, "active", gnoclOptGdkColorText },
	   { "-prelightTextColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorText },
	*/
	{ NULL },
};
/*****/

/****v* toggleButton/staticsVars
 * AUTHOR
 *  WJG
 * SOURCE
 */
static const int textIdx      = 0;
static const int onToggledIdx = 1;
static const int variableIdx  = 2;
static const int onValueIdx   = 3;
static const int offValueIdx  = 4;
static const int activeIdx    = 5;
static const int valueIdx     = 6;
static const int iconIdx      = 7;

/**
\brief
\author William J Giddings
\date
\note
**/
static int toggleDoCommand ( GnoclToggleParams *para, Tcl_Obj *val, int bg )
{
	if ( para->onToggled )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_OBJ },     /* value */
			{ 'g', GNOCL_STRING },  /* glade name */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.obj = val;
		ps[2].val.str = gtk_widget_get_name ( para->widget );

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onToggled, bg );
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
static int toggleIsOn ( Tcl_Interp *interp, Tcl_Obj *onValue, Tcl_Obj *offValue, Tcl_Obj *val )
{
	const char *strVal = Tcl_GetString ( val );

	if ( strcmp ( Tcl_GetString ( onValue ), strVal ) == 0 )
	{
		return 1;
	}

	if ( strcmp ( Tcl_GetString ( offValue ), strVal ) == 0 )
	{
		return 0;
	}

	if ( interp )
	{
		Tcl_AppendResult ( interp, "Invalid value \"", strVal,
						   "\", must be either \"", Tcl_GetString ( onValue ),
						   "\" or \"", Tcl_GetString ( offValue ), "\".", NULL );
	}

	return -1;
}

/**
\brief
\author William J Giddings
\date
\note
**/
static int toggleSetVariable ( GnoclToggleParams *para, Tcl_Obj *val )
{
	if ( para->variable && para->inSetVar == 0 )
	{
		Tcl_Obj *ret;
		para->inSetVar = 1;
		ret = Tcl_SetVar2Ex ( para->interp, para->variable, NULL,
							  val, TCL_GLOBAL_ONLY );
		para->inSetVar = 0;
		return ret == NULL ? TCL_ERROR : TCL_OK;
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
static int toggleSetState ( GnoclToggleParams *para, int on )
{
	int blocked = g_signal_handlers_block_matched ( G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) gnoclToggleToggledFunc, NULL );
	/* g_object_set works for both
	gtk_toggle_button_set_active( para->widget, on );
	gtk_check_menu_item_set_active( para->widget, on );
	*/
	g_object_set ( G_OBJECT ( para->widget ), "active", ( gboolean ) on, NULL );

	if ( blocked )
	{
		g_signal_handlers_unblock_matched ( G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) gnoclToggleToggledFunc, NULL );
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
int gnoclToggleSetValue ( GnoclToggleParams *para, Tcl_Obj *obj )
{
	int on = toggleIsOn ( para->interp, para->onValue, para->offValue, obj );

	if ( on >= 0 )
	{
		Tcl_Obj *valObj = on ? para->onValue : para->offValue;
		toggleSetState ( para, on );
		toggleSetVariable ( para, valObj );
		return TCL_OK;
	}

	return TCL_ERROR;
}

/**
\brief
\author William J Giddings
\date
\note
**/
int gnoclToggleToggle ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[], GnoclToggleParams *para )
{
	int on;
	Tcl_Obj *valObj;

	if ( objc != 2 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, NULL );
		return TCL_ERROR;
	}

	g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );

	valObj = on ? para->onValue : para->offValue;
	return toggleDoCommand ( para, valObj, 0 );
}

/**
\brief
\author William J Giddings
\date
\note
**/
void gnoclToggleDestroyFunc ( GtkWidget *widget, gpointer data )
{
	GnoclToggleParams *para = ( GnoclToggleParams * ) data;

	/* untrace and free command and variable */
	gnoclAttacheOptCmdAndVar (
		NULL, &para->onToggled,
		NULL, &para->variable,
		"toggled", G_OBJECT ( para->widget ),
		G_CALLBACK ( gnoclToggleToggledFunc ),
		para->interp, gnoclToggleTraceFunc, para );

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );
	g_free ( para->name );
	g_free ( para->onToggled );
	g_free ( para->variable );
	Tcl_DecrRefCount ( para->onValue );
	Tcl_DecrRefCount ( para->offValue );
	g_free ( para );
}

/**
\brief
\author William J Giddings
\date
\note
**/
void gnoclToggleToggledFunc ( GtkWidget *widget, gpointer data )
{
	gboolean on;
	Tcl_Obj *valObj;
	GnoclToggleParams *para = ( GnoclToggleParams * ) data;

	g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
	valObj = on ? para->onValue : para->offValue;
	toggleSetVariable ( para, valObj );
	toggleDoCommand ( para, valObj, 1 );
}

/**
\brief
\author William J Giddings
\date
\note
**/
char *gnoclToggleTraceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	GnoclToggleParams *para = ( GnoclToggleParams * ) data;

	if ( para->inSetVar == 0 )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );

		if ( val )
		{
			int on = toggleIsOn ( para->interp, para->onValue, para->offValue, val );

			if ( on >= 0 )
			{
				toggleSetState ( para, on );
			}
		}
	}

	return NULL;
}

/**
\brief
\author William J Giddings
\date
\note
**/
int gnoclToggleSetActive ( GnoclToggleParams *para, GnoclOption *active )
{
	if ( active->status == GNOCL_STATUS_CHANGED )
	{
		int on = active->val.b;
		Tcl_Obj *valObj = on ? para->onValue : para->offValue;
		toggleSetState ( para, on );
		toggleSetVariable ( para, valObj );
		return 1;
	}

	return 0;
}

/**
\brief
\author William J Giddings
\date
\note
**/
int gnoclToggleVariableValueChanged ( GnoclToggleParams *para )
{
	if ( para->variable != NULL )
	{
		/* if variable does not exist -> set it, else set widget state */
		Tcl_Obj *var = Tcl_GetVar2Ex ( para->interp, para->variable, NULL,
									   TCL_GLOBAL_ONLY );

		if ( var == NULL )
		{
			gboolean on;
			Tcl_Obj *valObj;

			g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
			valObj = on ? para->onValue : para->offValue;
			toggleSetVariable ( para, valObj );
		}

		else
		{
			int on = toggleIsOn ( para->interp, para->onValue,
								  para->offValue, var );

			if ( on >= 0 )
				toggleSetState ( para, on );
			else
				return TCL_ERROR;
		}
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
static int configure ( Tcl_Interp *interp, GnoclToggleParams *para, GnoclOption options[] )
{
	if ( options[textIdx].status == GNOCL_STATUS_CHANGED && gnoclConfigButtonText ( interp, GTK_BUTTON ( para->widget ), options[textIdx].val.obj ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	if ( options[onValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		GNOCL_MOVE_OBJ ( options[onValueIdx].val.obj, para->onValue );
	}

	if ( options[offValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		GNOCL_MOVE_OBJ ( options[offValueIdx].val.obj, para->offValue );
	}

	gnoclAttacheOptCmdAndVar (

		&options[onToggledIdx], &para->onToggled,
		&options[variableIdx], &para->variable,
		"toggled", G_OBJECT ( para->widget ),
		GTK_SIGNAL_FUNC ( gnoclToggleToggledFunc ),
		interp, gnoclToggleTraceFunc, para );

	if ( gnoclToggleSetActive ( para, &options[activeIdx] ) == 0
			&& ( options[variableIdx].status == GNOCL_STATUS_CHANGED
				 || options[onValueIdx].status == GNOCL_STATUS_CHANGED
				 || options[offValueIdx].status == GNOCL_STATUS_CHANGED ) )
	{
		gnoclToggleVariableValueChanged ( para );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclToggleSetValue ( para, options[valueIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
static int cget ( Tcl_Interp *interp, GnoclToggleParams *para, GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
	{
		obj = gnoclCgetButtonText ( interp, GTK_BUTTON ( para->widget ) );
	}

	else if ( idx == onToggledIdx )
	{
		obj = Tcl_NewStringObj ( para->onToggled ? para->onToggled : "", -1 );
	}

	else if ( idx == variableIdx )
	{
		obj = Tcl_NewStringObj ( para->variable, -1 );
	}

	else if ( idx == onValueIdx )
	{
		obj = para->onValue;
	}

	else if ( idx == offValueIdx )
	{
		obj = para->offValue;
	}

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

/**
\brief
\author William J Giddings
\date
\note
**/
int toggleButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "toggle", "class", "geometry", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ToggleIdx, ClassIdx, GeometryIdx };
	GnoclToggleParams *para = ( GnoclToggleParams * ) data;

	int idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case GeometryIdx:
			{
				g_print ( "toggleButton GeometryIdx\n" );
				char *txt = gnoclGetWidgetGeometry ( para->widget ) ;
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( txt , -1 ) );
				return TCL_OK;
			}

			break;
		case ClassIdx:
			{
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "toggleButton", -1 ) );
				break;
			}

		case DeleteIdx:
			{
				return gnoclDelete ( interp, para->widget, objc, objv );
			}

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, toggleButtonOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, toggleButtonOptions );
				}

				gnoclClearOptions ( toggleButtonOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ), toggleButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, toggleButtonOptions, idx );
				}
			}

		case ToggleIdx:
			return gnoclToggleToggle ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/**
\brief
\author William J Giddings
\date
\note
**/
int gnoclToggleButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	GnoclToggleParams *para;
	int ret;

	if ( gnoclParseOptions ( interp, objc, objv, toggleButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( toggleButtonOptions );
		return TCL_ERROR;
	}

	para = g_new ( GnoclToggleParams, 1 );

	para->interp = interp;
	para->name = gnoclGetAutoWidgetId();
	para->widget = gtk_toggle_button_new_with_label ( "" );
	para->onToggled = NULL;
	para->variable = NULL;
	para->onValue = Tcl_NewIntObj ( 1 );
	Tcl_IncrRefCount ( para->onValue );
	para->offValue = Tcl_NewIntObj ( 0 );
	Tcl_IncrRefCount ( para->offValue );
	para->inSetVar = 0;

	gtk_widget_show ( para->widget );

	ret = gnoclSetOptions ( interp, toggleButtonOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, para, toggleButtonOptions );
	}

	gnoclClearOptions ( toggleButtonOptions );

	if ( ret != TCL_OK )
	{
		Tcl_DecrRefCount ( para->onValue );
		Tcl_DecrRefCount ( para->offValue );
		g_free ( para );
		gtk_widget_destroy ( para->widget );
		return TCL_ERROR;
	}

	g_signal_connect ( G_OBJECT ( para->widget ), "destroy", G_CALLBACK ( gnoclToggleDestroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, para->widget );

	Tcl_CreateObjCommand ( interp, para->name, toggleButtonFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

