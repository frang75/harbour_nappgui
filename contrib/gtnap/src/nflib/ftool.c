/* Form tool button */

#include "ftool.h"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FTool *ftool_create(void)
{
    return dbind_create(FTool);
}
