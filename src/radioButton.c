/**
 \brief
  This module implements the gnocl::radioButton widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page38 gnocl::radioButton
  \section sec gnocl::radioButton
  Implementation of gnocl::radioButton
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
  \image html "../pics/radioButton.png"
*/
/*
 * $Id: radioButton.c,v 1.15 2005/02/25 21:33:38 baum Exp $
 *
 * This file implements the radioButton widget
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
   2003-01: unification with menuRadioItem
        08: switched from GnoclWidgetOptions to GnoclOption
            changed -command to -onToggled
        04: updates for gtk 2.0
        01: new command "getValue", "setValue"
   2002-01: new command "invoke"
        09: underlined accelerators
   2001-04: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****v* radioButton/radioButtonOptions
 * AUTHOR
 *  PGB
 * SOURCE
 */
static GnoclOption radioButtonOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },        /* 0 */
	{ "-onToggled", GNOCL_STRING },      /* 1 */
	{ "-variable", GNOCL_STRING },       /* 2 */
	{ "-onValue", GNOCL_OBJ, NULL },     /* 3 */
	{ "-active", GNOCL_BOOL, NULL },     /* 4 */
	{ "-value", GNOCL_OBJ, NULL },       /* 5 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-indicatorOn", GNOCL_BOOL, "draw-indicator" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL },
};
/*****/

/****v* radioButton/constants
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int textIdx      = 0;
static const int onToggledIdx = 1;
static const int variableIdx  = 2;
static const int onValueIdx   = 3;
static const int activeIdx    = 4;
static const int valueIdx     = 5;
/*****/

/* static function declarations */
static int radioSetVariable ( GnoclRadioParams *para, Tcl_Obj *value );
static int radioSetState ( GnoclRadioGroup *group, Tcl_Obj *obj );

/**
\brief
\author Peter G Baum
\date
\note
**/
/* -------------------------------
   functions for all radio widgets for managing radio groups
   ------------------------------- */
typedef GnoclRadioParams *GnoclRadioParamsPointer;

/**
\brief
\author Peter G Baum
\date
\note
**/
static GHashTable        *radioGroup = NULL;

