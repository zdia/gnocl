/**
\brief      This module implements the gnocl::aboutDialog widget.
\author     Peter G. Baum, William J Giddings
\date       2001-03:
\todo       The dialog is displayed using gtk_dialog_run function rather than gtk_widget_show_all.
\see        http://library.gnome.org/devel/gtk/2.16/GtkDialog.html#gtk-dialog-run
\see        Krause, (Ch.5),

/**
\page page1 gnocl::aboutDialog

\section sec DESCRIPTION
    \image html aboutDialog.png
    \htmlinclude About.html
    \image html aboutDialog.png

\\subsection subsection1 Implemented Options
    \\verbinclude aboutDialog_options.txt
\\subsection subsection2 Implemented Commands
    \\verbinclude aboutDialog_commands.txt
\\subsection subsection3 EXAMPLE
    \\include aboutDialog_example.tcl
\\subsection subsection4 Produces

*/
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
            resolved problems associated with use of the close button failiong to close the dialog.
            completed implementation of remaining standard properties
            added commands show & hide
   2009-12: added -visible
            fixed error in displaying the correct string in title bar
   2005-07: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/* declarations */
static int optStrv ( Tcl_Interp *interp, GnoclOption *opt,
					 GObject *obj, Tcl_Obj **ret );

/**
 */
static GnoclOption aboutDialogOptions[] =
{
	/* widget properties */
	{ "-artists", GNOCL_LIST, "artists", optStrv },
	{ "-authors", GNOCL_LIST, "authors", optStrv },
	{ "-comments", GNOCL_STRING, "comments" },
	{ "-copyright", GNOCL_STRING, "copyright" },
	{ "-documenters", GNOCL_LIST, "documenters", optStrv },
	{ "-license", GNOCL_STRING, "license" },
	{ "-logo", GNOCL_OBJ, NULL },
	{ "-logoIconName", GNOCL_STRING, "logo-icon-name"  },
	{ "-programName", GNOCL_STRING, "program-name" },
	{ "-translatorCredits", GNOCL_STRING, "translator-credits" },
	{ "-version", GNOCL_STRING, "version" },
	{ "-website", GNOCL_STRING, "website" },
	{ "-wrapLicense", GNOCL_BOOL, "wrap-license" },

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
	{ "-title", GNOCL_STRING, "program-name" },
	{ "-typeHint", GNOCL_OBJ, "", gnoclOptWindowTypeHint },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ NULL }
};

const int logoIdx = 0;

/**
 */
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
static int configure ( Tcl_Interp *interp, GtkAboutDialog *dialog,
					   GnoclOption options[] )
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

/*****/

/****f* static/cget
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, GtkLabel *label,
				  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/****f* function/dialogFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
int aboutDialogFunc ( ClientData data, Tcl_Interp *interp,
					  int objc, Tcl_Obj * const objv[] )
{

	static const char *cmds[] = { "delete", "configure", "cget", "show", "hide", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ShowIdx, HideIdx };
	int idx;
	GtkLabel *dialog = ( GtkLabel * ) data;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case HideIdx:
			{
				gtk_widget_hide ( GTK_WIDGET ( dialog ) );
			}
			break;
		case ShowIdx:
			{
				gtk_dialog_run ( GTK_WIDGET ( dialog ) );
				gtk_widget_destroy ( dialog );
				//gtk_widget_show_all ( GTK_WIDGET ( dialog ) );
			}
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( dialog ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   aboutDialogOptions, G_OBJECT ( dialog ) ) == TCL_OK )
				{
					ret = configure ( interp, dialog, aboutDialogOptions );
				}

				gnoclClearOptions ( aboutDialogOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				/* kill( 0, SIGINT ); */

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( dialog ),
									 aboutDialogOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, dialog, aboutDialogOptions, idx );
				}
			}
	}

	return TCL_OK;
}

/*****/

/****f* function/gnoclAboutDialogCmd
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclAboutDialogCmd ( ClientData data, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[] )
{
	int            ret;
	GtkAboutDialog *dialog;

	if ( gnoclParseOptions ( interp, objc, objv, aboutDialogOptions ) != TCL_OK )
	{
		gnoclClearOptions ( aboutDialogOptions );
		return TCL_ERROR;
	}

	dialog = GTK_ABOUT_DIALOG ( gtk_about_dialog_new( ) );

	ret = gnoclSetOptions ( interp, aboutDialogOptions, G_OBJECT ( dialog ), -1 );

	if ( ret == TCL_OK )
		ret = configure ( interp, dialog, aboutDialogOptions );

	gnoclClearOptions ( aboutDialogOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
		return TCL_ERROR;
	}

	//gtk_widget_show ( GTK_WIDGET ( dialog ) );

	gtk_dialog_run ( GTK_DIALOG ( dialog ) );
	gtk_widget_destroy ( dialog );

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( dialog ), aboutDialogFunc );
}

/*****/
