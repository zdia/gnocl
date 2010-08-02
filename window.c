/*
 * $Id: window.c,v 1.16 2005/08/16 20:57:45 baum Exp $
 *
 * This file implements the window widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/**
\brief
    This module implements the gnocl::window widget.
\authors
    Peter G. Baum, William J Giddings
\date
    2001-03:
\page page_window gnocl::window
\section sec gnocl::window
  Implementation of gnocl::window
\subsection subsection1 Implemented Options
    \verbinclude window_options.txt
\subsection subsection2 Implemented Commands
    \verbinclude window_commands.txt
\subsection subsection3 Sample Tcl Script
    \code
    set box [gnocl::box -buttonType 1]
    $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
    $box add [gnocl::button -text "%#Cancel"]
    $box add [gnocl::button -text "%#Save"]
    gnocl::window -title "Buttons" -child $box
    \endcode
\subsection subsection4 Produces
    \image html "../pics/window.png"
\paragraph history1 (Development History)
    \verbatim
    2009-12: added show, hide, synonymous with use of option -visible
    2009-02: added -bakckgroundImage, -mask
    2009-01: added -keepAbove, -keepBelow, -acceptFocus,
            -deletable, -destroyWithParent, -focusOnMap,
            -gravity, -hasTopLevelFocus, -isActive,
            -skipPagerHint, -skipTaskBarHint, -urgencyHint,
            -backgroundColor, fullScreen, geometry
    2008-10: added -transient, class
    2008-03: new windows are always centered on screen
    2007-10: added center
    2004-02: added -data
         11: added cget, -x, -y, -width and -height
         10: added onKey{Press,Release}
         04: added -icon
         02: added drag and drop options
    2003-01: small fixes, added options "-name"
         09: switched from GnoclWidgetOptions to GnoclOption
             more updates, more and some renamed options
    2002-04: update for gtk 2.0
    2001-03: Begin of developement
    \endverbatim
\todo
    \verbatim
    options
       gtk_window_maximize(GTK_WINDOW(window));
       add_accel_group
       remove_accel_group
       set_modal
       set_default_size
       set_geometry_hints
       set_gravity
       get_gravity
       set_position
       set_screen
       get_screen
       is_active
       get_focus
       set_focus
       set_default
       set_frame_dimensions
       set_has_frame
       set_role
       get_role
       set_startup_id
       set_default_icon_list
       default_icon_list
       get_icon_list
       get_icon_name
       set_default_icon_name
       set_icon
       set_icon_list
       set_icon_from_file
       set_icon_name
       set_auto_startup_notification
   commands
       activate_focus
       activate_default
       list_toplevels
       add_mnemonic
       remove_mnemonic
       set_mnemonic_modifier
       get_mnemonic_modifier
       mnemonic_activate
       activate_key
       propagate_key_event
       present
       present_with_time
       begin_resize_drag
       begin_move_drag
       get_modal
       window_get_position
       get_size
       get_group
       move
       parse_geometry
       resize
    Signals
       activate-default
       activate-focus
       frame-event
       keys-changed
       set-focus
    \endverbatim
*/

#include "gnocl.h"
#include <gdk/gdk.h>    /* wjg 05/07/08 */
#include <string.h>
#include <assert.h>
#include <stdlib.h>     /* atof */

#include <glade/glade.h>

/*! \page page3 Glade GUI Description Files Loader
  \section sec Glade File Load Command.
  Implementation Gnocl bindings to the Gtk Libglade Graphical Interface Description Loader API.
  \subsection subsection1 An example section

  http://library.gnome.org/devel/libglade/unstable/index.html
*/

