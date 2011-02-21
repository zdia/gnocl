/**
 \brief  This file implements a Tcl interface to gnome and GTK+
 \authors   Peter G. Baum, William J Giddings
 \date 2001-03:
*/

#ifndef GNOCL_H_INCLUDED
#define GNOCL_H_INCLUDED

/*
 * $Id: gnocl.h,v 1.36 2005/08/16 20:57:45 baum Exp $
 *
 * This file implements a Tcl interface to gnome and GTK+
 *
 * Copyright (c) 2001 - 2005 Peter G. Baum  http://www.dr-baum.net
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include "tcl.h"
#include <gtk/gtk.h>

#define GNOCL_PAD_TINY  2
#define GNOCL_PAD_SMALL 4
#define GNOCL_PAD       8
#define GNOCL_PAD_BIG  12

/* string prefix to mark stock items */
#define GNOCL_STOCK_PREFIX "%#"
/* transfer string ownership */
#define GNOCL_MOVE_STRING(src,dest) \
      do{ g_free(dest); dest = src; src = NULL; } while( 0 )
/* transfer obj ownership */
#define GNOCL_MOVE_OBJ(src,dest) \
      do{ if(dest) Tcl_DecrRefCount( dest ); \
      dest = src; Tcl_IncrRefCount( dest ); } while( 0 )

typedef enum GnoclStringType_
{
	GNOCL_STR_EMPTY     = 0,      /* empty string */
	GNOCL_STR_STR       = 1 << 0, /* normal string */
	GNOCL_STR_STOCK     = 1 << 1, /* (potentially) the name of a stock item */
	GNOCL_STR_FILE      = 1 << 2, /* (potentially) the name of a file */
	GNOCL_STR_TRANSLATE = 1 << 3, /* to be translated via gettext */
	GNOCL_STR_UNDERLINE = 1 << 4, /* '_' marks underline und accelerator */
	GNOCL_STR_MARKUP    = 1 << 5,  /* markup for label */
	GNOCL_STR_BUFFER    = 1 << 6  /* (potentially) the name of a pixbuf */
} GnoclStringType;

GnoclStringType gnoclGetStringType ( Tcl_Obj *obj );
char *gnoclGetString ( Tcl_Obj *op );
char *gnoclGetStringFromObj ( Tcl_Obj *op, int *len );
char *gnoclGetStringUline ( Tcl_Obj *op, char **pattern );
char *gnoclStringDup ( Tcl_Obj *op );
char *gnoclGetStockLabel ( Tcl_Obj *obj, Tcl_Interp *interp );
int gnoclGetStockItem ( Tcl_Obj *obj, Tcl_Interp *interp, GtkStockItem *sp );
Tcl_Obj *gnoclGtkToStockName ( const char *gtk );
const char *gnoclGetAppName ( Tcl_Interp *interp );
const char *gnoclGetAppVersion ( Tcl_Interp *interp );
char **gnoclGetArgv ( Tcl_Interp *interp, int *argc );

int gnoclRegisterWidget ( Tcl_Interp *interp, GtkWidget *widget,
						  Tcl_ObjCmdProc *proc );

/* WJG Added 05-03-09 */
int gnoclRegisterPixbuf ( Tcl_Interp *interp, GdkPixbuf *pixbuf, Tcl_ObjCmdProc *proc );


int gnoclMemNameAndWidget ( const char *name, GtkWidget *widget );
int gnoclForgetWidgetFromName ( const char *name );
char *gnoclGetAutoWidgetId ( void );
const char *gnoclGetNameFromWidget ( GtkWidget *widget );
GtkWidget *gnoclGetWidgetFromName ( const char *name, Tcl_Interp *interp );
GtkWidget *gnoclChildNotPacked ( const char *name, Tcl_Interp *interp );
int gnoclAssertNotPacked ( GtkWidget *child, Tcl_Interp *interp,
						   const char *name );

int gnoclGetBothAlign ( Tcl_Interp *interp, Tcl_Obj *obj, gfloat *xAlign,
						gfloat *yAlign );
int gnoclGetPadding ( Tcl_Interp *interp, Tcl_Obj *obj, int *pad );

enum GnoclOptionType
{
	GNOCL_STRING,
	GNOCL_DOUBLE,
	GNOCL_INT,
	GNOCL_BOOL,
	GNOCL_OBJ,
	GNOCL_LIST
};

