/**
 \brief         This module implements the gnocl::iconView
 \author       William J Giddings
 \date          20/05/09
*/

/** \page page3 gnocl::arrowButton
  \section sec DESCRIPTION
  Implementation of...
  \subsection subsection1 Implemented XXX gnocl::arrowButton
  \subsection subsection2 Implemented XXX gnocl::arrowButton

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/arrowButton.png"
*/


#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
 \brief     This module implements the gnocl::iconView
 \author    William J Giddings
 \date      20/05/09
*/
static int gnoclOptLoadIcons  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	g_print ( "gnoclOptLoadIcons\n" );

	return TCL_OK;
}

/*
  "column-spacing"           gint                  : Read / Write
  "columns"                  gint                  : Read / Write
  "item-width"               gint                  : Read / Write
  "margin"                   gint                  : Read / Write
  "markup-column"            gint                  : Read / Write
  "model"                    GtkTreeModel*         : Read / Write
  "orientation"              GtkOrientation        : Read / Write
  "pixbuf-column"            gint                  : Read / Write
  "reorderable"              gboolean              : Read / Write
  "row-spacing"              gint                  : Read / Write
  "selection-mode"           GtkSelectionMode      : Read / Write
  "spacing"                  gint                  : Read / Write
  "text-column"              gint                  : Read / Write
  "tooltip-column"           gint                  : Read / Write
*/

/*
  "activate-cursor-item"                           : Run Last / Action
  "item-activated"                                 : Run Last
  "move-cursor"                                    : Run Last / Action
  "select-all"                                     : Run Last / Action
  "select-cursor-item"                             : Run Last / Action
  "selection-changed"                              : Run First
  "set-scroll-adjustments"                         : Run Last
  "toggle-cursor-item"                             : Run Last / Action
  "unselect-all"                                   : Run Last / Action
*/

static GnoclOption iconViewOptions[] =
{

	/* custom option */
	{"-icons", GNOCL_OBJ, "", gnoclOptLoadIcons},

	/* widget signals */
	{ "-onActivateCursorItem", GNOCL_OBJ, "", NULL },
	{ "-onItemActivated", GNOCL_OBJ, "", NULL },
	{ "-onMoveCursor", GNOCL_OBJ, "", NULL },
	{ "-onSelectAll", GNOCL_OBJ, "", NULL },
	{ "-onSelectCursorIem", GNOCL_OBJ, "", NULL },
	{ "-onSelectionChanged", GNOCL_OBJ, "", NULL },
	{ "-onSetScrollAdjustments", GNOCL_OBJ, "", NULL },
	{ "-onToggleCursorItem", GNOCL_OBJ, "", NULL },
	{ "-onUnselectAll", GNOCL_OBJ, "", NULL },

	/* widget properties */
	{ "-columnSpacing", GNOCL_INT, "column-spacing" },  /* space between icon columns */
	{ "-columns", GNOCL_INT, "columns" },
	{ "-itemWidth", GNOCL_INT, "item-width" },
	{ "-margin", GNOCL_INT, "margin" },                 /* spacing between icons and frame */
	{ "-markupColumn", GNOCL_INT, "markup-column" },
	{ "-model", GNOCL_INT, NULL },
	{ "-orientation", GNOCL_INT, NULL },
	{ "-pixbufColumn", GNOCL_INT, "picbuf-column" },
	{ "-reoderable", GNOCL_BOOL, "reorderable" },
	{ "-rowSpacing", GNOCL_INT, "row-spacing" },
	{ "-selectionMode", GNOCL_INT, NULL },
	{ "-spacing", GNOCL_INT, "spacing" },
	{ "-textColumn", GNOCL_INT, "text-column" },
	{ "-tooltipColumn", GNOCL_INT, "tooltip-column" },

	/* inherited properties */

	{ NULL },
};


enum
{
	COL_DISPLAY_NAME,
	COL_PIXBUF,
	NUM_COLS
};

/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
/* Create the ListStore and fill it with required data */
GtkTreeModel * create_and_fill_model ( void )
{
	GtkListStore *list_store;
	GdkPixbuf *p1, *p2;
	GtkTreeIter iter;
	GError *err = NULL;
	int i = 0;

	p1 = gdk_pixbuf_new_from_file ( "wjg.png", &err ); /* No error checking is done here */
	p2 = gdk_pixbuf_new_from_file ( "gnocl.png", &err );

	list_store = gtk_list_store_new ( NUM_COLS, G_TYPE_STRING, GDK_TYPE_PIXBUF );

	do
	{
		gtk_list_store_append ( list_store, &iter );
		gtk_list_store_set ( list_store, &iter, COL_DISPLAY_NAME, "wjg.png", COL_PIXBUF, p1, -1 );
		gtk_list_store_append ( list_store, &iter );
		gtk_list_store_set ( list_store, &iter, COL_DISPLAY_NAME, "gnocl.png", COL_PIXBUF, p2, -1 );
	}

	while ( i++ < 100 );

	return GTK_TREE_MODEL ( list_store );
}


