/**
 \brief         This module implements the gnocl::colorWheel widget.
 \authors       William J Giddings
 \date          20/05/09
*/

/** \page page3 gnocl::colorWheel
  \section sec DESCRIPTION
  Implementation of...
  \subsection subsection1 Implemented XXX gnocl::colorWheel
  \subsection subsection2 Implemented XXX gnocl::colorWheel

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/colorWheel.png"
*/


#include "gnocl.h"
#include <string.h>
#include <assert.h>


/**
\brief      Callback function to focus change event.
\author     William J Giddings
\date       15/10/08
**/
static gboolean doOnColorWheelChanged ( GtkHSV  *hsv, gpointer data )
{

	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	gdouble h, s, v;
	gdouble r , g, b;
	gint R, G, B;
	gchar hex[16];

	/* define string substitution array */
	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget  */
		{ 'h', GNOCL_DOUBLE },  /* hue 0-1 */
		{ 's', GNOCL_DOUBLE },  /* saturation 0-1*/
		{ 'v', GNOCL_DOUBLE },  /* vaue 0-1  */
		{ 'r', GNOCL_DOUBLE },  /* red 0-1   */
		{ 'g', GNOCL_DOUBLE },  /* green 0-1 */
		{ 'b', GNOCL_DOUBLE },  /* blue 0-1  */
		{ 'R', GNOCL_INT },     /* red 0-255   */
		{ 'G', GNOCL_INT },     /* green 0-255 */
		{ 'B', GNOCL_INT },     /* blue 0-255 */
		{ 'x', GNOCL_STRING },  /* hexadecimal string, lower case */
		{ 0 }
	};


	/* gather and convert required data */
	gtk_hsv_get_color ( hsv, &h, &s, &v );
	gtk_hsv_to_rgb ( h, s, v, &r, &g, &b );

	R = 255 * r; G = 255 * g; B = 255 * b;

	/* this not ideal, black would be #000 and not #000000
	   check to see what other colour conversion funcs are in the
	   gnocl sourcecode
	*/
	sprintf ( hex, "#%2x%2x%2x", R, G, B );

	/* assign values for string substitution */
	ps[0].val.str = gnoclGetNameFromWidget ( hsv );

	ps[1].val.d = h;
	ps[2].val.d = s;
	ps[3].val.d = v;

	ps[4].val.d = r;
	ps[5].val.d = g;
	ps[6].val.d = b;

	ps[7].val.i = R;
	ps[8].val.i = G;
	ps[9].val.i = B;
	ps[9].val.i = B;

	ps[10].val.str = g_ascii_strup ( hex, -1 );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
	return 0;
}


/**
\brief      Callback function to focus change event.
\author     William J Giddings
\date       15/10/08
**/
static gboolean doOnColorWheelMove ( GtkHSV *hsv, GtkDirectionType arg1, gpointer data )
{
	GnoclCommandData *cs = ( GnoclCommandData * ) data;

	gdouble h, s, v;
	gdouble r , g, b;
	gint R, G, B;
	gint d;
	gchar hex[16];
	gchar *str;

	/* define string substitution array */
	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* widget  */
		{ 'd', GNOCL_STRING },     /* direction */
		{ 0 }
	};


	switch ( arg1 )
	{
		case GTK_DIR_TAB_FORWARD:
			{
				str = "forward";
			} break;
		case GTK_DIR_TAB_BACKWARD:
			{
				str = "backward";
			} break;
		case GTK_DIR_UP:
			{
				str = "up";
			} break;
		case GTK_DIR_DOWN:
			{
				str = "down";
			} break;
		case GTK_DIR_LEFT:
			{
				str = "left";
			}
			break;
		case GTK_DIR_RIGHT:
			{
				str = "right";
			} break;
		default: {}
	}


	/* assign values for string substitution */
	ps[0].val.str = gnoclGetNameFromWidget ( hsv );
	ps[1].val.str = str;

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
	return 0;
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
int gnoclOptOnColorWheelChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	printf ( "gnoclOptOnColorWheelChanged\n" );
	assert ( strcmp ( opt->optName, "-onChanged" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "changed", G_CALLBACK ( doOnColorWheelChanged ), opt, NULL, ret );
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
int gnoclOptOnColorWheelMove ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	printf ( "gnoclOptOnColorWheelMove\n" );
	assert ( strcmp ( opt->optName, "-onMove" ) == 0 );
	return gnoclConnectOptCmd ( interp, obj, "move", G_CALLBACK ( doOnColorWheelMove ), opt, NULL, ret );
}


/**
\brief
\author
\date
\note
**/
int gnoclOptColorWheelMetrics ( Tcl_Interp *interp, GnoclOption *opt,   GObject *obj, Tcl_Obj **ret )
{

	printf ( "gnoclOptColorWheelMetrics\n" );

	switch ( opt->optName[1] )
	{
		case 'S':
			{
				g_printf ( "Set ColorWheel Size\n" );
			} break;
		case 'R':
			{
				g_printf ( "Set ColorWheel RingWidth\n" );
			}      break;
		default: {};
	}


	return TCL_OK;
}

static const int sizeIdx = 0;
static const int ringWidthIdx = 1;
static const int colorRGBIdx = 2;
static const int colorHSVIdx = 2;

