/**
 \brief     This module implements the gnocl::labelEntry widget.
 \authors    Peter G. Baum, William J Giddings
 \date      2001-03:
**/

/* user documentation */

/**
  \page page26 gnocl::labelEntry
  \section sec gnocl::labelEntry
  Implementation of gnocl::labelEntry
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
 \image html "../pics/labelEntry.png"
*/

/*
 * $Id: entry.c,v 1.11 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the entry widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption labelEntryOptions[] =
{
	/* box and widget options */
	{ "-variable", GNOCL_STRING, NULL },       /* 0 */
	{ "-onChanged", GNOCL_STRING, NULL },      /* 1 */
	{ "-value", GNOCL_STRING, NULL },          /* 2 */
	{ "-name", GNOCL_STRING, NULL },           /* 4 */
	{ "-data", GNOCL_STRING, NULL },           /* 5 */
	{ "-sizeGroup", GNOCL_STRING, NULL },      /* 6 */
	{ "-visible", GNOCL_STRING, NULL },        /* 7 */

	/* label options */
	{ "-textVariable", GNOCL_STRING, NULL },   /* 9 */
	{ "-text", GNOCL_STRING, NULL},            /* 10 */
	{ "-textFont", GNOCL_STRING, NULL },       /* 11 */
	{ "-textBaseColor",  GNOCL_STRING, NULL }, /* 12 */
	{ "-textLength", GNOCL_STRING, NULL },     /* 13 */
	{ "-justify", GNOCL_STRING, NULL},        /* 14 */

	/* entry options */
	{ "-baseFont", GNOCL_OBJ, "Sans 14", gnoclOptGdkBaseFont },
	{ "-baseColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBase },

	{ "-editable", GNOCL_BOOL, "editable" },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-maxLength", GNOCL_INT, "max-length" },

	{ "-onActivate", GNOCL_OBJ, "activate", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },

	{ "-widthChars", GNOCL_INT, "width-chars" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },

	{ NULL }
};

static const int variableIdx       = 0;
static const int onChangedIdx      = 1;
static const int valueIdx          = 2;
static const int tooltipIdx        = 3;
static const int nameIdx           = 4;
static const int dataIdx           = 5;
static const int sizeGroupIdx      = 6;
static const int visibleIdx        = 7;
static const int textVariableIdx   = 8;
static const int textIdx           = 9;
static const int textFontIdx       = 10;
static const int textBaseColorIdx  = 11;

typedef struct
{
	GtkLabel    *widget;
	char        *name;
	char        *textVariable;
	char        *onChanged;
	int         inSetVar;
} _LabelParams;

typedef struct
{
	GtkEntry    *widget;
	char        *name;
	char        *variable;
	char        *onChanged;
	int         inSetVar;
} _EntryParams;

/* create a structure which contains vital data for each child widget */

typedef struct
{
	GtkBox      *box;
	Tcl_Interp  *interp;

	struct _LabelParams *label;

	struct _EntryParams *entry;
} LabelEntryParams;



static int setEntryVal ( GtkEntry *entry, const char *txt );
static void changedFunc ( GtkWidget *widget, gpointer data );

/**
\brief
\author
\date
**/
static int doCommand ( LabelEntryParams *para, const char *val, int background )
{
	if ( para->entry->onChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'v', GNOCL_STRING },  /* value */
			{ 0 }
		};

		ps[0].val.str = para->entry->name;
		ps[1].val.str = val;

		return gnoclPercentSubstAndEval ( para->interp, ps, para->entry->onChanged, background );
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
static int setEntryVal ( GtkEntry *entry, const char *txt )
{


	int blocked = g_signal_handlers_block_matched (
					  G_OBJECT ( entry ), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( gpointer * ) changedFunc, NULL );
	gtk_entry_set_text ( entry, txt );

	if ( blocked )
		g_signal_handlers_unblock_matched (
			G_OBJECT ( entry ), G_SIGNAL_MATCH_FUNC,
			0, 0, NULL, ( gpointer * ) changedFunc, NULL );

	return TCL_OK;
}

/**
\brief
\author
\date
**/
static char *traceEntryFunc (
	ClientData data,
	Tcl_Interp *interp,
	const char *name1,
	const char *name2,
	int flags )
{
	LabelEntryParams *para = ( LabelEntryParams * ) data;

	if ( para->inSetVar == 0 && name1 )
	{
		const char *txt = name1 ? Tcl_GetVar2 ( interp, name1, name2, 0 ) : NULL;

		if ( txt )
		{
			setEntryVal ( para->entry, txt );
			doCommand ( para, txt, 1 );
		}
	}

	return NULL;
}

