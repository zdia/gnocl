/**
\brief      This module implements the gnocl::drawingArea widget.
\author     Peter G. Baum, William J Giddings
\date       2007-08:
\note
**/

/* user documentation */

/**
\page page14 gnocl::drawingArea
\section sec gnocl::drawingArea
    Implementation of gnocl::drawingArea
\subsection subsection1 Implemented gnocl::drawingArea Options
\subsection subsection2 Implemented gnocl::drawingArea Commands

\subsection subsection3 Sample Tcl Script
\code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
\endcode
\subsection subsection4 Produces
\image html "../pics/drawingArea.png"
**/

#include "gnocl.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <assert.h>

/*
    The Widget implementation has a number of empty entries within its
    options array. This will allow the coder to configure the widget
    under scripted control, thereby providing a Gnocl framework for
    customised widget creation.
*/

static gint _n = 4; /* counter,number of implements widget options */

static GnoclOption drawingAreaOptions[] =
{
	/* common widget options */
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-cursor", GNOCL_OBJ, "", gnoclOptCursor },
	/* widget specific options, 15 blank spaces */
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ "", GNOCL_OBJ, "", NULL },
	{ NULL },
};

/**
\brief
\author     William J Giddings
\date       18/Feb/2010
\since      0.9.94
**/
static int configure ( Tcl_Interp *interp, GtkDrawingArea *area, GnoclOption options[] )
{

	gint ret = gnoclSetOptions ( interp, options, area, -1 );

	return TCL_OK;
}

static const char *options[] =
{
	"-onExpose",        "-onMotion",        "motionHint",
	"-onButtonMotion",  "-onButton1Motion", "-onButton2Motion",
	"-onButton3Motion", "-onButtonPress",   "-onButtonRelease",
	"-onKeyPress",      "-onKeyRelease",    "-onEnter",
	"-onLeave",         "-onFocus",         "structure",
	"propertyChange",   "visibility",       "proximityIn",
	"proximityOut",     "substructure",     "-onScroll",
	"all", NULL
};

enum OptsIdx
{
	OnExposeIdx,        OnMotionIdx,        MotionHintIdx,
	OnButtonMotionIdx,  OnButton1MotionIdx, OnButton2MotionIdx,
	OnButton3MotionIdx, OnButtonPressIdx,   OnButtonReleaseIdx,
	OnKeyPressIdx,      OnKeyReleaseIdx,    OnEnterIdx,
	OnLeaveIdx,         OnFocusIdx,         StructureIdx,
	PropertyChangeIdx,  VisibilityIdx,      ProximityInIdx,
	ProximityOutIdx,    SubstructureIdx,    OnScrollIdx,
	AllIdx
};


/**
/brief
/author William J Giddings
/date   2008-08
**/
int drawingAreaFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_DRAWING_AREA
	g_printf ( "drawingAreaFunc\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif

	static const char *cmds[] =
	{
		"draw", "cget", "configure", "delete", "class", "erase", "option", NULL
	};
	enum cmdIdx
	{
		DrawIdx, CgetIdx, ConfigureIdx, DeleteIdx, ClassIdx, EraseIdx, OptionIdx
	};


	/* relocate these to relevant parts of the souce file */
	static const char *drawOpts[] =
	{
		"point", "points", "line", "lines", "pixbuf", "segments",
		"rectangle", "arc", "polygon", "trapezoids", "glyph",
		"glyphTransformed", "layoutLine", "layoutWithColors",
		"string", "text", "image",
		NULL
	};

	enum drawOptsIdx
	{
		PointIdx, PointsIdx, LineIdx, LinesIdx, PixbufIdx, SegmentsIdx,
		RectangleIdx, ArcIdx, PolygonIdx, TrapezoidsIdx, GlypIdx,
		GlyphTransformedIdx, LayoutLineIdx, LayoutWithColorsIdx,
		StringIdx, TextIdx, ImageIdx
	};



	/*
	static const char *events[] =
	{
	    "expose",           "motion",           "motionHint",
	    "buttonMotion",     "button1Motion",    "button2Motion",
	    "button3Motion",    "buttonPress",      "buttonRelease",
	    "keyPress",         "keyRelease",       "enter",
	    "leave",            "focus",            "structure",
	    "propertyChange",   "visibility",       "proximityIn",
	    "proximityOut",     "substructure",     "scroll",
	    "all", NULL
	};
	*/

	GtkWidget *area = GTK_WIDGET ( data );

	int idx;
	int idx2;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{

		case OptionIdx:
			{
#ifdef DEBUG_DRAWING_AREA
				g_print ( "drawingArea OptionIdx\n" );
#endif

				/*
				other options here could include:
				    remove      -remove the option from the options array (incl. script)
				    available   -return a list of currently configured options
				    suspend     -remove option from the event mask only
				    resume      -add the option to the event mask
				*/

				if ( !strcmp ( Tcl_GetString ( objv[2] ), "add" )  )
				{
					g_printf ( " add\n" );
				}

				else
				{
					return TCL_ERROR;
				}

				/* create a hash table for events and handlers */

				int OptIdx;

				if ( Tcl_GetIndexFromObj ( interp, objv[3], options, "option", TCL_EXACT, &OptIdx ) != TCL_OK )
				{
					return TCL_ERROR;
				}

				_n = doOptionAdd ( interp, area, OptIdx );

				break;
			case ConfigureIdx:
				{
#ifdef DEBUG_DRAWING_AREA
					g_print ( "drawingArea ConfigureIdx\n" );
#endif
					int ret = TCL_ERROR;

					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
												   drawingAreaOptions, area ) == TCL_OK )
					{
						ret = configure ( interp, area, drawingAreaOptions );
					}

					gnoclClearOptions ( drawingAreaOptions );

					return ret;
				}

				break;
			case DeleteIdx:
				{
					g_print ( "delete\n" );
				}

				break;
			case ClassIdx:
				{
					g_print ( "Class = drawingArea\n" );
					Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "drawingArea", -1 ) );
				}

				break;
				/*
				case PointIdx
				case PointsIdx
				case LineIdx
				case LinesIdx
				case PixbufIdx
				case SegmentsIdx
				case RectangleIdx
				case ArcIdx
				case PolygonIdx
				case TrapezoidsIdx
				case GlypIdx
				case GlyphTransformedIdx
				case LayoutLineIdx
				case LayoutWithColorsIdx,
				case StringIdx
				case TextIdx
				case ImageIdx
				*/
			}
	}

	return TCL_OK;
}

