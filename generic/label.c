/**
 \brief
  This module implements the gnocl::label widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/** \page page24 gnocl::label
  \section sec gnocl::label
  Implementation of gnocl::label
  \subsection subsection1 Implemented Options
  \subsection subsection2 Implemented Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/label.png"
*/



/*
 * $Id: label.c,v 1.7 2005/08/16 20:57:45 baum Exp $
 *
 * This file implements the label widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
            resolved problems with use of pango markup with the -text option
   2010-01: added -ellipsize
   2009-11: added -tooltip
   2008-11: added option -angle
                         -textVariable
   2008-11: added command, class
   2008-10: added command, class
   2004-02: added -data
   2003-08: added cget
        08: switched from GnoclWidgetOptions to GnoclOption
        04: updates for gtk 2.0
   2002-01: _really_ set STD_OPTIONS
        12: removed {x,y}{Align,Pad}, use list parameters instead
        12: {x,y}{Align,Pad}
        09: underlined accelerators; bindUnderline widgetID
        07: Fixed reconfiguration, added std options
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include "gnoclparams.h"

#include <string.h>
#include <assert.h>

/****v* label/labelOptions
 * AUTHOR
 *  PGB
 * SOURCE
 *
 */

/*
typedef enum {
  PANGO_ELLIPSIZE_NONE,
  PANGO_ELLIPSIZE_START,
  PANGO_ELLIPSIZE_MIDDLE,
  PANGO_ELLIPSIZE_END
} PangoEllipsizeMode;
*/



static const int textVariableIdx = 0;
static const int onChangedIdx = 1;
static const int valueIdx = 2;
static const int textIdx = 3;

static GnoclOption labelOptions[] =
{
	{ "-textVariable", GNOCL_STRING, NULL },            /* 0 */
	{ "-onChanged", GNOCL_STRING, NULL },               /* 1 */
	{ "-text", GNOCL_STRING, NULL},                     /* 3 */

	{ "-mnemonicWidget", GNOCL_STRING, NULL },
	{ "-align", GNOCL_OBJ, "?align", gnoclOptBothAlign },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-justify", GNOCL_OBJ, "justify", gnoclOptJustification },
	{ "-ellipsize", GNOCL_OBJ, "ellipsize" , gnoclOptEllipsize },
#if GTK_CHECK_VERSION(2,6,0)
	{ "-maxWidthChars", GNOCL_INT, "max-width-chars"
	},

#endif
	{ "-name", GNOCL_STRING, "name" },
	{ "-selectable", GNOCL_BOOL, "selectable" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-visible", GNOCL_BOOL, "visible" },
#if GTK_CHECK_VERSION(2,6,0)
	{ "-widthChars", GNOCL_INT, "width-chars"
	},

#endif
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ "-wrap", GNOCL_BOOL, "wrap" },
	{ "-xPad", GNOCL_INT, "xpad" },
	{ "-yPad", GNOCL_INT, "ypad" },

	{ "-baseFont", GNOCL_OBJ, "Sans 14", gnoclOptGdkBaseFont },
	{ "-baseColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBase },
	{ "-background", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ "-angle", GNOCL_DOUBLE, "angle" },

	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },

	{ NULL }
};



/* moved to gnocl.h */
/*
typedef struct
{
    GtkLabel    *label;
    Tcl_Interp  *interp;
    char        *name;
    char        *textVariable;
    char        *onChanged;
    int         inSetVar;
} LabelParams;
*/

