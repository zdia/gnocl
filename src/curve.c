/**
 \brief         This module implements the gnocl::curve widget.
 \authors       William J Giddings
 \date          20/05/09
**/

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


int RangeIdx = 0;
int CurveTypeIdx = 1;

/*
  "lower"                    gdouble               : Read / Write
  "max-size"                 gdouble               : Read / Write
  "metric"                   GtkMetricType         : Read / Write
  "position"                 gdouble               : Read / Write
  "upper"                    gdouble               : Read / Write
*/
static GnoclOption curveOptions[] =
{
	/* custom commands */
	{ "-range", GNOCL_OBJ, NULL },    /* 0 */
	{ "-curvetype", GNOCL_OBJ, NULL }, /* 1 */

	/* handle events */
	{ "-onChanged", GNOCL_OBJ, "", gnoclOptOnShowHelp},
	{ NULL },
};

/**
\brief
\author     William J Giddings
\date       12-Feb-09
\since      0.9.94
**/
static int configure ( Tcl_Interp *interp, GtkCurve *widget, GnoclOption options[] )
{
	g_print ( "\tconfigure\n" );


	if ( options[RangeIdx].status == GNOCL_STATUS_CHANGED )
	{

		//char *str = Tcl_GetString ( options[RangeIdx].val.obj  );

		gfloat min_x;
		gfloat max_x;
		gfloat min_y;
		gfloat max_y;

		sprintf ( Tcl_GetString ( options[RangeIdx].val.obj ), "%f %f %f %f", &min_x, &max_x, &min_y, &max_y );

		g_print ( "\tRange %f %f %f %f\n", min_x, max_x, min_y, max_y );

		gtk_curve_set_range ( widget, min_x, max_x, min_y, max_y );
	}


	if ( options[CurveTypeIdx].status == GNOCL_STATUS_CHANGED )
	{

		char *str = Tcl_GetString ( options[CurveTypeIdx].val.obj  );

		/* linear interpolation */
		if ( !strcmp ( str, "linear" ) )
		{
			printf ( "1 gnoclOptCurvetype = %s\n", str );
			gtk_curve_set_curve_type ( GTK_CURVE ( widget ), GTK_CURVE_TYPE_LINEAR );

		}

		/* spline interpolation */
		if ( !strcmp ( str, "spline" ) )
		{

			printf ( "2 gnoclOptCurvetype = %s\n", str );
			gtk_curve_set_curve_type ( GTK_CURVE ( widget ), GTK_CURVE_TYPE_SPLINE );

		}

		/* free form curve */
		if ( !strcmp ( str, "free" ) )
		{
			printf ( "3 gnoclOptCurvetype = %s\n", str );
			gtk_curve_set_curve_type ( GTK_CURVE ( widget ) , GTK_CURVE_TYPE_FREE );

		}

	}

	return TCL_OK;
}


/**
\brief
\author     William J Giddings
\date       12-Feb-09
\since      0.9.94
**/
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

/**
\brief
\author     William J Giddings
\date       12-Feb-09
\since      0.9.94
**/
static int curveFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

#ifdef DEBUG_CURVE
	g_printf ( "curveFunc\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	static const char *cmds[] = { "delete", "configure", "cget", "onClicked", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, CgetIdx, OnClickedIdx, ClassIdx };

	GtkCurve *widget = GTK_WIDGET ( data );

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
			printf ( "Class\n" );
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "curve", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, GTK_WIDGET ( widget ), objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;
				printf ( "Configure\n" );

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, curveOptions, G_OBJECT ( widget ) ) == TCL_OK )
				{

					printf ( "Configure\n" );
					ret = configure ( interp, widget, curveOptions );
				}

				gnoclClearOptions ( curveOptions );

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

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( widget ), curveOptions, &idx ) )
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
							return cget ( interp, widget, curveOptions, idx );
						}
				}
			}
	}

	return TCL_OK;
}

/**
\brief
\author     William J Giddings
\date       12-Feb-09
\since      0.9.94
**/
int gnoclCurveCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

	int            ret = TCL_OK;
	GtkWidget      *curve;

	if ( 1 )
	{
		g_print ( "1\n" );

		if ( gnoclParseOptions ( interp, objc, objv, curveOptions ) != TCL_OK )
		{
			gnoclClearOptions ( curveOptions );
			return TCL_ERROR;
		}
	}

	curve = gtk_curve_new() ;

	gtk_widget_show ( GTK_WIDGET ( curve ) );


	if ( 1 )
	{
		g_print ( "2\n" );
		ret = gnoclSetOptions ( interp, curveOptions, G_OBJECT ( curve ), -1 );
		g_print ( "\t2a\n" );

		if ( ret == TCL_OK )
		{
			ret = configure ( interp, G_OBJECT ( curve ), curveOptions );
		}

		g_print ( "\t2b\n" );
	}

	g_print ( "3\n" );
	gnoclClearOptions ( curveOptions );

	/* STEP 3)  -show the widget */

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( curve ) );
		return TCL_ERROR;
	}


	g_print ( "4\n" );
	/* STEP 4)  -everything has worked, register the widget with the Tcl interpretor. */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( curve ), curveFunc );
}

