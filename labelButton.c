/**
 \brief
  This module implements the gnocl::labelButton widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/**
  \page page25 gnocl::labelButton
  \section sec gnocl::labelButton
  Implementation of gnocl::labelButton
  \subsection subsection1 Implemented Options
  \verfbatim
	-activeBackgroundColor
		type: color
		Background color. DISCLAIMER_COLOR

	-data
		type: string
		User defined data which can be retrieved via the cget subcommand.

	-hasFocus
		type: 1
		This sets the focus to the widget. To unset the focus it must be
		set to another widdet.

	-heightGroup
		type: string (default: "")
		heightGroup can be an arbitrary string. All widgets with the same
		heightGroup request the same height. If packed with the fill flag
		set, they will have nevertheless different heights.

	-icon
		type: percent-string (default: "")
		Image to be shown, can be either a file (prefix "%/") or a stock
		item (prefix "%#").

	-name
		type: string
		Name of the widget, can be used to set options in an rc file.

	-normalBackgroundColor
		type: color
		Background color. DISCLAIMER_COLOR

	-onButtonPress
		type: string (default: "")
		Tcl script which is executed if a mouse button is press inside
		the widget.
		Before evaluation the following percent strings are substituated:
			%w	widget name
			%t	type of event. One of:
				buttonPress, button2Press or button3Press
			%x	x coordinate
			%y	y coordinate
			%b	button number
			%s	state of the buttons and modifiers (bitmask)

	-onButtonRelease
		type: string (default: "")
		Tcl script which is executed if a mouse button is released inside
		the widget.
		Before evaluation the following percent strings are substituated:
			%w	widget name
			%t	type of event. One of:
				buttonPress, button2Press or button3Press
			%x	x coordinate
			%y	y coordinate
			%b	button number
			%s	state of the buttons and modifiers (bitmask)

	-onClicked
		type: string (default: "")
		Tcl script which is executed if the button is clicked.
		Before evaluation the following percent strings are substituted:
			%w	widget name

	-onEnter
		Tcl script which is executed whenever the mouse pointer enters
		the button.
		Before evaluation the following percent strings are substituated:
			%w	widget name
			%x	x coordinate
			%y	y coordinate
			%s	state of the buttons and modifiers (bitmask)

	-onLeave
		Tcl script which is executed whenever the mouse pointer leaves
		the button.
		Before evaluation the following percent strings are substituated:
			%w	widget name
			%x	x coordinate
			%y	y coordinate
			%s	state of the buttons and modifiers (bitmask)

	-onPopupMenu
		type: string (default: "")
		Tcl script which is executed if the "popup-menu" signal is recieved,
		which is normally the case if the user presses Shift-F10.
		Before evaluation the following percent strings are substituated:
 			%w	widget name

 	-onRealize
 		type: string (default: "")
 		Tcl script whih is executed in the global scope if the widget has
 		been realized.
 		Before evaluation the following percent strings are substituated:
 			%w	widget name

 	-onShowHelp
 		type: string (default: "")
 		Tcl script which is executed in the global scope if the "show-help"
 		signal is recieved, which is normally the case if the user presses
 		F1 or Ctrl-F1.
 		Before evaluation the following percent strings are substituated:
 			%w	widget name
 			%h	help type: either "whatsThis" or "tooltip".

 	-prelightBackgroundColor
 		type: color
 		Background color. DISCLAIMER_COLOR

 	-relief
 		type: ONEOF normal, half, none (default: normal)
 		Relief of the button.

 	-sensitive
 		type: boolean (default: 1)
 		Whether or not the item is sensitve to user input.

 	-sizeGroup
 		type: string (default: "")
 		The value passed to sizeGroup can be an arbitrary string. All
 		widgets with the same sizeGroup request the same size. If packed
 		with the fill flag set, they will have nevertheless different sizes.

 	-text
 		type: percent-string (default: "")
 		Text or stock item of the button.

 	-tooltip
 		type: string
 		Message that appear next to this widget when the mouse pointer is
 		held over it for a short amount of time.

 	-visible
 		type: boolean (default: 1)
 		Whether or not the item is visible.

 	-widthGroup
 		type: string (default: "")
 		The value passed to widthGroup can be an arbitrary string. All
 		widgets with the same widthGroup request the same width. If packed
 		with the fill flag set, they will have nevertheless different widths.
  \endverbatim

  \subsection subsection2 Implemented Commands
  \verbatim
 	id cget option
 		Returns the value for one option. The option may have any of the
 		values accepted by configure.

 	id configure [-option value...]
 		Configures the widget. Option may have any of the values accepted
 		on creation of the widget.

 	id delete
 		Deletes the widget and the associated tcl command.

 	id onClicked
 		Executes the Tcl command associated with the button if there is
 		one. This command is ignored if the button's state is not sensitive.
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
 \image html "../pics/button.png"
*/

