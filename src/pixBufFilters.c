/** filters.c
\brief
\author
\date
\since
\notes
\todo
        #define a clamp macro
**/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static guint32 convertRGBtoPixel ( gchar *clr );

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
static void clamp_8bit ( gint *r, gint *g, gint *b, gint *a )
{

	if ( *r > 255 ) *r = 255; if ( *r < 0 ) *r = 0;

	if ( *g > 255 ) *g = 255; if ( *g < 0 ) *g = 0;

	if ( *b > 255 ) *b = 255; if ( *b < 0 ) *b = 0;

	if ( *a > 255 ) *a = 255; if ( *a < 0 ) *a = 0;
}

/**
\brief      Template for the creation of new filters.
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_template ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gint dr, gint dg, gint db )
{

	gint j, k;
	gint p = 0;
	gint r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/* put action here ! */
			r += dr; g += dg; b += db;

			clamp_8bit ( &r, &g, &b, &a );

			/* action done */

			sprintf ( col, "#%.2x%.2x%.2x", r, g, b );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
static void clamp_16bit ( gint *r, gint *g, gint *b, gint *a )
{

	if ( *r > 65535 ) *r = 65535; if ( *r < 0 ) *r = 0;

	if ( *g > 65535 ) *g = 65535; if ( *g < 0 ) *g = 0;

	if ( *b > 65535 ) *b = 65535; if ( *b < 0 ) *b = 0;

	if ( *a > 65535 ) *a = 65535; if ( *a < 0 ) *a = 0;
}


/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_color ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gint dr, gint dg, gint db )
{

	gint j, k;
	gint p = 0;
	gint r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/* put action here ! */
			r += dr; g += dg; b += db;

			clamp_8bit ( &r, &g, &b, &a );

			/* action done */

			sprintf ( col, "#%.2x%.2x%.2x", r, g, b );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}


/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_gamma ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gfloat gamma )
{
	gint j, k;
	gint p = 0;
	gint r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	/* create lookup tables */
	gfloat redGamma[255];
	gfloat greenGamma[255];
	gfloat blueGamma[255];

	gfloat red, green, blue;

	red = gamma;
	green = gamma;
	blue = gamma;

	gfloat tmp;

	gchar col[32];

	int i;

	/* fill the table with values */
	for ( i = 0; i < 256; ++i )
	{
		tmp = 255.0 * pow ( i / 255.0, 1.0 / red ) + 0.5;
		redGamma[i] = ( tmp < 255 ) ? tmp : 255;

		tmp = 255.0 * pow ( i / 255.0, 1.0 / green ) + 0.5;
		greenGamma[i] = ( tmp < 255 ) ? tmp : 255;

		tmp = 255.0 * pow ( i / 255.0, 1.0 / blue ) + 0.5;
		blueGamma[i] = ( tmp < 255 ) ? tmp : 255;

	}

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			g_print ( "\t\tgamma r = %f g = %f b = %f \n", redGamma[r], greenGamma[g], blueGamma[b] );

			/*------------ action done -------------*/

			sprintf ( col, "#%.2x%.2x%.2x", redGamma[r], greenGamma[g], blueGamma[b] );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_brightness_range ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gint *min, gint *max )
{

	gint j, k;
	gint p = 0;
	guchar r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	guchar gray, bmin, bmax;

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/*---------- put action here! ----------*/
			gray = ( 0.299 * r ) + ( 0.587 * g ) + ( 0.114 * b );

			if ( gray >= bmax )
			{
				bmax = gray;
			}

			if ( gray <= bmin )
			{
				bmin = gray;
			}

			/*------------ action done -------------*/
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	*min = bmin;
	*max = bmax;
	return p;
}

