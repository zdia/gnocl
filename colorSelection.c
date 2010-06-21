/**
\brief      This module implements the gnocl::fileChooser widget.
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

static GnoclOption colorSelectionOptions[] =
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
	return gnoclCgetNotImplemented ( interp, options );
}

/**
\brief
\author     William J Giddings
**/
static int cget ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[], int idx )
{
	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief
\author     William J Giddings
**/
int colorSelectionFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "fileChooser", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				ret = configure ( interp, widget, colorSelectionOptions );

				if ( 1 )
				{
					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, colorSelectionOptions, G_OBJECT ( widget ) ) == TCL_OK )
					{
						ret = configure ( interp, widget, colorSelectionOptions );
					}
				}

				gnoclClearOptions ( colorSelectionOptions );

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

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), colorSelectionOptions, &idx ) )
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
							return cget ( interp, widget, colorSelectionOptions, idx );
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
int gnoclColorSelectionCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int            ret = TCL_OK;
	GtkWidget      *widget;

	if ( 0 )
	{
		if ( gnoclParseOptions ( interp, objc, objv, colorSelectionOptions ) != TCL_OK )
		{
			gnoclClearOptions ( colorSelectionOptions );
			return TCL_ERROR;
		}
	}

	/*
	typedef enum
	{
	  GTK_FILE_CHOOSER_ACTION_OPEN,
	  GTK_FILE_CHOOSER_ACTION_SAVE,
	  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
	  GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER
	} GtkFileChooserAction;
	*/

	widget = gtk_file_chooser_widget_new ( GTK_FILE_CHOOSER_ACTION_SAVE ) ;

	gtk_widget_show ( GTK_WIDGET ( widget ) );


	if ( 0 )
	{
		ret = gnoclSetOptions ( interp, colorSelectionOptions, G_OBJECT ( widget ), -1 );

		if ( ret == TCL_OK )
		{
			ret = configure ( interp, G_OBJECT ( widget ), colorSelectionOptions );
		}

		gnoclClearOptions ( colorSelectionOptions );

		/* STEP 3)  -show the widget */

		if ( ret != TCL_OK )
		{
			gtk_widget_destroy ( GTK_WIDGET ( widget ) );
			return TCL_ERROR;
		}
	}

	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), colorSelectionFunc );
}

/*****/
