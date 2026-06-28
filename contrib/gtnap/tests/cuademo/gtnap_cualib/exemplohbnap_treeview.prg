//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
PROC TST_FORM_TREEVIEW(O_PARENT_FORM)
********************************
LOCAL C_INFO

USE ../dados/customer NEW SHARED
SET ORDER TO TAG CODIGO
GOTO TOP

USE ../dados/invoices NEW SHARED
SET ORDER TO TAG NUMERO
GOTO TOP

USE ../dados/details NEW SHARED
SET ORDER TO TAG CODINV
GOTO TOP

C_INFO := "Bases de dados abertas:" + HB_EOL() + ;
          "  customer  : " + hb_ntos(CUSTOMER->(LASTREC())) + " registros" + HB_EOL() + ;
          "  invoices  : " + hb_ntos(INVOICES->(LASTREC())) + " registros" + HB_EOL() + ;
          "  details   : " + hb_ntos(DETAILS->(LASTREC())) + " registros"

// C_INFO := "Bases de dados abertas:"

INFO_MESSAGE_BOX(C_INFO, O_PARENT_FORM)

CLOSE CUSTOMER
CLOSE INVOICES
CLOSE DETAILS