/**
\brief      Load buffer, obtain alpha channel and return pointer.
\author     William J Giddings
\date       13-FEB-09
\todo       Rename the function to gnoclGetBitmapMask and move to helperFuncs.c
            See eventBox get_bitmap2
**/
static GdkBitmap *getBitmapMask ( gchar *filename )
{
	GdkPixbuf *pbuf;
	GdkBitmap *ret;
	GError *err = NULL;

	g_return_val_if_fail ( filename != NULL, NULL );

	pbuf = gdk_pixbuf_new_from_file ( filename, &err );

	if ( err != NULL )
	{
		g_warning ( "%s", err->message );
		g_error_free ( err );
		return NULL;
	}

	/* you may want to change the threshold, depending on your image */
	gdk_pixbuf_render_pixmap_and_mask ( pbuf, NULL, &ret, 1 );

	//g_object_unref ( pbuf );

	return ret;
}


/**
\brief      Description yet to be added.
\author     William J Giddings
\date       14-05-09
**/
static gboolean doOnActiveDefault ( GtkWidget *window, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 't', GNOCL_STRING },  /* toplevel window */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( window );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}

/**
\brief      Set handling of window specific signal.
\author     William J Giddings
\date       14-05-09
**/
int gnoclOptOnActivateDefault ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onActivateDefault" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "activate-default", G_CALLBACK ( doOnActiveDefault ), opt, NULL, ret );
}

/**
\brief      Description yet to be added.
\author     William J Giddings
\date       14-05-09
**/
static gboolean doOnActivateFocus ( GtkWidget *window, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 't', GNOCL_STRING },  /* toplevel window */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( window );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}


/**
\brief      Set handling of window specific signal.
\author     William J Giddings
\date       14-05-09
**/
int gnoclOptOnActivateFocus ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onActivateFocus" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "activate-focus", G_CALLBACK ( doOnActivateFocus ), opt, NULL, ret );
}



/**
\brief      Description yet to be added.
\author     William J Giddings
\date       14-05-09
**/
static gboolean doOnFrameEvent ( GtkWindow *window, GdkEvent  *event, gpointer  data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 't', GNOCL_STRING },  /* toplevel window */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( window );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}



/**
\brief      Set handling of window specific signal.
\author     William J Giddings
\date       14-05-09
**/
int gnoclOptOnFrameEvent ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onFrameEvent" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "frame-event", G_CALLBACK ( doOnFrameEvent ), opt, NULL, ret );
}


/**
\brief      Description yet to be added.
\author     William J Giddings
\date       14-05-09
**/
static gboolean doOnKeysChanged ( GtkWindow *window, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 't', GNOCL_STRING },  /* toplevel window */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( window );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}




/**
\brief      Set handling of window specific signal.
\author     William J Giddings
\date       14-05-09
**/
int gnoclOptOnKeysChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onKeysChanged" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "keys-changed", G_CALLBACK ( doOnKeysChanged ), opt, NULL, ret );
}

/**
\brief      Description yet to be added.
\author     William J Giddings
\date       14-05-09
**/
static gboolean doOnSetFocus ( GtkWidget *window, GtkWidget *widget, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 't', GNOCL_STRING },  /* toplevel window */
		{ 'w', GNOCL_STRING },  /* widget */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( window );
	ps[1].val.str = gnoclGetNameFromWidget ( widget );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}


/**
\brief      Set handling of window specific signal.
\author     William J Giddings
\date       14-05-09
**/
int gnoclOptOnSetFocus ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onSetFocus" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "set-focus", G_CALLBACK ( doOnSetFocus ), opt, NULL, ret );
}

