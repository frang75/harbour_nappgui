/* encoding: cp850 */
#INCLUDE "gtnap.ch"
#INCLUDE "cua.ch"

*******************
PROC EXEMPLO_ASPEC_FORMS
*******************
LOCAL V_JANELA
*
CUA20 @ 15,20,25,70 JANELA V_JANELA ;
     TITULO "Formulários GTNAP em aplicações ASPEC" SUBTITULO "%T";
     AJUDA "T?????"
*
ESPECIALIZE V_JANELA MENU
ADDOPCAO V_JANELA TEXTO "Gestão de Negócios" ;
    ACAO TST_FORM_EMPRESAS() AJUDA "P?????"
*
ATIVE(V_JANELA)
*

********************************
STAT PROC TST_FORM_EMPRESAS
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_NFORMS() + "Company_list.nfm")

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

NAP_FORM_TITLE(V_FORM, "Primeiro exemplo de formulário GTNAP")

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_NFORMS())

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

ENDIF

NAP_FORM_DESTROY(V_FORM)
