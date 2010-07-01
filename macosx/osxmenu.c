/* source: Andrew Krause, Foundations of GTK+ Development, chapter 9
 * 
 * skeleton for testing menu on gtk-osx
 * 
 * Zbigniew Diaczyszyn (c) 2010
 * 
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gtkosxapplication.h"

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *menubar, *file, *edit, *help, *filemenu, *editmenu, *helpmenu;
  GtkWidget *new, *open, *cut, *copy, *paste, *contents, *about, *vbox;
  GtkAccelGroup *group;
	GtkOSXApplicationMenuGroup *appmenugroup;

	GtkOSXApplication *theApp;
	
  gtk_init (&argc, &argv);

	g_print("starting ...\n");
	
	theApp = g_object_new(GTK_TYPE_OSX_APPLICATION, NULL);
	
	/* Build the menu */
  group = gtk_accel_group_new ();
  menubar = gtk_menu_bar_new ();
  file = gtk_menu_item_new_with_label ("File");
  edit = gtk_menu_item_new_with_label ("Edit");
  help = gtk_menu_item_new_with_label ("Help");
  filemenu = gtk_menu_new ();
  editmenu = gtk_menu_new ();
  helpmenu = gtk_menu_new ();

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), filemenu); 
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), editmenu); 
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), helpmenu);
  
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), file); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), edit); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), help);
  
  /* Create the File menu content. */
  new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, group);
  open = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), new);
  gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), open);
  
  /* Create the Edit menu content. */
  cut = gtk_image_menu_item_new_from_stock (GTK_STOCK_CUT, group);
  copy = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, group);
  paste = gtk_image_menu_item_new_from_stock (GTK_STOCK_PASTE, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), cut);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), copy);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), paste);
  
  /* Create the Help menu content. */
  contents = gtk_image_menu_item_new_from_stock (GTK_STOCK_HELP, group);
  about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu), contents);
  gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu), about);
  
	gtk_osxapplication_set_use_quartz_accelerators(theApp, FALSE);
	
	/* create a container vbox and put the menubar into it */
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, TRUE, 0);
                     
  gtk_widget_hide (menubar);
  gtk_osxapplication_set_menu_bar(theApp, GTK_MENU_SHELL(menubar));
  // insert the About item
  appmenugroup = gtk_osxapplication_add_app_menu_group (theApp);
	gtk_osxapplication_add_app_menu_item  (theApp, appmenugroup, GTK_MENU_ITEM (about));

	gtk_widget_show_all(menubar);
  gtk_osxapplication_ready(theApp);

  gtk_main ();
  
	g_object_unref(theApp);
  return 0;
}