static int setVal ( GtkLabel *label, const char *txt );
static void changedFunc ( GtkWidget *widget, gpointer data );

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static int doCommand ( LabelParams *para, const char *val, int background )
{
	if ( para->onChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_STRING },  /* value */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.str = val;

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onChanged, background );
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static int setVal ( GtkLabel *label, const char *txt )
{
	int blocked;
	blocked = g_signal_handlers_block_matched ( G_OBJECT ( label ), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	//gtk_label_set_text ( label, txt );
	gtk_label_set_markup ( label, txt );

	//OptLabelFull ( label, txt );

	if ( blocked )
	{
		g_signal_handlers_unblock_matched ( G_OBJECT ( label ), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static char *traceFunc (
	ClientData data,
	Tcl_Interp *interp,
	const char *name1,
	const char *name2,
	int flags )
{
	LabelParams *para = ( LabelParams * ) data;

	if ( para->inSetVar == 0 && name1 )
	{
		const char *txt = name1 ? Tcl_GetVar2 ( interp, name1, name2, 0 ) : NULL;

		if ( txt )
		{
			setVal ( para->label, txt );
			doCommand ( para, txt, 1 );
		}
	}

	return NULL;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static int setTextVariable (
	LabelParams *para,
	const char *val )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/setTextVariable\n" );
#endif


	if ( para->textVariable && para->inSetVar == 0 )
	{
		const char *ret;
		para->inSetVar = 1;
		ret = Tcl_SetVar ( para->interp, para->textVariable, val, TCL_GLOBAL_ONLY );
		para->inSetVar = 0;
		return ret == NULL ? TCL_ERROR : TCL_OK;
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static void changedFunc (
	GtkWidget *widget,
	gpointer data )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/changedFunc\n" );
#endif

	LabelParams *para = ( LabelParams * ) data;

	const char *val = gtk_label_get_text ( para->label );
	setTextVariable ( para, val );
	doCommand ( para, val, 1 );
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static void destroyFunc (
	GtkWidget *widget,
	gpointer data )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/destroyFunc\n" );
#endif


	LabelParams *para = ( LabelParams * ) data;

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	gnoclAttacheOptCmdAndVar (
		NULL, &para->onChanged,
		NULL, &para->textVariable,
		"changed", G_OBJECT ( para->label ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );

	g_free ( para->textVariable );
	g_free ( para->name );
	g_free ( para );
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static int configure (
	Tcl_Interp *interp,
	LabelParams *para,
	GnoclOption options[] )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/configure\n" );
#endif


	gnoclAttacheOptCmdAndVar ( &options[onChangedIdx], &para->onChanged,
							   &options[textVariableIdx], &para->textVariable, "changed", G_OBJECT ( para->label ),
							   G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	if ( options[textVariableIdx].status == GNOCL_STATUS_CHANGED
			&& options[valueIdx].status == 0  /* value is handled below */
			&& para->textVariable != NULL )
	{
		/* if variable does not exist -> set it, else set widget state */
		const char *val = Tcl_GetVar ( interp, para->textVariable, TCL_GLOBAL_ONLY );

		if ( val == NULL )
		{
			val = gtk_label_get_text ( para->label );
			setTextVariable ( para, val );
		}

		else
			setVal ( para->label, val );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		char *str = options[valueIdx].val.str;
		setVal ( para->label, str );
		setTextVariable ( para, str );
	}

	if ( options[textIdx].status == GNOCL_STATUS_CHANGED )
	{
		char *str = options[valueIdx].val.str;
		gtk_label_set_markup ( para->label, str );
	}



	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
static int cget (
	Tcl_Interp *interp,
	LabelParams *para,
	GnoclOption options[], int idx )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/cget\n" );
#endif



	Tcl_Obj *obj = NULL;

	if ( idx == textVariableIdx )
	{
		obj = Tcl_NewStringObj ( para->textVariable, -1 );
	}

	else if ( idx == onChangedIdx )
	{
		obj = Tcl_NewStringObj ( para->onChanged ? para->onChanged : "", -1 );
	}

	else if ( idx == valueIdx )
	{
		obj = Tcl_NewStringObj ( gtk_label_get_text ( para->label ), -1 );
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
\author     Peter G Baum, William J Giddings
\date
**/
int labelFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onChanged", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnChangedIdx, ClassIdx};

	LabelParams *para = ( LabelParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->label );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "label", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   labelOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, labelOptions );
				}

				gnoclClearOptions ( labelOptions );

				return ret;
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->label ), labelOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, labelOptions, idx );
				}
			}

		case OnChangedIdx:
			{
				const char *txt = gtk_label_get_text ( para->label );

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				return doCommand ( para, txt, 0 );
			}
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
\date
**/
int gnoclLabelCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
#ifdef DEBUG_LABEL
	printf ( "label/staticFuncs/gnoclLabelCmd\n" );
#endif

	LabelParams *para;
	int ret;

	if ( gnoclParseOptions ( interp, objc, objv, labelOptions ) != TCL_OK )
	{
		gnoclClearOptions ( labelOptions );
		return TCL_ERROR;
	}

	para = g_new ( LabelParams, 1 );

	para->label = GTK_LABEL ( gtk_label_new ( NULL ) );
	para->interp = interp;
	para->textVariable = NULL;
	para->onChanged = NULL;
	para->inSetVar = 0;

	gtk_widget_show ( GTK_WIDGET ( para->label ) );

	/* added 14/Jan/2010 */
	gtk_label_set_use_markup ( para->label, TRUE );


	ret = gnoclSetOptions ( interp, labelOptions, G_OBJECT ( para->label ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, para, labelOptions );
	}

	gnoclClearOptions ( labelOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( para->label ) );
		g_free ( para );
		return TCL_ERROR;
	}

	para->name = gnoclGetAutoWidgetId();

	g_signal_connect ( G_OBJECT ( para->label ), "destroy", G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->label ) );

	Tcl_CreateObjCommand ( interp, para->name, labelFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/*****/
