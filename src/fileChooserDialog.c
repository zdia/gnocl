/**
\brief      This module implements the gnocl::fileChooserDialog.
\author     Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation */

/** \page page18 gnocl::fileChooser
  \section sec gnocl::fileChooser
  Implementation of gnocl::fileChooser
  \subsection subsection1 Implemented gnocl::fileChooser Options
  \subsection subsection2 Implemented gnocl::fileChooser Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/fileChooser.png"
*/

/*
 * $Id: fileChooserDialog.c,v 1.4 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements the file chooser dialog
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2010-01: deprecated -action option "openFolder" in favor of Gtk compliant "selectFolder"
   2009-12: module renamed from fileChooser,c to fileChooserDialog.c
   2004-12: start of development
*/

#include "gnocl.h"
#include "gnoclparams.h"

#include <string.h>
#include <assert.h>

static int optCurrentName ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int optMiscFp ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

static GnoclOption options[] =
{
	{ "-action", GNOCL_OBJ, NULL },            /* 0 */
	{ "-onUpdatePreview", GNOCL_OBJ, NULL },   /* 1 */
	{ "-getURIs", GNOCL_BOOL, NULL },          /* 2 */
	{ "-currentName", GNOCL_OBJ, "", optCurrentName },
	{ "-currentFolder", GNOCL_OBJ, "f", optMiscFp },
	{ "-extraWidget", GNOCL_OBJ, "extra-widget", gnoclOptWidget },
	{ "-filename", GNOCL_OBJ, "n", optMiscFp },
	{ "-localOnly", GNOCL_BOOL, "local-only" },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ "-previewWidget", GNOCL_OBJ, "preview-widget", gnoclOptWidget },
	{ "-previewWidgetActive", GNOCL_BOOL, "preview-widget-active" },
	{ "-selectMultiple", GNOCL_BOOL, "select-multiple" },
	{ "-selectFilename", GNOCL_OBJ, "s", optMiscFp },
	{ "-showHidden", GNOCL_BOOL, "show-hidden" },
	{ "-title", GNOCL_STRING, "title" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};

static const int actionIdx          = 0;
static const int onUpdatePreviewIdx = 1;
static const int getURIsIdx         = 2;


/* moved to gnocl.h */
/*
typedef struct
{
    Tcl_Interp           *interp;
    char                 *name;
    int                  getURIs;
    GtkFileChooserDialog *fileDialog;
} FileSelDialogParams;
*/

/**
\brief
\author     Peter G Baum, William J Giddings
**/
static int optMiscFp ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	if ( ret == NULL ) /* set value */
	{
		const char *str;
		gboolean ( *fp ) ( GtkFileChooser *, const gchar * );

		switch ( opt->propName[0] )
		{
			case 'f': fp = gtk_file_chooser_set_current_folder; break;
			case 'n': fp = gtk_file_chooser_set_filename; break;
			case 's': fp = gtk_file_chooser_select_filename; break;
			default: assert ( 0 );
		}

		str = Tcl_GetString ( opt->val.obj );

		if ( *str )
			( *fp ) ( GTK_FILE_CHOOSER ( obj ), str );
	}

	else /* get value */
	{
		char *str;
		char * ( *fp ) ( GtkFileChooser * );

		switch ( opt->propName[0] )
		{
			case 'f':   fp = gtk_file_chooser_get_current_folder; break;
			case 'n':   fp = gtk_file_chooser_get_filename; break;
			case 's':   Tcl_SetResult ( interp,
											"Option -selectFilename can only be set",
											TCL_STATIC );
				return TCL_ERROR;
			default: assert ( 0 );
		}

		str = ( *fp ) ( GTK_FILE_CHOOSER ( obj ) );

		*ret = Tcl_NewStringObj ( str, -1 );
		g_free ( str );
	}

	return TCL_OK;
}

/**
\brief
\author     Peter G Baum, William J Giddings
**/
static int optCurrentName ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	if ( ret == NULL ) /* set value */
	{
		const char *str = Tcl_GetString ( opt->val.obj );
		gtk_file_chooser_set_current_name ( GTK_FILE_CHOOSER ( obj ), str );
	}

	else /* get value */
	{
		char *str = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( obj ) );
		*ret = Tcl_NewStringObj ( str, -1 );
		g_free ( str );
	}

	return TCL_OK;
}

