/* NAppGUI forms common base */

#include "nflib.h"
#include "nflib_res.h"
#include <gui/gui.h>
#include <draw2d/image.h>
#include <core/dbind.h>
#include <core/respack.h>
#include <osbs/log.h>
#include <sewer/blib.h>
#include <sewer/cassert.h>

static uint32_t i_NUM_USERS = 0;
static ResPack *i_RESPACK = NULL;

/*---------------------------------------------------------------------------*/

static void i_nflib_atexit(void)
{
    if (i_NUM_USERS != 0)
        log_printf("Error! nflib is not properly closed (%d)\n", i_NUM_USERS);
}

/*---------------------------------------------------------------------------*/

static void i_dbind(void)
{
    /* Registration of editable structures */
    dbind_enum(celltype_t, ekCELL_TYPE_EMPTY, "");
    dbind_enum(celltype_t, ekCELL_TYPE_LABEL, "");
    dbind_enum(celltype_t, ekCELL_TYPE_BUTTON, "");
    dbind_enum(celltype_t, ekCELL_TYPE_CHECK, "");
    dbind_enum(celltype_t, ekCELL_TYPE_EDIT, "");
    dbind_enum(celltype_t, ekCELL_TYPE_TEXT, "");
    dbind_enum(celltype_t, ekCELL_TYPE_IMAGE, "");
    dbind_enum(celltype_t, ekCELL_TYPE_SLIDER, "");
    dbind_enum(celltype_t, ekCELL_TYPE_PROGRESS, "");
    dbind_enum(celltype_t, ekCELL_TYPE_POPUP, "");
    dbind_enum(celltype_t, ekCELL_TYPE_LISTBOX, "");
    dbind_enum(celltype_t, ekCELL_TYPE_TABLEVIEW, "");
    dbind_enum(celltype_t, ekCELL_TYPE_LAYOUT, "");
    dbind_enum(halign_t, ekHALIGN_LEFT, "Left");
    dbind_enum(halign_t, ekHALIGN_CENTER, "Center");
    dbind_enum(halign_t, ekHALIGN_RIGHT, "Right");
    dbind_enum(halign_t, ekHALIGN_JUSTIFY, "Justify");
    dbind_enum(valign_t, ekVALIGN_TOP, "Top");
    dbind_enum(valign_t, ekVALIGN_CENTER, "Center");
    dbind_enum(valign_t, ekVALIGN_BOTTOM, "Bottom");
    dbind_enum(valign_t, ekVALIGN_JUSTIFY, "Justify");
    dbind_enum(scale_t, ekSCALE_NONE, "None");
    dbind_enum(scale_t, ekSCALE_AUTO, "Auto");
    dbind_enum(scale_t, ekSCALE_ASPECT, "Aspect");
    dbind_enum(scale_t, ekSCALE_FIT, "Fit");
    dbind(FLabel, String *, text);
    dbind(FLabel, bool_t, multiline);
    dbind(FLabel, real32_t, min_width);
    dbind(FLabel, halign_t, align);
    dbind(FButton, String *, text);
    dbind(FButton, real32_t, min_width);
    dbind(FCheck, String *, text);
    dbind(FEdit, bool_t, passmode);
    dbind(FEdit, bool_t, autosel);
    dbind(FEdit, halign_t, text_align);
    dbind(FEdit, real32_t, min_width);
    dbind(FText, bool_t, read_only);
    dbind(FText, real32_t, min_width);
    dbind(FText, real32_t, min_height);
    dbind(FImage, String *, path);
    dbind(FImage, scale_t, scale);
    dbind(FImage, real32_t, min_width);
    dbind(FImage, real32_t, min_height);
    dbind(FSlider, real32_t, min_width);
    dbind(FProgress, real32_t, min_width);
    dbind(FElem, String *, text);
    dbind(FElem, String *, iconpath);
    dbind(FPopUp, ArrSt(FElem) *, elems);
    dbind(FListBox, real32_t, min_width);
    dbind(FListBox, real32_t, min_height);
    dbind(FListBox, ArrSt(FElem) *, elems);
    dbind(FHeader, String *,title);
    dbind(FHeader, halign_t, align);
    dbind(FHeader, bool_t, resizable);
    dbind(FHeader, real32_t, width);
    dbind(FHeader, real32_t, min_width);
    dbind(FHeader, real32_t, max_width);
    dbind(FTable, real32_t, min_width);
    dbind(FTable, real32_t, min_height);
    dbind(FTable, ArrSt(FHeader) *, headers);
    dbind(FColumn, real32_t, margin_right);
    dbind(FColumn, real32_t, forced_width);
    dbind(FRow, real32_t, margin_bottom);
    dbind(FRow, real32_t, forced_height);
    dbind(FCell, String *, name);
    dbind(FCell, celltype_t, type);
    dbind(FCell, halign_t, halign);
    dbind(FCell, valign_t, valign);
    dbind(FLayout, String *, name);
    dbind(FLayout, real32_t, margin_left);
    dbind(FLayout, real32_t, margin_top);
    dbind(FLayout, real32_t, margin_right);
    dbind(FLayout, real32_t, margin_bottom);
    dbind(FLayout, ArrSt(FColumn) *, cols);
    dbind(FLayout, ArrSt(FRow) *, rows);
    dbind(FLayout, ArrSt(FCell) *, cells);

    dbind_default(FColumn, real32_t, margin_right, 0);
    dbind_default(FColumn, real32_t, forced_width, 0);
    dbind_increment(FColumn, real32_t, margin_right, 1);
    dbind_increment(FColumn, real32_t, forced_width, 1);
    dbind_range(FColumn, real32_t, margin_right, 0, 100);
    dbind_range(FColumn, real32_t, forced_width, 0, 1000);
    dbind_precision(FColumn, real32_t, margin_right, 1);
    dbind_precision(FColumn, real32_t, forced_width, 1);

    dbind_default(FRow, real32_t, margin_bottom, 0);
    dbind_default(FRow, real32_t, forced_height, 0);
    dbind_increment(FRow, real32_t, margin_bottom, 1);
    dbind_increment(FRow, real32_t, forced_height, 1);
    dbind_range(FRow, real32_t, margin_bottom, 0, 100);
    dbind_range(FRow, real32_t, forced_height, 0, 1000);
    dbind_precision(FRow, real32_t, margin_bottom, 1);
    dbind_precision(FRow, real32_t, forced_height, 1);

    dbind_default(FButton, real32_t, min_width, 0);
    dbind_increment(FButton, real32_t, min_width, 1);
    dbind_precision(FButton, real32_t, min_width, 1);
    dbind_range(FButton, real32_t, min_width, 10, 1000);

    dbind_default(FLabel, bool_t, multiline, FALSE);
    dbind_default(FLabel, real32_t, min_width, 0);
    dbind_increment(FLabel, real32_t, min_width, 1);
    dbind_precision(FLabel, real32_t, min_width, 1);
    dbind_range(FLabel, real32_t, min_width, 0, 1000);
    dbind_default(FLabel, halign_t, align, ekHALIGN_LEFT);

    dbind_default(FEdit, real32_t, min_width, 100);
    dbind_increment(FEdit, real32_t, min_width, 1);
    dbind_precision(FEdit, real32_t, min_width, 1);
    dbind_range(FEdit, real32_t, min_width, 10, 1000);

    dbind_default(FText, bool_t, read_only, FALSE);
    dbind_default(FText, real32_t, min_width, 100);
    dbind_increment(FText, real32_t, min_width, 1);
    dbind_precision(FText, real32_t, min_width, 1);
    dbind_range(FText, real32_t, min_width, 10, 1000);
    dbind_default(FText, real32_t, min_height, 100);
    dbind_increment(FText, real32_t, min_height, 1);
    dbind_precision(FText, real32_t, min_height, 1);
    dbind_range(FText, real32_t, min_height, 10, 1000);

    dbind_default(FImage, scale_t, scale, ekSCALE_ASPECT);
    dbind_default(FImage, real32_t, min_width, 100);
    dbind_increment(FImage, real32_t, min_width, 1);
    dbind_precision(FImage, real32_t, min_width, 1);
    dbind_range(FImage, real32_t, min_width, 10, 1000);
    dbind_default(FImage, real32_t, min_height, 100);
    dbind_increment(FImage, real32_t, min_height, 1);
    dbind_precision(FImage, real32_t, min_height, 1);
    dbind_range(FImage, real32_t, min_height, 10, 1000);

    dbind_default(FSlider, real32_t, min_width, 100);
    dbind_increment(FSlider, real32_t, min_width, 1);
    dbind_precision(FSlider, real32_t, min_width, 1);
    dbind_range(FSlider, real32_t, min_width, 10, 1000);

    dbind_default(FProgress, real32_t, min_width, 100);
    dbind_increment(FProgress, real32_t, min_width, 1);
    dbind_precision(FProgress, real32_t, min_width, 1);
    dbind_range(FProgress, real32_t, min_width, 10, 1000);

    dbind_default(FListBox, real32_t, min_width, 100);
    dbind_increment(FListBox, real32_t, min_width, 1);
    dbind_precision(FListBox, real32_t, min_width, 1);
    dbind_range(FListBox, real32_t, min_width, 10, 1000);
    dbind_default(FListBox, real32_t, min_height, 100);
    dbind_increment(FListBox, real32_t, min_height, 1);
    dbind_precision(FListBox, real32_t, min_height, 1);
    dbind_range(FListBox, real32_t, min_height, 10, 1000);

    dbind_default(FHeader, halign_t, align, ekHALIGN_LEFT);
    dbind_default(FHeader, bool_t, resizable, TRUE);
    dbind_default(FHeader, real32_t, width, 100);
    dbind_increment(FHeader, real32_t, width, 1);
    dbind_precision(FHeader, real32_t, width, 1);
    dbind_range(FHeader, real32_t, width, 0, 1000);
    dbind_default(FHeader, real32_t, min_width, 100);
    dbind_increment(FHeader, real32_t, min_width, 1);
    dbind_precision(FHeader, real32_t, min_width, 1);
    dbind_default(FHeader, real32_t, max_width, 1000);
    dbind_increment(FHeader, real32_t, max_width, 1);
    dbind_precision(FHeader, real32_t, max_width, 1);
    dbind_default(FTable, real32_t, min_width, 100);
    dbind_increment(FTable, real32_t, min_width, 1);
    dbind_precision(FTable, real32_t, min_width, 1);
    dbind_range(FTable, real32_t, min_width, 10, 1000);
    dbind_default(FTable, real32_t, min_height, 100);
    dbind_increment(FTable, real32_t, min_height, 1);
    dbind_precision(FTable, real32_t, min_height, 1);
    dbind_range(FTable, real32_t, min_height, 10, 1000);

    dbind_default(FCell, celltype_t, type, ekCELL_TYPE_EMPTY);
    dbind_default(FCell, halign_t, halign, ekHALIGN_LEFT);
    dbind_default(FCell, valign_t, valign, ekVALIGN_TOP);

    dbind_default(FLayout, real32_t, margin_left, 0);
    dbind_default(FLayout, real32_t, margin_top, 0);
    dbind_default(FLayout, real32_t, margin_right, 0);
    dbind_default(FLayout, real32_t, margin_bottom, 0);
    dbind_increment(FLayout, real32_t, margin_left, 1);
    dbind_increment(FLayout, real32_t, margin_top, 1);
    dbind_increment(FLayout, real32_t, margin_right, 1);
    dbind_increment(FLayout, real32_t, margin_bottom, 1);
    dbind_range(FLayout, real32_t, margin_left, 0, 100);
    dbind_range(FLayout, real32_t, margin_top, 0, 100);
    dbind_range(FLayout, real32_t, margin_right, 0, 100);
    dbind_range(FLayout, real32_t, margin_bottom, 0, 100);
    dbind_precision(FLayout, real32_t, margin_left, 1);
    dbind_precision(FLayout, real32_t, margin_top, 1);
    dbind_precision(FLayout, real32_t, margin_right, 1);
    dbind_precision(FLayout, real32_t, margin_bottom, 1);

    /* Don't move, we must first declare the inner struct */
    dbind(FWidget, FLabel *, label);
    dbind(FWidget, FButton *, button);
    dbind(FWidget, FCheck *, check);
    dbind(FWidget, FEdit *, edit);
    dbind(FWidget, FText *, text);
    dbind(FWidget, FImage *, image);
    dbind(FWidget, FSlider *, slider);
    dbind(FWidget, FProgress *, progress);
    dbind(FWidget, FPopUp *, popup);
    dbind(FWidget, FListBox *, listbox);
    dbind(FWidget, FTable*, table);
    dbind(FWidget, FLayout *, layout);
    dbind(FCell, FWidget, widget);

    dbind_default(FWidget, FLabel *, label, NULL);
    dbind_default(FWidget, FButton *, button, NULL);
    dbind_default(FWidget, FCheck *, check, NULL);
    dbind_default(FWidget, FEdit *, edit, NULL);
    dbind_default(FWidget, FText *, text, NULL);
    dbind_default(FWidget, FImage *, image, NULL);
    dbind_default(FWidget, FSlider *, slider, NULL);
    dbind_default(FWidget, FProgress *, progress, NULL);
    dbind_default(FWidget, FPopUp *, popup, NULL);
    dbind_default(FWidget, FListBox *, listbox, NULL);
    dbind_default(FWidget, FTable*, table, NULL);
    dbind_default(FWidget, FLayout *, layout, NULL);
}

/*---------------------------------------------------------------------------*/

void nflib_start(void)
{
    if (i_NUM_USERS == 0)
    {
        gui_start();
        i_dbind();
        blib_atexit(i_nflib_atexit);
    }

    i_NUM_USERS += 1;
}

/*---------------------------------------------------------------------------*/

void nflib_finish(void)
{
    cassert(i_NUM_USERS > 0);
    if (i_NUM_USERS == 1)
    {
        if (i_RESPACK != NULL)
            respack_destroy(&i_RESPACK);

        /* Unregister types (when dbind support it) */
        gui_finish();
    }

    i_NUM_USERS -= 1;
}

/*---------------------------------------------------------------------------*/

const Image *nflib_default_image(void)
{
    if (i_RESPACK == NULL)
        i_RESPACK = nflib_res_respack("");
    return image_from_resource(i_RESPACK, NOIMAGE_PNG);
}

