/* Form label */

#include "flabel.h"
#include "ffont.h"
#include "nflib.inl"
#include <gui/gui.h>
#include <gui/label.h>
#include <draw2d/color.h>
#include <draw2d/font.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FLabel *flabel_create(void)
{
    return dbind_create(FLabel);
}

/*---------------------------------------------------------------------------*/

void flabel_destroy(FLabel **flabel)
{
    dbind_destroy(flabel, FLabel);
}

/*---------------------------------------------------------------------------*/

void flabel_synchro(const FLabel *flabel, Label *label)
{
    Font *font = NULL;
    cassert_no_null(flabel);
    font = ffont_font(&flabel->font);
    label_text(label, tc(flabel->text));
    label_font(label, font);
    label_multiline(label, flabel->multiline);
    label_width(label, flabel->min_width);
    label_align(label, _nflib_halign(flabel->align));

    if (flabel->with_color == TRUE)
        label_color(label, gui_alt_color(flabel->color_light, flabel->color_dark));
    else
        label_color(label, kCOLOR_TRANSPARENT);

    if (flabel->with_bgcolor == TRUE)
        label_bgcolor(label, gui_alt_color(flabel->bgcolor_light, flabel->bgcolor_dark));
    else
        label_bgcolor(label, kCOLOR_TRANSPARENT);

    font_destroy(&font);
}
