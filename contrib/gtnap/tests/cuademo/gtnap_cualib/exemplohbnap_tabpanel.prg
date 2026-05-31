//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
STAT PROC CHANGE_EMBEDDED(O_FORM, O_EMBEDDED)
********************************
// Get the current selected tab
LOCAL N_TABID := HBNAP_FORMS_GET_INT(O_FORM, "tab_control")
LOCAL C_TABFORMS := {"Tab1_Panel.nfm", "Tab2_Panel.nfm", "Tab3_Panel.nfm" }

IF N_TABID >= 1 .AND. N_TABID <= 3

    // Load the form, if not previously loaded
    IF O_EMBEDDED[N_TABID] == NIL
        O_EMBEDDED[N_TABID] := HBNAP_FORMS_LOAD(DIRET_FORMS() + C_TABFORMS[N_TABID], DIRET_FORMS(), HBNAP_FORMS_EMBEDDED_PANEL)
    ENDIF

    // Just change the inner form of the main form
    HBNAP_FORMS_EMBED(O_FORM, O_EMBEDDED[N_TABID], "runtime_panel")

ENDIF

********************************
PROC TST_FORM_TABPANELS(O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "TabControl.nfm", DIRET_FORMS(), HBNAP_FORMS_CLOSE_ON_ESC)
LOCAL O_EMBEDDED := { NIL, NIL, NIL }
LOCAL N_RES := 0
LOCAL i := 0

HBNAP_FORMS_TITLE(O_FORM, "TabControl and TabPanels demo")
CHANGE_EMBEDDED(O_FORM, O_EMBEDDED)

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

FOR i := 1 TO Len( O_EMBEDDED )
    IF O_EMBEDDED[i] != NIL
        HBNAP_FORMS_DESTROY( O_EMBEDDED[i] )
    ENDIF
NEXT
