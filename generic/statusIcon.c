/**
\brief     This module implements the gnocl::statusIcon widget.
\author    William J Giddings
\date      13/03/09
*/

/* user documentation */

/**
\page page_statusIcon gnocl::statusIcon
\section sec gnocl::statusIcon
    Implementation of gnocl::statusIcon
\subsection subsection1 Implemented Options
    \verbinclude statusIcon_options.txt
\subsection subsection2 Implemented Commands
    \verbinclude statusIcon_commands.txt
\subsection subsection3 Sample Tcl Script
    \include statusIcon_example.tcl
\subsection subsection4 Produces
    \image html statusIcon.png
\subpage MySubPage
*/

#include "gnocl.h"
#include <assert.h>


static GHashTable *name2StatusIconList;
static const char idPrefix[] = "::gnocl::_STATICON";

static int StatusIcon_icon ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int StatusIcon_tooltip ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
static int StatusIcon_on_menu ( GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data );
static int StatusIcon_on_click ( GtkStatusIcon *status_icon, gpointer user_data );
static int configure ( Tcl_Interp *interp, GtkStatusIcon *StatusIcon, GnoclOption options[] );

/**
 * \brief   options for the object
 * \author  William J Giddings
 * \date    13/03/09
 */
static GnoclOption statusIconOptions[] =
{
	/* propeties, these now work */
	{"-blink" ,  GNOCL_BOOL, "blinking" },
	{"-icon",  GNOCL_OBJ, "", StatusIcon_icon },
	{"-tooltip", GNOCL_OBJ, "", StatusIcon_tooltip },

	/* GtkObject signals, note cannot use  gnoclOptCommand */
	{ "-onActivate", GNOCL_OBJ, "activate", gnoclOptCommand },
	{ "-onPopupMenu", GNOCL_OBJ, "popup-menu", gnoclOptPopupMenu },
	{ "-onSizeChange", GNOCL_OBJ, "size-changed", gnoclOptCommand },

	{ NULL },
};


/**
 * \brief   Configure the object using parameters passed from script.
 * \author  William J Giddings
 * \date    13/03/09
 * \param   interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
 */
static int configure (
	Tcl_Interp *interp,
	GtkStatusIcon *StatusIcon,
	GnoclOption options[]  )
{
	g_print ( "configure \n" );




	int i;

	for ( i = 0 ; i <= 4 ; i++ )
	{
		g_print ( "options  %d %s\n", i, options[i] );
	}

	return TCL_OK;
}


/**
\brief     Use name to retreive pointer to the statusIcon from hashtable name2StatusIconList
\author    William J Giddings
\date      13/03/09
\param     id       name of statusIcon object
\param     interp   pointer to a Tcl_Interp structure, client visible fields of interpreter structure
*/
GtkStatusIcon *gnoclStatusIconFromName (
	const char *id,
	Tcl_Interp *interp )
{
	GtkStatusIcon *statusIcon = NULL;
	int       n;

	if ( strncmp ( id, idPrefix, sizeof ( idPrefix ) - 1 ) == 0
			&& ( n = atoi ( id + sizeof ( idPrefix ) - 1 ) ) > 0 )
	{
		statusIcon = g_hash_table_lookup ( name2StatusIconList, GINT_TO_POINTER ( n ) );
	}

	if ( statusIcon == NULL && interp != NULL )
		Tcl_AppendResult ( interp, "Unknown Status Icon \"", id, "\".", ( char * ) NULL );

	return statusIcon;
}

