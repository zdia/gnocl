/** filters.c
\brief
\author
\date
\since
\notes
**/


#include "gnocl.h"
#include <string.h>
#include <assert.h>

int filter_invert ( GdkPixbuf *pixbuf, gint x, gint y, gint w, gint h )
{
	gchar _n = "filter_invert";
	gint  _i = 1;

	g_printf ( "%d %d\n", _n, _i++ );
	g_printf ( "x %d y %d w %d h %d\n", x, y, w, h );

	gint j = pixbuf->height;

	while ( j-- )
	{
		g_printf ( "%d\n", j );
	}

	g_printf ( "%d %d\n", _n, _i++ );

	gint p;


	return TCL_OK;
}



void gnoclPixBufFilters ( GdkPixbuf * pixbuf, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{

#ifdef DEBUG_FILTERS
	g_printf ( "gnoclPixBufFilters\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif


	static char *filterOptions[] =
	{
		"invert", "greyScale", "brightness",
		"contrast", "gamma", "color",
		NULL
	};

	static enum  optsIdx
	{
		InvertIdx, GreyScaleIdx, BrightnessIdx,
		ContrastIdx, GammaIdx, ColorIdx
	};

	int Idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[2], filterOptions, "option", TCL_EXACT, &Idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( Idx )
	{
		case InvertIdx:
			{
				gint i;
				g_print ( "Invert 1\n", i );
				i = filter_invert ( pixbuf, 0, 0, 10, 10 );
				g_print ( "Total Pixels Draw = %d\n", i );
			} break;
		case GreyScaleIdx: {} break;
		case BrightnessIdx: {} break;
		case ContrastIdx: {} break;
		case GammaIdx: {} break;
		case ColorIdx: {} break;
		default:
			{
				g_print ( "Got nothing to do!\n" );
			}
	}

	return TCL_OK;
}






/* the fill command straight out of the lib sources */
/*
void _gdk_pixbuf_fill ( GdkPixbuf *pixbuf, guint32 pixel )
{
    guchar *pixels;
    guint r, g, b, a;
    guchar *p;
    guint w, h;

    g_return_if_fail ( GDK_IS_PIXBUF ( pixbuf ) );

    if ( pixbuf->width == 0 || pixbuf->height == 0 ) {
        return; }

    pixels = pixbuf->pixels;

    r = ( pixel & 0xff000000 ) >> 24;
    g = ( pixel & 0x00ff0000 ) >> 16;
    b = ( pixel & 0x0000ff00 ) >> 8;
    a = ( pixel & 0x000000ff );

    h = pixbuf->height;

    while ( h-- )
    {
        w = pixbuf->width;
        p = pixels;

        switch ( pixbuf->n_channels )
        {
            case 3:

                while ( w-- )
                {
                    p[0] = r;
                    p[1] = g;
                    p[2] = b;
                    p += 3;
                }

                break;
            case 4:

                while ( w-- )
                {
                    p[0] = r;
                    p[1] = g;
                    p[2] = b;
                    p[3] = a;
                    p += 4;
                }

                break;
            default:
                break;
        }

        pixels += pixbuf->rowstride;
    }
}
*/