static GnoclOption windowOptions[] =
{
	{ "-defaultWidth", GNOCL_INT, "default-width" },     /* these must be */
	{ "-defaultHeight", GNOCL_INT, "default-height" },   /* before -visible! */
	{ "-decorated", GNOCL_BOOL, "decorated" },           /* 2 */
	{ "-visible", GNOCL_BOOL, "visible" },               /* 3 */
	{ "-x", GNOCL_INT, NULL },                           /* 4 */
	{ "-y", GNOCL_INT, NULL },                           /* 5 */
	{ "-width", GNOCL_INT, NULL },                       /* 6 */
	{ "-height", GNOCL_INT, NULL },                      /* 7 */
	{ "-allowGrow", GNOCL_BOOL, "allow-grow" },
	{ "-allowShrink", GNOCL_BOOL, "allow-shrink" },
	{ "-borderWidth", GNOCL_OBJ, "border-width", gnoclOptPadding },
	{ "-child", GNOCL_OBJ, "", gnoclOptChild },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-dragTargets", GNOCL_LIST, "s", gnoclOptDnDTargets },
	{ "-dropTargets", GNOCL_LIST, "t", gnoclOptDnDTargets },
	{ "-heightRequest", GNOCL_INT, "height-request" },
	{ "-icon", GNOCL_OBJ, "", gnoclOptIcon },
	{ "-modal", GNOCL_BOOL, "modal" },
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
	{ "-widthRequest", GNOCL_INT, "width-request" },
	{ "-typeHint", GNOCL_OBJ, "", gnoclOptWindowTypeHint },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },

	{ "-transient", GNOCL_OBJ, "", gnoclOptTransientWindow },
	{ "-keepAbove", GNOCL_OBJ, "", gnoclOptKeepAbove },
	{ "-keepBelow", GNOCL_OBJ, "", gnoclOptKeepBelow },
	{ "-stick", GNOCL_OBJ, "", gnoclOptStick },
	{ "-opacity", GNOCL_OBJ, "", gnoclOptOpacity },

	{ "-acceptFocus", GNOCL_BOOL, "accept-focus" },
	{ "-deletable", GNOCL_BOOL, "deletable" },
	{ "-destroyWithParent", GNOCL_BOOL, "destroy-with-parent" },
	{ "-focusOnMap", GNOCL_BOOL, "focus-on-map" },
	{ "-gravity", GNOCL_BOOL, "gravity" },
	{ "-hasToplevelFocus", GNOCL_BOOL, "has-toplevel-focus" },
	{ "-isActive", GNOCL_BOOL, "is-active" },
	{ "-fullScreen", GNOCL_OBJ, "", gnoclOptFullScreen},

	{ "-skipPagerHint", GNOCL_BOOL, "skip-pager-hint" },
	{ "-skipTaskBarHint", GNOCL_BOOL, "skip-taskbar-hint" },
	{ "-urgencyHint", GNOCL_BOOL, "urgency-hint" },

	{ "-backgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ "-onEnter", GNOCL_OBJ, "E", gnoclOptOnEnterLeave },
	{ "-onLeave", GNOCL_OBJ, "L", gnoclOptOnEnterLeave },

	/* I'm not certain if these events are suitable for the window widget */
	{ "-onMoveFocus", GNOCL_OBJ, "", gnoclOptOnMoveFocus },
	{ "-onFocusIn", GNOCL_OBJ, "I", gnoclOptOnFocus },
	{ "-onFocusOut", GNOCL_OBJ, "O", gnoclOptOnFocus },

	{ "-mask", GNOCL_OBJ, "", gnoclOptMask },
	{ "-backgroundImage", GNOCL_OBJ, "", gnoclOptBackgroundImage },
	{ "-cursor", GNOCL_OBJ, "", gnoclOptCursor },
	{ "-hasFrame", GNOCL_OBJ, "", gnoclOptHasFrame },

	/* non-default options for the GtkWindow */
	{ "-onMotion", GNOCL_OBJ, "", gnoclOptOnMotion },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },

	/* window specific signals */
	/* "activate-default" , "activate-focus", "frame-event", "keys-changed", "set-focus" */

	{ "-onActivateDefault", GNOCL_OBJ, "", gnoclOptOnActivateDefault },
	{ "-onActivateFocus", GNOCL_OBJ, "", gnoclOptOnActivateFocus },
	{ "-onFrameEvent", GNOCL_OBJ, "", gnoclOptOnFrameEvent },
	{ "-onKeysChanged", GNOCL_OBJ, "", gnoclOptOnKeysChanged },
	{ "-onSetFocus", GNOCL_OBJ, "", gnoclOptOnSetFocus },


	{ NULL }
};