/**
\brief
\author
**/
static void destroyFunc ( GtkWidget *widget, gpointer data )
{
	FileSelDialogParams *para = ( FileSelDialogParams * ) data;

	gnoclForgetWidgetFromName ( para->name );
	Tcl_DeleteCommand ( para->interp, para->name );

	g_free ( para->name );
	g_free ( para );
}

/**
\brief
\author
**/
static int getActionType ( Tcl_Interp *interp, Tcl_Obj *obj, GtkFileChooserAction *action )
{

	// deprecate openFolder in favour of selectFolder
	const char *txt[] = { "open", "save", "openFolder", "createFolder", "selectFolder", NULL };
	GtkFileChooserAction types[] = { GTK_FILE_CHOOSER_ACTION_OPEN,
									 GTK_FILE_CHOOSER_ACTION_SAVE,
									 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
									 GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
									 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
								   };

	int idx;

	if ( Tcl_GetIndexFromObj ( interp, obj, txt, "action", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	*action = types[idx];

	return TCL_OK;
}

/**
\brief
\author
**/
static Tcl_Obj *getUTF8FileName ( const char *name, int *errFlag )
{
	Tcl_Obj *ret;

	*errFlag = 0;

	if ( name != NULL )
	{
		GError *error = NULL;
		gsize  bytesRead, bytesWritten;
		char   *txt = g_filename_to_utf8 ( name, -1, &bytesRead, &bytesWritten,
										   &error );

		if ( txt == NULL )
		{
			ret = Tcl_NewStringObj ( error->message, -1 );
			*errFlag = 1;
		}

		else
		{
			ret = Tcl_NewStringObj ( name, bytesWritten );
			g_free ( txt );
		}
	}

	else
		ret = Tcl_NewStringObj ( "", 0 );

	return ret;
}

/**
\brief
\author
**/
static Tcl_Obj *getFileList ( Tcl_Interp *interp, FileSelDialogParams *para )
{
	int     errFlag = 0;
	Tcl_Obj *err;
	Tcl_Obj *ret;
	GtkFileChooser *chooser = GTK_FILE_CHOOSER ( para->fileDialog );

	if ( gtk_file_chooser_get_select_multiple ( chooser ) )
	{
		GSList *files = para->getURIs
						? gtk_file_chooser_get_uris ( chooser )
						: gtk_file_chooser_get_filenames ( chooser );
		GSList *p = files;
		ret = Tcl_NewListObj ( 0, NULL  );

		for ( ; p != NULL; p = p->next )
		{
			if ( errFlag == 0 )
			{
				err = getUTF8FileName ( p->data, &errFlag );
				Tcl_ListObjAppendElement ( interp, ret, err );
			}

			g_free ( p->data );
		}

		g_slist_free ( files );

		if ( errFlag )
			Tcl_DecrRefCount ( ret );
	}

	else
	{
		char *txt = para->getURIs
					? gtk_file_chooser_get_uri ( chooser )
					: gtk_file_chooser_get_filename ( chooser );
		ret = err = getUTF8FileName ( txt, &errFlag );
		g_free ( txt );
	}

	if ( errFlag )
	{
		Tcl_SetObjResult ( interp, err );
		return NULL;
	}

	return ret;
}

/**
\brief
\author
**/
#if 0
static void onButtonFunc ( FileSelDialogParams *para, int isOk )
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
		ps[2].val.obj = isOk ? getFileList ( para ) : NULL;

		gnoclPercentSubstAndEval ( para->interp, ps, para->onClicked, 1 );
	}
}

/**
\brief
\author
**/
static void onOkFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FileSelDialogParams * ) data, 1 );
}

/**
\brief
\author
**/
static void onCancelFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FileSelDialogParams * ) data, 0 );
}

#endif

/**
\brief
\author
**/
static void onUpdatePreview ( GtkFileChooser *chooser, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;
	FileSelDialogParams *para = ( FileSelDialogParams * ) cs->data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget */
		{ 'f', GNOCL_STRING },  /* file name */
		{ 0 }
	};
	int ret;

	assert ( chooser == GTK_FILE_CHOOSER ( para->fileDialog ) );
	assert ( cs->interp == para->interp );

	ps[0].val.str = para->name;
	ps[1].val.str = gtk_file_chooser_get_preview_filename ( chooser );

	ret = gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

	g_free ( ( char * ) ps[1].val.str );

	return;
}

