/**
 \brief
  This module implements the gnocl::menu widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page28 gnocl::menu
  \section sec gnocl::menu
  Implementation of gnocl::menu
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
 \image html "../pics/button.png"
*/

/****h* gnocl/menu
 * NAME
 *  menu.c
 * AUTHOR
 *  PGB
 * SYNOPSIS
 *  This file implements the menu widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/menu.png">
 * HISTORY
 *  2009-05: modified popup to support positioning of popup menu, with x y coordinates.
 *  2008-10: added command, class
 *       10: switched from GnoclWidgetOptions to GnoclOption
 *  2002-04: updates for gtk 2.0
 *       09: accelerator for menuItems
 *  2001-03: Begin of developement
 * SEE ALSO
 *****/

/*
 * $Id: menu.c,v 1.4 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the menu widget
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
        10: switched from GnoclWidgetOptions to GnoclOption
   2002-04: updates for gtk 2.0
        09: accelerator for menuItems
   2001-03: Begin of developement
 */

/*
   TODO?: menu factory? But as tcl function!
*/

#include "gnocl.h"
#include <string.h>
#include <assert.h>



/**
/author PGB
**/
static GnoclOption menuOptions[] =
{
	{ "-children", GNOCL_LIST, NULL },
	{ "-tearoff", GNOCL_BOOL, NULL },
	{ "-title", GNOCL_STRING, "tearoff-title" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ NULL }
};

/**
/author PGB
**/
static const int childrenIdx = 0;
static const int tearoffIdx  = 1;


/**
/brief      Structure used to pass use script supplied values pointer positition to popup menu command.
/author     William J Giddings
/date       17/05/09
**/

typedef struct
{
	int x;
	int y;
} MenuPositionData;

static void position_function ( GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data )
{
	/* Do not do this in your own code */

	MenuPositionData *position_data = user_data;

	if ( x )
	{
		*x = position_data->x;
	}

	if ( y )
	{
		*y = position_data->y;
	}

	if ( push_in )
	{
		*push_in = FALSE;
	}
}

/**
**/
int gnoclMenuShellAddChildren ( Tcl_Interp *interp, GtkMenuShell *shell, Tcl_Obj *children, int atEnd )
{
	int n, noChilds;

	if ( Tcl_ListObjLength ( interp, children, &noChilds ) != TCL_OK || noChilds < 1 )
	{
		Tcl_SetResult ( interp, "Widget-list must be a list with at least one element", TCL_STATIC );
		return TCL_ERROR;
	}

	for ( n = 0; n < noChilds; ++n )
	{
		GtkWidget   *childWidget;
		Tcl_Obj     *tp;
		const char  *childName;

		if ( Tcl_ListObjIndex ( interp, children, n, &tp ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		childName = Tcl_GetString ( tp );

		childWidget = gnoclChildNotPacked ( childName, interp );

		if ( childWidget == NULL )
		{
			return TCL_ERROR;
		}

		if ( !GTK_CHECK_TYPE ( childWidget, GTK_TYPE_MENU_ITEM ) )
		{
			Tcl_AppendResult ( interp, "child window \"",
							   childName, "\" is not a menu item.", ( char * ) NULL );
			return TCL_ERROR;
		}

		if ( atEnd )
		{
			gtk_menu_shell_append ( shell, childWidget );
		}

		else
		{
			gtk_menu_shell_prepend ( shell, childWidget );
		}
	}

	return TCL_OK;
}

/*****/

/****f* menu/staticFuncs/configure
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkMenu *menu, GnoclOption options[] )
{
	if ( options[tearoffIdx].status == GNOCL_STATUS_CHANGED )
	{
		/* the tearoff widget is created on creation of the menu.
		   Here it is only hidden or shown */
		GtkWidget *widget = GTK_WIDGET ( GTK_MENU_SHELL ( menu )->children->data );

		if ( options[tearoffIdx].val.b )
		{
			gtk_widget_show ( widget );
		}

		else
		{
			gtk_widget_hide ( widget );
		}
	}

	if ( options[childrenIdx].status == GNOCL_STATUS_CHANGED )
	{
		return gnoclMenuShellAddChildren ( interp, GTK_MENU_SHELL ( menu ), options[childrenIdx].val.obj, 1 );
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum
**/
int menuFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "add", "addBegin", "addEnd", "popup", "popdown", "class", NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, AddIdx, BeginIdx, EndIdx, PopupIdx, PopdownIdx, ClassIdx };

	GtkMenu *menu = GTK_MENU ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "menu", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( menu ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   menuOptions, G_OBJECT ( menu ) ) == TCL_OK )
				{
					ret = configure ( interp, menu, menuOptions );
				}

				gnoclClearOptions ( menuOptions );

				return ret;
			}

			break;

		case AddIdx:      /* AddIdx and EndIdx is the same */
		case BeginIdx:
		case EndIdx:
			{
				if ( objc != 3 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "widget-list" );
					return TCL_ERROR;
				}

				return gnoclMenuShellAddChildren (

						   interp,
						   GTK_MENU_SHELL ( menu ),
						   objv[2],
						   idx != BeginIdx );
			}

		case PopupIdx:
			/* supply coordinates to accurately place the menu */
			g_print ( "POPUP \n" );

			MenuPositionData position_data;
			position_data.x = atoi ( Tcl_GetString ( objv[2] ) );
			position_data.y = atoi ( Tcl_GetString ( objv[3] ) );

			g_print ( "POPUP x: %d y: %d\n", position_data.x, position_data.y );


			if ( position_data.x  != 0 && position_data.y  != 0 )
			{
				gtk_menu_popup ( menu, NULL, NULL, position_function, &position_data, 0, 0 );
			}

			else
			{
				gtk_menu_popup ( menu, NULL, NULL, NULL, NULL, 0, 0 );
			}

			break;

		case PopdownIdx:
			g_print ( "POPDOWN x: %d y: %d\n", objv[3], objv[4] );
			gtk_menu_popdown ( menu );
			break;
	}

	return TCL_OK;
}

/*****/

/****f* menu/gnoclMenuCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclMenuCmd ( ClientData data, Tcl_Interp *interp,
				   int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkMenu   *menu;
	GtkWidget *tearoff;

	if ( gnoclParseOptions ( interp, objc, objv, menuOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( menuOptions );
		return TCL_ERROR;
	}

	/* set tearoff to "on" as default */
	menu = GTK_MENU ( gtk_menu_new() );

	tearoff = gtk_tearoff_menu_item_new();

	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), tearoff );

	gtk_widget_show ( tearoff );

	gtk_widget_show ( GTK_WIDGET ( menu ) );

	ret = gnoclSetOptions ( interp, menuOptions, G_OBJECT ( menu ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, menu, menuOptions );

	gnoclClearOptions ( menuOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( menu ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( menu ), menuFunc );
}
