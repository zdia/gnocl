/**
\brief      This module implements the gnocl::comboBox widget.
             It implements both forms, ie GtkComboBox and the GtkComboBoxEntry
\author    Peter G. Baum, William J Giddings
\date       2010-10   added -widthChars
            2009-06   added -entryWidth
                            -heightRequest
            2009-02:  added -widthRequest
            2008-11:  added -baseFont (WJG)
            2008-10:  added command, class (WJG)
            2008-05:  began -onActivate (WJG) -how to run a command when <CR> is pressed?
            2004-12:  Begin of developement
*/

/* user documentation */

/**
 \page page11 gnocl::comboBox
 \section sec gnocl::comboBox
  Implementation of gnocl::comboBox
  \htmlinclude ComboBox.html
 \\subsection subsection1 Implemented gnocl::comboBox Options
 \\subsection subsection2 Implemented gnocl::comboBox Commands

 \subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
 \subsection subsection4 Produces
 \image html "../pics/comboBox.png"
*/

/*
* $Id: comboBox.c,v 1.3 2005/02/22 23:16:09 baum Exp $
*
* This file implements the comboBox widget
*
* Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
*
* See the file "license.terms" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*
*/

/*
 * Problems: There is no activation event for the comboEntry entry widget.
 *
 * /


/*
   History:

*   2010-10   added -widthChars
*   2009-06   added -entryWidth
*                   -heightRequest
*   2009-02:  added -widthRequest
*   2008-11:  added -baseFont (WJG)
*   2008-10:  added command, class (WJG)
*   2008-05:  began -onActivate (WJG) -how to run a command when <CR> is pressed?
*   2004-12:  Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
\author     Peter G Baum
**/
static const int variableIdx  = 0;
static const int onChangedIdx = 1;
static const int itemsIdx     = 2;
static const int itemValueIdx = 3;
static const int valueIdx     = 4;
static const int widthCharsIdx   = 5;   /* WJG 18-02-09 */

static const int VALUE_COLUMN  = 0;
static const int TEXT_COLUMN   = 1;
static const int PIXMAP_COLUMN = 2;


/**
\author     Peter G Baum
**/
/* moved to goncl.h */

typedef struct
{
	char        *name;
	Tcl_Interp  *interp;
	GtkComboBox *comboBox;
	char        *onChanged;
	char        *variable;
	int         inSetVar;
} ComboParams;


/**
\brief     Set the overall width of the widget to accomodate an entry box equal
            in width to the number of pixels required to display a string of X
            characters of the specified length using the default theme font.
\notes      As the overall width of the widget included the downarrow, the requested
            width of the widget equals the string length in pixels + button width.
            This additional allowance is approx 30 pixels.
\author    William J Giddings
\date       12-June-09
\see        http://www.gtkforums.com/viewtopic.php?t=3495&highlight=width
*/
static int gnoclOptComboBoxEntryWidth ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	gint l;

	l = atoi ( Tcl_GetStringFromObj ( opt->val.obj, NULL ) );

	gtk_entry_set_width_chars ( GTK_BIN ( obj )->child, l );

	return TCL_OK;
}


/**
\author     Peter G Baum
**/
GnoclOption comboBoxOptions[] =
{
	/* specific widget properties */
	{ "-variable", GNOCL_STRING, NULL },                /* 0 */
	{ "-onChanged", GNOCL_STRING, NULL },               /* 1 */
	{ "-items", GNOCL_LIST, NULL },                     /* 2 */
	{ "-itemList", GNOCL_LIST, NULL },                  /* 3 */
	{ "-value", GNOCL_OBJ, NULL },                      /* 4 */

	{ "-entryWidth", GNOCL_OBJ, "", gnoclOptComboBoxEntryWidth }, /* 5 */

	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-onActivate", GNOCL_OBJ, "activate", gnoclOptCommand },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-baseFont", GNOCL_OBJ, "Sans 14", gnoclOptGdkBaseFont },

	/* inherited widget properties */
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightRequest", GNOCL_INT, "height-request" },
	{ "-widthRequest", GNOCL_INT, "width-request" },

	{ "-onKeyPress", GNOCL_OBJ, "", gnoclOptOnKeyPress },
	{ "-onKeyRelease", GNOCL_OBJ, "", gnoclOptOnKeyRelease },

	{ "-widthChars", GNOCL_INT, "width-chars" },

	{ NULL },
};


