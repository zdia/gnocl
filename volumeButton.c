/**
\brief     This module implements the gnocl::volumeButton widget.
\authors    Peter G. Baum, William J Giddings
\date      2001-03:
**/

/* user documentation */

/**
  \page page50 gnocl::volumeButton
  \section sec gnocl::volumeButton
  Implementation of gnocl::volumeButton
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
  \image html "../pics/volumeButton.png"
*/


#include "gnocl.h"
#include <string.h>
#include <assert.h>

GnoclOption volumeButtonOptions[] =
{
	/* Properties inherited from GtkScaleButton*/
	{ "-adjustment", GNOCL_DOUBLE, "adjustment" },
	{ "-size", GNOCL_OBJ, "size" },
	{ "-value", GNOCL_DOUBLE, "value"},
	{ "-icons", GNOCL_OBJ, "icons", gnoclOptIcon },         /* 1 */


	{ "-onValueChanged", GNOCL_STRING, "", gnoclOptOnValueChanged },  /* 2 */

	/* other utltility options */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },

	{ NULL },
};

static const int textIdx = 0;
static const int iconIdx = 1;

/**
\brief
\author
\date
**/
static int configure ( Tcl_Interp *interp, GtkWidget *widget,
					   GnoclOption options[] )
{

	//if( options[textIdx].status == GNOCL_STATUS_CHANGED
	//&& gnoclConfigVolumeButtonText( interp, widget, options[textIdx].val.obj )
	//!= TCL_OK )
	//{
	//return TCL_ERROR;
	//}

	//if( options[iconIdx].status == GNOCL_STATUS_CHANGED )
	//{
	//GnoclStringType type = gnoclGetStringType( options[iconIdx].val.obj );
	//GtkWidget *label = gnoclFindChild( GTK_WIDGET( widget ), GTK_TYPE_LABEL );

	//if( type == GNOCL_STR_EMPTY )
	//{
	///* remove all children apart from label */
	//GtkWidget *child = gtk_bin_get_child( GTK_BIN( widget ) );
	//if( child && (child != label) )
	//{
	//gtk_widget_ref( label );
	//gtk_container_remove( GTK_CONTAINER( widget ), child );
	//gtk_container_add( GTK_CONTAINER( widget ), label );
	//gtk_widget_unref( label );
	//gtk_widget_show( label );
	//}
	//}
	//else
	//{
	//GtkWidget *image = gnoclFindChild( GTK_BUTTON( widget ),
	//GTK_TYPE_IMAGE );
	//if( label == NULL )
	//{
	//gtk_link_button_set_uri( widget, "" );
	//label = gnoclFindChild( GTK_WIDGET( widget ), GTK_TYPE_LABEL );
	//}
	//else if( (type & (GNOCL_STR_STOCK|GNOCL_STR_FILE)) == 0 )
	//{
	//Tcl_AppendResult( interp, "Unknown type for \"",
	//Tcl_GetString( options[iconIdx].val.obj ),
	//"\" must be of type FILE (%/) or STOCK (%#)", NULL );
	//return TCL_ERROR;

	//}

	//if( image == NULL )
	//{
	///* this should match GtkWidget.c */
	//GtkWidget *hbox = gtk_hbox_new( 0, 2 );
	//GtkWidget *align = gtk_alignment_new( 0.5, 0.5, 0.0, 0.0 );
	//image = gtk_image_new( );

	//gtk_box_pack_start( GTK_BOX( hbox ), image, 0, 0, 0);

	//gtk_widget_ref( label );
	//gtk_container_remove( GTK_CONTAINER( widget ), label );
	//gtk_box_pack_end( GTK_BOX( hbox ), label, 0, 0, 0);
	//gtk_widget_unref( label );

	//gtk_container_add( GTK_CONTAINER( widget ), align );
	//gtk_container_add( GTK_CONTAINER( align ), hbox );
	//gtk_widget_show_all( align );
	//}

	//if( type & GNOCL_STR_STOCK )
	//{
	//GtkStockItem item;
	//if( gnoclGetStockItem( options[iconIdx].val.obj, interp, &item )
	//!= TCL_OK )
	//return TCL_ERROR;
	//gtk_image_set_from_stock( GTK_IMAGE( image ), item.stock_id,
	//GTK_ICON_SIZE_widget );
	//}
	//else if( type & GNOCL_STR_FILE )
	//{
	//GdkPixbuf *pix = gnoclPixbufFromObj( interp, options + iconIdx );
	//if( pix == NULL )
	//return TCL_ERROR;
	//gtk_image_set_from_pixbuf( GTK_IMAGE( image ), pix );
	//}
	//}
	//}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
int gnoclConfigVolumeButtonText ( Tcl_Interp *interp, GtkWidget *widget,
								  Tcl_Obj *txtObj )
{
	GnoclStringType type = gnoclGetStringType ( txtObj );

	if ( type & GNOCL_STR_STOCK )
	{
		GtkStockItem sp;

		if ( gnoclGetStockItem ( txtObj, interp, &sp ) != TCL_OK )
			return TCL_ERROR;

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
		gtk_label_set_use_underline ( label,
									  ( type & GNOCL_STR_UNDERLINE ) != 0 );
	}

	return TCL_OK;
}

/**
\brief
\author
\date
**/
Tcl_Obj *gnoclCgetVolumeButtonText ( Tcl_Interp *interp, GtkWidget *widget )
{
	Tcl_Obj *obj = NULL;

	if ( gtk_button_get_use_stock ( widget ) )
	{
		const char *st = gtk_button_get_label ( widget );

		if ( st == NULL )
			obj = Tcl_NewStringObj ( "", 0 );
		else
		{
			obj = Tcl_NewStringObj ( "%#", 2 );
			Tcl_AppendObjToObj ( obj, gnoclGtkToStockName ( st ) );
		}
	}

	else
	{
		GtkLabel *label = GTK_LABEL ( gnoclFindChild ( GTK_WIDGET ( widget ),
									  GTK_TYPE_LABEL ) );
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
static int cget ( Tcl_Interp *interp, GtkWidget *widget,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == textIdx )
		obj = gnoclCgetVolumeButtonText ( interp, widget );
	else if ( idx == iconIdx )
	{
		GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( widget ),
											GTK_TYPE_IMAGE );

		if ( image == NULL )
			obj = Tcl_NewStringObj ( "", 0 );
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
int volumeButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
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

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "volumeButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   volumeButtonOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, widget, volumeButtonOptions );
				}

				gnoclClearOptions ( volumeButtonOptions );

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
				gtk_button_clicked ( widget );

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ),
									 volumeButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, widget, volumeButtonOptions, idx );
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
int gnoclVolumeButtonCmd ( ClientData data, Tcl_Interp *interp,
						   int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkWidget *widget;

	if ( gnoclParseOptions ( interp, objc, objv, volumeButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( volumeButtonOptions );
		return TCL_ERROR;
	}

	printf ( "vb1\n" );

	/*
	GtkWidget*          gtk_volume_button_new               (void);
	 */

	/* TODO: how to change the size? */
	GtkIconSize size = GTK_ICON_SIZE_BUTTON;

	printf ( "vb2\n" );

	widget = gtk_volume_button_new () ;

	printf ( "vb3\n" );

	gtk_widget_show ( GTK_WIDGET ( widget ) );

	printf ( "vb4\n" );

	ret = gnoclSetOptions ( interp, volumeButtonOptions, G_OBJECT ( widget ), -1 );

	printf ( "vb5\n" );

	if ( ret == TCL_OK )
		ret = configure ( interp, widget, volumeButtonOptions );

	printf ( "vb6\n" );

	gnoclClearOptions ( volumeButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( widget ) );
		return TCL_ERROR;
	}

	printf ( "vb7\n" );

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), volumeButtonFunc );
}