/**
/brief
/author William J Giddings
/date   2008-08
**/
int gnoclDrawingAreaCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	int       ret;
	GtkWidget *widget;

	if ( gnoclParseOptions ( interp, objc, objv, drawingAreaOptions ) != TCL_OK )
	{
		gnoclClearOptions ( drawingAreaOptions );
		return TCL_ERROR;
	}

	widget = gtk_drawing_area_new ();

	gtk_widget_show ( GTK_WIDGET ( widget ) );

	gnoclSetOptions ( interp, drawingAreaOptions, G_OBJECT ( widget ), -1 );

	gnoclClearOptions ( drawingAreaOptions );


	/* register the new widget for use with the Tcl interpretor */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( widget ), drawingAreaFunc );

}

/**
\brief      Add new settings to the widget option array.
\author     William J Giddings
\date       19/Feb/2010
\since      0.9.94
\see        http://library.gnome.org/devel/gdk/stable/gdk-Events.html
\note       Event Mask Options:

      GDK_EXPOSURE_MASK         GDK_POINTER_MOTION_MASK     GDK_POINTER_MOTION_HINT_MASK
      GDK_BUTTON_MOTION_MASK    GDK_BUTTON1_MOTION_MASK     GDK_BUTTON2_MOTION_MASK
      GDK_BUTTON3_MOTION_MASK   GDK_BUTTON_PRESS_MASK       GDK_BUTTON_RELEASE_MASK
      GDK_KEY_PRESS_MASK        GDK_KEY_RELEASE_MASK        GDK_ENTER_NOTIFY_MASK
      GDK_LEAVE_NOTIFY_MASK     GDK_FOCUS_CHANGE_MASK       GDK_STRUCTURE_MASK
      GDK_PROPERTY_CHANGE_MASK  GDK_VISIBILITY_NOTIFY_MASK  GDK_PROXIMITY_IN_MASK
      GDK_PROXIMITY_OUT_MASK    GDK_SUBSTRUCTURE_MASK       GDK_SCROLL_MASK
      GDK_ALL_EVENTS_MASK

**/

int doOptionAdd ( Tcl_Interp *interp, GtkWidget *area, gint OptIdx )
{
#ifdef DEBUG_DRAWING_AREA
	g_print ( "drawingArea OptionIdx\n" );
#endif

	/* return error the maximum number of options has been reached */
	gint j = sizeof ( drawingAreaOptions ) / sizeof ( GnoclOption );

	if ( j == _n )
	{
		return TCL_ERROR;
	}

	/* set the option name directly from list of possible options */
	drawingAreaOptions[_n].optName =  options[OptIdx];

	/* set the remaining values, assign callback function */
	switch ( OptIdx )
	{

		case OnButtonMotionIdx:
			{
				gtk_widget_add_events ( area, GDK_BUTTON_MOTION_MASK );
				drawingAreaOptions[_n].func = gnoclOptOnButtonMotion;
			}
			break;

		case OnMotionIdx:
			{
				gtk_widget_add_events ( area, GDK_BUTTON_MOTION_MASK );
				drawingAreaOptions[_n].func = gnoclOptOnMotion;
			}
			break;

		case OnEnterIdx:
			{
				gtk_widget_add_events ( area, GDK_ENTER_NOTIFY_MASK );
				drawingAreaOptions[_n].propName = "E";
				drawingAreaOptions[_n].func = gnoclOptOnEnterLeave;
			}
		case OnLeaveIdx:
			{
				gtk_widget_add_events ( area, GDK_LEAVE_NOTIFY_MASK );
				drawingAreaOptions[_n].propName = "L";
				drawingAreaOptions[_n].func = gnoclOptOnEnterLeave;
			}
			break;
		case OnButtonPressIdx:
			{
				gtk_widget_add_events ( area, GDK_BUTTON_PRESS_MASK ) ;
				drawingAreaOptions[_n].propName = "P";
				drawingAreaOptions[_n].func = gnoclOptOnButton;
			}
			break;
		case OnButtonReleaseIdx:
			{
				gtk_widget_add_events ( area, GDK_BUTTON_RELEASE_MASK ) ;
				drawingAreaOptions[_n].propName = "R";
				drawingAreaOptions[_n].func = gnoclOptOnButton;
			}
			break;
	}

	/* implement changes */
	gnoclSetOptions ( interp, drawingAreaOptions, area, -1 );

	return _n + 1;
}

