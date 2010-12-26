/**
\brief      This module implements the gnocl::builder and gnocl::glade commands
\author     William J Giddings
\date       24-11-2009
\todo       -whilst parsing the builder widget tree, assign specific handlers
            -make public other constructor functions
\since    December-09 implemented widget support
                    button, togglebutton, checkbutton, entry, spinbutton, linkbutton,
                    filebuttonchooser, colorbutton, scale button, volumebutton,
                    image, label, hscale, vscale, hscrollbar, vscrollbar, accelarator

\note      Developer Documentation
            http://library.gnome.org/devel/libglade/unstable/index.html
            http://library.gnome.org/devel/libglade/unstable/GladeXML.html
            http://library.gnome.org/devel/gtk/2.16/GtkBuilder.html

            Development
            13/03/2010  Extended buffer command to include options
                -container
                -objects

**/
#include "gnocl.h"
#include "gnoclparams.h"

#include <gdk/gdk.h>
#include <string.h>
#include <assert.h>

#include <glade/glade.h>

/* these need to be global so that they can be accessed usind other builder empowered modules */
static char *GtkTypes[] =
{
	"GtkAccelLabel",
	"GtkLabel",
	"GtkProgressBar",
	"GtkStatusbar",
	"GtkStatusIcon",
	"GtkButton",
	"GtkCheckButton",
	"GtkRadioButton",
	"GtkToggleButton",
	"GtkLinkButton",
	"GtkScaleButton",
	"GtkVolumeButton",
	"GtkEntry",
	"GtkHScale",
	"GtkVScale",
	"GtkSpinButton",
	"GtkEditable",
	"GtkTextView",
	"GtkTreeView",
	"GtkCellView",
	"GtkIconView",
	"GtkComboBox",
	"GtkComboBoxEntry",
	"GtkMenu",
	"GtkMenuBar",
	"GtkMenuItem",
	"GtkToolbar",
	"GtkColorButton",
	"GtkColorSelectionDialog",
	"GtkColorSelection",
	"GtkHSV",
	"GtkFileChooser",
	"GtkFileChooserButton",
	"GtkFileChooserDialog",
	"GtkFileChooserWidget",
	"GtkFontButton",
	"GtkFontSelectionDialog",
	"GtkAlignment",
	"GtkHBox",
	"GtkVBox",
	"GtkHButtonBox",
	"GtkVButtonBox",
	"GtkFixed",
	"GtkHPaned",
	"GtkVPaned",
	"GtkLayout",
	"GtkNotebook",
	"GtkTable",
	"GtkExpander",
	"GtkFrame",
	"GtkHSeparator",
	"GtkVSeparator",
	"GtkHScrollbar",
	"GtkVScrollbar",
	"GtkScrolledWindow",
	"GtkPrintUnixDialog",
	"GtkPageSetupUnixDialog",
	"GtkArrow",
	"GtkCalendar",
	"GtkDrawingArea",
	"GtkEventBox",
	"GtkHandleBox",
	"GtkWindow",
	"GtkToolButton",
	"GtkImage",
	"GtkRecentChooserWidget",
	"GtkAboutDialog",
	"GtkDialog",
	"GtkInputDialog",
	"GtkMessageDialog",
	"GtkRecentChooserDialog",
	"GtkAssistant",
	NULL
};

