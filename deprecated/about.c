/*
 * $Id: about.c,v 1.1 2005/01/01 15:28:48 baum Exp $
 *
 * This file implements the gnome about dialog
 *
 * Copyright (c) 2001 - 2004 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2001-04: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>



typedef struct
{
	Gnocl_StringBool title;
	Gnocl_StringBool version;
	Gnocl_StringBool copyright;
	Gnocl_ListBool authors;
	Gnocl_StringBool comments;
	Gnocl_ObjBool logo;

	GnomeAbout *widget;
} AboutParams;

static const GnoclWidgetOptions aboutOptions[] =
{
	{ "-title", GNOCL_STRING, GNOCL_OFFSET ( AboutParams, title ) },
	{ "-version", GNOCL_STRING, GNOCL_OFFSET ( AboutParams, version ) },
	{ "-copyright", GNOCL_STRING, GNOCL_OFFSET ( AboutParams, copyright ) },
	{ "-authors", GNOCL_LIST, GNOCL_OFFSET ( AboutParams, authors ) },
	{ "-comments", GNOCL_STRING, GNOCL_OFFSET ( AboutParams, comments ) },
	{ "-logo", GNOCL_OBJ, GNOCL_OFFSET ( AboutParams, logo ) },
	{ NULL, 0, 0 }
};

/*
   test, if params are valid
   set params and show app with new settings
*/
static int aboutConfigure ( Tcl_Interp *interp, AboutParams *pp )
{
	return TCL_OK;
}

static int aboutFunc ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	const char *cmds[] = { GNOCL_STD_CMD_NAMES,
						   NULL
						 };
	enum cmdIdx { GNOCL_STD_CMD_ENUMS };

	AboutParams *pp = ( AboutParams * ) data;
	int idx;

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
			GNOCL_STD_CMD_CASE ( interp, objc, objv, GTK_WIDGET ( pp->widget ) );
			break;
	}

	return TCL_OK;
}


int gnoclAboutCmd ( ClientData data, Tcl_Interp *interp,
					int objc, Tcl_Obj * const objv[] )
{
	AboutParams *pp = g_new ( AboutParams, 1 );
	char *name = gnoclGetAutoWidgetId();
	const char **authors = NULL;
	char *logo = NULL;
	int k, no;

	memset ( pp, 0, sizeof ( *pp ) );

	if ( gnoclParseAllWidgetOpts ( interp, objc, objv, 0,
								   aboutOptions, ( char * ) pp ) != TCL_OK )
		return TCL_ERROR;

	if ( pp->authors.val == NULL )
	{
		Tcl_SetResult ( interp, "parameter \"authors\" is required",
						TCL_STATIC );
		return TCL_ERROR;
	}

	else
	{
		int ret = Tcl_ListObjLength ( interp, pp->authors.val, &no );
		authors = g_new ( const char *, no + 1 );

		if ( ret != TCL_OK )
			return ret;

		for ( k = 0; k < no; ++k )
		{
			Tcl_Obj *tp;
			int ret = Tcl_ListObjIndex ( interp, pp->authors.val, k, &tp );

			if ( ret != TCL_OK )
				return ret;

			authors[k] = ( char * ) gnoclGetStringFromObj ( tp, NULL );
		}

		authors[no] = NULL;
	}

	if ( pp->logo.changed )
	{
		if ( gnoclGetStringType ( pp->logo.val ) != GNOCL_STR_FILE )
		{
			Tcl_SetResult ( interp,
							"logo must have string type \"file\" (prefix \"%/\")",
							TCL_STATIC );
			return TCL_ERROR;

		}

		logo = gnoclGetStringFromObj ( pp->logo.val, NULL );
	}

	/* FIXME
	   pp->widget = GNOME_ABOUT( gnome_about_new( pp->title.val, pp->version.val,
	         pp->copyright.val, authors, pp->comments.val, logo ) );
	*/

	/* FIXME: free author strings for( k = 0; k < no; ++k ) ... */
	g_free ( authors );

	if ( aboutConfigure ( interp, pp ) != TCL_OK )
	{
		/* FIXME: g_free pp->name ... */
		gtk_widget_destroy ( GTK_WIDGET ( pp->widget ) );
		return TCL_ERROR;
	}

	gnoclMemNameAndWidget ( name, GTK_WIDGET ( pp->widget ) );

	gtk_widget_show ( GTK_WIDGET ( pp->widget ) );

	Tcl_CreateObjCommand ( interp, name, aboutFunc, pp,
						   NULL /* FIXME: delFunc */ );
	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( name, -1 ) );

	return TCL_OK;
}

