/**
\brief      This module implements the gnocl::recentChooser widget.
\author     William J Giddings
\since      20/12/09
**/

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

static GnoclOption recentChooserOptions[] =
{
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ NULL },
};

/**
\brief
\author     William J Giddings
**/
static int configure ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[] )
{

}

/**
\brief
\author     William J Giddings
**/
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
\brief
\author     William J Giddings
**/
int recentChooserFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	printf ( "widgetFunc\n" );

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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "recentChooser", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				ret = configure ( interp, widget, recentChooserOptions );

				if ( 1 )
				{
					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, recentChooserOptions, G_OBJECT ( widget ) ) == TCL_OK )
					{
						ret = configure ( interp, widget, recentChooserOptions );
					}
				}

				gnoclClearOptions ( recentChooserOptions );

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

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), recentChooserOptions, &idx ) )
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
							return cget ( interp, widget, recentChooserOptions, idx );
						}
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author     William J Giddings
**/
int gnoclRecentChooserCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int            ret = TCL_OK;
	GtkWidget      *widget;

	if ( 0 )
	{
		if ( gnoclParseOptions ( interp, objc, objv, recentChooserOptions ) != TCL_OK )
		{
			gnoclClearOptions ( recentChooserOptions );
			return TCL_ERROR;
		}
	}

	widget = gtk_recent_chooser_widget_new() ;

	gtk_widget_show ( GTK_WIDGET ( widget ) );


	if ( 0 )
	{
		ret = gnoclSetOptions ( interp, recentChooserOptions, G_OBJECT ( widget ), -1 );

		if ( ret == TCL_OK )
		{
			ret = configure ( interp, G_OBJECT ( widget ), recentChooserOptions );
		}

		gnoclClearOptions ( recentChooserOptions );

		/* STEP 3)  -show the widget */

		if ( ret != TCL_OK )
		{
			gtk_widget_destroy ( GTK_WIDGET ( widget ) );
			return TCL_ERROR;
		}
	}

	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), recentChooserFunc );
}

/*****/
