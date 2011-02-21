/*
 * $Id: fileSelection.c,v 1.1 2005/01/01 15:28:49 baum Exp $
 *
 * This file implements the file selection dialog
 *
 * Copyright (c) 2001 - 2004 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:

   2004-12: replaced by fileChooser, moved to deprecated
   2003-02: cleanups with GnoclOption
   2002-07: complete rewrite
*/

#include "../gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption options[] =
{
	{ "-onClicked", GNOCL_STRING, NULL },      /* 0 */
	{ "-modal", GNOCL_BOOL, "modal" },         /* 1 */
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-file", GNOCL_STRING, "filename" },
	{ "-title", GNOCL_STRING, "title" },
	{ "-selectMultiple", GNOCL_BOOL, "select-multiple" },
	{ "-showFileOps", GNOCL_BOOL, "show-fileops" },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ NULL }
};

static const int commandIdx = 0;
static const int modalIdx   = 1;

typedef struct
{
	char             *name;
	Tcl_Interp       *interp;
	char             *onClicked;
	GtkFileSelection *fileSel;
} FileSelParams;


static Tcl_Obj *getFileList ( GtkFileSelection *fileSel, Tcl_Interp *interp )
{
	Tcl_Obj *ret = Tcl_NewListObj ( 0, NULL  );
	gchar **files = gtk_file_selection_get_selections ( fileSel );
	int k;

	for ( k = 0; files[k]; ++k )
		Tcl_ListObjAppendElement ( interp, ret,
								   Tcl_NewStringObj ( files[k], -1 ) );

	g_strfreev ( files );

	return ret;
}

static void onButtonFunc ( FileSelParams *para, int isOk )
{
	if ( para->onClicked )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'x', GNOCL_STRING },  /* exit button */
			{ 'f', GNOCL_OBJ },     /* files */
			{ 0 }
		};

		ps[0].val.str = para->name;
		ps[1].val.str = isOk ? "OK" : "CANCEL";
		ps[2].val.obj = isOk ? getFileList ( para->fileSel, para->interp ) : NULL;

		gnoclPercentSubstAndEval ( para->interp, ps, para->onClicked, 1 );
	}
}

static void onOkFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FileSelParams * ) data, 1 );
}

static void onCancelFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FileSelParams * ) data, 0 );
}

static int fileSelFunc ( ClientData data, Tcl_Interp *interp,
						 int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx };
	FileSelParams *para = ( FileSelParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->fileSel );
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
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:

			if ( gnoclParseOptions ( interp, objc - 2, objv + 2, options )
					== TCL_OK )
			{
				if ( options[commandIdx].status == GNOCL_STATUS_CHANGED )
				{
					para->onClicked = options[commandIdx].val.str;
					options[commandIdx].val.str = NULL;   /* avoid double free */
				}

				return TCL_OK;
			}

			return TCL_ERROR;
	}

	return TCL_OK;
}

int gnoclFileSelectionCmd ( ClientData data, Tcl_Interp *interp,
							int objc, Tcl_Obj * const objv[] )
{
	FileSelParams *para = NULL;
	int           ret = TCL_ERROR;
	int           isModal = 1;           /* default: is modal */

	assert ( strcmp ( options[commandIdx].optName, "-onClicked" ) == 0 );
	assert ( strcmp ( options[modalIdx].optName, "-modal" ) == 0 );

	if ( gnoclParseOptions ( interp, objc, objv, options )  != TCL_OK )
		goto cleanExit;

	para = g_new ( FileSelParams, 1 );

	para->fileSel = GTK_FILE_SELECTION ( gtk_file_selection_new ( "" ) );

	para->interp = interp;

	para->name = NULL;

	if ( options[commandIdx].status == GNOCL_STATUS_CHANGED )
	{
		para->onClicked = options[commandIdx].val.str;
		options[commandIdx].val.str = NULL;   /* avoid double free */

	}

	else
		para->onClicked = NULL;


	if ( gnoclSetOptions ( interp, options, G_OBJECT ( para->fileSel ),
						   -1 ) != TCL_OK )
		goto cleanExit;


	if ( options[modalIdx].status == GNOCL_STATUS_SET )
		isModal = options[modalIdx].val.b;
	else
		gtk_window_set_modal ( GTK_WINDOW ( para->fileSel ), isModal );

	g_signal_connect ( GTK_OBJECT ( para->fileSel->ok_button ),
					   "clicked", G_CALLBACK ( onOkFunc ), para );

	g_signal_connect ( GTK_OBJECT ( para->fileSel->cancel_button ),
					   "clicked", G_CALLBACK ( onCancelFunc ), para );

	gtk_widget_show ( GTK_WIDGET ( para->fileSel ) );

	if ( isModal )
	{
		gint res = gtk_dialog_run ( GTK_DIALOG ( para->fileSel ) );

		if ( res == GTK_RESPONSE_OK )
			Tcl_SetObjResult ( para->interp,
							   getFileList ( para->fileSel, para->interp ) );

		gtk_widget_destroy ( GTK_WIDGET ( para->fileSel ) );
	}

	else
	{
		para->name = gnoclGetAutoWidgetId();
		gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->fileSel ) );
		gtk_widget_show ( GTK_WIDGET ( para->fileSel ) );

		Tcl_CreateObjCommand ( interp, para->name, fileSelFunc, para, NULL );
		Tcl_SetObjResult ( interp, Tcl_NewStringObj ( para->name, -1 ) );
	}

	ret = TCL_OK;

cleanExit:
	/* TODO:
	if( ret != TCL_OK )
	   freeParams
	*/

	gnoclClearOptions ( options );

	return ret;
}