static enum  GtkTypesIdx
{
	GtkAccelLabelIdx,
	GtkLabelIdx,
	GtkProgressBarIdx,
	GtkStatusbarIdx,
	GtkStatusIconIdx,
	GtkButtonIdx,
	GtkCheckButtonIdx,
	GtkRadioButtonIdx,
	GtkToggleButtonIdx,
	GtkLinkButtonIdx,
	GtkScaleButtonIdx,
	GtkVolumeButtonIdx,
	GtkEntryIdx,
	GtkHScaleIdx,
	GtkVScaleIdx,
	GtkSpinButtonIdx,
	GtkEditableIdx,
	GtkTextViewIdx,
	GtkTreeViewIdx,
	GtkCellViewIdx,
	GtkIconViewIdx,
	GtkComboBoxIdx,
	GtkComboBoxEntryIdx,
	GtkMenuIdx,
	GtkMenuBarIdx,
	GtkMenuItemIdx,
	GtkToolbarIdx,
	GtkColorButtonIdx,
	GtkColorSelectionDialogIdx,
	GtkColorSelectionIdx,
	GtkHSVIdx,
	GtkFileChooserIdx,
	GtkFileChooserButtonIdx,
	GtkFileChooserDialogIdx,
	GtkFileChooserWidgetIdx,
	GtkFontButtonIdx,
	GtkFontSelectionDialogIdx,
	GtkAlignmentIdx,
	GtkHBoxIdx,
	GtkVBoxIdx,
	GtkHButtonBoxIdx,
	GtkVButtonBoxIdx,
	GtkFixedIdx,
	GtkHPanedIdx,
	GtkVPanedIdx,
	GtkLayoutIdx,
	GtkNotebookIdx,
	GtkTableIdx,
	GtkExpanderIdx,
	GtkFrameIdx,
	GtkHSeparatorIdx,
	GtkVSeparatorIdx,
	GtkHScrollbarIdx,
	GtkVScrollbarIdx,
	GtkScrolledWindowIdx,
	GtkPrintUnixDialogIdx,
	GtkPageSetupUnixDialogIdx,
	GtkArrowIdx,
	GtkCalendarIdx,
	GtkDrawingAreaIdx,
	GtkEventBoxIdx,
	GtkHandleBoxIdx,
	GtkWindowIdx,
	GtkToolButtonIdx,
	GtkImageIdx,
	GtkRecentChooserWidgetIdx,
	GtkAboutDialogIdx,
	GtkDialogIdx,
	GtkInputDialogIdx,
	GtkMessageDialogIdx,
	GtkRecentChooserDialogIdx,
	GtkAssistantIdx
};

/**
\brief
\author William J Giddings
\date   23/11/09
\note
*/
static int builderFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_BUILDER
	g_print ( " builderFunc, this will provide a command to access items(?) resulting from the loading of this tree!\n" );
#endif
	return TCL_OK;
}


/**
\brief  Dummy function for assigning to newly created gnocl widget within this module
\author William J Giddings
\date   23/11/09
\note
*/
static int builderWidgetFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_BUILDER
	g_print ( " builderWidgetFunc, this is just a stub!\n" );
#endif
	return TCL_OK;
}


/**
\brief
\author Peter G Baum
\date
\note   Borrowed from gnocl.c
**/
static void simpleDestroyFunc (
	GtkWidget *widget,
	gpointer data )
{
	const char *name = gnoclGetNameFromWidget ( widget );
	gnoclForgetWidgetFromName ( name );
	Tcl_DeleteCommand ( ( Tcl_Interp * ) data, ( char * ) name );
	g_free ( ( char * ) name );
}



