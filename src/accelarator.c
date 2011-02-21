/**
\brief        This module implements the gnocl::accelarator widget.
\authors       William J Giddings
\date
**/



#include "gnocl.h"
#include <string.h>
#include <assert.h>


static const int textIdx = 0;
static const int widgetIdx = 1;

static GnoclOption accelaratorOptions[] =
{
	{ "-text", GNOCL_OBJ, NULL },        /* 0 */
	{ "-widget", GNOCL_OBJ, NULL },      /* 1 */

	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ NULL }
};



static int configure ( Tcl_Interp *interp, GtkWidget *accelarator, GnoclOption options[] )
{

	if ( options[textIdx].status == GNOCL_STATUS_CHANGED )
	{
		gtk_label_set_text_with_mnemonic    ( GTK_LABEL ( accelarator ), Tcl_GetString ( options[textIdx].val.obj ) );

		g_printf ( "text changed %s\n", Tcl_GetString ( options[textIdx].val.obj ) );
	}

	if ( options[widgetIdx].status == GNOCL_STATUS_CHANGED )
	{
		GtkWidget *widget;
		widget = gnoclGetWidgetFromName ( Tcl_GetString ( options[widgetIdx].val.obj ), interp );
		gtk_label_set_mnemonic_widget ( GTK_LABEL ( accelarator ), widget );
		g_printf ( "widget changed %s\n", Tcl_GetString ( options[widgetIdx].val.obj ) );
	}

	return TCL_OK;
}


static int cget ( Tcl_Interp *interp, GtkWidget *accelarator, GnoclOption options[], int idx )
{

	return gnoclCgetNotImplemented ( interp, options + idx );
}

int accelaratorFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

#ifdef DEBUG_ACCELARATOR
	g_printf ( "accelaratorFunc\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	static const char *cmds[] =
	{
		"delete", "configure", "cget", "class",  NULL
	};

	enum cmdIdx
	{
		DeleteIdx, ConfigureIdx, CgetIdx, ClassIdx
	};

	GtkWidget *accelarator = GTK_WIDGET ( data );
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "accelarator", -1 ) );
			break;

		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( accelarator ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, accelaratorOptions, G_OBJECT ( accelarator ) ) == TCL_OK )
				{
					ret = configure ( interp, accelarator, accelaratorOptions );
				}

				gnoclClearOptions ( accelaratorOptions );

				return ret;
			}

			break;


		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( accelarator ), accelaratorOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, accelarator, accelaratorOptions, idx );
				}
			}
	}

	return TCL_OK;
}


int gnoclAcceleratorCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

#ifdef DEBUG_ACCELARATOR
	g_printf ( "gnoclAcceleratorCmd\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	int       ret;
	GtkLabel  *accelarator;

	/* check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, accelaratorOptions ) != TCL_OK )
	{
		gnoclClearOptions ( accelaratorOptions );
		return TCL_ERROR;
	}


	accelarator = gtk_label_new_with_mnemonic ( "_something" );

	/* it would be useful to add markup */
	//gtk_label_set_use_markup ( accelarator, TRUE );

	gtk_widget_show ( GTK_WIDGET ( accelarator ) );

	ret = gnoclSetOptions ( interp, accelaratorOptions, G_OBJECT ( accelarator ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, accelarator, accelaratorOptions );
	}

	gnoclClearOptions ( accelaratorOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( accelarator ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( accelarator ), accelaratorFunc );
}


