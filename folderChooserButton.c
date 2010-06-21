/**
\brief		This module implements the gnocl::fileChooserButton widget.
\author	Peter G. Baum, William J Giddings
\date		2001-03:
**/

/* user documentation */

/** \page page19 gnocl::fileChooserButton
  \section sec gnocl::fileChooserButton
  Implementation of gnocl::fileChooserButton
  \subsection subsection1 Implemented gnocl::fileChooserButton Options
  \subsection subsection2 Implemented gnocl::fileChooserButton Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/fileChooserButton.png"
*/



/*
 *    2008-10: added command, class
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>


static GnoclOption chooserButtonOptions[] =
{
	{ "-onChanged", GNOCL_OBJ, NULL },					/* 0 */
	{ "-folder", GNOCL_OBJ, NULL },						/* 1 */
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },		/* 2 */

};

/**
\brief     This function is called whenever the widget content changes due to user selection
\author    Peter G Baum
\date
**/
static void folder_changed ( GtkFileChooser *chooser )
{
	gchar *folder = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( chooser ) );
#ifdef DEBUG_FOLDERCHOOSER_BUTTON
	printf ( "You just picked: %s\n", folder );
#endif
}

/**
\brief
\author     Peter G Baum
\date
**/
static int configure ( Tcl_Interp *interp, GtkWidget *chooserButton, GnoclOption options[] )
{
	/* what is contained in the options list? */
#ifdef DEBUG_FOLDERCHOOSER_BUTTON
	printf ( "Element 0 contains %s \n", options[0] );
	printf ( "Element 1 contains %s \n", options[1] );
	printf ( "Element 2 contains %s \n", options[2] );
#endif
	/* monitor when the selected folder or file are changed */
	g_signal_connect ( G_OBJECT ( chooserButton ), "selection-changed", G_CALLBACK ( folder_changed ), NULL );

	/* set the location to the user's home director */
	gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( chooserButton ), g_get_home_dir() );

	return TCL_OK;
}

/**
\brief
\author		Peter G Baum
\date
**/
static int cget ( Tcl_Interp *interp, GtkButton *chooserButton, GnoclOption options[], int idx )
{
	return TCL_OK;
}

/**
\brief     A function to manage changes and events occuring to the widget during runtime
\author    Peter G Baum
\date
**/
static int chooserButtonFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	/* configure	reconfigure widget using switched
	 * cget			get configuration setting for the widget
	 * onChanged	run command already assigned to the widget
	 */
	GtkWidget *button;
	int idx;

	static const char *cmds[] = { "configure", "cget", "delete", "onChanged", "class", NULL };
	enum cmdIdx { ConfigureIdx, CgetIdx, DeleteIdx, OnChangedIdx, ClassIdx };

	button = GTK_WIDGET ( data );

	/* check the total number of args sent to the function */

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	/* get the idx from the ???? */

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "folderChooserButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( button ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, chooserButtonOptions, G_OBJECT ( button ) ) == TCL_OK )
				{
					ret = configure ( interp, button, chooserButtonOptions );
				}

				gnoclClearOptions ( chooserButtonOptions );

				return ret;
			}

			break;
			/* this needs to be altered */
		case OnChangedIdx:

			if ( objc != 2 )
			{
				Tcl_WrongNumArgs ( interp, 2, objv, NULL );
				return TCL_ERROR;
			}

			if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( button ) ) )
			{
				gtk_button_clicked ( button );
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( button ), chooserButtonOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, button, chooserButtonOptions, idx );
				}
			}
	}

	return TCL_OK;

}

/**
\brief		The command used to create the widget and assign handlers to event signals
\author		Peter G Baum
\date
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
	/* gnoclSetOptions -found in parseOption.c */
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
	//g_signal_connect (G_OBJECT ( chooserButton ), "selection-changed", G_CALLBACK (folder_changed), NULL);


	/* register the new widget for use with the Tcl interpretor */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( chooserButton ), chooserButtonFunc );
}