/**
\brief
\author
**/
static int cget ( FileSelDialogParams *para, GnoclOption options[], int idx )
{
	/*
	   Tcl_Obj *obj = NULL;

	   if( obj != NULL )
	   {
	      Tcl_SetObjResult( interp, obj );
	      return TCL_OK;
	   }
	*/

	return gnoclCgetNotImplemented ( para->interp, options + idx );
}

/**
\brief
\author
**/
int fileDialogFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "hide", "show", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, HideIdx, ShowIdx };
	FileSelDialogParams *para = ( FileSelDialogParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->fileDialog );
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
		case HideIdx:
			{
				gtk_widget_hide ( widget );
			}
			break;
		case ShowIdx:
			{
				gtk_widget_show_all ( widget );
			}
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:

			if ( gnoclParseOptions ( interp, objc - 1, objv + 1, options )
					== TCL_OK )
			{
				return TCL_OK;
			}

			return TCL_ERROR;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ),
									 options, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( para, options, idx );
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author
**/
int gnoclFileChooserDialogCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	FileSelDialogParams *para = NULL;
	int ret = TCL_ERROR;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	const char *okTxt;
	gint res;

	if ( gnoclParseOptions ( interp, objc, objv, options )  != TCL_OK )
	{
		goto cleanExit;
	}

	para = g_new ( FileSelDialogParams, 1 );

	para->interp = interp;

	para->getURIs = 0;

	if ( options[getURIsIdx].status == GNOCL_STATUS_CHANGED )
	{
		para->getURIs = options[getURIsIdx].val.b;
	}

	if ( options[actionIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( getActionType ( interp, options[actionIdx].val.obj, &action ) != TCL_OK )
		{
			goto cleanExit;
		}
	}

	switch ( action )
	{
		case GTK_FILE_CHOOSER_ACTION_SAVE:
			okTxt = GTK_STOCK_SAVE;
			break;
		default:
			okTxt = GTK_STOCK_OPEN;
			break;
	}

	if ( para->getURIs )
	{
		para->fileDialog = GTK_FILE_CHOOSER_DIALOG (
							   gtk_file_chooser_dialog_new_with_backend ( "", NULL, action, "gnome-vfs",
									   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, okTxt, GTK_RESPONSE_ACCEPT, NULL ) );
	}

	else
	{
		para->fileDialog = GTK_FILE_CHOOSER_DIALOG (
							   gtk_file_chooser_dialog_new ( "", NULL, action,
															 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, okTxt, GTK_RESPONSE_ACCEPT, NULL ) );
	}

	para->getURIs = 0;

	if ( gnoclSetOptions ( interp, options, G_OBJECT ( para->fileDialog ),
						   -1 ) != TCL_OK )
		goto cleanExit2;

	if ( gnoclConnectOptCmd ( interp,
							  G_OBJECT ( para->fileDialog ), "update-preview",
							  G_CALLBACK ( onUpdatePreview ), options + onUpdatePreviewIdx,
							  para, NULL ) != TCL_OK )
		goto cleanExit2;

	g_signal_connect ( G_OBJECT ( para->fileDialog ), "destroy",
					   G_CALLBACK ( destroyFunc ), para );

	para->name = gnoclGetAutoWidgetId();

	gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->fileDialog ) );

	Tcl_CreateObjCommand ( interp, para->name, fileDialogFunc, para, NULL );

	res = gtk_dialog_run ( GTK_DIALOG ( para->fileDialog ) );

	gtk_widget_hide ( GTK_WIDGET ( para->fileDialog ) );

	if ( res == GTK_RESPONSE_ACCEPT )
	{
		Tcl_Obj *obj = getFileList ( interp, para );

		if ( obj == NULL )
			goto cleanExit2;

		Tcl_SetObjResult ( interp, obj );
	}

	ret = TCL_OK;

cleanExit2:
	gtk_widget_destroy ( GTK_WIDGET ( para->fileDialog ) );

cleanExit:
	gnoclClearOptions ( options );

	return ret;
}

