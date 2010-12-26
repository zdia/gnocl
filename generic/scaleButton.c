/****h* widget/scaleButton
 * NAME
 *  scaleButton.c
 * SYNOPSIS
 *  This file implements the scaleButton widget.
 * FUNCTION
 * NOTES
 *  Modification of the scaleButton widget module.
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/scaleButton.png">
 * SEE ALSO
 *****/

/*
 * $Id: widget.c,v 1.17 2005/02/25 21:33:38 baum Exp $
 *
 * This file implements the widget widget
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
   2008-06: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption scaleButtonOptions[] =
{
	/* GtkScaleButton specific properties */
	{ "-adjustment", GNOCL_OBJ, NULL },
	{ "-size", GNOCL_OBJ, NULL },
	{ "-value", GNOCL_OBJ, NULL},
	{ "-icons", GNOCL_OBJ, "icons", gnoclOptIcons },            /* 1 */

	/* GtkScaleButton specific events */
	{ "-onValueChanged", GNOCL_OBJ, "", gnoclOptOnValueChanged },
	{ "-onPopDown", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onPopUp", GNOCL_OBJ, "P", gnoclOptOnButton },

	/* Inherited properties */
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },

	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },

	/* GtkObject Properties */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },

	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-name", GNOCL_STRING, "name" },
	{ "-normalBackgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },

	{ "-prelightBackgroundColor", GNOCL_OBJ, "prelight", gnoclOptGdkColorBg },
	{ "-relief", GNOCL_OBJ, "relief", gnoclOptRelief },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },

	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },

	/* TODO?: for all widgets,
	   or only for box/frame: {min,max}{Width,Height}, resizeInc, ...
	{ "-width", GNOCL_INT, "width-request" },
	{ "-height", GNOCL_INT, "height-request" },
	*/
	{ NULL },
};

static const int textIdx = 0;
static const int iconsIdx = 1;

/**
\brief
\author
\date
**/
static int configure ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[] )
{

	if ( options[textIdx].status == GNOCL_STATUS_CHANGED
			&& gnoclConfigScaleButtonText ( interp, widget, options[textIdx].val.obj )
			!= TCL_OK )
	{
		return TCL_ERROR;
	}

	if ( options[iconsIdx].status == GNOCL_STATUS_CHANGED )
	{
		GnoclStringType type = gnoclGetStringType ( options[iconsIdx].val.obj );
		GtkWidget *label = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_LABEL );

		if ( type == GNOCL_STR_EMPTY )
		{
			/* remove all children apart from label */
			GtkWidget *child = gtk_bin_get_child ( GTK_BIN ( widget ) );

			if ( child && ( child != label ) )
			{
				gtk_widget_ref ( label );
				gtk_container_remove ( GTK_CONTAINER ( widget ), child );
				gtk_container_add ( GTK_CONTAINER ( widget ), label );
				gtk_widget_unref ( label );
				gtk_widget_show ( label );
			}
		}

		else
		{
			GtkWidget *image = gnoclFindChild ( GTK_BUTTON ( widget ), GTK_TYPE_IMAGE );

			if ( label == NULL )
			{
				gtk_link_button_set_uri ( widget, "" );
				label = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_LABEL );
			}

			else if ( ( type & ( GNOCL_STR_STOCK | GNOCL_STR_FILE ) ) == 0 )
			{
				Tcl_AppendResult ( interp, "Unknown type for \"",
								   Tcl_GetString ( options[iconsIdx].val.obj ),
								   "\" must be of type FILE (%/) or STOCK (%#)", NULL );
				return TCL_ERROR;

			}

			if ( image == NULL )
			{
				/* this should match GtkWidget.c */
				GtkWidget *hbox = gtk_hbox_new ( 0, 2 );
				GtkWidget *align = gtk_alignment_new ( 0.5, 0.5, 0.0, 0.0 );
				image = gtk_image_new( );

				gtk_box_pack_start ( GTK_BOX ( hbox ), image, 0, 0, 0 );

				gtk_widget_ref ( label );
				gtk_container_remove ( GTK_CONTAINER ( widget ), label );
				gtk_box_pack_end ( GTK_BOX ( hbox ), label, 0, 0, 0 );
				gtk_widget_unref ( label );

				gtk_container_add ( GTK_CONTAINER ( widget ), align );
				gtk_container_add ( GTK_CONTAINER ( align ), hbox );
				gtk_widget_show_all ( align );
			}

			if ( type & GNOCL_STR_STOCK )
			{
				GtkStockItem item;

				if ( gnoclGetStockItem ( options[iconsIdx].val.obj, interp, &item ) != TCL_OK )
				{
					return TCL_ERROR;
				}

				gtk_image_set_from_stock ( GTK_IMAGE ( image ), item.stock_id, GTK_ICON_SIZE_BUTTON );


			}

			else if ( type & GNOCL_STR_FILE )
			{
				GdkPixbuf *pix = gnoclPixbufFromObj ( interp, options + iconsIdx );

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

/**
\brief
\author
\date
**/
int gnoclConfigScaleButtonText ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj )
{
	GnoclStringType type = gnoclGetStringType ( txtObj );

	if ( type & GNOCL_STR_STOCK )
	{
		GtkStockItem sp;

		if ( gnoclGetStockItem ( txtObj, interp, &sp ) != TCL_OK )
		{
			return TCL_ERROR;
		}

		gtk_button_set_label ( widget, sp.stock_id );

		gtk_button_set_use_stock ( widget, 1 );
	}

	else
	{
		GtkLabel *label;
		char *txt = gnoclGetString ( txtObj );

		gtk_button_set_label ( widget, txt );
		gtk_button_set_use_stock ( widget, 0 );

		label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( widget ),
											 GTK_TYPE_LABEL ) );
		assert ( label );
		/* TODO? pango_parse_markup for error message */
		gtk_label_set_use_markup ( label, ( type & GNOCL_STR_MARKUP ) != 0 );
		gtk_label_set_use_underline ( label, ( type & GNOCL_STR_UNDERLINE ) != 0 );
	}

	return TCL_OK;
}

