/* Example of ASPEC application using HBNAP-Forms */
#INCLUDE "ord.ch"       // For RDD API Index Order
#INCLUDE "hbnap.ch"     // For HBNAP API

STATIC O_MAINWINDOW := NIL

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
    LOCAL V_COVER := { ;
        { "Orçamento", ASPEC_RESPATH() + "images/main/grid.png", "#20B2AA", "", .F., { || BUDGET_START() }}, ;
        { "Contábil SIAFIC", ASPEC_RESPATH() + "images/main/calc.png", "#1E90FF", "", .F., { || ACCOUNTING_START() }}, ;
        { "Licitaçao", ASPEC_RESPATH() + "images/main/auction.png", "#DAA520", "Lei 8.666/1993", .F., { || BIDDING_START() }}, ;
        { "Licitaçao", ASPEC_RESPATH() + "images/main/auction.png", "#DAA520", "Lei 14.133/2021", .T., { || BIDDING_NEW_START() }}, ;
        { "PPA", ASPEC_RESPATH() + "images/main/trend.png", "#FF6347", "", .F., { || TREND_START() }}, ;
        { "Patrimonial", ASPEC_RESPATH() + "images/main/skyline.png", "#CD853F", "", .F., { || ASSETS_START() }}, ;
        { "Almoxarifado", ASPEC_RESPATH() + "images/main/cubes.png", "#6A5ACD", "", .F., { || WAREHOUSE_START() }}, ;
        { "Frota", ASPEC_RESPATH() + "images/main/fleet.png", "#5B5784", "", .F., { || FLEET_START() }}, ;
        { "Doações", ASPEC_RESPATH() + "images/main/carry.png", "#8FBC8F", "", .F., { || DONATIONS_START() }}, ;
        { "Backup", ASPEC_RESPATH() + "images/main/carry.png", "#C0C0C0", "", .F., { || BACKUP_START() }} ;
        }

    O_MAINWINDOW := HBNAP_FORMS_LOAD(ASPEC_RESPATH() + "MainWindow.nfm", ASPEC_RESPATH(), HBNAP_FORMS_RESIZABLE)
    HBNAP_FORMS_TITLE(O_MAINWINDOW, "Sistemas Aspec")
    HBNAP_FORMS_MAIN_COVER(O_MAINWINDOW, "canvas", "Sistema de Gestão Pública", ASPEC_RESPATH() + "images/logo_aspec.png", V_COVER)
    HBNAP_FORMS_SHOW(O_MAINWINDOW, {|| MAIN_WINDOW_CLOSE() })
    RETURN

// Callback for main window closed. By default, exit the app.
FUNCTION MAIN_WINDOW_CLOSE()
    HBNAP_FORMS_DESTROY(O_MAINWINDOW)
    HBNAP_FORMS_EXIT_APP()
    RETURN .T.


// Main cover items callbacks
FUNCTION BUDGET_START()
    RETURN .T.

FUNCTION ACCOUNTING_START()
    RETURN .T.

FUNCTION BIDDING_START()
    RETURN .T.

FUNCTION BIDDING_NEW_START()
    RETURN .T.

FUNCTION TREND_START()
    RETURN .T.

FUNCTION ASSETS_START()
    RETURN .T.

FUNCTION WAREHOUSE_START()
    RETURN .T.

FUNCTION FLEET_START()
    RETURN .T.

FUNCTION DONATIONS_START()
    RETURN .T.

FUNCTION BACKUP_START()
    RETURN .T.

// Support func
FUNCTION ASPEC_RESPATH(C_NEW_PATH)
    STATIC C_RESPATH := ""
    LOCAL C_PREV_PATH := C_RESPATH
    IF C_NEW_PATH # NIL
        C_RESPATH := C_NEW_PATH
    ENDIF
    RETURN C_PREV_PATH
