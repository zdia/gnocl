/*
 * $Id: combo.c,v 1.1 2005/01/01 15:28:48 baum Exp $
 *
 * This file implements the combo widget
 *
 * Copyright (c) 2001 - 2004 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2004-12: replaced by comboEntry, moved to deprecated
   2004-02: added -data
        09: added cget
            removed getValue
        07: added -onChanged, -value
   2003-06: block callback function if setting the text
   2002-10: switched from GnoclWidgetOptions to GnoclOption
        09: new command getValue, setValue
   2001-03: Begin of developement
 */

#include "../gnocl.h"
#include <string.h>
#include <assert.h>


/* moved to gnocl.h */

typedef struct
{
	GtkCombo   *combo;
	Tcl_Interp *interp;
	char       *name;
	char       *variable;
	char       *onChanged;
	int        inSetVar;
} ComboParams;


static GnoclOption comboOptions[] =
{
	{ "-variable", GNOCL_STRING, NULL },         /* 0 */
	{ "-onChanged", GNOCL_STRING, NULL },        /* 1 */
	{ "-items", GNOCL_LIST, NULL },              /* 2 */
	{ "-value", GNOCL_STRING, NULL },            /* 3 */
	{ "-tooltip", GNOCL_OBJ, NULL },             /* 4 */
	{ "-editable", GNOCL_BOOL, NULL },           /* 5 */
	{ "-allowEmpty", GNOCL_BOOL, "allow-empty" },
	{ "-caseSensitive", GNOCL_BOOL, "case-sensitive" },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-enableArrowKeys", GNOCL_BOOL, "enable-arrow-keys" },
	{ "-enableArrowsAlways", GNOCL_BOOL, "enable-arrows-always" },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-mustMatch", GNOCL_BOOL, "value-in-list" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};

static const int variableIdx  = 0;
static const int onChangedIdx = 1;
static const int itemsIdx     = 2;
static const int valueIdx     = 3;
static const int tooltipIdx   = 4;
static const int editableIdx  = 5;

static int setVal ( GtkEntry *entry, const char *txt );
static void changedFunc ( GtkWidget *widget, gpointer data );

/* this is copied from entry.c */
static int doCommand ( ComboParams *para, const char *val, int background )
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

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onChanged,
										  background );
	}

	return TCL_OK;
}

static int setVal ( GtkEntry *entry, const char *txt )
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

static char *traceFunc ( ClientData data,
						 Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	ComboParams *para = ( ComboParams * ) data;

	/* FIXME: with GTK 2.2.1 is this called 2 times: ones with the empty
	   string, and another time with the selected item. Why? How to prevent?
	*/

	if ( para->inSetVar == 0 && name1 )
	{
		const char *txt = name1 ? Tcl_GetVar2 ( interp, name1, name2, 0 ) : NULL;

		if ( txt )
		{
			GtkEntry  *entry = GTK_ENTRY ( para->combo->entry );
			setVal ( entry, txt );
			doCommand ( para, txt, 1 );
		}
	}

	return NULL;
}

static int setVariable ( ComboParams *para, const char *val )
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

static void changedFunc ( GtkWidget *widget, gpointer data )
{
	ComboParams *para = ( ComboParams * ) data;
	GtkEntry    *entry = GTK_ENTRY ( para->combo->entry );
	const char *val = gtk_entry_get_text ( entry );
	setVariable ( para, val );
	doCommand ( para, val, 1 );
}