/**
\author     Peter G Baum
**/
static GtkEntry *getEntry ( GtkComboBox *comboBox )
{
	return GTK_ENTRY ( GTK_BIN ( comboBox )->child );
}

/**
\author     Peter G Baum
**/
static int findItemFromValue ( GtkComboBox *comboBox, const char *txt )
{
	GtkTreeModel *model = gtk_combo_box_get_model ( comboBox );
	int          row = -1;

	if ( model )
	{
		GtkTreeIter  iter;
		gboolean valid =  gtk_tree_model_get_iter_first ( model, &iter );

		while ( valid )
		{
			const char *val;
			++row;
			gtk_tree_model_get ( model, &iter, VALUE_COLUMN, &val, -1 );

			if ( strcmp ( txt, val ) == 0 )
				break;

			valid = gtk_tree_model_iter_next ( model, &iter );
		}
	}

	return row;
}

/**
\author     Peter G Baum
**/
static int setVariable ( ComboParams *para, const char *val )
{
	if ( para->variable && para->inSetVar == 0 )
	{
		const char *ret;
		para->inSetVar = 1;
		ret = Tcl_SetVar2 ( para->interp, para->variable, NULL,
							val, TCL_GLOBAL_ONLY );
		para->inSetVar = 0;

		return ret == NULL ? TCL_ERROR : TCL_OK;
	}

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
static int doCommand ( ComboParams *para, const char *val, int background )
{
	if ( para->onChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING }, /* widget */
			{ 'v', GNOCL_STRING }, /* value */
			{ 0 }
		};
		ps[0].val.str = para->name;
		ps[1].val.str = val;

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onChanged,
										  background );
	}

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
static const char *getActiveValue ( GtkComboBox *comboBox )
{
	const char *val = NULL;

	if ( GTK_IS_COMBO_BOX_ENTRY ( comboBox ) )
	{
		val = gtk_entry_get_text ( getEntry ( comboBox ) );
	}

	else
	{
		const int active  = gtk_combo_box_get_active ( comboBox );

		if ( active > -1 )
		{
			GtkTreePath  *path  = gtk_tree_path_new_from_indices ( active, -1 );
			GtkTreeModel *model = gtk_combo_box_get_model ( comboBox );
			GtkTreeIter  iter;
			gtk_tree_model_get_iter ( model, &iter, path );
			gtk_tree_path_free ( path );
			gtk_tree_model_get ( model, &iter, VALUE_COLUMN, &val, -1 );
		}
	}

	return val;
}

/**
\author     Peter G Baum
**/
static void changedFunc ( GtkWidget *widget, gpointer data )
{
	ComboParams *para = ( ComboParams * ) data;
	const char  *val = getActiveValue ( para->comboBox );

	if ( val )
	{
		setVariable ( para, val );
		doCommand ( para, val, 1 );
	}
}

/**
\author     Peter G Baum
**/
static GObject *getSigObj ( GtkComboBox *comboBox )
{
	return GTK_IS_COMBO_BOX_ENTRY ( comboBox )
		   ? G_OBJECT ( getEntry ( comboBox ) )
		   : G_OBJECT ( comboBox );
}

/**
\author     Peter G Baum
**/
static int setState ( ComboParams *para, const char *val )
{
	GObject *sigObj = getSigObj ( para->comboBox );
	int n = 0;
	int blocked = g_signal_handlers_block_matched (
					  sigObj, G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL, ( void * ) changedFunc, NULL );

	if ( GTK_IS_COMBO_BOX_ENTRY ( para->comboBox ) )
	{
		gtk_entry_set_text ( getEntry ( para->comboBox ), val );
	}

	else
	{
		n = findItemFromValue ( para->comboBox, val );

		if ( n >= 0 )
		{
			gtk_combo_box_set_active ( para->comboBox, n );
		}
	}

	if ( blocked )
	{
		g_signal_handlers_unblock_matched ( sigObj, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( void * ) changedFunc, NULL );
	}

	return n;
}

