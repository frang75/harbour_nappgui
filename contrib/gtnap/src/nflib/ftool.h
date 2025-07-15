/* Form tool button */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FTool *ftool_create(void);

_nflib_api void ftool_synchro(const FTool *ftool, Button *button, const char_t *resource_path);

__END_C