typedef struct
{
	char *command;
	Tcl_Interp *interp;
	void       *data;
} GnoclCommandData;

typedef struct
{
	char c;
	enum GnoclOptionType type;
	union
	{
		double     d;
		int        i;
		int        b;
		const char *str;
		Tcl_Obj    *obj;
	}    val;
} GnoclPercSubst;

const char *gnoclPercentSubstitution ( GnoclPercSubst *p, int no,
									   const char *str );

enum GnoclOptionStatus
{
	/* the order is important for gnoclClearOptions */
	GNOCL_STATUS_CLEAR,
	GNOCL_STATUS_CHANGED_ERROR,
	GNOCL_STATUS_CHANGED,
	GNOCL_STATUS_SET_ERROR,
	GNOCL_STATUS_SET
};

enum GnoclCgetReturn
{
	GNOCL_CGET_ERROR,
	GNOCL_CGET_HANDLED,
	GNOCL_CGET_NOTHANDLED
};

struct GnoclOption_;
typedef int ( gnoclOptFunc ) ( Tcl_Interp *, struct GnoclOption_ *, GObject *, Tcl_Obj **ret );

typedef struct GnoclOption_
{
	const char           *optName;
	enum GnoclOptionType type;
	const char           *propName;        /* NULL for no automatic setting */
	gnoclOptFunc         *func;
	enum GnoclOptionStatus status;
	union
	{
		gboolean b;
		gint     i;
		gdouble  d;
		gchar    *str;
		Tcl_Obj  *obj;
	}          val;
} GnoclOption;


/* WJG STUFF */

int gnoclOptWindowCenter ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptGdkBaseFont ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

/*
static void doOnEvent (
    GtkTextTag *texttag,
    GObject *arg1,
    GdkEvent *event,
    GtkTextIter *arg2,
    gpointer data );

static void _doOnEvent (
    GtkTextTag *texttag,
    GObject *arg1,
    GdkEvent *event,
    GtkTextIter *arg2,
    gpointer data );
*/

gnoclOptFunc gnoclOptOnEvent;
gnoclOptFunc gnoclOptOnScroll;

/* WJG STUFF ENDS */


int gnoclOptGeneric ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, const char *optName, const char *txt[], const int types[], Tcl_Obj **ret );

gnoclOptFunc gnoclOptAnchor;
gnoclOptFunc gnoclOptBothAlign;
gnoclOptFunc gnoclOptChild;
gnoclOptFunc gnoclOptCommand;
gnoclOptFunc gnoclOptData;
gnoclOptFunc gnoclOptDnDTargets;
gnoclOptFunc gnoclOptGdkColor;
gnoclOptFunc gnoclOptGdkBaseFont;
gnoclOptFunc gnoclOptGdkColorBase;
gnoclOptFunc gnoclOptGdkColorBg;
gnoclOptFunc gnoclOptGdkColorFg;

gnoclOptFunc gnoclOptGdkColorText;
gnoclOptFunc gnoclOptHalign;
gnoclOptFunc gnoclOptIcon;
gnoclOptFunc gnoclOptJustification;
gnoclOptFunc gnoclOptEllipsize;
gnoclOptFunc gnoclOptLabelFull;

gnoclOptFunc gnoclOptOnButton;
gnoclOptFunc gnoclOptOnSelectionChanged;

gnoclOptFunc gnoclOptOnFontSet;
gnoclOptFunc gnoclOptOnFileSet;
gnoclOptFunc gnoclOptOnFolderSet;

gnoclOptFunc gnoclOptOnConfigure;
gnoclOptFunc gnoclOptOnDelete;
gnoclOptFunc gnoclOptOnDragData;
gnoclOptFunc gnoclOptOnDropData;
gnoclOptFunc gnoclOptOnEnterLeave;
gnoclOptFunc gnoclOptOnKeyPress;
gnoclOptFunc gnoclOptOnKeyRelease;
gnoclOptFunc gnoclOptOnMotion;
gnoclOptFunc gnoclOptOnButtonMotion;

/* WJG ADDED 18/02/02 */
gnoclOptFunc gnoclOptCharWidth;

