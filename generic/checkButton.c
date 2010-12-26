/**
 \brief
  This module implements the gnocl::checkButton widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/** \page page8 gnocl::checkButton
  \section sec gnocl::checkButton
  Implementation of...
  \subsection subsection1 Implemented gnocl::checkButton Options
  \subsection subsection2 Implemented gnocl::checkButton Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/checkButton.png"
*/



/****h* widget/checkButton
 * NAME
 *  checkButton.c
 * SYNOPSIS
 *  This file implements the checkButton widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/checkButton.png">
 * SEE ALSO
 *****/

/****v* checkButton/arrays
* AUTHOR
*   PGB
* SOURCE
******/

/****f* checkButton/functions
 * AUTHOR
 *  PGB
 * SOURCE
 ******/

/*
 * $Id: checkButton.c,v 1.12 2005/02/25 21:33:38 baum Exp $
 *
 * This file implements the checkButton widget
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
        09: added cget
   2003-01: unification with menuCheckItem
        08: switched from GnoclWidgetOptions to GnoclOption
            changed -command to -onToggled
   2002-01: new command "invoke"
        10: added getValue, setValue
        09: underlined accelerators
        07: percent substitution in command
            cleanups
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>



/****v* /checkButton/checkButtonOptions
* AUTHOR
*   PGB
* SOURCE
*/
static GnoclOption checkButtonOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },        /* 0 */
	{ "-onToggled", GNOCL_STRING },      /* 1 */
	{ "-variable", GNOCL_STRING },       /* 2 */
	{ "-onValue", GNOCL_OBJ, NULL },     /* 3 */
	{ "-offValue", GNOCL_OBJ, NULL },    /* 4 */
	{ "-active", GNOCL_BOOL, NULL },     /* 5 */
	{ "-value", GNOCL_OBJ, NULL },       /* 6 */
	{ "-activeBackgroundColor", GNOCL_OBJ, "active", gnoclOptGdkColorBg },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-inconsistent", GNOCL_BOOL, "inconsistent" },
	{ "-indicatorOn", GNOCL_BOOL, "draw-indicator" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-normalBackgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-prelightBackgroundColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorBg },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
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

