//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
PROC TST_FORM_TREEVIEW(O_PARENT_FORM)
********************************

LOCAL C_INFO
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "TreeView.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))

LOCAL V_AREAS := { "CUSTOMER", "INVOICES", "DETAILS" }

LOCAL V_RELS := { ;
    { {|| CUSTOMER->CODIGO},  {|| INVOICES->CODCLIENTE} }, ;
    { {|| INVOICES->NUMERO},  {|| DETAILS->CODINVOICE}  } ;
}

LOCAL V_COLS := { ;
    { ; // Customer table columns
        { HBNAP_LEFT,  {|| "Customer-Col1"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col2"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col3"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col4"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col5"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col6"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col7"} }, ;
        { HBNAP_LEFT,  {|| "Customer-Col8"} } ;
    }, ;
    { ; // Invoice table columns
        { HBNAP_LEFT,  {|| "Invoice-Col1"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col2"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col3"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col4"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col5"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col6"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col7"} }, ;
        { HBNAP_LEFT,  {|| "Invoice-Col8"} } ;
    }, ;
    { ; // Details table columns
        { HBNAP_LEFT,  {|| "Details-Col1"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col2"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col3"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col4"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col5"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col6"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col7"} }, ;
        { HBNAP_LEFT,  {|| "Details-Col8"} } ;
    } ;
}

USE ../dados/customer NEW SHARED
SET ORDER TO TAG CODIGO
GOTO TOP

USE ../dados/invoices NEW SHARED
SET ORDER TO TAG CODCLI
GOTO TOP

USE ../dados/details NEW SHARED
SET ORDER TO TAG CODINV
GOTO TOP

HBNAP_FORMS_TITLE(O_FORM, "TreeView with database relations")
HBNAP_FORMS_TREE_BIND(O_FORM, "table", V_AREAS, V_RELS, V_COLS)

C_INFO := "Bases de dados abertas:" + HB_EOL() + ;
          "  customer  : " + hb_ntos(CUSTOMER->(LASTREC())) + " registros" + HB_EOL() + ;
          "  invoices  : " + hb_ntos(INVOICES->(LASTREC())) + " registros" + HB_EOL() + ;
          "  details   : " + hb_ntos(DETAILS->(LASTREC())) + " registros"

// C_INFO := "Bases de dados abertas:"

INFO_MESSAGE_BOX(C_INFO, O_PARENT_FORM)

CLOSE CUSTOMER
CLOSE INVOICES
CLOSE DETAILS
