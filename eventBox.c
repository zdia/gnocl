/**
 \brief
  This module implements the gnocl::eventBox widget.
 \authors
  Peter G. Baum, William J Giddings
 \date 2001-03:
*/

/* user documentation */

/** \page page16 gnocl::eventBox
  \section sec gnocl::eventBox
  Implementation of gnocl::eventBox
  \subsection subsection1 Implemented gnocl::eventBox Options
  \subsection subsection2 Implemented gnocl::eventBox Commands

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/eventBox.png"
*/

/****h* widget/eventBox
 * NAME
 *  eventBox.c
 * SYNOPSIS
 *  This file implements the eventBox widget.
 * FUNCTION
 * NOTES
 * BUGS
 * EXAMPLE
 * |hmtl <B>PRODUCES</B><P>
 * |html <image src="../pics/eventBox.png">
 * SEE ALSO
 *****/

/*
 * $Id: eventBox.c,v 1.9 2005/02/22 23:16:10 baum Exp $
 *
 * This file implements the eventBox widget
 *
 * Copyright (c) 2002 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-02: added -width, -height
   2008-10: added command, class
   2003-02: added drag and drop options
   2003-01: switched from GnoclWidgetOptions to GnoclOption
   2002-01: Begin of developement
 */
/*
   TODO? handle invisible for user? Or only for gnocl::box?
*/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
\brief
    Set the list of options available to the gnocl::eventBox widget and
    the necessary parameters to respond to the call.
 **/
static GnoclOption boxOptions[] =
{
	{ "-child", GNOCL_OBJ, "", gnoclOptChild },
	{ "-name", GNOCL_STRING, "name" },
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },
	{ "-background", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ "-onButtonPress", GNOCL_OBJ, "P", gnoclOptOnButton },
	{ "-onButtonRelease", GNOCL_OBJ, "R", gnoclOptOnButton },
	{ "-onMotion", GNOCL_OBJ, "", gnoclOptOnMotion },
	{ "-dropTargets", GNOCL_LIST, "t", gnoclOptDnDTargets },
	{ "-dragTargets", GNOCL_LIST, "s", gnoclOptDnDTargets },
	{ "-onDropData", GNOCL_OBJ, "", gnoclOptOnDropData },
	{ "-onDragData", GNOCL_OBJ, "", gnoclOptOnDragData },
	{ "-hasFocus", GNOCL_BOOL, "has-focus" },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },
	{ "-backgroundImage", GNOCL_OBJ, "", gnoclOptBackgroundImage2 },
	{ "-mask", GNOCL_OBJ, "", gnoclOptMask2 },

	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-width", GNOCL_INT, "width-request" },     /* these must be */
	{ "-height", GNOCL_INT, "height-request" },   /* before -visible! */

	{ NULL }
};
/*****/

/****f* eventBox/staticFunc/*get_bitmap2
 * NAME
 *  get_bitmap2
 * PURPOSE
 * AUTHOR
 *  William J Giddings
 * CREATION DATE
 *  13-FEB-09
 * USAGE
 *  Load buffer, obtain alpha channel and return pointer.
 * ARGUMENTS
 * RETURN VALUE
 * NOTE
 * TODO
 *  see notes for window.c - getBitmapMask
 * USES
 * USED BY
 * MODIFICATION HISTORY
 * SOURCE
 */
static GdkBitmap *get_bitmap2 ( gchar *filename )
{
	GdkPixbuf *pbuf2;
	GdkBitmap *ret2;
	GError *err = NULL;

	g_return_val_if_fail ( filename != NULL, NULL );

	pbuf2 = gdk_pixbuf_new_from_file ( filename, &err );

	if ( err != NULL )
	{
		g_warning ( "%s", err->message );
		g_error_free ( err );
		return NULL;
	}

	/* you may want to change the threshold, depending on your image */
	gdk_pixbuf_render_pixmap_and_mask ( pbuf2, NULL, &ret2, 0 );

	g_object_unref ( pbuf2 );

	return ret2;
}

/*****/


/****f* function/name
 * NAME
 *  fuctionName
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
int gnoclOptMask2 (
	Tcl_Interp  *interp,
	GnoclOption *opt,
	GObject     *obj,
	Tcl_Obj     **ret )
{

	printf ( "Eventbox mask\n" );
	GdkBitmap *mask;

	mask = get_bitmap2 ( Tcl_GetString ( opt->val.obj ) );

	gtk_widget_shape_combine_mask ( obj, mask, 0, 0 );

	return TCL_OK;
}

/*****/

/****f* function/name
 * NAME
 *  fuctionName
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
int gnoclOptBackgroundImage2 (
	Tcl_Interp  *interp,
	GnoclOption *opt,
	GObject     *obj,
	Tcl_Obj     **ret )
{

	GdkPixmap *background2;
	GdkBitmap *mask2;
	GdkPixbuf *pixbuf2;

	GtkStyle *style2;

	GError *error = NULL;


	mask2 = get_bitmap2 ( Tcl_GetString ( opt->val.obj ) );

	pixbuf2 = gdk_pixbuf_new_from_file ( Tcl_GetString ( opt->val.obj ), &error );

	//gdk_pixbuf_render_pixmap_and_mask ( pixbuf2, &background2, NULL, 0 );
	gdk_pixbuf_render_pixmap_and_mask ( pixbuf2, &background2, &mask2, 255 );

	style2 =  gtk_style_copy  ( GTK_WIDGET ( obj )->style );

	style2->bg_pixmap[0] = background2;

	// gtk_widget_realize ( GTK_WIDGET ( obj ) );
	gtk_widget_set_style ( GTK_WIDGET ( obj ), GTK_STYLE ( style2 ) );

	return TCL_OK;

}

/*****/

/****f* eventBox/staticFunc/eventBoxFunc
 * NAME
 *  fuctionName
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
int eventBoxFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "delete", "configure", "cget", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, ClassIdx };
	GtkEventBox *box = GTK_EVENT_BOX ( data );
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
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "eventBox", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( box ), objc, objv );

		case ConfigureIdx:
			{
				int ret = gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
													boxOptions, G_OBJECT ( box ) );
				gnoclClearOptions ( boxOptions );
				return ret;
			}

			break;
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( box ),
									 boxOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						assert ( 0 );
				}

				assert ( 0 );
			}
	}

	return TCL_OK;
}

/*****/

/****f* eventBox/gnoclEventBoxCmd
 * NAME
 *  fuctionName
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
int gnoclEventBoxCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	int         ret;
	GtkEventBox *box;

	if ( gnoclParseOptions ( interp, objc, objv, boxOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( boxOptions );
		return TCL_ERROR;
	}

	box = GTK_EVENT_BOX ( gtk_event_box_new( ) );

	gtk_widget_show ( GTK_WIDGET ( box ) );

	ret = gnoclSetOptions ( interp, boxOptions, G_OBJECT ( box ), -1 );
	gnoclClearOptions ( boxOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( box ) );
		return TCL_ERROR;
	}

	return gnoclRegisterWidget ( interp, GTK_WIDGET ( box ), eventBoxFunc );
}

/*****/
