/* Form radio button */

#include "fradio.h"
#include <gui/button.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FRadio *fradio_create(void)
{
    return dbind_create(FRadio);
}

/*---------------------------------------------------------------------------*/

void fradio_synchro(const FRadio *fradio, Button *button)
{
    cassert_no_null(fradio);
    button_text(button, tc(fradio->text));
}