/* WJG ADDED 22/03/08 */
gnoclOptFunc gnoclOptOnBackSpace;
gnoclOptFunc gnoclOptOnClipboard;
gnoclOptFunc gnoclOptOnTextInsert;
gnoclOptFunc gnoclOptOnChanged;
gnoclOptFunc gnoclOptOnModified;
gnoclOptFunc gnoclOptOnApplyTag;
gnoclOptFunc gnoclOptOnRemoveTag;
gnoclOptFunc gnoclOptOnFocus;
gnoclOptFunc gnoclOptOnEnterLeave;
gnoclOptFunc gnoclOptOnBeginUserAction;
gnoclOptFunc gnoclOptOnEndUserAction;
gnoclOptFunc gnoclOptOnInsertChildAnchor;
gnoclOptFunc gnoclOptOnLInkButton;

gnoclOptFunc gnoclOptOnDeletFromCursor;
gnoclOptFunc gnoclOptOnInsertAtCursor;
gnoclOptFunc gnoclOptOnMoveCursor;
gnoclOptFunc gnoclOptOnMoveFocus;
gnoclOptFunc gnoclOptOnMoveViewport;
gnoclOptFunc gnoclOptOnPageHorizontally ;
gnoclOptFunc gnoclOptOnPasteClipboard ;
gnoclOptFunc gnoclOptOnPopulatePopup;
gnoclOptFunc gnoclOptOnSelectAll;
gnoclOptFunc gnoclOptOnSetAnchor;
gnoclOptFunc gnoclOptOnScrollAdjustments;
gnoclOptFunc gnoclOptOnToggleOverwrite;
gnoclOptFunc gnoclOptIcons;

gnoclOptFunc gnoclOptOnPasteDone;   /* WJG Added 15/Jan/2010 */

/* WJG ADDED 29/07/08 */
gnoclOptFunc gnoclOptOnDelete;
gnoclOptFunc gnoclOptOnMarkSet;
gnoclOptFunc gnoclOptOnMarDelete;

gnoclOptFunc gnoclOptOnShowHelp;
gnoclOptFunc gnoclOptPadding;
gnoclOptFunc gnoclOptPangoScaledInt;
gnoclOptFunc gnoclOptPangoStretch;
gnoclOptFunc gnoclOptPangoStyle;
gnoclOptFunc gnoclOptPangoVariant;
gnoclOptFunc gnoclOptPangoWeight;

gnoclOptFunc gnoclOptPosition;
gnoclOptFunc gnoclOptRelief;
gnoclOptFunc gnoclOptRGBAColor;
gnoclOptFunc gnoclOptScale;
gnoclOptFunc gnoclOptShadow;
gnoclOptFunc gnoclOptSizeGroup;
gnoclOptFunc gnoclOptTooltip;
gnoclOptFunc gnoclOptUnderline;
gnoclOptFunc gnoclOptWidget;
gnoclOptFunc gnoclOptWindowTypeHint;
gnoclOptFunc gnoclOptWrapmode;

gnoclOptFunc gnoclOptTransientWindow;


/* windows options */
/* wjg added 23/03/09 */
gnoclOptFunc gnoclOptPopupMenu;


/* wjg added 06/02/09 */
gnoclOptFunc gnoclOptMask;
gnoclOptFunc gnoclOptMask2;
gnoclOptFunc gnoclOptBackgroundImage;
gnoclOptFunc gnoclOptBackgroundImage2;
gnoclOptFunc gnoclOptHasFrame;

gnoclOptFunc gnoclOptShadow;
gnoclOptFunc gnoclOptArrow;
gnoclOptFunc gnoclOptCursor;

/* wjg added 15/01/09 */
gnoclOptFunc gnoclOptKeepAbove;
gnoclOptFunc gnoclOptKeepBelow;
gnoclOptFunc gnoclOptOpacity;
gnoclOptFunc gnoclOptStick;
gnoclOptFunc gnoclOptFullScreen;

/* wjg added 06/12/08 */
gnoclOptFunc gnoclGetParent;

/* wjg added 24/10/09 */
gnoclOptFunc gnoclOptOnActivate;
gnoclOptFunc gnoclOptOnBackspace;
gnoclOptFunc gnoclOptOnDeleteFromCursor;
gnoclOptFunc gnoclOptOnEntryInsert;

int gnoclClearOptions ( GnoclOption *opts );
int gnoclResetSetOptions ( GnoclOption *opts );
int gnoclSetOptions ( Tcl_Interp *interp,
					  GnoclOption *opts, GObject *object, int no );
int gnoclGetIndexFromObjStruct ( Tcl_Interp *interp, Tcl_Obj *objPtr,
								 char **tablePtr, int offset, char *msg, int flags, int *indexPtr );
