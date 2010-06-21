/****h* widget/recentChooserDialog
 * NAME
 *  aboutDialog.c
 * AUTHOR
 *  Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 * SYNOPSIS
 *  This file implements the gtk dialog and messageDialog
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/recentChooserDialog.png">
 * SEE ALSO
 *****/

/*
 * $Id: aboutDialog.c,v 1.1 2005/08/16 20:57:45 baum Exp $
 *
 * This file implements the gtk dialog and messageDialog
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
   2005-07: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/* declarations */
static int optStrv ( Tcl_Interp *interp, GnoclOption *opt,
					 GObject *obj, Tcl_Obj **ret );


static GnoclOption recentChooserDialogOptions[] =
{
	{ "-logo", GNOCL_OBJ, NULL },
	{ "-artists", GNOCL_LIST, "artists", optStrv },
	{ "-authors", GNOCL_LIST, "authors", optStrv },
	{ "-comments", GNOCL_STRING, "comments" },
	{ "-copyright", GNOCL_STRING, "copyright" },
	{ "-documenters", GNOCL_LIST, "documenters", optStrv },
	{ "-license", GNOCL_STRING, "license" },
	{ "-version", GNOCL_STRING, "version" },
	{ "-website", GNOCL_STRING, "website" },

	{ "-allowGrow", GNOCL_BOOL, "allow-grow" },
	{ "-allowShrink", GNOCL_BOOL, "allow-shrink" },
	{ "-defaultHeight", GNOCL_INT, "default-height" },
	{ "-defaultWidth", GNOCL_INT, "default-width" },
	{ "-dragTargets", GNOCL_LIST, "s", gnoclOptDnDTargets },
	{ "-dropTargets", GNOCL_LIST, "t", gnoclOptDnDTargets },
	{ "-icon", GNOCL_OBJ, "", gnoclOptIcon },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onDelete", GNOCL_OBJ, "", gnoclOptOnDelete },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ "-onDragData", GNOCL_OBJ, "", gnoclOptOnDragData },
	{ "-onDropData", GNOCL_OBJ, "", gnoclOptOnDropData },
	{ "-onKeyPress", GNOCL_OBJ, "", gnoclOptOnKeyPress },
	{ "-onKeyRelease", GNOCL_OBJ, "", gnoclOptOnKeyRelease },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onRealize", GNOCL_OBJ, "realize", gnoclOptCommand },
	{ "-onResize", GNOCL_OBJ, "", gnoclOptOnConfigure },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-resizable", GNOCL_BOOL, "resizable" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-title", GNOCL_STRING, "title" },
	{ "-typeHint", GNOCL_OBJ, "", gnoclOptWindowTypeHint },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ NULL }
};

static int optStrv ( Tcl_Interp *interp, GnoclOption *opt,
					 GObject *obj, Tcl_Obj **ret )
{
	if ( ret == NULL ) /* set value */
	{
		typedef char *charp;
		int  no;
		int  k;
		char **strv;

		Tcl_ListObjLength ( interp, opt->val.obj, &no );
		strv = g_new ( charp, no + 1 );

		for ( k = 0; k < no; ++k )
		{
			Tcl_Obj *pobj;

			if ( Tcl_ListObjIndex ( interp, opt->val.obj, k, &pobj ) != TCL_OK )
			{
				g_free ( strv );
				return TCL_ERROR;
			}

			strv[k] = Tcl_GetString ( pobj );
		}

		strv[no] = NULL;

		g_object_set ( obj, opt->propName, strv, NULL );
		g_free ( strv );
	}

	else /* get value */
	{
		gchar **strv;
		int   no;
		int   k;

		g_object_get ( obj, opt->propName, &strv, NULL );

		for ( no = 0; strv[no] != NULL; ++no )
			;

		*ret = Tcl_NewListObj ( 0, NULL );

		for ( k = 0; k < no; ++k )
			Tcl_ListObjAppendElement ( NULL, *ret,
									   Tcl_NewStringObj ( strv[k], -1 ) );

		g_strfreev ( strv );
	}

	return TCL_OK;
}

/*****/

/****f* static/configure
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkRecentChooserDialog *dialog,
					   GnoclOption options[] )
{


	return TCL_OK;
}


static int cget ( Tcl_Interp *interp, GtkLabel *label,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	return gnoclCgetNotImplemented ( interp, options + idx );
}


int recentChooserDialogFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	static const char *cmds[] = { "delete", "configure", "cget", "class", "hide", "show", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ClassIdx, HideIdx, ShowIdx };
	int idx;
	GtkWidget *widget = GTK_WIDGET ( data );

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
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "recentChooserDialog", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   recentChooserDialogOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{
					ret = configure ( interp, widget, recentChooserDialogOptions );
				}

				gnoclClearOptions ( recentChooserDialogOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				/* kill( 0, SIGINT ); */

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ),
									 recentChooserDialogOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, widget, recentChooserDialogOptions, idx );
				}
			}
	}

	return TCL_OK;
}

/*****/

/****f* function/gnoclRecentChooserDialogCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclRecentChooserDialogCmd ( ClientData data, Tcl_Interp *interp,
								  int objc, Tcl_Obj * const objv[] )
{
	int            ret;
	GtkWidget     *dialog;
	gchar*            open_file;

	if ( gnoclParseOptions ( interp, objc, objv, recentChooserDialogOptions ) != TCL_OK )
	{
		gnoclClearOptions ( recentChooserDialogOptions );
		return TCL_ERROR;
	}

	/*
	 * GtkWidget *dialog;

	dialog = gtk_recent_chooser_dialog_new ("Recent Documents",
	                    parent_window,
	                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                    NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    GtkRecentInfo *info;

	    info = gtk_recent_chooser_get_current_item (GTK_RECENT_CHOOSER (dialog));
	    open_file (gtk_recent_info_get_uri (info));
	    gtk_recent_info_unref (info);
	  }

	gtk_widget_destroy (dialog);
	 */

	dialog = gtk_recent_chooser_dialog_new ( "Recent Documents", NULL, NULL );

	if ( gtk_dialog_run ( GTK_DIALOG ( dialog ) ) == GTK_RESPONSE_ACCEPT )
	{
		GtkRecentInfo *info;

		info = gtk_recent_chooser_get_current_item ( GTK_RECENT_CHOOSER ( dialog ) );
		open_file = ( gtk_recent_info_get_uri ( info ) );
		gtk_recent_info_unref ( info );
	}

	/*
	   dialog = GTK_ABOUT_DIALOG( gtk_about_dialog_new( ) );
	   *
	   ret = gnoclSetOptions( interp, recentChooserDialogOptions, G_OBJECT( dialog ), -1 );
	   if( ret == TCL_OK )
	      ret = configure( interp, dialog, recentChooserDialogOptions );
	   gnoclClearOptions( recentChooserDialogOptions );
	*/

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
		return TCL_ERROR;
	}

	gtk_widget_show ( GTK_WIDGET ( dialog ) );

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( dialog ), recentChooserDialogFunc );
}

/*****/
