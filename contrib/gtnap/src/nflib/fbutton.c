/* Form button */

#include "fbutton.h"
#include <gui/button.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FButton *fbutton_create(void)
{
    return dbind_create(FButton);
}

/*---------------------------------------------------------------------------*/

void fbutton_synchro(const FButton *fbutton, Button *button)
{
    cassert_no_null(fbutton);
    button_text(button, tc(fbutton->text));
    button_min_width(button, fbutton->min_width);
    button_hpadding(button, fbutton->hpadding);
    button_vpadding(button, fbutton->vpadding);
}