int gnoclParseOptions ( Tcl_Interp *interp, int objc,
						Tcl_Obj * const objv[], GnoclOption *opts );
int gnoclParseAndSetOptions ( Tcl_Interp *interp, int objc,
							  Tcl_Obj * const objv[], GnoclOption *opts, GObject *object );
int gnoclCget ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[],
				GObject *gObj, GnoclOption *opts, int *idx );
int gnoclCgetOne ( Tcl_Interp *interp, Tcl_Obj *obj,
				   GObject *gObj, GnoclOption *opts, int *idx );
int gnoclCgetNotImplemented ( Tcl_Interp *interp, GnoclOption *opt );
Tcl_Obj *gnoclCgetButtonText ( Tcl_Interp *interp, GtkButton *button );
int gnoclConfigButtonText ( Tcl_Interp *interp, GtkButton *button, Tcl_Obj *txtObj );

/* wjg added 17/01/09 */
int gnoclButtonSetSize ( Tcl_Interp  *interp, GnoclOption *opt, GObject     *obj, Tcl_Obj     **ret );
/*-----*/

/* added 22/11/09 */
int gnoclGladeCmd ( ClientData data,    Tcl_Interp *interp, int objc,   Tcl_Obj * const objv[] );

int gnoclBuilderCmd (   ClientData data,    Tcl_Interp *interp, int objc,   Tcl_Obj * const objv[] );


const char *gnoclGetOptCmd ( GObject *obj, const char *signal );
int gnoclDisconnect ( GObject *obj, const char *signal, GCallback handler );
int gnoclConnectOptCmd ( Tcl_Interp *interp, GObject *object, const char *signal, GCallback handler, GnoclOption *opt, void *data, Tcl_Obj **ret );

/* WJG added 26/07/08 to handle specific Gtk textview signals */
int gnoclOptOnInsertText ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclConnectSignalCmd ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnDeleteRange ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

int gnoclOptOnChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnModified ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnApplyTag ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnBeginUserAction  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnEndUserAction  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnInsertChildAnchor  ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnRemoveTag ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnMarkSet ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnMarkDelete ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnChildAttached ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnChildDetached ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnLinkButton ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnValueChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );

/* WJG added 16/03/10 to handle Gtk ColorWheel signals */
int gnoclOptOnColorWheelChanged ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
int gnoclOptOnColorWheelMove ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );
/*-----*/

/* WJG added 25/04/10 to handle entry activation */
int gnoclOptOnACtivate ( Tcl_Interp *interp, GnoclOption *opt, GObject *obj, Tcl_Obj **ret );


int gnoclDelete ( Tcl_Interp *interp, GtkWidget *widget,
				  int objc, Tcl_Obj * const objv[] );
int gnoclAttacheVariable ( GnoclOption *newVar, char **oldVar,
						   const char *signal, GObject *obj, GCallback gtkFunc,
						   Tcl_Interp *interp, Tcl_VarTraceProc tclFunc,
						   gpointer data );
int gnoclAttacheOptCmdAndVar ( GnoclOption *newCmd, char **oldCmd,
							   GnoclOption *newVar, char **oldVar,
							   const char *signal,
							   GObject *obj, GCallback gtkFunc,
							   Tcl_Interp *interp, Tcl_VarTraceProc tclFunc,
							   gpointer data );
int gnoclGetScrollbarPolicy ( Tcl_Interp *interp, Tcl_Obj *obj,
							  GtkPolicyType *hor, GtkPolicyType *vert );
int gnoclGetSelectionMode ( Tcl_Interp *interp, Tcl_Obj *obj,
							GtkSelectionMode *selection );
int gnoclGetFontTxt ( Tcl_Interp *interp, Tcl_Obj *obj, const char **font );
int gnoclGetGdkFont ( Tcl_Interp *interp, Tcl_Obj *obj, GdkFont **font );
int gnoclGetGdkColorAlloc ( Tcl_Interp *interp, Tcl_Obj *obj,
							GtkWidget *widget, GdkColor *color, int *a );
int gnoclGetAnchorStyle ( Tcl_Interp *interp, Tcl_Obj *obj,
						  GtkAnchorType *style );
int gnoclGetJustification ( Tcl_Interp *interp, Tcl_Obj *obj,
							GtkJustification *type );
int gnoclGetOrientationType ( Tcl_Interp *interp, Tcl_Obj *obj,
							  GtkOrientation *orient );
