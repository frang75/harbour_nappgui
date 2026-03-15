/* Form label */

#include "flabel.h"
#include "nflib.inl"
#include <gui/label.h>
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
    cassert_no_null(flabel);
    label_text(label, tc(flabel->text));
    label_multiline(label, flabel->multiline);
    label_width(label, flabel->min_width);
    label_align(label, _nflib_halign(flabel->align));
}
