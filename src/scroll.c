/**
\brief     This module implements the gnocl::scroll widget.
\authors    Peter G. Baum, William J Giddings
\date      2009-12:
*/

/* user documentation */

/**
  \page page39 gnocl::scroll
  \section sec gnocl::scroll
  Implementation of gnocl::scroll
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
  \image html "../pics/scroll.png"
*/
/*
* $Id: scroll.c,v 1.9 2005/01/01 15:27:54 baum Exp $
*
* This file implements the scroll widget
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
   2003-09: added cget
            removed getValue
        12: switched from GnoclWidgetOptions to GnoclOption
        02: removed "invoke"
   2002-01: new command "invoke"
   2001-10: Begin of developement
 */

#include "gnocl.h"
#include "gnoclparams.h"

#include <string.h>
#include <assert.h>

/* static function declarations */
static int optUpdatePolicy ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int setValue ( GtkScrollbar *scroll, double d );
static void changedFunc ( GtkWidget *widget, gpointer data );

/****v* scroll/scrollOptions
 * AUTHOR
 *  PCB
 * SOURCE
 */
/* compare with spinButton widget which is very similar */
GnoclOption scrollOptions[] =
{
	{ "-orientation", GNOCL_OBJ, NULL },         /* 0 */
	{ "-variable", GNOCL_STRING, NULL },         /* 1 */
	{ "-onValueChanged", GNOCL_STRING, NULL },   /* 2 */
	{ "-lower", GNOCL_DOUBLE, NULL },            /* 3 */
	{ "-upper", GNOCL_DOUBLE, NULL },            /* 4 */
	{ "-stepInc", GNOCL_DOUBLE, NULL },          /* 5 */
	{ "-pageInc", GNOCL_DOUBLE, NULL },          /* 6 */
	{ "-value", GNOCL_DOUBLE, NULL },            /* 7 */

	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-digits", GNOCL_INT, "digits" },
	{ "-drawValue", GNOCL_BOOL, "draw-value" },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-inverted", GNOCL_BOOL, "inverted" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-updatePolicy", GNOCL_OBJ, "update-policy", optUpdatePolicy },
	{ "-valuePos", GNOCL_OBJ, "value-pos", gnoclOptPosition },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};
/*****/

/***d* scroll/constants
 * AUTHOR
 *  PCB
 * SOURCE
 */
static const int orientationIdx    = 0;
static const int variableIdx       = 1;
static const int onValueChangedIdx = 2;
static const int lowerIdx          = 3;
static const int upperIdx          = 4;
static const int stepIncIdx        = 5;
static const int pageIncIdx        = 6;
static const int valueIdx          = 7;
/*****/

/***s* scroll/ScrollParams
 * AUTHOR
 *  PCB
 * SOURCE
 */
/* moved to gnocl.h */
/*
typedef struct
{
    char       *name;
    Tcl_Interp *interp;
    Gtkscroll   *scroll;
    char       *onValueChanged;
    char       *variable;
    int        inSetVar;
} ScrollParams;
*/

/*****/

/***f* scroll/staticFuncs/optUpdatePolicy
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int optUpdatePolicy ( Tcl_Interp *interp, GnoclOption *opt,
							 GObject *obj, Tcl_Obj **ret )
{
	const char *txt[] = { "continuous", "discontinuous", "delayed", NULL };
	const int types[] = { GTK_UPDATE_CONTINUOUS,
						  GTK_UPDATE_DISCONTINUOUS, GTK_UPDATE_DELAYED
						};

	assert ( sizeof ( GTK_UPDATE_DELAYED ) == sizeof ( int ) );

	return gnoclOptGeneric ( interp, opt, obj, "updatePolicy", txt, types, ret );
}

/*****/

/***f* scroll/staticFuncs/getObjValue
 * AUTHOR
 *  PGB
 * SOURCE
 */
static Tcl_Obj *getObjValue ( GtkScrollbar *scroll )
{
	GtkAdjustment *adjust = gtk_range_get_adjustment ( GTK_RANGE ( scroll ) );
	double d = gtk_adjustment_get_value ( adjust );

	/* scroll has no digits, only the scale */
	/*
	    if ( gtk_scroll_get_digits ( scroll ) < 1 )
	        return Tcl_NewIntObj ( ( int ) ( d + 0.5 ) );
	*/
	return Tcl_NewDoubleObj ( d );
}

/*****/

/***f* scroll/staticFuncs/doCommand
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int doCommand ( ScrollParams *para, Tcl_Obj *val, int background )
{
	if ( para->onValueChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_OBJ },  /* value */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.obj = val;

		return gnoclPercentSubstAndEval ( para->interp, ps,
										  para->onValueChanged, background );
	}

	return TCL_OK;
}

