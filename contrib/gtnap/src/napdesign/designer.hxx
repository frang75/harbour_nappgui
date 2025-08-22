/* NAppGUI Designer types */

#include <nflib/nflib.hxx>

#ifndef __DESIGNER_HXX__
#define __DESIGNER_HXX__

typedef struct _desiger_t Designer;
typedef struct _dcolumn_t DColumn;
typedef struct _drow_t DRow;
typedef struct _dcell_t DCell;
typedef struct _dlayout_t DLayout;
typedef struct _dform_t DForm;
typedef struct _dselect_t DSelect;

typedef enum _widget_t
{
    ekWIDGET_SELECT = 0,
    ekWIDGET_VERT_LAYOUT,
    ekWIDGET_HORZ_LAYOUT,
    ekWIDGET_GRID_LAYOUT,
    ekWIDGET_PUSH_BUTTON,
    ekWIDGET_TOOL_BUTTON,
    ekWIDGET_CHECK_BUTTON,
    ekWIDGET_RADIO_BUTTON,
    ekWIDGET_LABEL,
    ekWIDGET_EDITBOX,
    ekWIDGET_COMBOBOX,
    ekWIDGET_TEXTVIEW,
    ekWIDGET_LISTBOX,
    ekWIDGET_POPUP,
    ekWIDGET_TABLEVIEW,
    ekWIDGET_IMAGEVIEW,
    ekWIDGET_HORZ_SLIDER,
    ekWIDGET_VERT_SLIDER,
    ekWIDGET_PROGRESS
} widget_t;

typedef enum _drawer_t
{
    ekDRAWER_WIDGET_SELECT = 1,
    ekDRAWER_WIDGET_LAYOUTS,
    ekDRAWER_WIDGET_BUTTONS,
    ekDRAWER_WIDGET_TEXT,
    ekDRAWER_WIDGET_ITEMS,
    ekDRAWER_WIDGET_OTHERS,
    ekDRAWER_LAYOUT_PROPS,
    ekDRAWER_COLUMN_PROPS,
    ekDRAWER_ROW_PROPS,
    ekDRAWER_CELL_PROPS,
    ekDRAWER_LABEL_PROPS,
    ekDRAWER_BUTTON_PROPS,
    ekDRAWER_CHECKBOX_PROPS,
    ekDRAWER_RADIO_PROPS,
    ekDRAWER_TOOL_PROPS,
    ekDRAWER_POPUP_PROPS,
    ekDRAWER_EDIT_PROPS,
    ekDRAWER_COMBO_PROPS,
    ekDRAWER_LIST_PROPS
} drawer_t;

typedef enum _layelem_t
{
    ekLAYELEM_MARGIN_LEFT,
    ekLAYELEM_MARGIN_TOP,
    ekLAYELEM_MARGIN_RIGHT,
    ekLAYELEM_MARGIN_BOTTOM,
    ekLAYELEM_MARGIN_COLUMN,
    ekLAYELEM_MARGIN_ROW,
    ekLAYELEM_CELL
} layelem_t;

typedef enum _devent_t
{
    ekDEVENT_HEADER_CLOSE = 0x500,
    ekDEVENT_DRAWER_CHANGE
} devent_t;

struct _dcolumn_t
{
    real32_t width;
    R2Df margin_rect;
};

struct _drow_t
{
    real32_t height;
    R2Df margin_rect;
};

struct _dcell_t
{
    DLayout *sublayout;
    ArrPt(Image) *nimages;
    ArrPt(Image) *simages;
    R2Df rect;
    R2Df content_rect;
};

struct _dlayout_t
{
    ArrSt(DColumn) *cols;
    ArrSt(DRow) *rows;
    ArrSt(DCell) *cells;
    R2Df rect;
    R2Df rect_left;
    R2Df rect_top;
    R2Df rect_right;
    R2Df rect_bottom;
};

struct _dselect_t
{
    DLayout *dlayout;
    FLayout *flayout;
    Layout *glayout;
    layelem_t elem;
    uint32_t col;
    uint32_t row;
};

DeclSt(DColumn);
DeclSt(DRow);
DeclSt(DCell);
DeclSt(DSelect);

#endif