/**
\brief      function called when Gnocl object invoked from Tcl script
\author     William J Giddings
\date       13/03/09
\param      interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
*/
static int statusIconFunc (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	g_print ( "statusIconFunc\n" );

	static const char *cmds[] =
	{
		"isEmbedded", "configure", "cget", "geometry", "orientation", "class",  NULL
	};

	enum cmdIdx
	{
		IsEmbeddedIdx, ConfigureIdx, CgetIdx, GeometryIdx, OrientationIdx, ClassIdx
	};

	GtkStatusIcon *statusIcon = GTK_STATUS_ICON ( data );
	int idx;

	if ( objc < 2 )
	{
		g_print ( "statusIconFunc -1\n" );
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		g_print ( "statusIconFunc -2\n" );
		return TCL_ERROR;
	}


	switch ( idx )
	{
		case OrientationIdx:
			{
				g_print ( "statusIconFunc -3\n" );

				char str[128];
				GtkOrientation orientation ;
				/*
				typedef struct {
				  gint x;
				  gint y;
				  gint width;
				  gint height;
				} GdkRectangle;

				typedef enum
				{
				  GTK_ORIENTATION_HORIZONTAL,
				  GTK_ORIENTATION_VERTICAL
				} GtkOrientation;
				*/
				gtk_status_icon_get_geometry (
					GTK_STATUS_ICON ( data ),
					NULL,
					NULL,
					&orientation );

				if ( orientation )
				{
					sprintf ( str, "%s", "vertical" );
				}

				else
				{
					sprintf ( str, "%s", "horizontal" );
				}

				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
			}

			break;
		case GeometryIdx:
			{
				g_print ( "statusIconFunc -3\n" );

				char str[128];
				GdkRectangle rect;
				GtkOrientation orientation ;
				/*
				typedef struct {
				  gint x;
				  gint y;
				  gint width;
				  gint height;
				} GdkRectangle;

				typedef enum
				{
				  GTK_ORIENTATION_HORIZONTAL,
				  GTK_ORIENTATION_VERTICAL
				} GtkOrientation;
				*/
				gtk_status_icon_get_geometry (
					GTK_STATUS_ICON ( data ),
					NULL,
					&rect,
					&orientation );


				sprintf ( str, "%d %d %d %d", rect.x, rect.y, rect.width, rect.height );

				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( &str, -1 ) );
			}

			break;
		case CgetIdx:
			{
				g_print ( "statusIconFunc -4\n" );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "DOING cget", -1 ) );
			}

			break;
		case ConfigureIdx:
			{
				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1, statusIconOptions, G_OBJECT ( statusIcon ) ) == TCL_OK )
				{
					ret = configure ( interp, statusIcon, statusIconOptions );
				}

				gnoclClearOptions ( statusIconOptions );

				return ret;
			}

			break;

		case ClassIdx:
			{
				g_print ( "statusIconFunc -6\n" );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( "statusIcon", -1 ) );
			}

			break;
	}

	g_print ( "statusIconFunc -7\n" );

	return TCL_OK;
}

/**
\brief      Implement tooltip functionality
\author     William J Giddings
\date       13/03/09
\param      interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
*/
static int StatusIcon_tooltip (
	Tcl_Interp *interp,
	GnoclOption *opt,
	GObject *obj,
	Tcl_Obj **ret )
{
	g_print ( "StatusIcon_tooltip\n" );
	assert ( strcmp ( opt->optName, "-tooltip" ) == 0 );
	const char *txt = Tcl_GetString ( opt->val.obj );
	gtk_status_icon_set_tooltip ( GTK_STATUS_ICON ( obj ), txt );
	return TCL_OK;
}

/**
 * \brief   implment icon blink functionality
 * \author  William J Giddings
 * \date    13/03/09
 * \param   interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
 */
static int StatusIcon_blink (
	Tcl_Interp *interp,
	GnoclOption *opt,
	GObject *obj,
	Tcl_Obj **ret )
{
	g_print ( "StatusIcon_blink\n" );
	assert ( strcmp ( opt->optName, "-blink" ) == 0 );
	const char *txt = Tcl_GetString ( opt->val.obj );
	gtk_status_icon_set_blinking ( GTK_STATUS_ICON ( obj ), 1 );
	return TCL_OK;
}

/**
 * \brief   Set the object icon
 * \author  William J Giddings
 * \date    13/03/09
 * \param   interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
 */
static int StatusIcon_icon (
	Tcl_Interp *interp,
	GnoclOption *opt,
	GObject *obj,
	Tcl_Obj **ret )
{
	g_print ( "StatusIcon_icon\n" );
	const char *txt = Tcl_GetString ( opt->val.obj );
	/* set a default value */
	gtk_status_icon_set_from_icon_name ( GTK_STATUS_ICON ( obj ), GTK_STOCK_MEDIA_STOP );
	gtk_status_icon_set_from_file      ( GTK_STATUS_ICON ( obj ), txt );

	return TCL_OK;
}




/**
 * \brief   Code strub to handle effectice But1 click on icon
 * \author  William J Giddings
 * \date    13/03/09
 */
static int StatusIcon_on_click (
	GtkStatusIcon *status_icon,
	gpointer user_data )
{
	printf ( "Clicked on tray icon\n" );
	return TCL_OK;

}

/**
 * \brief   Respond to chang in icon size
 * \author  William J Giddings
 * \date    13/03/09
 */
static int StatusIcon_on_sizeChange (
	GtkStatusIcon *status_icon,
	gint size,
	gpointer user_data )
{
	printf ( "Tray icon size-change\n" );
	return TCL_OK;
}

/**
 * \brief   code stub to handl B3 click
 * \author  William J Giddings
 * \date    13/03/09
 */
