/**
\brief      This module implements the gnocl::assistant widget.
\author     William J Giddings
\date       07-Apr-2009:
\note       Working on -linear option. It can be configured a start, but runtime reconfig not working yet.

 g_signal_emit(assistant, go_forward, 0, NULL);


GTK_ASSISTANT_PAGE_CONTENT
    The page has regular contents.

GTK_ASSISTANT_PAGE_INTRO
    The page contains an introduction to the assistant task.

GTK_ASSISTANT_PAGE_CONFIRM
    The page lets the user confirm or deny the changes.

GTK_ASSISTANT_PAGE_SUMMARY
    The page informs the user of the changes done.

GTK_ASSISTANT_PAGE_PROGRESS
    Used for tasks that take a long time to complete, blocks the assistant until the page is marked as complete.

Usage
1) create the widget
2) add page
3) populate pages with widgets


inherited window properties
    -x
    -y
    -widthRequest
    -widthHeight
    -title
    -onDelete
    -onDestroy

page properties
    -complete
        type: boolean
        Sets whether page contents are complete. This will make assistant
        update the buttons state to be able to continue the task.

    -headerImage
        type:
        Sets a header image for page. This image is displayed in the
        header area of the assistant when page is the current page.

    -sideImage
        type:
        Sets a header image for page. This image is displayed in the side
        area of the assistant when page is the current page.

    -type
        type: string
        An enum for determining the page role inside the GtkAssistant.
        It's used to handle buttons sensitivity and visibility.
        One of content, intro, confirm, summary or progress.

    -title
        type: string
        Sets a title for page. The title is displayed in the header area
        of the assistant when page is the current page.

signals
    -onApply
    -onCancel
    -onClose
    -onPrepare

commands
    <widgetid> insert
    $wid addPage $type $title ??options...
    page append
    page prepend

*/

/* user documentation */

/**
\page page_assistant gnocl::assistant
\section sec DESCRIPTION
    Implementation of gnocl::assistant
\subsection subsection1 Implemented gnocl::assistant Options
    \verbinclude assistant_options.txt
\subsection subsection2 Implemented gnocl::assistant Commands
    \verbinclude assistant_commands.txt
\subsection subsection3 Sample Tcl Script
    \include assistant_example.tcl
\subsection subsection4 Produces
    \image html assistant.png
**/

/*
 \par Modification History
 \verbatim
 2009-04: Began developement
 \endverbatim

 \todo  assistant - BLAH, BLAH
*/

#include "gnocl.h"
#include <string.h>
#include <assert.h>

/* temporary idx vars */
static const int decoratedIdx   = 2;
static const int visibleIdx     = 3;
static const int xIdx           = 4;
static const int yIdx           = 5;
static const int widthIdx       = 6;
static const int heightIdx      = 7;
/**
\brief
\author     William J Giddings
\date       20/01/2010
**/
static int cget (   Tcl_Interp *interp, GtkWindow *window,  GnoclOption options[], int idx )
{
	Tcl_Obj *obj = NULL;

	if ( idx == xIdx )
	{
		int x, y;
		gtk_window_get_position ( window, &x, &y );
		obj = Tcl_NewIntObj ( x );
	}

	else if ( idx == yIdx )
	{
		int x, y;
		gtk_window_get_position ( window, &x, &y );
		obj = Tcl_NewIntObj ( y );
	}

	else if ( idx == widthIdx )
	{
		int width, height;
		gtk_window_get_size ( window, &width, &height );
		obj = Tcl_NewIntObj ( width );
	}

	else if ( idx == heightIdx )
	{
		int width, height;
		gtk_window_get_size ( window, &width, &height );
		obj = Tcl_NewIntObj ( height );
	}

	if ( obj != NULL )
	{
		Tcl_SetObjResult ( interp, obj );
		return TCL_OK;
	}

	return gnoclCgetNotImplemented ( interp, options + idx );
}