static const int decoratedIdx   = 2;
static const int visibleIdx     = 3;
static const int xIdx           = 4;
static const int yIdx           = 5;
static const int widthIdx       = 6;
static const int heightIdx      = 7;

/**
\brief      Description yet to be added.
\author
\date
**/
static gboolean doOnConfigure ( GtkWidget *widget, GdkEventConfigure *event, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget */
		{ 'x', GNOCL_INT },     /*  */
		{ 'y', GNOCL_INT },     /*  */
		{ 'W', GNOCL_INT },     /*  */
		{ 'H', GNOCL_INT },     /*  */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( widget );
	ps[1].val.i = event->x;
	ps[2].val.i = event->y;
	ps[3].val.i = event->width;
	ps[4].val.i = event->height;
	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );

}

/**
**/
int gnoclOptOnConfigure ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( strcmp ( opt->optName, "-onResize" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "configure-event", G_CALLBACK ( doOnConfigure ), opt, NULL, ret );
}

/**
**/
int gnoclOptTransientWindow ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	/* get the name of the parent window from options */
	const char *name = Tcl_GetString ( opt->val.obj );
	GtkWidget *parent = gnoclGetWidgetFromName ( name, interp );

	gtk_window_set_transient_for (  GTK_WINDOW ( obj ) , GTK_WINDOW ( parent ) );
	gtk_window_set_destroy_with_parent  ( GTK_WINDOW ( obj ) , 1 );

	return TCL_OK;
}

/**
\brief      Causes window to remain displayed on top of all others.
\author     William J Giddings
\date       12-01-09
**/

int gnoclOptKeepAbove ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	if ( strcmp ( Tcl_GetString ( opt->val.obj ), "1" ) == 0 )
	{
		gtk_window_set_keep_above ( GTK_WINDOW ( obj ), 1 );
	}

	else
	{
		gtk_window_set_keep_above ( GTK_WINDOW ( obj ), 0 );
	}

	return TCL_OK;
}

/**
\brief      Causes window to remain displayed below of all others.
\author     William J Giddings
\date       12-01-09
**/
int gnoclOptKeepBelow (
	Tcl_Interp  *interp,
	GnoclOption *opt,
	GObject     *obj,
	Tcl_Obj     **ret )
{

	if ( strcmp ( Tcl_GetString ( opt->val.obj ), "1" ) == 0 )
	{
		gtk_window_set_keep_below ( GTK_WINDOW ( obj ), 1 );
	}

	else
	{
		gtk_window_set_keep_below ( GTK_WINDOW ( obj ), 0 );
	}

	return TCL_OK;
}

/**
\brief      Causes window to remain displayed below of all others.
\author     William J Giddings
\date       12-01-09
**/
int gnoclOptFullScreen ( Tcl_Interp  *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	if ( strcmp ( Tcl_GetString ( opt->val.obj ), "1" ) == 0 )
	{
		gtk_window_fullscreen ( GTK_WINDOW ( obj ) );
	}

	else
	{
		gtk_window_set_keep_above ( GTK_WINDOW ( obj ), 0 );
		gtk_window_unfullscreen ( GTK_WINDOW ( obj ) );
	}

	return TCL_OK;
}

/**
\brief      Causes window to be displayed on all desktops.
\author     William J Giddings
\date       12 -01 - 09
**/
int gnoclOptStick ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	if ( strcmp ( Tcl_GetString ( opt->val.obj ), "1" ) == 0 )
	{
		gtk_window_stick   ( GTK_WINDOW ( obj ) );
	}

	else
	{
		gtk_window_unstick ( GTK_WINDOW ( obj ) );
	}

	return TCL_OK;
}

/*****/

/****f* window/gnoclOptMask
 * NAME
 *  functionName
 * PURPOSE
 *  Apply mask to the
 * AUTHOR
 *  William J Giddings
 * CREATION DATE
 *  12-01-09
 * USAGE
 *  how this function is used
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 *  http://www.gnu.org/software/guile-gnome/docs/gdk/html/Windows.html
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
int gnoclOptMask ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{


	GdkBitmap *mask;
	mask = getBitmapMask ( Tcl_GetString ( opt->val.obj ) );

	gtk_widget_shape_combine_mask ( obj, mask, 0, 0 );

	return TCL_OK;
}

/*****/


