/**
 \brief
  This module implements the gnocl::optionMenu widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page106 gnocl::optionMenu
  \section sec gnocl::optionMenu
  Implementation of gnocl::optionMenu
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
  \image html "../pics/optionMenu.png"
*/


/*
 * $Id: optionMenu.c,v 1.1 2005/01/01 15:28:49 baum Exp $
 *
 * This file implements the optionMenu widget
 *
 * Copyright (c) 2001 - 2004 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2004-12: replaced by comboBox, moved to deprecated
   2004-02: added -data
   2003-09: added cget
            removed getValue
        10: switched from GnoclWidgetOptions to GnoclOption
        05: updates for gtk 2.0
        02: renamed "invoke" to "eval"
   2002-01: new command "invoke"
   2001-10: Begin of developement
 */

#include "../gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption optionMenuOptions[] =
{
	{ "-variable", GNOCL_STRING, NULL },         /* 0 */
	{ "-onChanged", GNOCL_STRING, NULL },        /* 1 */
	{ "-items", GNOCL_LIST, NULL },              /* 2 */
	{ "-itemValueList", GNOCL_LIST, NULL },      /* 3 */
	{ "-value", GNOCL_OBJ, NULL },               /* 4 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	/* -onShowHelp and -onPopupMenu seem not to work with GTK+ 2.0.6 */
	{ NULL },
};

static const int variableIdx  = 0;
static const int onChangedIdx = 1;
static const int itemsIdx     = 2;
static const int itemValueIdx = 3;
static const int valueIdx     = 4;

typedef struct
{
	char          *name;
	Tcl_Interp    *interp;
	GtkOptionMenu *optionMenu;
	char          *onChanged;
	char          *variable;
	GSList        *items;
	GSList        *group;
	int           inSetVar;
} OptionMenuParams;

typedef struct
{
	GtkMenuItem      *item;
	Tcl_Obj          *val;
	OptionMenuParams *omp;
} MenuItemParam;

static void changedFunc ( GtkWidget *widget, gpointer data );
static int setState ( OptionMenuParams *para, Tcl_Obj *obj );
static int doCommand ( OptionMenuParams *para, Tcl_Obj *val, int background );

static int deleteItems ( OptionMenuParams *para )
{
	GtkWidget *menuWidget = gtk_option_menu_get_menu ( para->optionMenu );
	GSList *p = para->items;

	if ( p == NULL )
		return 0;

	for ( ; p != NULL; p = p->next )
	{
		MenuItemParam *mip = ( MenuItemParam * ) p->data;
		Tcl_DecrRefCount ( mip->val );
		gtk_container_remove ( GTK_CONTAINER ( menuWidget ),
							   GTK_WIDGET ( mip->item ) );
		g_free ( mip );
	}

	g_slist_free ( para->items );

	para->items = NULL;

	return 1;
}

static char *traceFunc ( ClientData data,
						 Tcl_Interp *interp, const char *name1, const char *name2, int flags )
{
	OptionMenuParams *para = ( OptionMenuParams * ) data;

	/* setValue should not invoke the trace function */

	if ( para->inSetVar == 0 && name1 )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( interp, name1, name2, flags );

		if ( val )
		{
			setState ( para, val );
			doCommand ( para, val, 1 );
		}
	}

	return NULL;
}

static void destroyFunc ( GtkWidget *widget, gpointer data )
{
	OptionMenuParams *para = ( OptionMenuParams * ) data;

	gnoclAttacheOptCmdAndVar (
		NULL, &para->onChanged,
		NULL, &para->variable,
		"changed", G_OBJECT ( para->optionMenu ),
		G_CALLBACK ( changedFunc ), para->interp, traceFunc, para );

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	deleteItems ( para );
	g_free ( para->variable );
	g_free ( para->onChanged );
	g_free ( para->name );
	g_free ( para );
}

static MenuItemParam *findActiveItem ( OptionMenuParams *para )
{
	GSList    *p = para->items;
	GtkMenu   *menu = GTK_MENU ( gtk_option_menu_get_menu ( para->optionMenu ) );
	GtkWidget *active = gtk_menu_get_active ( menu );
	int n = 0;

	for ( ; p != NULL; ++n, p = p->next )
	{
		MenuItemParam *mip = ( MenuItemParam * ) p->data;

		if ( GTK_WIDGET ( mip->item ) == active )
			return mip;
	}

	assert ( 0 );

	return NULL;
}