/**
\brief
\author     William J Giddings
\date       08/05/09
\bug        For some reason, %p returns an emptry string.
\note       Developed from ~~/gtk+-2.12.0/tests/testassistant.c
**/

static gchar selected_branch = 'A';

static gint forward_page ( gint current_page, gpointer data )
{
	g_print ( "doOnForward %d\n", current_page );

	return 3;

}

/**
\brief
\author     William J Giddings
\date       08/05/09
\bug        For some reason, %p returns an emptry string.
**/
static int doOnPrepare ( GtkAssistant *assistant, GtkWidget *page, gpointer user_data )
{

	GnoclCommandData *cs = ( GnoclCommandData * ) user_data;

	GtkWidget *child =  gnoclFindChild ( GTK_WIDGET ( page ), GTK_TYPE_CONTAINER );

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* assistant */
		{ 'n', GNOCL_INT },     /* page number */
		{ 'p', GNOCL_STRING },  /* page child widget */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( assistant );
	ps[1].val.i   = gtk_assistant_get_current_page ( assistant );
	ps[2].val.str = gnoclGetNameFromWidget ( child );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}

/**
\brief
\author     William J Giddings
\date       08/05/09
\bug        For some reason, %p returns an emptry string.
**/
static int doOnAssistantClose ( GtkAssistant *assistant, gpointer user_data )
{

	GnoclCommandData *cs = ( GnoclCommandData * ) user_data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* assistant */
		{ 'n', GNOCL_INT },     /* page number */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( assistant );
	ps[1].val.i   = gtk_assistant_get_current_page ( assistant );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}

/**
\brief
\author     William J Giddings
\date       08/05/09
\bug        For some reason, %p returns an emptry string.
**/
static int doOnAssistantCancel ( GtkAssistant *assistant, gpointer user_data )
{

	GnoclCommandData *cs = ( GnoclCommandData * ) user_data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* assistant */
		{ 'n', GNOCL_INT },     /* page number */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( assistant );
	ps[1].val.i   = gtk_assistant_get_current_page ( assistant );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}

/**
\brief
\author     William J Giddings
\date       08/05/09
\bug        For some reason, %p returns an emptry string.
**/
static int doOnAssistantApply ( GtkAssistant *assistant, gpointer user_data )
{

	GnoclCommandData *cs = ( GnoclCommandData * ) user_data;

	GnoclPercSubst ps[] =
	{
		{ 'w', GNOCL_STRING },  /* assistant */
		{ 'n', GNOCL_INT },     /* page number */
		{ 0 }
	};

	ps[0].val.str = gnoclGetNameFromWidget ( assistant );
	ps[1].val.i   = gtk_assistant_get_current_page ( assistant );

	gnoclPercentSubstAndEval ( cs->interp, ps, cs->command, 1 );
}
/**
\brief      Respond to a click on either the Apply or Forward buttons on any page.
\note       http://library.gnome.org/devel/gtk/2.14/GtkAssistant.html#GtkAssistant-apply
            A handler for the ::apply signal should carry out the actions for which
            the wizard has collected data. If the action takes a long time to complete,
            you might consider to put a page of type GTK_ASSISTANT_PAGE_PROGRESS
            after the confirmation page and handle this operation within
            the ::prepare signal of the progress page.
*/
static int gnoclOptOnApply  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
#ifdef DEFINE_ASSISTANT
	g_print ( "gnoclOptOnApply\n" );
#endif

	/* check the name of the signal is correct for this function */
	assert ( strcmp ( opt->optName, "-onApply" ) == 0 );

	/* connect the signal with its callback function */
	return gnoclConnectOptCmd ( interp, GTK_ASSISTANT ( obj ) , "apply", G_CALLBACK ( doOnAssistantApply ), opt, NULL, ret );

}

/**
\brief      Respond to a click on page Cancel button on any page.
\author     William J Giddings
\date       06/05/09
**/

static int gnoclOptOnCancel  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
#ifdef DEFINE_ASSISTANT
	g_print ( "gnoclOptOnCancel\n" );