/****f* window/gnoclOptOpacity
 * NAME
 *  functionName
 * PURPOSE
 *  Apply mask to the
 * AUTHOR
 *  William J Giddings
 * CREATION DATE
 *  12-01-09
 * USAGE
 *  how this function is used
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 *  http://www.gnu.org/software/guile-gnome/docs/gdk/html/Windows.html
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
int gnoclOptOpacity (   Tcl_Interp  *interp,    GnoclOption *opt,   GObject     *obj,   Tcl_Obj     **ret )
{
	g_print ( "window/gnoclOptOpacity\n" );
	gtk_window_set_opacity ( GTK_WINDOW ( obj ), atof ( Tcl_GetString ( opt->val.obj ) ) );

	return TCL_OK;
}

/*****/

/****f* window/gnoclOptHasFrame
 * NAME
 *  functionName
 * PURPOSE
 *
 * AUTHOR
 *  William J Giddings
 * CREATION DATE
 *  09-02-09
 * USAGE
 *  how this function is used
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 *
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
int gnoclOptHasFrame (  Tcl_Interp *interp, GnoclOption *opt,   GObject *obj, Tcl_Obj **ret )
{
	g_print ( "window/gnoclOptHasFrame\n" );

	gtk_window_set_has_frame ( GTK_WINDOW ( obj ),  Tcl_GetString ( opt->val.obj ) );
	//gtk_window_set_opacity ( GTK_WINDOW ( obj ), atof ( Tcl_GetString ( opt->val.obj ) ) );

	return TCL_OK;
}

/**
\brief     Set the opacity of the specified window.
\author    William J Giddings
\date      12-01-09
\bug       Setting the backgroundImage for any window will set change the style
           for all windows created by this instance of the interpreter.
           How can the bgImage be changed without using styles?
\todo      By default, cause the window to be resized to fit the graphic.
           ie. widthRequest & hieghtRequest set from pixBud size...
 */
