/**
\brief
\author     William J Giddings
\date       12/Jan/2010
**/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
\brief
\author     William J Giddings
\date       12/Jan/2010
**/
static GnoclOption TEMPLATEOptions[] =
{
	/* widget properties */
	{ "-artists", GNOCL_LIST, "artists"},

	/* widget signals */
	{ "-artists", GNOCL_LIST, "artists"},

	{NULL}
};

static const int logoIdx = 0;


static int configure ( Tcl_Interp *interp, GtkAboutDialog *dialog, GnoclOption options[] )
{
	if ( options[logoIdx].status == GNOCL_STATUS_CHANGED )
	{
		GdkPixbuf *pix;

		if ( gnoclGetStringType ( options[logoIdx].val.obj ) != GNOCL_STR_FILE )
		{
			Tcl_SetResult ( interp, "Logo must be of file type", TCL_STATIC );
			return TCL_ERROR;
		}

		pix = gnoclPixbufFromObj ( interp, options + logoIdx );

		if ( pix == NULL )
			return TCL_ERROR;

		gtk_about_dialog_set_logo ( dialog, pix );
	}

	return TCL_OK;
}


/**
\brief
\author     William J Giddings
\date       12/Jan/2010
**/
int TEMPLATEFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	/* set list of valid commands for this widget */
	static const char *cmds[] = { "delete", "configure", "cget", "show", "hide", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ShowIdx, HideIdx, ClassIdx };

	int idx;
	GtkWidget *widget;

	widget  = GTK_WIDGET ( data );

	/* check the number of arguments received from the interpreter */
	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	/* get which command was passed */
	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	/* respsond to the commands received */
	switch ( idx )
	{
		case ClassIdx:
			{
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "TEMPLATE_CLASS", -1 ) );
			}
			break;
		case DeleteIdx:
			{
				return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );
			}
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, TEMPLATEOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, widget, TEMPLATEOptions );
				}

				gnoclClearOptions ( TEMPLATEOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), TEMPLATEOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, widget, TEMPLATEOptions, idx );
				}
			}
	}

	return TCL_OK;

}

/**
\brief
\author     William J Giddings
\date       12/Jan/2010
**/
int gnoclTEMPLATECmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int ret;
	GtkWidget *widget;

	/* 1) check the validity of the options passed from the interpeter */
	if ( gnoclParseOptions ( interp, objc, objv, TEMPLATEOptions ) != TCL_OK )
	{
		gnoclClearOptions ( TEMPLATEOptions );
		return TCL_ERROR;
	}

	/* 2) initialize the widget and set options passed from the interpreter */
	widget = gtk_TEMPLATE_new();

	/* a) apply base widget properties and signals */
	ret = gnoclSetOptions ( interp, TEMPLATEOptions, G_OBJECT ( widget ), -1 );

	/* b) apply options not handled above */
	if ( ret == TCL_OK )
	{
		ret = configure ( interp, widget, TEMPLATEOptions );
	}

	/* c) free up memory */
	gnoclClearOptions ( TEMPLATEOptions );

	/* d) in the case of invalid options, destroy the widget instances and quit */
	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( widget ) );
		return TCL_ERROR;
	}

	/* 3) everything is ok, so realize the widget */
	gtk_widget_show ( GTK_WIDGET ( widget ) );

	/* 4) register the widget with the interpreter and associate it with a handler function */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), TEMPLATEFunc );

}