/**
\brief      Adjust contrast of image based upon grayscale sample midpoint.
\author     William J Giddings
\date       20/03/2010
\note       The aim is to produce contrast chage by adjusting brightness
            rather than colour, ideally to eliminsate excessive false colour.
**/
int filter_contrast ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gfloat contrast )
{
	gint j, k;
	gint p = 0;
	gint r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	/* determine brightness range of the sample */

	gint min, max, mid;
	filter_brightness_range ( pixbuf, x, y, w, h, &min, &max );

	mid = ( min + max ) / 2;

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/*---------- put action here! ----------*/

			/* get the gray value at this point */
			guchar gray = ( 0.299 * r ) + ( 0.587 * g ) + ( 0.114 * b );

			if ( gray > mid )
			{
				r += ( r * contrast );
				g += ( g * contrast );
				b += ( b * contrast );
			}

			else
			{
				r -= ( r * contrast );
				g -= ( g * contrast );
				b -= ( b * contrast );
			}

			clamp_8bit ( &r, &g, &b, &a );
			/*------------ action done -------------*/

			sprintf ( col, "#%.2x%.2x%.2x", r, g, b );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_brightness ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h, gint brightness )
{

	gint j, k;
	gint p = 0;
	gint r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/*---------- put action here! ----------*/
			r += brightness;
			g += brightness;
			b += brightness;
			clamp_8bit ( &r, &g, &b, &a );
			/*------------ action done -------------*/

			sprintf ( col, "#%.2x%.2x%.2x", r, g, b );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_grayscale ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h )
{

	gint j, k;
	gint p = 0;
	guchar r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/* put action here ! */
			guchar gray = ( 0.299 * r ) + ( 0.587 * g ) + ( 0.114 * b );
			/* action done */

			sprintf ( col, "#%.2x%.2x%.2x", gray, gray, gray );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
int filter_invert ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h )
{
	gint j, k;
	gint p = 0;
	guchar r, g, b, a;

	g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

	/* this method prevents a deferencing error */
	k = gdk_pixbuf_get_width ( pixbuf );
	j = gdk_pixbuf_get_height ( pixbuf );

	gchar col[32];

	for ( j = y; j < y + h; j++ )
	{
		for ( k = x; k < x + h; k++ )
		{
			gdk_pixbuf_get_pixel ( pixbuf, k, j, &r, &g, &b, &a );

			/*---------- put action here! ----------*/

			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
			a = 225 - a;
			/*------------ action done -------------*/

			sprintf ( col, "#%.2x%.2x%.2x", r, g, b );
			gdk_pixbuf_set_pixel ( pixbuf, convertRGBtoPixel ( col ), k, j );
			p++;
		}

		k = gdk_pixbuf_get_width ( pixbuf );
	}

	return p;
}

