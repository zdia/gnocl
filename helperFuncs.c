/****h* helper/helperFuncs
 * NAME
 *  helperFuncs.c
 * SYNOPSIS
 *  This file implements some helper functions.
 * FUNCTION
 * NOTES
 * BUGS
 * SEE ALSO
 *****/

/*
 * $Id: helperFuncs.c,v 1.11 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements some helper functions
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-01: added getIdx
   2009-01: added gnoclGetWigdetGeometry
        05: added gnoclPosOffset from text.c
   2003-04: added gnoclPixbufFromObj
   2002-12: removed gnoclEventToString
            joined with convert.c
        09: gnoclFindLabel, gnoclLabelParseULine
   2001-07: Begin of developement
 */

#include "gnocl.h"
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct
{
	GtkWidget *widget;
	GtkType   type;
} FindWidgetStruct;


/**
/brief    Compare string with list of acceptable choices.
          Return index of any match.
/author   William J Giddings
/date     24-11-2009
/note     For use in conjuction with switch to determine action based upon
          one of a range of choices given choices.
          Pretty well does the same as getIDx below
**/
int compare ( char *str, char *choices[] )
{

	int i;

	for ( i = 0; choices[i] != NULL ; i++ )
	{
		if ( strcmp ( str, choices[i] ) == 0 )
		{
			return i;
		}
	}
}

/**
\brief      Get index of matched string in an NULL terminated array of strings.
\author     William J Giddings
\date       05/05/09
\param      gchar *opts[]   Pointer the array containing keywords
\param      gchar *str      Pointer to the string to match
\param      gint *idx       Pointer to int which contains the matched index
\return     boolean, returns 0 is match found else -1
\note       Useful for making choices from a list and processing with switch.
**/
int getIdx ( gchar *opts[], gchar *str, gint *idx )
{
	int i = 0;

	while ( opts[i] != NULL ) /* go through array until NULL encountered */
	{

		if ( strcmp ( str , opts[i] ) == 0 )
		{
			*idx = i;
			return;
		}

		i++;
	}

	/* must be an error here! */
	return -1;
}


/**
\author     William J Giddings
\date       15/01/09
**/
char *gnoclGetWidgetGeometry ( GtkWidget *widget )
{
	gint x, y;
	gint w, h;
	char txt[500];

	x = widget->allocation.x;
	y = widget->allocation.y;
	w = widget->allocation.width;
	h = widget->allocation.height;

	sprintf ( txt, "%d %d %d %d", x, y, w, h );

	return txt;
}


/**
\brief      Find the "internal children" of a container widget.
\author     Peter G Baum
**/
static void findChildIntern ( GtkWidget *widget, gpointer data )
{
	FindWidgetStruct *fw = ( FindWidgetStruct * ) data;

	if ( fw->widget == NULL )
	{
		if ( GTK_CHECK_TYPE ( widget, fw->type ) )
		{
			fw->widget = widget;
		}

		else if ( GTK_IS_CONTAINER ( widget ) )
		{
			gtk_container_foreach ( GTK_CONTAINER ( widget ), findChildIntern, data );
		}
	}
}

/**
**/
GtkWidget *gnoclFindChild ( GtkWidget *widget, GtkType type )
{
	FindWidgetStruct fw;
	fw.widget = NULL;
	fw.type = type;
	findChildIntern ( widget, &fw );
	return fw.widget;
}


/**
**/
int gnoclPosOffset ( Tcl_Interp *interp, const char *txt, int *offset )
{
	*offset = 0;
	/* pos[+-]offset */

	if ( *txt == '+' || *txt == '-' )
	{
		if ( sscanf ( txt + 1, "%d", offset ) != 1 )
		{
			Tcl_AppendResult ( interp, "invalid offset \"", txt + 1,
							   "\"", NULL );
			return TCL_ERROR;
		}

		if ( *txt == '-' )
			*offset *= -1;

		for ( ++txt; isdigit ( *txt ); ++txt )
			;
	}

	if ( *txt )
	{
		Tcl_AppendResult ( interp, "invalid appendix \"", txt,
						   "\"", NULL );
		return TCL_ERROR;
	}

	return TCL_OK;
}




