// gnoclOSX.c
// 
// providing the package GnoclMacOSX for Gnocl
// 
// Version 0.0
// 
// Author: z_dot_dia_at_gmx_dot_de
// 
// All Macosx functions will be gathered in this file
// gnocl.dylib is compiled with the Linux sources
// just replacing a X11 windows call in socket.c
// 

// testing: 
// cd .../src/macosx
// tclsh test.tcl

// TODO: Set the menu accelerators for Aqua

#include <stdlib.h>
#include "../gnocl.h"
#include <tcl.h>
#include <gtk/gtk.h>
#include "gtkosxapplication.h"
#include <assert.h>

#define GNOCLMACOSXDEBUG

/* 
 * Global declarations
 */

GtkWidget *GtkOSXMenubar;
GtkWidget *vbox, *about, *appmenu, *preferences, *quit;
GtkOSXApplication *GtkOSXMacmenu; 
GtkOSXApplicationMenuGroup *appmenugroup;
GtkAccelGroup *group;

Tcl_ObjCmdProc gnoclMacCmd;

// Syntax: "option, TYPE, property, OptCommandHandler
static GnoclOption macMenuOptions[] =
{
	{ "-onClicked", GNOCL_OBJ, "activate", gnoclOptCommand },
	// { "-text", GNOCL_OBJ, NULL },
	{ "-text", GNOCL_STRING, "label", gnoclOptCommand },
	{ NULL }
};

/*
 *  -------------------- Functions ---------------------------------
 */

/* we have to replace the original callback-handler 'doCommand'
 * because it is requiring the type Gtkwidget as first argument
 * whereas the object which is sending the signal is of type
 * GtkOSXApplication
 */

static void macQuitSignalHandler ( GtkOSXApplication *app, gpointer data )
{

	int ret; 
	GnoclCommandData *cs = ( GnoclCommandData * ) data;
	char *script = cs->command;

	// Tcl_EvalObj(cs->interp, script); 		/* not working! Segmentation fault!*/
	ret = Tcl_EvalEx ( cs->interp, script, -1, TCL_EVAL_GLOBAL | TCL_EVAL_DIRECT );
		
	if ( ret != TCL_OK )
	{
		Tcl_BackgroundError ( cs->interp );
	}

	return ret;
}

/*
 * replace gnoclConnectOptCmd because: 
 * 'The signals connected here are those of the GtkWidget base class.'
 * so we have to struggle with the following message:
 * Gtk-CRITICAL **: gtk_widget_get_name: assertion `GTK_IS_WIDGET (widget)' failed
*/

int gnoclMacOptCommand ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret )
{
	assert ( opt->type == GNOCL_OBJ );

	return gnoclConnectOptCmd ( interp, obj, opt->propName, G_CALLBACK ( macQuitSignalHandler ), opt, NULL, ret );
}

static GnoclOption macMenuQuitOptions[] =
{
	{ "-onClicked", GNOCL_OBJ, "NSApplicationBlockTermination", gnoclMacOptCommand },
	{ NULL }
};

