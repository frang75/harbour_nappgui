//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

STATIC C_MAIN_TITLE := "Sistemas Aspec"
STATIC C_APP_NAME := "Sistema de Gestão Pública"
STATIC C_VERSION := "Versão 25.1h(b426)-S07688"
STATIC C_URL := "www.aspec.com.br"
STATIC C_COPYRIGHT := "Aspec ©1993-2026. Todos os direitos reservados"
STATIC O_MAINWINDOW := NIL

*********************
PROC EXEMPLO_HBNAP
*********************
    LOCAL V_COVER := { ;
        { "Orçamento", DIRET_FORMS() + "images/main/grid.png", "#20B2AA", "", .F., { || BUDGET_START() }}, ;
        { "Contábil SIAFIC", DIRET_FORMS() + "images/main/calc.png", "#1E90FF", "", .F., { || ACCOUNTING_START() }}, ;
        { "Licitaçao", DIRET_FORMS() + "images/main/auction.png", "#DAA520", "Lei 8.666/1993", .F., { || BIDDING_START() }}, ;
        { "Licitaçao", DIRET_FORMS() + "images/main/auction.png", "#DAA520", "Lei 14.133/2021", .T., { || BIDDING_NEW_START() }}, ;
        { "PPA", DIRET_FORMS() + "images/main/trend.png", "#FF6347", "", .F., { || TREND_START() }}, ;
        { "Patrimonial", DIRET_FORMS() + "images/main/skyline.png", "#CD853F", "", .F., { || ASSETS_START() }}, ;
        { "Almoxarifado", DIRET_FORMS() + "images/main/cubes.png", "#6A5ACD", "", .F., { || WAREHOUSE_START() }}, ;
        { "Frota", DIRET_FORMS() + "images/main/fleet.png", "#5B5784", "", .F., { || FLEET_START() }}, ;
        { "Doações", DIRET_FORMS() + "images/main/carry.png", "#8FBC8F", "", .F., { || DONATIONS_START() }}, ;
        { "Backup", DIRET_FORMS() + "images/main/backup.png", "#C0C0C0", "", .F., { || BACKUP_START() }} ;
    }

    O_MAINWINDOW := HBNAP_FORMS_LOAD(DIRET_FORMS() + "MainWindow.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC))
    HBNAP_FORMS_TITLE(O_MAINWINDOW, C_MAIN_TITLE)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "url", C_URL)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "version", C_VERSION)
    HBNAP_FORMS_SET_TEXT(O_MAINWINDOW, "copyright", C_COPYRIGHT)
    HBNAP_FORMS_MAIN_COVER(O_MAINWINDOW, "canvas", C_APP_NAME, DIRET_FORMS() + "images/logo_aspec.png", V_COVER)
    HBNAP_FORMS_MODAL_GTNAP(O_MAINWINDOW)
    HBNAP_FORMS_DESTROY(O_MAINWINDOW)
    RETURN

FUNCTION INFO_MESSAGE_BOX(C_TEXT, O_PARENT_WINDOW)
    LOCAL O_MESSAGE := HBNAP_FORMS_LOAD(DIRET_FORMS() + "InfoMessageBox.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
    HBNAP_FORMS_SET_TEXT(O_MESSAGE, "text", C_TEXT)
    HBNAP_FORMS_TITLE(O_MESSAGE, "Aviso")
    HBNAP_FORMS_ONCLICK(O_MESSAGE, "button", {|| HBNAP_FORMS_STOP_MODAL(O_MESSAGE, 0)})
    HBNAP_FORMS_MODAL(O_MESSAGE, O_PARENT_WINDOW)
    HBNAP_FORMS_DESTROY(O_MESSAGE)
    RETURN .T.

// Main cover items callbacks
FUNCTION BUDGET_START()
    TST_FORM_EMPRESAS(O_MAINWINDOW)
    RETURN .T.
    //RETURN INFO_MESSAGE_BOX("Aqui será implementado o código dedicado à gestão orçamentária.")

FUNCTION ACCOUNTING_START()
    RETURN INFO_MESSAGE_BOX("Accounting", O_MAINWINDOW)

FUNCTION BIDDING_START()
    RETURN INFO_MESSAGE_BOX("Bidding", O_MAINWINDOW)

FUNCTION BIDDING_NEW_START()
    RETURN INFO_MESSAGE_BOX("Bidding new", O_MAINWINDOW)

FUNCTION TREND_START()
    RETURN INFO_MESSAGE_BOX("Trend", O_MAINWINDOW)

FUNCTION ASSETS_START()
    RETURN INFO_MESSAGE_BOX("Assets", O_MAINWINDOW)

FUNCTION WAREHOUSE_START()
    RETURN INFO_MESSAGE_BOX("Warehouse", O_MAINWINDOW)

FUNCTION FLEET_START()
    RETURN INFO_MESSAGE_BOX("Fleet", O_MAINWINDOW)

FUNCTION DONATIONS_START()
    RETURN INFO_MESSAGE_BOX("Donations", O_MAINWINDOW)

FUNCTION BACKUP_START()
    RETURN INFO_MESSAGE_BOX("Backup", O_MAINWINDOW)