/**
\author     Peter G Baum
Tcl_Interp *interp
GnoclPercSubst *ps
const char *orig_script
int background

Problems here with gnocl::entry

/bug reports mention problems with free(); -attempting to free unallocated memory is it because of Tcl_Free?
Tcl_Free is only used in this function, can it be replaced with g_free?

**/
int gnoclPercentSubstAndEval ( Tcl_Interp *interp, GnoclPercSubst *ps, const char *orig_script, int background )
{
#ifdef DEBUG
	g_print ( "gnoclPercentSubstAndEval\n" );
#endif
	int        len = strlen ( orig_script );
	const char *old_perc = orig_script;
	const char *perc;
	GString    *script = g_string_sized_new ( len + 20 );
	int        ret;

	for ( ; ( perc = strchr ( old_perc, '%' ) ) != NULL; old_perc = perc + 2 )
	{
		g_string_sprintfa ( script, "%.*s", perc - old_perc, old_perc );

#ifdef DEBUG
		printf ( "\t1) script: \"%s\"\n", script->str );
#endif

		if ( perc[1] == '%' )
			g_string_append_c ( script, '%' );
		else
		{
			int k = 0;

			while ( ps[k].c && ps[k].c != perc[1] )
				++k;

			if ( ps[k].c == 0 )
			{
#ifdef DEBUG
				printf ( "\t2) DEBUG: unknown percent substitution %c\n", perc[1] );
#endif
				/*
				Tcl_AppendResult( interp, "unknown percent substitution" ,
				      (char *)NULL );
				g_string_free( script, 1 );
				return TCL_ERROR;
				*/
				g_string_append_c ( script, '%' );
				g_string_append_c ( script, perc[1] );
			}

			else
			{
				switch ( ps[k].type )
				{
					case GNOCL_STRING:
						/* FIXME: escape special characters: ' ', '\n', '\\' ...
						   or use Tcl_EvalObj? */

						if ( ps[k].val.str != NULL )
						{
							/* handle special characters correctly
							   TODO: would it be better to escape
							         special characters?
							*/
							char *txt = Tcl_Merge ( 1, &ps[k].val.str );
							g_string_append ( script, txt );
#ifdef DEBUG
							printf ( "\t3) percent string: \"%s\" -> \"%s\"\n", ps[k].val.str, txt );
#endif
							Tcl_Free ( txt );
							//g_string_free (txt,1);
						}

						else
							g_string_append ( script, "{}" );

						break;

					case GNOCL_OBJ:

						if ( ps[k].val.obj != NULL )
						{
							/* embedden 0s should be UTF encoded. Should
							   therefor also work. */
							/*
							g_string_sprintfa( script, "%s",
							      Tcl_GetString( ps[k].val.obj ) );
							*/
							const char *argv[2] = { NULL, NULL };
							char *txt;
							argv[0] = Tcl_GetString ( ps[k].val.obj );
							txt = Tcl_Merge ( 1, argv );
							g_string_append ( script, txt );
							Tcl_Free ( txt );

						}

						else
							g_string_append ( script, "{}" );

						break;

					case GNOCL_INT:
						g_string_sprintfa ( script, "%d", ps[k].val.i );

						break;

					case GNOCL_BOOL:
						g_string_sprintfa ( script, "%d", ps[k].val.b != 0 );

						break;

					case GNOCL_DOUBLE:
						g_string_sprintfa ( script, "%f", ps[k].val.d );

						break;

					default:
						assert ( 0 );

						break;
				}
			}
		}
	}

	g_string_append ( script, old_perc );

	/* Tcl_EvalObj would be faster and more elegant, but incompatible: eg.
	two consecutive percent substitutions without space */
	ret = Tcl_EvalEx ( interp, script->str, -1, TCL_EVAL_GLOBAL | TCL_EVAL_DIRECT );
#ifdef DEBUG
	printf ( "\t4) DEBUG: script in percEval: %s -> %d %s\n", script->str, ret, Tcl_GetString ( Tcl_GetObjResult ( interp ) ) );
#endif
	g_string_free ( script, 1 );

	if ( background && ret != TCL_OK )
	{
		Tcl_BackgroundError ( interp );
	}

	return ret;
}

/**
**/
int gnoclAttacheVariable ( GnoclOption *newVar, char **oldVar, const char *signal, GObject *obj,
						   GCallback gtkFunc, Tcl_Interp *interp, Tcl_VarTraceProc tclFunc, gpointer data )
{
	if ( *oldVar && ( newVar == NULL || newVar->status == GNOCL_STATUS_CHANGED ) )
		Tcl_UntraceVar ( interp, *oldVar, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY,
						 tclFunc, data );

	if ( newVar == NULL || newVar->status != GNOCL_STATUS_CHANGED
			|| newVar->val.str[0] == 0 )
	{
		/* no new variable -> delete all */
		if ( *oldVar )
		{
			g_signal_handlers_disconnect_matched ( obj,
												   G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) gtkFunc, NULL );
			g_free ( *oldVar );
			*oldVar = NULL;
		}
	}

	else
	{
		if ( *oldVar == NULL )    /* new variable but old didn't exist */
			g_signal_connect ( obj, signal, gtkFunc, data );
		else                      /* new variable and old did exist */
			g_free ( *oldVar );

		*oldVar = newVar->val.str;    /* transfer ownership */

		newVar->val.str = NULL;

		Tcl_TraceVar ( interp, *oldVar, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY,
					   tclFunc, data );
	}

	return TCL_OK;
}