/****v* checkButton/staticsVars
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int textIdx      = 0;
static const int onToggledIdx = 1;
static const int variableIdx  = 2;
static const int onValueIdx   = 3;
static const int offValueIdx  = 4;
static const int activeIdx    = 5;
static const int valueIdx     = 6;

/**
\brief
\author Peter G Baum
\date
\note
**/
static int checkDoCommand ( GnoclCheckParams *para, Tcl_Obj *val, int bg )
{
	if ( para->onToggled )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_OBJ },     /* value */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.obj = val;

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onToggled, bg );
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
static int checkIsOn ( Tcl_Interp *interp, Tcl_Obj *onValue,
					   Tcl_Obj *offValue, Tcl_Obj *val )
{
	const char *strVal = Tcl_GetString ( val );

	if ( strcmp ( Tcl_GetString ( onValue ), strVal ) == 0 )
		return 1;

	if ( strcmp ( Tcl_GetString ( offValue ), strVal ) == 0 )
		return 0;

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
\author Peter G Baum
\date
\note
**/
static int checkSetVariable ( GnoclCheckParams *para, Tcl_Obj *val )
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
\author Peter G Baum
\date
\note
**/
static int checkSetState ( GnoclCheckParams *para, int on )
{
	int blocked = g_signal_handlers_block_matched (
					  G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( gpointer * ) gnoclCheckToggledFunc, NULL );
	/* g_object_set works for both
	gtk_toggle_button_set_active( para->widget, on );
	gtk_check_menu_item_set_active( para->widget, on );
	*/
	g_object_set ( G_OBJECT ( para->widget ), "active", ( gboolean ) on, NULL );

	if ( blocked )
		g_signal_handlers_unblock_matched (
			G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC,
			0, 0, NULL, ( gpointer * ) gnoclCheckToggledFunc, NULL );

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclCheckSetValue ( GnoclCheckParams *para, Tcl_Obj *obj )
{
	int on = checkIsOn ( para->interp, para->onValue, para->offValue, obj );

	if ( on >= 0 )
	{
		Tcl_Obj *valObj = on ? para->onValue : para->offValue;
		checkSetState ( para, on );
		checkSetVariable ( para, valObj );
		return TCL_OK;
	}

	return TCL_ERROR;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclCheckOnToggled ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[],
						  GnoclCheckParams *para )
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
	return checkDoCommand ( para, valObj, 0 );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
void gnoclCheckDestroyFunc ( GtkWidget *widget, gpointer data )
{
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;

	/* untrace and free command and variable */
	gnoclAttacheOptCmdAndVar (
		NULL, &para->onToggled,
		NULL, &para->variable,
		"toggled", G_OBJECT ( para->widget ),
		G_CALLBACK ( gnoclCheckToggledFunc ),
		para->interp, gnoclCheckTraceFunc, para );

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
\author Peter G Baum
\date
\note
**/
void gnoclCheckToggledFunc ( GtkWidget *widget, gpointer data )
{
	gboolean on;
	Tcl_Obj *valObj;
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;

	g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
	valObj = on ? para->onValue : para->offValue;
	checkSetVariable ( para, valObj );
	checkDoCommand ( para, valObj, 1 );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
char *gnoclCheckTraceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;

	if ( para->inSetVar == 0 )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );

		if ( val )
		{
			int on = checkIsOn ( para->interp, para->onValue,
								 para->offValue, val );

			if ( on >= 0 )
				checkSetState ( para, on );
		}
	}

	return NULL;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclCheckSetActive ( GnoclCheckParams *para, GnoclOption *active )
{
	if ( active->status == GNOCL_STATUS_CHANGED )
	{
		int on = active->val.b;
		Tcl_Obj *valObj = on ? para->onValue : para->offValue;
		checkSetState ( para, on );
		checkSetVariable ( para, valObj );
		return 1;
	}

	return 0;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclCheckVariableValueChanged ( GnoclCheckParams *para )
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
			checkSetVariable ( para, valObj );
		}

		else
		{
			int on = checkIsOn ( para->interp, para->onValue,
								 para->offValue, var );

			if ( on >= 0 )
				checkSetState ( para, on );
			else
				return TCL_ERROR;
		}
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
static int configure ( Tcl_Interp *interp, GnoclCheckParams *para, GnoclOption options[] )
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
		GTK_SIGNAL_FUNC ( gnoclCheckToggledFunc ),
		interp, gnoclCheckTraceFunc, para );

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
		{
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
static int cget ( Tcl_Interp *interp, GnoclCheckParams *para, GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
		obj = gnoclCgetButtonText ( interp, GTK_BUTTON ( para->widget ) );
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

/**
\brief
\author Peter G Baum
\date
\note
**/
int checkButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onToggled", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnToggledIdx, ClassIdx };
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;
	int idx;

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "checkButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   checkButtonOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, checkButtonOptions );
				}

				gnoclClearOptions ( checkButtonOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 checkButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, checkButtonOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclCheckOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclCheckButtonCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	GnoclCheckParams *para;
	int ret;

	if ( gnoclParseOptions ( interp, objc, objv, checkButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( checkButtonOptions );
		return TCL_ERROR;
	}

	para = g_new ( GnoclCheckParams, 1 );

	para->interp = interp;
	para->name = gnoclGetAutoWidgetId();
	para->widget = gtk_check_button_new_with_label ( "" );
	para->onToggled = NULL;
	para->variable = NULL;
	para->onValue = Tcl_NewIntObj ( 1 );
	Tcl_IncrRefCount ( para->onValue );
	para->offValue = Tcl_NewIntObj ( 0 );
	Tcl_IncrRefCount ( para->offValue );
	para->inSetVar = 0;

	gtk_widget_show ( para->widget );

	ret = gnoclSetOptions ( interp, checkButtonOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, para, checkButtonOptions );
	}

	gnoclClearOptions ( checkButtonOptions );

	if ( ret != TCL_OK )
	{
		Tcl_DecrRefCount ( para->onValue );
		Tcl_DecrRefCount ( para->offValue );
		g_free ( para );
		gtk_widget_destroy ( para->widget );
		return TCL_ERROR;
	}

	g_signal_connect ( G_OBJECT ( para->widget ), "destroy", G_CALLBACK ( gnoclCheckDestroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, para->widget );

	Tcl_CreateObjCommand ( interp, para->name, checkButtonFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/******/