static int findItemFromValue ( OptionMenuParams *para, Tcl_Obj *obj,
							   MenuItemParam **item )
{
	if ( obj != NULL )
	{
		GSList *p = para->items;
		/* TODO: compare int, if possible */
		char *txt = Tcl_GetString ( obj );
		int n = 0;

		for ( ; p != NULL; ++n, p = p->next )
		{
			MenuItemParam *mip = ( MenuItemParam * ) p->data;

			if ( obj == mip->val || strcmp ( txt, Tcl_GetString ( mip->val ) ) == 0 )
			{
				if ( item )
					*item = mip;

				return n;
			}
		}
	}

	return -1;
}

static int setState ( OptionMenuParams *para, Tcl_Obj *obj )
{
	MenuItemParam *mip;
	int n = findItemFromValue ( para, obj, &mip );

	if ( n >= 0 )
	{
		int blocked = g_signal_handlers_block_matched (
						  G_OBJECT ( para->optionMenu ), G_SIGNAL_MATCH_FUNC,
						  0, 0, NULL, ( void * ) changedFunc, NULL );

		gtk_option_menu_set_history ( para->optionMenu, n );
		g_object_set ( G_OBJECT ( mip->item ), "active", ( gboolean ) 1, NULL );

		if ( blocked )
			g_signal_handlers_unblock_matched (
				G_OBJECT ( para->optionMenu ), G_SIGNAL_MATCH_FUNC,
				0, 0, NULL, ( void * ) changedFunc, NULL );
	}

	return n;
}

static int doCommand ( OptionMenuParams *para, Tcl_Obj *val, int background )
{
	if ( para->onChanged )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING }, /* widget */
			{ 'v', GNOCL_OBJ },    /* value */
			{ 0 }
		};
		ps[0].val.str = para->name;
		ps[1].val.obj = val;

		return gnoclPercentSubstAndEval ( para->interp, ps, para->onChanged,
										  background );
	}

	return TCL_OK;
}

static int setVariable ( OptionMenuParams *para, Tcl_Obj *obj )
{
	if ( para->variable && para->inSetVar == 0 )
	{
		Tcl_Obj *ret;
		para->inSetVar = 1;
		ret = Tcl_SetVar2Ex ( para->interp, para->variable, NULL,
							  obj, TCL_GLOBAL_ONLY );
		para->inSetVar = 0;

		return ret == NULL ? TCL_ERROR : TCL_OK;
	}

	return TCL_OK;
}

static void changedFunc ( GtkWidget *widget, gpointer data )
{
	OptionMenuParams *para = ( OptionMenuParams * ) data;
	MenuItemParam *mip = findActiveItem ( para );
	setVariable ( para, mip->val );
	doCommand ( para, mip->val, 1 );
}

#if 0
/* TODO: set menu to the width of the option menu?
         But how to get the current width of the option menu?
*/
static void menuShowFunc ( GtkWidget *widget, gpointer data )
{
	OptionMenuParams *para = ( OptionMenuParams * ) data;
	GtkRequisition menuReq, butReq;
	/*
	gint menuWidth, buttonWidth, height;
	gtk_widget_get_size_request( widget, &menuWidth, &height );
	gtk_widget_get_size_request( GTK_WIDGET( para->optionMenu ), &buttonWidth,
	      &height );
	*/
	gtk_widget_size_request ( widget, &menuReq );
	gtk_widget_size_request ( GTK_WIDGET ( para->optionMenu ), &butReq );

	printf ( "%d %d\n", menuReq.width, butReq.width );

	if ( menuReq.width < butReq.width )
		gtk_widget_set_size_request ( widget, butReq.width, -1 );

	/*
	g_object_set( G_OBJECT( widget ), "default-width", menuReq.width, NULL );
	*/
}

#endif