#endif

	/* check the name of the signal is correct for this function */
	assert ( strcmp ( opt->optName, "-onCancel" ) == 0 );

	/* connect the signal with its callback function */
	return gnoclConnectOptCmd ( interp, GTK_ASSISTANT ( obj ) , "cancel", G_CALLBACK ( doOnAssistantCancel ), opt, NULL, ret );

}

/**
\brief
    Respond to a click on either the Close or Apply buttons on the final page.
*/
static int gnoclOptOnClose  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
#ifdef DEFINE_ASSISTANT
	g_print ( "gnoclOptOnClose\n" );
#endif
	/* check the name of the signal is correct for this function */
	assert ( strcmp ( opt->optName, "-onClose" ) == 0 );

	/* connect the signal with its callback function */
	return gnoclConnectOptCmd ( interp, GTK_ASSISTANT ( obj ) , "close", G_CALLBACK ( doOnAssistantClose ), opt, NULL, ret );
}

/**
\brief
    Script to execute prior to displaying the specified page.
*/
static int gnoclOptOnPrepare ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
#ifdef DEFINE_ASSISTANT
	g_print ( "gnoclOptOnPrepare opt->val.b %d\n", opt->val.b );
#endif

	/* check the name of the signal is correct for this function */
	assert ( strcmp ( opt->optName, "-onPrepare" ) == 0 );

	/* connect the signal with its callback function */
	return gnoclConnectOptCmd ( interp, GTK_ASSISTANT ( obj ) , "prepare", G_CALLBACK ( doOnPrepare ), opt, NULL, ret );
};

/**
\brief
    Script to execute prior to displaying the specified page.
*/
static int gnoclOptLinear ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
#ifdef DEFINE_ASSISTANT
	g_print ( "gnoclOptLinear opt->val.b %d\n", opt->val.b );
#endif

	/* check the name of the signal is correct for this function */
	assert ( strcmp ( opt->optName, "-linear" ) == 0 );

	/* connect the signal with its callback function */
	return gnoclConnectOptCmd ( interp, GTK_ASSISTANT ( obj ) , "prepare", G_CALLBACK ( doOnPrepare ), opt, NULL, ret );
};

/**
 */

/* provide non-linear hanlding of the forward button click, default = 1, ie. linear */
static const int linearIdx = 0;