static GnoclOption colorWheelOptions[] =
{
	/* configure directly, widget has no "propertied" */
	{ "-size", GNOCL_OBJ, NULL },         /* 0 */
	{ "-ringWidth", GNOCL_OBJ, NULL},     /* 1 */
	{ "-colorRGB", GNOCL_OBJ, NULL},      /* 2 */
	{ "-colorHSV", GNOCL_OBJ, NULL},      /* 3 */

	/* handle widget signals */
	{ "-onChanged", GNOCL_OBJ, "", gnoclOptOnColorWheelChanged },
	{ "-onMove", GNOCL_OBJ, "", gnoclOptOnColorWheelMove },

	/* add object properties */
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },

	{ NULL },
};

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
static int configure ( Tcl_Interp *interp, GtkWidget *hsv, GnoclOption options[] )
{

	printf ( "CONFIGURE\n" );

	gint size;
	gint ring_width;
	gchar *clr;

	/* get some initial values */
	gtk_hsv_get_metrics ( hsv, &size, &ring_width );

	/* reconfigure */
	if ( options[sizeIdx].status == GNOCL_STATUS_CHANGED )
	{

		Tcl_GetIntFromObj ( NULL, options[sizeIdx].val.obj, &size );
		/* reset hsv metrics */
		gtk_hsv_set_metrics ( hsv, size, ring_width );
		printf ( "\t -size value changed = %d\n", size );
	}

	if ( options[ringWidthIdx].status == GNOCL_STATUS_CHANGED )
	{

		Tcl_GetIntFromObj ( NULL, options[ringWidthIdx].val.obj, &ring_width );
		/* reset hsv metrics */
		gtk_hsv_set_metrics ( hsv, size, ring_width );

		printf ( "\t -ringWidth value changed = %d\n", ring_width ) ;
	}

	if ( options[colorRGBIdx].status == GNOCL_STATUS_CHANGED )
	{
		/* conversion from RGB to HSV needed */
		double h, s, v;

		clr = Tcl_GetString ( options[colorRGBIdx].val.obj );
		/* reset hsv metrics */
		//gtk_hsv_set_metrics ( hsv, size, ring_width );
		gtk_hsv_set_color ( hsv, h, s, v );
		printf ( "\t -colorRGB value changed = %s\n", clr ) ;
	}

	if ( options[colorHSVIdx].status == GNOCL_STATUS_CHANGED )
	{
		double h, s, v;
		Tcl_GetIntFromObj ( NULL, options[colorHSVIdx].val.obj, &ring_width );
		/* reset hsv metrics */
		gtk_hsv_set_color ( hsv, h, s, v );
		printf ( "\t -colorHSV value changed = %d\n", ring_width ) ;
	}


	return TCL_OK;
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
int gnoclConfigColorWheel ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj )
{
	printf ( "gnoclConfigcolorWheel\n" );

	GtkArrow *arrow;

	/* the arrow is the child of the widget */

	arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_ARROW ) ;
	gtk_arrow_set ( arrow, GTK_ARROW_DOWN, GTK_SHADOW_OUT );

	return TCL_OK;
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
static int cget ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[], int idx )
{
	printf ( "cget\n" );

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
static int widgetFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

#ifdef DEBUG_COLORWHEEL
	g_printf ( "widgetFunc\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	printf ( "widgetFunc\n" );

	static const char *cmds[] = { "delete", "configure", "cget", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx };
	GtkWidget *widget = GTK_WIDGET ( data );
	int idx;

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
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "colorWheel", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				ret = configure ( interp, widget, colorWheelOptions );

				if ( 1 )
				{
					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, colorWheelOptions, G_OBJECT ( widget ) ) == TCL_OK )
					{
						ret = configure ( interp, widget, colorWheelOptions );
					}
				}

				gnoclClearOptions ( colorWheelOptions );

				return ret;
			}

			break;
		case OnClickedIdx:

			if ( objc != 2 )
			{
				Tcl_WrongNumArgs ( interp, 2, objv, NULL );
				return TCL_ERROR;
			}

			if ( GTK_WIDGET_IS_SENSITIVE ( GTK_WIDGET ( widget ) ) )
			{
				gtk_button_clicked ( widget );
			}

			break;

		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), colorWheelOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						{
							return TCL_ERROR;
						}

					case GNOCL_CGET_HANDLED:
						{
							return TCL_OK;
						}

					case GNOCL_CGET_NOTHANDLED:
						{
							return cget ( interp, widget, colorWheelOptions, idx );
						}
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author     William J Giddings
\date       19/03/2010
\since      0.9.94g
**/
int gnoclColorWheelCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int            ret = TCL_OK;
	GtkWidget      *colorWheel;


	if ( gnoclParseOptions ( interp, objc, objv, colorWheelOptions ) != TCL_OK )
	{
		gnoclClearOptions ( colorWheelOptions );
		return TCL_ERROR;
	}


	colorWheel = gtk_hsv_new () ;

	gtk_widget_show ( GTK_WIDGET ( colorWheel ) );


	ret = gnoclSetOptions ( interp, colorWheelOptions, G_OBJECT ( colorWheel ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, G_OBJECT ( colorWheel ), colorWheelOptions );
	}

	gnoclClearOptions ( colorWheelOptions );

	/* STEP 3)  -show the widget */

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( colorWheel ) );
		return TCL_ERROR;
	}

	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( colorWheel ), widgetFunc );
}