static void destroyFunc ( GtkWidget *widget, gpointer data )
{
	ComboParams *para = ( ComboParams * ) data;

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	gnoclAttacheOptCmdAndVar (
		NULL, &para->onChanged,
		NULL, &para->variable,
		"changed", G_OBJECT ( para->combo->entry ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );

	g_free ( para->variable );
	g_free ( para->name );
	g_free ( para );
}

static void removeAll ( GtkWidget *widget, gpointer data )
{
	gtk_container_remove ( GTK_CONTAINER ( data ), widget );
}

static int configure ( Tcl_Interp *interp, ComboParams *para,
					   GnoclOption options[] )
{
	GtkEntry *entry = GTK_ENTRY ( para->combo->entry );

	if ( options[itemsIdx].status == GNOCL_STATUS_CHANGED )
	{
		Tcl_Obj *strings = options[itemsIdx].val.obj;
		int no;

		if ( Tcl_ListObjLength ( interp, strings, &no ) != TCL_OK )
			return TCL_ERROR;

		if ( no == 0 )
		{
			gtk_container_foreach ( GTK_CONTAINER ( para->combo->list ),
									removeAll, GTK_CONTAINER ( para->combo->list ) );
		}

		else
		{
			int n;
			GList *items = NULL;

			for ( n = 0; n < no; ++n )
			{
				Tcl_Obj *tp;
				int ret = Tcl_ListObjIndex ( interp, strings, n, &tp );

				if ( ret != TCL_OK )
					return ret;

				items = g_list_append ( items,
										( char * ) gnoclGetStringFromObj ( tp, NULL ) );
			}

			gtk_combo_set_popdown_strings ( para->combo, items );
		}
	}

	gnoclAttacheOptCmdAndVar (

		&options[onChangedIdx], &para->onChanged,
		&options[variableIdx], &para->variable,
		"changed", G_OBJECT ( entry ),
		G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	if ( options[variableIdx].status == GNOCL_STATUS_CHANGED
			&& options[valueIdx].status == 0  /* value is handled below */
			&& para->variable != NULL )
	{
		/* if variable does not exist -> set it, else set widget state */
		const char *val = Tcl_GetVar ( interp, para->variable, TCL_GLOBAL_ONLY );

		if ( val == NULL )
		{
			val = gtk_entry_get_text ( entry );
			setVariable ( para, val );
		}

		else
			setVal ( entry, val );
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		char *str = options[valueIdx].val.str;
		setVal ( entry, str );
		setVariable ( para, str );
	}

	/* gnoclOptTooltip does not work since the tooltip must be
	   associated to the entry, not the combo.
	   I think this is a BUG in GTK 2.0.6
	*/

	if ( options[tooltipIdx].status == GNOCL_STATUS_CHANGED )
		gnoclOptTooltip ( interp, &options[tooltipIdx], G_OBJECT ( entry ), NULL );

	if ( options[editableIdx].status == GNOCL_STATUS_CHANGED )
		g_object_set ( G_OBJECT ( entry ), "editable", options[editableIdx].val.b,
					   NULL );

	return TCL_OK;
}

static void getAllItems ( GtkWidget *widget, gpointer data )
{
	Tcl_Obj *resList = ( Tcl_Obj * ) data;
	GtkLabel *label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( widget ),
								  GTK_TYPE_LABEL ) );
	Tcl_Obj *obj = Tcl_NewStringObj ( gtk_label_get_text ( label ), -1 );
	Tcl_ListObjAppendElement ( NULL, resList, obj );
}

static int cget ( Tcl_Interp *interp, ComboParams *para,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;
	GtkEntry *entry = GTK_ENTRY ( para->combo->entry );

	if ( idx == variableIdx )
		obj = Tcl_NewStringObj ( para->variable, -1 );
	else if ( idx == onChangedIdx )
		obj = Tcl_NewStringObj ( para->onChanged ? para->onChanged : "", -1 );
	else if ( idx == itemsIdx )
	{
		obj = Tcl_NewListObj ( 0, NULL );
		gtk_container_foreach ( GTK_CONTAINER ( para->combo->list ),
								getAllItems, obj );
	}

	else if ( idx == valueIdx )
		obj = Tcl_NewStringObj ( gtk_entry_get_text ( entry ), -1 );
	else if ( idx == tooltipIdx )
		gnoclOptTooltip ( interp, &options[tooltipIdx], G_OBJECT ( entry ), &obj );
	else if ( idx == editableIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( entry ), "editable", &on, NULL );
		obj = Tcl_NewBooleanObj ( on );
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}


static int comboFunc ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget",
								  "onChanged", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx,
				  OnChangedIdx
				};

	ComboParams *para = ( ComboParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->combo );
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
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   comboOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, comboOptions );
				}

				gnoclClearOptions ( comboOptions );

				return ret;
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->combo ),
									 comboOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, comboOptions, idx );
				}
			}

		case OnChangedIdx:
			{
				GtkEntry *entry = GTK_ENTRY ( para->combo->entry );
				const char *txt = gtk_entry_get_text ( entry );

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

int gnoclComboCmd ( ClientData data, Tcl_Interp *interp,
					int objc, Tcl_Obj * const objv[] )
{
	ComboParams *para;
	int ret;

	if ( gnoclParseOptions ( interp, objc, objv, comboOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( comboOptions );
		return TCL_ERROR;
	}

	para = g_new ( ComboParams, 1 );

	para->interp = interp;
	para->combo = GTK_COMBO ( gtk_combo_new( ) );
	para->variable = NULL;
	para->onChanged = NULL;
	para->inSetVar = 0;

	ret = gnoclSetOptions ( interp, comboOptions,
							G_OBJECT ( para->combo ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, comboOptions );

	gnoclClearOptions ( comboOptions );

	if ( ret != TCL_OK )
	{
		g_free ( para );
		gtk_widget_destroy ( GTK_WIDGET ( para->combo ) );
		return TCL_ERROR;
	}

	para->name = gnoclGetAutoWidgetId();

	g_signal_connect ( G_OBJECT ( para->combo ), "destroy",
					   G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->combo ) );
	gtk_widget_show ( GTK_WIDGET ( para->combo ) );

	Tcl_CreateObjCommand ( interp, para->name, comboFunc, para, NULL );

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