int gnoclGetImage ( Tcl_Interp *interp,
					Tcl_Obj *obj, GtkIconSize size, GtkWidget **widget );
GtkTooltips *gnoclGetTooltips( );
GtkAccelGroup *gnoclGetAccelGroup( );

int gnoclEditablePosToIndex ( Tcl_Interp *interp, Tcl_Obj *obj,
							  GtkEditable *editable, int *pidx );
int gnoclHandleEditableCmds ( int idx, Tcl_Interp *interp, int objc,
							  Tcl_Obj * const objv[], GtkEditable *editable );

#ifdef GNOCL_USE_GNOME
int gnoclRegisterHintAppBar ( GtkWidget *widget, GnomeAppBar *appBar );
GnomeAppBar *gnoclGetHintAppBar ( GtkWidget *widget );
#endif

/* in helperFuncs */
GtkWidget *gnoclFindChild ( GtkWidget *widget, GtkType type );
int gnoclPosOffset ( Tcl_Interp *interp, const char *txt, int *offset );
int gnoclPercentSubstAndEval ( Tcl_Interp *interp, GnoclPercSubst *ps,
							   const char *orig_script, int background );
int gnoclGet2Boolean ( Tcl_Interp *interp, Tcl_Obj *obj, int *b1, int *b2 );
int gnoclGet2Int ( Tcl_Interp *interp, Tcl_Obj *obj, int *b1, int *b2 );
int gnoclGet2Double ( Tcl_Interp *interp, Tcl_Obj *obj, double *b1, double *b2 );
GdkPixbuf *gnoclPixbufFromObj ( Tcl_Interp *interp, GnoclOption *opt );

/* WJG Added 16-02-09 */
GdkPixbuf *gnoclBlendPixbufFromObj ( Tcl_Interp *interp, GnoclOption *opt );

/* WJG Added 16-01-09 */
char *gnoclGetWidgetGeometry ( GtkWidget *widget );

/* in menu */
int gnoclMenuShellAddChildren ( Tcl_Interp *interp, GtkMenuShell *shell,
								Tcl_Obj *children, int atEnd );

/* in menuItem */
Tcl_Obj *gnoclCgetMenuItemAccel ( Tcl_Interp *interp, GtkMenuItem *item );
int gnoclMenuItemHandleAccel ( Tcl_Interp *interp, GtkMenuItem *item,
							   Tcl_Obj *accelObj );
Tcl_Obj *gnoclCgetMenuItemText ( Tcl_Interp *interp, GtkMenuItem *item );
int gnoclMenuItemHandleText ( Tcl_Interp *interp, GtkMenuItem *item,
							  Tcl_Obj *textObj );

/* in text.c */
int gnoclTextCommand ( GtkTextBuffer *buffer, Tcl_Interp *interp, int objc,
					   Tcl_Obj * const objv[], int cmdNo, int allowDeleteConfigure );

/* in sourceVSiew.c */

/*
 * recentChooserDialog declarations
 * WJG 16/06/08
 */
int gnoclRecentChooserDialogCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

/*
 * scaleButton declarations
 * WJG 14/06/08
 */
Tcl_Obj *gnoclVolumeButtonText ( Tcl_Interp *interp, GtkWidget *widget );
int gnoclConfigVolumeButtonText ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj );
int gnoclVolumeButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );



/*
 * scaleButton declarations
 * WJG 14/06/08
 */
Tcl_Obj *gnoclCgetScaleButtonText ( Tcl_Interp *interp, GtkWidget *widget );
int gnoclConfigScaleButtonText ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj );
int gnoclScaleButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

/*
 * linkButton declarations
 * WJG 14/06/08
 */
Tcl_Obj *gnoclCgetLinkButtonText ( Tcl_Interp *interp, GtkWidget *widget );
int gnoclConfigLinkButtonText ( Tcl_Interp *interp, GtkWidget *widget, Tcl_Obj *txtObj );
int gnoclLinkButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );
int gnoclArrowButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );
int gnoclAcceleratorCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

/*
 * WJG 20/12/09
 */
int gnoclRecentChooserCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

/*
 * toggleButton declarations
 * WJG 14/06/08
 */

/* in toggleButton.c  for menuCheckItem and toolBar checkItem */

typedef struct
{
	char       *name;
	Tcl_Interp *interp;
	GtkWidget  *widget;
	char       *onToggled;
	char       *variable;
	Tcl_Obj    *onValue;
	Tcl_Obj    *offValue;
	int        inSetVar;
} GnoclToggleParams;