/**
\brief  Provides tracer function on variables
 */
int gnoclAttacheOptCmdAndVar ( GnoclOption *newCmd, char **oldCmd, GnoclOption *newVar, char **oldVar,
							   const char *signal, GObject *obj, GCallback gtkFunc, Tcl_Interp *interp,
							   Tcl_VarTraceProc tclFunc, gpointer data )
{
	const int wasConnected = *oldVar != NULL || *oldCmd != NULL;

	/* handle variable */

	if ( newVar == NULL || newVar->status == GNOCL_STATUS_CHANGED )
	{
		if ( *oldVar )
		{
			Tcl_UntraceVar ( interp, *oldVar, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY, tclFunc, data );
			g_free ( *oldVar );
			*oldVar = NULL;
		}
	}

	if ( newVar && newVar->status == GNOCL_STATUS_CHANGED && *newVar->val.str != '\0' )
	{
		*oldVar = g_strdup ( newVar->val.str );
		Tcl_TraceVar ( interp, *oldVar, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY, tclFunc, data );
	}

	/* handle command */

	if ( newCmd == NULL || newCmd->status == GNOCL_STATUS_CHANGED )
	{
		if ( *oldCmd )
		{
			g_free ( *oldCmd );
			*oldCmd = NULL;
		}
	}

	if ( newCmd && newCmd->status == GNOCL_STATUS_CHANGED && *newCmd->val.str != '\0' )
	{
		*oldCmd = g_strdup ( newCmd->val.str );
	}

	/* if cmd or var is set, we need the gtkFunc */

	if ( *oldVar || *oldCmd )
	{
		if ( wasConnected == 0 )
		{
			g_signal_connect ( G_OBJECT ( obj ), signal, gtkFunc, data );
		}
	}

	else if ( wasConnected )
	{
		g_signal_handlers_disconnect_matched ( G_OBJECT ( obj ), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, ( gpointer * ) gtkFunc, NULL );
	}

	return TCL_OK;
}



/**
**/
int gnoclGet2Boolean ( Tcl_Interp *interp, Tcl_Obj *obj, int *b1, int *b2 )
{
	int no;

	if ( Tcl_ListObjLength ( interp, obj, &no ) == TCL_OK
			&& ( no == 2 || no == 1 ) )
	{
		if ( no == 1 )
		{
			if ( Tcl_GetBooleanFromObj ( interp, obj, b1 ) != TCL_OK )
				return TCL_ERROR;

			*b2 = *b1;

			return TCL_OK;
		}

		else
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, obj, 0, &tp ) == TCL_OK )
			{
				if ( Tcl_GetBooleanFromObj ( interp, tp, b1 ) != TCL_OK )
					return TCL_ERROR;

				if ( Tcl_ListObjIndex ( interp, obj, 1, &tp ) == TCL_OK )
				{
					if ( Tcl_GetBooleanFromObj ( interp, tp, b2 ) != TCL_OK )
						return TCL_ERROR;
				}

				return TCL_OK;
			}
		}
	}

	Tcl_AppendResult ( interp, "Expected boolean value or list of "

					   "two boolean values but got \"", Tcl_GetString ( obj ), "\"", NULL );

	return TCL_ERROR;
}

/**
**/
int gnoclGet2Int ( Tcl_Interp *interp, Tcl_Obj *obj, int *b1, int *b2 )
{
	int no;

	if ( Tcl_ListObjLength ( interp, obj, &no ) == TCL_OK
			&& ( no == 2 || no == 1 ) )
	{
		if ( no == 1 )
		{
			if ( Tcl_GetIntFromObj ( interp, obj, b1 ) != TCL_OK )
				return TCL_ERROR;

			*b2 = *b1;

			return TCL_OK;
		}

		else
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, obj, 0, &tp ) == TCL_OK )
			{
				if ( Tcl_GetIntFromObj ( interp, tp, b1 ) != TCL_OK )
					return TCL_ERROR;

				if ( Tcl_ListObjIndex ( interp, obj, 1, &tp ) == TCL_OK )
				{
					if ( Tcl_GetIntFromObj ( interp, tp, b2 ) != TCL_OK )
						return TCL_ERROR;
				}

				return TCL_OK;
			}
		}
	}

	Tcl_AppendResult ( interp, "Expected integer value or list of "
					   "two integer values but got \"", Tcl_GetString ( obj ), "\"", NULL );

	return TCL_ERROR;
}



