/* Form label */

#include "flabel.h"
#include "nflib.inl"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <gui/label.h>

/*---------------------------------------------------------------------------*/

FLabel *flabel_create(void)
{
    return dbind_create(FLabel);
}

/*---------------------------------------------------------------------------*/

void flabel_syncro(const FLabel *flabel, Label *label)
{
    cassert_no_null(flabel);
    label_text(label, tc(flabel->text));
    label_multiline(label, flabel->multiline);
    label_min_width(label, flabel->min_width);
    label_align(label, _nflib_halign(flabel->align));
}
