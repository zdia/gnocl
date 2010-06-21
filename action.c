/**
 \brief
  This module implements the gnocl::action widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/** \page page2 gnocl::action
  \section sec gnocl::action
  Implementation of gnocl::action.
  \subsection subsection1 Implemented gnocl::action Options
  \subsection subsection2 Implemented gnocl::action Commands

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

/*
 * $Id: action.c,v 1.1 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements GtkAction
 *
 * Copyright (c) 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2005-01: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/****v* action/options
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GnoclOption actionOptions[] =
{
	{ "-name", GNOCL_STRING, NULL },             /* 0 */
	{ "-text", GNOCL_OBJ, NULL },                /* 1 */
	{ "-accelerator", GNOCL_OBJ, NULL },         /* 2 */
	{ "-group", GNOCL_OBJ, NULL },               /* 3 */
	{ "-onActivate", GNOCL_OBJ, "activate", gnoclOptCommand },
	{ "-icon", GNOCL_STRING, NULL },
	{ "-isImportant", GNOCL_BOOL, "is-important" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_STRING, "tooltip" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL },
};
/*****/

/****v* action/constants
 * AUTHOR
 *	PGB
 * SOURCE
 */
const int NameIdx  = 0;
const int TextIdx  = 1;
const int AccelIdx = 2;
const int GroupIdx = 3;
/*****/

/*
set copyAction [gnocl::action -name "Copy" -text "%#Copy"]
set someAction [gnocl::action -name "Some" -text "Some" -icon "%#Quit"]


  set manager [gnocl::uiManager]
  # construct action_group
  set group [gnocl::actionGroup MenuActions ...]
  $group addActions {{FileMenu Icon "_File"} ...}
  $group addToggleActions {{...}}
  $group addRadioActions {{...}}

  # add action_group to ui_manager
  $manager add $group

  # set accelWindow from ui_manager
  $manager setAccelWindow $window

  # add ui to ui_manager
  $manager addUI $description

  # get widget from ui_manager
  $manger getWidget "/MainMenu"


  set manager [gnocl::uiManager]
   $manager addActionGroup
   $manager addActionItem
   $manager setAccelWindow
   $manager getWidget qqq
*/

/****f* action/getGlobalActionGroup
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GtkActionGroup *getGlobalActionGroup ( void )
{
	static GtkActionGroup *group = NULL;

	if ( group == NULL )
	{
		/*
		group = gtk_action_group_new( "global" );
		GtkUIManager *manager = gtk_ui_manager_new();
		gtk_ui_manager_insert_action_group( manager, group, 0 );
		*/
	}

	return group;
}

/*****/

/****f* action/configure
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkAction *action,
					   GnoclOption options[] )
{
	const char *accel = NULL;

	if ( options[TextIdx].status == GNOCL_STATUS_CHANGED )
	{
		Tcl_Obj *obj = options[TextIdx].val.obj;
		GnoclStringType type = gnoclGetStringType ( obj );
		GtkStockItem  item;

		if ( type & GNOCL_STR_STOCK
				&& gnoclGetStockItem ( obj, interp, &item ) == TCL_OK )
		{
			g_object_set ( G_OBJECT ( action ), "stock-id", item.stock_id, NULL );
		}

		else
		{
			g_object_set ( G_OBJECT ( action ), "label", gnoclGetString ( obj ),
						   NULL );
		}
	}

	if ( options[AccelIdx].status == GNOCL_STATUS_CHANGED )
	{
		accel = Tcl_GetString ( options[AccelIdx].val.obj );
		gtk_action_set_accel_group ( action, gnoclGetAccelGroup() );
		gtk_action_set_accel_path ( action, gtk_action_get_name ( action ) );
		gtk_action_connect_accelerator ( action );
	}

	/*
	if( accel == NULL || *accel )
	{
	   GtkActionGroup *group = getGlobalActionGroup();
	   gtk_action_group_add_action_with_accel( group, action, accel );
	}
	*/

	return TCL_OK;
}

/*****/

/****f* action/actionFunc
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int actionFunc ( ClientData data, Tcl_Interp *interp,
						int objc, Tcl_Obj * const objv[] )
{

	const char *cmds[] = { "delete", "configure", "createMenuItem", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CreateMenuItemIdx };
	int idx;
	GtkAction *action = GTK_ACTION ( data );

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
			return gnoclDelete ( interp, GTK_WIDGET ( action ), objc, objv );
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   actionOptions, G_OBJECT ( action ) ) == TCL_OK )
				{
					ret = configure ( interp, action, actionOptions );
				}

				gnoclClearOptions ( actionOptions );

				return ret;
			}

			break;
		case CreateMenuItemIdx:
			{
				GtkWidget *widget;

				if ( objc != 2 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "command" );
					return TCL_ERROR;
				}

				widget = gtk_action_create_menu_item ( action );

				gnoclRegisterWidget ( interp, widget, NULL );
				return TCL_OK;
			}
	}

	return TCL_OK;
}

/*****/

/****f* action/registerAction
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int registerAction ( Tcl_Interp *interp, GtkAction *action,
							Tcl_ObjCmdProc *proc )
{
	GtkActionGroup *group;
	const char idPrefix[] = "::gnocl::_AID";
	static int no = 0;

	char *name = g_new ( char, sizeof ( idPrefix ) + 15 );
	strcpy ( name, idPrefix );

	/* with namespace, since the Id is also the widget command */
	sprintf ( name + sizeof ( idPrefix ) - 1, "%d", ++no );

	Tcl_CreateObjCommand ( interp, name, proc, action, NULL );
	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( name, -1 ) );

	/*
	group = getGlobalActionGroup();
	gtk_action_group_add_action( group, action );
	*/

	return 0;
}

/*****/

/****f* action/gnoclActionCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclActionCmd ( ClientData data, Tcl_Interp *interp,
					 int objc, Tcl_Obj * const objv[] )
{
	int        ret;
	GtkAction  *action;
	const char *name;

	/*
	   if( gnoclParseOptions( interp, objc, objv, actionOptions ) != TCL_OK )
	   {
	      gnoclClearOptions( actionOptions );
	      return TCL_ERROR;
	   }

	   if( actionOptions[NameIdx].status == GNOCL_STATUS_CHANGED )
	      name = actionOptions[NameIdx].val.str;
	   else if( actionOptions[TextIdx].status == GNOCL_STATUS_CHANGED )
	      name = gnoclGetString( actionOptions[TextIdx].val.obj );
	   else
	   {
	      Tcl_SetResult( interp,
	            "Either parameter \"-name\" or \"-text\" must be given",
	            TCL_STATIC );
	      gnoclClearOptions( actionOptions );
	      return TCL_ERROR;
	   }
	printf( "name: %s\n", name );

	   action = GTK_ACTION( gtk_action_new( name, NULL, NULL, "" ) );

	   ret = gnoclSetOptions( interp, actionOptions, G_OBJECT( action ), -1 );
	   if( ret == TCL_OK )
	      ret = configure( interp, action, actionOptions );
	   gnoclClearOptions( actionOptions );

	   if( ret != TCL_OK )
	   {
	      g_object_unref( G_OBJECT( action ) );
	      return TCL_ERROR;
	   }
	*/
	GtkActionGroup *action_group = gtk_action_group_new ( "AppWindowActions" );
	action = gtk_action_new ( "Quit", "Label", "tooltip", "gtk-quit" );
	gtk_action_group_add_action_with_accel ( action_group, action, "<control>q" );

	return registerAction ( interp, action, actionFunc );
}

/*****/

