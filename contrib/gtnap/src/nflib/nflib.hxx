/* NAppGUI forms serialization */

#include <gui/gui.hxx>
#include "nflib.def"

#ifndef __NFLIB_HXX__
#define __NFLIB_HXX__

typedef struct _ffont_t FFont;
typedef struct _flabel_t FLabel;
typedef struct _fbutton_t FButton;
typedef struct _fcheck_t FCheck;
typedef struct _fradio_t FRadio;
typedef struct _ftool_t FTool;
typedef struct _felem_t FElem;
typedef struct _fpopup_t FPopUp;
typedef struct _fedit_t FEdit;
typedef struct _fcombo_t FCombo;
typedef struct _ftabs_t FTabs;
typedef struct _flistbox_t FListBox;
typedef struct _fslider_t FSlider;
typedef struct _fvslider_t FVSlider;
typedef struct _fprogress_t FProgress;
typedef struct _fview_t FView;
typedef struct _fsview_t FSView;
typedef struct _ftext_t FText;
typedef struct _fimage_t FImage;
typedef struct _fhline_t FHline;
typedef struct _fvline_t FVline;
typedef struct _fpanel_t FPanel;
typedef struct _fheader_t FHeader;
typedef struct _ftable_t FTable;
typedef struct _fcolumn_t FColumn;
typedef struct _frow_t FRow;
/* Must be a union (when dbind supports) */
typedef struct _fwidget_t FWidget;
typedef struct _fcell_t FCell;
typedef struct _flayout_t FLayout;
typedef struct _fform_t FForm;

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
    ekCELL_TYPE_TABLEVIEW,
    ekCELL_TYPE_TOOL,
    ekCELL_TYPE_RADIO,
    ekCELL_TYPE_COMBO,
    ekCELL_TYPE_VSLIDER,
    ekCELL_TYPE_VIEW,
    ekCELL_TYPE_SCROLL_VIEW,
    ekCELL_TYPE_HLINE,
    ekCELL_TYPE_VLINE,
    ekCELL_TYPE_TABS,
    ekCELL_TYPE_PANEL
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
typedef enum _pos_t
{
    ekPOS_NONE,
    ekPOS_LEFT,
    ekPOS_TOP,
    ekPOS_RIGHT,
    ekPOS_BOTTOM
} pos_t;

/* Don't change the order. Add new values to end */
typedef enum _scale_t
{
    ekSCALE_NONE,
    ekSCALE_AUTO,
    ekSCALE_ASPECT,
    ekSCALE_FIT
} scale_t;

/* Don't change the order. Add new values to end */
typedef enum _ffamily_t
{
    ekFFAMILY_REGULAR,
    ekFFAMILY_MONOSPACE
} ffamily_t;

struct _ffont_t
{
    ffamily_t family;
    real32_t size;
    bool_t bold;
    bool_t italic;
    bool_t underline;
    bool_t strikeout;
};

struct _flabel_t
{
    String *text;
    FFont font;
    bool_t multiline;
    real32_t min_width;
    halign_t align;
    bool_t with_color;
    bool_t with_bgcolor;
    uint32_t color_light;
    uint32_t color_dark;
    uint32_t bgcolor_light;
    uint32_t bgcolor_dark;
};

struct _fbutton_t
{
    String *text;
    String *tooltip;
    real32_t min_width;
    real32_t hpadding;
    real32_t vpadding;
};

struct _fcheck_t
{
    String *text;
};

struct _fradio_t
{
    String *text;
};

struct _ftool_t
{
    String *path;
    String *text;
    String *tooltip;
    pos_t imgpos;
    real32_t hpadding;
    real32_t vpadding;
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

struct _fedit_t
{
    bool_t passmode;
    bool_t autosel;
    halign_t text_align;
    real32_t min_width;
};

struct _fcombo_t
{
    bool_t passmode;
    bool_t autosel;
    halign_t text_align;
    real32_t min_width;
};

struct _ftabs_t
{
	real32_t min_width;
    ArrSt(FElem) *elems;
};

struct _flistbox_t
{
    real32_t min_width;
    real32_t min_height;
    ArrSt(FElem) *elems;
};

struct _fslider_t
{
	real32_t min_width;
};

struct _fvslider_t
{
	real32_t min_height;
};

struct _fprogress_t
{
	real32_t min_width;
};

struct _fview_t
{
    real32_t min_width;
    real32_t min_height;
};

struct _fsview_t
{
    real32_t min_width;
    real32_t min_height;
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

struct _fhline_t
{
    real32_t length;
};

struct _fvline_t
{
    real32_t length;
};

struct _fpanel_t
{
    bool_t autosize;
    real32_t min_width;
    real32_t min_height;
};

struct _fheader_t
{
    String *title;
    halign_t align;
    halign_t dalign;
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
    bool_t expand;
    real32_t margin_right;
    real32_t forced_width;
};

struct _frow_t
{
    bool_t expand;
    real32_t margin_bottom;
    real32_t forced_height;
};

struct _fwidget_t
{
    FLabel *label;
    FButton *button;
    FCheck *check;
    FRadio *radio;
    FTool *tool;
    FPopUp *popup;
    FEdit *edit;
    FCombo *combo;
    FTabs *tabs;
    FListBox *listbox;
    FSlider *slider;
    FVSlider *vslider;
    FProgress *progress;
    FView *view;
    FSView *sview;
    FText *text;
    FImage *image;
    FTable *table;
    FHline *hline;
    FVline *vline;
    FPanel *panel;
    FLayout *layout;
};

struct _fcell_t
{
    String *name;
    bool_t tabstop;
    celltype_t type;
    halign_t halign;
    valign_t valign;
    real32_t padding_left;
    real32_t padding_top;
    real32_t padding_right;
    real32_t padding_bottom;
    FWidget widget;
};

struct _flayout_t
{
    String *name;
    bool_t row_tabstop;
    real32_t margin_left;
    real32_t margin_top;
    real32_t margin_right;
    real32_t margin_bottom;
    bool_t with_border;
    bool_t with_background;
    bool_t with_group;
    uint32_t border_light;
    uint32_t border_dark;
    uint32_t backgd_light;
    uint32_t backgd_dark;
    String *group_title;
    ArrSt(FColumn) *cols;
    ArrSt(FRow) *rows;
    ArrSt(FCell) *cells;
};

struct _fform_t
{
    String *description;
    FLayout *layout;
};

DeclSt(FElem);
DeclSt(FHeader);
DeclSt(FColumn);
DeclSt(FRow);
DeclSt(FCell);

#endif
