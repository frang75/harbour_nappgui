/* Form radio button */

#include "fradio.h"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FRadio *fradio_create(void)
{
    return dbind_create(FRadio);
}
