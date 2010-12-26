/**
\brief      This module implements the gnocl::??commands module.
\authors    Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation
 * split each description into its own file..
 */

/**
\page page12 gnocl::??command??
\section sec DESCRIPTION
        Implementation of...
\subsection subsection2 Implemented Commands
    \verbinclude commands.txt
\subsection subsection3 Sample Tcl Script
**/

/****h* gnocl/command
 * NAME
 * commands.c
 * DESCRIPTION
 * This file implements a Tcl interface to GTK+ and Gnome
 * AUTHOR
 *  Peter G. Baum
 *  William J Giddings
 * CREATION DATE
 * PURPOSE
 * USAGE
 * PARAMETERS
 * COMMANDS
 *  winfo parent id
 *    Returns the id of the widget parent.
 *  winfo toplevel id
 *    Returns the toplevel container for widget. This will be non-window
 *    if the container has not yet been embedded into a GtkWindow.
 *  winfo geometry id
 *    Returns the position and dimensions of the widget in its containing
 *    window as a list in the form {xwin ywin width height xroot yroot}.
 *    Where:
 *      xwin,ywin      position of the widget in its toplevel window
 *      width, height  displayed width and height of th widget
 *      xroot, yroot   position of the widget on the displayed screen
 *  winfo style
 *    Returns a list of style parameters and their setting as applied to
 *    the widget.
 * OPTIONS
 * EXAMPLE
 * FUNCTION
 * NOTES
 * BUGS
 * SEE ALSO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 *
 *  2009-02: added gnoclResourceCmd
 *  2003-03: split from gnocl.c
 * TODO
 * SOURCE
 *****

/*
 * $Id: commands.c,v 1.9 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements a Tcl interface to GTK+ and Gnome
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-04: added gnocl::signalStop
   2009-03: added gnocl::grab, gnocl::beep
            added gnocl::winfo geometry
   2009-03: added gnocl::winfo
   2009-02: added gnocl::resource
   2003-03: split from gnocl.c
**/

#include "gnocl.h"
#include <string.h>
#include <ctype.h>

/**
\brief      Perform an effective break, let the script decide what to do next.
\author     William J Giddings
\date       02-Mar-09
**/
int gnoclSignalStopCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	if ( objc != 3 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "widget signal-type" );
		return TCL_ERROR;
	}

	GtkWidget *widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[1] ), interp );

	g_signal_stop_emission_by_name ( widget , Tcl_GetString ( objv[2] ) );
	return TCL_OK;
}

/**
\brief      Emits a short beep.
\author     William J Giddings
\date       12-Apr-09
\note       Gdk Documentation for the implemented features.
            http://library.gnome.org/devel/gdk/stable/gdk-General.html
**/
int gnoclBeepCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
g_print("gnoclBeepCmd\n");

	gdk_beep;
	return TCL_OK;
}

/**
\brief      Control redirection input events to window.
\author     William J Giddings
\date       12-Apr-09
\note       Gdk Documentation for the implemented features.
            http://library.gnome.org/devel/gdk/stable/gdk-General.html
**/
int gnoclGrabCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmd[] = { "pointer", "keyboard", "release", "status", NULL };
	enum optIdx { PointerIdx, KeyboardIdx, ReleaseIdx, StatusIdx };
	int idx;

	if ( objc != 3 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option widgetid " );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	/*
	    g_print ( "opt = %s  id = %s\n",  Tcl_GetString ( objv[1] ),  Tcl_GetString ( objv[2] ) );
	*/
	switch ( idx )
	{
			/* deny any pointer events, ie. grab all pointer inputs */
		case PointerIdx:
			{
				GtkWidget *widget;
				guint32 activate_time;

				widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );

				gdk_pointer_grab ( widget->window ,
								   TRUE,
								   GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK,
								   NULL, NULL, GDK_CURRENT_TIME );
				break;
			}

			/* deny any keyboard events, ie. grab all keyboard inputs */
		case KeyboardIdx:
			{
				GtkWidget *widget;
				guint32 activate_time;

				widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );
				gdk_keyboard_grab ( widget->window , TRUE, GDK_CURRENT_TIME );
				break;
			}

			/* relase all grabs applied to the current display */
		case ReleaseIdx:
			{
				GtkWidget *widget;
				guint32 activate_time;

				widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );
				gdk_display_pointer_ungrab ( gdk_drawable_get_display ( widget->window ), GDK_CURRENT_TIME );
				gdk_keyboard_ungrab ( GDK_CURRENT_TIME );
			}

			break;

			/* give status on pointer input grab, no equivalent for the keyboard */
		case StatusIdx:
			{
				char buffer[5];

				sprintf ( buffer, "%s", gdk_pointer_is_grabbed );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

	}

	return TCL_OK;
}


