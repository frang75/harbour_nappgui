//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
STAT FUNC VALIDAR_EMPRESA(O_EMPRESA)
********************************
RETURN .T.

********************************
STAT PROC SALVAR_EMPRESA(O_FORM, O_EMPRESA)
********************************
LOCAL C_MESSAGE

// Recovery data from form to O_EMPRESA vector
HBNAP_FORMS_BIND_STORE(O_FORM)

IF VALIDAR_EMPRESA(O_EMPRESA) == .T.

    IF RLOCK()
        empresas->uf := O_EMPRESA[1]
        empresas->codcid := O_EMPRESA[2]
        empresas->cidade := O_EMPRESA[3]
        empresas->codent := O_EMPRESA[4]
        empresas->gestora := O_EMPRESA[5]
        UNLOCK
    ENDIF

    C_MESSAGE := "C_UF: " + O_EMPRESA[1] + HB_EOL() + ;
                 "C_CODCID: " + O_EMPRESA[2] + HB_EOL() + ;
                 "C_CIDADE: " + O_EMPRESA[3] + HB_EOL() + ;
                 "C_CODENT: " + O_EMPRESA[4] + HB_EOL() + ;
                 "C_GESTORA: " + O_EMPRESA[5]

    INFO_MESSAGE_BOX(C_MESSAGE, O_FORM)

ENDIF

********************************
STAT PROC FORM_EMPRESA_DETAIL(C_FORM_TITLE, O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_detail.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
LOCAL O_EMPRESA := { empresas->uf, empresas->codcid, empresas->cidade, empresas->codent, empresas->gestora }
LOCAL V_BIND := { ;
                    {"uf_combo", @O_EMPRESA[1] }, ;
                    {"codcid_edit", @O_EMPRESA[2] }, ;
                    {"cidade_combo", @O_EMPRESA[3] }, ;
                    {"codent_edit", @O_EMPRESA[4] }, ;
                    {"gestora_edit", @O_EMPRESA[5] } ;
                }
LOCAL C_UF := { ;
                    "AC", "AL", "AP", "AM", "BA", "CE", "DF", "ES", "GO", "MA", "MT", "MS", "MG", ;
                    "PA", "PB", "PR", "PE", "PI", "RJ", "RN", "RS", "RO", "RR", "SC", "SP", "SE", "TO" ;
              }

LOCAL C_CIDADE := { ;
                    "São Paulo", ;
                    "Rio de Janeiro", ;
                    "Brasília", ;
                    "Salvador", ;
                    "Fortaleza", ;
                    "Belo Horizonte", ;
                    "Manaus", ;
                    "Curitiba", ;
                    "Recife", ;
                    "Porto Alegre", ;
                    "Goiânia", ;
                    "Belém", ;
                    "Guarulhos", ;
                    "Campinas", ;
                    "São Luís" ;
              }

LOCAL N_RES := 0

HBNAP_FORMS_TITLE(O_FORM, C_FORM_TITLE)
HBNAP_FORMS_BIND(O_FORM, V_BIND)
HBNAP_FORMS_ITEM_LIST(O_FORM, "uf_combo", C_UF)
HBNAP_FORMS_ITEM_LIST(O_FORM, "cidade_combo", C_CIDADE)
HBNAP_FORMS_ONCLICK(O_FORM, "salvar_button", {|| SALVAR_EMPRESA(O_FORM, O_EMPRESA) })

N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_ESC
ELSEIF N_RES == HBNAP_CLOSED_BY_RETURN
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
ELSE
ENDIF

HBNAP_FORMS_DESTROY(O_FORM)

********************************
STAT PROC ADD_EMPRESA(O_PARENT_FORM)
********************************
APPEND BLANK
FORM_EMPRESA_DETAIL("Inclusão de empresa", O_PARENT_FORM)
HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)

********************************
STAT PROC EDIT_EMPRESA(O_PARENT_FORM)
********************************
LOCAL SRECNO := HBNAP_FORMS_AREA_RECNO(O_PARENT_FORM)
IF SRECNO != -1
    GO SRECNO
    FORM_EMPRESA_DETAIL("Alterar de empresa", O_PARENT_FORM)
    HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)
ENDIF

********************************
STAT PROC REMOVE_EMPRESA(O_PARENT_FORM)
********************************
LOCAL SRECNO := HBNAP_FORMS_AREA_RECNO(O_PARENT_FORM)
IF SRECNO != -1
    GO SRECNO
    IF RLOCK()
        DELETE
        UNLOCK
    ENDIF
    HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)
ENDIF

********************************
PROC TST_FORM_EMPRESAS(O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_list.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

LOCAL V_COLS := { ; // table columns
    { HBNAP_LEFT, {|| empresas->uf} }, ;
    { HBNAP_LEFT, {|| empresas->codcid} }, ;
    { HBNAP_LEFT, {|| empresas->cidade} }, ;
    { HBNAP_LEFT, {|| empresas->gestora} }, ;
    { HBNAP_LEFT, {|| "Principal"} } ;
}

USE ../dados/empresas NEW SHARED
GOTO TOP

HBNAP_FORMS_TITLE(O_FORM, "Empresas")
HBNAP_FORMS_TABLE_BIND(O_FORM, "table", "EMPRESAS", V_COLS)
HBNAP_FORMS_ONCLICK(O_FORM, "add_button", {|| ADD_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "add_tool", {|| ADD_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "edit_button", {|| EDIT_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "edit_tool", {|| EDIT_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "remove_button", {|| REMOVE_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "remove_tool", {|| REMOVE_EMPRESA(O_FORM) })

// Launch the form
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

HBNAP_FORMS_DESTROY(O_FORM)

CLOSE EMPRESAS
