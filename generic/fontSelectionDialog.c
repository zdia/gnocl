/**
 \brief
  This module implements the gnocl::FontSelection widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/** \page page21 gnocl::FontSelection
  \section sec gnocl::FontSelection
  Implementation of gnocl::FontSelection
  \subsection subsection1 Implemented gnocl::FontSelection Options
  \subsection subsection2 Implemented gnocl::FontSelection Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/FontSelection.png"
*/

/****h* dialog/FontSelection
 * NAME
 *  FontSelection.c
 * SYNOPSIS
 *  This file implements the file selection dialog.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/FontSelection.png">
 * SEE ALSO
 *****/

/*
 * $Id: FontSelection.c,v 1.5 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the file selection dialog
 *
 * Copyright (c) 2002 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2008-10: added command, class
   2003-02: cleanups with GnoclOption
   2002-07: start of developement
*/

#include "gnocl.h"
#include "gnoclparams.h"

#include "string.h"
#include <assert.h>

#define DEBUG 1

static GnoclOption options[] =
{
	{ "-onClicked", GNOCL_STRING, NULL },                   /* 0 */
	{ "-modal", GNOCL_BOOL, "modal" },                    /* 1 */
	{ "-visible", GNOCL_BOOL, "visible" },                /* 2 */
	{ "-name", GNOCL_STRING, "name" },                    /* 3 */
	{ "-title", GNOCL_STRING, "title" },                  /* 4 */
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },  /* 5 */
	{ "-font", GNOCL_STRING, "font-name", NULL }, /* 6 */
	{ "-previewText", GNOCL_STRING, "preview-text" },
	{ NULL }
};

static const int commandIdx    = 0;
static const int modalIdx      = 1;
static const int fontSelectIdx = 6; /* first index which relates to the GtkFontSelection and not to the dialog */
/*****/


/* moved to gnocl.h */
/*
typedef struct
{
    char             *name;
    Tcl_Interp       *interp;
    char             *onClicked;
    GtkFontSelectionDialog *fontSel;
} FontSelDialogParams;
*/

/**
\brief
\author     Peter G Baum
**/
static void onButtonFunc ( FontSelDialogParams *para, int isOk )
{
	if ( para->onClicked )
	{
		GnoclPercSubst ps[] =
		{
			{ 'w', GNOCL_STRING },  /* widget */
			{ 'x', GNOCL_STRING },  /* exit button */
			{ 'f', GNOCL_STRING },  /* font */
			{ 0 }
		};
		ps[0].val.str = para->name;
		ps[1].val.str = isOk ? "OK" : "CANCEL";
		ps[2].val.str = gtk_font_selection_get_font_name (
							GTK_FONT_SELECTION ( para->fontSel->fontsel ) );

		gnoclPercentSubstAndEval ( para->interp, ps, para->onClicked, 1 );
	}
}

/**
\brief
\author     Peter G Baum
**/
static void onOkFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FontSelDialogParams * ) data, 1 );
}

/**
\brief
\author     Peter G Baum
**/
static void onCancelFunc ( GtkWidget *widget, gpointer data )
{
	onButtonFunc ( ( FontSelDialogParams * ) data, 0 );
}

/**
\brief
\author     Peter G Baum
**/
int fontSelDialogFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "class", "hide", "show", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, ClassIdx, HideIdx, ShowIdx };
	FontSelDialogParams *para = ( FontSelDialogParams * ) data;
	GtkWidget *widget = GTK_WIDGET ( para->fontSel );
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
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "fontSelectionDialog", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:

			if ( gnoclParseOptions ( interp, objc - 2, objv + 2, options )
					== TCL_OK )
			{
				/* TODO? error if modalIdx has changed? */
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

/**
\brief
\author     Peter G Baum
**/
int gnoclFontSelectionDialogCmd ( ClientData data, Tcl_Interp *interp,
								  int objc, Tcl_Obj * const objv[] )
{
	FontSelDialogParams *para = NULL;
	int           ret = TCL_ERROR;
	int           isModal = 1;           /* default: is modal */

	assert ( strcmp ( options[commandIdx].optName, "-onClicked" ) == 0 );
	assert ( strcmp ( options[modalIdx].optName, "-modal" ) == 0 );
	assert ( strcmp ( options[fontSelectIdx].optName, "-font" ) == 0 );

	if ( gnoclParseOptions ( interp, objc, objv, options )  != TCL_OK )
		goto cleanExit;

	para = g_new ( FontSelDialogParams, 1 );

	para->fontSel = GTK_FONT_SELECTION_DIALOG (
						gtk_font_selection_dialog_new ( "" ) );

	para->interp = interp;

	para->name = NULL;

	if ( options[commandIdx].status == GNOCL_STATUS_CHANGED )
	{
		para->onClicked = options[commandIdx].val.str;
		options[commandIdx].val.str = NULL;   /* avoid double free */

	}

	else
		para->onClicked = NULL;

	if ( gnoclSetOptions ( interp, options, G_OBJECT ( para->fontSel ),
						   fontSelectIdx ) != TCL_OK )
		goto cleanExit;

	if ( gnoclSetOptions ( interp, options + fontSelectIdx,
						   G_OBJECT ( para->fontSel->fontsel ), -1 ) != TCL_OK )
		goto cleanExit;

	if ( options[modalIdx].status == GNOCL_STATUS_SET )
		isModal = options[modalIdx].val.b;
	else
		gtk_window_set_modal ( GTK_WINDOW ( para->fontSel ), isModal );

	g_signal_connect ( GTK_OBJECT ( para->fontSel->ok_button ),
					   "clicked", G_CALLBACK ( onOkFunc ), para );

	g_signal_connect ( GTK_OBJECT ( para->fontSel->cancel_button ),
					   "clicked", G_CALLBACK ( onCancelFunc ), para );

	gtk_widget_show ( GTK_WIDGET ( para->fontSel ) );

	if ( isModal )
	{
		gint res = gtk_dialog_run ( GTK_DIALOG ( para->fontSel ) );

		if ( res == GTK_RESPONSE_OK )
		{
			Tcl_Obj *obj = Tcl_NewStringObj (
							   gtk_font_selection_get_font_name (
								   GTK_FONT_SELECTION ( para->fontSel->fontsel ) ), -1 );

			Tcl_SetObjResult ( interp, obj );
		}

		gtk_widget_destroy ( GTK_WIDGET ( para->fontSel ) );
	}

	else
	{
		para->name = gnoclGetAutoWidgetId();
		gnoclMemNameAndWidget ( para->name, GTK_WIDGET ( para->fontSel ) );
		gtk_widget_show ( GTK_WIDGET ( para->fontSel ) );

		Tcl_CreateObjCommand ( interp, para->name, fontSelDialogFunc, para, NULL );
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
