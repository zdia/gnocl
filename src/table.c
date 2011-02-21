/**
\brief      This module implements the gnocl::table widget.
\authors    Peter G. Baum, William J Giddings
\date       2001-03:
**/

/* user documentation */

/**
\page page45 gnocl::table
\section sec gnocl::table
  Implementation of gnocl::table
\subsection subsection1 Implemented Options
\verbatim

\endverbatim
\subsection subsection2 Implemented Commands
\verbatim

\endverbatim
\subsection subsection3 Sample Tcl Script
\code
   set box [gnocl::box -buttonType 1]
   $box add [gnocl::button -text "%_Do_n't save" -icon "%#No"]
   $box add [gnocl::button -text "%#Cancel"]
   $box add [gnocl::button -text "%#Save"]
   gnocl::window -title "Buttons" -child $box
\endcode
\subsection subsection4 Produces
\image html "../pics/table.png"
**/
/*
 * $Id: table.c,v 1.10 2005/01/01 15:27:54 baum Exp $
 *
 * This file implements the table widget which is a combination of the
 * gtk table widget and the gtk frame widget
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/*
   History:
   2009-12: changed defaults for borderwidth and row spacing to to GNOCL_PAD_TINY
   2008-10: added class
        08: added gnoclOptPadding
            default value for fill is now 0
        05: fixed -fill and -expand options
            added index "end+-offset"
   2003-02: moved packOptions also to GnoclOption
            added drag and drop options
   2002-08: switched from GnoclWidgetOptions to GnoclOption
        12: removed {x,y}{Expand,Shrink,Fill,Padding}, use list instead
            added alignment
            add{Row,Column}
            {row,column}Spacing
            exchange order of row and column in "add"
            STD_OPTIONS
        12: added frame options
   2001-03: Begin of developement
 */

#include "gnocl.h"
#include <string.h>
#include <assert.h>

static GnoclOption tableOptions[] =
{
	/* table options */
	{ "-homogeneous", GNOCL_BOOL, "homogeneous" },
	{ "-rows", GNOCL_INT, "n-rows" },
	{ "-columns", GNOCL_INT, "n-columns" },
	{ "-rowSpacing", GNOCL_OBJ, "row-spacing", gnoclOptPadding },
	{ "-columnSpacing", GNOCL_OBJ, "column-spacing", gnoclOptPadding },
	{ "-borderWidth", GNOCL_OBJ, "border-width", gnoclOptPadding },

	/* frame options */
	{ "-label", GNOCL_STRING, "label" },    /* 6 */
	{ "-shadow", GNOCL_OBJ, "shadow", gnoclOptShadow },
	{ "-labelAlign", GNOCL_OBJ, "label-xalign", gnoclOptHalign },

	/* common options */
	{ "-name", GNOCL_STRING, "name" },      /* 9 */
	{ "-visible", GNOCL_BOOL, "visible" },
	{ "-sensitive", GNOCL_BOOL, "sensitive" },
	{ "-tooltip", GNOCL_OBJ, "", gnoclOptTooltip },
	{ "-onShowHelp", GNOCL_OBJ, "", gnoclOptOnShowHelp },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptCommand },
	{ "-dropTargets", GNOCL_LIST, "t", gnoclOptDnDTargets },
	{ "-dragTargets", GNOCL_LIST, "s", gnoclOptDnDTargets },
	{ "-onDropData", GNOCL_OBJ, "", gnoclOptOnDropData },
	{ "-onDragData", GNOCL_OBJ, "", gnoclOptOnDragData },
	{ "-widthGroup", GNOCL_OBJ, "w", gnoclOptSizeGroup },
	{ "-heightGroup", GNOCL_OBJ, "h", gnoclOptSizeGroup },
	{ "-sizeGroup", GNOCL_OBJ, "s", gnoclOptSizeGroup },

	{ "-backgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },
	{ NULL },
};

static const int startFrameOpts  = 6;
static const int startCommonOpts = 9;

static GnoclOption packOptions[] =
{
	{ "-start", GNOCL_INT, NULL },       /* 0 */ /* this must be the first! */
	{ "-rowSpan", GNOCL_INT, NULL },     /* 1 */
	{ "-columnSpan", GNOCL_INT, NULL },  /* 2 */
	{ "-expand", GNOCL_LIST, NULL },     /* 3 */
	{ "-shrink", GNOCL_LIST, NULL },     /* 4 */
	{ "-fill", GNOCL_LIST, NULL },       /* 5 */
	{ "-padding", GNOCL_LIST, NULL },    /* 6 */
	{ "-align", GNOCL_LIST, NULL },      /* 7 */
	{ NULL, 0, 0 }
};