int gnoclOptBackgroundImage (   Tcl_Interp  *interp, GnoclOption *opt,  GObject *obj, Tcl_Obj **ret )
{

	GdkPixmap *background;
	GdkBitmap *mask = NULL;
	GdkPixbuf *pixbuf;
	GdkScreen *ourscreen;
	GdkColormap *colormap;
	GtkStyle *style;
	GError *error = NULL;
	gchar pixbufName;
	gchar maskName;

	printf ( "files %s\n", Tcl_GetString ( opt->val.obj ) );

	pixbuf = gnoclBlendPixbufFromObj ( interp, opt );


	/* create a pixbuf from a disk file */

	if ( 0 )
	{
		char * str = Tcl_GetString ( opt->val.obj );
		char * pch;

		pch = strtok ( str, " " );
		int aa = 0;

		while ( pch != NULL )
		{
			switch ( aa )
			{
				case 0:
					{
						//printf ( "pixbuf %s\n", pch );
						pixbuf = gdk_pixbuf_new_from_file ( pch , &error );

						if ( error != NULL )
						{
							if ( error->domain == GDK_PIXBUF_ERROR )
							{
								g_print ( "Pixbuf Related Error:\n" );
							}

							if ( error->domain == G_FILE_ERROR )
							{
								g_print ( "File Error: Check file permissions and state:\n" );
							}

							g_printerr ( "%s\n", error[0].message );

							exit ( 1 );
						}

					}

					break;
				case 1:
					{
						//printf ( "mask %s\n", pch );
						mask = getBitmapMask ( pch );
					}

					break;
			}

			if ( aa < 3 )
			{
				aa++ ;
			}

			else
			{
				aa = 0;
			}

			pch = strtok ( NULL, " " );
		}

	}

	/* create the background graphic */

	if ( 0 )
	{
		/* for some reason this doesn't work! */
		gint w, h, alpha;
		GdkPixbuf *background2;
		alpha = 255;

		GtkWidget *widget;

		widget = GTK_WIDGET ( obj );

		w = widget->allocation.width;
		h = widget->allocation.height;

		printf ( "w = %d h = %d\n", w, h );

		/* get the drawing are of the window */

		background2 = gdk_pixbuf_get_from_drawable ( NULL, widget->window, NULL, 0, 0, 0, 0, w, h );

		/*
		void gdk_draw_pixbuf (
		    GdkDrawable *drawable,
		    GdkGC *gc,
		    const GdkPixbuf *pixbuf,
		    gint src_x, gint src_y, gint dest_x, gint dest_y,
		    gint width, gint height,
		    GdkRgbDither dither,
		    gint x_dither, gint y_dither);
		*/

		gdk_draw_pixbuf (
			GDK_DRAWABLE ( obj ) ,
			NULL,
			background2,
			0, 0, 0, 0,
			w, h,
			GDK_RGB_DITHER_NONE,
			0, 0 );

		gdk_pixbuf_composite ( pixbuf, background2, 0, 0, w, h, 0, 0, 1, 1, GDK_INTERP_BILINEAR, alpha );

	}

	else
	{
		/* this method uses styles */
		/* this will load the buffer, but ignore transparency settings */

		/* this is spliting pixbuf into the bg image and its alpha-channel */
		gdk_pixbuf_render_pixmap_and_mask ( pixbuf, &background, &mask, 255 );

		/* get a copy of the new window style */
		style = gtk_style_copy ( GTK_WIDGET ( obj )->style );

		/* modify it */
		style->bg_pixmap[0] = background;

		/* apply the new style to the window */
		gtk_widget_set_style ( GTK_WIDGET ( obj ), GTK_STYLE ( style ) );

		/* release allocated buffers, take care, if they are released they will not be rendered! */

		if ( 0 )
		{
			g_object_unref ( pixbuf );
			g_object_unref ( background );
		}

	}

	return TCL_OK;

}

/**
/brief
/author   Peter G Baum
          William J Giddings
/date
**/
int gnoclLabelEntryCmd (    ClientData data,    Tcl_Interp *interp, int objc,   Tcl_Obj * const objv[] )
{
	return TCL_OK;
}

/**
\brief    Description yet to be added.
*/
int gnoclOptWindowTypeHint ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	const char *txt[] = { "normal", "dialog", "menu", "toolbar", "splashscreen", "utility", "dock", "desktop", NULL
						};
	GdkWindowTypeHint types[] = { GDK_WINDOW_TYPE_HINT_NORMAL,
								  GDK_WINDOW_TYPE_HINT_DIALOG, GDK_WINDOW_TYPE_HINT_MENU,
								  GDK_WINDOW_TYPE_HINT_TOOLBAR, GDK_WINDOW_TYPE_HINT_SPLASHSCREEN,
								  GDK_WINDOW_TYPE_HINT_UTILITY, GDK_WINDOW_TYPE_HINT_DOCK,
								  GDK_WINDOW_TYPE_HINT_DESKTOP
								};

	if ( ret == NULL ) /* set value */
	{
		int idx;

		if ( Tcl_GetIndexFromObj ( interp, opt->val.obj, txt, "type hint", TCL_EXACT, &idx ) != TCL_OK )
			return TCL_ERROR;

		gtk_window_set_type_hint ( GTK_WINDOW ( obj ), types[idx] );
	}

	else /* get value */
	{
		GdkWindowTypeHint val = gtk_window_get_type_hint ( GTK_WINDOW ( obj ) );
		int k;

		for ( k = 0; txt[k]; ++k )
		{
			if ( types[k] == val )
			{
				*ret = Tcl_NewStringObj ( txt[k], -1 );
				return TCL_OK;
			}
		}

		Tcl_SetResult ( interp, "Unknown setting for parameter", TCL_STATIC );

		return TCL_ERROR;
	}

	return TCL_OK;
}

