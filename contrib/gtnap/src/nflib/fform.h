/* Form */

#include "nflib.hxx"

__EXTERN_C

FForm *fform_create(void);

FForm *fform_read(Stream *stm);

void fform_destroy(FForm **form);

void fform_write(Stream *stm, const FForm *form);

__END_C

