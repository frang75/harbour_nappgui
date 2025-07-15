/* Form checkbox */

#include "fcheck.h"
#include <gui/button.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FCheck *fcheck_create(void)
{
    return dbind_create(FCheck);
}

/*---------------------------------------------------------------------------*/

void fcheck_synchro(const FCheck *fcheck, Button *button)
{
    cassert_no_null(fcheck);
    button_text(button, tc(fcheck->text));
}
