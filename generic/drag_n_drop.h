/*
drag_n_drop.h

/author Sergio Villar Senín, 2007. Igalia.

*/

/*---------- drap and drop functions ----------*/

/**
/brief
/author
/date
**/

/* The types of the data that could be dragged */
static const GtkTargetEntry drag_types[] =
{
	{ "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, 0 }
};

static guint timer_expander;

/* This function expands a tree node candidate to be the destination
   row of a drag if it's not already expanded. Comes from the
   gtktreeview.c source */
static gint expand_row_timeout ( gpointer data )
{
	GtkTreeView *tree_view = data;
	GtkTreePath *dest_path = NULL;
	GtkTreeViewDropPosition pos;
	gboolean result = FALSE;

	GDK_THREADS_ENTER ();

	gtk_tree_view_get_drag_dest_row ( tree_view,
									  &dest_path,
									  &pos );

	if ( dest_path &&
			( pos == GTK_TREE_VIEW_DROP_INTO_OR_AFTER ||
			  pos == GTK_TREE_VIEW_DROP_INTO_OR_BEFORE ) )
	{
		gtk_tree_view_expand_row ( tree_view, dest_path, FALSE );
		gtk_tree_path_free ( dest_path );
	}

	else
	{
		if ( dest_path )
			gtk_tree_path_free ( dest_path );

		result = TRUE;
	}

	GDK_THREADS_LEAVE ();

	return result;
}

/**
/brief
/author
/date
**/
static gboolean drag_motion_cb ( GtkWidget      *widget,
								 GdkDragContext *context,
								 gint            x,
								 gint            y,
								 guint           time,
								 gpointer        user_data )
{
	GtkTreeViewDropPosition pos;
	GtkTreePath *dest_row;

	/* Remove all pending expander timeouts */
	if ( timer_expander != 0 )
	{
		g_source_remove ( timer_expander );
		timer_expander = 0;
	}

	/* Get the destination row */
	gtk_tree_view_get_dest_row_at_pos ( GTK_TREE_VIEW ( widget ),
										x, y,
										&dest_row,
										&pos );

	if ( !dest_row )
		return FALSE;

	/* Expand the selected row after 1/2 second */
	if ( !gtk_tree_view_row_expanded ( GTK_TREE_VIEW ( widget ), dest_row ) )
	{
		gtk_tree_view_set_drag_dest_row ( GTK_TREE_VIEW ( widget ), dest_row, pos );
		timer_expander = g_timeout_add ( 500, expand_row_timeout, widget );
	}

	gtk_tree_path_free ( dest_row );

	return TRUE;
}

/**
/brief
/author
/date
**/
static void drag_data_get_cb ( GtkWidget *widget,
							   GdkDragContext *context,
							   GtkSelectionData *selection_data,
							   guint info,
							   guint time,
							   gpointer data )
{
	GtkTreeSelection *selection;
	GtkTreeModel *model_sort, *model;
	GtkTreeIter iter;
	GtkTreePath *source_row_sort;
	GtkTreePath *source_row;

	/* Get the path to the selected row */

	g_print ( "drag_data_get_cb 1 \n" );
	selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( widget ) );

	g_print ( "drag_data_get_cb 2 \n" );
	gtk_tree_selection_get_selected ( selection, &model_sort, &iter );

	g_print ( "drag_data_get_cb 3 \n" );
	source_row_sort = gtk_tree_model_get_path ( model_sort, &iter );

	/* Get the unsorted model and the path to the selected row in
	   the unsorted model */
	g_print ( "drag_data_get_cb 4 \n" );


// GtkTreeModel *gtk_tree_model_sort_get_model       (GtkTreeModelSort *tree_model);
	model = gtk_tree_model_sort_get_model ( GTK_TREE_MODEL_SORT ( &model_sort ) );

	/* this is where the crash is happening! */
	g_print ( "drag_data_get_cb 5 \n" );
	source_row = gtk_tree_model_sort_convert_path_to_child_path ( GTK_TREE_MODEL_SORT ( model_sort ), source_row_sort );

	/* Set the row as the drag data */
	g_print ( "drag_data_get_cb 1 \n" );
	gtk_tree_set_row_drag_data ( selection_data, model, source_row );

	gtk_tree_path_free ( source_row_sort );
	gtk_tree_path_free ( source_row );
}