/**
\brief
\author Peter G Baum
\date
\note
**/
GnoclRadioGroup *gnoclRadioGroupNewGroup ( const char *var, Tcl_Interp *interp )
{
	GnoclRadioGroup *group = g_new ( GnoclRadioGroup, 1 );
	group->widgets = g_array_new ( 0, 0, sizeof ( void * ) );
	group->variable = g_strdup ( var );
	group->interp = interp;
	group->inSetVar = 0;

	if ( radioGroup == NULL )
		radioGroup = g_hash_table_new ( g_str_hash, g_str_equal );

	/* add this group to map variable -> group
	   use group->variable and not var since former is persistent */
	g_hash_table_insert ( radioGroup, group->variable, group );

	Tcl_TraceVar ( interp, var, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY,
				   gnoclRadioTraceFunc, group );

	return group;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioGroupAddWidgetToGroup ( GnoclRadioGroup *group,
									  GnoclRadioParams *para )
{
	group->widgets = g_array_append_val ( group->widgets, para );
	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioSetValueActive ( GnoclRadioParams *para, GnoclOption *value,
							   GnoclOption *active )
{
	GnoclRadioGroup *group = para->group;

	if ( value->status == GNOCL_STATUS_CHANGED )
	{
		/* test, if this value is already used in this group */
		const char *val = Tcl_GetString ( value->val.obj );
		guint k;

		for ( k = 0; k < group->widgets->len; ++k )
		{
			GnoclRadioParams *p = g_array_index ( group->widgets,
												  GnoclRadioParamsPointer, k );

			if ( p != para && strcmp ( Tcl_GetString ( p->onValue ), val ) == 0 )
			{
				Tcl_AppendResult ( group->interp, "Widget \"", p->name,
								   "\" has same value \"", val,
								   "\" in this radio group.", NULL );
				return TCL_ERROR;
			}
		}

		/* move value to button parameter */
		GNOCL_MOVE_OBJ ( value->val.obj, para->onValue );

		/* if variable does not exist or this widget is active,
		   create it with this onValue,
		   else set group according to variable
		*/
		if ( active->status != GNOCL_STATUS_CHANGED )
		{
			Tcl_Obj *var = Tcl_GetVar2Ex ( group->interp, group->variable, NULL,
										   TCL_GLOBAL_ONLY );

			if ( var == NULL )
			{
				radioSetVariable ( para, para->onValue );
				radioSetState ( group, para->onValue );
			}

			else
				radioSetState ( group, var );
		}
	}

	if ( active->status == GNOCL_STATUS_CHANGED && active->val.b )
	{
		radioSetVariable ( para, para->onValue );
		radioSetState ( group, para->onValue );
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioRemoveWidgetFromGroup ( GnoclRadioGroup *group,
									  GnoclRadioParams *para )
{
	guint k;

	for ( k = 0; k < group->widgets->len; ++k )
	{
		void *p = g_array_index ( group->widgets, gpointer, k );

		if ( p == para )
		{
			g_array_remove_index_fast ( group->widgets, k );

			if ( group->widgets->len == 0 )
			{
				/* remove complete group */
				Tcl_UntraceVar ( group->interp, group->variable,
								 TCL_TRACE_WRITES | TCL_GLOBAL_ONLY, gnoclRadioTraceFunc,
								 group );
				g_hash_table_remove ( radioGroup, group->variable );
				g_free ( group->variable );
				g_array_free ( group->widgets, 1 );
				g_free ( group );
				return 0;
			}

			return group->widgets->len;
		}
	}

	assert ( 0 );

	return -1;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
GnoclRadioParams *gnoclRadioGetActivePara ( GnoclRadioGroup *group )
{
	guint k;

	/*
	for( k = 0; k < group->widgets->len; ++k )
	{
	   int on;
	   GnoclRadioParams *p = g_array_index( group->widgets,
	                     GnoclRadioParamsPointer, k );
	   g_object_get( G_OBJECT( p->widget ), "active", &on, NULL );
	   printf( "%d %s active: %d\n", k, p->name, on );
	}
	*/

	for ( k = 0; k < group->widgets->len; ++k )
	{
		int on;
		GnoclRadioParams *p = g_array_index ( group->widgets,
											  GnoclRadioParamsPointer, k );
		g_object_get ( G_OBJECT ( p->widget ), "active", &on, NULL );

		if ( on )
			return p;
	}

	return NULL;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
GnoclRadioGroup *gnoclRadioGetGroupFromVariable ( const char *var )
{
	if ( var == NULL  || radioGroup == NULL )
		return NULL;

	return g_hash_table_lookup ( radioGroup, var );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
GnoclRadioParams *gnoclRadioGetParam ( GnoclRadioGroup *group, int n )
{
	return g_array_index ( group->widgets, gpointer, n );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
/* -------------------------------
   functions for all radio widgets:
      callback, trace etc.
   ------------------------------- */
static int radioSetVariable ( GnoclRadioParams *para, Tcl_Obj *value )
{
	if ( para->group->inSetVar == 0 )
	{
		Tcl_Obj *ret;
		para->group->inSetVar = 1;
		ret = Tcl_SetVar2Ex ( para->group->interp, para->group->variable, NULL,
							  value, TCL_GLOBAL_ONLY );
		para->group->inSetVar = 0;
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
static int radioSetState ( GnoclRadioGroup *group, Tcl_Obj *obj )
{
	const char *val = Tcl_GetString ( obj );
	guint k;
	int set = 0;

	for ( k = 0; k < group->widgets->len; ++k )
	{
		GnoclRadioParams *para = g_array_index ( group->widgets,
								 GnoclRadioParamsPointer, k );
		int on = ( strcmp ( Tcl_GetString ( para->onValue ), val ) == 0 );
		int blocked = g_signal_handlers_block_matched (
						  G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC,
						  0, 0, NULL, ( gpointer * ) gnoclRadioToggledFunc, NULL );
		g_object_set ( G_OBJECT ( para->widget ), "active", ( gboolean ) on, NULL );
		set += on;

		if ( blocked )
			g_signal_handlers_unblock_matched (
				G_OBJECT ( para->widget ), G_SIGNAL_MATCH_FUNC,
				0, 0, NULL, ( gpointer * ) gnoclRadioToggledFunc, NULL );
	}

	assert ( set <= 1 );

	return set;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
static int radioDoCommand ( GnoclRadioParams *para, int background )
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
		ps[1].val.obj = para->onValue;

		return gnoclPercentSubstAndEval ( para->group->interp, ps,
										  para->onToggled, background );
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
void gnoclRadioDestroyFunc ( GtkWidget *widget, gpointer data )
{
	GnoclRadioParams *para = ( GnoclRadioParams * ) data;
	Tcl_Interp *interp = para->group->interp;

	gnoclRadioRemoveWidgetFromGroup ( para->group, para );

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( interp, para->name );

	Tcl_DecrRefCount ( para->onValue );
	g_free ( para->onToggled );
	g_free ( para->name );
	g_free ( para );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
void gnoclRadioToggledFunc ( GtkWidget *widget, gpointer data )
{
	int on;
	GnoclRadioParams *para = ( GnoclRadioParams * ) data;
	g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );

	if ( on )
	{
		radioSetVariable ( para, para->onValue );
		radioDoCommand ( para, 1 );
	}
}

/**
\brief
\author Peter G Baum
\date
\note
**/
char *gnoclRadioTraceFunc ( ClientData data,
							Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	GnoclRadioGroup *group = ( GnoclRadioGroup * ) data;

	if ( group->inSetVar == 0 )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );

		if ( val )
			radioSetState ( group, val );
	}

	return NULL;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
Tcl_Obj *gnoclRadioGetValue ( GnoclRadioParams *para )
{
	GnoclRadioParams *p = gnoclRadioGetActivePara ( para->group );

	if ( p )
		return p->onValue;

	return Tcl_NewStringObj ( "", -1 );
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioSetValue ( GnoclRadioParams *para, Tcl_Obj *val )
{
	if ( radioSetState ( para->group, val ) == 1 )
	{
		radioSetVariable ( para, val );
		return TCL_OK;
	}

	Tcl_AppendResult ( para->group->interp, "Value \"", Tcl_GetString ( val ),

					   "\" not found in group.", NULL );
	return TCL_ERROR;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioOnToggled ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[],
						  GnoclRadioParams *para )
{
	GnoclRadioParams *p = gnoclRadioGetActivePara ( para->group );

	if ( p )
		return radioDoCommand ( p, 0 );

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
/* --------------------
   radioButton specific functions
   -------------------- */

static int cget ( Tcl_Interp *interp, GnoclRadioParams *para,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
		obj = gnoclCgetButtonText ( interp, GTK_BUTTON ( para->widget ) );
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

	else if ( idx == valueIdx )
		obj = gnoclRadioGetValue ( para );

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
static int configure ( Tcl_Interp *interp, GnoclRadioParams *para,
					   GnoclOption options[] )
{
	if ( options[textIdx].status == GNOCL_STATUS_CHANGED
			&& gnoclConfigButtonText ( interp, GTK_BUTTON ( para->widget ),
									   options[textIdx].val.obj ) != TCL_OK )
	{
		return TCL_ERROR;
	}

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

/**
\brief
\author Peter G Baum
\date
\note
**/
int radioButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onToggled", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx,  CgetIdx, OnToggledIdx, ClassIdx };
	GnoclRadioParams *para = ( GnoclRadioParams * ) data;
	int idx;

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "radioButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   radioButtonOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, radioButtonOptions );
				}

				gnoclClearOptions ( radioButtonOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 radioButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, radioButtonOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclRadioOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/**
\brief
\author Peter G Baum
\date
\note
**/
int gnoclRadioButtonCmd ( ClientData data, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[] )
{
	GnoclRadioParams  *para;
	int                ret;

	if ( gnoclParseOptions ( interp, objc, objv, radioButtonOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( radioButtonOptions );
		return TCL_ERROR;
	}

	if ( radioButtonOptions[onValueIdx].status != GNOCL_STATUS_CHANGED
			|| radioButtonOptions[variableIdx].status != GNOCL_STATUS_CHANGED )
	{
		gnoclClearOptions ( radioButtonOptions );
		Tcl_SetResult ( interp,
						"Option \"-onValue\" and \"-variable\" are required.", TCL_STATIC );
		return TCL_ERROR;
	}

	para = g_new ( GnoclRadioParams, 1 );

	para->name = gnoclGetAutoWidgetId();
	para->widget = gtk_radio_button_new ( NULL );
	para->onToggled = NULL;
	para->onValue = NULL;
	gtk_widget_show ( para->widget );

	para->group = gnoclRadioGetGroupFromVariable (
					  radioButtonOptions[variableIdx].val.str );

	if ( para->group == NULL )
	{
		para->group = gnoclRadioGroupNewGroup (
						  radioButtonOptions[variableIdx].val.str, interp );
	}

	else
	{
		GnoclRadioParams *p = gnoclRadioGetParam ( para->group, 0 );
		gtk_radio_button_set_group ( GTK_RADIO_BUTTON ( para->widget ),
									 gtk_radio_button_get_group ( GTK_RADIO_BUTTON ( p->widget ) ) );
	}

	gnoclRadioGroupAddWidgetToGroup ( para->group, para );

	ret = gnoclSetOptions ( interp, radioButtonOptions,
							G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, radioButtonOptions );

	gnoclClearOptions ( radioButtonOptions );

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

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->widget ) );

	Tcl_CreateObjCommand ( interp, para->name, radioButtonFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/*****/
