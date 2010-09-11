// 
// All Macosx functions will be gathered in this file
// gnocl.dylib is compiled just with replacing X11 windows call in socket.c
// 

#include <stdlib.h>
#include "../gnocl.h"
// #include "osxmenu.h"
#include <tcl.h>
#include <gtk/gtk.h>
#include "gtkosxapplication.h"

#define MACDEBUG

// global declarations
GtkWidget *GtkOSXMenubar;
GtkWidget *vbox;
GtkOSXApplication *GtkOSXMacmenu; 
int macFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

Tcl_ObjCmdProc gnoclMacCmd;


/*
 * 'package require GnoclMacOSX' starts with this function
 */

int setupDefaultMacmenu ( Tcl_Interp *interp ) 
{
  // we have to create a menubar object for Aqua
  GtkOSXMacmenu = g_object_new(GTK_TYPE_OSX_APPLICATION, NULL);
  GtkOSXMenubar = gtk_menu_bar_new ();
  
  /* create a container vbox and put the GtkOSXMenubar into it */
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), GtkOSXMenubar, FALSE, TRUE, 0);
  gtk_widget_hide (GtkOSXMenubar);
  gtk_osxapplication_set_menu_bar(GtkOSXMacmenu, GTK_MENU_SHELL(GtkOSXMenubar));
  // gtk_widget_show_all(GtkOSXMenubar);
  gtk_osxapplication_ready(GtkOSXMacmenu);
  
  // return TCL_ERROR ; wenn GtkOSXMacmenu und GtkOSXMenubar leer sind
  return TCL_OK;
}

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
  
  static const char *cmds[] = { "menubar", "about", "quit", "preferences", "help", NULL};
	enum cmdIdx { MenuBarIdx, AboutIdx, QuitIdx, PreferencesIdx, HelpIdx };
	int idx;
  
  // objv[0] = gnocl::mac
  // objv[1] = command
  // objc = 2 ; i.e. gnocl::mac command
  
	if ( objc < 2 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "command" );
		return TCL_ERROR;
	}

  // Tcl_GetIndexFromObj returns the index of "command" in the list "cmds"
  // to the variable idx
  // error returns message: bad "command" - must be *cmds
  
	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
		return TCL_ERROR;

  // argument is valid, let's evaluate the index
  
  switch ( idx )
	{
		case MenuBarIdx: {
			gtk_osxapplication_set_menu_bar(GtkOSXMacmenu, GTK_MENU_SHELL(GtkOSXMenubar));
      gtk_widget_show_all(GtkOSXMenubar);
      return gnoclRegisterWidget ( interp, GtkOSXMenubar, menuBarFunc );
		}

		// case AboutIdx {
				// option -text:
				// the variable "contents" is passed by the option -text
				// appmenugroup = gtk_osxapplication_add_app_menu_group (GtkOSXMacmenu);
				// gtk_osxapplication_add_app_menu_item  (GtkOSXMacmenu, appmenugroup, GTK_MENU_ITEM (contents));

				// option -onClicked:
				// accelerator and g_signal stuff ... still not implemented
		// }

			/*
			 * the rest of the cases
			 */

	}
    
	return TCL_OK;
}