/**
\brief
\author     William J Giddings
\date       20/03/2010
\note
\todo
**/
void gnoclPixBufFilters ( GdkPixbuf *pixbuf, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{


#ifdef DEBUG_FILTERS
	g_printf ( "gnoclPixBufFilters\n" );
	gint _i;


	gint x;
	gint y;
	gint width;
	gint height;

	x = 0;
	y = 0;

	width = gdk_pixbuf_get_width ( pixbuf );
	height = gdk_pixbuf_get_height ( pixbuf );


	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	static char *filterOptions[] =
	{
		"invert", "grayscale", "brightness",
		"contrast", "gamma", "color", "brightnessRange",
		NULL
	};

	static enum  optsIdx
	{
		InvertIdx, GrayScaleIdx, BrightnessIdx,
		ContrastIdx, GammaIdx, ColorIdx, BrightnessRangeIdx
	};

	int Idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[2], filterOptions, "option", TCL_EXACT, &Idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( Idx )
	{
		case BrightnessRangeIdx:
			{
				gint i;
				gint min = 0;
				gint max = 0;
				g_print ( "Invert 1\n", i );
				i = filter_brightness_range ( pixbuf, 10, 10, 100, 100, &min, &max );
				g_print ( "Total Pixels Draw = %d, min = %d max = %d\n", i, min, max );
			}
			break;
		case InvertIdx:
			{
				gint i;
				g_print ( "Invert 1\n", i );
				i = filter_invert ( pixbuf, 10, 10, 100, 100 );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case GrayScaleIdx:
			{
				gint i;
				g_print ( "Grayscale 1\n", i );
				i = filter_grayscale ( pixbuf, 10, 10, 100, 100 );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case BrightnessIdx:
			{
				gint i;
				g_print ( "Brightness 1\n", i );
				i = filter_brightness ( pixbuf, 10, 10, 100, 100, 50 );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case ContrastIdx:
			{

				gint i;
				g_print ( "Contrast 1\n", i );

				gdouble factor;

				if ( Tcl_GetDoubleFromObj ( interp, objv[3], &factor ) )
				{
					Tcl_SetResult ( interp, "Must have contrast factor in range 0 - 1,0.", TCL_STATIC );
					TCL_ERROR;
				}

				i = filter_contrast ( pixbuf, 10, 10, 100, 100, factor );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case GammaIdx:
			{
				gint i;
				g_print ( "Gamma 1\n", i );

				gdouble factor;

				if ( Tcl_GetDoubleFromObj ( interp, objv[3], &factor ) )
				{
					Tcl_SetResult ( interp, "Must have contrast factor in range 0 - 1,0.", TCL_STATIC );
					TCL_ERROR;
				}

				i = filter_gamma ( pixbuf, 10, 10, 100, 100, factor );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case ColorIdx:
			{
				gint i, j;
				gint x, y, width, height;
				g_print ( "Color 1\n", i );
				gchar *opt;
				int idx;
				guchar r, g, b;

				static enum opIdx { OP_ADD, OP_SUBTRACT };
				gint op = OP_ADD;

				gdouble factor;

				static char *newOptions[] =
				{
					"-operation", "-x", "-y", "-width", "-height", "-color", NULL
				};

				static enum  optsIdx
				{
					OperationIdx, XIdx, YIdx, WidthIdx, HeightIdx, ColorIdx
				};

				for ( i = 3; i < objc; i += 2 )
				{

					j = i + 1;

					opt = Tcl_GetString ( objv[i] );

					if ( Tcl_GetIndexFromObj ( interp, objv[i], newOptions, "command", TCL_EXACT, &idx ) != TCL_OK )
					{
						return TCL_ERROR;
					}

					switch ( idx )
					{

						case OperationIdx:
							{
								g_print ( "Operations = %s\n", Tcl_GetString ( objv[j] ) );

								if ( g_string_equal ( Tcl_GetString ( objv[j] ) , "add" ) )
								{
									op = OP_ADD;
									g_print ( "ADD\n" );
								}

								if ( g_string_equal ( Tcl_GetString ( objv[j] ) , "subtract" ) )
								{
									g_print ( "SUBTRACT\n" );
									op = OP_SUBTRACT;
								}

								g_print ( "operation  = %d\n", op );
							}
							break;
						case XIdx:
							{

								g_print ( "X\n" );
								Tcl_GetIntFromObj ( NULL, objv[j], &x );
							} break;
						case YIdx:

							g_print ( "Y\n" );
							{
								Tcl_GetIntFromObj ( NULL, objv[j], &y );
							} break;
						case WidthIdx:
							{

								g_print ( "Width \n" );
								Tcl_GetIntFromObj ( NULL, objv[j], &width );
							} break;
						case HeightIdx:
							{

								g_print ( "Height \n" );
								Tcl_GetIntFromObj ( NULL, objv[j], &height );
							} break;
						case ColorIdx:
							{
								g_print ( "Color\n" );
								sscanf ( Tcl_GetString ( objv[j] ) , " % d % d % d", &r, &g, &b );
							}
							break;
						default: {}
					}

					g_print ( "op = %d x = % d y = % d width = % d height = % d r = % d g = % d b = % d\n", op, x, y, width, height, r, g, b );
				}

				i = filter_color ( pixbuf, x, y, x + width, y + height, r, g, b );
				g_print ( "Total Pixels Draw = % d\n", i );

			} break;
		default:
			{

				g_print ( "Got nothing to do!\n" );
			}
	}

	return TCL_OK;
}

/**
\brief
\author     Tadej Borovšak
\date       14/Feb/2010
\note       gdk_color_parse() will only fill red, green and blue elements
of GdkColor. In order to fill pixel value, you'll need to
allocate your color using gdk_colormap_alloc_color().
But event if you do this, pixel value is not what you want
in this case, since pixel value is only meaningful in context
of your underlying windowing system.

**/
static guint32 convertRGBtoPixel ( gchar *clr )
{

	GdkColor color;
	guint32 pixel;

	/* Conversion factor from 16-bit color to 8-bit color (0xff / 0xffff) */
	const gdouble f = 0.00389105;

	/* create the colour from the supplied string, added by WJG */
	gdk_color_parse ( clr, &color );

	/* fill with colour */
	pixel = ( ( ( guint ) ( color.red   * f + 0.5 ) ) << 24 ) | /* R */
			( ( ( guint ) ( color.green * f + 0.5 ) ) << 16 ) | /* G */
			( ( ( guint ) ( color.blue  * f + 0.5 ) ) <<  8 ) | /* B */
			( 0xff <<  0 );                                     /* A */

	return pixel;
}