/***s* spinButton/SpinButtonParams
 * AUTHOR
 *  PGB
 * SOURCE
 */

typedef struct
{
	char          *name;
	char          *variable;
	char          *onValueChanged;
	GtkSpinButton *spinButton;
	Tcl_Interp    *interp;
	int           inSetVar;
} SpinButtonParams;



/*
 * file & folderChooseButton declarations
 * WJG 11/07/08
 */
int gnoclFileChooserButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );
int gnoclFolderChooserButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );


/*
 * LugRadioLive demo stuff
 * WJG 16/07/08
 */
int gnoclDrawingAreaCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

void gnoclToggleDestroyFunc ( GtkWidget *widget, gpointer data );
int gnoclToggleToggle ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[], GnoclToggleParams *para );
int gnoclToggleSetActive ( GnoclToggleParams *para, GnoclOption *active );
int gnoclToggleSetValue ( GnoclToggleParams *para, Tcl_Obj *obj );
void gnoclToggleToggledFunc ( GtkWidget *widget, gpointer data );
char *gnoclToggleTraceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags );
int gnoclToggleVariableValueChanged ( GnoclToggleParams *para );
int gnoclToggleButtonCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );
int gnoclHandleBoxCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );
int gnoclAssistantCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

/*
 * VteTerminal widget stuff
 * WJG 20/07/08
 */
int gnoclVteTerminalCmd ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );


/*
 * checkButton declarations
 */
/* in checkButton.c  for menuCheckItem and toolBar checkItem */

typedef struct
{
	char       *name;
	Tcl_Interp *interp;
	GtkWidget  *widget;
	char       *onToggled;
	char       *variable;
	Tcl_Obj    *onValue;
	Tcl_Obj    *offValue;
	int        inSetVar;
} GnoclCheckParams;

int gnoclCheckIsOn ( Tcl_Interp *interp, Tcl_Obj *onValue,
					 Tcl_Obj *offValue, Tcl_Obj *val );
void gnoclCheckDestroyFunc ( GtkWidget *widget, gpointer data );
void gnoclCheckToggledFunc ( GtkWidget *widget, gpointer data );
char *gnoclCheckTraceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags );
int gnoclCheckSetValue ( GnoclCheckParams *para, Tcl_Obj *obj );
int gnoclCheckOnToggled ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[],
						  GnoclCheckParams *para );
int gnoclCheckSetActive ( GnoclCheckParams *para, GnoclOption *opt );
int gnoclCheckVariableValueChanged ( GnoclCheckParams *para );

/* in radioButton.c  for menuRadioItem */

typedef struct
{
	Tcl_Interp *interp;
	GArray     *widgets;
	int        inSetVar;
	char       *variable;
	/* GSList    *gtkGroup; */
} GnoclRadioGroup;

/*
 * radioButton declarations
 */

typedef struct
{
	char            *name;
	GnoclRadioGroup *group;
	GtkWidget       *widget;
	char            *onToggled;
	Tcl_Obj         *onValue;
} GnoclRadioParams;


/*
 * radioButton declarations
 */

GnoclRadioGroup *gnoclRadioGroupNewGroup ( const char *var, Tcl_Interp *interp );
int gnoclRadioGroupAddWidgetToGroup ( GnoclRadioGroup *group,
									  GnoclRadioParams *para );
int gnoclRadioRemoveWidgetFromGroup ( GnoclRadioGroup *group,
									  GnoclRadioParams *para );
GnoclRadioParams *gnoclRadioGetActivePara ( GnoclRadioGroup *group );
GnoclRadioGroup *gnoclRadioGetGroupFromVariable ( const char *var );
GnoclRadioParams *gnoclRadioGetParam ( GnoclRadioGroup *group, int n );
int gnoclRadioSetValueActive ( GnoclRadioParams *para, GnoclOption *value,
							   GnoclOption *active );
void gnoclRadioDestroyFunc ( GtkWidget *widget, gpointer data );
void gnoclRadioToggledFunc ( GtkWidget *widget, gpointer data );
char *gnoclRadioTraceFunc ( ClientData data, Tcl_Interp *interp, const char *name1, const char *name2, int flags );
Tcl_Obj *gnoclRadioGetValue ( GnoclRadioParams *para );
int gnoclRadioSetValue ( GnoclRadioParams *para, Tcl_Obj *val );
int gnoclRadioOnToggled ( Tcl_Interp *interp, int objc, Tcl_Obj * const objv[],
						  GnoclRadioParams *para );