/*****/

/***f* scroll/staticFuncs/setValue
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int setValue ( GtkScrollbar *scroll, double d )
{
	GtkAdjustment *adjust = gtk_range_get_adjustment ( GTK_RANGE ( scroll ) );
	int blocked = g_signal_handlers_block_matched (
					  G_OBJECT ( adjust ), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	gtk_adjustment_set_value ( adjust, d );

	if ( blocked )
		g_signal_handlers_unblock_matched (
			G_OBJECT ( adjust ), G_SIGNAL_MATCH_FUNC,
			0, 0, NULL, ( gpointer * ) changedFunc, NULL );

	return TCL_OK;
}

/*****/

/***f* scroll/staticFuncs/traceFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
static char *traceFunc ( ClientData data,
						 Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	ScrollParams *para = ( ScrollParams * ) data;

	if ( para->inSetVar == 0 && name1 != NULL )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );
		double d;

		if ( val && Tcl_GetDoubleFromObj ( NULL, val, &d ) == TCL_OK )
		{
			setValue ( para->scroll, d );
			doCommand ( para, getObjValue ( para->scroll ), 1 );
		}
	}

	return NULL;
}

/*****/

/***f* scroll/staticFuncs/destroyFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
static void destroyFunc ( GtkWidget *widget, gpointer data )
{
	ScrollParams *para = ( ScrollParams * ) data;

	gnoclAttacheOptCmdAndVar (
		NULL, &para->onValueChanged,
		NULL, &para->variable,
		"value-changed",
		G_OBJECT ( gtk_range_get_adjustment ( GTK_RANGE ( para->scroll ) ) ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	g_free ( para->name );
	g_free ( para );
}

/*****/

/***f* scroll/staticFuncs/setVariable
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int setVariable ( ScrollParams *para, Tcl_Obj *val )
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

/*****/

/***f* scroll/staticFuncs/changedFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
static void changedFunc ( GtkWidget *widget, gpointer data )
{
	ScrollParams *para = ( ScrollParams * ) data;
	Tcl_Obj *val = getObjValue ( para->scroll );
	setVariable ( para, val );
	doCommand ( para, val, 1 );
}

/*****/

/***f* scroll/staticFuncs/configure
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, ScrollParams *para, GnoclOption options[] )
{

#ifdef DEBUG
	printf ( "scroll configure\n" );
#endif

	int ret = TCL_ERROR;
	int setAdjust = 0;
	GtkAdjustment *adjust = gtk_range_get_adjustment ( GTK_RANGE ( para->scroll ) );

	if ( gnoclSetOptions ( interp, options, G_OBJECT ( para->scroll ), -1 )
			!= TCL_OK )
		goto cleanExit;

	gnoclAttacheOptCmdAndVar (
		options + onValueChangedIdx, &para->onValueChanged,
		options + variableIdx, &para->variable,
		"value-changed", G_OBJECT ( adjust ),
		G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	/* if variable is set, synchronize variable and widget */
	if ( options[variableIdx].status == GNOCL_STATUS_CHANGED
			&& para->variable != NULL
			&& options[valueIdx].status != GNOCL_STATUS_CHANGED )
	{
		Tcl_Obj *var = Tcl_GetVar2Ex ( interp, para->variable, NULL,
									   TCL_GLOBAL_ONLY );

		assert ( strcmp ( options[variableIdx].optName, "-variable" ) == 0 );

		if ( var == NULL ) /* variable does not yet exist */
		{
			setVariable ( para, getObjValue ( para->scroll ) );
		}

		else
		{
			double d;

			if ( Tcl_GetDoubleFromObj ( interp, var, &d ) != TCL_OK )
				goto cleanExit;

			setValue ( para->scroll, d );
		}

	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[valueIdx].optName, "-value" ) == 0 );
		setValue ( para->scroll, options[valueIdx].val.d );
		setVariable ( para, getObjValue ( para->scroll ) );
	}

	if ( options[lowerIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[lowerIdx].optName, "-lower" ) == 0 );
		adjust->lower = options[lowerIdx].val.d;
		setAdjust = 1;
	}

	if ( options[upperIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[upperIdx].optName, "-upper" ) == 0 );
		adjust->upper = options[upperIdx].val.d;
		setAdjust = 1;
	}

	if ( options[stepIncIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[stepIncIdx].optName, "-stepInc" ) == 0 );
		adjust->step_increment = options[stepIncIdx].val.d;
		setAdjust = 1;
	}

	if ( options[pageIncIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[pageIncIdx].optName, "-pageInc" ) == 0 );
		adjust->page_increment = options[pageIncIdx].val.d;
		setAdjust = 1;
	}

	if ( setAdjust )
		gtk_adjustment_changed ( adjust );

	/*
	spinButtonTraceFunc( para, interp, para->variable, NULL, 0 );
	*/

	ret = TCL_OK;

