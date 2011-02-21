/**
 \brief
  This module implements the gnocl::menuItem widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page31 gnocl::menuItem
  \section sec gnocl::menuItem
  Implementation of gnocl::menuItem
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
  \image html "../pics/menuItem.png"
*/


/****h* widget/menuItem
 * NAME
 *  menuItem.c
 * SYNOPSIS
 *  This file implements the menuItem widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/menuItem.png">
 * SEE ALSO
 *****/

/*
 * $Id: menuItem.c,v 1.7 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements the menuItem widget
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
            moved Separator, (Check|Radio)Item into separate files
        05: updates for gtk 2.0
        02: added "invoke" to label and checkButton
   2002-01: bugfixes concerning -command and -variable handling
        12: added pixmap_menu_item
        11: added STD_OPTIONS
        10: check item
        09: underlined accelerators
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>


static GnoclOption labelOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },
	{ "-icon", GNOCL_OBJ, NULL },
	{ "-accelerator", GNOCL_OBJ, NULL },
	{ "-submenu", GNOCL_STRING, NULL },
	{ "-onClicked", GNOCL_OBJ, "activate", gnoclOptCommand },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};

static const int textIdx    = 0;
static const int iconIdx    = 1;
static const int accelIdx   = 2;
static const int submenuIdx = 3;

/**
/brief
/author  Peter G Baum
**/
Tcl_Obj *gnoclCgetMenuItemAccel ( Tcl_Interp *interp, GtkMenuItem *item )
{
	return NULL;
}

/**
/brief
/author  Peter G Baum
**/
int gnoclMenuItemHandleAccel ( Tcl_Interp *interp, GtkMenuItem *item,
							   Tcl_Obj *accelObj )
{
	const char *accel  = Tcl_GetString ( accelObj );
	/*
	GtkWidget *al = gtk_accel_label_new( "Ctl+Q" );
	gtk_accel_label_set_accel_widget( GTK_ACCEL_LABEL( al ),
	      GTK_WIDGET( pp->widget ) );
	*/
	guint key;
	GdkModifierType mod;

	gtk_accelerator_parse ( accel, &key, &mod );
	/*
	printf( "\"%s\" key: %d mod: %d\n", popt->accelerator.val, key, mod );
	*/

	if ( key != 0 || mod != 0 )
		gtk_widget_add_accelerator ( GTK_WIDGET ( item ),
									 "activate", gnoclGetAccelGroup(), key, mod,
									 GTK_ACCEL_VISIBLE );

	return TCL_OK;
}

/**
/brief
/author  Peter G Baum
**/
Tcl_Obj *gnoclCgetMenuItemText ( Tcl_Interp *interp, GtkMenuItem *item )
{
	Tcl_Obj *obj = NULL;

	/* FIXME
	if( gtk_button_get_use_stock( button ) )
	{
	   const char *st = gtk_button_get_label( button );

	   if( st == NULL )
	      obj = Tcl_NewStringObj( "", 0 );
	   else
	   {
	      obj = Tcl_NewStringObj( "%#", 2 );
	      Tcl_AppendObjToObj( obj, gnoclGtkToStockName( st ) );
	   }
	}
	else
	*/
	{
		GtkLabel *label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( item ),
									  GTK_TYPE_LABEL ) );
		Tcl_Obj *old = Tcl_NewStringObj ( gtk_label_get_label ( label ), -1 );
		assert ( label );

		if ( gtk_label_get_use_underline ( label ) )
		{
			obj = Tcl_NewStringObj ( "%_", 2 );
			Tcl_AppendObjToObj ( obj, old );
		}

		else
			obj = old;
	}

	return obj;
}


