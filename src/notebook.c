/**
 \brief
  This module implements the gnocl::notebook widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page34 gnocl::notebook
  \section sec gnocl::notebook
  Implementation of gnocl::notebook
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
  \image html "../pics/notebook.png"
*/

/****h* widget/notebook
 * NAME
 *  notebook.c
 * SYNOPSIS
 *  This file implements the notebook widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/notebook.png">
 * SEE ALSO
 *****/

/*
 * $Id: notebook.c,v 1.11 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the notebook widget
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
   2003-10: added removePage
   2002-08: switched from GnoclWidgetOptions to GnoclOption
            gtk+ 2.0 related cleanups
        11: made accelerators work
        09: underlined character: accelerators still don't work
   2001-05: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****v* notebook/notebookOptions
 * AUTHOR
 *  PGB
 * SOURCE
 */
static GnoclOption notebookOptions[] =
{
	{ "-children", GNOCL_LIST, NULL },        /* 0 */
	{ "-onSwitchPage", GNOCL_OBJ, NULL },     /* 1 */
	{ "-tabPosition", GNOCL_OBJ, "tab-pos", gnoclOptPosition },
	{ "-showTabs", GNOCL_BOOL, "show-tabs" },
	{ "-scrollable", GNOCL_BOOL, "scrollable" },
	{ "-enablePopup", GNOCL_BOOL, "enable-popup" },
	{ "-homogeneous", GNOCL_BOOL, "homogeneous" },
	{ "-page", GNOCL_INT, "page" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-borderWidth", GNOCL_OBJ, "border-width", gnoclOptPadding },
	{ "-showBorder", GNOCL_BOOL, "show-border" },
	{ NULL }
};

/**
\brief
    Description yet to be added.
**/
static const int childrenIdx     = 0;
static const int onSwitchPageIdx = 1;

/**
\brief
    Description yet to be added.
**/
static void switchPageFunc ( GtkNotebook *notebook, GtkNotebookPage *page, gint page_num, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget */
		{ 'p', GNOCL_INT },
		{ 0 }
	};
	ps[0].val.str = gnoclGetNameFromWidget ( GTK_WIDGET ( notebook ) );
	ps[1].val.i = page_num;
	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}

/**
\brief
    Description yet to be added.
**/
static GtkWidget *getLabel ( Tcl_Interp *interp, Tcl_Obj *obj )
{
	const char *name = Tcl_GetString ( obj );
	GtkWidget *widget = gnoclGetWidgetFromName ( name, NULL );

	if ( widget == NULL )
	{
		GnoclStringType type = gnoclGetStringType ( obj );
		widget = gtk_label_new ( gnoclGetString ( obj ) );
		gtk_label_set_use_markup ( GTK_LABEL ( widget ),
								   ( type & GNOCL_STR_MARKUP ) != 0 );
		gtk_label_set_use_underline ( GTK_LABEL ( widget ),
									  ( type & GNOCL_STR_UNDERLINE ) != 0 );
	}

	else if ( gnoclAssertNotPacked ( widget, interp, name ) )
		return NULL;

	return widget;
}

/**
\brief
    Description yet to be added.
**/
static int addPage ( GtkNotebook *notebook, Tcl_Interp *interp,
					 Tcl_Obj *child, Tcl_Obj *label, Tcl_Obj *menu, int begin )
{
	GtkWidget *menuWidget;
	GtkWidget *labelWidget;
	GtkWidget *childWidget = gnoclChildNotPacked ( Tcl_GetString ( child ),
							 interp );

	if ( childWidget == NULL )
		return -1;

	labelWidget = getLabel ( interp, label );

	if ( labelWidget == NULL )
		return -1;

	menuWidget = getLabel ( interp, menu ? menu : label );

	if ( menuWidget == NULL )
		return -1;

	if ( begin )
		gtk_notebook_append_page_menu ( notebook, childWidget,
										labelWidget, menuWidget );
	else
		gtk_notebook_prepend_page_menu ( notebook, childWidget,
										 labelWidget, menuWidget );

	return gtk_notebook_page_num ( notebook, childWidget );
}


