/* source: Andrew Krause, Foundations of GTK+ Development, chapter 9
 * .../ gtk/source/ige-macintegration/test-integration.c John Ralls
 * 
 * skeleton for testing menu on gtk-osx: getting the built menu from a xml file
 * 
 * FILE: gnocl/src/macosx/osxmenu_ui.c
 * Based on gtk_action
 * 
 * Zbigniew Diaczyszyn (c) 2010
 * 
 */

// BEGIN header gnoclosx.h
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gtkosxapplication.h"
// END header gnoclosx.h

// void setup_mac_menu ()
// {
		// do all the initialization stuff for the Mac
// }

/* In an actual application, you would implement these callback functions! */
static void open (GtkMenuItem *menuitem, gpointer data) {g_print("open something\n");};
static void save (GtkMenuItem *menuitem, gpointer data) {};
static void quit (GtkMenuItem *menuitem, gpointer data) {};
static void cut (GtkMenuItem *menuitem, gpointer data) {};
static void copy (GtkMenuItem *menuitem, gpointer data) {};
static void paste (GtkMenuItem *menuitem, gpointer data) {};
static void selectall (GtkMenuItem *menuitem, gpointer data) {};
static void deselect (GtkMenuItem *menuitem, gpointer data) {};
static void help (GtkMenuItem *menuitem, gpointer data) {};
static void about (GtkMenuItem *menuitem, gpointer data) {};

#define NUM_ENTRIES 13
static GtkActionEntry entries[] = 
{
  { "File", NULL, "_File", NULL, NULL, NULL },
  { "Open", GTK_STOCK_OPEN, NULL, NULL, 
     "Open an existing file", G_CALLBACK (open) },
  { "Save", GTK_STOCK_SAVE, NULL, NULL, 
     "Save the document to a file", G_CALLBACK (save) },
  { "Quit", GTK_STOCK_QUIT, NULL, NULL, 
    "Quit the application", G_CALLBACK (quit) },
  { "Edit", NULL, "_Edit", NULL, NULL, NULL },
  { "Cut", GTK_STOCK_CUT, NULL, NULL, 
    "Cut the selection to the clipboard", G_CALLBACK (cut) },
  { "Copy", GTK_STOCK_COPY, NULL, NULL, 
    "Copy the selection to the clipboard", G_CALLBACK (copy) },
  { "Paste", GTK_STOCK_PASTE, NULL, NULL, 
    "Paste text from the clipboard", G_CALLBACK (paste) },
  { "SelectAll", GTK_STOCK_SELECT_ALL, NULL, NULL, 
    "Select all of the text", G_CALLBACK (selectall) },
  { "Deselect", NULL, "_Deselect", "<control>d", 
    "Deselect all of the text", G_CALLBACK (deselect) },
  { "Help", NULL, "_Help", NULL, NULL, NULL },
  { "Contents", GTK_STOCK_HELP, NULL, NULL, 
    "Get help on using the application", G_CALLBACK (help) },
  { "About", GTK_STOCK_ABOUT, NULL, NULL, 
    "More information about the application", G_CALLBACK (about) }
};


int main (int argc, 
          char *argv[])
{
	GtkWidget *window, *menubar, *vbox;
  GtkActionGroup *group;
  GtkUIManager *uimanager;
  
  GtkOSXApplication *theApp;
	
  gtk_init (&argc, &argv);

	g_print("starting ...\n");
	
	// #ifdef GDK_WINDOWING_QUARTZ
	// osxmenubar = g_object_new(GTK_TYPE_OSX_APPLICATION, NULL);
	// #endif
	theApp = g_object_new(GTK_TYPE_OSX_APPLICATION, NULL);
	// GtkOSXApplicationMenuGroup *group;
	
	// show an empty window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	/* Create a new action group and add all of the actions to it. */
  group = gtk_action_group_new ("MainActionGroup");
  gtk_action_group_add_actions (group, entries, NUM_ENTRIES, NULL);

	/* Create a new UI manager and build the menu bar and toolbar. */
  uimanager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (uimanager, group, 0);
  gtk_ui_manager_add_ui_from_file (uimanager, "menu.ui", NULL);
  // gtk_ui_manager_add_ui_from_file (uimanager, "toolbar.ui", NULL);

	/* Retrieve the necessary widgets and associate accelerators. */
  menubar = gtk_ui_manager_get_widget (uimanager, "/MenuBar");
  // toolbar = gtk_ui_manager_get_widget (uimanager, "/Toolbar");
  // gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
  // gtk_window_add_accel_group (GTK_WINDOW (window), 
                              // gtk_ui_manager_get_accel_group (uimanager));
  
  // gnocl::Init
	gtk_osxapplication_set_use_quartz_accelerators(theApp, FALSE);
	
	/* create a container vbox and put the menubar into it */
	// gnocl::Init
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, TRUE, 0);
                     
  gtk_widget_hide (menubar);
	gtk_osxapplication_set_menu_bar(theApp, GTK_MENU_SHELL(menubar));

  // gtk_osxapplication_set_window_menu(theApp, NULL);
  
  // insert the About item

	gtk_widget_show_all(menubar);
	gtk_widget_show_all(window);
  gtk_osxapplication_ready(theApp);

  // gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_main ();
  
	g_object_unref(theApp);
  return 0;
}