/**
/brief
/author  Peter G Baum
**/
int gnoclMenuItemHandleText ( Tcl_Interp *interp, GtkMenuItem *item,
							  Tcl_Obj *textObj )
{
	GnoclStringType type = gnoclGetStringType ( textObj );
	GtkStockItem  stockItem;
	GtkLabel      *label;
	char          *text;

	if ( type & GNOCL_STR_STOCK )
	{
		if ( gnoclGetStockItem ( textObj, interp, &stockItem ) != TCL_OK )
			return TCL_ERROR;

		text = stockItem.label;
	}

	else
		text = gnoclGetString ( textObj );

	label = GTK_LABEL ( gtk_bin_get_child ( GTK_BIN ( item ) ) );

	if ( type & ( GNOCL_STR_STOCK | GNOCL_STR_UNDERLINE ) )
		gtk_label_set_text_with_mnemonic ( label, text );
	else
		gtk_label_set_text ( label, text );

	if ( type & GNOCL_STR_STOCK )
	{
		GtkWidget *image = gtk_image_new_from_stock ( stockItem.stock_id,
						   GTK_ICON_SIZE_MENU );
		gtk_widget_show ( GTK_WIDGET ( image ) );
		gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM ( item ),
										image );

		if ( stockItem.keyval )
		{
			gtk_widget_add_accelerator ( GTK_WIDGET ( item ), "activate",
										 gnoclGetAccelGroup(), stockItem.keyval,
										 stockItem.modifier, GTK_ACCEL_VISIBLE );
		}
	}

	return TCL_OK;
}

/**
/brief
/author  Peter G Baum
**/
static int configure ( Tcl_Interp *interp, GtkMenuItem *label,
					   GnoclOption options[] )
{
	if ( options[textIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclMenuItemHandleText ( interp, label, options[textIdx].val.obj )
				!= TCL_OK )
			return TCL_ERROR;
	}

	if ( options[accelIdx].status == GNOCL_STATUS_CHANGED )
		gnoclMenuItemHandleAccel ( interp, label, options[accelIdx].val.obj );

	if ( options[iconIdx].status == GNOCL_STATUS_CHANGED )
	{
		GtkWidget *image;

		if ( gnoclGetImage ( interp, options[iconIdx].val.obj,
							 GTK_ICON_SIZE_MENU, &image ) != TCL_OK )
			return TCL_ERROR;

		/* if icon == "", ret == TCL_OK, but image == NULL */
		if ( image )
		{
			gtk_widget_show ( GTK_WIDGET ( image ) );
			gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM ( label ), image );
		}
	}

	if ( options[submenuIdx].status == GNOCL_STATUS_CHANGED )
	{
		const char *subMenu = options[submenuIdx].val.str;
		GtkWidget *childWidget = gnoclGetWidgetFromName ( subMenu, interp );

		if ( childWidget == NULL )
			return TCL_ERROR;

		if ( !GTK_CHECK_TYPE ( childWidget, GTK_TYPE_MENU ) )
		{
			Tcl_AppendResult ( interp, "submenu window \"", subMenu,
							   "\" is not a menu item.", ( char * ) NULL );
			return TCL_ERROR;
		}

		if ( gtk_menu_get_attach_widget ( GTK_MENU ( childWidget ) ) != NULL )
		{
			Tcl_AppendResult ( interp, "submenu \"", subMenu,
							   "\" has already been attached.", ( char * ) NULL );
			return TCL_ERROR;
		}

		gtk_menu_item_set_submenu ( label, childWidget );

		/* TODO: if submenu is detachable: set title of submenu to text */
	}

	return TCL_OK;
}


/**
/brief
/author  Peter G Baum
**/
int menuItemFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, OnClickedIdx, ClassIdx };
	GtkMenuItem *label = GTK_MENU_ITEM ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "menuItem", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( label ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   labelOptions, G_OBJECT ( label ) ) == TCL_OK )
				{
					ret = configure ( interp, label, labelOptions );
				}

				gnoclClearOptions ( labelOptions );

				return ret;
			}

			break;
		case OnClickedIdx:
			/* TODO: return value should be the return value of the function */
			gtk_menu_item_activate ( label );
			break;
	}

	return TCL_OK;
}

/**
/brief
/author  Peter G Baum
**/
int gnoclMenuItemCmd ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	int             ret;
	GtkMenuItem     *menuItem;

	if ( gnoclParseOptions ( interp, objc, objv, labelOptions ) != TCL_OK )
	{
		return TCL_ERROR;
	}


	menuItem = GTK_MENU_ITEM ( gtk_image_menu_item_new_with_mnemonic ( "" ) );

	gtk_widget_show ( GTK_WIDGET ( menuItem ) );

	ret = gnoclSetOptions ( interp, labelOptions, G_OBJECT ( menuItem ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, menuItem, labelOptions );
	}

	gnoclClearOptions ( labelOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( menuItem ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( menuItem ), menuItemFunc );
}