/**
\brief
    Description yet to be added.
*/
static int configure ( Tcl_Interp *interp, GtkWindow *window, GnoclOption options[] )
{

	/* make only one move if x and y are set */
	if ( options[xIdx].status == GNOCL_STATUS_CHANGED
			&& options[xIdx].status == GNOCL_STATUS_CHANGED )
	{
		gtk_window_move ( window, options[xIdx].val.i, options[yIdx].val.i );
	}

	else if ( options[xIdx].status == GNOCL_STATUS_CHANGED
			  || options[yIdx].status == GNOCL_STATUS_CHANGED )
	{
		int x, y;
		gtk_window_get_position ( window, &x, &y );

		if ( options[xIdx].status == GNOCL_STATUS_CHANGED )
			x = options[xIdx].val.i;
		else
			y = options[yIdx].val.i;

		gtk_window_move ( window, x, y );
	}

	/* get_size does not return size set by resize if the resize event
	   is not yet handled, we therefor use get_size only if really necessary */

	if ( options[widthIdx].status == GNOCL_STATUS_CHANGED
			&& options[heightIdx].status == GNOCL_STATUS_CHANGED )
	{
		gtk_window_resize ( window, options[widthIdx].val.i,
							options[heightIdx].val.i );
	}

	else if ( options[widthIdx].status == GNOCL_STATUS_CHANGED
			  || options[heightIdx].status == GNOCL_STATUS_CHANGED )
	{
		int width, height;
		gtk_window_get_size ( window, &width, &height );

		if ( options[widthIdx].status == GNOCL_STATUS_CHANGED )
			width = options[widthIdx].val.i;
		else
			height = options[heightIdx].val.i;

		gtk_window_resize ( window, width, height );
	}

	return TCL_OK;
}

/**
\brief
    Description yet to be added.
*/
static int cget (   Tcl_Interp *interp, GtkWindow *window,  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == xIdx )
	{
		int x, y;
		gtk_window_get_position ( window, &x, &y );
		obj = Tcl_NewIntObj ( x );
	}

	else if ( idx == yIdx )
	{
		int x, y;
		gtk_window_get_position ( window, &x, &y );
		obj = Tcl_NewIntObj ( y );
	}

	else if ( idx == widthIdx )
	{
		int width, height;
		gtk_window_get_size ( window, &width, &height );
		obj = Tcl_NewIntObj ( width );
	}

	else if ( idx == heightIdx )
	{
		int width, height;
		gtk_window_get_size ( window, &width, &height );
		obj = Tcl_NewIntObj ( height );
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/**
/brief
/author   Peter G Baum
          William J Giddings
/date
**/
int windowFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "iconify",
								  "center", "centre", "beep", "class", "reshow",
								  "geometry", "pointer", "reposition", "grab", "ungrab",
								  "hide", "show",
								  NULL
								};
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, IconifyIdx, CenterIdx, CentreIdx,
				  BeepIdx, ClassIdx, ReshowIdx, GeometryIdx, PointerIdx, RepositionIdx,
				  GrabIdx, UngrabIdx, HideIdx, ShowIdx
				};
	GtkWindow *window = GTK_WINDOW ( data );
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
				gtk_widget_hide ( window );
			}
			break;
		case ShowIdx:
			{
				gtk_widget_show_all ( window );
			}
			break;
		case ClassIdx:
			{
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "window", -1 ) );
			}

			break;
			/* WJG added 16/01/09 */
		case GrabIdx:
			{
				gtk_grab_add ( window );
			}

			break;
		case UngrabIdx:
			{
				gtk_grab_remove ( window );
			}

			break;
		case RepositionIdx:
			{
				gint x, y, w, h;

				/* get values from the args list */
				sscanf ( Tcl_GetString ( objv[2] ), "%d %d %d %d", &x, &y, &w, &h );
				gtk_window_move ( window, x, y );
				gtk_window_resize ( window, w, h );
			}

			break;
		case PointerIdx:
			{
				char str[250];
				gint x;
				gint y;
				gtk_widget_get_pointer ( window, &x, &y );
				sprintf ( &str, "%d %d", x, y );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
			}

			break;
		case GeometryIdx:
			{
				/* is this task best achieved using gtk or gdk library calls? */
				char str[250];
				gint root_x;
				gint root_y;
				gint width;
				gint height;
				gtk_window_get_position ( window, &root_x, &root_y );
				gtk_window_get_size ( window, &width, &height );
				sprintf ( &str, "%d %d %d %d", root_x, root_y, width, height );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
			}

			break;
		case ReshowIdx:
			{
				gtk_window_reshow_with_initial_size ( window );
			}

			break;
		case DeleteIdx:
			{
				return gnoclDelete ( interp, GTK_WIDGET ( window ), objc, objv );
			}

			break;
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   windowOptions, G_OBJECT ( window ) ) == TCL_OK )
				{
					ret = configure ( interp, window, windowOptions );
				}

				gnoclClearOptions ( windowOptions );

				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( window ),
									 windowOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, window, windowOptions, idx );
				}

				assert ( 0 );
			}

			break;
		case IconifyIdx:
			{
				int iconify = 1;

				if ( objc == 3 )
				{
					if ( Tcl_GetBooleanFromObj ( interp, objv[2], &iconify )
							!= TCL_OK )
						return TCL_ERROR;
				}

				else if ( objc > 3 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "?state?" );
					return TCL_ERROR;
				}

				if ( iconify )
					gtk_window_iconify ( window );
				else
					gtk_window_deiconify ( window );
			}

			break;
		case CentreIdx:
		case CenterIdx:
			{
				/* WJG (28/03/08)  For some reason GTK_WIN_POS_CENTER has no effect! */
				gtk_window_set_position ( window , GTK_WIN_POS_CENTER_ALWAYS );
			}

			break;
	}

	return TCL_OK;
}

