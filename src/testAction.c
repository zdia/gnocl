/****h* debug/testAction
 * NAME
 * 	testAction.c
 * SYNOPSIS
 * 	???
 * FUNCTION
 * NOTES
 * BUGS
 * SEE ALSO
 *****/

#include <gtk/gtk.h>

/***f* testAction/funcs
* AUTHOR
*	PGB
* SOURCE
*/
void func1 ( GtkAction *action, gpointer user_data )
{
	printf ( "hallo1\n" );
}

void func2 ( GtkAction *action, gpointer user_data )
{
	printf ( "hallo2\n" );
}

#if 0
static GtkActionEntry entries[] =
{
	{ "Open", GTK_STOCK_OPEN,                    /* name, stock id */
		"_Open", "<control>O",                     /* label, accelerator */
		"Open a file",                             /* tooltip */
		G_CALLBACK ( func1 ) },
};
#endif

static const char *ui_description =
	"<ui>"
	"  <menubar name='MainMenu'>"
	"    <menu action='FileMenu'>"
	"      <menuitem action='Open'/>"
	"      <menuitem action='Exit'/>"
	"    </menu>"
	"    <menu action='ViewMenu'>"
	"      <menuitem action='ZoomIn'/>"
	"      <menuitem action='ZoomOut'/>"
	"      <separator/>"
	"      <menuitem action='FullScreen'/>"
	"      <separator/>"
	"      <menuitem action='HighQuality'/>"
	"      <menuitem action='NormalQuality'/>"
	"      <menuitem action='LowQuality'/>"
	"    </menu>"
	"  </menubar>"
	"</ui>";

/* Normal items */
static GtkActionEntry entries[] =
{
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "Open", GTK_STOCK_OPEN, "_Open", "<control>O", "Open a file", func1 },
	{ "Exit", GTK_STOCK_QUIT, "E_xit", "<control>Q", "Exit the program", func1 },
	{ "ZoomIn", GTK_STOCK_ZOOM_IN, "Zoom _In", "plus", "Zoom into the image", func1 },
	{ "ZoomOut", GTK_STOCK_ZOOM_OUT, "Zoom _Out", "minus", "Zoom away from the image", func1 },
};

/* Toggle items */
static GtkToggleActionEntry toggle_entries[] =
{
	{ "FullScreen", NULL, "_Full Screen", "F11", "Switch between full screen and windowed mode", func1, FALSE }
};

/* Radio items */
static GtkRadioActionEntry radio_entries[] =
{
	{ "HighQuality", "my-stock-high-quality", "_High Quality", NULL, "Display images in high quality, slow mode", 0 },
	{ "NormalQuality", "my-stock-normal-quality", "_Normal Quality", NULL, "Display images in normal quality", 1 },
	{ "LowQuality", "my-stock-low-quality", "_Low Quality", NULL, "Display images in low quality, fast mode", 2 }
};

int main ( int argc, char **argv )
{
	int k;
	GtkWidget *window, *child, *box, *menu, *subMenu;
	GtkAction *action[2];
	GtkUIManager  *manager;

	gtk_init ( &argc, &argv );
	window = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
	box = gtk_vbox_new ( 0, 0 );
	gtk_container_add ( GTK_CONTAINER ( window ), box );
	/*
	   menuBar = gtk_menu_bar_new();
	   gtk_container_add( GTK_CONTAINER( box ), menuBar );
	   menu = gtk_menu_item_new_with_label( "Menu" );
	   gtk_container_add( GTK_CONTAINER( menuBar ), menu );
	   subMenu = gtk_menu_new();
	   gtk_menu_item_set_submenu( menu, subMenu );
	*/

	{
		/*
		   actionGroup name -entries ... -toggleEntries ... -radioEntries
		   $name addEntries
		   $name addToggleEntries
		   $name addRadioEntries
		*/
		GtkActionGroup *action_group = gtk_action_group_new ( "MenuActions" );
		gtk_action_group_add_actions ( action_group, entries,
									   G_N_ELEMENTS ( entries ), window );
		gtk_action_group_add_toggle_actions ( action_group, toggle_entries,
											  G_N_ELEMENTS ( toggle_entries ), window );
		gtk_action_group_add_radio_actions ( action_group, radio_entries,
											 G_N_ELEMENTS ( radio_entries ), 0, func1, window );
		/*
		   uiManager addUI ui_description -groups $name
		   $manager addGroup
		*/
		manager = gtk_ui_manager_new ();
		gtk_ui_manager_insert_action_group ( manager , action_group, 0 );
	}

	{
		/*
		   $manager setAccelWindow window
		*/
		GtkAccelGroup *accel_group = gtk_ui_manager_get_accel_group ( manager );
		gtk_window_add_accel_group ( GTK_WINDOW ( window ), accel_group );
	}

	{
		/*
		   $manager setUiDescription xxx
		*/
		GError *error = NULL;

		if ( !gtk_ui_manager_add_ui_from_string ( manager, ui_description, -1, &error ) )
		{
			g_message ( "building menus failed: %s", error->message );
			g_error_free ( error );
			exit ( 1 );
		}
	}

	{
		/*
		   $manager getWidget name
		*/
		GtkWidget *menuBar = gtk_ui_manager_get_widget ( manager, "/MainMenu" );
		gtk_box_pack_start ( GTK_BOX ( box ), menuBar, FALSE, FALSE, 0 );
	}

	gtk_widget_show_all ( window );

	/*
	   action[0] = GTK_ACTION( gtk_action_new( "name", "label", "tooltip", NULL ) );
	   action[1] = GTK_ACTION( gtk_action_new( "name2", "label2", "tooltip2", "gtk-quit" ) );

	   g_signal_connect( action[0], "activate", G_CALLBACK( func1 ), NULL);
	   g_signal_connect( action[1], "activate", G_CALLBACK( func2 ), NULL);
	*/
	/* accelGroup =  gtk_accel_group_new(); */


#if 0
	group = gtk_action_group_new ( "global" );
	gtk_action_group_add_actions ( group, entries, 1, window );
	manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group ( manager, group, 0 );
	accelGroup = gtk_ui_manager_get_accel_group ( manager );

	gtk_window_add_accel_group ( GTK_WINDOW ( window ), accelGroup );

	for ( k = 0; k < 2; ++k )
	{
		child = gtk_action_create_menu_item ( action[k] );
		gtk_container_add ( GTK_CONTAINER ( subMenu ), child );
		child = gtk_action_create_tool_item ( action[k] );
		gtk_container_add ( GTK_CONTAINER ( box ), child );
	}

	gtk_action_group_add_action_with_accel ( group, action[0], "<control>w" );

	gtk_action_group_add_action_with_accel ( group, action[1], NULL );
#endif

	gtk_widget_show_all ( window );

	gtk_main();
	return 0;
}

/*

gcc $(pkg-config --cflags gtk+-2.0) $(pkg-config --libs gtk+-2.0) testAction.c
*/
/*****/