static int StatusIcon_on_menu (
	GtkStatusIcon *status_icon,
	guint button,
	guint activate_time,
	gpointer user_data )
{
	g_print ( "Popup menu\n" );
	return TCL_OK;
}



/**
 * \brief   generate id for the new SI object
 * \author  William J Giddings
 * \date    13/03/09
 */
static char *gnoclGetAutoStatusIcontId ( void )
{
	static int no = 0;
	/*
	static char buffer[30];
	*/

	char *buffer = g_new ( char, sizeof ( idPrefix ) + 15 );
	strcpy ( buffer, idPrefix );

	/* with namespace, since the Id is also the widget command */
	sprintf ( buffer + sizeof ( idPrefix ) - 1, "%d", ++no );

	return buffer;
}

/**
 * \brief   ???????
 * \author  William J Giddings
 * \date    13/03/09
 */
static int gnoclMemNameAndStatusIcon (
	const char *path,
	GtkStatusIcon *status_icon )
{
	int n = atoi ( path + sizeof ( idPrefix ) - 1 );

	assert ( n > 0 );
	assert ( g_hash_table_lookup ( name2StatusIconList, GINT_TO_POINTER ( n ) )
			 == NULL );
	assert ( strncmp ( path, idPrefix, sizeof ( idPrefix ) - 1 ) == 0 );

	/* memorize the name of the status_icon in the status_icon */
	g_object_set_data ( G_OBJECT ( status_icon ), "gnocl::name", ( char * ) path );
	g_hash_table_insert ( name2StatusIconList, GINT_TO_POINTER ( n ), status_icon );

	return 0;
}

/**
 * \brief   register the new object with the Tcl interperter
 * \author  William J Giddings
 * \date    13/03/09
 * \param   interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
 */
static int registerStatusIcon (
	Tcl_Interp *interp,
	GtkStatusIcon *status_icon,
	Tcl_ObjCmdProc *proc )
{
	const char *name = gnoclGetAutoStatusIcontId();

	// const char *name = gnoclGetAutoWidgetId();
	gnoclMemNameAndStatusIcon ( name, status_icon );

	/*
	    g_signal_connect_after ( G_OBJECT ( status_icon ), "destroy",
	                             G_CALLBACK ( simpleDestroyFunc ), interp );
	*/

	if ( proc != NULL )
	{
		Tcl_CreateObjCommand ( interp, ( char * ) name, proc, status_icon, NULL );
	}

	Tcl_SetObjResult ( interp, Tcl_NewStringObj ( name, -1 ) );

	return TCL_OK;
}

/**
 * \brief   Display an icon in the system tray, i.e the notification area panel applet
 * \author  William J Giddings
 * \date    13/03/09
 * \param   interp  pointer to a Tcl_Interp structure, client visible fields of interpreter structure
 */
int gnoclStatusIconCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{

	g_print ( "gnoclStatusIconCmd\n" );

	int       ret;
	GtkStatusIcon *StatusIcon;

	/* StatusIcon = create_StatusIcon(); */


	/* check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, statusIconOptions ) != TCL_OK )
	{
		gnoclClearOptions ( statusIconOptions );
		return TCL_ERROR;
	}


	StatusIcon = gtk_status_icon_new();

	gtk_status_icon_set_visible ( StatusIcon, TRUE );

	if ( 1 )
	{


		ret = gnoclSetOptions ( interp, statusIconOptions, G_OBJECT ( StatusIcon ), -1 );
		//gtk_status_icon_set_from_icon_name ( StatusIcon, GTK_STOCK_MEDIA_STOP );


		if ( ret == TCL_OK )
		{
			g_print ( "HERE\n" );
			// ret = configure ( interp, button, buttonOptions );
			ret = configure ( interp, StatusIcon, statusIconOptions );
		}

		g_print ( "THERE\n" );

		gnoclClearOptions ( statusIconOptions );
	}

	else
	{
		gtk_status_icon_set_from_icon_name ( StatusIcon, GTK_STOCK_MEDIA_STOP );
		g_signal_connect ( G_OBJECT ( StatusIcon ), "activate", G_CALLBACK ( StatusIcon_on_click ), NULL );
		g_signal_connect ( G_OBJECT ( StatusIcon ), "popup-menu", G_CALLBACK ( StatusIcon_on_menu ), NULL );
		gtk_status_icon_set_tooltip ( StatusIcon, "Example Tray Icon" );
	}

	/* create the hash table, the widget equivalent is created in Gnocl_Init() */
	name2StatusIconList = g_hash_table_new ( g_direct_hash, g_direct_equal );

	return registerStatusIcon ( interp, StatusIcon, statusIconFunc );

}

