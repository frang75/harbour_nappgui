/* Example of ASPEC application using HBNAP-Forms */
#INCLUDE "ord.ch"       // For RDD API Index Order
#INCLUDE "hbnap.ch"     // For HBNAP API

STATIC C_MAIN_TITLE := "Sistemas Aspec"
STATIC C_APP_NAME := "Sistema de Gestão Pública"
STATIC C_VERSION := "Versão 25.1h(b426)-S07688"
STATIC C_URL := "www.aspec.com.br"
STATIC C_COPYRIGHT := "Aspec ©1993-2026. Todos os direitos reservados"
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
        { "Backup", ASPEC_RESPATH() + "images/main/backup.png", "#C0C0C0", "", .F., { || BACKUP_START() }} ;
    }

    O_MAINWINDOW := HBNAP_FORMS_LOAD(ASPEC_RESPATH() + "MainWindow.nfm", ASPEC_RESPATH(), HBNAP_FORMS_RESIZABLE)
    HBNAP_FORMS_TITLE(O_MAINWINDOW, C_MAIN_TITLE)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "url", C_URL)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "version", C_VERSION)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "copyright", C_COPYRIGHT)
    HBNAP_FORMS_MAIN_COVER(O_MAINWINDOW, "canvas", C_APP_NAME, ASPEC_RESPATH() + "images/logo_aspec.png", V_COVER)
    HBNAP_FORMS_MAXIMIZE(O_MAINWINDOW)
    HBNAP_FORMS_SHOW(O_MAINWINDOW, {|| MAIN_WINDOW_CLOSE() })
    RETURN

// Callback for main window closed. By default, exit the app.
FUNCTION MAIN_WINDOW_CLOSE()
    HBNAP_FORMS_DESTROY(O_MAINWINDOW)
    HBNAP_FORMS_EXIT_APP()
    RETURN .T.


FUNCTION INFO_MESSAGE_BOX(C_TEXT)
    LOCAL O_MESSAGE := HBNAP_FORMS_LOAD(ASPEC_RESPATH() + "InfoMessageBox.nfm", ASPEC_RESPATH(), hb_bitOr(HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
    HBNAP_FORMS_SET_TEXT(O_MESSAGE, "text", C_TEXT)
    HBNAP_FORMS_TITLE(O_MESSAGE, "Aviso")
    HBNAP_FORMS_ONCLICK(O_MESSAGE, "button", {|| HBNAP_FORMS_STOP_MODAL(O_MESSAGE, 0)})
    HBNAP_FORMS_MODAL(O_MESSAGE, O_MAINWINDOW)
    HBNAP_FORMS_DESTROY(O_MESSAGE)
    RETURN .T.

// Main cover items callbacks
FUNCTION BUDGET_START()
    RETURN INFO_MESSAGE_BOX("Aqui será implementado o código dedicado à gestão orçamentária.")

FUNCTION ACCOUNTING_START()
    RETURN INFO_MESSAGE_BOX("Accounting")

FUNCTION BIDDING_START()
    RETURN INFO_MESSAGE_BOX("Bidding")

FUNCTION BIDDING_NEW_START()
    RETURN INFO_MESSAGE_BOX("Bidding new")

FUNCTION TREND_START()
    RETURN INFO_MESSAGE_BOX("Trend")

FUNCTION ASSETS_START()
    RETURN INFO_MESSAGE_BOX("Assets")

FUNCTION WAREHOUSE_START()
    RETURN INFO_MESSAGE_BOX("Warehouse")

FUNCTION FLEET_START()
    RETURN INFO_MESSAGE_BOX("Fleet")

FUNCTION DONATIONS_START()
    RETURN INFO_MESSAGE_BOX("Donations")

FUNCTION BACKUP_START()
    RETURN INFO_MESSAGE_BOX("Backup")

// Support funcs
FUNCTION ASPEC_RESPATH(C_NEW_PATH)
    STATIC C_RESPATH := ""
    LOCAL C_PREV_PATH := C_RESPATH
    IF C_NEW_PATH # NIL
        C_RESPATH := C_NEW_PATH
    ENDIF
    RETURN C_PREV_PATH
