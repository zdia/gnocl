/**
 \brief
  This module implements the gnocl::spinButton widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page43 gnocl::spinButton
  \section sec gnocl::spinButton
  Implementation of gnocl::spinButton
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
  \image html "../pics/spinButton.png"
*/
/*
 * $Id: spinButton.c,v 1.9 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the spinButton widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   TODO:
      - getValue -type string|integer|float
      - -itemList {January February March...}
      - -onFocusOut

   History:
   2008-10: added command, class
   2004-02: added -data
   2003-09: cget
            removed getValue, setValue
        12: cleanups
        08: switched from GnoclWidgetOptions to GnoclOption
   2002-01: bugfixes concerning behavior of -variable and -command
   2001-10: Begin of developement
*/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/***v* spinButton/spinButtonOptions
 * AUTHOR
 *  PGB
 * SOURCE
 */
/* compare with scale widget which is very similar */
static GnoclOption spinButtonOptions[] =
{
	{ "-variable", GNOCL_STRING, NULL },         /* 0 */
	{ "-onValueChanged", GNOCL_STRING, NULL },   /* 1 */
	{ "-lower", GNOCL_DOUBLE, NULL },            /* 2 */
	{ "-upper", GNOCL_DOUBLE, NULL },            /* 3 */
	{ "-stepInc", GNOCL_DOUBLE, NULL },          /* 4 */
	{ "-pageInc", GNOCL_DOUBLE, NULL },          /* 5 */
	{ "-value", GNOCL_DOUBLE, NULL },            /* 6 */

	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-digits", GNOCL_INT, "digits" },
	{ "-editable", GNOCL_BOOL, "editable" },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-maxLength", GNOCL_INT, "max-length" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-numeric", GNOCL_BOOL,  "numeric" },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ "-wrap", GNOCL_BOOL,  "wrap" },
	{ NULL }
};
/*****/

/***d* spinButton/constants
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int variableIdx       = 0;
static const int onValueChangedIdx = 1;
static const int lowerIdx          = 2;
static const int upperIdx          = 3;
static const int stepIncIdx        = 4;
static const int pageIncIdx        = 5;
static const int valueIdx          = 6;
/*****/

/***s* spinButton/SpinButtonParams
 * AUTHOR
 *  PGB
 * SOURCE
 */

/* moved to gnocl.h */
/*
typedef struct
{
    char          *name;
    char          *variable;
    char          *onValueChanged;
    GtkSpinButton *spinButton;
    Tcl_Interp    *interp;
    int           inSetVar;
} SpinButtonParams;
*/

/* static function declarations */
static void changedFunc ( GtkWidget *widget, gpointer data );
static int setValue ( SpinButtonParams *para, Tcl_Obj *val );

/**
\brief
\author  PGB
\date
 */
static int doCommand ( SpinButtonParams *para, Tcl_Obj *val, int background )
{
	if ( para->onValueChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_OBJ },     /* value */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.obj = val;

		return gnoclPercentSubstAndEval ( para->interp, ps,
										  para->onValueChanged, background );
	}

	return TCL_OK;
}

/**
\brief
\author  PGB
\date
 */
static int setValue ( SpinButtonParams *para, Tcl_Obj *val )
{
	int blocked = g_signal_handlers_block_matched (
					  G_OBJECT ( para->spinButton ), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	/* first try to set as double (no string conversion) */
	double d;

	if ( Tcl_GetDoubleFromObj ( NULL, val, &d ) == TCL_OK )
		gtk_spin_button_set_value ( para->spinButton , d );
	else
		gtk_entry_set_text ( GTK_ENTRY ( para->spinButton ),
							 Tcl_GetString ( val ) );

	if ( blocked )
	{
		g_signal_handlers_unblock_matched (
			G_OBJECT ( para->spinButton ), G_SIGNAL_MATCH_FUNC, 0,
			0, NULL, ( gpointer * ) changedFunc, NULL );
	}

	return 0;
}

/**
\brief
\author  PGB
\date
 */
static char *traceFunc ( ClientData data,
						 Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	SpinButtonParams *para = ( SpinButtonParams * ) data;

	if ( para->inSetVar == 0 && name1 != NULL )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );

		if ( val )
		{
			setValue ( para, val );
			doCommand ( para, val, 1 );
		}
	}

	return NULL;
}