static int addItem ( OptionMenuParams *para, Tcl_Obj *txt, Tcl_Obj *value )
{
	MenuItemParam  *mPara = g_new ( MenuItemParam, 1 );
	GtkWidget *menuWidget = gtk_option_menu_get_menu ( para->optionMenu );
	GtkMenu *menu = GTK_MENU ( menuWidget ? menuWidget : gtk_menu_new( ) );

	/* if no value is given, we use the txt as value */

	if ( value == NULL )
		value = txt;

	mPara->omp = para;

	mPara->val = value;

	Tcl_IncrRefCount ( value );

	/* mPara->item = GTK_MENU_ITEM( gtk_menu_item_new_with_label(
	         gnoclGetString( txt ) ) );
	*/
	mPara->item = GTK_MENU_ITEM ( gtk_radio_menu_item_new_with_label (
									  para->group, gnoclGetString ( txt ) ) );

	para->group = gtk_radio_menu_item_get_group (
					  GTK_RADIO_MENU_ITEM ( mPara->item ) );

	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), GTK_WIDGET ( mPara->item ) );

	gtk_widget_show ( GTK_WIDGET ( mPara->item ) );

	para->items = g_slist_append ( para->items, mPara );

	/* insert new menu and set value to first entry */
	if ( menuWidget == NULL )
	{
		/* gtk_option_menu_set_menu emits "changed"! */
		int blocked = g_signal_handlers_block_matched (
						  G_OBJECT ( para->optionMenu ), G_SIGNAL_MATCH_FUNC,
						  0, 0, NULL, ( void * ) changedFunc, NULL );
		gtk_option_menu_set_menu ( para->optionMenu, GTK_WIDGET ( menu ) );

		if ( blocked )
			g_signal_handlers_unblock_matched (
				G_OBJECT ( para->optionMenu ), G_SIGNAL_MATCH_FUNC,
				0, 0, NULL, ( void * ) changedFunc, NULL );

		/*
		g_signal_connect( GTK_OBJECT( menu ), "map",
		      G_CALLBACK( menuShowFunc ), para );
		*/
	}

	return TCL_OK;
}

static int configure ( Tcl_Interp *interp, OptionMenuParams *para,
					   GnoclOption options[] )
{
	int setToFirst = 0;

	gnoclAttacheOptCmdAndVar (
		&options[onChangedIdx], &para->onChanged,
		&options[variableIdx], &para->variable,
		"changed", G_OBJECT ( para->optionMenu ),
		G_CALLBACK ( changedFunc ), interp, traceFunc, para );

	if ( options[itemsIdx].status == GNOCL_STATUS_CHANGED )
	{
		int k, no;
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

		deleteItems ( para );

		for ( k = 0; k < no; ++k )
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, items, k, &tp ) != TCL_OK )
				return TCL_ERROR;

			addItem ( para, tp, NULL );
		}
	}

	if ( options[itemValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		int k, no;
		Tcl_Obj *items = options[itemValueIdx].val.obj;

		if ( Tcl_ListObjLength ( interp, items, &no ) != TCL_OK )
		{
			Tcl_SetResult ( interp, "itemValueList must be proper list",
							TCL_STATIC );
			return TCL_ERROR;
		}

		deleteItems ( para );

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

			addItem ( para, txt, val );
		}
	}

	if ( options[valueIdx].status == GNOCL_STATUS_CHANGED )
	{
		int n = setState ( para, options[valueIdx].val.obj );

		if ( n < 0 )
		{
			Tcl_AppendResult ( interp, "Invalid value for option \"-value\" \"",
							   Tcl_GetString ( options[valueIdx].val.obj ), "\"", NULL );
			return TCL_ERROR;
		}

		setVariable ( para, options[valueIdx].val.obj );
	}

	else if ( options[variableIdx].status == GNOCL_STATUS_CHANGED
			  && para->variable != NULL )
	{
		Tcl_Obj *val = Tcl_GetVar2Ex ( para->interp, para->variable,
									   NULL, TCL_GLOBAL_ONLY );

		if ( val != NULL )
			setState ( para, val );
		else
			setToFirst = 1;
	}

	else if ( options[itemValueIdx].status == GNOCL_STATUS_CHANGED
			  || options[itemsIdx].status == GNOCL_STATUS_CHANGED )
	{
		setToFirst = 1;
	}

	if ( setToFirst && para->items != NULL )
	{
		MenuItemParam *mip = ( MenuItemParam * ) para->items->data;
		setState ( para, mip->val );
		setVariable ( para, mip->val );
	}

	return TCL_OK;
}