/**
\brief
    Description yet to be added.
**/
static int addChildren ( GtkNotebook *notebook, Tcl_Interp *interp, Tcl_Obj *children, int begin )
{
	int n, noChilds;

	if ( Tcl_ListObjLength ( interp, children, &noChilds ) != TCL_OK
			|| noChilds < 1 )
	{
		Tcl_SetResult ( interp, "widget-list must be proper list",
						TCL_STATIC );
		return TCL_ERROR;
	}

	for ( n = 0; n < noChilds; ++n )
	{
		Tcl_Obj   *subList, *child, *label;
		Tcl_Obj   *menu = NULL;
		int       noMem;

		if ( Tcl_ListObjIndex ( interp, children, n, &subList ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		if ( Tcl_ListObjLength ( interp, subList, &noMem ) != TCL_OK
				|| ( noMem != 2 && noMem != 3 ) )
		{
			/* if it's not a list of lists, test, if it is a single list
			   with content and bookmark */
			if ( noMem == 1 && ( noChilds == 2 || noChilds == 3 ) )
			{
				noMem = noChilds;
				noChilds = 1;
				subList = children;
			}

			else
			{
				Tcl_SetResult ( interp, "list must consists of two or three elements: "
								"\"widget\" \"bookmark\" \"menu\"", TCL_STATIC );
				return TCL_ERROR;
			}
		}

		if ( Tcl_ListObjIndex ( interp, subList, 0, &child ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		if ( Tcl_ListObjIndex ( interp, subList, 1, &label ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		if ( noMem > 2 )
		{
			if ( Tcl_ListObjIndex ( interp, subList, 2, &menu ) != TCL_OK )
			{
				return TCL_ERROR;
			}
		}

		if ( addPage ( notebook, interp, child, label, menu, begin ) < 0 )
		{
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

/**
\brief
    Description yet to be added.
**/
static int configure ( Tcl_Interp *interp, GtkNotebook *notebook, GnoclOption options[] )
{
	if ( options[childrenIdx].status == GNOCL_STATUS_CHANGED )
	{
		int ret = addChildren ( notebook, interp, options[childrenIdx].val.obj, 1 );

		if ( ret != TCL_OK )
		{
			return ret;
		}
	}

	if ( gnoclConnectOptCmd ( interp, G_OBJECT ( notebook ), "switch-page",
							  G_CALLBACK ( switchPageFunc ), options + onSwitchPageIdx, NULL,
							  NULL ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief
    Description yet to be added.
**/
static int notebookNext ( GtkNotebook *notebook, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[], int isNext )
{
	int cur = gtk_notebook_get_current_page ( notebook );
	int num = 1;

	/* widget next ?count? */

	if ( objc < 2 || objc > 3 )
	{
		Tcl_WrongNumArgs ( interp, 2, objv, "?count?" );
		return TCL_ERROR;
	}

	if ( objc == 3 && Tcl_GetIntFromObj ( interp, objv[2], &num ) != TCL_OK )
		return TCL_ERROR;

	if ( isNext )
	{
		cur = gtk_notebook_get_current_page ( notebook );
		gtk_notebook_set_current_page ( notebook, cur + num );
	}

	else if ( objc == 3 ) /* current val */
		gtk_notebook_set_current_page ( notebook, num );

	cur = gtk_notebook_get_current_page ( notebook );

	Tcl_SetObjResult ( interp, Tcl_NewIntObj ( cur ) );

	return TCL_OK;
}


/**
\brief
    Description yet to be added.
**/
int notebookFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	/* TODO?: notebook insert pos child label ?menu? */
	static const char *cmds[] = { "delete", "configure", "addPage", "currentPage", "nextPage", "removePage", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, AddPageIdx, CurrentIdx, NextPageIdx, RemovePageIdx, ClassIdx };

	GtkNotebook *notebook = GTK_NOTEBOOK ( data );
	int idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "notebook", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( notebook ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   notebookOptions, G_OBJECT ( notebook ) ) == TCL_OK )
				{
					ret = configure ( interp, notebook, notebookOptions );
				}

				gnoclClearOptions ( notebookOptions );

				return ret;
			}

			break;
		case AddPageIdx:
			{
				int ret;

				/* ID addPage child tab ?label? */

				if ( objc < 4 || objc > 5 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "child label ?menu?" );
					return TCL_ERROR;
				}

				ret = addPage ( notebook, interp, objv[2], objv[3], objc < 5 ? NULL : objv[4], 1 );

				if ( ret < 0 )
				{
					return TCL_ERROR;
				}

				/* TODO? associate an ID to every page? */
				Tcl_SetObjResult ( interp, Tcl_NewIntObj ( ret ) );

				return TCL_OK;
			}

		case CurrentIdx:
		case NextPageIdx:
			return notebookNext ( notebook, interp, objc, objv,
								  idx == NextPageIdx );
		case RemovePageIdx:
			{
				int k;

				if ( objc != 3 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "pageNumber" );
					return TCL_ERROR;
				}

				if ( Tcl_GetIntFromObj ( interp, objv[2], &k ) != TCL_OK )
					return TCL_ERROR;

				gtk_notebook_remove_page ( notebook, k );

				return TCL_OK;
			}

	}

	return TCL_OK;
}

/**
\brief
    Description yet to be added.
**/
int gnoclNotebookCmd ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	int ret;
	GtkWidget *widget;

	if ( gnoclParseOptions ( interp, objc, objv, notebookOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( notebookOptions );
		return TCL_ERROR;
	}

	widget = gtk_notebook_new();

	ret = gnoclSetOptions ( interp, notebookOptions,
							G_OBJECT ( widget ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, GTK_NOTEBOOK ( widget ), notebookOptions );

	gnoclClearOptions ( notebookOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( widget );
		return TCL_ERROR;
	}

	gtk_widget_show ( widget );

	return gnoclRegisterWidget ( interp, widget, notebookFunc );
}