/*
 * $Id: button.c,v 1.17 2005/02/25 21:33:38 baum Exp $
 *
 * This file implements the button widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/****h* widget/button
 * NAME
 *  button.c
 * DESCRIPTION
 *  A button widget executes a command on mouse click.
 * AUTHOR
 *  Peter G. Baum
 *  William J Giddings
 * CREATION DATE
 *  2001-03:

 * MODIFICATION HISTORY
 *	2009-01: added geometry
 * 		 12: added parent
 * 	2008-10: added class
 * 	2004-02: added -data
 * 		 09: added cget
 * 			 renamed subcommand "invoke" to "onClicked"
 * 	2003-03: added -icon
 * 		 08: switched from GnoclWidgetOptions to GnoclOption
 * 			 many cleanups, e.g. no more associated parameters
 * 			 renamed -command to -onClicked
 * 		 04: update for gtk 2.0
 * 	2002-01: new command "invoke"
 * 		 09: underlined accelerators
 * 	2001-03: Begin of developement
 *****/





/*
   History:
   2009-01: added geometry
   		12: added parent
   2008-10: added class
   2004-02: added -data
        09: added cget
            renamed subcommand "invoke" to "onClicked"
   2003-03: added -icon
        08: switched from GnoclWidgetOptions to GnoclOption
            many cleanups, e.g. no more associated parameters
            renamed -command to -onClicked
        04: update for gtk 2.0
   2002-01: new command "invoke"
        09: underlined accelerators
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static void stubOnActivate ()
{
}

/****v* button/options
 * AUTHOR
 *	PGB
 * SOURCE
 */
static GnoclOption buttonOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },        /* 0 */
	{ "-icon", GNOCL_OBJ, NULL },        /* 1 */
	{ "-activeBackgroundColor", GNOCL_OBJ, "active", gnoclOptGdkColorBg },

	/* GtkObject Properties */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-name", GNOCL_STRING, "name" },
	{ "-normalBackgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },

	/* GtkButton specific signals */
	{ "-onEnter", GNOCL_OBJ, "E", gnoclOptOnEnterLeave },
	{ "-onLeave", GNOCL_OBJ, "L", gnoclOptOnEnterLeave },
	{ "-onClicked", GNOCL_OBJ, "clicked", gnoclOptCommand },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },

	/* GtkWidget signals */
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },

	{ "-prelightBackgroundColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorBg },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },

	{ NULL },
};
/*****/

/****v* button/local
 * AUTHOR
 *	PGB
 * SOURCE
 */
static const int textIdx = 0;
static const int iconIdx = 1;
/*****/