static GnoclOption assistantOptions[] =
{

	/* gnocl widget specific options */
	{ "-linear", GNOCL_BOOL, NULL },    /* 0 */

	/* GtkWindow inherited properties */
	{ "-defaultWidth", GNOCL_INT, "default-width" },     /* these must be */
	{ "-defaultHeight", GNOCL_INT, "default-height" },   /* before -visible! */
	{ "-heightRequest", GNOCL_INT, "height-request" },
	{ "-widthRequest", GNOCL_INT, "width-request" },
	{ "-title", GNOCL_STRING, "title" },
	{ "-resizable", GNOCL_BOOL, "resizable" },
	{ "-transient", GNOCL_OBJ, "", gnoclOptTransientWindow },
	{ "-keepAbove", GNOCL_OBJ, "", gnoclOptKeepAbove },
	{ "-icon", GNOCL_OBJ, "", gnoclOptIcon },
	{ "-backgroundImage", GNOCL_OBJ, "", gnoclOptBackgroundImage },
	{ "-modal", GNOCL_BOOL, "modal" },
	{ "-mask", GNOCL_OBJ, "", gnoclOptMask },
	{ "-backgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },

	/* GtkWindow inherited signals */
	{ "-onDelete", GNOCL_OBJ, "", gnoclOptOnDelete },
	{ "-onDestroy", GNOCL_OBJ, "destroy", gnoclOptCommand },

	/* GtkAssistant signals */
	{ "-onApply", GNOCL_OBJ, "", gnoclOptOnApply } ,
	{ "-onCancel", GNOCL_OBJ, "", gnoclOptOnCancel },
	{ "-onClose", GNOCL_OBJ, "", gnoclOptOnClose } ,
	{ "-onPrepare", GNOCL_OBJ, "", gnoclOptOnPrepare },

	/* GtkObject Properties */
	{ "-data", GNOCL_OBJ, "", gnoclOptData },
	{ "-name", GNOCL_STRING, "name" },
	{ "-normalBackgroundColor", GNOCL_OBJ, "normal", gnoclOptGdkColorBg },


	{ NULL }
};

static const int forwardIdx = 0;



/**
\brief      Parse and set assistant page options
\author     William J Giddings
\date       05/05/09
\param      Tcl_Interp *interp
\return
\note       based upon gnoclSetOptions
**/
static GdkPixbuf *load_pixbuf ( gchar *filename )
{
	GdkPixbuf *pbuf;
	GError *err = NULL;

	g_return_val_if_fail ( filename != NULL, NULL );

	pbuf = gdk_pixbuf_new_from_file ( filename, &err );

	if ( err != NULL )
	{
		g_warning ( "%s", err->message );
		g_error_free ( err );
		return NULL;
	}

	return pbuf;
}




/**
\brief      Configure individual pages of the assistant.
\author     William J Giddings
\date       05/05/09
**/
/*
    GnoclOption pageOptions[] =
    {
        { "-child", GNOCL_OBJ, "",SetPageChild },
        { "-complete",  GNOCL_BOOL, "complete" },
        { "-headerImage",  GNOCL_STRING, "header-image" },
        { "-pageType",  GNOCL_INT, "page-type" },
        { "-sidebarImage",  GNOCL_STRING, "side-image" },
        { "-headerImage",  GNOCL_STRING, "header-image" },
        { "-title", GNOCL_STRING, "title"},
        { NULL }
    };

    See parseOptions.c for more ideas.

*/
/**
\brief      Configure individual pages of the assistant.
\author     William J Giddings
\date       18/05/09
**/
/*
    if ( options[stepIncIdx].status == GNOCL_STATUS_CHANGED )
    {
        assert ( strcmp ( options[stepIncIdx].optName, "-stepInc" ) == 0 );
        adjust->step_increment = options[stepIncIdx].val.d;
        setAdjust = 1;
    }
*/
static int configure ( Tcl_Interp *interp, GtkAssistant *assistant, GnoclOption options[], gint page_num )
{
#ifdef DEFINE_ASSISTANT
	g_printf ( "assistant:configure\n\tPAGENUM = %d\n", page_num );
#endif

	/* check to see if the option value has changed */

	if ( options[linearIdx].status == GNOCL_STATUS_CHANGED )
	{

		assert ( strcmp ( options[linearIdx].optName, "-linear" ) == 0 );

#ifdef DEFINE_ASSISTANT
		/* the seeting is type GNOCL_BOOL. so access the value using */
		g_print ( "checking -linear option = %d\n", options[linearIdx].val.b );
#endif

		if ( options[linearIdx].val.b )
		{
#ifdef DEFINE_ASSISTANT
			g_printf ( "LINEAR\n" );
#endif
			/* restore linear option funcitonality of the forward button, default state*/
			gtk_assistant_set_forward_page_func ( GTK_ASSISTANT ( assistant ), NULL, NULL, NULL );
		}

		else
		{
#ifdef DEFINE_ASSISTANT
			g_printf ( "NON-LINEAR\n" );
#endif
			/* divert response to custom handler */
			gtk_assistant_set_forward_page_func ( GTK_ASSISTANT ( assistant ), forward_page, NULL, NULL );
		}

	}


	GtkWidget *page = gtk_assistant_get_nth_page ( assistant, page_num );

	//int gnoclSetOptions ( Tcl_Interp *interp, GnoclOption *opts, GObject *obj, int no )
	gint ret = gnoclSetOptions ( interp, options, assistant, -1 );

	return TCL_OK;

}


/**
\brief
\author     William J Giddings
\date       05/05/09
\param      GtkAssistant *assistant,
\param      Tcl_Interp *interp,
\param      int objc,
\param      Tcl_Obj *const objv[],
\param      int cmdNo   minimum number of parameters required for a valid command
\return
**/
static int pageCmd ( GtkAssistant *assistant, Tcl_Interp *interp, int objc, Tcl_Obj *  const objv[], int cmdNo )
{

	int idx;
	const char *cmds[] = { "cget", "create", "configure", "apply", "delete", "remove", "set", NULL };
	enum cmdIdx { CgetIdx, CreateIdx, ConfigureIdx, ApplyIdx, DeleteIdx, RemoveIdx, SetIdx, };

	int idx2;
	const char *opts[] = { "-child", "-headerImage", "-sidebarImage", "-pageType", "-title", NULL};
	enum optsIdx { ChildIdx, HeaderImageIdx, SidebarImageIdx, PageTypeIdx, TitleIdx };

	/* test the number of argumetnts passed */

	if ( objc < cmdNo + 1 )
	{
		Tcl_WrongNumArgs ( interp, cmdNo, objv, "subcommand ? option val ... ? " );
		return TCL_ERROR;
	}

	//~ if ( strcmp ( Tcl_GetString ( objv[cmdNo] ), "configure" ) == 0 )
	//~ {
	//~ g_print ( "subcommand = % s, page = % s\n", Tcl_GetString ( objv[3] ), Tcl_GetString ( objv[3] ) );
	//~ }

	if ( Tcl_GetIndexFromObj ( interp, objv[2], cmds, "subcommand", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{


		case SetIdx:
			{
				gtk_assistant_set_current_page    ( assistant, atoi ( Tcl_GetString ( objv[3] ) ) ) ;
				return TCL_OK;
			}

			/* currently only working on page configurations */
		case ConfigureIdx:
			{

				/* get a handle on the page to modify based upon the default child widget */
				GtkWidget *page = gtk_assistant_get_nth_page ( assistant, atoi ( Tcl_GetString ( objv[3] ) ) ) ;

				int i;
				/* parse the list of arguments options passed from the interpreter */

				for ( i = cmdNo + 2 ; i < objc ; i += 2 )
				{

					if ( getIdx ( opts, Tcl_GetString ( objv[i] ), &idx2 ) != 0 )
					{
#ifdef DEFINE_ASSISTANT
						g_print ( "This one is for the assistant widget, not the page\n" );
						//configure ( Tcl_Interp *interp, GtkAssistant *assistant, GnoclOption options[], gint page_num )
						g_print ( "HOUSTON, we have an error!\n" );
						g_print ( "idx2 = % d val = % s\n", idx2, opts[idx2] );
#endif
						//break;
					}

					/* handle the choices */
#ifdef DEFINE_ASSISTANT
					g_print ( "This one is for the page\n" );
#endif

					switch ( idx2 )
					{
						case ChildIdx:
							{
								GtkWidget *widget = gnoclGetWidgetFromName ( Tcl_GetString ( objv[i+1] ), interp  );
								/* pack with expand, fill and no paddings */
								gtk_box_pack_start ( page, widget, 1 , 1, 0 );
								break;
							}

						case HeaderImageIdx:
							{
								GdkPixbuf *pixbuf1 = load_pixbuf ( Tcl_GetString ( objv[i+1] )  );
								gtk_assistant_set_page_header_image ( assistant, page, pixbuf1 );
								break;
							}

						case SidebarImageIdx:
							{
								GdkPixbuf *pixbuf1 = load_pixbuf ( Tcl_GetString ( objv[i+1] )  );
								gtk_assistant_set_page_side_image ( assistant, page, pixbuf1 );
								break;
							}

						case TitleIdx:
							{
								gtk_assistant_set_page_title  ( assistant, page, Tcl_GetString ( objv[i+1] ) );
								break;
							}

						default:
							{
								g_print ( "Sorry, you've missed the point!\n" );
							}

					}

					/* finished handling the parsing of options, so return */

				}

				return TCL_OK;
			}

		case CgetIdx:

			{
				printf ( "Cget not yet implemented\n" );
				break;
			}

			return TCL_OK;
	}

	return TCL_OK;
}


typedef struct
{
	GtkWidget *widget;
	gint index;
	const gchar *title;
	GtkAssistantPageType type;
	gboolean complete;
} PageInfo;

PageInfo page[5] =
{
	{ NULL, -1, "Introduction",           GTK_ASSISTANT_PAGE_INTRO,   TRUE},
	{ NULL, -1, NULL,                     GTK_ASSISTANT_PAGE_CONTENT, FALSE},
	{ NULL, -1, "Click the Check Button", GTK_ASSISTANT_PAGE_CONTENT, FALSE},
	{ NULL, -1, "Click the Button",       GTK_ASSISTANT_PAGE_PROGRESS, FALSE},
	{ NULL, -1, "Confirmation",           GTK_ASSISTANT_PAGE_CONFIRM, TRUE},
};




/* If the dialog is cancelled, delete it from memory and then clean up after
* the Assistant structure. */
static void assistant_cancel ( GtkAssistant *assistant, gpointer data )
{
	gtk_widget_destroy ( GTK_WIDGET ( assistant ) );

}

/* This function is where you would apply the changes and destroy the assistant. */
static void assistant_close ( GtkAssistant *assistant, gpointer data )
{
	g_print ( "You would apply your changes now!\n" );
	gtk_widget_destroy ( GTK_WIDGET ( assistant ) );
}















/**
\brief      Add new page to the assistant.
\author     William J Giddings
\date       05/May/09
\param      <GtkAssistant *assistant> Pointer to the assistance widget to which the page will be added.
\param      <gchar *title> Title of the page added.
\param      <gint *type> Type of page added.
\return     index of page added
**/
static int addPage ( GtkAssistant *assistant, gchar *title, gint type )
{

	gboolean homogeneous = TRUE;
	gint spacing = 5;
	char str[128];
	gint page_num;
	GtkWidget *page;

	/* a) create a container for the page */
	page = gtk_vbox_new ( homogeneous, spacing );

	/* b) add the widget as a page */
	int index = gtk_assistant_append_page ( assistant, page );

	/* c) set the page title */
	gtk_assistant_set_page_title ( assistant, page, title );

	/* d) set the page type */
	gtk_assistant_set_page_type  ( assistant, page, type );

	/* e) specific the page as complete */
	gtk_assistant_set_page_complete ( assistant, page, TRUE );

	gtk_widget_show_all ( assistant );

#ifdef DEBUG_ASSISTANT
	printf ( "addPage :  index of added page = %d\n", index );
#endif

	return index;
}

/**
\brief
\author     William J Giddings
\date       05/05/09
\param      ClientData data,
\param      Tcl_Interp *interp,
\param      int objc,
\param      Tcl_Obj * const objv[]
*/
int assistantFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_ASSISTANT
	g_printf ( "assistantFunc\n" );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif

	static const char *cmds[] =
	{
		"cget", "delete", "page", "configure", "addPage", "insertPage", "currentPage", "pages", "addWidget", "removeWidget", "update", NULL
	};

	enum cmdIdx
	{
		CgetIdx, DeleteIdx, PageIdx, ConfigureIdx, AddPageIdx, InsertPageIdx, CurrentPageIdx, PagesIdx, AddWidgetIdx, RemoveWidgetIdx, UpdateIdx
	};

	GtkAssistant *assistant = GTK_ASSISTANT ( data );
	int idx;

	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command ??options" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case CgetIdx:
			{
				int     idx;

				switch ( gnoclCget ( interp, objc, objv, G_OBJECT ( assistant ),
									 assistantOptions, &idx ) )
				{
					case GNOCL_CGET_ERROR:
						return TCL_ERROR;
					case GNOCL_CGET_HANDLED:
						return TCL_OK;
					case GNOCL_CGET_NOTHANDLED:
						return cget ( interp, assistant, assistantOptions, idx );
				}

				assert ( 0 );
			}

			break;
		case UpdateIdx:
			{
				gtk_assistant_update_buttons_state ( assistant );
				break;
			}

		case RemoveWidgetIdx:
			{
				GtkWidget *widget = gnoclGetWidgetFromName ( Tcl_GetString ( objv[2] ), interp );
				gtk_assistant_remove_action_widget ( assistant, widget );
				break;
			}

		case AddWidgetIdx:
			{
#ifdef DEBUG_ASSISTANT
				g_print ( "args %d = 0 %s 1 %s 2 %s \n", objc, Tcl_GetString ( objv[0] ), Tcl_GetString ( objv[1] ), Tcl_GetString ( objv[2] )  );
#endif
				GtkWidget *widget = gnoclGetWidgetFromName ( Tcl_GetString ( objv[2] ), interp );
				gtk_assistant_add_action_widget ( assistant, widget );
				break;
			}

		case DeleteIdx:
			{
#ifdef DEBUG_ASSISTANT
				g_print ( "assistantFunc\AddIdx %s %s \n",  Tcl_GetString ( objv[2] ),  Tcl_GetString ( objv[3] ) );
#endif
				break;
			}

		case PageIdx:
			{
#ifdef DEBUG_ASSISTANT
				g_printf ( "assistantFunc:PageIdx\n" );
#endif


				if ( pageCmd ( assistant, interp, objc, objv, 2 ) != TCL_OK )
				{
					return -1;
				}

				break;

			}

			/* this block of code is working fine, applies to window elements of the assistant */
		case ConfigureIdx:
			{

				/* test the number of argumetnts passed */
				if ( objc <= 3 )
				{
					Tcl_WrongNumArgs ( interp, 1, objv, "configure <option>" );
					return TCL_ERROR;
				}

#ifdef DEBUG_ASSISTANT
				g_print ( "assistant configure %s %s \n",  Tcl_GetString ( objv[2] ),  Tcl_GetString ( objv[3] ) );
#endif

				int ret = TCL_ERROR;

				if ( gnoclParseAndSetOptions ( interp, objc - 1, objv + 1,
											   assistantOptions, G_OBJECT ( assistant ) ) == TCL_OK )
				{
					/* this line is unecessary here, no specific configuration options necessary */
					//ret = configure ( interp, assistant, assistantOptions );
					ret = TCL_OK;
				}

				gnoclClearOptions ( assistantOptions );

				return ret;

			}

			/* format: widget addPage */
		case AddPageIdx:
			{
#ifdef DEBUG_ASSISTANT
				g_print ( "assistant AddPageIdx -1\n" );
#endif

				GtkWidget *page;
				gint n, idx;

				/*
				GTK_ASSISTANT_PAGE_CONTENT, GTK_ASSISTANT_PAGE_INTRO,
				GTK_ASSISTANT_PAGE_CONFIRM, GTK_ASSISTANT_PAGE_SUMMARY,
				GTK_ASSISTANT_PAGE_PROGRESS
				*/

				/* step 2) determine what page type is to be created, and modify */
				static const char *types[] = { "content", "intro", "confirm", "summary", "progress", NULL };
				enum optIdx { ContentIdx, IntroIdx, ConfirmIdx, SummaryIdx, ProgressIdx };

				if ( Tcl_GetIndexFromObj ( interp, objv[2], types, "option", TCL_EXACT, &idx ) != TCL_OK )
				{
					return TCL_ERROR;
				}

				/* set a default page title */

				gchar *title = "";

				switch ( idx )
				{
					case ContentIdx:
						{
							title = "Assistant Content Page" ;
						} break;
					case IntroIdx:
						{
							title = "Assistant Introduction Page" ;
						} break;
					case ConfirmIdx:
						{
							title = "Assistant Confirmation Page" ;
						} break;
					case SummaryIdx:
						{
							title = "Assistant Summary Page" ;
						} break;
					case ProgressIdx:
						{
							title = "Assistant Progress Page" ;
						} break;
				}

				if ( objc > 2 )
				{

					if ( !strcmp ( "-title" , Tcl_GetString ( objv[3] ) ) )
					{
						title = Tcl_GetString ( objv[4] );
					}
				}

#ifdef DEBUG_ASSISTANT
				g_print ( "assistant AddPageIdx -2\n" );
#endif

				/* create the page, and obtain its index number */
				int pageNo = addPage ( assistant, title , idx );
				pageNo = pageNo + 1;
#ifdef DEBUG_ASSISTANT
				g_print ( "---------> page created index = %d\n", pageNo );
#endif

				char str[8];
				sprintf ( &str, "%d", pageNo );
#ifdef DEBUG_ASSISTANT
				g_print ( "---------> page created index = %s\n", str );
#endif


				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
				break;
			}
		case InsertPageIdx:
			{
				int ret;

				if ( objc < 4 || objc > 5 )
				{
					Tcl_WrongNumArgs ( interp, 2, objv, "command id ??options??" );
					return TCL_ERROR;
				}

				//ret = pageCmd ( assistant, interp, objv[2], objv[3], objc < 5 ? NULL : objv[4], 1 );

				if ( ret < 0 )
					return TCL_ERROR;

				/* TODO? associate an ID to every page? */
				Tcl_SetObjResult ( interp, Tcl_NewIntObj ( ret ) );

				break;
			}

		case CurrentPageIdx:
			{

				char str[3];
				gint n;
				n = gtk_assistant_get_current_page ( assistant );
				sprintf ( &str, "%d", n );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
				break;
			}

		case PagesIdx:  /* return the total number of pages in the assistant sequence */
			{
				char str[3];
				gint n;
				n = gtk_assistant_get_n_pages ( assistant );
				sprintf ( &str, "%d", n );
				Tcl_SetObjResult ( interp, Tcl_NewStringObj ( str, -1 ) );
				break;
			}
	}

	return TCL_OK;
}

/**
\brief      Function to create and configure an new instance of the asistant widget.
\author     William J Giddings
\date       07-Apr-2009
\remarks    An assistant widget creates a minimum of four pages
                Introduction, Content, Confirm, Summary
            The scripter can then modify the content of each page or add further
            pages to the dialog as required.
\since      GTK2.10 ; Gnocl-0.9.94a
\note       KRAUSE(2007) 146-157
*/
int gnoclAssistantCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
	int  ret;
	gint index;

	/* step 1) check validity of switches */

	if ( gnoclParseOptions ( interp, objc, objv, assistantOptions )
			!= TCL_OK )
	{
		gnoclClearOptions ( assistantOptions );
		return TCL_ERROR;
	}

	/* step 2) create a new assistant widget, minimum size, centred on screen. */
	GtkAssistant *assistant = gtk_assistant_new ();

	gtk_window_set_position ( GTK_WINDOW ( assistant ) , GTK_WIN_POS_CENTER );

	gtk_widget_set_size_request ( assistant, 450, 300 );

	/* step 3) check the options passed for the creation of the widget */
	ret = gnoclSetOptions ( interp, assistantOptions, G_OBJECT ( assistant ), -1 );

	/* step 4) if this is ok, then configure the new widget */

	if ( ret == TCL_OK )
	{
		/* unecessary? */
		//static int configure ( Tcl_Interp *interp, GtkAssistant *assistant, GnoclOption options[], gint page_num )
		ret = configure ( interp, assistant, assistantOptions, 0 );
	}

	/* step 5) clear the memory set assigned to the options */
	gnoclClearOptions ( assistantOptions );

	/* step 6) if the options passed were incorrect, then delete the widget */
	if ( ret != TCL_OK )
	{
		gtk_widget_destroy ( GTK_WIDGET ( assistant ) );
		return TCL_ERROR;
	}

	/* step 7) the process has been successful, so 'register' the widget with the interpreter */
	return gnoclRegisterWidget ( interp, GTK_WIDGET ( assistant ), assistantFunc );
}