/*****/

/****f* widget/gnoclCgetScaleButtonText
 * AUTHOR
 *  PGB
 * SOURCE
 */
Tcl_Obj *gnoclCgetScaleButtonText ( Tcl_Interp *interp, GtkWidget *widget )
{
	Tcl_Obj *obj = NULL;

	if ( gtk_button_get_use_stock ( widget ) )
	{
		const char *st = gtk_button_get_label ( widget );

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
		GtkLabel *label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_LABEL ) );
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
			obj = old;
	}

	return obj;
}

/**
\brief
\author
\date
**/
static int cget ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
	{
		obj = gnoclCgetScaleButtonText ( interp, widget );
	}

	else if ( idx == iconsIdx )
	{
		GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( widget ),
											GTK_TYPE_IMAGE );

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
				Tcl_SetResult ( interp, "Could not determine icon type.",
								TCL_STATIC );
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

/**
\brief
\author
\date
**/
int scaleButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx };
	GtkWidget *widget = GTK_WIDGET ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "scaleButton", -1 ) );
			break;
		case DeleteIdx:
			{
				return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );
			}

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, scaleButtonOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, widget, scaleButtonOptions );
				}

				gnoclClearOptions ( scaleButtonOptions );

				return ret;
			}

			break;
		case OnClickedIdx:

			if ( objc != 2 )
			{
				Tcl_WrongNumArgs ( interp, 2, objv, NULL );
				return TCL_ERROR;
			}

			if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( widget ) ) )
			{
				gtk_button_clicked ( widget );
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), scaleButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, widget, scaleButtonOptions, idx );
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
int gnoclScaleButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkWidget *widget;

	if ( gnoclParseOptions ( interp, objc, objv, scaleButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( scaleButtonOptions );
		return TCL_ERROR;
	}

	/* TODO: how to change the size? */
	GtkIconSize size = GTK_ICON_SIZE_BUTTON;

	static const gchar *icons[] =
	{
		GTK_STOCK_DIALOG_QUESTION
	};

	static char *icons_[] =
	{
		GTK_STOCK_ZOOM_IN,
		GTK_STOCK_ZOOM_OUT,
		GTK_STOCK_ZOOM_100,
	};

	/* create the button and set a default valued */
	widget = GTK_WIDGET ( gtk_scale_button_new ( size, 0, 100, 5, NULL ) );

	gtk_widget_add_events ( widget, GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );


	gtk_scale_button_set_icons ( GTK_SCALE_BUTTON ( widget ), icons_ );


	gtk_scale_button_set_value ( widget, 1.0 );

	gtk_widget_show ( GTK_WIDGET ( widget ) );

	ret = gnoclSetOptions ( interp, scaleButtonOptions, G_OBJECT ( widget ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, widget, scaleButtonOptions );
	}

	gnoclClearOptions ( scaleButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( widget ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), scaleButtonFunc );
}

/*****/