/**
\brief      Screen and Display
            size - returns the physical screen size in millimeters
            geometry -returns the logicaL screen size in pixels
\author     William J Giddings
\date       03-Jan-2010
**/
int gnoclScreenCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmd[] = { "size", "geometry", "name", "screens", "composited", "windowList", "toplevels", NULL };
	enum optIdx { SizeIdx, GeometryIdx, NameIdx, ScreensIdx, CompositedIdx, WindowListIdx, TopLevelsIdx };
	int idx;


	if ( objc != 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option " );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	/*
	display = gdk_display_get_default ();
	num_screen = gdk_display_get_n_screens (display);
	displayname = gdk_display_get_name (display);

	*/
	switch ( idx )
	{
		case TopLevelsIdx:
		case WindowListIdx:
			{
				gchar buffer[16];
				GdkScreen *screen;
				GSList *toplevels, *p;
				gchar *widgetName;

				screen = gdk_screen_get_default();

				toplevels = gdk_screen_get_toplevel_windows ( screen );

				/* check to see if the windows are registered with gnocl */
				for ( p = toplevels; p != NULL ; p = p->next )
				{

					/* this returns GDKWindow, not Gtk */
					widgetName = gnoclGetNameFromWidget ( GTK_WIDGET ( p->data ) );
					printf ( "toplevel %s\n", widgetName );

					if ( widgetName != NULL )
					{
						printf ( "toplevel %s\n", widgetName );
					}

					else
					{
						printf ( "Missed this one! List length = %d\n", g_slist_length ( toplevels ) );
						printf ( "toplevel %s\n", widgetName );
					}
				}

				gboolean state = 0;

				sprintf ( &buffer, "%d", state );
				Tcl_SetObjResult ( interp, Tcl_NewIntObj ( state ) );
			}
			break;
		case CompositedIdx:
			{
				gchar buffer[16];
				GdkScreen *screen;
				gboolean state;

				screen = gdk_screen_get_default();
				state =  gdk_screen_is_composited ( screen );

				sprintf ( &buffer, "%d", state );
				Tcl_SetObjResult ( interp, Tcl_NewIntObj ( state ) );
			}
			break;
		case ScreensIdx:
			{
				gchar buffer[16];
				GdkDisplay *display;

				display = gdk_display_get_default ();
				gint   screens =  gdk_display_get_n_screens ( display );

				sprintf ( &buffer, "%d", screens );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}
			break;
		case NameIdx:
			{
				gchar buffer[16];
				GdkDisplay *display;
				const gchar *name;

				display  = gdk_display_get_default ();
				name = gdk_display_get_name ( display );

				sprintf ( &buffer, "%s", name );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}
			break;
		case SizeIdx:
			{
				gchar buffer[16];
				gint width_mm = 0;
				gint height_mm = 0;
				GdkScreen *screen;

				screen = gdk_screen_get_default();
				width_mm = gdk_screen_get_width_mm ( screen );
				height_mm = gdk_screen_get_height_mm ( screen );

				sprintf ( &buffer, "%d %d", width_mm, height_mm );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
		case GeometryIdx:
			{
				gchar buffer[16];

				gint width = 0;
				gint height = 0;
				GdkScreen *screen;
				screen = gdk_screen_get_default();

				width = gdk_screen_get_width ( screen );
				height = gdk_screen_get_height ( screen );

				sprintf ( &buffer, "%d %d", width, height );

				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
	}

	return TCL_OK;
}

// checking end

/**
\brief      Return window/widget-related information
\author     William J Giddings
\date       01-Mar-09
**/
int gnoclSnapshotCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "snapshot widgetid -rectangle {x y w h}" );
		return TCL_ERROR;
	}

	GtkWidget *widget;
	GdkRectangle *rect;

	widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );

	return TCL_OK;
}