/**
\brief
\author  PGB
\date
 */
static Tcl_Obj *getObjValue ( GtkSpinButton *spinButton )
{
	if ( gtk_spin_button_get_digits ( spinButton ) == 0 )
		return Tcl_NewIntObj ( gtk_spin_button_get_value_as_int ( spinButton ) );

	return Tcl_NewDoubleObj ( gtk_spin_button_get_value ( spinButton ) );
}

/**
\brief
\author  PGB
\date
 */
static int setVariable ( SpinButtonParams *para, Tcl_Obj *val )
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
\author  PGB
\date
 */
static void changedFunc ( GtkWidget *widget, gpointer data )
{
	SpinButtonParams *para = ( SpinButtonParams * ) data;
	Tcl_Obj *val = getObjValue ( para->spinButton );
	setVariable ( para, val );
	doCommand ( para, val, 1 );
}

/**
\brief
\author  PGB
\date
 */
static void spinButtonDestroyFunc ( GtkWidget *widget, gpointer data )
{
	SpinButtonParams *para = ( SpinButtonParams * ) data;

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	gnoclAttacheOptCmdAndVar ( NULL, &para->onValueChanged,
							   NULL, &para->variable,
							   "changed", G_OBJECT ( para->spinButton ), G_CALLBACK ( changedFunc ),
							   para->interp, traceFunc, para );

	g_free ( para->variable );
	g_free ( para->name );
	g_free ( para );
}

/**
\brief
\author  PGB
\date
 */
static int configure ( Tcl_Interp *interp, SpinButtonParams *para,
					   GnoclOption options[] )
{
	int ret = TCL_ERROR;
	int   blocked = 0;

	int setAdjust = 0;
	GtkAdjustment *oldAdjust = gtk_spin_button_get_adjustment (
								   para->spinButton );
	gfloat lower = oldAdjust->lower;
	gfloat upper = oldAdjust->upper;
	gfloat stepInc = oldAdjust->step_increment;
	gfloat pageInc = oldAdjust->page_increment;


	if ( gnoclSetOptions ( interp, options, G_OBJECT ( para->spinButton ), -1 )
			!= TCL_OK )
		goto cleanExit;

	gnoclAttacheOptCmdAndVar ( options + onValueChangedIdx, &para->onValueChanged,
							   options + variableIdx, &para->variable,
							   "value-changed", G_OBJECT ( para->spinButton ),
							   G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	if ( para->onValueChanged != NULL )
	{
		blocked = g_signal_handlers_block_matched (
					  G_OBJECT ( para->spinButton ), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[valueIdx].optName, "-value" ) == 0 );
		gtk_spin_button_set_value ( para->spinButton, options[valueIdx].val.d );

		if ( para->variable )
		{
			Tcl_Obj *obj = getObjValue ( para->spinButton );
			para->inSetVar++;
			obj = Tcl_SetVar2Ex ( para->interp, para->variable, NULL, obj,
								  TCL_GLOBAL_ONLY );
			para->inSetVar--;

			if ( obj == NULL )
				goto cleanExit;
		}
	}

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
			Tcl_Obj *obj = getObjValue ( para->spinButton );

			para->inSetVar++;
			obj = Tcl_SetVar2Ex ( para->interp, para->variable, NULL, obj,
								  TCL_GLOBAL_ONLY );
			para->inSetVar--;

			if ( obj == NULL )
				goto cleanExit;
		}

		else
		{
			double d;

			if ( Tcl_GetDoubleFromObj ( interp, var, &d ) != TCL_OK )
				goto cleanExit;

			gtk_spin_button_set_value ( para->spinButton, d );
		}

	}

	if ( options[lowerIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[lowerIdx].optName, "-lower" ) == 0 );
		lower = options[lowerIdx].val.d;
		setAdjust = 1;
	}

	if ( options[upperIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[upperIdx].optName, "-upper" ) == 0 );
		upper = options[upperIdx].val.d;
		setAdjust = 1;
	}

	if ( options[stepIncIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[stepIncIdx].optName, "-stepInc" ) == 0 );
		stepInc = options[stepIncIdx].val.d;
		setAdjust = 1;
	}

	if ( options[pageIncIdx].status == GNOCL_STATUS_CHANGED )
	{
		assert ( strcmp ( options[pageIncIdx].optName, "-pageInc" ) == 0 );
		pageInc = options[pageIncIdx].val.d;
		setAdjust = 1;
	}


	if ( setAdjust )
	{
		/* see also scale.c */
		/* last parameter is pageSize, where it is used? */
		gtk_spin_button_set_adjustment ( para->spinButton,
										 GTK_ADJUSTMENT ( gtk_adjustment_new ( oldAdjust->value, lower, upper,
																			   stepInc, pageInc, 0 ) ) );
		/* gtk_spin_button_update( para->spinButton ); */
	}

	/*
	spinButtonTraceFunc( para, interp, para->variable, NULL, 0 );
	*/

	ret = TCL_OK;