/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
static int configure ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[] )
{

	const char *dataIDa = "gnocl::data1";
	const char *dataIDb = "gnocl::data2";

	GtkWidget *arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_BUTTON );
	char *str;

	/* this will enable us to obtain widget data for the arrow object */
	char *result = g_object_get_data ( G_OBJECT ( arrow ), dataIDa );
	printf ( "cget result = %s\n", result );

	result = g_object_get_data ( G_OBJECT ( arrow ), dataIDb );
	printf ( "cget result = %s\n", result );

	str = gnoclGetNameFromWidget ( arrow );
	printf ( "configure %s\n", str );
	return TCL_OK;
}

/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
int gnoclConfigIconView ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj )
{
	printf ( "gnoclConfigArrowButton\n" );

	GtkArrow *arrow;

	/* the arrow is the child of the widget */

	arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_ARROW ) ;
	gtk_arrow_set ( arrow, GTK_ARROW_DOWN, GTK_SHADOW_OUT );

	return TCL_OK;
}

/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
static int cget ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[], int idx )
{
	printf ( "cget\n" );

	const char *dataIDa = "gnocl::data1";
	const char *dataIDb = "gnocl::data2";

	GtkWidget *arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_BUTTON );
	char *str;

	/* this will enable us to obtain widget data for the arrow object */
	char *result = g_object_get_data ( G_OBJECT ( arrow ), dataIDa );
	printf ( "cget result = %s\n", result );

	result = g_object_get_data ( G_OBJECT ( arrow ), dataIDb );
	printf ( "cget result = %s\n", result );

	str = gnoclGetNameFromWidget ( arrow );
	printf ( "configure %s\n", str );


	/*-----*/
	/*
	    Tcl_Obj *obj = NULL;

	    if ( idx == textIdx )
	    {
	        obj = gnoclCgetButtonText ( interp, widget );
	    }

	    else if ( idx == iconsIdx )
	    {
	        GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_IMAGE );

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
	*/


	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
int iconViewFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	printf ( "iconViewFunc\n" );

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "arrowButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				ret = configure ( interp, widget, iconViewOptions );

				if ( 1 )
				{
					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, iconViewOptions, G_OBJECT ( widget ) ) == TCL_OK )
					{
						ret = configure ( interp, widget, iconViewOptions );
					}
				}

				gnoclClearOptions ( iconViewOptions );

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

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), iconViewOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						{
							return TCL_ERROR;
						}

					case GNOCL_CGET_HANDLED:
						{
							return TCL_OK;
						}

					case GNOCL_CGET_NOTHANDLED:
						{
							return cget ( interp, widget, iconViewOptions, idx );
						}
				}
			}
	}

	return TCL_OK;
}

/**
 \brief         This module implements the gnocl::iconView
 \authors       William J Giddings
 \date          20/05/09
*/
int gnoclIconViewCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int            ret = TCL_OK;
	GtkWidget      *iconView;
	GtkWidget      *scrolledWindow;

	if ( 1 )
	{
		if ( gnoclParseOptions ( interp, objc, objv, iconViewOptions ) != TCL_OK )
		{
			gnoclClearOptions ( iconViewOptions );
			return TCL_ERROR;
		}
	}


	if ( 0 )
	{
		iconView = gtk_icon_view_new() ;
		gtk_widget_show ( GTK_WIDGET ( iconView ) );
	}

	else
	{

		scrolledWindow = gtk_scrolled_window_new ( NULL, NULL );

		gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolledWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
		gtk_scrolled_window_set_shadow_type ( GTK_SCROLLED_WINDOW ( scrolledWindow ), GTK_SHADOW_IN );

		iconView = gtk_icon_view_new_with_model ( create_and_fill_model () );
		gtk_container_add ( GTK_CONTAINER ( scrolledWindow ), iconView );

		if ( 1 )
		{
			gtk_icon_view_set_text_column ( GTK_ICON_VIEW ( iconView ), COL_DISPLAY_NAME );
			gtk_icon_view_set_pixbuf_column ( GTK_ICON_VIEW ( iconView ), COL_PIXBUF );
			gtk_icon_view_set_selection_mode ( GTK_ICON_VIEW ( iconView ), GTK_SELECTION_MULTIPLE );
		}


		if ( 0 )
		{
			iconView = gtk_icon_view_new_with_model ( create_and_fill_model () );
			gtk_container_add ( GTK_CONTAINER ( scrolledWindow ), iconView );

			gtk_icon_view_set_text_column ( GTK_ICON_VIEW ( iconView ), COL_DISPLAY_NAME );
			gtk_icon_view_set_pixbuf_column ( GTK_ICON_VIEW ( iconView ), COL_PIXBUF );
			gtk_icon_view_set_selection_mode ( GTK_ICON_VIEW ( iconView ), GTK_SELECTION_MULTIPLE );
		}

		gtk_widget_show_all ( scrolledWindow );

	}


	if ( 1 )
	{
		ret = gnoclSetOptions ( interp, iconViewOptions, G_OBJECT ( iconView ), -1 );

		if ( ret == TCL_OK )
		{
			ret = configure ( interp, G_OBJECT ( iconView ), iconViewOptions );
		}

		gnoclClearOptions ( iconViewOptions );

		/* STEP 3)  -show the widget */

		if ( ret != TCL_OK )
		{
			gtk_widget_destroy ( GTK_WIDGET ( iconView ) );
			return TCL_ERROR;
		}
	}

	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( scrolledWindow ), iconViewFunc );
}