/**
\author     Peter G Baum
**/
static char *traceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	ComboParams *para = ( ComboParams * ) data;

	/* setValue should not invoke the trace function */

	if ( para->inSetVar == 0 && name1 )
	{
		const char *val = Tcl_GetVar2 ( interp, name1, name2, flags );

		if ( val )
		{
			setState ( para, val );
			doCommand ( para, val, 1 );
		}
	}

	return NULL;
}

/**
\author     Peter G Baum
**/
static void destroyFunc ( GtkWidget *widget, gpointer data )
{
	ComboParams *para = ( ComboParams * ) data;
	gnoclAttacheOptCmdAndVar (
		NULL, &para->onChanged,
		NULL, &para->variable,
		"changed", getSigObj ( para->comboBox ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );
	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	g_free ( para->variable );
	g_free ( para->onChanged );
	g_free ( para->name );
	g_free ( para );
}

/**
\author     Peter G Baum
**/
static int addItem ( GtkTreeModel *model, const char *val, const char *txt )
{
	GtkTreeIter iter;
	gtk_list_store_append ( GTK_LIST_STORE ( model ), &iter );
	gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
						 VALUE_COLUMN, val, TEXT_COLUMN, txt, -1 );

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
static int clearModel ( GtkComboBox *comboBox, GtkTreeModel *model )
{
	GObject *sigObj = getSigObj ( comboBox );
	int blocked = g_signal_handlers_block_matched ( sigObj, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( void * ) changedFunc, NULL );
	gtk_list_store_clear ( GTK_LIST_STORE ( model ) );

	if ( blocked )
	{
		g_signal_handlers_unblock_matched ( sigObj, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( void * ) changedFunc, NULL );
	}

	return 0;
}

/**
\author     Peter G Baum
**/
static int configure ( Tcl_Interp *interp, ComboParams *para, GnoclOption options[] )
{

#ifdef DEBUG_COMBO_BOX
	printf ( "******************** comboBox configure ********************\n" );
#endif

	int setToFirst = 0;
	GtkTreeModel *model = gtk_combo_box_get_model ( para->comboBox );

	/* set various custom options for these widgets */
	gnoclAttacheOptCmdAndVar (
		&options[onChangedIdx],
		&para->onChanged,
		&options[variableIdx],
		&para->variable,
		"changed",
		getSigObj ( para->comboBox ),
		G_CALLBACK ( changedFunc ),
		interp, traceFunc, para );

	if ( options[itemsIdx].status == GNOCL_STATUS_CHANGED )
	{
		int     k, no;
		Tcl_Obj *items = options[itemsIdx].val.obj;

		if ( options[itemValueIdx].status == GNOCL_STATUS_CHANGED )
		{
			Tcl_SetResult ( interp,
							"Either -items or -itemValueList may be given, but not both.",
							TCL_STATIC );
			return TCL_ERROR;
		}

		if ( Tcl_ListObjLength ( interp, items, &no ) != TCL_OK )
		{
			Tcl_SetResult ( interp, "items must be proper list",
							TCL_STATIC );
			return TCL_ERROR;
		}

		clearModel ( para->comboBox, model );

		for ( k = 0; k < no; ++k )
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, items, k, &tp ) != TCL_OK )
				return TCL_ERROR;

			addItem ( model, Tcl_GetString ( tp ), Tcl_GetString ( tp ) );
		}

		setToFirst = 1;
	}

	if ( options[itemValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		int     k, no;
		Tcl_Obj *items = options[itemValueIdx].val.obj;

		if ( Tcl_ListObjLength ( interp, items, &no ) != TCL_OK )
		{
			Tcl_SetResult ( interp, "itemValueList must be proper list",
							TCL_STATIC );
			return TCL_ERROR;
		}

		clearModel ( para->comboBox, model );

		for ( k = 0; k < no; ++k )
		{
			Tcl_Obj *tp, *txt, *val;

			if ( Tcl_ListObjIndex ( interp, items, k, &tp ) != TCL_OK )
				return TCL_ERROR;

			if ( Tcl_ListObjIndex ( interp, tp, 0, &txt ) != TCL_OK
					|| Tcl_ListObjIndex ( interp, tp, 1, &val ) != TCL_OK )
			{
				return TCL_ERROR;
			}

			addItem ( model, Tcl_GetString ( val ) , Tcl_GetString ( txt ) );
		}

		setToFirst = 1;
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		const char *val = Tcl_GetString ( options[valueIdx].val.obj );
		int n = setState ( para, val );

		if ( n < 0 )
		{
			Tcl_AppendResult ( interp, "Invalid value for option \"-value\" \"",
							   Tcl_GetString ( options[valueIdx].val.obj ), "\"", NULL );
			return TCL_ERROR;
		}

		setVariable ( para, val );

		setToFirst = 0;
	}

	else if ( options[variableIdx].status == GNOCL_STATUS_CHANGED
			  && para->variable != NULL )
	{
		const char *val = Tcl_GetVar2 ( para->interp, para->variable,
										NULL, TCL_GLOBAL_ONLY );

		if ( val != NULL )
		{
			setState ( para, val );
			setToFirst = 0;
		}

		else
		{
			setToFirst = 1;
		}
	}

	if ( setToFirst )
	{
		GtkTreeIter iter;

		if ( gtk_tree_model_get_iter_first ( model, &iter ) )
		{
			const char *val;
			gtk_tree_model_get ( model, &iter, VALUE_COLUMN, &val, -1 );
			setState ( para, val );
			setVariable ( para, val );
		}
	}

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
static int cget ( Tcl_Interp *interp, ComboParams *para, GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == variableIdx )
	{
		obj = Tcl_NewStringObj ( para->variable, -1 );
	}

	else if ( idx == onChangedIdx )
	{
		obj = Tcl_NewStringObj ( para->onChanged ? para->onChanged : "", -1 );
	}

	else if ( idx == itemsIdx )
	{
		GtkTreeModel *model = gtk_combo_box_get_model ( para->comboBox );
		obj = Tcl_NewListObj ( 0, NULL );

		if ( model )
		{
			GtkTreeIter  iter;
			gboolean valid =  gtk_tree_model_get_iter_first ( model, &iter );

			while ( valid )
			{
				const char *txt;
				gtk_tree_model_get ( model, &iter, TEXT_COLUMN, &txt, -1 );
				Tcl_ListObjAppendElement ( NULL, obj, Tcl_NewStringObj ( txt, -1 ) );
				valid = gtk_tree_model_iter_next ( model, &iter );
			}
		}
	}

	else if ( idx == itemValueIdx )
	{
		GtkTreeModel *model = gtk_combo_box_get_model ( para->comboBox );
		obj = Tcl_NewListObj ( 0, NULL );

		if ( model )
		{
			GtkTreeIter  iter;
			gboolean valid =  gtk_tree_model_get_iter_first ( model, &iter );

			while ( valid )
			{
				const char *val, *txt;
				Tcl_Obj *ll = Tcl_NewListObj ( 0, NULL );
				gtk_tree_model_get ( model, &iter, VALUE_COLUMN, &val,
									 TEXT_COLUMN, &txt, -1 );
				Tcl_ListObjAppendElement ( NULL, ll, Tcl_NewStringObj ( txt, -1 ) );
				Tcl_ListObjAppendElement ( NULL, ll, Tcl_NewStringObj ( val, -1 ) );
				Tcl_ListObjAppendElement ( NULL, obj, ll );

				valid = gtk_tree_model_iter_next ( model, &iter );
			}
		}
	}

	else if ( idx == valueIdx )
	{
		const char *val = getActiveValue ( para->comboBox );
		obj = Tcl_NewStringObj ( val ? val : "", -1 );
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}


	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\author     Peter G Baum
**/
static int addItemTcl ( ComboParams *para, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	GnoclOption addOption[] =
	{
		{ "-value", GNOCL_OBJ, NULL },    /* 0 */
		/* { "-icon", GNOCL_OBJ, NULL },     1 */
		{ NULL }
	};
	/* static const int addValueIdx = 0; */

	int ret;

	/* id add text ? opt ...? */

	if ( objc < 3 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, "text ?option val ...?" );
		return TCL_ERROR;
	}

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, addOption ) != TCL_OK )
	{
		gnoclClearOptions ( addOption );
		return TCL_ERROR;
	}