/**
\brief      Return window/widget-related information
\author     William J Giddings
\date       01-Mar-09
**/
int gnoclWinfoCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmd[] = { "parent", "toplevel", "geometry", "style", NULL };
	enum optIdx { ParentIdx, ToplevelIdx, GeometryIdx, StyleIdx };
	int idx;


	if ( objc != 3 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option widgetid " );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}


	// g_print ( "opt = %s  id = %s\n",  Tcl_GetString ( objv[1] ),  Tcl_GetString ( objv[2] ) );

	switch ( idx )
	{
		case ParentIdx:
			{
				char buffer[128];
				GtkWidget *widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );
				GtkWidget *parent = gtk_widget_get_parent ( widget );

				sprintf ( buffer, "%s", gnoclGetNameFromWidget ( parent ) );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
		case ToplevelIdx:
			{
				char buffer[128];
				GtkWidget *widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );
				GtkWidget *toplevel = gtk_widget_get_toplevel ( widget );
				sprintf ( buffer, "%s", gnoclGetNameFromWidget ( toplevel ) );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
			/* probably best leave this to the widget commands themselves */
		case GeometryIdx:
			{
				char buffer[128];
				GtkWidget *widget;
				GtkRequisition req;
				gint x, y;

				widget = gnoclGetWidgetFromName (  Tcl_GetString ( objv[2] ), interp );

				gdk_window_get_origin ( widget->window, &x, &y );

				x += widget->allocation.x;
				x -= widget->style->xthickness;

				y += widget->allocation.y;
				y -= widget->style->ythickness;

				gtk_widget_size_request ( widget, &req );

				sprintf ( buffer, "%d %d %d %d", x, y, req.width, req.height );

				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
		case StyleIdx:
			{
				char buffer[128];
				sprintf ( buffer, "%s", "Style" );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
	}

	return TCL_OK;
}

/**
\brief      Retrieve different information.
\author     Peter G. Baum
**/
int gnoclInfoCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmd[] = { "version", "gtkVersion",
								 "hasGnomeSupport", "allStockItems", "breakpoint", NULL
							   };
	enum optIdx { VersionIdx, GtkVersionIdx,
				  HasGnomeIdx, AllStockItems, BreakpointIdx
				};
	int idx;

	if ( objc != 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT,
							   &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case VersionIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( VERSION, -1 ) );
			break;
		case GtkVersionIdx:
			{
				char buffer[128];
				sprintf ( buffer, "%d.%d.%d", gtk_major_version,
						  gtk_minor_version, gtk_micro_version );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( buffer, -1 ) );
			}

			break;
		case HasGnomeIdx:
			Tcl_SetObjResult ( interp, Tcl_NewBooleanObj (
#ifdef GNOCL_USE_GNOME
								   1
#else
								   0
#endif
							   ) );
			break;
		case AllStockItems:
			{
				Tcl_Obj *res = Tcl_NewListObj ( 0, NULL );
				GSList *ids = gtk_stock_list_ids();
				GSList *p;

				for ( p = ids; p != NULL; p = p->next )
				{
					char *txt = p->data;
					int skip = 0;
					/* FIXME: gtk-missing-image, gtk-dnd-multiple and gtk-dnd
					          fail lookup, why?
					{
					   GtkStockItem sp;
					   printf( "%s lookup: %d\n", txt,
					         gtk_stock_lookup( txt, &sp ) );
					}
					**/

					/* see createStockName and gnoclGetStockName */

					if ( strncmp ( txt, "gtk", 3 ) == 0 )
						skip = 3;

#ifdef GNOCL_USE_GNOME
					else if ( strncmp ( txt, "gnome-stock", 11 ) == 0 )
						skip = 11;

#endif

					if ( skip > 0 )
					{
						GString *name = g_string_new ( NULL );
						char *tp = txt + skip;

						for ( ; *tp; ++tp )
						{
							if ( *tp == '-' )
							{
								++tp;
								g_string_append_c ( name, toupper ( *tp ) );
							}

							else
								g_string_append_c ( name, *tp );
						}

						Tcl_ListObjAppendElement ( interp, res,

												   Tcl_NewStringObj ( name->str, -1 ) );
						/* printf( "%s -> %s\n", (char *)p->data, name->str ); */
						g_string_free ( name, 1 );
					}

					else
						Tcl_ListObjAppendElement ( interp, res,
												   Tcl_NewStringObj ( txt, -1 ) );

					g_free ( p->data );
				}

				g_slist_free ( ids );

				Tcl_SetObjResult ( interp, res );
			}

			break;
		case BreakpointIdx:
			/* this is only for debugging */
			G_BREAKPOINT();
			break;
	}

	return TCL_OK;
}

/**
**/
int gnoclUpdateCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int nMax = 500;
	int n;

	if ( objc != 1 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, NULL );
		return TCL_ERROR;
	}

	for ( n = 0; n < nMax && gtk_events_pending(); ++n )
		gtk_main_iteration_do ( 0 );

	Tcl_SetObjResult ( interp, Tcl_NewIntObj ( n ) );

	return TCL_OK;
}