/****f* button/configure
 * AUTHOR
 *	PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkButton *button, GnoclOption options[] )
{
	if ( options[textIdx].status == GNOCL_STATUS_CHANGED && gnoclConfigButtonText ( interp, button, options[textIdx].val.obj ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	if ( options[iconIdx].status == GNOCL_STATUS_CHANGED )
	{
		GnoclStringType type = gnoclGetStringType ( options[iconIdx].val.obj );
		GtkWidget *label = gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_LABEL );

		if ( type == GNOCL_STR_EMPTY )
		{
			/* remove all children apart from label */
			GtkWidget *child = gtk_bin_get_child ( GTK_BIN ( button ) );

			if ( child && ( child != label ) )
			{
				gtk_widget_ref ( label );
				gtk_container_remove ( GTK_CONTAINER ( button ), child );
				gtk_container_add ( GTK_CONTAINER ( button ), label );
				gtk_widget_unref ( label );
				gtk_widget_show ( label );
			}
		}

		else
		{
			GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_IMAGE );

			if ( label == NULL )
			{
				gtk_button_set_label ( button, "" );
				label = gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_LABEL );
			}

			else if ( ( type & ( GNOCL_STR_STOCK | GNOCL_STR_FILE ) ) == 0 )
			{
				Tcl_AppendResult ( interp, "Unknown type for \"",
								   Tcl_GetString ( options[iconIdx].val.obj ),
								   "\" must be of type FILE (%/) or STOCK (%#)", NULL );
				return TCL_ERROR;

			}

			if ( image == NULL )
			{
				/* this should match gtkbutton.c */
				GtkWidget *hbox = gtk_hbox_new ( 0, 2 );
				GtkWidget *align = gtk_alignment_new ( 0.5, 0.5, 0.0, 0.0 );
				image = gtk_image_new( );

				gtk_box_pack_start ( GTK_BOX ( hbox ), image, 0, 0, 0 );

				gtk_widget_ref ( label );
				gtk_container_remove ( GTK_CONTAINER ( button ), label );
				gtk_box_pack_end ( GTK_BOX ( hbox ), label, 0, 0, 0 );
				gtk_widget_unref ( label );

				gtk_container_add ( GTK_CONTAINER ( button ), align );
				gtk_container_add ( GTK_CONTAINER ( align ), hbox );
				gtk_widget_show_all ( align );
			}

			if ( type & GNOCL_STR_STOCK )
			{
				GtkStockItem item;

				if ( gnoclGetStockItem ( options[iconIdx].val.obj, interp, &item ) != TCL_OK )
				{
					return TCL_ERROR;
				}

				gtk_image_set_from_stock ( GTK_IMAGE ( image ), item.stock_id, GTK_ICON_SIZE_BUTTON );
			}

			else if ( type & GNOCL_STR_FILE )
			{
				GdkPixbuf *pix = gnoclPixbufFromObj ( interp, options + iconIdx );

				if ( pix == NULL )
				{
					return TCL_ERROR;
				}

				gtk_image_set_from_pixbuf ( GTK_IMAGE ( image ), pix );
			}
		}
	}

	return TCL_OK;
}

/*****/

/****f* button/gnoclConfigButtonText
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclConfigButtonText ( Tcl_Interp *interp, GtkButton *button, Tcl_Obj *txtObj )
{
	GnoclStringType type = gnoclGetStringType ( txtObj );

	if ( type & GNOCL_STR_STOCK )
	{
		GtkStockItem sp;

		if ( gnoclGetStockItem ( txtObj, interp, &sp ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		gtk_button_set_label ( button, sp.stock_id );

		gtk_button_set_use_stock ( button, 1 );
	}

	else
	{
		GtkLabel *label;
		char *txt = gnoclGetString ( txtObj );

		gtk_button_set_label ( button, txt );
		gtk_button_set_use_stock ( button, 0 );

		label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_LABEL ) );
		assert ( label );

		/* TODO? pango_parse_markup for error message */
		gtk_label_set_use_markup ( label, ( type & GNOCL_STR_MARKUP ) != 0 );
		gtk_label_set_use_underline ( label, ( type & GNOCL_STR_UNDERLINE ) != 0 );
	}

	return TCL_OK;
}

/*****/

/****f* button/gnoclCgetButtonText
 * AUTHOR
 *	PGB
 * SOURCE
 */
Tcl_Obj *gnoclCgetButtonText ( Tcl_Interp *interp, GtkButton *button )
{
	Tcl_Obj *obj = NULL;

	if ( gtk_button_get_use_stock ( button ) )
	{
		const char *st = gtk_button_get_label ( button );

		if ( st == NULL )
		{
			obj = Tcl_NewStringObj ( "", 0 );
		}

		else
		{
			obj = Tcl_NewStringObj ( "%#", 2 );
			Tcl_AppendObjToObj ( obj, gnoclGtkToStockName ( st ) );
		}
	}

	else
	{
		GtkLabel *label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_LABEL ) );
		Tcl_Obj *old = Tcl_NewStringObj ( gtk_label_get_label ( label ), -1 );
		assert ( label );

		if ( gtk_label_get_use_markup ( label ) )
		{
			obj = Tcl_NewStringObj ( "%<", 2 );
			Tcl_AppendObjToObj ( obj, old );
		}

		else if ( gtk_label_get_use_underline ( label ) )
		{
			obj = Tcl_NewStringObj ( "%_", 2 );
			Tcl_AppendObjToObj ( obj, old );
		}

		else
		{
			obj = old;
		}
	}

	return obj;
}

/*****/