/*****/

/****f* window/gnoclWindowCmd
 * NAME
 *  functionName
 * PURPOSE
 * AUTHOR
 *  Peter G. Baum
 *  William J Giddings
 * CREATION DATE
 *  When created
 * USAGE
 *  how this function is used
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 * TODO
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
int gnoclWindowCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	int        ret;
	GtkWindow  *window;

	assert ( strcmp ( windowOptions[visibleIdx].optName, "-visible" ) == 0 );

	if ( gnoclParseOptions ( interp, objc, objv, windowOptions ) != TCL_OK )
	{
		gnoclClearOptions ( windowOptions );
		return TCL_ERROR;
	}

	window = GTK_WINDOW ( gtk_window_new ( GTK_WINDOW_TOPLEVEL ) );

	/* WJG (28/03/08) New windows are always centred on screen */
	gtk_window_set_position ( window , GTK_WIN_POS_CENTER );

	/* FIXME: each window own accel_group */
	gtk_window_add_accel_group ( window, gnoclGetAccelGroup() );

	ret = gnoclSetOptions ( interp, windowOptions, G_OBJECT ( window ), -1 );

	if ( ret == TCL_OK )
	{
		/* This must be after setting "default-{width,height}".
		   If it is after setting the child widget, we get problems if
		   the child contains a combo widget. Bizarre!
		*/
		if ( windowOptions[visibleIdx].status == 0 )
		{
			gtk_widget_show ( GTK_WIDGET ( window ) );
		}


// added 02.08.2010 zdia
#include "osxmenu.h"

#ifdef GDK_WINDOWING_QUARTZ
		// g_print("menubar\n") ;
		gtk_widget_show_all(GtkOSXMenubar);
#endif

		/* let the window also receive buttonPress events */
		gtk_widget_add_events ( GTK_WIDGET ( window ) , 0x3FFFFE ); /* ie. GDK_ALL_EVENTS_MASK */

		//gdk_window_set_events( GTK_WIDGET ( window ) , 0x3FFFFE );

		ret = configure ( interp, window, windowOptions );
	}

	gnoclClearOptions ( windowOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( window ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( window ), windowFunc );
}




