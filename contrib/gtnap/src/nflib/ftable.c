/* Form tableview */

#include "ftable.h"
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FTable *ftable_create(void)
{
    return dbind_create(FTable);
}