cleanExit:

	if ( blocked )
	{
		g_signal_handlers_unblock_matched (
			G_OBJECT ( para->spinButton ), G_SIGNAL_MATCH_FUNC, 0,
			0, NULL, ( gpointer * ) changedFunc, NULL );
	}

	return ret;
}

/**
\brief
\author  PGB
\date
 */
static int cget ( Tcl_Interp *interp, SpinButtonParams *para,
				  GnoclOption options[], int idx )
{
	GtkAdjustment *adjust = gtk_spin_button_get_adjustment ( para->spinButton );

	Tcl_Obj *obj = NULL;

	if ( idx == variableIdx )
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
		obj = getObjValue ( para->spinButton );

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief
\author  PGB
\date
 */
int spinButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onValueChanged", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnValueChangedIdx, ClassIdx};
	SpinButtonParams *para = ( SpinButtonParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->spinButton );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "spinButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseOptions ( interp, objc - 1, objv + 1,
										 spinButtonOptions ) == TCL_OK )
				{
					ret = configure ( interp, para, spinButtonOptions );
				}

				gnoclClearOptions ( spinButtonOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv,
									 G_OBJECT ( para->spinButton ), spinButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, spinButtonOptions, idx );
				}
			}

		case OnValueChangedIdx:
			{
				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				return doCommand ( para, getObjValue ( para->spinButton ), 0 );
			}
	}

	return TCL_OK;
}

/**
\brief
\author
\date
 */
int gnoclSpinButtonCmd ( ClientData data, Tcl_Interp *interp,
						 int objc, Tcl_Obj * const objv[] )
{
	SpinButtonParams *para;

	if ( gnoclParseOptions ( interp, objc, objv, spinButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( spinButtonOptions );
		return TCL_ERROR;
	}

	para = g_new ( SpinButtonParams, 1 );

	para->spinButton = GTK_SPIN_BUTTON ( gtk_spin_button_new (
											 GTK_ADJUSTMENT ( gtk_adjustment_new ( 0, 0, 100,
																				   1, 10, 20 ) ), 1.0, 1 ) );


	para->interp = interp;
	para->variable = NULL;
	para->onValueChanged = NULL;
	para->inSetVar = 0;

	gtk_spin_button_set_numeric ( para->spinButton, 1 );

	gtk_widget_show ( GTK_WIDGET ( para->spinButton ) );

	if ( configure ( interp, para, spinButtonOptions ) != TCL_OK )
	{
		g_free ( para );
		gtk_widget_destroy ( GTK_WIDGET ( para->spinButton ) );
		gnoclClearOptions ( spinButtonOptions );
		return TCL_ERROR;
	}

	gnoclClearOptions ( spinButtonOptions );

	para->name = gnoclGetAutoWidgetId();

	g_signal_connect ( G_OBJECT ( para->spinButton ), "destroy",
					   G_CALLBACK ( spinButtonDestroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->spinButton ) );

	Tcl_CreateObjCommand ( interp, para->name, spinButtonFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}
