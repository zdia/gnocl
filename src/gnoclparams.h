
/*
gnoclparams.h
*/

/**
\brief    Function associated with the widget.
\note     These are specific to the gnocl package alone, perhaps these
           should be put into specific library?
*/
typedef struct
{
	GtkEntry    *entry;
	Tcl_Interp  *interp;
	char        *name;
	char        *variable;
	char        *onChanged;
	int         inSetVar;
} EntryParams;


typedef struct
{
	GtkLabel    *label;
	Tcl_Interp  *interp;
	char        *name;
	char        *textVariable;
	char        *onChanged;
	int         inSetVar;
} LabelParams;

typedef struct
{
	char       *name;
	Tcl_Interp *interp;
	GtkScale   *scale;
	char       *onValueChanged;
	char       *variable;
	int        inSetVar;
} ScaleParams;

typedef struct
{
	GtkLabel    *label;
	Tcl_Interp  *interp;
	char        *name;
	char        *textVariable;
	char        *onChanged;
	int         inSetVar;
} AccelaratorParams;

typedef struct
{
	char         *name;
	Tcl_Interp   *interp;
	GtkScrollbar *scroll;
	char         *onValueChanged;
	char         *variable;
	int          inSetVar;
} ScrollParams;


typedef struct
{
	char        *name;
	Tcl_Interp  *interp;
	GtkComboBox *comboBox;
	char        *onChanged;
	char        *variable;
	int         inSetVar;
} ComboBoxParams;

typedef struct
{
	Tcl_Interp        *interp;
	char              *name;
	GtkTreeView       *view;
	GtkScrolledWindow *scrollWin;
	int               noColumns;
	int               isTree;
	GHashTable        *idToIter;
} TreeListParams;

typedef struct
{
	char       *onResponse;
	char       *name;
	GtkDialog  *dialog;
	Tcl_Interp *interp;
	GPtrArray  *butRes;
	int        isModal;
	int        ret;
} DialogParams;

typedef struct
{
	char       *onResponse;
	char       *name;
	GtkDialog  *dialog;
	Tcl_Interp *interp;
	GPtrArray  *butRes;
	int        isModal;
	int        ret;
} InputDialogParams;


typedef struct
{
	char             *name;
	Tcl_Interp       *interp;
	char             *onClicked;
	GtkColorSelectionDialog *colorSel;
} ColorSelDialogParams;

typedef struct
{
	Tcl_Interp           *interp;
	char                 *name;
	int                  getURIs;
	GtkFileChooserDialog *fileDialog;
} FileSelDialogParams;

typedef struct
{
	char             *name;
	Tcl_Interp       *interp;
	char             *onClicked;
	GtkFontSelectionDialog *fontSel;
} FontSelDialogParams;

typedef struct
{
	GtkWidget  *item;
	char       *name;
	char       *onClicked;
	Tcl_Interp *interp;
} ToolButtonParams;