/**
/brief
/author   William J Giddings
/date     24-11-2009
**/
int setGtkWidgetFunc ( Tcl_Interp *interp, GtkWidget *widget, int idx, Tcl_Obj *resList , gchar *builderName, char *name )
{
#ifdef DEBUG_BUILDER
	printf ( "setGtkWidgetFunc UI_name = %s, Gnocl_ame = %s\n", builderName, name );
#endif

	Tcl_ObjCmdProc *proc;
	int set = 0; /* flag to denote if registration functions arelady completed, ie widgets using 'para' entry /toggle */

	switch ( idx )
	{
		case GtkAssistantIdx:
			{
				proc = assistantFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkRecentChooserDialogIdx:
			{
				proc = recentChooserDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkMessageDialogIdx:
			{
				DialogParams *para = g_new ( DialogParams, 1 );
				para->dialog = GTK_DIALOG ( widget );
				para->interp = interp;
				para->name = NULL;
				para->butRes = g_ptr_array_new();
				para->isModal = 1;

				proc = dialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkInputDialogIdx:
			{
				proc = inputDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkDialogIdx:
			{
				proc = inputDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkAboutDialogIdx:
			{
				proc = aboutDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkLabelIdx:
			{
				LabelParams *para = g_new ( LabelParams, 1 );

				para->label = widget;
				para->interp = interp;
				para->textVariable = NULL;
				para->onChanged = NULL;
				para->inSetVar = 0;
				para->name = name;

				proc = labelFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkProgressBarIdx:
			{
				proc = progressBarFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkStatusbarIdx:
			{
				proc = statusBarFuc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkStatusIconIdx:
			{
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkButtonIdx:
			{
				proc = buttonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkCheckButtonIdx:
			{

				GnoclCheckParams * para = g_new ( GnoclCheckParams, 1 );

				para->interp = interp;
				para->name = name;
				para->widget = widget;
				para->onToggled = NULL;
				para->variable = NULL;
				para->onValue = Tcl_NewIntObj ( 1 );
				Tcl_IncrRefCount ( para->onValue );
				para->offValue = Tcl_NewIntObj ( 0 );
				Tcl_IncrRefCount ( para->offValue );
				para->inSetVar = 0;

				proc = checkButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkRadioButtonIdx:
			{

				GnoclRadioParams  *para = g_new ( GnoclRadioParams, 1 );

				para->name = name;
				para->widget = widget;
				para->onToggled = NULL;
				para->onValue = NULL;
				/*
				                para->group = gnoclRadioGetGroupFromVariable (
				                                  radioButtonOptions[variableIdx].val.str );

				                if ( para->group == NULL )
				                {
				                    para->group = gnoclRadioGroupNewGroup (
				                                      radioButtonOptions[variableIdx].val.str, interp );
				                }

				                else
				                {
				                    GnoclRadioParams *p = gnoclRadioGetParam ( para->group, 0 );
				                    gtk_radio_button_set_group ( GTK_RADIO_BUTTON ( para->widget ),
				                                                 gtk_radio_button_get_group ( GTK_RADIO_BUTTON ( p->widget ) ) );
				                }

				                gnoclRadioGroupAddWidgetToGroup ( para->group, para );
				*/

				proc = radioButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkToggleButtonIdx:
			{

				GnoclToggleParams *para;

				para = g_new ( GnoclToggleParams, 1 );

				para->interp = interp;

				para->name = name;
				para->widget = widget;
				para->onToggled = NULL;
				para->variable = NULL;
				para->onValue = Tcl_NewIntObj ( 1 );
				Tcl_IncrRefCount ( para->onValue );
				para->offValue = Tcl_NewIntObj ( 0 );
				Tcl_IncrRefCount ( para->offValue );
				para->inSetVar = 0;

				proc = toggleButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkLinkButtonIdx:
			{
				proc = linkButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkScaleButtonIdx:
			{
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkVolumeButtonIdx:
			{

				proc = volumeButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkEntryIdx:
			{

				EntryParams *para = g_new ( EntryParams, 1 );
				para->entry = GTK_ENTRY ( widget );
				para->interp = interp;
				para->name = name;
				para->variable = NULL;
				para->onChanged = NULL;
				para->inSetVar = 0;
				Tcl_CreateObjCommand ( interp, name, entryFunc, para, NULL );

			}
			break;
		case GtkHScaleIdx:
		case GtkVScaleIdx:
			{
				ScaleParams    *para = g_new ( ScaleParams, 1 );

				para->scale = widget;
				para->interp = interp;
				para->variable = NULL;
				para->onValueChanged = NULL;
				para->inSetVar = 0;
				para->name = name;

				proc = scaleFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkSpinButtonIdx:
			{

				SpinButtonParams *para = g_new ( SpinButtonParams, 1 );

				para->spinButton = widget;
				para->interp = interp;
				para->variable = NULL;
				para->onValueChanged = NULL;
				para->inSetVar = 0;

				proc = spinButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkEditableIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkEditableIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkTextViewIdx:
			{
				proc = textViewFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkTreeViewIdx:
			{
				proc = treeListFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkCellViewIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkCellViewIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkIconViewIdx:
			{
				proc = iconViewFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkComboBoxIdx:
		case GtkComboBoxEntryIdx:
			{
				ComboBoxParams *para = g_new ( ComboBoxParams, 1 );
				para->interp = interp;
				para->onChanged = NULL;
				para->variable = NULL;
				para->name = name;
				para->comboBox = GTK_COMBO_BOX ( widget );
				para->inSetVar = 0;

				proc = comboBoxFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
			/*--------------- MENU HANDLERS ---------------*/
		case GtkMenuIdx:
			{
				proc = menuFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkMenuBarIdx:
			{
				proc = menuBarFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;

		case GtkMenuItemIdx:
			{

				proc = menuItemFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkAccelLabelIdx:
			{

				proc = accelaratorFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkToolbarIdx:
			{
				proc = toolBarFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkColorButtonIdx:
			{
				proc = clrButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkColorSelectionDialogIdx:
			{

				ColorSelDialogParams *para = g_new ( ColorSelDialogParams, 1 );
				para->colorSel = GTK_COLOR_SELECTION_DIALOG ( widget );

				para->interp = interp;
				para->name = name;
				para->onClicked = NULL;

				proc = colorSelDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkColorSelectionIdx:
			{
				proc = colorSelectionFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHSVIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkHSVIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFileChooserIdx:
			{
				proc = fileChooserFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFileChooserButtonIdx:
			{
				proc = fileChooserButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFileChooserDialogIdx:
			{
				FileSelDialogParams *para = g_new ( FileSelDialogParams, 1 );
				para->interp = interp;
				para->getURIs = 0;
				para->fileDialog = GTK_FILE_CHOOSER_DIALOG ( widget );
				para->getURIs = 0;
				para->name = name;
				proc = fileDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkFileChooserWidgetIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkFileChooserWidgetIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFontButtonIdx:
			{
				proc = fontButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFontSelectionDialogIdx:
			{
				FontSelDialogParams *para = NULL;
				para = g_new ( FontSelDialogParams, 1 );
				para->fontSel = GTK_FONT_SELECTION_DIALOG ( widget );
				para->interp = interp;
				para->name = name;
				para->onClicked = NULL;
				proc = fontSelDialogFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkAlignmentIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkAlignmentIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHBoxIdx:
			{

				proc = boxFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkVBoxIdx:
			{

				proc = boxFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHButtonBoxIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkHButtonBoxIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkVButtonBoxIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkVButtonBoxIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFixedIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkFixedIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHPanedIdx:
		case GtkVPanedIdx:
			{
				proc = panedFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkLayoutIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkLayoutIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkNotebookIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkNotebookIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = notebookFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkTableIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkTableIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkExpanderIdx:
			{
				proc = expanderFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkFrameIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkFrameIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHSeparatorIdx:
		case GtkVSeparatorIdx:
			{
				proc = separatorFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHScrollbarIdx:
		case GtkVScrollbarIdx:
			{
				ScrollParams    *para = g_new ( ScrollParams, 1 );

				para->scroll = widget;
				para->interp = interp;
				para->variable = NULL;
				para->onValueChanged = NULL;
				para->inSetVar = 0;
				para->name = name;

				proc = scrollFunc;
				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkScrolledWindowIdx:
			{
				proc =  scrlWindowFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkPrintUnixDialogIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkPrintUnixDialogIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkPageSetupUnixDialogIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( "GtkPageSetupUnixDialogIdx Not Sorted Yet!!!! %s\n", name );
#endif
				proc = builderWidgetFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkArrowIdx:
			{
				proc = arrowButtonFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkCalendarIdx:
			{
				proc = calendarFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkDrawingAreaIdx:
			{
				proc = drawingAreaFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkEventBoxIdx:
			{

				proc = eventBoxFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkHandleBoxIdx:
			{
				proc = handleBoxFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkWindowIdx:
			{

				proc = windowFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkToolButtonIdx:
			{
				/*
				extra code needed here to fully inplement gnocl widget
				see toolBar.c fuction addButton
				*/


				/**
				\brief Description yet to be added.
				\author Peter G Baum
				*/
				static const int buttonTextIdx      = 0;
				static const int buttonIconIdx      = 1;
				static const int buttonOnClickedIdx = 2;
				extern toolButtonOptions;

				char         *txt;
				int          isUnderline;
				GtkWidget    *icon;
				GtkToolbar *toolbar = gtk_widget_get_parent ( widget );

				if ( getTextAndIcon ( interp, toolbar, toolButtonOptions + buttonTextIdx,
									  toolButtonOptions + buttonIconIdx, &txt, &icon, &isUnderline ) != TCL_OK )
				{
					Tcl_SetResult ( interp, "GNOCL ERROR: Unable to add tool button.\n", TCL_STATIC );
					return TCL_ERROR;
				}

				ToolButtonParams *para = g_new ( ToolButtonParams, 1 );
				para->item = widget;
				para->interp = interp;
				para->name = name;
				para->onClicked = NULL;

				proc = toolButtonFunc;

				Tcl_CreateObjCommand ( interp, para->name, toolButtonFunc, para, NULL );

				Tcl_CreateObjCommand ( interp, name, proc, para, NULL );
			}
			break;
		case GtkImageIdx:
			{
				proc = imageFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}
			break;
		case GtkRecentChooserWidgetIdx:
			{
				proc = recentChooserFunc;
				Tcl_CreateObjCommand ( interp, name, proc, widget, NULL );
			}

			break;
		default:
			{
#ifdef DEBUG_BUILDER
				printf ( "do nothing\n" );
#endif
				return -1;
			}
	}

	/* perform the equivalent of gnoclRegisterWidget */
	g_signal_connect_after ( G_OBJECT ( widget ), "destroy", G_CALLBACK ( simpleDestroyFunc ), interp );
	gnoclMemNameAndWidget ( name, GTK_WIDGET ( widget ) );

	gchar str[50];
	sprintf ( str, "%s %s", name, builderName );

	Tcl_ListObjAppendElement ( interp, resList, Tcl_NewStringObj ( str, -1 ) );

	return TCL_OK;
}


/*
int gnoclRegisterWidget (
    Tcl_Interp *interp,
    GtkWidget *widget,
    Tcl_ObjCmdProc *proc )
{
    const char *name = gnoclGetAutoWidgetId();
    gnoclMemNameAndWidget ( name, widget );

    g_signal_connect_after ( G_OBJECT ( widget ), "destroy",
                             G_CALLBACK ( simpleDestroyFunc ), interp );

    if ( proc != NULL )
        Tcl_CreateObjCommand ( interp, ( char * ) name, proc, widget, NULL );

    Tcl_SetObjResult ( interp, Tcl_NewStringObj ( name, -1 ) );

    return TCL_OK;
}
*/

static void  getWidgetNames ( GSList *widgetList, GSList *widgetNames )
{

	GSList *p = NULL;
	GSList *names = NULL;
	gchar *name;

	widgetNames = NULL;

	for ( p = widgetList; p != NULL; p = p->next )
	{
		name =    ( gchar * ) gtk_widget_get_name ( GTK_WIDGET ( p->data ) );
		widgetNames = g_slist_append ( widgetNames, name );
	}

}

static GSList * getBuilderObjectNames ( GtkBuilder *builder )
{

	GSList *p = NULL;
	GSList *namelist = NULL;
	const gchar *name;

	for ( p = ( GSList * ) gtk_builder_get_objects ( builder ); p != NULL; p = p->next )
	{
		name =  gtk_widget_get_name ( p->data );
		namelist = g_slist_append ( namelist, ( gchar * ) name );
	}

	return namelist;
}

static void display_list ( GSList *list )
{

	GSList *p = NULL;

	//print the list data
	for ( p = list; p != NULL; p = p->next )
	{
		printf ( "%s \n", p->data );
	}


}


/**
/brief  Load a builder file and create user interface.
        Register widget, and create variables holding names based upon id given in the builder file.
/author William J Giddings
/date   22-Nov-09
/todo   Obtain widget tree and then register widgets with gnocl.
**/
int gnoclBuilderCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{

#ifdef DEBUG_BUILDER
	g_printf ( "gnoclBuilderCmd %d\n", objc );
	gint _i;

	for ( _i = 0; _i < objc; _i++ )
	{
		g_printf ( "\targ %d = %s\n", _i,  Tcl_GetString ( objv[_i] ) );
	}

#endif

	GtkBuilder *builder = NULL;
	GtkWidget *container;
	GtkWidget *objects;
	gchar *objname;
	gchar *cname;
	char *builder_ui;
	gchar str[128];
	GtkWidget *obj;
	GSList *objectList;
	GSList *widgetNames, *p;


	static const char *cmds[] = { "file", "buffer", NULL };
	enum optIdx { FileIdx, BufferIdx };
	int idx;


// objc must be either 3 or 7

	if ( ( objc != 3 ) && ( objc != 7 ) )
	{
		Tcl_SetResult ( interp, "GNOCL ERROR: Wrong number of arguments.\n", TCL_STATIC );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "GNOCL ERROR: --- no message available ---\n" );
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case FileIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( ".....load from file %s\n", Tcl_GetString ( objv[2] ) );
#endif

				sprintf ( str, "%s", Tcl_GetString ( objv[2] ) );
				//GtkWidget  *window1 = NULL;
				builder = gtk_builder_new ();
				gtk_builder_add_from_file ( builder, Tcl_GetString ( objv[2] ), NULL );
			}
			break;
		case BufferIdx:
			{

#ifdef DEBUG_BUILDER
				printf ( ".....load from buffer %s\n", Tcl_GetString ( objv[3] ) );
#endif

				switch ( objc )
				{
					case 3:
						{
#ifdef DEBUG_BUILDER
							printf ( "the whole buffer %s\n", Tcl_GetString ( objv[2] ) );
#endif
							//sprintf ( str, "%s", Tcl_GetString ( objv[2] ) );

							builder_ui = Tcl_GetString ( objv[2] );
							builder = gtk_builder_new ();

							gtk_builder_add_from_string ( builder, builder_ui, strlen ( builder_ui ) * sizeof ( gchar ), NULL );
							goto SwitchIdxEnd;
						}
						break;
					case 7:
						{

							/* is there some way of putting all this into a function in order to eliminate the goto?

							                            /* then must load a partial buffer into a container */
							if (  strcmp ( Tcl_GetString ( objv[3] ) , "-objects"  ) == 0
									&& strcmp ( Tcl_GetString ( objv[5] ) , "-container" ) == 0 )
							{

								objname = Tcl_GetString ( objv[4] );
								cname = Tcl_GetString ( objv[6] );
							}

							else if (  strcmp ( Tcl_GetString ( objv[5] ) , "-objects"  ) == 0
									   && strcmp ( Tcl_GetString ( objv[3] ) , "-container" ) == 0 )
							{
								objname = Tcl_GetString ( objv[6] );
								cname = Tcl_GetString ( objv[4] );
							}

						}
						break;
				} /* end switch(objc) */

				container = gnoclGetWidgetFromName ( cname, interp );

				if  ( !GTK_IS_CONTAINER ( container ) )
				{

					sprintf ( str, "GNOCL ERROR: Widget \"%s\" specfied by option -container is not a valid container.\n", cname );
					Tcl_SetResult ( interp, str, TCL_STATIC );
					return TCL_ERROR;
				}


				/* values obtained, error checking completed, now to load the buffer */
#ifdef DEBUG_BUILDER
				printf ( "Build -step 1\n" );
#endif
				builder_ui = Tcl_GetString ( objv[2] );
#ifdef DEBUG_BUILDER
				printf ( "Build -step 2\n" );
#endif                builder = gtk_builder_new();
#ifdef DEBUG_BUILDER
				printf ( "Build -step 3\n" );
				printf ( "builder_ui = %s \nobjname = %s\n", builder_ui, objname );
#endif
				gtk_builder_add_objects_from_string ( builder, builder_ui,
													  strlen ( builder_ui ) * sizeof ( gchar ),
													  g_strsplit_set ( objname, " ", -1 ), NULL );
#ifdef DEBUG_BUILDER
				printf ( "Build -step 4\n" );
#endif

				objects = gtk_builder_get_object ( builder, objname ) ;

#ifdef DEBUG_BUILDER
				printf ( "Build -step 5\n" );
				printf ( "Build -step 5a do we have a box conntainer?\n" );
#endif

				if ( !GTK_IS_BOX ( container ) )
				{

					if ( GTK_IS_WIDGET ( objects ) )
					{
						gtk_container_add ( GTK_CONTAINER ( container ), objects );
					}

					else
					{
						Tcl_SetResult ( interp, "GNOCL ERROR: The specified container can only hold one child widget.\n", TCL_STATIC );
						return TCL_ERROR;
					}

				}

				else
				{
#ifdef DEBUG_BUILDER
					printf ( "I am a box! Yippie, lets pack everything in then...!\n" );
#endif

					/* modify this section to pack all the elements of the array */
					int x = 0;
					char *str1;

					str1 = strtok ( objname, " " );
					//str1 =  strtok( g_strsplit_set ( objname, " ", -1 ), " " );

					if ( str1 == NULL )
					{
						/* just one object */
#ifdef DEBUG_BUILDER
						printf ( "just one object\n" );
#endif
						gtk_box_pack_end ( GTK_CONTAINER ( container ), GTK_WIDGET ( objects ), 1, 1, 0 );
						break;
					}

#ifdef DEBUG_BUILDER
					printf ( "pack first object object\n" );
#endif
					objects = gtk_builder_get_object ( builder, str1 ) ;
					gtk_box_pack_end ( GTK_CONTAINER ( container ), GTK_WIDGET ( objects ), 1, 1, 0 );

					/* otherwise, loop until finishied */
					while ( 1 )
					{
						/* extract next item from string sequence */
						str1 = strtok ( NULL, " " );


						/* check if there is nothing else to extract */
						if ( str1 == NULL )
						{

							break;
						}

						/* print string after tokenized */
#ifdef DEBUG_BUILDER
						printf ( "\n\n>>>>>%i: %s\n\n", x, str1 );
#endif

						objects = gtk_builder_get_object ( builder, str1 ) ;

#ifdef DEBUG_BUILDER
						printf ( "Build -step 5\n" );
#endif
						// gtk_box_pack_end (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
						gtk_box_pack_start ( GTK_CONTAINER ( container ), GTK_WIDGET ( objects ), 1, 1, 0 );
						//gtk_container_add ( GTK_CONTAINER ( container ), GTK_WIDGET ( objects ) );

						x++;

					}

#ifdef DEBUG_BUILDER
					printf ( "Build -step 6\n" );
#endif
				} /* end else */


			} /* end case BufferIdx: */
	} /* end switch(idx) */

SwitchIdxEnd:

	/* the convenience function left out of the basic set of library functions */
	widgetNames = getBuilderObjectNames ( builder );
#ifdef DEBUG_BUILDER
	display_list ( widgetNames );
#endif

	GtkWidget *window;
	GtkWidget *widget;


	/*  build up a list of names */
	Tcl_Obj *resList = Tcl_NewListObj ( 0, NULL );

	/* ignore gnomeApp widgets, these are handled in their own modules */
	for ( p = widgetNames; p != NULL; p = p->next )
	{
#ifdef DEBUG_BUILDER
		printf ( "Now sorting out handler function for %s\n", p->data );
#endif
		/* obtain pointer to widget based upon list of names */
		widget = gtk_builder_get_object ( builder, p->data );

		/* obtain a new gnocl name */
		const char *name = gnoclGetAutoWidgetId();

		gchar *type =  G_OBJECT_TYPE_NAME ( widget );

#ifdef DEBUG_BUILDER == builder
		printf ( "The gnocl name created is %s, type = %s\n", name, type );
#endif
		/* determine how to handle the new widget */
		gint idx = compare ( type, GtkTypes );
#ifdef DEBUG_BUILDER == builder
		printf ( "The gnocl name created is %s, type = %s, idx = %d\n", name, type, idx );
#endif
		/* now, effectively a multi-widget gnoclRegisterWidget function */
		setGtkWidgetFunc ( interp, widget, idx , resList, p->data, name );

		if ( 0 )
		{
			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkWindow" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkWindow\n" );
#endif
				window = GTK_WIDGET (  p->data );

			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );

			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkAboutDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkAboutDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkColorSelectionDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkColorSelectionDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkFileChooserDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkFileChooserDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkFontSelectionDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkFontSelectionDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkInputDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkInputDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkMessageDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkMessageDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkRecentChooserDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkRecentChooserDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkAssistant" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkAssistant\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

		}

	}

	/* create a specific command for each of the builder files loaded */
	Tcl_CreateObjCommand ( interp, Tcl_GetString ( objv[2] ), builderFunc, builder, NULL );

	/* return the list of created widgets */
	Tcl_SetObjResult ( interp, resList );
	return TCL_OK;

}


/* glade equivalents */

/**
\brief
\author William J Giddings
\date   23/11/09
\note
*/
static int gladeFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_BUILDER
	g_print ( " gladeFunc, this will provide a command to access items(?) resulting from the loading of this tree!\n" );
#endif
	return TCL_OK;
}


/**
\brief  Dummy function for assigning to newly created gnocl widget within this module
\author William J Giddings
\date   23/11/09
\note
*/
static int gladeWidgetFunc ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] )
{
#ifdef DEBUG_BUILDER
	g_print ( " gladeWidgetFunc, this is just a stub!\n" );
#endif
	return TCL_OK;
}





/**
/brief  Load a Glade file and create user interface.
        Register widget, and create variables holding names based upon id given in the glade file.
/author William J Giddings
/date   22-Nov-09
/todo   Obtain widget tree and then register widgets with gnocl.
**/
int gnoclGladeCmd (
	ClientData data,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj * const objv[] )
{
	static const char *cmds[] = { "file", "buffer", NULL };
	enum optIdx { FileIdx, BufferIdx };
	int idx;

	GladeXML *xml;

	if ( objc != 3 )
	{
		Tcl_WrongNumArgs ( interp, 1, objv, "GNOCL ERROR: option widgetid " );
		return TCL_ERROR;
	}

	if ( Tcl_GetIndexFromObj ( interp, objv[1], cmds, "command", TCL_EXACT, &idx ) != TCL_OK )
	{
		return TCL_ERROR;
	}

	switch ( idx )
	{
		case FileIdx:
			{
#ifdef DEBUG_BUILDER
				printf ( ".....load from file\n" );
#endif
				xml = glade_xml_new ( Tcl_GetString ( objv[2] ), NULL, NULL );
			}
			break;
		case BufferIdx:
			{
				char *glade_ui = Tcl_GetString ( objv[2] );
				xml = glade_xml_new_from_buffer ( glade_ui, strlen ( glade_ui ) * sizeof ( gchar ), NULL, NULL );
#ifdef DEBUG_BUILDER
				printf ( ".....load from buffer\n" );
#endif
			}
			break;
	}

	GtkWidget *window;
	GtkWidget *widget;

	GList *widgetList, *p;
	gchar str[50];

	/* process remaining widgets in the gui description */
	widgetList = glade_xml_get_widget_prefix ( xml, "" );

	/*  build up a list of names */
	Tcl_Obj *resList = Tcl_NewListObj ( 0, NULL );

	/* ignore gnomeApp widgets, these are handled in their own modules */
	for ( p = widgetList; p != NULL; p = p->next )
	{

		widget = GTK_WIDGET (  p->data );

		const char *name = gnoclGetAutoWidgetId();

		/* name and type, add to list */
		gchar *gladeName = glade_get_widget_name ( widget );

		gchar *type =  G_OBJECT_TYPE_NAME ( widget );

		/* determine how to handle the new widget */
		gint idx = compare ( type, GtkTypes );

		/* now, effectively a multi-widget gnoclRegisterWidget function */
		setGtkWidgetFunc ( interp, widget, idx , resList, gladeName, name );

		if ( 0 )
		{
			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkWindow" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkWindow\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkAboutDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkAboutDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkColorSelectionDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkColorSelectionDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkFileChooserDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkFileChooserDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkFontSelectionDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkFontSelectionDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkInputDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkInputDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkMessageDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkMessageDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkRecentChooserDialog" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkRecentChooserDialog\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

			/* search out the toplevel window for this file */
			if ( strcmp ( type, "GtkAssistant" ) == 0 )
			{
#ifdef DEBUG_BUILDER
				g_print ( "I am the GtkAssistant\n" );
#endif
				window = GTK_WIDGET (  p->data );
			}

		}

	}

	/* create a specific command for each of the glade files loaded */
	Tcl_CreateObjCommand ( interp, Tcl_GetString ( objv[2] ), gladeFunc, xml, NULL );

	/* return the list of created widgets */
	Tcl_SetObjResult ( interp, resList );
	return TCL_OK;

}
