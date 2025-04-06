/* NAppGUI forms common base */

#include <gui/gui.hxx>
#include "nflib.def"

#ifndef __NFLIB_HXX__
#define __NFLIB_HXX__

typedef struct _flabel_t FLabel;
typedef struct _fbutton_t FButton;
typedef struct _fcheck_t FCheck;
typedef struct _fedit_t FEdit;
typedef struct _ftext_t FText;
typedef struct _fimage_t FImage;
typedef struct _fslider_t FSlider;
typedef struct _fprogress_t FProgress;
typedef struct _felem_t FElem;
typedef struct _fpopup_t FPopUp;
typedef struct _flistbox_t FListBox;
typedef struct _fheader_t FHeader;
typedef struct _ftable_t FTable;
typedef struct _fcolumn_t FColumn;
typedef struct _frow_t FRow;
/* Must be a union (when dbind supports) */
typedef struct _fwidget_t FWidget;
typedef struct _fcell_t FCell;
typedef struct _flayout_t FLayout;

/* Don't change the order. Add new values to end */
typedef enum _celltype_t
{
    ekCELL_TYPE_EMPTY,
    ekCELL_TYPE_LABEL,
    ekCELL_TYPE_BUTTON,
    ekCELL_TYPE_CHECK,
    ekCELL_TYPE_EDIT,
    ekCELL_TYPE_LAYOUT,
    ekCELL_TYPE_TEXT,
    ekCELL_TYPE_IMAGE,
    ekCELL_TYPE_SLIDER,
    ekCELL_TYPE_PROGRESS,
    ekCELL_TYPE_POPUP,
    ekCELL_TYPE_LISTBOX,
    ekCELL_TYPE_TABLEVIEW
} celltype_t;

/* Don't change the order. Add new values to end */
typedef enum _halign_t
{
    ekHALIGN_LEFT,
    ekHALIGN_CENTER,
    ekHALIGN_RIGHT,
    ekHALIGN_JUSTIFY
} halign_t;

/* Don't change the order. Add new values to end */
typedef enum _valign_t
{
    ekVALIGN_TOP,
    ekVALIGN_CENTER,
    ekVALIGN_BOTTOM,
    ekVALIGN_JUSTIFY
} valign_t;

/* Don't change the order. Add new values to end */
typedef enum _scale_t
{
    ekSCALE_NONE,
    ekSCALE_AUTO,
    ekSCALE_ASPECT,
    ekSCALE_FIT
} scale_t;

struct _flabel_t
{
    String *text;
    bool_t multiline;
    real32_t min_width;
    halign_t align;
};

struct _fbutton_t
{
    String *text;
    real32_t min_width;
};

struct _fcheck_t
{
    String *text;
};

struct _fedit_t
{
    bool_t passmode;
    bool_t autosel;
    halign_t text_align;
    real32_t min_width;
};

struct _ftext_t
{
    bool_t read_only;
    real32_t min_width;
    real32_t min_height;
};

struct _fimage_t
{
    String *path;
    scale_t scale;
	real32_t min_width;
    real32_t min_height;
};

struct _fslider_t
{
	real32_t min_width;
};

struct _fprogress_t
{
	real32_t min_width;
};

struct _felem_t
{
    String *text;
    String *iconpath;
};

struct _fpopup_t
{
    ArrSt(FElem) *elems;
};

struct _flistbox_t
{
    real32_t min_width;
    real32_t min_height;
    ArrSt(FElem) *elems;
};

struct _fheader_t
{
    String *title;
    halign_t align;
    bool_t resizable;
    real32_t width;
    real32_t min_width;
    real32_t max_width;
};

struct _ftable_t
{
    real32_t min_width;
    real32_t min_height;
    ArrSt(FHeader) *headers;
};

struct _fcolumn_t
{
    real32_t margin_right;
    real32_t forced_width;
};

struct _frow_t
{
    real32_t margin_bottom;
    real32_t forced_height;
};

struct _fwidget_t
{
    FLabel *label;
    FButton *button;
    FCheck *check;
    FEdit *edit;
    FText *text;
    FImage *image;
    FSlider *slider;
    FProgress *progress;
    FPopUp *popup;
    FListBox *listbox;
    FTable *table;
    FLayout *layout;
};

struct _fcell_t
{
    String *name;
    celltype_t type;
    halign_t halign;
    valign_t valign;
    FWidget widget;
};

struct _flayout_t
{
    String *name;
    real32_t margin_left;
    real32_t margin_top;
    real32_t margin_right;
    real32_t margin_bottom;
    ArrSt(FColumn) *cols;
    ArrSt(FRow) *rows;
    ArrSt(FCell) *cells;
};

DeclSt(FElem);
DeclSt(FHeader);
DeclSt(FColumn);
DeclSt(FRow);
DeclSt(FCell);

#endif
