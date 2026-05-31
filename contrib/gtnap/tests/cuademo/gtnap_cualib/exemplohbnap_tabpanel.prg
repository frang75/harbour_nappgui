//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
PROC TST_FORM_TABPANELS(O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "TabControl.nfm", DIRET_FORMS(), HBNAP_FORMS_CLOSE_ON_ESC)
LOCAL N_RES := 0

HBNAP_FORMS_TITLE(O_FORM, "TabControl and TabPanels demo")

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
