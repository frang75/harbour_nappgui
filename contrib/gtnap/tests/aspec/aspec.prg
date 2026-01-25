/* Example of ASPEC application using HBNAP-Forms */
#INCLUDE "ord.ch"       // For RDD API Index Order
#INCLUDE "hbnap.ch"     // For HBNAP API

STATIC V_MAINWINDOW := NIL

PROC MAIN
    // Global Harbour config
    // A HBNAP application is a pure UT8 application. No codepages supported.
    REQUEST DBFCDX
    RDDSETDEFAULT("DBFCDX")
    RDDINFO(RDDI_MEMOTYPE,1)

    // HBNAP application init
    IF HB_GTVERSION()=="NAP"
        ASPEC_RESPATH("forms/")
        HBNAP_FORMS_INIT_APP({|| RUN_MAIN() })

    ELSE
        BREAK "Error fatal: Only HBNAP GUI allowed"

    ENDIF

// Real MAIN function. HBNAP will callback after GUI platform initialization
PROC RUN_MAIN
    V_MAINWINDOW := HBNAP_FORMS_LOAD(ASPEC_RESPATH() + "MainWindow.nfm", ASPEC_RESPATH(), HBNAP_FORMS_RESIZABLE)
    HBNAP_FORMS_TITLE(V_MAINWINDOW, "Sistemas Aspec")
    HBNAP_FORMS_SHOW(V_MAINWINDOW, {|| MAIN_WINDOW_CLOSE() })
    RETURN

// Callback for main window closed. By default, exit the app.
FUNCTION MAIN_WINDOW_CLOSE()
    HBNAP_FORMS_DESTROY(V_MAINWINDOW)
    HBNAP_FORMS_EXIT_APP()
    RETURN .T.

// Support func
FUNCTION ASPEC_RESPATH(C_NEW_PATH)
    STATIC C_RESPATH := ""
    LOCAL C_PREV_PATH := C_RESPATH
    IF C_NEW_PATH # NIL
        C_RESPATH := C_NEW_PATH
    ENDIF
    RETURN C_PREV_PATH