static int cget ( Tcl_Interp *interp, OptionMenuParams *para,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == variableIdx )
		obj = Tcl_NewStringObj ( para->variable, -1 );
	else if ( idx == onChangedIdx )
		obj = Tcl_NewStringObj ( para->onChanged ? para->onChanged : "", -1 );
	else if ( idx == itemsIdx )
	{
		GSList *p = para->items;
		obj = Tcl_NewListObj ( 0, NULL );

		for ( ; p != NULL; p = p->next )
		{
			MenuItemParam *mip = ( MenuItemParam * ) p->data;
			GtkWidget *widget = gnoclFindChild ( GTK_WIDGET ( mip->item ),
												 GTK_TYPE_LABEL );
			const char *txt;
			/* the selected item is reparented, so look directly at optionMenu */

			if ( widget == NULL )
			{
				widget = gnoclFindChild ( GTK_WIDGET ( para->optionMenu ),
										  GTK_TYPE_LABEL );
			}

			txt = gtk_label_get_text ( GTK_LABEL ( widget ) );

			Tcl_ListObjAppendElement ( NULL, obj, Tcl_NewStringObj ( txt, -1 ) );
		}
	}

	else if ( idx == itemValueIdx )
	{
		GSList *p = para->items;
		obj = Tcl_NewListObj ( 0, NULL );

		for ( ; p != NULL; p = p->next )
		{
			MenuItemParam *mip = ( MenuItemParam * ) p->data;
			Tcl_Obj *ll = Tcl_NewListObj ( 0, NULL );
			GtkWidget *widget = gnoclFindChild ( GTK_WIDGET ( mip->item ),
												 GTK_TYPE_LABEL );
			const char *txt;
			/* the selected item is reparented, so look directly at optionMenu */

			if ( widget == NULL )
			{
				widget = gnoclFindChild ( GTK_WIDGET ( para->optionMenu ),
										  GTK_TYPE_LABEL );
			}

			txt = gtk_label_get_text ( GTK_LABEL ( widget ) );

			Tcl_ListObjAppendElement ( NULL, ll, Tcl_NewStringObj ( txt, -1 ) );
			Tcl_ListObjAppendElement ( NULL, ll, mip->val );
			Tcl_ListObjAppendElement ( NULL, obj, ll );
		}
	}

	else if ( idx == valueIdx )
	{
		MenuItemParam *mip = findActiveItem ( para );
		obj = mip->val;
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

static int addItemTcl ( OptionMenuParams *para, Tcl_Interp *interp,
						int objc, Tcl_Obj * const objv[] )
{
	GnoclOption addOption[] =
	{
		{ "-value", GNOCL_OBJ, NULL },    /* 0 */
		/* { "-icon", GNOCL_OBJ, NULL },     1 */
		{ NULL }
	};
	static const int addValueIdx = 0;

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

	ret = addItem ( para, objv[2],

					addOption[addValueIdx].status == GNOCL_STATUS_CHANGED ?
					addOption[addValueIdx].val.obj : NULL );

	return ret;
}

static int optionMenuFunc ( ClientData data, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget",
								  "add", "onChanged", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx,
				  AddIdx, DoOnChangedIdx
				};

	OptionMenuParams *para = ( OptionMenuParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->optionMenu );
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
											   optionMenuOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, para, optionMenuOptions );
				}

				gnoclClearOptions ( optionMenuOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv,
									 G_OBJECT ( para->optionMenu ), optionMenuOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, para, optionMenuOptions, idx );
				}
			}

		case AddIdx:
			return addItemTcl ( para, interp, objc, objv );
		case DoOnChangedIdx:
			{
				MenuItemParam *mip;

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				mip = findActiveItem ( para );

				if ( mip )
					return doCommand ( para, mip->val, 0 );

				return TCL_OK;
			}

			break;
	}

	return TCL_OK;
}

int gnoclOptionMenuCmd ( ClientData data, Tcl_Interp *interp,
						 int objc, Tcl_Obj * const objv[] )
{
	OptionMenuParams *para;
	int          ret;

	if ( gnoclParseOptions ( interp, objc, objv, optionMenuOptions ) != TCL_OK )
	{
		gnoclClearOptions ( optionMenuOptions );
		return TCL_ERROR;
	}

	para = g_new ( OptionMenuParams, 1 );

	para->interp = interp;
	para->onChanged = NULL;
	para->variable = NULL;
	para->name = gnoclGetAutoWidgetId();
	para->optionMenu = GTK_OPTION_MENU ( gtk_option_menu_new() );
	para->items = NULL;
	para->group = NULL;
	para->inSetVar = 0;

	gtk_widget_show ( GTK_WIDGET ( para->optionMenu ) );

	ret = gnoclSetOptions ( interp, optionMenuOptions,
							G_OBJECT ( para->optionMenu ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, para, optionMenuOptions );

	gnoclClearOptions ( optionMenuOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( para->optionMenu ) );
		g_free ( para );
		return TCL_ERROR;
	}

	g_signal_connect ( GTK_OBJECT ( para->optionMenu ), "destroy",

					   G_CALLBACK ( destroyFunc ), para );

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->optionMenu ) );

	Tcl_CreateObjCommand ( interp, para->name, optionMenuFunc, para, NULL );
	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );

	return TCL_OK;
}