int setupDefaultMacmenu ( Tcl_Interp *interp ) 
{
  // create a default application menu
  GtkOSXMacmenu = g_object_new(GTK_TYPE_OSX_APPLICATION, NULL);
  GtkOSXMenubar = gtk_menu_bar_new ();
  // group = gtk_accel_group_new ();
  	
  /* create a container vbox and put the GtkOSXMenubar into it */
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), GtkOSXMenubar, FALSE, TRUE, 0);
  gtk_widget_hide (GtkOSXMenubar);
  
  gtk_osxapplication_set_menu_bar(GtkOSXMacmenu, GTK_MENU_SHELL(GtkOSXMenubar));
  gtk_widget_show_all(GtkOSXMenubar);
  
  appmenu = gtk_menu_new ();
  
  // add the appmenuItem 'About'
	about = gtk_menu_item_new_with_label ("About Gnocl");
  // about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, appmenugroup);
  gtk_menu_shell_append (GTK_MENU_SHELL (appmenu), about );
	appmenugroup = gtk_osxapplication_add_app_menu_group (GtkOSXMacmenu);
	gtk_osxapplication_add_app_menu_item  (GtkOSXMacmenu, appmenugroup, GTK_MENU_ITEM(about));
	// add the appmenuItem 'Preferences'
	preferences = gtk_image_menu_item_new_with_mnemonic ( "Preferences" ) ;
  gtk_menu_shell_append (GTK_MENU_SHELL (appmenu), preferences);
	appmenugroup = gtk_osxapplication_add_app_menu_group (GtkOSXMacmenu);
	gtk_osxapplication_add_app_menu_item (GtkOSXMacmenu, appmenugroup, GTK_MENU_ITEM(preferences));

	gtk_widget_show (about);
	gtk_widget_show (preferences);
  
  gtk_osxapplication_ready(GtkOSXMacmenu);
  
  return TCL_OK;
}

/*
 * 'package require GnoclMacOSX' starts with this function
 */

int Gnoclmacosx_Init ( Tcl_Interp *interp )
{
  if ( Tcl_InitStubs ( interp, "8.3", 0 ) == NULL )
      return TCL_ERROR;

  if ( Tcl_PkgRequire ( interp, "Gnocl", "0.9.95", 0 ) == NULL )
      return TCL_ERROR;

  if ( Tcl_PkgProvide ( interp, "GnoclMacOSX", "0.0" ) != TCL_OK )
      return TCL_ERROR;
      
  if ( setupDefaultMacmenu ( interp ) != TCL_OK )
    {
      Tcl_SetResult ( interp, "could not initialize GnoclMacOSX", TCL_STATIC );
      return TCL_ERROR;
    }

  Tcl_CreateObjCommand ( interp, "gnocl::mac", gnoclMacCmd, NULL, NULL );

  return TCL_OK;
}

/* 
 * gnocl::mac MenuBar is a widget
 * it can be configured with the following options:
 * "menubar", "about", "quit", "preferences", "help"
 * When it is registered with gnoclRegisterWidget then it
 * gets the commands from menuBarFunc, so it can be accessed with
 * "delete", "configure","add", "addBegin", "addEnd"
*/

int gnoclMacCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
  // check if there is a valid command argument
  
  static const char *cmds[] = { "menubar", "about", "preferences", "quit", NULL};
	enum cmdIdx { MenuBarIdx, AboutIdx, PreferencesIdx, QuitIdx  };
	int idx, ret;
  
  // objv[0] = gnocl::mac
  // objv[1] = command
  // objc = 2 ; i.e. gnocl::mac command
  
	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

  switch ( idx )
	{
		case MenuBarIdx: 
			gtk_osxapplication_set_menu_bar(GtkOSXMacmenu, GTK_MENU_SHELL(GtkOSXMenubar));
      gtk_widget_show_all(GtkOSXMenubar);
      return gnoclRegisterWidget ( interp, GtkOSXMenubar, menuBarFunc );

		case AboutIdx: 
			
			/* ParseOptions starts evaluation with objv[1] -> gnocl::mac
			 * But we have to start ParseOptions with objv[2] -> about
			 * so we go one element forth */
			
			gnoclParseAndSetOptions (interp, objc - 1, objv + 1, macMenuOptions, G_OBJECT(about) ) ;
			break;
		
		case PreferencesIdx: 
			
			gnoclParseAndSetOptions (interp, objc - 1, objv + 1, macMenuOptions, G_OBJECT(preferences) ) ;
			break;
		
		case QuitIdx: 
			
			gnoclParseAndSetOptions (interp, objc - 1, objv + 1, macMenuQuitOptions, G_OBJECT(GtkOSXMacmenu) ) ;
			break;

	}
	return TCL_OK;
}