Tcl_ObjCmdProc gnoclDebugCmd;
Tcl_ObjCmdProc gnoclCallbackCmd;
Tcl_ObjCmdProc gnoclClipboardCmd;

Tcl_ObjCmdProc gnoclConfigureCmd;
Tcl_ObjCmdProc gnoclInfoCmd;
Tcl_ObjCmdProc gnoclMainLoop;
Tcl_ObjCmdProc gnoclUpdateCmd;

Tcl_ObjCmdProc gnoclResourceFileCmd;
Tcl_ObjCmdProc gnoclWinfoCmd;
Tcl_ObjCmdProc gnoclScreenCmd;

Tcl_ObjCmdProc gnoclPrintCmd;
Tcl_ObjCmdProc gnoclPixBufCmd;
Tcl_ObjCmdProc gnoclPixMapCmd;

Tcl_ObjCmdProc gnoclClr2RGBCmd;
Tcl_ObjCmdProc gnoclRGB2HexCmd;

Tcl_ObjCmdProc gnoclStatusIconCmd;


Tcl_ObjCmdProc gnoclAboutDialogCmd;
Tcl_ObjCmdProc gnoclActionCmd;
Tcl_ObjCmdProc gnoclButtonCmd;
Tcl_ObjCmdProc gnoclBoxCmd;
Tcl_ObjCmdProc gnoclCheckButtonCmd;
Tcl_ObjCmdProc gnoclColorButtonCmd;
Tcl_ObjCmdProc gnoclColorSelectionCmd;
Tcl_ObjCmdProc gnoclColorWheelCmd;
Tcl_ObjCmdProc gnoclComboBoxCmd;
Tcl_ObjCmdProc gnoclComboEntryCmd;
Tcl_ObjCmdProc gnoclComboCmd;
Tcl_ObjCmdProc gnoclDialogCmd;
Tcl_ObjCmdProc gnoclEntryCmd;
Tcl_ObjCmdProc gnoclEventBoxCmd;
Tcl_ObjCmdProc gnoclExpanderCmd;
Tcl_ObjCmdProc gnoclFileSelectionCmd;
Tcl_ObjCmdProc gnoclFileChooserCmd;

/* anticipate some problems with this clashing old codes */
Tcl_ObjCmdProc gnoclFileChooserDialogCmd;
Tcl_ObjCmdProc gnoclFontSelectionDialogCmd;
Tcl_ObjCmdProc gnoclColorSelectionDialogCmd;

Tcl_ObjCmdProc gnoclFontButtonCmd;
Tcl_ObjCmdProc gnoclFontSelectionCmd;
Tcl_ObjCmdProc gnoclImageCmd;
Tcl_ObjCmdProc gnoclLabelCmd;
Tcl_ObjCmdProc gnoclListCmd;
Tcl_ObjCmdProc gnoclMenuCmd;
Tcl_ObjCmdProc gnoclMenuBarCmd;
Tcl_ObjCmdProc gnoclMenuItemCmd;
Tcl_ObjCmdProc gnoclMenuCheckItemCmd;
Tcl_ObjCmdProc gnoclMenuRadioItemCmd;
Tcl_ObjCmdProc gnoclMenuSeparatorCmd;
Tcl_ObjCmdProc gnoclNotebookCmd;
Tcl_ObjCmdProc gnoclOptionMenuCmd;
Tcl_ObjCmdProc gnoclPanedCmd;
Tcl_ObjCmdProc gnoclPlugCmd;
Tcl_ObjCmdProc gnoclProgressBarCmd;
Tcl_ObjCmdProc gnoclRadioButtonCmd;
Tcl_ObjCmdProc gnoclScaleCmd;
Tcl_ObjCmdProc gnoclScrolledWindowCmd;
Tcl_ObjCmdProc gnoclSeparatorCmd;
Tcl_ObjCmdProc gnoclSocketCmd;

Tcl_ObjCmdProc gnoclSpinButtonCmd;
Tcl_ObjCmdProc gnoclStatusBarCmd;
Tcl_ObjCmdProc gnoclTableCmd;
Tcl_ObjCmdProc gnoclTextCmd;
Tcl_ObjCmdProc gnoclToolBarCmd;
Tcl_ObjCmdProc gnoclTreeCmd;