/**
**/
int gnoclGet2Double ( Tcl_Interp *interp, Tcl_Obj *obj, double *b1, double *b2 )
{
	int no;

	if ( Tcl_ListObjLength ( interp, obj, &no ) == TCL_OK
			&& ( no == 2 || no == 1 ) )
	{
		if ( no == 1 )
		{
			if ( Tcl_GetDoubleFromObj ( interp, obj, b1 ) != TCL_OK )
				return TCL_ERROR;

			*b2 = *b1;

			return TCL_OK;
		}

		else
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, obj, 0, &tp ) == TCL_OK )
			{
				if ( Tcl_GetDoubleFromObj ( interp, tp, b1 ) != TCL_OK )
					return TCL_ERROR;

				if ( Tcl_ListObjIndex ( interp, obj, 1, &tp ) == TCL_OK )
				{
					if ( Tcl_GetDoubleFromObj ( interp, tp, b2 ) != TCL_OK )
						return TCL_ERROR;
				}

				return TCL_OK;
			}
		}
	}

	Tcl_AppendResult ( interp, "Expected float value or list of "

					   "two float values but got \"", Tcl_GetString ( obj ), "\"", NULL );

	return TCL_ERROR;
}



/**
 */
static int getScrollbarPolicy ( Tcl_Interp *interp, Tcl_Obj *obj,
								GtkPolicyType *pol )
{
	const char *txt[] = { "always", "never", "automatic", NULL };
	GtkPolicyType policies[] =
	{
		GTK_POLICY_ALWAYS, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC
	};
	int on;

	if ( Tcl_GetBooleanFromObj ( NULL, obj, &on ) == TCL_OK )
		*pol = on ? GTK_POLICY_ALWAYS : GTK_POLICY_NEVER;
	else
	{
		int idx;

		if ( Tcl_GetIndexFromObj ( interp, obj, txt, "scrollbar policy",
								   TCL_EXACT, &idx ) != TCL_OK )
			return TCL_ERROR;

		*pol = policies[idx];
	}

	return TCL_OK;
}



/**
 */
int gnoclGetScrollbarPolicy ( Tcl_Interp *interp, Tcl_Obj *obj, GtkPolicyType *hor, GtkPolicyType *vert )
{
	int no;

	if ( Tcl_ListObjLength ( interp, obj, &no ) != TCL_OK  || no > 2 )
	{
		Tcl_SetResult ( interp, "policy must be either a single value "
						"or a list with two elements.", TCL_STATIC );
		return TCL_ERROR;
	}

	if ( no == 1 )
	{
		if ( getScrollbarPolicy ( interp, obj, hor ) != TCL_OK )
			return TCL_ERROR;

		*vert = *hor;
	}

	else
	{
		Tcl_Obj *tp;

		if ( Tcl_ListObjIndex ( interp, obj, 0, &tp ) != TCL_OK )
			return TCL_ERROR;

		if ( getScrollbarPolicy ( interp, tp, hor ) != TCL_OK )
			return TCL_ERROR;

		if ( Tcl_ListObjIndex ( interp, obj, 1, &tp ) != TCL_OK )
			return TCL_ERROR;

		if ( getScrollbarPolicy ( interp, tp, vert ) != TCL_OK )
			return TCL_ERROR;
	}

	return TCL_OK;
}



/**
 */