/**
/brief
/author
/date
**/
static void drag_data_received_cb ( GtkWidget *widget,
									GdkDragContext *context,
									gint x,
									gint y,
									GtkSelectionData *selection_data,
									guint info,
									guint time,
									gpointer data )
{
	GtkTreeModel *model_sort, *model;
	GtkTreeRowReference *source_row_reference;
	GtkTreePath *source_row, *dest_row, *child_dest_row;
	GtkTreeViewDropPosition pos;
	gboolean success = FALSE;

	/* Do not allow further process */
	g_print ( "drag_data_received_cb 1\n" );
	g_signal_stop_emission_by_name ( widget, "drag-data-received" );

	if ( selection_data->length == 0 )
		goto out;

	/* Get the unsorted model, and the path to the source row */
	g_print ( "drag_data_received_cb 2\n" );
	model_sort = gtk_tree_view_get_model ( GTK_TREE_VIEW ( widget ) );

	g_print ( "drag_data_received_cb 3\n" );
	gtk_tree_get_row_drag_data ( selection_data, &model, &source_row );

	/* Get the path to the destination row. Can not call
	   gtk_tree_view_get_drag_dest_row() because the source row
	   it's not selected anymore */
	gtk_tree_view_get_dest_row_at_pos ( GTK_TREE_VIEW ( widget ), x, y, &dest_row, &pos );

	/* Only allow drops IN other rows */
	if ( !dest_row ||
			pos == GTK_TREE_VIEW_DROP_BEFORE ||
			pos == GTK_TREE_VIEW_DROP_AFTER )
		goto out;

	/* Get the destination row in the usorted model */
	child_dest_row = gtk_tree_model_sort_convert_path_to_child_path ( GTK_TREE_MODEL_SORT ( model_sort ), dest_row );
	gtk_tree_path_free ( dest_row );

	/* Check if the drag is possible */
	if ( !gtk_tree_drag_dest_row_drop_possible ( GTK_TREE_DRAG_DEST ( model ), child_dest_row, selection_data ) )
	{
		gtk_tree_path_free ( child_dest_row );
		goto out;
	}

	/* Get a row reference to the source path because the path
	   could change after the insertion. The gtk_drag_finish() is
	   not able to delete the source because that, so we have to
	   do it manually */
	source_row_reference = gtk_tree_row_reference_new ( model, source_row );
	gtk_tree_path_free ( source_row );

	/* Insert the dragged row as a child of the dest row */
	gtk_tree_path_down ( child_dest_row );

	if ( gtk_tree_drag_dest_drag_data_received ( GTK_TREE_DRAG_DEST ( model ), child_dest_row, selection_data ) )
	{

		/* Clean dest row */
		gtk_tree_view_set_drag_dest_row ( GTK_TREE_VIEW ( widget ), NULL, GTK_TREE_VIEW_DROP_BEFORE );

		/* Get the new path of the source row */
		source_row = gtk_tree_row_reference_get_path ( source_row_reference );

		/* Delete the source row */
		gtk_tree_drag_source_drag_data_delete ( GTK_TREE_DRAG_SOURCE ( model ), source_row );

		success = TRUE;

		gtk_tree_path_free ( source_row );
	}

	gtk_tree_row_reference_free ( source_row_reference );
	gtk_tree_path_free ( child_dest_row );
out:
	/* Never delete the source, we do it manually */
	gtk_drag_finish ( context, success, FALSE, time );
}

/**
/brief
/author
/date
**/
static void setup_drag_and_drop ( GtkTreeView *self )
{
	/* We could not use the specific treeview dnd API because that
	   does not give us enough control over the process. Most of
	   these functions do not work with sorted model so we have to
	   redefine several behaviours */

	g_print ( "setup_drag_and_drop 1\n" );
	/* Config the treeview as destination for dnd */
	gtk_drag_dest_set ( GTK_WIDGET ( self ),
						GTK_DEST_DEFAULT_ALL,
						drag_types,
						G_N_ELEMENTS ( drag_types ),
						GDK_ACTION_MOVE );

	g_print ( "setup_drag_and_drop 2\n" );
	gtk_signal_connect ( GTK_OBJECT ( self ),
						 "drag_data_received",
						 GTK_SIGNAL_FUNC ( drag_data_received_cb ),
						 NULL );


	/* Config the treeview as source for dnd */
	g_print ( "setup_drag_and_drop 3\n" );
	gtk_drag_source_set ( GTK_WIDGET ( self ),
						  GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
						  drag_types,
						  G_N_ELEMENTS ( drag_types ),
						  GDK_ACTION_MOVE );

	g_print ( "setup_drag_and_drop 4\n" );
	gtk_signal_connect ( GTK_OBJECT ( self ),
						 "drag_motion",
						 GTK_SIGNAL_FUNC ( drag_motion_cb ),
						 NULL );

	g_print ( "setup_drag_and_drop 5\n" );
	gtk_signal_connect ( GTK_OBJECT ( self ),
						 "drag_data_get",
						 GTK_SIGNAL_FUNC ( drag_data_get_cb ),
						 NULL );
}

/*------------- end of drag-n-drop -------------*/
