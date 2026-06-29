//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
PROC TST_FORM_TREEVIEW(O_PARENT_FORM)
********************************

LOCAL C_INFO
LOCAL N_RES := 0
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "TreeView.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))

LOCAL V_AREAS := { "CUSTOMER", "INVOICES", "DETAILS" }

LOCAL V_RELS := { ;
    { {|| CUSTOMER->CODIGO}, "CODCLI" }, ;
    { {|| INVOICES->NUMERO}, "CODINV" } ;
}

LOCAL V_COLS := { ;
    { ; // Customer table columns
        { HBNAP_LEFT,  {|| TRIM(CUSTOMER->NOME) + " (" + TRIM(CUSTOMER->NIF) + ")"} }, ; // Nome
        { HBNAP_LEFT,  {|| ""} }, ; // Quantia
        { HBNAP_LEFT,  {|| ""} }, ; // Preço
        { HBNAP_RIGHT, {|| "--"} }, ; // Subtotal
        { HBNAP_RIGHT, {|| "--"} }, ; // Impostos
        { HBNAP_RIGHT, {|| "--"} } ;  // Total
    }, ;
    { ; // Invoice table columns
        { HBNAP_LEFT,  {|| "Invoice: " + hb_ntos(INVOICES->NUMERO) + " (" + DTOC(INVOICES->DATE) + ")"} }, ; // Nome
        { HBNAP_LEFT,  {|| ""} }, ; // Quantia
        { HBNAP_LEFT,  {|| ""} }, ; // Preço
        { HBNAP_RIGHT, {|| hb_ntos(INVOICES->SUBTOTAL)} }, ; // Subtotal
        { HBNAP_RIGHT, {|| hb_ntos(INVOICES->IMPOSTOS)} }, ; // Impostos
        { HBNAP_RIGHT, {|| hb_ntos(INVOICES->TOTAL)} } ;     // Total
    }, ;
    { ; // Details table columns
        { HBNAP_LEFT,  {|| PADL(hb_ntos(DETAILS->NUMLINHA), 3, "0") + "-" + TRIM(DETAILS->CONCEITO) } }, ; // Nome
        { HBNAP_RIGHT, {|| hb_ntos(DETAILS->QUANTIDADE)} }, ; // Quantia
        { HBNAP_RIGHT, {|| hb_ntos(DETAILS->PRECUNIT)} }, ;   // Preço
        { HBNAP_RIGHT,  {|| hb_ntos(DETAILS->SUBTOTAL)} }, ;  // Subtotal
        { HBNAP_RIGHT,  {|| hb_ntos(DETAILS->IMPOSTOS)} }, ;  // Impostos
        { HBNAP_RIGHT,  {|| hb_ntos(DETAILS->TOTAL)} } ;      // Total
    } ;
}

USE ../dados/customer NEW SHARED
SET ORDER TO TAG CODIGO
GOTO TOP

USE ../dados/invoices NEW SHARED
USE ../dados/details NEW SHARED

HBNAP_FORMS_TITLE(O_FORM, "TreeView with database relations")
HBNAP_FORMS_TREE_BIND(O_FORM, "table", V_AREAS, V_RELS, V_COLS)

N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_ESC
    INFO_MESSAGE_BOX("Form closed by pressing [ESC] key", O_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_RETURN
    INFO_MESSAGE_BOX("Form closed by pressing [RETURN] key", O_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
    INFO_MESSAGE_BOX("Form closed by pressing [X] button", O_FORM)
ELSE
    INFO_MESSAGE_BOX("Form closed by user func with code: " + hb_ntos(N_RES), O_FORM)
ENDIF

C_INFO := "Bases de dados abertas:" + HB_EOL() + ;
          "  customer  : " + hb_ntos(CUSTOMER->(LASTREC())) + " registros" + HB_EOL() + ;
          "  invoices  : " + hb_ntos(INVOICES->(LASTREC())) + " registros" + HB_EOL() + ;
          "  details   : " + hb_ntos(DETAILS->(LASTREC())) + " registros"

INFO_MESSAGE_BOX(C_INFO, O_PARENT_FORM)
HBNAP_FORMS_DESTROY(O_FORM)

CLOSE CUSTOMER
CLOSE INVOICES
CLOSE DETAILS
