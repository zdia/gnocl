/**
 \brief         This module implements the gnocl::ruler widget.
 \authors       William J Giddings
 \date          20/05/09
*/

/** \page page3 gnocl::arrowButton
  \section sec DESCRIPTION
  Implementation of...
  \subsection subsection1 Implemented XXX gnocl::arrowButton
  \subsection subsection2 Implemented XXX gnocl::arrowButton

\subsection subsection3 Sample Tcl Script
 \code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
 \endcode
\subsection subsection4 Produces
 \image html "../pics/arrowButton.png"
*/


#include "gnocl.h"
#include <string.h>
#include <assert.h>

/**
\brief
\author
\date
**/
static int gnoclOptMetric ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	const char *txt[] = { "pixels", "inches", "centimeters", NULL };
	const int types[] = {  GTK_PIXELS, GTK_INCHES, GTK_CENTIMETERS };

	assert ( sizeof ( GTK_PIXELS ) == sizeof ( int ) );

	return gnoclOptGeneric ( interp, opt, obj, "metric", txt, types, ret );
}

/**
\brief
\author
\date
**/
static int gnoclOptRange ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{

	char *str = Tcl_GetString ( opt->val.str );
	GtkWidget *ruler = GTK_WIDGET ( obj );
	gdouble lower, upper, position, max_size;

	/* why isn't this parsing the string correctly? */
	sscanf ( str, "%f %f %f %f", &lower, &upper, &position, &max_size );

	g_print ( "%s %f %f %f %f\n", str, lower, upper, position, max_size );

	/* Paramters for the set range library call
	lower :     the lower limit of the ruler
	upper :     the upper limit of the ruler
	position :  the mark on the ruler
	max_size :  the maximum size of the ruler used when calculating the space to leave for the text
	*/

	gtk_ruler_set_range ( GTK_RULER ( ruler ), lower, upper, position, max_size );


	return TCL_OK;
}


/*
  "lower"                    gdouble               : Read / Write
  "max-size"                 gdouble               : Read / Write
  "metric"                   GtkMetricType         : Read / Write
  "position"                 gdouble               : Read / Write
  "upper"                    gdouble               : Read / Write
*/
static GnoclOption rulerOptions[] =
{
	/* options based upon widget properties */
	{ "-orientation", GNOCL_OBJ, NULL },    /* 0 */
	{ "-lower", GNOCL_DOUBLE, "arrow"},
	{ "-maxSize", GNOCL_DOUBLE, "shadow"},
	{ "-metric", GNOCL_OBJ, "metric", gnoclOptMetric},
	{ "-position", GNOCL_DOUBLE, "position"},
	{ "-upper", GNOCL_DOUBLE, "upper"},

	/* custom options */
	{ "-range", GNOCL_OBJ, "", gnoclOptRange},


	{ NULL },
};


static const int orientationIdx  = 0;
static const int buttonTypeIdx   = 1;
static const int childrenIdx     = 2;
static const int layoutIdx       = 3;

static const int startFrameOpts  = 7;
static const int startCommonOpts = 11;

static const int startPackOpts   = 24;
static const int paddingDiff     = 0;
static const int fillDiff        = 1;
static const int expandDiff      = 2;
static const int alignDiff       = 3;


/****f* widget/configure
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int configure ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[] )
{

	const char *dataIDa = "gnocl::data1";
	const char *dataIDb = "gnocl::data2";

	GtkWidget *arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_BUTTON );
	char *str;

	/* this will enable us to obtain widget data for the arrow object */
	char *result = g_object_get_data ( G_OBJECT ( arrow ), dataIDa );
	printf ( "cget result = %s\n", result );

	result = g_object_get_data ( G_OBJECT ( arrow ), dataIDb );
	printf ( "cget result = %s\n", result );

	str = gnoclGetNameFromWidget ( arrow );
	printf ( "configure %s\n", str );
	return TCL_OK;
}

/*****/

/****f* widget/gnoclConfigArrowButtonText
 * AUTHOR
 *  PGB
 * SOURCE
 */
int gnoclConfigRuler ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj )
{
	printf ( "gnoclConfigArrowButton\n" );

	GtkArrow *arrow;

	/* the arrow is the child of the widget */

	arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_ARROW ) ;
	gtk_arrow_set ( arrow, GTK_ARROW_DOWN, GTK_SHADOW_OUT );

	return TCL_OK;
}

/*****/

