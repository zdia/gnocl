/**dialog/fileChooserButton.c
 * NAME
 *  fileChooserButton.c
 * SYNOPSIS
 *  This file implements the file chooser button.
 * FUNCTION
 * AUTHOR
 *  WJG
 * HISTORY
 *  28/10/08    added class
 *  13/07/08    began development
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/fileChooserButton.png">
 * SEE ALSO
 *****/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static const int actionIdx          = 0;
static const int onUpdatePreviewIdx = 1;
static const int getURIsIdx         = 2;

static GnoclOption chooserButtonOptions[] =
{
	{ "-action", GNOCL_OBJ, NULL },            /* 0 */
	{ "-onUpdatePreview", GNOCL_OBJ, NULL },   /* 1 */
	{ "-getURIs", GNOCL_BOOL, NULL },          /* 2 */
	{ "-onFileSet", GNOCL_OBJ, "file-set", gnoclOptOnFileSet },
	{ "-onSelectionChanged", GNOCL_OBJ, "selection-changed", gnoclOptOnSelectionChanged },
};

/**
/brief
/author     William J Giddings
**/
static int configure ( Tcl_Interp *interp, GtkWidget *chooserButton, GnoclOption options[] )
{
	GtkFileFilter *filter1, *filter2, *filter3;

	/* set the location to the user's home director */
	// gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(chooserButton), g_get_home_dir() );
#ifdef DEBUG_FILECHOOSER_BUTTON
	printf ( "Element 0 contains %s \n", options[0] );
	printf ( "Element 1 contains %s \n", options[1] );
	printf ( "Element 2 contains %s \n", options[2] );
#endif
	/* NOTE: once perfected, this switch also needs to be applied to the fileChooser selector widget */

	/* this is the fileChooser, so some filters needed */
	filter1 = gtk_file_filter_new ();
	gtk_file_filter_set_name ( filter1, "All Files" );
	gtk_file_filter_add_pattern ( filter1, "*" );
	gtk_file_chooser_add_filter ( GTK_FILE_CHOOSER ( chooserButton ), filter1 );

	filter2 = gtk_file_filter_new ();
	gtk_file_filter_set_name ( filter2, "Image Files" );
	gtk_file_filter_add_pattern ( filter2, "*.png" );
	gtk_file_filter_add_pattern ( filter2, "*.jpg" );
	gtk_file_filter_add_pattern ( filter2, "*.gif" );
	gtk_file_filter_add_pattern ( filter2, "*.tif" );
	gtk_file_chooser_add_filter ( GTK_FILE_CHOOSER ( chooserButton ), filter2 );

	filter3 = gtk_file_filter_new ();
	gtk_file_filter_set_name ( filter3, "Document Files" );
	gtk_file_filter_add_pattern ( filter3, "*.jml" );
	gtk_file_filter_add_pattern ( filter3, "*.txt" );
	gtk_file_filter_add_pattern ( filter3, "*.odt" );
	gtk_file_filter_add_pattern ( filter3, "*.rtf" );
	gtk_file_chooser_add_filter ( GTK_FILE_CHOOSER ( chooserButton ), filter3 );

	return TCL_OK;
}

/**
/brief
/author     William J Giddings
**/
static int cget ( Tcl_Interp *interp, GtkButton *chooserButton, GnoclOption options[], int idx )
{
	return TCL_OK;
}

/**
/brief
/author     William J Giddings
**/
/* a function to manage changes and events occuring to the widget during runtime */
int fileChooserButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	static const char *cmds[] =
	{
		"delete", "configure", "cget", "onClicked", "class", "parent", "geometry", "toplevel", NULL
	};

	enum cmdIdx
	{
		DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx, ParentIdx, GeometryIdx, ToplevelIdx
	};

	GtkButton *button = GTK_FILE_CHOOSER_BUTTON ( data );
	int idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case DeleteIdx:
			{
			}
			break;
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, chooserButtonOptions, G_OBJECT ( button ) ) == TCL_OK )
				{
					ret = configure ( interp, button, chooserButtonOptions );
				}

				gnoclClearOptions ( chooserButtonOptions );

				return ret;

			} break;
		case CgetIdx: {} break;
		case OnClickedIdx:
			{

#ifdef DEBUG_FILECHOOSER_BUTTON
				printf ( "fileChooserButtonFunc - onClicked \n" );
#endif
			}
			break;
		case ClassIdx:
			{
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "fileChooseButton", -1 ) );
			} break;
		case ParentIdx: {} break;
		case GeometryIdx: {} break;
		case ToplevelIdx: {} break;
	}


	return TCL_OK;
}

/**
/brief
/author     William J Giddings
**/
static void file_changed ( GtkFileChooser *chooser )
{
	gchar *folder = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( chooser ) );
#ifdef DEBUG_FILECHOOSER_BUTTON
	printf ( "%s", folder );
#endif
}

/**
/brief
/author     William J Giddings
**/
int gnoclFileChooserButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkWidget *chooserButton;

	/* check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, chooserButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( chooserButtonOptions );
		return TCL_ERROR;
	}

	/* create and show the chooserButton */
	chooserButton = gtk_file_chooser_button_new ( "Choose a file", GTK_FILE_CHOOSER_ACTION_OPEN );

	gtk_widget_show ( GTK_WIDGET ( chooserButton ) );

	/* intitalise options, and clear memory when done, return error initialisation fails */
	ret = gnoclSetOptions ( interp, chooserButtonOptions, G_OBJECT ( chooserButton ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, chooserButton, chooserButtonOptions );
	}

	gnoclClearOptions ( chooserButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( chooserButton ) );
		return TCL_ERROR;
	}

	/* monitor when the selected folder or file are changed */
//  g_signal_connect (G_OBJECT ( chooserButton ), "selection-changed", G_CALLBACK (folder_changed), NULL;


	/* register the new widget for use with the Tcl interpretor */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( chooserButton ), fileChooserButtonFunc );
}


/**
/brief
/author     William J Giddings
**/
int gnoclFolderChooserButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int       ret;
	GtkWidget *chooserButton;

	/* check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, chooserButtonOptions ) != TCL_OK )
	{
		gnoclClearOptions ( chooserButtonOptions );
		return TCL_ERROR;
	}

	/* create and show the chooserButton */
	chooserButton = gtk_file_chooser_button_new ( "Choose a folder", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER );

	gtk_widget_show ( GTK_WIDGET ( chooserButton ) );

	/* intitalise options, and clear memory when done, return error initialisation fails */
	ret = gnoclSetOptions ( interp, chooserButtonOptions, G_OBJECT ( chooserButton ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, chooserButton, chooserButtonOptions );
	}

	gnoclClearOptions ( chooserButtonOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( chooserButton ) );
		return TCL_ERROR;
	}

	/* monitor when the selected folder or file are changed */
//  g_signal_connect (G_OBJECT ( chooserButton ), "selection-changed", G_CALLBACK (folder_changed), NULL;


	/* register the new widget for use with the Tcl interpretor */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( chooserButton ), fileChooserButtonFunc );
}


/*****/