int gnoclGetSelectionMode ( Tcl_Interp *interp, Tcl_Obj *obj, GtkSelectionMode *selection )
{
	const char *txt[] = { "single", "browse", "multiple", "extended", NULL };
	GtkSelectionMode modes[] = { GTK_SELECTION_SINGLE, GTK_SELECTION_BROWSE, GTK_SELECTION_MULTIPLE, GTK_SELECTION_EXTENDED };

	int idx;

	if ( Tcl_GetIndexFromObj ( interp, obj, txt, "selection modes",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	*selection = modes[idx];

	return TCL_OK;
}



/**
 */
int gnoclGetOrientationType (
	Tcl_Interp *interp,
	Tcl_Obj *obj,
	GtkOrientation *orient )
{
	const char *txt[] = { "horizontal", "vertical", NULL };
	GtkOrientation types[] = { GTK_ORIENTATION_HORIZONTAL,
							   GTK_ORIENTATION_VERTICAL
							 };

	int idx;

	if ( Tcl_GetIndexFromObj ( interp, obj, txt, "orientation",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	*orient = types[idx];

	return TCL_OK;
}



/**
\brief      This function simply loads a single file from disk into memory.
 */
GdkPixbuf *gnoclPixbufFromObj ( Tcl_Interp *interp, GnoclOption *opt )
{
	char *txt = gnoclGetString ( opt->val.obj );
	GError *error = NULL;
	GdkPixbuf *pix = gdk_pixbuf_new_from_file ( txt, &error );

	assert ( gnoclGetStringType ( opt->val.obj ) == GNOCL_STR_FILE );

	if ( pix == NULL )
	{
		Tcl_SetResult ( interp, error->message, TCL_VOLATILE );
		g_error_free ( error );
		return NULL;
	}

	return pix;
}



/**
\brief      Read in a list of images and bitmasks, composite them to a single
            pixbuf and then return a pointer to the pixbuf to the calling function
\author     WJG
\date       16-Feb-09
\note      This assumes that the images to be composite and the masks are ALL of
            matching sizes. There is no attempt to reposition or resize graphics.
            Key Library Functions
            GtkWidget  *gtk_image_new_from_file (const gchar *filename);
            GtkWidget  *gtk_image_new_from_pixbuf (GdkPixbuf *pixbuf);
            void gtk_image_get_image (GtkImage *image, GdkImage **gdk_image, GdkBitmap **mask);
            GtkWidget  *gtk_image_new_from_image (GdkImage *image, GdkBitmap *mask);
            GdkPixbuf  *gtk_image_get_pixbuf (GtkImage *image);
            void gdk_pixbuf_render_threshold_alpha (GdkPixbuf *pixbuf,GdkBitmap *bitmap, int src_x,
                int src_y, int dest_x, int dest_y, int width, int height, int alpha_threshold);
            ie. gdk_pixbuf_render_threshold_alpha (pb,mp,0,0,0,0,-1,-1,128);
 */
GdkPixbuf *gnoclBlendPixbufFromObj (
	Tcl_Interp *interp,
	GnoclOption *opt )
{

	GdkPixbuf *pixbuf = NULL;
	GError *err = NULL;
	GtkWidget *gtk_image = NULL;
	GtkWidget *gtk_image_blend = NULL;
	GdkImage *gdk_image = NULL;
	GdkBitmap *gdk_bitmap = NULL;

	char * pch;

	printf ( "helperFuncs/gnoclBlendPixbufFromObj pixbuf %s\n", Tcl_GetString ( opt->val.obj ) );

	pch = strtok ( Tcl_GetString ( opt->val.obj ), " " );
	int aa = 0;
	int bb = 0;

	while ( pch != NULL )
	{
		switch ( aa )
		{
			case 0:
				{
					g_print ( "pch = %s\n", pch );
					pixbuf = gdk_pixbuf_new_from_file ( pch , &err );

					if ( err != NULL )
					{
						g_warning ( "%s", err->message );
						g_error_free ( err );
						return NULL;
					}

					/* error checking over, create the image */
					gtk_image = gtk_image_new_from_pixbuf ( pixbuf );

					/* create a gdk_image from this, dump any alpha channel */

					if ( gdk_image == NULL )
					{

						/* get the size of the pixbuf */

						g_print ( "make a new gdk_image\n" );
						gdk_image = gdk_image_new (
										GDK_IMAGE_FASTEST,
										gdk_visual_get_system(),
										gdk_pixbuf_get_width ( pixbuf ),
										gdk_pixbuf_get_height ( pixbuf ) );
					}

					g_print ( "AAA\n" );

					gtk_image_get_image ( gtk_image, &gdk_image, NULL );
					g_print ( "BBB\n" );

				}

				break;
			case 1:
				{

					pixbuf = gdk_pixbuf_new_from_file ( pch, &err );

					if ( err != NULL )
					{
						g_warning ( "%s", err->message );
						g_error_free ( err );
						return NULL;
					}

					/* error checking over, create the clipping mask */
					gdk_pixbuf_render_threshold_alpha ( pixbuf, gdk_bitmap, 0, 0, 0, 0, -1, -1, 1 );
				}

				break;
		}

		if ( aa < 3 )
		{
			aa++;
		}

		else
		{
			aa = 0;
		}


		pch = strtok ( NULL, " " );


		/* do the actual compositing onto the buffer background */

		gtk_image_blend = gtk_image_new_from_image ( gdk_image, gdk_bitmap );

		g_print ( "composite images now!\n" );


	}

	if ( 0 )
	{
		return gtk_image_get_pixbuf ( gtk_image_blend );
	}

	else
	{
		return pixbuf;
	}
}



