/* encoding: cp850 */
#INCLUDE "gtnap.ch"
#INCLUDE "hbnap.ch"     // For HBNAP API

// ********************************
// STAT FUNC VALIDAR_EMPRESA(O_EMPRESA)
// ********************************
// RETURN .T.

// ********************************
// STAT PROC SALVAR_EMPRESA(V_FORM, O_EMPRESA)
// ********************************
// LOCAL C_MESSAGE

// // Recovery data from form
// NAP_FORM_DBIND_STORE(V_FORM)

// IF VALIDAR_EMPRESA(O_EMPRESA) == .T.

//     IF RLOCK()
//         empresas->uf := O_EMPRESA[1]
//         empresas->codcid := O_EMPRESA[2]
//         empresas->cidade := O_EMPRESA[3]
//         empresas->codent := O_EMPRESA[4]
//         empresas->gestora := O_EMPRESA[5]
//         UNLOCK
//     ENDIF

//     C_MESSAGE := "C_UF: " + O_EMPRESA[1] + ";" + ;
//                     "C_CODCID: " + O_EMPRESA[2] + ";" + ;
//                     "C_CIDADE: " + O_EMPRESA[3] + ";" + ;
//                     "C_CODENT: " + O_EMPRESA[4] + ";" + ;
//                     "C_GESTORA: " + O_EMPRESA[5]

//     MOSTRAR("M?????",C_Message)

// ENDIF

// ********************************
// STAT PROC FORM_EMPRESA_DETAIL(C_FORM_TITLE)
// ********************************
// LOCAL V_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_detail.nfm", DIRET_FORMS(), HBNAP_FORMS_CLOSE_ON_ESC)
// LOCAL O_EMPRESA := { empresas->uf, empresas->codcid, empresas->cidade, empresas->codent, empresas->gestora }
// LOCAL V_BIND := { ;
//                     {"uf_combo", @O_EMPRESA[1] }, ;
//                     {"codcid_edit", @O_EMPRESA[2] }, ;
//                     {"cidade_combo", @O_EMPRESA[3] }, ;
//                     {"codent_edit", @O_EMPRESA[4] }, ;
//                     {"gestora_edit", @O_EMPRESA[5] } ;
//                 }
// LOCAL C_UF := { ;
//                     "AC", "AL", "AP", "AM", "BA", "CE", "DF", "ES", "GO", "MA", "MT", "MS", "MG", ;
//                     "PA", "PB", "PR", "PE", "PI", "RJ", "RN", "RS", "RO", "RR", "SC", "SP", "SE", "TO" ;
//               }

// LOCAL C_CIDADE := { ;
//                     "São Paulo", ;
//                     "Rio de Janeiro", ;
//                     "Brasília", ;
//                     "Salvador", ;
//                     "Fortaleza", ;
//                     "Belo Horizonte", ;
//                     "Manaus", ;
//                     "Curitiba", ;
//                     "Recife", ;
//                     "Porto Alegre", ;
//                     "Goiânia", ;
//                     "Belém", ;
//                     "Guarulhos", ;
//                     "Campinas", ;
//                     "São Luís" ;
//               }

// LOCAL N_RES := 0

// NAP_FORM_TITLE(V_FORM, C_FORM_TITLE)
// NAP_FORM_DBIND(V_FORM, V_BIND)
// NAP_FORM_ITEM_LIST(V_FORM, "uf_combo", C_UF)
// NAP_FORM_ITEM_LIST(V_FORM, "cidade_combo", C_CIDADE)
// NAP_FORM_ONCLICK(V_FORM, "salvar_button", {|| SALVAR_EMPRESA(V_FORM, O_EMPRESA) })

// N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS(), .T.)


// IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

// ENDIF

// NAP_FORM_DESTROY(V_FORM)

// ********************************
// STAT PROC ADD_EMPRESA(V_FORM)
// ********************************
// APPEND BLANK
// FORM_EMPRESA_DETAIL("Inclusão de empresa")
// NAP_FORM_REFRESH_AREA(V_FORM)

// ********************************
// STAT PROC EDIT_EMPRESA(V_FORM)
// ********************************
// LOCAL SELRECNO := NAP_FORM_SEL_RECNO(V_FORM)
// IF SELRECNO != -1
//     GO SELRECNO
//     FORM_EMPRESA_DETAIL("Alterar de empresa")
//     NAP_FORM_REFRESH_AREA(V_FORM)
// ENDIF

// ********************************
// STAT PROC REMOVE_EMPRESA(V_FORM)
// ********************************
// LOCAL SELRECNO := NAP_FORM_SEL_RECNO(V_FORM)
// IF SELRECNO != -1
//     GO SELRECNO
//     IF RLOCK()
//         DELETE
//         UNLOCK
//     ENDIF
//     NAP_FORM_REFRESH_AREA(V_FORM)
// ENDIF

********************************
PROC TST_FORM_EMPRESAS(O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_list.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

LOCAL V_DBBIND := { ;
    "table", ;
     { {|| empresas->uf} }, ;
     { {|| empresas->codcid} }, ;
     { {|| empresas->cidade} }, ;
     { {|| empresas->gestora} }, ;
     { {|| "Principal"} } ;
}

USE ../dados/empresas NEW SHARED
GOTO TOP

HBNAP_FORMS_TITLE(O_FORM, "Empresas")
HBNAP_FORMS_BIND_AREA(O_FORM, V_DBBIND)
//NAP_FORM_DBIND_AREA(V_FORM, V_DBBIND)

// NAP_FORM_ONCLICK(V_FORM, "add_button", {|| ADD_EMPRESA(V_FORM) })
// NAP_FORM_ONCLICK(V_FORM, "add_tool", {|| ADD_EMPRESA(V_FORM) })
// NAP_FORM_ONCLICK(V_FORM, "edit_button", {|| EDIT_EMPRESA(V_FORM) })
// NAP_FORM_ONCLICK(V_FORM, "edit_tool", {|| EDIT_EMPRESA(V_FORM) })
// NAP_FORM_ONCLICK(V_FORM, "remove_button", {|| REMOVE_EMPRESA(V_FORM) })
// NAP_FORM_ONCLICK(V_FORM, "remove_tool", {|| REMOVE_EMPRESA(V_FORM) })

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