/****f* button/cget
 * NAME
 *	cget
 * PURPOSE
 * AUTHOR
 *	PGB
 * DATE
 * 	When created
 * USAGE
 *	how this function is used
 * SYNOPSIS
 *	static int cget (
 * 		Tcl_Interp *interp, GtkButton *button,
 * 		GnoclOption options[],int idx )
 * ARGUMENTS
 * RETURN VALUE
 * 	None
 * NOTE
 * 	This function is NOT called when cget colour values made.
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
static int cget (
	Tcl_Interp *interp,
	GtkButton *button,
	GnoclOption options[],
	int idx )
{

	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
	{
		obj = gnoclCgetButtonText ( interp, button );
	}

	else if ( idx == iconIdx )
	{
		GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( button ), GTK_TYPE_IMAGE );

		if ( image == NULL )
		{
			obj = Tcl_NewStringObj ( "", 0 );
		}

		else
		{
			gchar   *st;
			g_object_get ( G_OBJECT ( image ), "stock", &st, NULL );

			if ( st )
			{
				obj = Tcl_NewStringObj ( "%#", 2 );
				Tcl_AppendObjToObj ( obj, gnoclGtkToStockName ( st ) );
				g_free ( st );
			}

			else
			{
				Tcl_SetResult ( interp, "Could not determine icon type.", TCL_STATIC );
				return TCL_ERROR;
			}
		}
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/****f* button/buttonFunc
 * NAME
 *	buttonFunc
 * PURPOSE
 * AUTHOR
 *	PGB
 * DATE
 * 	When created
 * USAGE
 *	how this function is used
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
static int buttonFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	static const char *cmds[] =
	{
		"delete", "configure", "cget", "onClicked",
		"class", "parent", "geometry", "toplevel", NULL
	};

	enum cmdIdx
	{
		DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx,
		ClassIdx, ParentIdx, GeometryIdx, ToplevelIdx
	};

	GtkButton *button = GTK_BUTTON ( data );
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
		case ToplevelIdx:
			{
				g_print ( "button ToplevelIdx\n" );
				GtkWidget *toplevel;
				Tcl_Obj *obj = NULL;
				toplevel = gtk_widget_get_toplevel  ( button ) ;
				obj = Tcl_NewStringObj ( gnoclGetNameFromWidget ( toplevel ), -1 );
				Tcl_SetObjResult ( interp, obj );
				return TCL_OK;
			}

			break;
		case GeometryIdx:
			{
				g_print ( "button GeometryIdx\n" );
				char *txt = gnoclGetWidgetGeometry ( button ) ;
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( txt , -1 ) );
				return TCL_OK;
			}

			break;
		case ParentIdx:
			{

				GtkWidget * parent;
				Tcl_Obj *obj = NULL;
				parent = gtk_widget_get_parent ( GTK_WIDGET ( button ) );
				obj = Tcl_NewStringObj ( gnoclGetNameFromWidget ( parent ), -1 );
				Tcl_SetObjResult ( interp, obj );

				/* this function not working too well! */
				/*	return gnoclGetParent ( interp, data ); */
				return TCL_OK;
			}

			break;
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "button", -1 ) );
			break;

		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( button ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, buttonOptions, G_OBJECT ( button ) ) == TCL_OK )
				{
					ret = configure ( interp, button, buttonOptions );
				}

				gnoclClearOptions ( buttonOptions );

				return ret;
			}

			break;

		case OnClickedIdx:

			if ( objc != 2 )
			{
				Tcl_WrongNumArgs ( interp, 2, objv, NULL );
				return TCL_ERROR;
			}

			if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( button ) ) )
			{
				gtk_button_clicked ( button );
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( button ), buttonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, button, buttonOptions, idx );
				}
			}
	}

	return TCL_OK;
}

/*****/

/****f* button/gnoclButtonCmd
 * AUTHOR
 *	PGB
 * SOURCE
 */
int gnoclButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkButton *button;

	/* check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, buttonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( buttonOptions );
		return TCL_ERROR;
	}

	/* create an instance of the widget itself */
	button = GTK_BUTTON ( gtk_button_new( ) );

	gtk_widget_show ( GTK_WIDGET ( button ) );

	ret = gnoclSetOptions ( interp, buttonOptions, G_OBJECT ( button ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, button, buttonOptions );
	}

	gnoclClearOptions ( buttonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( button ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( button ), buttonFunc );
}

/*****/