/**
\brief
\author
\date
**/
static int setVariable (
	LabelEntryParams *para,
	const char *val )
{


	if ( para->variable && para->inSetVar == 0 )
	{
		const char *ret;
		para->inSetVar = 1;
		ret = Tcl_SetVar ( para->interp, para->variable, val, TCL_GLOBAL_ONLY );
		para->inSetVar = 0;
		return ret == NULL ? TCL_ERROR : TCL_OK;
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
static void changedFunc (
	GtkWidget *widget,
	gpointer data )
{


	LabelEntryParams *para = ( LabelEntryParams * ) data;
	const char *val = gtk_entry_get_text ( para->entry );
	setVariable ( para, val );
	doCommand ( para, val, 1 );
}

/**
\brief
\author
\date
**/
static void destroyFunc (
	GtkWidget *widget,
	gpointer data )
{



	LabelEntryParams *para = ( LabelEntryParams * ) data;

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	gnoclAttacheOptCmdAndVar (
		NULL, &para->onChanged,
		NULL, &para->variable,
		"changed", G_OBJECT ( para->entry ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );

	g_free ( para->variable );
	g_free ( para->name );
	g_free ( para );
}


/**
\brief
\author
\date
**/
static int _configure (
	Tcl_Interp *interp,
	Tcl_Obj * const objv[],
	LabelEntryParams *para,
	GnoclOption options[] )
{

	printf ( "labeEntry/staticFuncs/_configure\n" );

	static const char *cmds[] =
	{
		"variable", "onChanged", "value", "tooltip",
		"name", "sizeGroup", "visible", "textVariable",
		"text", "textBaseFont", "textBaseColor", NULL
	};

	enum cmdIdx  { VariableIdx, OnChangedIdx, ValueIdx, TooltipIdx,
				   NameIdx, SizeGroupIdx, VisibleIdx, TextVariableIdx,
				   TextIdx, TextBaseFontIdx, TextBaseColorIdx
				 };

	int idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		printf ( "labeEntry/staticFuncs/_configure/TextIdx TCL ERROR\n" );
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case VariableIdx: {}

		case OnChangedIdx: {}

		case ValueIdx: {}

		case TooltipIdx: {}

		case NameIdx: {}

		case SizeGroupIdx: {}

		case VisibleIdx: {}

		case TextVariableIdx: {}

		case TextIdx:
			{
				printf ( "labeEntry/staticFuncs/_configure/TextIdx\n" );
				gtk_label_set_label ( GTK_WIDGET ( para->label ) , "OM MANI PADME HUM" );
			}

		case TextBaseFontIdx: {}

		case TextBaseColorIdx: {}

		default: {}
	}


	return TCL_OK;
}

/*****/


/****f* entry/staticFuncs/configure
 * AUTHOR
 *  PGM
 * SOURCE
 */
static int configure_entry (
	Tcl_Interp *interp,
	LabelEntryParams *para,
	GnoclOption options[] )
{

	printf ( "labeEntry/staticFuncs/configure\n" );

	/* Associate a variable with an object, update when either change */
	gnoclAttacheOptCmdAndVar (
		&options[onChangedIdx], &para->onChanged,
		&options[variableIdx], &para->variable,
		"changed", G_OBJECT ( para->entry ),
		G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	if ( options[variableIdx].status == GNOCL_STATUS_CHANGED
			&& options[valueIdx].status == 0  /* value is handled below */
			&& para->variable != NULL )
	{
		/* if variable does not exist -> set it, else set widget state */
		const char *val = Tcl_GetVar ( interp, para->variable, TCL_GLOBAL_ONLY );

		if ( val == NULL )
		{
			val = gtk_entry_get_text ( para->entry );
			setVariable ( para, val );
		}

		else
			setEntryVal ( para->entry, val );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		char *str = options[valueIdx].val.str;
		setEntryVal ( para->entry, str );
		setVariable ( para, str );
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
static int setTextVariable (
	LabelEntryParams *para,
	const char *val )
{
	printf ( "labelEntry/staticFuncs/setTextVariable\n" );

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
\author
\date
**/
static int configure_label (
	Tcl_Interp *interp,
	LabelEntryParams *para,
	GnoclOption options[] )
{
	printf ( "labelEntry/staticFuncs/configure_label\n" );

	gnoclAttacheOptCmdAndVar (
		&options[onChangedIdx], &para->onChanged,
		&options[textVariableIdx], &para->textVariable,
		"changed", G_OBJECT ( para->label ),
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
			setEntryVal ( para->label, val );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		char *str = options[valueIdx].val.str;
		setEntryVal ( para->label, str );
		setTextVariable ( para, str );
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
static int cget (
	Tcl_Interp *interp,
	LabelEntryParams *para,
	GnoclOption options[], int idx )
{
	printf ( "entry/staticFuncs/cget\n" );

	Tcl_Obj *obj = NULL;

	if ( idx == variableIdx )
	{
		obj = Tcl_NewStringObj ( para->variable, -1 );
	}

	else if ( idx == onChangedIdx )
	{
		obj = Tcl_NewStringObj ( para->onChanged ? para->onChanged : "", -1 );
	}

	else if ( idx == valueIdx )
	{
		obj = Tcl_NewStringObj ( gtk_entry_get_text ( para->entry ), -1 );
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
\author
\date
**/
static int labelEntryFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onChanged", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnChangedIdx, ClassIdx};

	LabelEntryParams *para = ( LabelEntryParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->entry );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "entry", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   labelEntryOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure_entry ( interp, para, labelEntryOptions );
				}

				gnoclClearOptions ( labelEntryOptions );

				return ret;
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->entry ), labelEntryOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, labelEntryOptions, idx );
				}
			}

		case OnChangedIdx:
			{
				const char *txt = gtk_entry_get_text ( para->entry );

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
\author
\date
**/
int gnoclLabelEntryCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{

	LabelEntryParams *para;
	int ret;

	if ( gnoclParseOptions ( interp, objc, objv, labelEntryOptions ) != TCL_OK )
	{
		gnoclClearOptions ( labelEntryOptions );
		return TCL_ERROR;
	}

	char    *A_VAL, *B_VAL, *C_VAL;

	gint idx, k, a, b, c;

	for ( k = 1; k < objc; k += 2 )
	{
		if ( gnoclGetIndexFromObjStruct (
					interp, objv[k],
					( char ** ) &labelEntryOptions[0].optName,
					sizeof ( GnoclOption ),
					"option",
					TCL_EXACT, &idx ) != TCL_OK )
		{
			return -1;
		}

		/* match and allocate specific options */

		if ( strcmp ( Tcl_GetString ( objv[k] ), "-text" ) == 0 )
		{
			A_VAL = Tcl_GetString ( objv[k+1] );
			a = 1;
		}

		if ( strcmp ( Tcl_GetString ( objv[k] ), "-textLength" ) == 0 )
		{
			B_VAL = Tcl_GetString ( objv[k+1] );
			b = 1;
		}

		if ( strcmp ( Tcl_GetString ( objv[k] ), "-justify" ) == 0 )
		{
			C_VAL = Tcl_GetString ( objv[k+1] );
			c = 1;
		}


	}


	para = g_new ( LabelEntryParams, 1 );




	/* create three objects
	 * 1) a box container to hold child objects
	 * 2) label widget
	 * 3) entry widget
	 */

	para->box = gtk_hbox_new ( 0, GNOCL_PAD );
	para->label.label = GTK_LABEL ( gtk_label_new ( NULL ) );
	para->entry.entry = GTK_ENTRY ( gtk_entry_new( ) );
	para->interp = interp;
	para->entry.variable = NULL;
	para->entry.onChanged = NULL;
	para->entry.inSetVar = 0;

	gtk_container_add ( GTK_CONTAINER ( para->box ), GTK_WIDGET ( para->label ) );
	gtk_container_add ( GTK_CONTAINER ( para->box ), GTK_WIDGET ( para->entry ) );
	gtk_widget_show_all ( GTK_WIDGET ( para->box ) );


	/* update the label */

	if ( a )
	{
		//gtk_label_set_label ( GTK_WIDGET ( para->label ) , A_VAL );
	}

	if ( b )
	{
		//gtk_label_set_width_chars ( GTK_WIDGET ( para->label ) , atoi(B_VAL) );
	}

	if ( c )
	{
		// int gnoclOptJustification ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
		//gnoclOptJustification ( interp, C_VAL, GTK_WIDGET ( para->label ), NULL );
	}

	/* set options for the child widgets */


	//ret = _configure ( interp, objv[1], para, labelEntryOptions );


	ret = gnoclSetOptions ( interp, labelEntryOptions, G_OBJECT ( para->label ), -1 );


	if ( ret == TCL_OK )
	{
		ret = configure_entry ( interp, para, labelEntryOptions );

		if ( 0 )
		{
			/* this is causing a crash */
			ret = configure_label ( interp, para, labelEntryOptions );
		}
	}

	gnoclClearOptions ( labelEntryOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( para->box ) );
		gtk_widget_destroy ( GTK_WIDGET ( para->entry ) );
		gtk_widget_destroy ( GTK_WIDGET ( para->label ) );
		g_free ( para );
		return TCL_ERROR;
	}

	para->name = gnoclGetAutoWidgetId();

	g_signal_connect ( G_OBJECT ( para->box ), "destroy", G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->box ) );

	Tcl_CreateObjCommand ( interp, para->name, labelEntryFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}