/**
\brief      Load and configure widget styles using a resource (.rsc) file.
\author     William j Giddings
\date       25-FEB-2009
**/
int gnoclResourceFileCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	g_print ( "gnoclResourceFileCmd\n" ) ;

	int nMax = 500;
	int n;

	if ( objc != 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, NULL );
		return TCL_ERROR;
	}

	char * str = Tcl_GetString ( objv[1] );

	gtk_rc_parse ( str );

	Tcl_SetObjResult ( interp, Tcl_NewIntObj ( n ) );

	return TCL_OK;
}

/**
**/
int gnoclConfigureCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	GnoclOption options[] =
	{
		{ "-tooltip", GNOCL_BOOL, NULL },
		{ "-defaultIcon", GNOCL_OBJ, NULL },
		{ NULL }
	};
	const int tooltipIdx     = 0;
	const int defaultIconIdx = 1;

	int ret = TCL_ERROR;

	if ( gnoclParseOptions ( interp, objc, objv, options ) != TCL_OK )
		goto cleanExit;

	if ( options[defaultIconIdx].status == GNOCL_STATUS_CHANGED )
	{
		GnoclStringType type = gnoclGetStringType (
								   options[defaultIconIdx].val.obj );

		switch ( type )
		{
			case GNOCL_STR_EMPTY:
				gtk_window_set_default_icon_list ( NULL );
				break;
			case GNOCL_STR_FILE:
				{
					GdkPixbuf *pix = gnoclPixbufFromObj ( interp,
														  options + defaultIconIdx );
					GList *list = NULL;

					if ( pix == NULL )
						goto cleanExit;

					list = g_list_append ( list, pix );

					gtk_window_set_default_icon_list ( list );

					g_list_free ( list );
				}

				break;
			default:
				Tcl_AppendResult ( interp, "Unknown type for \"",
								   Tcl_GetString ( options[defaultIconIdx].val.obj ),
								   "\" must be of type FILE (%/) or empty", NULL );
				goto cleanExit;
		}
	}

	if ( options[tooltipIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( options[tooltipIdx].val.b )
			gtk_tooltips_enable ( gnoclGetTooltips() );
		else
			gtk_tooltips_disable ( gnoclGetTooltips() );
	}

	ret = TCL_OK;

cleanExit:
	gnoclClearOptions ( options );
	return ret;
}

/**
\author     Peter G. Baum
**/
int gnoclClipboardCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	GnoclOption options[] =
	{
		{ "-primary", GNOCL_BOOL, NULL },
		{ NULL }
	};
	const int usePrimaryIdx = 0;

	static const char *cmd[] = { "hasText", "setText", "getText", "clear",
								 NULL
							   };
	enum optIdx { HasTextIdx, SetTextIdx, GetTextIdx, ClearIdx };
	int idx;
	int optNum;
	GtkClipboard *clip;
	int usePrimary = 0;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "option" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmd, "option", TCL_EXACT,
							   &idx ) != TCL_OK )
		return TCL_ERROR;

	if ( idx == SetTextIdx )
	{
		optNum = 2;

		if ( objc < 3 )
		{
			Tcl_WrongNumArgs ( interp, 1, objv, "text ?option value?" );
			return TCL_ERROR;
		}
	}

	else
	{
		optNum = 1;

		if ( objc < 2 )
		{
			Tcl_WrongNumArgs ( interp, 1, objv, NULL );
			return TCL_ERROR;
		}
	}

	if ( gnoclParseOptions ( interp, objc - optNum, objv + optNum, options )
			!= TCL_OK )
		return TCL_ERROR;

	if ( options[usePrimaryIdx].status == GNOCL_STATUS_CHANGED )
		usePrimary = options[usePrimaryIdx].val.b;

	clip = gtk_clipboard_get ( usePrimary ? gdk_atom_intern ( "PRIMARY", 1 )
							   : GDK_NONE );

	switch ( idx )
	{
		case HasTextIdx:
			{
				int ret = gtk_clipboard_wait_is_text_available ( clip );
				Tcl_SetObjResult ( interp, Tcl_NewBooleanObj ( ret ) );
			}

			break;
		case SetTextIdx:
			gtk_clipboard_set_text ( clip, Tcl_GetString ( objv[2] ), -1 );
			break;
		case GetTextIdx:
			{
				char *txt = gtk_clipboard_wait_for_text ( clip );

				if ( txt )
				{
					Tcl_SetObjResult ( interp, Tcl_NewStringObj ( txt, -1 ) );
					g_free ( txt );
				}

				/* FIXME? else error? */
			}

			break;
		case ClearIdx:
			gtk_clipboard_clear ( clip );
			break;
	}

	return TCL_OK;
}