/****f* widget/cget
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int cget ( Tcl_Interp *interp, GtkWidget *widget, GnoclOption options[], int idx )
{
	printf ( "cget\n" );

	const char *dataIDa = "gnocl::data1";
	const char *dataIDb = "gnocl::data2";

	GtkWidget *arrow = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_BUTTON );
	char *str;

	/* this will enable us to obtain widget data for the arrow object */
	char *result = g_object_get_data ( G_OBJECT ( arrow ), dataIDa );
	printf ( "cget result = %s\n", result );

	result = g_object_get_data ( G_OBJECT ( arrow ), dataIDb );
	printf ( "cget result = %s\n", result );

	str = gnoclGetNameFromWidget ( arrow );
	printf ( "configure %s\n", str );


	/*-----*/
	/*
	    Tcl_Obj *obj = NULL;

	    if ( idx == textIdx )
	    {
	        obj = gnoclCgetButtonText ( interp, widget );
	    }

	    else if ( idx == iconsIdx )
	    {
	        GtkWidget *image = gnoclFindChild ( GTK_WIDGET ( widget ), GTK_TYPE_IMAGE );

	        if ( image == NULL )
	            obj = Tcl_NewStringObj ( "", 0 );
	        else
	        {
	            gchar   *st;
	            g_object_get ( G_OBJECT ( image ), "stock", &st, NULL );

	            if ( st )
	            {
	                obj = Tcl_NewStringObj ( "%#", 2 );
	                Tcl_AppendObjToObj ( obj, gnoclGtkToStockName ( st ) );
	                g_free ( st );
	            }

	            else
	            {
	                Tcl_SetResult ( interp, "Could not determine icon type.",
	                                TCL_STATIC );
	                return TCL_ERROR;
	            }
	        }
	    }

	    if ( obj != NULL )
	    {
	        Tcl_SetObjResult ( interp, obj );
	        return TCL_OK;
	    }
	*/


	return gnoclCgetNotImplemented ( interp, options + idx );
}

/*****/

/****f* widget/widgetFunc
 * AUTHOR
 *  PGB
 * SOURCE
 */
static int widgetFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
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
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "arrowButton", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				ret = configure ( interp, widget, rulerOptions );

				if ( 1 )
				{
					if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, rulerOptions, G_OBJECT ( widget ) ) == TCL_OK )
					{
						ret = configure ( interp, widget, rulerOptions );
					}
				}

				gnoclClearOptions ( rulerOptions );

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

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), rulerOptions, &idx ) )
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
							return cget ( interp, widget, rulerOptions, idx );
						}
				}
			}
	}

	return TCL_OK;
}

/*****/

/****f* widget/gnoclArrowButtonCmd
 * AUTHOR
 *  PGB
 * SOURCE
 * PARAMETERS
 *  Format specified in Tcl specs
 *      ClientData data,
 *      Tcl_Interp *interp,
 *      int objc,                   total number of items in the array objv[]
 *      Tcl_Obj * const objv[]      array containing an array of arguments passed to the Tcl command
 * NOTES
 * int gnoclOptData (
 *  Tcl_Interp *interp,         -yes we have this from arg 2
 *  GnoclOption *opt,
 *  GObject *obj,
 *  Tcl_Obj **ret )
 *
 *  Shadow Types
 *  GTK_SHADOW_NONE, GTK_SHADOW_IN, GTK_SHADOW_OUT, GTK_SHADOW_ETCHED_IN, GTK_SHADOW_ETCHED_OUT
 */
int gnoclRulerCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	int            ret = TCL_OK;
	GtkWidget      *ruler;

	if ( gnoclParseOptions ( interp, objc, objv, rulerOptions ) != TCL_OK )
	{
		gnoclClearOptions ( rulerOptions );
		return TCL_ERROR;
	}

	if ( rulerOptions[orientationIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGetOrientationType ( interp, rulerOptions[orientationIdx].val.obj, &orient ) != TCL_OK )
		{
			gnoclClearOptions ( rulerOptions );
			return TCL_ERROR;
		}
	}




	/* create the required type of ruler, it cannot be re-configured */

	if ( orient == GTK_ORIENTATION_HORIZONTAL )
	{
		g_printf ( "horizontal\n" );
		ruler = GTK_WIDGET ( gtk_hruler_new() );
		/* set some default values */

		if ( 1 )
		{
			gtk_ruler_set_metric ( GTK_RULER ( ruler ), GTK_PIXELS );
			gtk_ruler_set_range ( GTK_RULER ( ruler ), 0, 100, 0, 20 );
		}
	}

	else
	{
		g_printf ( "vertical\n" );
		ruler = GTK_WIDGET ( gtk_vruler_new() );

		if ( 1 )
		{
			gtk_ruler_set_metric ( GTK_RULER ( ruler ), GTK_PIXELS );
			gtk_ruler_set_range ( GTK_RULER ( ruler ), 0, 100, 0, 20 );
		}
	}

	if ( gnoclParseOptions ( interp, objc, objv, rulerOptions ) != TCL_OK )
	{
		gnoclClearOptions ( rulerOptions );
		return TCL_ERROR;
	}

	gtk_widget_show ( GTK_WIDGET ( ruler ) );


	ret = gnoclSetOptions ( interp, rulerOptions, G_OBJECT ( ruler ), -1 );

	if ( ret == TCL_OK )
	{
		ret = configure ( interp, G_OBJECT ( ruler ), rulerOptions );
	}

	gnoclClearOptions ( rulerOptions );

	/* STEP 3)  -show the widget */

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( ruler ) );
		return TCL_ERROR;
	}

	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( ruler ), widgetFunc );
}

/*****/