cleanExit:

	return ret;
}

/*****/

/***f* scroll/staticFuncs/cget
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, ScrollParams *para,
				  GnoclOption options[], int idx )
{
	GtkAdjustment *adjust = gtk_range_get_adjustment ( GTK_RANGE ( para->scroll ) );

	Tcl_Obj *obj = NULL;

	if ( idx == orientationIdx )
	{
		if ( GTK_CHECK_TYPE ( para->scroll, GTK_TYPE_HSCROLLBAR ) )
			obj = Tcl_NewStringObj ( "horizontal", -1 );
		else
			obj = Tcl_NewStringObj ( "vertical", -1 );

	}

	else if ( idx == variableIdx )
		obj = Tcl_NewStringObj ( para->variable, -1 );
	else if ( idx == onValueChangedIdx )
	{
		obj = Tcl_NewStringObj (
				  para->onValueChanged ? para->onValueChanged : "", -1 );
	}

	else if ( idx == lowerIdx )
		obj = Tcl_NewDoubleObj ( adjust->lower );
	else if ( idx == upperIdx )
		obj = Tcl_NewDoubleObj ( adjust->upper );
	else if ( idx == stepIncIdx )
		obj = Tcl_NewDoubleObj ( adjust->step_increment );
	else if ( idx == pageIncIdx )
		obj = Tcl_NewDoubleObj ( adjust->page_increment );
	else if ( idx == valueIdx )
		obj = getObjValue ( para->scroll );

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/***f* scroll/staticFuncs/scrollFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
int scrollFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )

{
	static const char *cmds[] = { "delete", "configure", "cget", "onValueChanged", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnValueChangedIdx, ClassIdx };
	ScrollParams *para = ( ScrollParams * ) data;
	GtkWidget   *widget = GTK_WIDGET ( para->scroll );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "scroll", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseOptions ( interp, objc - 1, objv + 1,
										 scrollOptions ) == TCL_OK )
				{
					ret = configure ( interp, para, scrollOptions );
				}

				gnoclClearOptions ( scrollOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->scroll ),
									 scrollOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, scrollOptions, idx );
				}
			}

		case OnValueChangedIdx:
			{
				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				return doCommand ( para, getObjValue ( para->scroll ), 0 );
			}
	}

	return TCL_OK;
}

/*****/

/***f* scroll/gnoclscrollCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclscrollCmd ( ClientData data, Tcl_Interp *interp,
					 int objc, Tcl_Obj * const objv[] )
{
	GtkOrientation orient = GTK_ORIENTATION_VERTICAL;
	GtkAdjustment  *adjust;
	ScrollParams    *para;

	if ( gnoclParseOptions ( interp, objc, objv, scrollOptions ) != TCL_OK )
	{
		gnoclClearOptions ( scrollOptions );
		return TCL_ERROR;
	}

	if ( scrollOptions[orientationIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGetOrientationType ( interp,
									   scrollOptions[orientationIdx].val.obj, &orient ) != TCL_OK )
		{
			gnoclClearOptions ( scrollOptions );
			return TCL_ERROR;
		}
	}

	para = g_new ( ScrollParams, 1 );

	adjust = GTK_ADJUSTMENT ( gtk_adjustment_new ( 0, 0, 100, 1, 10, 0 ) );

	if ( orient == GTK_ORIENTATION_HORIZONTAL )
		para->scroll = GTK_SCROLLBAR ( gtk_hscrollbar_new ( adjust ) );
	else
		para->scroll = GTK_SCROLLBAR ( gtk_vscrollbar_new ( adjust ) );

	para->interp = interp;

	para->variable = NULL;

	para->onValueChanged = NULL;

	para->inSetVar = 0;

	gtk_widget_show ( GTK_WIDGET ( para->scroll ) );

	if ( configure ( interp, para, scrollOptions ) != TCL_OK )
	{
		g_free ( para );
		gtk_widget_destroy ( GTK_WIDGET ( para->scroll ) );
		return TCL_ERROR;
	}

	para->name = gnoclGetAutoWidgetId();

	g_signal_connect ( G_OBJECT ( para->scroll ), "destroy",
					   G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->scroll ) );

	Tcl_CreateObjCommand ( interp, para->name, scrollFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/*****/