Tcl_ObjCmdProc gnoclWindowCmd;

Tcl_ObjCmdProc gnoclCalendarCmd;
Tcl_ObjCmdProc gnoclCurveCmd;
Tcl_ObjCmdProc gnoclGammaCurveCmd;
Tcl_ObjCmdProc gnoclRulerCmd;

Tcl_ObjCmdProc gnoclIconViewCmd;
Tcl_ObjCmdProc gnoclAssistantCmd;

/* some simple megawidgets */
Tcl_ObjCmdProc gnoclLabelEntryCmd;

/* extra debugging func */
Tcl_ObjCmdProc gnoclPutsOb;

/* some miscellaneous functions */
Tcl_ObjCmdProc gnoclSignalStopCmd;
Tcl_ObjCmdProc gnoclBeepCmd;
Tcl_ObjCmdProc gnoclSnapshotCmd;

#ifdef GNOCL_USE_GNOME
Tcl_ObjCmdProc gnoclAboutCmd;
Tcl_ObjCmdProc gnoclAppCmd;
Tcl_ObjCmdProc gnoclAppBarCmd;
#endif

#endif


/* the following constructors now need to be made public in order to run glade */

int setGtkWidgetFunc ( Tcl_Interp *interp, GtkWidget *widget, int idx, Tcl_Obj *resList , gchar *gladeName, char *name );

typedef int ( gnoclWidgetFunc ) ( ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] );

gnoclWidgetFunc windowFunc;
gnoclWidgetFunc textViewFunc;
gnoclWidgetFunc labelFunc;
gnoclWidgetFunc toolButtonFunc;
gnoclWidgetFunc statusBarFuc;
gnoclWidgetFunc toolBarFunc;
gnoclWidgetFunc buttonFunc;
gnoclWidgetFunc entryFunc;
gnoclWidgetFunc toggleButtonFunc;
gnoclWidgetFunc boxFunc;
gnoclWidgetFunc menuBarFunc;
gnoclWidgetFunc menuFunc;
gnoclWidgetFunc menuItemFunc;
gnoclWidgetFunc checkButtonFunc;
gnoclWidgetFunc spinButtonFunc;
gnoclWidgetFunc radioButtonFunc;
gnoclWidgetFunc fileChooserButtonFunc;
gnoclWidgetFunc clrButtonFunc;
gnoclWidgetFunc fontButtonFunc;
gnoclWidgetFunc linkButtonFunc;
gnoclWidgetFunc scaleButtonFunc;
gnoclWidgetFunc volumeButtonFunc;
gnoclWidgetFunc imageFunc;
gnoclWidgetFunc scaleFunc;
gnoclWidgetFunc scrollFunc;
gnoclWidgetFunc comboBoxFunc;
gnoclWidgetFunc progressBarFunc;
gnoclWidgetFunc treeListFunc;
gnoclWidgetFunc iconViewFunc;
gnoclWidgetFunc handleBoxFunc;
gnoclWidgetFunc calendarFunc;
gnoclWidgetFunc separatorFunc;
gnoclWidgetFunc arrowButtonFunc;
gnoclWidgetFunc drawingAreaFunc;
gnoclWidgetFunc recentChooserFunc;
gnoclWidgetFunc aboutDialogFunc;
gnoclWidgetFunc dialogFunc;
gnoclWidgetFunc colorSelDialogFunc;
gnoclWidgetFunc colorSelectionFunc;
gnoclWidgetFunc fileDialogFunc;
gnoclWidgetFunc expanderFunc;
gnoclWidgetFunc scrlWindowFunc;
gnoclWidgetFunc fontSelDialogFunc;
gnoclWidgetFunc inputDialogFunc;
gnoclWidgetFunc recentChooserDialogFunc;
gnoclWidgetFunc assistantFunc;
gnoclWidgetFunc accelaratorFunc;
gnoclWidgetFunc panedFunc;
gnoclWidgetFunc notebookFunc;
gnoclWidgetFunc eventBoxFunc;
gnoclWidgetFunc fileChooserFunc;

/* whole block removed into gnoclparams.h */


/* appended by module-assistant-pkg */
Tcl_ObjCmdProc gnoclTextBufferCmd;

/* used in creating lists from widget and pixbuf hashtables */
void hash_to_list ( gpointer key, gpointer value, gpointer user_data );
gint sorter ( gconstpointer a, gconstpointer b );

