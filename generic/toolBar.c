/**
 \brief
  This module implements the gnocl::toolBar widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
 \bug
    Toobar item -onClicked can cause a crash, returning error code 139.
*/

/* user documentation */

/**
  \page page48 gnocl::toolBar
  \section sec gnocl::toolBar
  Implementation of gnocl::toolBar
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
  \image html "../pics/toolBar.png"
*/
/*
 * $Id: toolBar.c,v 1.13 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements the toolbar widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-11  renamed static buttonFunc to non-static toolButtonFunc to
            enable use in parsing glade files
   2008-10: added command, class
   2007-12: propose addition of MenuToolButton
            http://www.gtk.org/api/2.6/gtk/GtkMenuToolButton.html
            change of term 'item' to button
            use command menuButton

   2004-02: added -data for radio and check item
   2003-09: changed -value to -onValue for radio item
            added cget to radio and check item
        12: switched from GnoclWidgetOptions to GnoclOption
        05: updates for gtk 2.0
   2002-02: percent subst. in command
            added STD_OPTIONS
        12: icon text qualified by "%#" or "%/"
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include "gnoclparams.h"

#include <string.h>
#include <assert.h>

/* static function declarations */
static int optOrientation ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int optStyle ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static GnoclOption toolBarOptions[] =
{
	{ "-orientation", GNOCL_OBJ, "orientation", optOrientation },
	{ "-style", GNOCL_OBJ, "toolbar-style", optStyle },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-name", GNOCL_STRING, "name" },
	{ NULL, 0, 0 }
};
/**
\brief Description yet to be added.
\author Peter G Baum
*/
GnoclOption toolButtonOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },
	{ "-icon", GNOCL_OBJ, NULL },
	{ "-onClicked", GNOCL_STRING, NULL },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-name", GNOCL_STRING, "name" },
	{ "-label", GNOCL_STRING, "label" },
	{ NULL, 0, 0 }
};

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static const int buttonTextIdx      = 0;
static const int buttonIconIdx      = 1;
static const int buttonOnClickedIdx = 2;

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static GnoclOption checkOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },           /* 0 */
	{ "-icon", GNOCL_OBJ, NULL },           /* 1 */
	{ "-onToggled", GNOCL_STRING, NULL },   /* 2 the order is important! */
	{ "-onValue", GNOCL_OBJ, NULL },        /* 3 */
	{ "-offValue", GNOCL_OBJ, NULL },       /* 4 */
	{ "-variable", GNOCL_STRING, NULL },    /* 5 */
	{ "-active", GNOCL_BOOL, NULL },        /* 6 */
	{ "-value", GNOCL_OBJ, NULL },          /* 7 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL, 0, 0 }
};
/*****/

/***d* toolBar/constands
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int checkTextIdx      = 0;
static const int checkIconIdx      = 1;
static const int checkOnToggledIdx = 2;
static const int checkOnValueIdx   = 3;
static const int checkOffValueIdx  = 4;
static const int checkVariableIdx  = 5;
static const int checkActiveIdx    = 6;
static const int checkValueIdx     = 7;
/*****/

/***v* toolBar/radioOptions
 * AUTHOR
 *  PGB
 * SOURCE
 */
static GnoclOption radioOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },           /* 0 */
	{ "-icon", GNOCL_OBJ, NULL },           /* 1 */
	{ "-onValue", GNOCL_OBJ, NULL },        /* 2 */
	{ "-variable", GNOCL_STRING, NULL },    /* 3 */
	{ "-active", GNOCL_BOOL, NULL },        /* 4 the order is important! */
	{ "-onToggled", GNOCL_STRING, NULL },   /* 5 */
	{ "-value", GNOCL_OBJ, NULL },          /* 6 */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL, 0, 0 }
};
/*****/

/***d* toolBar/constands
 * AUTHOR
 *  PGB
 * SOURCE
 */