#if 0
	ret = addItem ( para, objv[2],
					addOption[addValueIdx].status == GNOCL_STATUS_CHANGED ?
					addOption[addValueIdx].val.obj : NULL );

#endif

	return ret;
}

/**
\author     Peter G Baum
**/
int comboBoxFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "add", "onChanged", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, AddIdx, DoOnChangedIdx, ClassIdx };

	ComboParams *para = ( ComboParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->comboBox );
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
			/* need to differentiate between comboBox and comboEntry */
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "comboBox", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   comboBoxOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, comboBoxOptions );
				}

				gnoclClearOptions ( comboBoxOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv,
									 G_OBJECT ( para->comboBox ), comboBoxOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, comboBoxOptions, idx );
				}
			}

		case AddIdx:
			return addItemTcl ( para, interp, objc, objv );
		case DoOnChangedIdx:
			{
				const char *val;

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				val = getActiveValue ( para->comboBox );

				if ( val )
					return doCommand ( para, val, 0 );

				return TCL_OK;
			}

			break;
	}

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
static int makeComboBox ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[], int isEntry )
{
	ComboParams     *para;
	GtkTreeModel    *model;
	int             ret;

	if ( gnoclParseOptions ( interp, objc, objv, comboBoxOptions ) != TCL_OK )
	{
		gnoclClearOptions ( comboBoxOptions );
		return TCL_ERROR;
	}

	model = GTK_TREE_MODEL ( gtk_list_store_new ( 3, G_TYPE_STRING,
							 G_TYPE_STRING, GDK_TYPE_PIXBUF ) );

	para = g_new ( ComboParams, 1 );
	para->interp = interp;
	para->onChanged = NULL;
	para->variable = NULL;
	para->name = gnoclGetAutoWidgetId();
	para->comboBox = GTK_COMBO_BOX (
						 isEntry ? gtk_combo_box_entry_new_with_model ( model, VALUE_COLUMN )
						 : gtk_combo_box_new_with_model ( model ) );
	para->inSetVar = 0;

	/* excluded these operations if object is simply a GtkComboBox */
	if ( isEntry == 0 )
	{
#ifdef DEBUG_COMBO_BOX
		printf ( "this one is for the plain box...............\n" );
#endif
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		gtk_cell_layout_pack_start ( GTK_CELL_LAYOUT ( para->comboBox ), renderer, TEXT_COLUMN );
		gtk_cell_layout_set_attributes ( GTK_CELL_LAYOUT ( para->comboBox ), renderer, "text", TEXT_COLUMN, NULL );
	}

	gtk_widget_show ( GTK_WIDGET ( para->comboBox ) );

	ret = gnoclSetOptions ( interp, comboBoxOptions, G_OBJECT ( para->comboBox ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, comboBoxOptions );

	gnoclClearOptions ( comboBoxOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( para->comboBox ) );
		g_free ( para );
		return TCL_ERROR;
	}

	g_signal_connect ( GTK_OBJECT ( para->comboBox ), "destroy",

					   G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->comboBox ) );

	Tcl_CreateObjCommand ( interp, para->name, comboBoxFunc, para, NULL );
	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

/**
\author     Peter G Baum
**/
int gnoclComboBoxCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	return makeComboBox ( interp, objc, objv, 0 );
}

/**
\author     Peter G Baum
**/
int gnoclComboEntryCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	return makeComboBox ( interp, objc, objv, 1 );
}