static const int startIdx      = 0;
static const int rowSpanIdx    = 1;
static const int columnSpanIdx = 2;
static const int expandIdx     = 3;
static const int shrinkIdx     = 4;
static const int fillIdx       = 5;
static const int paddingIdx    = 6;
static const int alignIdx      = 7;

typedef struct
{
	char       *name;
	Tcl_Interp *interp;
	GtkTable   *table;
	GtkFrame   *frame;
} TableParams;

typedef struct
{
	GtkAttachOptions xOptions, yOptions;
	int              xPad, yPad;
	int              rowSpan, columnSpan;
	int              useAlign;
	double           xScale, yScale,
	xAlign, yAlign;
} AttachOptions;

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static void alignDestroyFunc ( GtkWidget *widget, gpointer data )
{
	gtk_widget_destroy ( GTK_WIDGET ( data ) );
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int needFrame ( const GnoclOption options[] )
{
	int k;

	for ( k = startFrameOpts; k < startCommonOpts; ++k )
		if ( options[k].status == GNOCL_STATUS_CHANGED )
			return 1;

	return 0;
}

/* FIXME: create or delete frame if necessary
          also change: gnoclMemNameAndWidget( name, widget );
*/

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int configure ( Tcl_Interp *interp, GtkFrame *frame, GtkTable *table,
					   GnoclOption options[] )
{
	GtkWidget *widget = frame ? GTK_WIDGET ( frame ) : GTK_WIDGET ( table );

	if ( frame == NULL && needFrame ( options ) )
	{
		Tcl_SetResult ( interp, "Frame options can only be set if a "
						"frame option is given on creation (for example -shadow none).",
						TCL_STATIC );
		return TCL_ERROR;
	}

	if ( frame != NULL )
	{
		if ( gnoclSetOptions ( interp, options + startFrameOpts,
							   G_OBJECT ( frame ), startCommonOpts - startFrameOpts ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( gnoclSetOptions ( interp, tableOptions, G_OBJECT ( table ),
						   startFrameOpts ) != TCL_OK )
		return TCL_ERROR;

	if ( gnoclSetOptions ( interp, tableOptions + startCommonOpts,
						   G_OBJECT ( widget ), -1 ) != TCL_OK )
		return TCL_ERROR;

	return TCL_OK;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int parsePackOptions ( Tcl_Interp *interp, GnoclOption opts[],
							  AttachOptions *pa )
{
	double xFill = 1.,   yFill = 1.;
	gfloat xAlign = 0.5, yAlign = 0.5;
	int    xShrink = 1,  yShrink = 1;
	int    xExpand = 1,  yExpand = 1;
	int    xPadding = 0, yPadding = 0;

	pa->useAlign = 0;

	if ( opts[alignIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGetBothAlign ( interp, opts[alignIdx].val.obj,
								 &xAlign, &yAlign ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( opts[expandIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGet2Boolean ( interp, opts[expandIdx].val.obj,
								&xExpand, &yExpand ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( opts[shrinkIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGet2Boolean ( interp, opts[shrinkIdx].val.obj,
								&xShrink, &yShrink ) != TCL_OK )
			return TCL_ERROR;
	}

	if ( opts[fillIdx].status == GNOCL_STATUS_CHANGED )
	{
		if ( gnoclGet2Double ( interp, opts[fillIdx].val.obj,
							   &xFill, &yFill ) != TCL_OK )
			return TCL_ERROR;

		if ( xFill < .0 || yFill < .0 || xFill > 1. || yFill > 1. )
		{
			Tcl_SetResult ( interp, "Options \"fill\" must be between 0 and 1",
							TCL_STATIC );
			return TCL_ERROR;
		}
	}

	if ( opts[paddingIdx].status == GNOCL_STATUS_CHANGED )
	{
		int no;

		if ( Tcl_ListObjLength ( interp, opts[paddingIdx].val.obj, &no ) != TCL_OK
				|| ( no != 2 && no != 1 ) )
		{
			Tcl_AppendResult ( interp, "Expected integer value or list of "
							   "two integer values but got \"",
							   Tcl_GetString ( opts[paddingIdx].val.obj ), "\"", NULL );
			return TCL_ERROR;
		}

		if ( no == 1 )
		{
			if ( gnoclGetPadding ( interp, opts[paddingIdx].val.obj,
								   &xPadding ) != TCL_OK )
				return TCL_ERROR;

			yPadding = xPadding;
		}

		else
		{
			Tcl_Obj *tp;

			if ( Tcl_ListObjIndex ( interp, opts[paddingIdx].val.obj, 0,
									&tp ) != TCL_OK )
				return TCL_ERROR;

			if ( gnoclGetPadding ( interp, tp, &xPadding ) != TCL_OK )
				return TCL_ERROR;

			if ( Tcl_ListObjIndex ( interp, opts[paddingIdx].val.obj, 1,
									&tp ) != TCL_OK )
				return TCL_ERROR;

			if ( gnoclGetPadding ( interp, tp, &yPadding ) != TCL_OK )
				return TCL_ERROR;
		}
	}

	pa->columnSpan = 1;

	if ( opts[columnSpanIdx].status == GNOCL_STATUS_CHANGED )
		pa->columnSpan = opts[columnSpanIdx].val.i;

	pa->rowSpan = 1;

	if ( opts[rowSpanIdx].status == GNOCL_STATUS_CHANGED )
		pa->rowSpan = opts[rowSpanIdx].val.i;

	pa->xOptions = pa->yOptions = 0;

	pa->xPad = xPadding;

	pa->yPad = yPadding;

	pa->xAlign = xAlign;

	pa->yAlign = yAlign;

	pa->xScale = xFill;

	pa->yScale = yFill;

	if ( ( xFill > .0 && xFill < 1. ) || ( yFill > .0 && yFill < 1. ) )
		pa->useAlign = 1;

	if ( xAlign != 0.5 || yAlign != 0.5 )
		pa->useAlign = 1;

	if ( xExpand ) pa->xOptions |= GTK_EXPAND;

	if ( yExpand ) pa->yOptions |= GTK_EXPAND;

	if ( xFill == 1. ) pa->xOptions |= GTK_FILL;

	if ( yFill == 1. ) pa->yOptions |= GTK_FILL;

	if ( pa->useAlign )
	{
		if ( xExpand )
			pa->xOptions |= ( GTK_FILL | GTK_EXPAND );
		else
			pa->xOptions |= GTK_FILL;

		if ( yExpand )
			pa->yOptions |= ( GTK_FILL | GTK_EXPAND );
		else
			pa->yOptions |= GTK_FILL;
	}

	return TCL_OK;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
typedef struct
{
	GtkContainer *container;
	guint         val;
	const char   *property;
} MaxChildStruct;

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static void getMaxChildProperty ( GtkWidget *child, gpointer data )
{
	MaxChildStruct *ms = ( MaxChildStruct * ) data;
	guint val;
	GValue gValue = { 0 };

	g_value_init ( &gValue, G_TYPE_UINT );

	gtk_container_child_get_property ( ms->container, child,
									   ms->property, &gValue );
	val = g_value_get_uint ( &gValue );

	if ( ms->val < val )
		ms->val = val;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int getMaxRowCol ( GtkTable *table, int getCol )
{
	MaxChildStruct ms;
	/* to be C compliant, this cannot be put in the initalizer */
	ms.container =  GTK_CONTAINER ( table );
	ms.val       = 0u;
	ms.property  = getCol ? "bottom_attach" : "right_attach";

	gtk_container_foreach ( GTK_CONTAINER ( table ),
							getMaxChildProperty, &ms );
	return ms.val;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int addRowCol ( GtkTable *table, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[], int addRowFlag )
{
	int              ret = TCL_ERROR;
	AttachOptions    ao;
	int              start = -1;

	if ( objc < 2 )
	{
		/* id add{Row,Column} list ... */
		Tcl_WrongNumArgs ( interp, 2, objv,
						   "widget-list ?option val ...?" );
		return TCL_ERROR;
	}

	if ( gnoclParseOptions ( interp, objc - 2, objv + 2, packOptions ) == TCL_OK )
	{
		ret = parsePackOptions ( interp, packOptions, &ao );

		if ( packOptions[startIdx].status == GNOCL_STATUS_CHANGED )
			start = packOptions[startIdx].val.i;
	}

	gnoclClearOptions ( packOptions );

	if ( ret == TCL_OK )
	{
		int    noChilds;
		int    n;
		int    column = 0,
						row = 0;

		if ( start >= 0 )
		{
			if ( addRowFlag )
				column = start;
			else
				row = start;
		}

		else
		{
			if ( addRowFlag )
				row = getMaxRowCol ( table, 1 );
			else
				column = getMaxRowCol ( table, 0 );
		}

		if ( Tcl_ListObjLength ( interp, objv[2], &noChilds ) != TCL_OK
				|| noChilds < 1 )
		{
			Tcl_SetResult ( interp, "widget-list must be proper list",
							TCL_STATIC );
			noChilds = 0;
			ret = TCL_ERROR;
		}

		for ( n = 0; n < noChilds; ++n )
		{
			GtkWidget *childWidget;
			Tcl_Obj *tp;
			const char *childName;

			if ( Tcl_ListObjIndex ( interp, objv[2], n, &tp ) != TCL_OK )
			{
				ret = TCL_ERROR;
				break;
			}

			/* TODO? if list object: options */
			childName = Tcl_GetString ( tp );

			/* empty widget-ID: skip this row/column */
			if ( *childName )
			{
				childWidget = gnoclChildNotPacked ( childName, interp );

				if ( childWidget == NULL )
				{
					ret = TCL_ERROR;
					break;
				}

				if ( ao.useAlign )
				{
					GtkWidget *alignment = gtk_alignment_new ( ao.xAlign, ao.yAlign,
										   ao.xScale, ao.yScale );
					/* alignment is deleted on deletion of childWidget
					   only necessary, if not whole box is destroyed */
					g_signal_connect ( G_OBJECT ( childWidget ), "destroy",
									   G_CALLBACK ( alignDestroyFunc ), alignment );

					gtk_container_add ( GTK_CONTAINER ( alignment ), childWidget );
					gtk_widget_show ( alignment );
					childWidget = alignment;
				}

				gtk_table_attach ( table, childWidget,

								   column, column + ao.columnSpan,
								   row, row + ao.rowSpan,
								   ao.xOptions, ao.yOptions,
								   ao.xPad, ao.yPad );
			}

			if ( addRowFlag )
				column += ao.columnSpan;
			else
				row += ao.rowSpan;
		}
	}

	return ret;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int getIntOrEnd ( Tcl_Interp *interp, GtkTable *table,
						 Tcl_Obj *obj, int getCol )
{
	int        ret;
	int        offset = 0;
	const char *txt;

	if ( Tcl_GetIntFromObj ( interp, obj, &ret ) == TCL_OK && ret >= 0 )
		return ret;

	txt = Tcl_GetString ( obj );

	if ( strncmp ( txt, "end", 3 ) )
	{
		Tcl_SetResult ( interp,
						"Index must be an integer > 0 or \"end[+-offset]\"", TCL_STATIC );
		return -1;
	}

	if ( gnoclPosOffset ( interp, txt + 3, &offset ) != TCL_OK )
		return TCL_ERROR;

	ret = getMaxRowCol ( table, getCol ) + offset;

	if ( ret < 0 )
	{
		Tcl_SetResult ( interp, "Offset is greater than end", TCL_STATIC );
		return -1;
	}

	return ret;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int tableFuncAdd ( GtkTable *table, Tcl_Interp *interp,
						  int objc, Tcl_Obj * const objv[] )
{
	int row, column;
	char *childName;
	GtkWidget *childWidget;
	AttachOptions ao;
	/* default is expand, fill and shrink */
	int ret = TCL_ERROR;

	if ( objc < 5 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv,
						   "add widget column row ?option val ...?" );
		return TCL_ERROR;
	}

	childName = Tcl_GetString ( objv[2] );

	childWidget = gnoclGetWidgetFromName ( childName, interp );

	if ( childWidget == NULL )
		return TCL_ERROR;

	column = getIntOrEnd ( interp, table, objv[3], 0 );

	if ( column < 0 )
		return TCL_ERROR;

	row = getIntOrEnd ( interp, table, objv[4], 1 );

	if ( row < 0 )
		return TCL_ERROR;

	/* packOptions to skip startIdx */
	if ( gnoclParseOptions ( interp, objc - 4, objv + 4, packOptions + 1 )
			== TCL_OK )
		ret = parsePackOptions ( interp, packOptions, &ao );

	if ( ret == TCL_OK )
	{
		if ( ao.useAlign )
		{
			GtkWidget *alignment = gtk_alignment_new ( ao.xAlign, ao.yAlign,
								   ao.xScale, ao.yScale );

			/* alignment is deleted on deletion of childWidget
			   only necessary, if not whole box is destroyed */
			g_signal_connect ( G_OBJECT ( childWidget ), "destroy",
							   G_CALLBACK ( alignDestroyFunc ), alignment );

			gtk_container_add ( GTK_CONTAINER ( alignment ), childWidget );
			gtk_widget_show ( alignment );
			childWidget = alignment;
		}

		gtk_table_attach ( table, childWidget,

						   column, column + ao.columnSpan,
						   row, row + ao.rowSpan,
						   ao.xOptions, ao.yOptions,
						   ao.xPad, ao.yPad );
	}

	return ret;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
static int tableFunc ( ClientData data, Tcl_Interp *interp,
					   int objc, Tcl_Obj * const objv[] )
{
	const char *cmds[] = { "delete", "configure", "add", "addRow", "addColumn", "class", NULL };
	enum cmdIdx { DeleteIdx, ConfigureIdx, AddIdx, AddRowIdx, AddColumnIdx, ClassIdx };

	int idx;

	GtkWidget *widget = GTK_WIDGET ( data );
	GtkFrame *frame;
	GtkTable  *table;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( GTK_IS_FRAME ( widget ) )
	{
		frame = GTK_FRAME ( widget );
		table = GTK_TABLE ( gtk_bin_get_child ( GTK_BIN ( frame ) ) );
	}

	else
	{
		frame = NULL;
		table = GTK_TABLE ( widget );
	}

	/* For debugging purposes
	{
	   guint rows, columns;

	   g_object_get( G_OBJECT( table ), "n_rows", &rows, NULL );
	   g_object_get( G_OBJECT( table ), "n_columns", &columns, NULL );
	   printf( "rows: %d cols: %d\n", (int)rows, (int)columns );
	}
	*/

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command",
							   TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

	switch ( idx )
	{
		case ClassIdx:
			Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "table", -1 ) );
			break;
		case DeleteIdx:
			return gnoclDelete ( interp, widget, objc, objv );

		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseOptions ( interp, objc - 1, objv + 1,
										 tableOptions ) == TCL_OK )
				{
					ret = configure ( interp, frame, table, tableOptions );
				}

				gnoclClearOptions ( tableOptions );

				return ret;
			}

			break;
		case AddIdx:
			return tableFuncAdd ( table, interp, objc, objv );
		case AddColumnIdx:
		case AddRowIdx:
			return addRowCol ( table, interp, objc, objv, idx == AddRowIdx );
	}

	return TCL_OK;
}

/**
/brief
/author     Peter G Baum, William J Giddings
**/
int gnoclTableCmd ( ClientData data, Tcl_Interp *interp,
					int objc, Tcl_Obj * const objv[] )
{
	int            ret = TCL_OK;
	GtkTable       *table;
	GtkFrame       *frame = NULL;
	GtkWidget      *widget;

	assert ( strcmp ( tableOptions[startFrameOpts].optName, "-label" ) == 0 );
	assert ( strcmp ( tableOptions[startCommonOpts].optName, "-name" ) == 0 );

	if ( gnoclParseOptions ( interp, objc, objv, tableOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( tableOptions );
		return TCL_ERROR;
	}

	table = GTK_TABLE ( gtk_table_new ( 1, 1, 0 ) );

	if ( needFrame ( tableOptions ) )
	{
		frame = GTK_FRAME ( gtk_frame_new ( NULL ) );
		gtk_container_add ( GTK_CONTAINER ( frame ), GTK_WIDGET ( table ) );
		widget = GTK_WIDGET ( frame );
	}

	else
		widget = GTK_WIDGET ( table );

	/* set default values */
	gtk_table_set_row_spacings ( table, GNOCL_PAD_TINY );

	gtk_table_set_col_spacings ( table, GNOCL_PAD );

	gtk_container_set_border_width ( GTK_CONTAINER ( table ), GNOCL_PAD_TINY );

	ret = configure ( interp, frame, table, tableOptions );

	gnoclClearOptions ( tableOptions );

	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( widget );
		return TCL_ERROR;
	}

	gtk_widget_show_all ( widget );

	return gnoclRegisterWidget ( interp, widget, tableFunc );
}