static const int radioTextIdx      = 0;
static const int radioIconIdx      = 1;
static const int radioOnValueIdx   = 2;
static const int radioVariableIdx  = 3;
static const int radioActiveIdx    = 4;
static const int radioOnToggledIdx = 5;
static const int radioValueIdx     = 6;
/**
\brief Description yet to be added.
\author Peter G Baum
*/
/* moved to gnocl.h */
/*
typedef struct
{
    GtkWidget  *item;
    char       *name;
    char       *onClicked;
    Tcl_Interp *interp;
} ToolButtonParams;
*/

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int optOrientation ( Tcl_Interp *interp, GnoclOption *opt,
							GObject *obj, Tcl_Obj **ret )
{
	const char *txt[] = { "horizontal", "vertical", NULL };
	const int types[] = { GTK_ORIENTATION_HORIZONTAL,
						  GTK_ORIENTATION_VERTICAL
						};

	assert ( sizeof ( GTK_ORIENTATION_VERTICAL ) == sizeof ( int ) );

	return gnoclOptGeneric ( interp, opt, obj, "orientation", txt, types, ret );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int optStyle ( Tcl_Interp *interp, GnoclOption *opt,
					  GObject *obj, Tcl_Obj **ret )
{
	const char *txt[] = { "icons", "text", "both", NULL };
	const int types[] = { GTK_TOOLBAR_ICONS, GTK_TOOLBAR_TEXT,
						  GTK_TOOLBAR_BOTH
						};

	assert ( sizeof ( GTK_TOOLBAR_ICONS ) == sizeof ( int ) );

	return gnoclOptGeneric ( interp, opt, obj, "style", txt, types, ret );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
int getTextAndIcon ( Tcl_Interp *interp, GtkToolbar *toolbar,
					 GnoclOption *txtOpt, GnoclOption *iconOpt,
					 char **txt, GtkWidget **icon, int *isUnderline )
{
	*icon = NULL;
	*txt = NULL;
	*isUnderline = 0;

	if ( txtOpt->status == GNOCL_STATUS_CHANGED )
	{
		GnoclStringType type = gnoclGetStringType ( txtOpt->val.obj );

		if ( type & GNOCL_STR_STOCK )
		{
			GtkStockItem item;

			if ( iconOpt->status == GNOCL_STATUS_CHANGED )
			{
				Tcl_SetResult ( interp,
								"If option -text ist set, -icon shall not be set.",
								TCL_STATIC );
				return TCL_ERROR;
			}

			if ( gnoclGetStockItem ( txtOpt->val.obj, interp, &item ) != TCL_OK )
				return TCL_ERROR;

			/* gnoclGetStockName checks gtk_stock_lookup,
			   it will therefore always succeed */
			*icon = gtk_image_new_from_stock ( item.stock_id,
											   gtk_toolbar_get_icon_size ( toolbar ) );

			*txt = g_strdup ( item.label );

			*isUnderline = 1;
		}

		else
			*txt = gnoclGetString ( txtOpt->val.obj );

		if ( type & GNOCL_STR_UNDERLINE )
			*isUnderline = 1;
	}

	if ( iconOpt->status == GNOCL_STATUS_CHANGED )
	{
		GtkIconSize sz = gtk_toolbar_get_icon_size ( toolbar );

		if ( gnoclGetImage ( interp, iconOpt->val.obj, sz, icon ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static void setUnderline ( GtkWidget *item )
{
	/* FIXME: is there really only one label? */
	GtkWidget *label = gnoclFindChild ( item, GTK_TYPE_LABEL );
	assert ( label );
	gtk_label_set_use_underline ( GTK_LABEL ( label ), 1 );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static Tcl_Obj *cgetText ( GtkWidget *item )
{
	/* FIXME: is there really only one label? */
	GtkWidget *label = gnoclFindChild ( item, GTK_TYPE_LABEL );

	if ( label != NULL )
	{
		const char *txt = gtk_label_get_label ( GTK_LABEL ( label ) );

		if ( txt != NULL )
		{
			Tcl_Obj *obj = Tcl_NewStringObj ( txt, -1 );
			/* FIXME: that does not work
			if( gtk_button_get_use_stock( GTK_BUTTON( item ) ) )
			{
			   Tcl_Obj *old = obj;
			   obj = Tcl_NewStringObj( "%#", 2 );
			   Tcl_AppendObjToObj( obj, old );
			}
			else
			*/

			if ( gtk_label_get_use_underline ( GTK_LABEL ( label ) ) )
			{
				Tcl_Obj *old = obj;
				obj = Tcl_NewStringObj ( "%_", 2 );
				Tcl_AppendObjToObj ( obj, old );
			}

			return obj;
		}
	}

	return Tcl_NewStringObj ( "", 0 );;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
/*
   -------------- check functions ----------------------------
*/
static void checkDestroyFunc ( GtkWidget *widget, gpointer data )
{
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;

	gnoclAttacheVariable ( NULL, &para->variable,
						   "toggled", G_OBJECT ( para->widget ),
						   G_CALLBACK ( gnoclCheckToggledFunc ),
						   para->interp, gnoclCheckTraceFunc, para );

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );
	g_free ( para->name );

	Tcl_DecrRefCount ( para->onValue );
	Tcl_DecrRefCount ( para->offValue );
	g_free ( para );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int checkConfigure ( Tcl_Interp *interp, GnoclCheckParams *para,
							GnoclOption options[] )
{
	if ( options[checkOnValueIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_OBJ ( options[checkOnValueIdx].val.obj, para->onValue );

	if ( options[checkOffValueIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_OBJ ( options[checkOffValueIdx].val.obj, para->offValue );

	if ( options[checkOnToggledIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_STRING ( options[checkOnToggledIdx].val.str, para->onToggled );

	gnoclAttacheVariable ( &options[checkVariableIdx], &para->variable,
						   "toggled", G_OBJECT ( para->widget ),
						   G_CALLBACK ( gnoclCheckToggledFunc ), para->interp,
						   gnoclCheckTraceFunc, para );

	if ( gnoclCheckSetActive ( para, &options[checkActiveIdx] ) == 0
			&& ( options[checkVariableIdx].status == GNOCL_STATUS_CHANGED
				 || options[checkOnValueIdx].status == GNOCL_STATUS_CHANGED
				 || options[checkOffValueIdx].status == GNOCL_STATUS_CHANGED ) )
	{
		gnoclCheckVariableValueChanged ( para );
	}

	if ( options[checkValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclCheckSetValue ( para, options[checkValueIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int checkCget ( Tcl_Interp *interp, GnoclCheckParams *para,
					   GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == checkTextIdx )
		obj = cgetText ( para->widget );
	else if ( idx == checkIconIdx )
		; /* TODO */
	else if ( idx == checkOnToggledIdx )
		obj = Tcl_NewStringObj ( para->onToggled ? para->onToggled : "", -1 );
	else if ( idx == checkOnValueIdx )
		obj = para->onValue;
	else if ( idx == checkOffValueIdx )
		obj = para->offValue;
	else if ( idx == checkVariableIdx )
		obj = Tcl_NewStringObj ( para->variable, -1 );
	else if ( idx == checkActiveIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
		obj = Tcl_NewBooleanObj ( on );
	}

	else if ( idx == checkValueIdx )
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

/*****/

/***f* toolBar/staticFuncs/checktoolButtonFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int checktoolButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onToggled", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnToggledIdx, ClassIdx };
	GnoclCheckParams *para = ( GnoclCheckParams * ) data;
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "toolBarCheckButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   checkOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					int k;

					for ( k = 0; k < checkOnToggledIdx; ++k )
					{
						if ( checkOptions[k].status == GNOCL_STATUS_CHANGED )
						{
							Tcl_AppendResult ( interp, "Option \"",
											   checkOptions[k].optName,
											   "\" cannot be set after widget creation.",
											   NULL );
							gnoclClearOptions ( checkOptions );
							return TCL_ERROR;
						}
					}

					ret = checkConfigure ( interp, para, checkOptions );
				}

				gnoclClearOptions ( checkOptions );

				return ret;
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 checkOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return checkCget ( interp, para, checkOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclCheckOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int addCheckButton ( GtkToolbar *toolbar, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[], int atEnd )
{
	int              ret;
	GnoclCheckParams *para = NULL;
	char             *txt;
	int              isUnderline;
	GtkWidget        *icon;

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, checkOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( checkOptions );
		return TCL_ERROR;
	}

	if ( getTextAndIcon ( interp, toolbar,
						  checkOptions + checkTextIdx, checkOptions + checkIconIdx,
						  &txt, &icon, &isUnderline ) != TCL_OK )
		return TCL_ERROR;

	para = g_new ( GnoclCheckParams, 1 );

	para->onToggled = NULL;

	para->interp = interp;

	para->name = gnoclGetAutoWidgetId();

	para->variable = NULL;

	para->onValue = Tcl_NewIntObj ( 1 );

	Tcl_IncrRefCount ( para->onValue );

	para->offValue = Tcl_NewIntObj ( 0 );

	Tcl_IncrRefCount ( para->offValue );

	para->inSetVar = 0;

	para->widget =
		( atEnd ?  gtk_toolbar_append_element : gtk_toolbar_prepend_element )
		( toolbar, GTK_TOOLBAR_CHILD_TOGGLEBUTTON, NULL, txt, NULL, NULL,
		  icon, G_CALLBACK ( gnoclCheckToggledFunc ), para );

	if ( isUnderline )
		setUnderline ( para->widget );

	ret = gnoclSetOptions ( interp, checkOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
		ret = checkConfigure ( interp, para, checkOptions );

	if ( ret != TCL_OK )
		g_free ( para );
	else
	{
		g_signal_connect_after ( G_OBJECT ( para->widget ), "destroy",
								 G_CALLBACK ( checkDestroyFunc ), para );

		gnoclMemNameAndWidget ( para->name, para->widget );

		Tcl_CreateObjCommand ( interp, para->name, checktoolButtonFunc, para, NULL );
		Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );
	}

	gnoclClearOptions ( checkOptions );

	return ret;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
/*
   -------------- radio functions ----------------------------
*/
static int radioConfigure ( Tcl_Interp *interp, GnoclRadioParams *para,
							GnoclOption options[] )
{
	if ( gnoclRadioSetValueActive ( para, &options[radioOnValueIdx],
									&options[radioActiveIdx] ) != TCL_OK )
		return TCL_ERROR;

	if ( options[radioOnToggledIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_STRING ( options[radioOnToggledIdx].val.str, para->onToggled );

	if ( options[radioValueIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclRadioSetValue ( para, options[radioValueIdx].val.obj ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int radioCget ( Tcl_Interp *interp, GnoclRadioParams *para,
					   GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == radioTextIdx )
		obj = cgetText ( para->widget );
	else if ( idx == radioIconIdx )
		; /* TODO */
	else if ( idx == radioOnToggledIdx )
		obj = Tcl_NewStringObj ( para->onToggled ? para->onToggled : "", -1 );
	else if ( idx == radioOnValueIdx )
	{
		GnoclRadioParams *p = gnoclRadioGetActivePara ( para->group );
		obj = p->onValue;
	}

	else if ( idx == radioVariableIdx )
		obj = Tcl_NewStringObj ( para->group->variable, -1 );
	else if ( idx == radioActiveIdx )
	{
		gboolean on;
		g_object_get ( G_OBJECT ( para->widget ), "active", &on, NULL );
		obj = Tcl_NewBooleanObj ( on );
	}

	else if ( idx == radioValueIdx )
		obj = gnoclRadioGetValue ( para );

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int radiotoolButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onToggled", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnToggledIdx, ClassIdx};
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "toolBarRadioButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, para->widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   radioOptions, G_OBJECT ( para->widget ) ) == TCL_OK )
				{
					int k;

					for ( k = 0; k < radioActiveIdx; ++k )
					{
						if ( radioOptions[k].status == GNOCL_STATUS_CHANGED )
						{
							Tcl_AppendResult ( interp, "Option \"",
											   radioOptions[k].optName,
											   "\" cannot be set after widget creation.",
											   NULL );
							gnoclClearOptions ( radioOptions );
							return TCL_ERROR;
						}
					}

					ret = radioConfigure ( interp, para, radioOptions );
				}

				gnoclClearOptions ( radioOptions );

				return ret;
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( para->widget ),
									 radioOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return radioCget ( interp, para, radioOptions, idx );
				}
			}

		case OnToggledIdx:
			return gnoclRadioOnToggled ( interp, objc, objv, para );
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int addRadioButton ( GtkToolbar *toolbar, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[], int atEnd )
{
	int              ret;
	GnoclRadioParams *para = NULL;
	char             *txt;
	int              isUnderline;
	GtkWidget        *icon;
	GtkWidget        *firstWidget = NULL;

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, radioOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( radioOptions );
		return TCL_ERROR;
	}

	if ( radioOptions[radioOnValueIdx].status != GNOCL_STATUS_CHANGED
			|| radioOptions[radioVariableIdx].status != GNOCL_STATUS_CHANGED )
	{
		gnoclClearOptions ( radioOptions );
		Tcl_SetResult ( interp,
						"Option \"-onValue\" and \"-variable\" are required.", TCL_STATIC );
		return TCL_ERROR;
	}

	if ( getTextAndIcon ( interp, toolbar,
						  radioOptions + radioTextIdx, radioOptions + radioIconIdx,
						  &txt, &icon, &isUnderline ) != TCL_OK )
		return TCL_ERROR;

	para = g_new ( GnoclRadioParams, 1 );

	para->name = gnoclGetAutoWidgetId();

	para->onToggled = NULL;

	para->onValue = NULL;

	para->group = gnoclRadioGetGroupFromVariable (
					  radioOptions[radioVariableIdx].val.str );

	if ( para->group == NULL )
	{
		para->group = gnoclRadioGroupNewGroup (
						  radioOptions[radioVariableIdx].val.str, interp );
	}

	else
	{
		GnoclRadioParams *p = gnoclRadioGetParam ( para->group, 0 );
		firstWidget = p->widget;
	}

	para->widget =

		( atEnd ?  gtk_toolbar_append_element : gtk_toolbar_prepend_element )
		( toolbar, GTK_TOOLBAR_CHILD_RADIOBUTTON, firstWidget, txt,
		  NULL, NULL, icon, G_CALLBACK ( gnoclRadioToggledFunc ), para );

	if ( isUnderline )
		setUnderline ( para->widget );

	gnoclRadioGroupAddWidgetToGroup ( para->group, para );

	ret = gnoclSetOptions ( interp, radioOptions, G_OBJECT ( para->widget ), -1 );

	if ( ret == TCL_OK )
		ret = radioConfigure ( interp, para, radioOptions );

	if ( ret != TCL_OK )
		g_free ( para );
	else
	{
		g_signal_connect_after ( G_OBJECT ( para->widget ), "destroy",
								 G_CALLBACK ( gnoclRadioDestroyFunc ), para );

		gnoclMemNameAndWidget ( para->name, para->widget );

		Tcl_CreateObjCommand ( interp, para->name, radiotoolButtonFunc, para, NULL );
		Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );
	}

	gnoclClearOptions ( radioOptions );

	return ret;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/

/*
   -------------- button functions ----------------------------
*/

static int buttonDoCommand ( ToolButtonParams *para, int background )
{
#ifdef DEBUG_TOOLBAR
	printf ( "buttonDoCommand\n" );
#endif

	if ( para->onClicked )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 0 }
		};

		ps[0].val.str = para->name;
		return gnoclPercentSubstAndEval ( para->interp, ps, para->onClicked,
										  background );
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static void buttonCallback ( GtkWidget *widget, gpointer data )
{
	ToolButtonParams *para = ( ToolButtonParams * ) data;
	buttonDoCommand ( para, 1 );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static void buttonDestroyFunc ( GtkWidget *widget, gpointer data )
{
	ToolButtonParams *p = ( ToolButtonParams * ) data;

	gnoclForgetWidgetFromName ( p->name );
	Tcl_DeleteCommand ( p->interp, p->name );

	g_free ( p->onClicked );
	g_free ( p->name );
	g_free ( p );
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int buttonConfigure ( Tcl_Interp *interp, ToolButtonParams *para,
							 GnoclOption options[] )
{
	if ( options[buttonOnClickedIdx].status == GNOCL_STATUS_CHANGED )
		GNOCL_MOVE_STRING ( options[buttonOnClickedIdx].val.str, para->onClicked );

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
**/
int toolButtonFunc ( ClientData data, Tcl_Interp *interp,
					 int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, OnClickedIdx, ClassIdx };
	ToolButtonParams *para = ( ToolButtonParams * ) data;
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "toolBarButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( para->item ), objc, objv );

		case ConfigureIdx:
			{
#ifdef DEBUG_TOOLBAR
				printf ( "toolButtonFunc/configure\n" );
#endif
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   toolButtonOptions, G_OBJECT ( para->item ) ) == TCL_OK )
				{
					ret = buttonConfigure ( interp, para, toolButtonOptions );
				}

				gnoclClearOptions ( toolButtonOptions );

				return ret;
			}

			break;
		case OnClickedIdx:
			{
				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, NULL );
					return TCL_ERROR;
				}

				return buttonDoCommand ( para, 0 );
			}
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
static int addButton ( GtkToolbar *toolbar, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[], int atEnd )
{
	int          ret;
	ToolButtonParams *para = NULL;
	char         *txt;
	int          isUnderline;
	GtkWidget    *icon;

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, toolButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( toolButtonOptions );
		return TCL_ERROR;
	}

	if ( getTextAndIcon ( interp, toolbar, toolButtonOptions + buttonTextIdx,
						  toolButtonOptions + buttonIconIdx, &txt, &icon, &isUnderline ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	para = g_new ( ToolButtonParams, 1 );

	para->onClicked = NULL;

	para->interp = interp;

	para->name = gnoclGetAutoWidgetId();

	/* we handle stock in getTextAndIcon
	para->item = gtk_toolbar_insert_stock( toolbar, txt, NULL, NULL,
	      G_CALLBACK( buttonCallback ), para, atEnd ? -1 : 0 );
	*/
	para->item =
		( atEnd ?  gtk_toolbar_append_element : gtk_toolbar_prepend_element )
		( toolbar, GTK_TOOLBAR_CHILD_BUTTON, NULL, txt, NULL, NULL,
		  icon, G_CALLBACK ( buttonCallback ), para );

	if ( isUnderline )
	{
		setUnderline ( para->item );
	}

	ret = gnoclSetOptions ( interp, toolButtonOptions, G_OBJECT ( para->item ), -1 );

	if ( ret == TCL_OK )
	{
		ret = buttonConfigure ( interp, para, toolButtonOptions );
	}

	if ( ret != TCL_OK )
	{
		g_free ( para );
	}

	else
	{
		g_signal_connect_after ( G_OBJECT ( para->item ), "destroy",
								 G_CALLBACK ( buttonDestroyFunc ), para );

		gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->item ) );

		Tcl_CreateObjCommand ( interp, para->name, toolButtonFunc, para, NULL );
		Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );
	}

	gnoclClearOptions ( toolButtonOptions );

	return ret;
}

/**
\brief Description yet to be added.
\author Peter G Baum
    added by WJG 30/12/2007
    http://www.gtk.org/api/2.6/gtk/GtkMenuToolButton.html
*/
static int addMenuButton (
	GtkToolbar *toolbar,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[],
	int atEnd )
{
	int          ret;
	ToolButtonParams *para = NULL;
	char         *txt;
	int          isUnderline;
	GtkWidget    *icon;

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, toolButtonOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( toolButtonOptions );
		return TCL_ERROR;
	}

	if ( getTextAndIcon ( interp, toolbar,
						  toolButtonOptions + buttonTextIdx, toolButtonOptions + buttonIconIdx,
						  &txt, &icon, &isUnderline ) != TCL_OK )
		return TCL_ERROR;

	para = g_new ( ToolButtonParams, 1 );

	para->onClicked = NULL;

	para->interp = interp;

	para->name = gnoclGetAutoWidgetId();

	/* we handle stock in getTextAndIcon
	para->item = gtk_toolbar_insert_stock( toolbar, txt, NULL, NULL,
	      G_CALLBACK( buttonCallback ), para, atEnd ? -1 : 0 );
	*/

	// GTK_TOOLBAR_CHILD_BUTTON
	// gtk_menu_tool_button_new
	para->item =
		( atEnd ?  gtk_toolbar_append_element : gtk_toolbar_prepend_element )
		( toolbar, GTK_TOOLBAR_CHILD_BUTTON, NULL, txt, NULL, NULL,
		  icon, G_CALLBACK ( buttonCallback ), para );

	/*
	   para->item =
	      (atEnd ?  gtk_toolbar_append_element : gtk_toolbar_prepend_element)
	      ( toolbar, GTK_TOOLBAR_CHILD_BUTTON, NULL, txt, NULL, NULL,
	      icon, G_CALLBACK( buttonCallback ), para );
	*/

	if ( isUnderline )
		setUnderline ( para->item );

	ret = gnoclSetOptions ( interp, toolButtonOptions, G_OBJECT ( para->item ), -1 );

	if ( ret == TCL_OK )
		ret = buttonConfigure ( interp, para, toolButtonOptions );

	if ( ret != TCL_OK )
		g_free ( para );
	else
	{
		g_signal_connect_after ( G_OBJECT ( para->item ), "destroy",
								 G_CALLBACK ( buttonDestroyFunc ), para );

		gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->item ) );

		Tcl_CreateObjCommand ( interp, para->name, toolButtonFunc, para, NULL );
		Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );
	}

	gnoclClearOptions ( toolButtonOptions );

	return ret;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
/* WJG added menuButton item 29/12/07 */

static int addItem ( GtkToolbar *toolbar, Tcl_Interp *interp,
					 int objc, Tcl_Obj * const objv[], int atEnd )
{
	const char *txt[] =
	{
		"space",
		"widget",
		"item",
		"menuButton",
		"checkItem",
		"radioItem",
		NULL
	};
	enum typeIdx
	{
		SpaceIdx,
		WidgetIdx,
		ButtonIdx,
		MenuButtonIdx,
		CheckButtonIdx,
		RadioItemIdx
	};
	int idx;

	if ( objc < 3 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, "type ?option val ...?" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[2], txt, "type",
							   TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case SpaceIdx:
			{
				if ( objc != 3 )
				{
					Tcl_WrongNumArgs ( interp, 3, objv, NULL );
					return TCL_ERROR;
				}

				if ( atEnd )
				{
					gtk_toolbar_append_space ( toolbar );
				}

				else
				{
					gtk_toolbar_prepend_space ( toolbar );
				}
			}

			break;
		case WidgetIdx:
			{
				GtkWidget *child;

				if ( objc != 4 )
				{
					Tcl_WrongNumArgs ( interp, 3, objv, "widget-ID" );
					return TCL_ERROR;
				}

				child = gnoclGetWidgetFromName ( Tcl_GetString ( objv[3] ),

												 interp );

				if ( child == NULL )
				{
					return TCL_ERROR;
				}

				( atEnd ?  gtk_toolbar_append_element :
				  gtk_toolbar_prepend_element )
				( toolbar, GTK_TOOLBAR_CHILD_WIDGET, child,
				  NULL,  NULL, NULL, NULL, NULL, NULL );

				Tcl_SetObjResult ( interp, objv[3] );

				return TCL_OK;
			}

		case ButtonIdx:
			return addButton ( toolbar, interp, objc, objv, atEnd );
		case CheckButtonIdx:
			return addCheckButton ( toolbar, interp, objc, objv, atEnd );
		case RadioItemIdx:
			return addRadioButton ( toolbar, interp, objc, objv, atEnd );
		case MenuButtonIdx:
			/* wjg stuff here */
			return addMenuButton ( toolbar, interp, objc, objv, atEnd );
	}

	return TCL_OK;
}

/**
\brief Description yet to be added.
\author Peter G Baum
*/
int toolBarFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "add", "addBegin", "addEnd", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, AddIdx, BeginIdx, EndIdx, ClassIdx };

	GtkToolbar *toolBar = GTK_TOOLBAR ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "toolBar", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( toolBar ), objc, objv );

		case ConfigureIdx:
			{
				int ret = gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
													toolBarOptions, G_OBJECT ( toolBar ) );
				gnoclClearOptions ( toolBarOptions );
				return ret;
			}

			break;

		case AddIdx:      /* add is a shortcut for addEnd */
		case BeginIdx:
		case EndIdx:
			return addItem ( toolBar, interp, objc, objv,
							 idx != BeginIdx );
	}

	return TCL_OK;
}

/*****/

/***f* toolBar/gnoclToolBarCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclToolBarCmd ( ClientData data, Tcl_Interp *interp,
					  int objc, Tcl_Obj * const objv[] )
{
	int        ret;
	GtkToolbar *toolBar;

	if ( gnoclParseOptions ( interp, objc, objv, toolBarOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( toolBarOptions );
		return TCL_ERROR;
	}

	toolBar = GTK_TOOLBAR ( gtk_toolbar_new( ) );

	gtk_widget_show ( GTK_WIDGET ( toolBar ) );

	ret = gnoclSetOptions ( interp, toolBarOptions, G_OBJECT ( toolBar ), -1 );
	gnoclClearOptions ( toolBarOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( toolBar ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( toolBar ), toolBarFunc );
}


