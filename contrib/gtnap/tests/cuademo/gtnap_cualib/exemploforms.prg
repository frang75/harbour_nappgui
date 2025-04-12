/* encoding: cp850 */
#INCLUDE "gtnap.ch"
#INCLUDE "cua.ch"

*******************
PROC EXEMPLO_FORMS
*******************
LOCAL V_JANELA
*
CUA20 @ 15,20,25,70 JANELA V_JANELA ;
     TITULO "Escolha o tipo de formulário" SUBTITULO "%T";
     AJUDA "T?????"
*
ESPECIALIZE V_JANELA MENU
ADDOPCAO V_JANELA TEXTO "Formulário #básico" ;
    ACAO TST_FORM_BASICO() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "Formulário com #TextView" ;
    ACAO TST_FORM_TEXT() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "Formulário de #login" ;
    ACAO TST_FORM_LOGIN() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "PopUp e ListBox" ;
    ACAO TST_POPUP_LIST() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "#Menus dinâmicos" ;
    ACAO TST_FORM_DYNMENU() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "Browse de #DBF" ;
    ACAO TST_FORM_TABLEVIEW() AJUDA "P?????"
*
ATIVE(V_JANELA)
*

********************************
STAT PROC TST_FORM_BASICO
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "Customer.nfm")
LOCAL C_NAME := "Francisco"
LOCAL C_LAST := "García Collado"
LOCAL C_ADDRESS := "Calle de la Cueva, 23"
LOCAL C_CITY := "Alicante"
LOCAL C_PHONE := "+34 600111777"
LOCAL C_USER := "fran75"
LOCAL C_PASS := "pass5566"
LOCAL C_BANK := {"WE0012", "556431", "887652", "223477"}
LOCAL C_CARD := {"5678", "9900", "1122", "3344", "5566"}
LOCAL L_MAIL_LIST := .T.
LOCAL L_SECURE_PASS := .F.
LOCAL L_SHOW_ALERTS := .T.
LOCAL L_CONNECT_BANK := .F.
// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"edit_first_name", @C_NAME }, ;
                    {"edit_last_name", @C_LAST }, ;
                    {"edit_address", @C_ADDRESS }, ;
                    {"edit_city", @C_CITY }, ;
                    {"edit_phone", @C_PHONE }, ;
                    {"edit_user", @C_USER }, ;
                    {"edit_pass", @C_PASS }, ;
                    {"edit_bank1", @C_BANK[1] }, ;
                    {"edit_bank1", @C_BANK[1] }, ;
                    {"edit_bank2", @C_BANK[2] }, ;
                    {"edit_bank3", @C_BANK[3] }, ;
                    {"edit_bank4", @C_BANK[4] }, ;
                    {"edit_credit1", @C_CARD[1] }, ;
                    {"edit_credit2", @C_CARD[2] }, ;
                    {"edit_credit3", @C_CARD[3] }, ;
                    {"edit_credit4", @C_CARD[4] }, ;
                    {"edit_credit5", @C_CARD[5] }, ;
                    {"check_mail_list", @L_MAIL_LIST }, ;
                    {"check_secure_pass", @L_SECURE_PASS }, ;
                    {"check_show_alerts", @L_SHOW_ALERTS }, ;
                    {"check_connect_bank", @L_CONNECT_BANK } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Primeiro exemplo de formulário GTNAP")
// Write the variable values into the form controls (Edit, Buttons, etc)
NAP_FORM_DBIND(V_FORM, V_BIND)
// Buttons callback
NAP_FORM_ONCLICK(V_FORM, "button_ok", {|| NAP_FORM_STOP_MODAL(V_FORM, 1000) })
NAP_FORM_ONCLICK(V_FORM, "button_cancel", {|| NAP_FORM_STOP_MODAL(V_FORM, 1001) })

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == 1000
    MOSTRAR("M?????","Botão [OK] pressionado, dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSEIF N_RES == 1001
    MOSTRAR("M?????","Botão [Cancelar] pressionado, dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

    // Write the values from the GUI controls to Harbour variables
    NAP_FORM_DBIND_STORE(V_FORM)

    C_MESSAGE := "C_NAME: " + C_NAME + ";" + ;
                 "C_LAST: " + C_LAST + ";" + ;
                 "C_ADDRESS: " + C_ADDRESS + ";" + ;
                 "C_CITY: " + C_CITY + ";" + ;
                 "C_PHONE: " + C_PHONE + ";" + ;
                 "C_USER: " + C_USER + ";" + ;
                 "C_PASS: " + C_PASS + ";" + ;
                 "C_BANK: " + C_BANK[1] + "-" + C_BANK[2] + "-" + C_BANK[3] + "-" + C_BANK[4] + ";" + ;
                 "C_CARD: " + C_CARD[1] + "-" + C_CARD[2] + "-" + C_CARD[3] + "-" + C_CARD[4] + "-" + C_CARD[5] + ";" + ;
                 "L_MAIL_LIST: " + hb_ValToStr(L_MAIL_LIST) + ";" + ;
                 "L_SECURE_PASS: " + hb_ValToStr(L_SECURE_PASS) + ";" + ;
                 "L_SHOW_ALERTS: " + hb_ValToStr(L_SHOW_ALERTS) + ";" + ;
                 "L_CONNECT_BANK: " + hb_ValToStr(L_CONNECT_BANK)

    MOSTRAR("M?????",C_Message)

ENDIF

NAP_FORM_DESTROY(V_FORM)

********************************
STAT PROC TST_FORM_TEXT
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "TextView.nfm")
LOCAL C_TEXT1 := "Lorem ipsum odor amet, consectetuer adipiscing elit. Fringilla luctus sagittis lacus praesent sagittis accumsan. Egestas ut ridiculus adipiscing et pretium efficitur. Venenatis curabitur tristique semper; cras cubilia iaculis."
LOCAL C_TEXT2 := "Mi penatibus penatibus magnis tortor habitant commodo dapibus vitae tortor. Commodo elit nascetur fermentum primis proin augue mollis. Cubilia laoreet habitant ultricies; feugiat donec volutpat. Semper eleifend eget tempus aliquam in dis ornare felis."
// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"text1", @C_TEXT1 }, ;
                    {"text2", @C_TEXT2 } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Formulário com visualizações de texto")
// Write the variable values into the form controls (Edit, Buttons, etc)
NAP_FORM_DBIND(V_FORM, V_BIND)
// Buttons callback
NAP_FORM_ONCLICK(V_FORM, "button_ok", {|| NAP_FORM_STOP_MODAL(V_FORM, 1000) })
NAP_FORM_ONCLICK(V_FORM, "button_cancel", {|| NAP_FORM_STOP_MODAL(V_FORM, 1001) })

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == 1000
    MOSTRAR("M?????","Botão [OK] pressionado, dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSEIF N_RES == 1001
    MOSTRAR("M?????","Botão [Cancelar] pressionado, dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

    // Write the values from the GUI controls to Harbour variables
    NAP_FORM_DBIND_STORE(V_FORM)

    C_MESSAGE := "C_TEXT1: " + C_TEXT1 + ";" + ;
                 "C_TEXT2: " + C_TEXT2

    MOSTRAR("M?????",C_Message)

ENDIF

NAP_FORM_DESTROY(V_FORM)

********************************
STAT PROC TST_FORM_LOGIN
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "Login.nfm")
LOCAL C_USER := "frang@nappgui.com"
LOCAL C_PASS := "1234567890assddf"
LOCAL N_SLIDER := 0.2

// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"user_edit", @C_USER }, ;
                    {"pass_edit", @C_PASS }, ;
                    {"slider", @N_SLIDER }, ;
                    {"progress", N_SLIDER } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Formulário de login")
// Write the variable values into the form controls (Edit, Buttons, etc)
NAP_FORM_DBIND(V_FORM, V_BIND)

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == 1000
    MOSTRAR("M?????","Botão [OK] pressionado, dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSEIF N_RES == 1001
    MOSTRAR("M?????","Botão [Cancelar] pressionado, dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

    // Write the values from the GUI controls to Harbour variables
    NAP_FORM_DBIND_STORE(V_FORM)

    C_MESSAGE := "C_USER: " + C_USER + ";" + ;
                 "C_PASS: " + C_PASS + ";" + ;
                 "N_SLIDER: " + hb_ntos(N_SLIDER)

    MOSTRAR("M?????",C_Message)

ENDIF

NAP_FORM_DESTROY(V_FORM)


**************************
STAT PROC TST_POPUP_LIST()
**************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "PopUp_And_List.nfm")
LOCAL N_POPUP_SEL := 1
LOCAL N_LISTBOX_SEL := 2

// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"popup_cell", @N_POPUP_SEL }, ;
                    {"listbox_cell", @N_LISTBOX_SEL } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Exemplo de PopUp e ListBox")
// Write the variable values into the form controls (Edit, Buttons, etc)
NAP_FORM_DBIND(V_FORM, V_BIND)

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER

    // Write the values from the GUI controls to Harbour variables
    NAP_FORM_DBIND_STORE(V_FORM)

    C_MESSAGE := "N_POPUP_SEL: " + hb_ntos(N_POPUP_SEL) + ";" + ;
                 "N_LISTBOX_SEL: " + hb_ntos(N_LISTBOX_SEL)

    MOSTRAR("M?????",C_Message)

ENDIF

NAP_FORM_DESTROY(V_FORM)

********************************
STAT PROC ITEM_CLICKED(V_FORM, O_ITEM)
********************************
LOCAL C_TEXT := NAP_DMENUITEM_GET_TEXT(O_ITEM)
NAP_FORM_INSERT_TEXT(V_FORM, "textview", "Clicked: " + C_TEXT + hb_eol())

*******************************************
STAT FUNCTION EXAMPLE_APP_DYN_MENU(V_FORM)
*******************************************
// Main menu
LOCAL O_MAINMENU := NAP_DMENU_CREATE()
// Submenus
LOCAL O_FILEMENU := NAP_DMENU_CREATE()
LOCAL O_NAVMENU := NAP_DMENU_CREATE()
LOCAL O_SERVMENU := NAP_DMENU_CREATE()
LOCAL O_LANGMENU := NAP_DMENU_CREATE()
LOCAL O_HELPMENU := NAP_DMENU_CREATE()
// Items (clickable)
LOCAL O_MAINITEM1 := NAP_DMENUITEM_CREATE("File", NIL, NIL)
LOCAL O_MAINITEM2 := NAP_DMENUITEM_CREATE("Navigate", NIL, NIL)
LOCAL O_MAINITEM3 := NAP_DMENUITEM_CREATE("Server", NIL, NIL)
LOCAL O_MAINITEM4 := NAP_DMENUITEM_CREATE("Language", NIL, NIL)
LOCAL O_MAINITEM5 := NAP_DMENUITEM_CREATE("Help", NIL, NIL)
LOCAL O_FILEITEM1 := NAP_DMENUITEM_CREATE("Open", DIRET_FORMS() + "icons/open.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_FILEITEM2 := NAP_DMENUITEM_CREATE("Save", DIRET_FORMS() + "icons/save.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_FILEITEM3 := NAP_DMENUITEM_CREATE("Exit", DIRET_FORMS() + "icons/exit.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_NAVITEM1 := NAP_DMENUITEM_CREATE("First", DIRET_FORMS() + "icons/first.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_NAVITEM2 := NAP_DMENUITEM_CREATE("Back", DIRET_FORMS() + "icons/back.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_NAVITEM3 := NAP_DMENUITEM_CREATE("Next", DIRET_FORMS() + "icons/next.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_NAVITEM4 := NAP_DMENUITEM_CREATE("Last", DIRET_FORMS() + "icons/last.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_SERVITEM1 := NAP_DMENUITEM_CREATE("Login", DIRET_FORMS() + "icons/login.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_SERVITEM2 := NAP_DMENUITEM_CREATE("Logout", DIRET_FORMS() + "icons/logout.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_SERVITEM3 := NAP_DMENUITEM_CREATE("Settings", DIRET_FORMS() + "icons/settings.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM1 := NAP_DMENUITEM_CREATE("English", DIRET_FORMS() + "icons/usa.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM2 := NAP_DMENUITEM_CREATE("Portuguese", DIRET_FORMS() + "icons/portugal.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM3 := NAP_DMENUITEM_CREATE("Spanish", DIRET_FORMS() + "icons/spain.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM4 := NAP_DMENUITEM_CREATE("Italian", DIRET_FORMS() + "icons/italy.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM5 := NAP_DMENUITEM_CREATE("Japanese", DIRET_FORMS() + "icons/japan.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM6 := NAP_DMENUITEM_CREATE("Russian", DIRET_FORMS() + "icons/russia.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_LANGITEM7 := NAP_DMENUITEM_CREATE("Vietnamese", DIRET_FORMS() + "icons/vietnam.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
LOCAL O_HELPITEM1 := NAP_DMENUITEM_CREATE("About", DIRET_FORMS() + "icons/about.png", {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})

// Add items to each menu
NAP_DMENU_ADD_ITEM(O_FILEMENU, O_FILEITEM1)
NAP_DMENU_ADD_ITEM(O_FILEMENU, O_FILEITEM2)
NAP_DMENU_ADD_ITEM(O_FILEMENU, NAP_DMENUITEM_SEPARATOR())
NAP_DMENU_ADD_ITEM(O_FILEMENU, O_FILEITEM3)

NAP_DMENU_ADD_ITEM(O_NAVMENU, O_NAVITEM1)
NAP_DMENU_ADD_ITEM(O_NAVMENU, O_NAVITEM2)
NAP_DMENU_ADD_ITEM(O_NAVMENU, O_NAVITEM3)
NAP_DMENU_ADD_ITEM(O_NAVMENU, O_NAVITEM4)

NAP_DMENU_ADD_ITEM(O_SERVMENU, O_SERVITEM1)
NAP_DMENU_ADD_ITEM(O_SERVMENU, O_SERVITEM2)
NAP_DMENU_ADD_ITEM(O_SERVMENU, NAP_DMENUITEM_SEPARATOR())
NAP_DMENU_ADD_ITEM(O_SERVMENU, O_SERVITEM3)

NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM1)
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM2)
NAP_DMENU_ADD_ITEM(O_LANGMENU, NAP_DMENUITEM_SEPARATOR())
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM3)
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM4)
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM5)
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM6)
NAP_DMENU_ADD_ITEM(O_LANGMENU, O_LANGITEM7)

NAP_DMENU_ADD_ITEM(O_HELPMENU, O_HELPITEM1)

// Link submenus with main menu items
NAP_DMENUITEM_SUBMENU(O_MAINITEM1, O_FILEMENU)
NAP_DMENUITEM_SUBMENU(O_MAINITEM2, O_NAVMENU)
NAP_DMENUITEM_SUBMENU(O_MAINITEM3, O_SERVMENU)
NAP_DMENUITEM_SUBMENU(O_MAINITEM4, O_LANGMENU)
NAP_DMENUITEM_SUBMENU(O_MAINITEM5, O_HELPMENU)

// Add main menu items to main menu
NAP_DMENU_ADD_ITEM(O_MAINMENU, O_MAINITEM1)
NAP_DMENU_ADD_ITEM(O_MAINMENU, O_MAINITEM2)
NAP_DMENU_ADD_ITEM(O_MAINMENU, O_MAINITEM3)
NAP_DMENU_ADD_ITEM(O_MAINMENU, O_MAINITEM4)
NAP_DMENU_ADD_ITEM(O_MAINMENU, O_MAINITEM5)

RETURN O_MAINMENU

*******************************************
STAT PROC LAUNCH_POPUP_MENU(O_MENU, V_FORM)
*******************************************
// Get the button frame in screen coordinates {x, y, width, height}
LOCAL V_FRAME := NAP_FORM_CONTROL_FRAME(V_FORM, "button_launchpopup")

// Unset the menu as menubar rol (if has this role)
NAP_DMENU_BAR(NIL, V_FORM)

// Launch the menu as popup, at the top-left corner of 'button_launchpopup'
NAP_DMENU_POPUP(O_MENU, V_FORM, V_FRAME[1], V_FRAME[2])

********************************
STAT FUNCTION FORM_NEW_ITEM(C_NAME, N_ITEMS)
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "NewItem.nfm")
LOCAL L_OK := .F.

// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"edit_name", @C_NAME }, ;
                    {"edit_num_subitems", @N_ITEMS } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Add new menu items")

// Write the variable values into the form controls (Edit, Buttons, etc)
NAP_FORM_DBIND(V_FORM, V_BIND)

// Buttons callback
NAP_FORM_ONCLICK(V_FORM, "button_ok", {|| NAP_FORM_STOP_MODAL(V_FORM, 1000) })
NAP_FORM_ONCLICK(V_FORM, "button_cancel", {|| NAP_FORM_STOP_MODAL(V_FORM, 1001) })

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == 1000
    MOSTRAR("M?????","Botão [OK] pressionado, dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSEIF N_RES == 1001
    MOSTRAR("M?????","Botão [Cancelar] pressionado, dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

    // Write the values from the GUI controls to Harbour variables
    NAP_FORM_DBIND_STORE(V_FORM)

    C_MESSAGE := "C_NAME: " + C_NAME + ";" + ;
                 "N_ITEMS: " + hb_ntos(N_ITEMS)

    MOSTRAR("M?????",C_Message)
    L_OK := .T.

ENDIF

NAP_FORM_DESTROY(V_FORM)
RETURN L_OK

***********************************************
STAT PROC INSERT_NEW_MENUITEM(O_MENU, V_FORM)
***********************************************
// Add a new submenu to main menu bar at runtime
LOCAL C_NAME := "NewItem"
LOCAL N_ITEMS := 3
LOCAL O_SUBMENU := NIL
LOCAL O_ITEM := NIL
LOCAL N_CONT := 0

// A form to get the name and items of new menu option
LOCAL L_OK := FORM_NEW_ITEM(@C_NAME, @N_ITEMS)

IF L_OK == .T.
    // Create the submenu
    O_SUBMENU := NAP_DMENU_CREATE()
    FOR N_CONT := 1 TO N_ITEMS
        O_ITEM := NAP_DMENUITEM_CREATE(C_NAME + "-" + hb_ntos(N_CONT), NIL, {| O_ITEM | ITEM_CLICKED(V_FORM, O_ITEM)})
        NAP_DMENU_ADD_ITEM(O_SUBMENU, O_ITEM)
    NEXT

    // Create the new item, attach its submenu and insert the item in first position of main menu
    O_ITEM = NAP_DMENUITEM_CREATE(C_NAME, NIL, NIL)
    NAP_DMENUITEM_SUBMENU(O_ITEM, O_SUBMENU)
    NAP_DMENU_INS_ITEM(O_MENU, 0, O_ITEM)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF NAP_DMENU_IS_MENUBAR(O_MENU) == .T.
        NAP_FORM_UPDATE(V_FORM)
    ENDIF

ENDIF

******************************************
STAT PROC REMOVE_MENUITEM(O_MENU, V_FORM)
******************************************
LOCAL N_ITEMS := NAP_DMENU_COUNT(O_MENU)

IF N_ITEMS > 0
    // Remove the first item in main menu
    NAP_DMENU_DEL_ITEM(O_MENU, 0)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF NAP_DMENU_IS_MENUBAR(O_MENU) == .T.
        NAP_FORM_UPDATE(V_FORM)
    ENDIF
ENDIF


********************************
STAT PROC TST_FORM_DYNMENU
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "DynMenus.nfm")
LOCAL O_MENU := EXAMPLE_APP_DYN_MENU(V_FORM)
LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
NAP_FORM_TITLE(V_FORM, "Exemplo de Menus Dinâmicos")

// Buttons callback
NAP_FORM_ONCLICK(V_FORM, "button_setmenubar", {|| NAP_DMENU_BAR(O_MENU, V_FORM) })
NAP_FORM_ONCLICK(V_FORM, "button_unsetmenubar", {|| NAP_DMENU_BAR(NIL, V_FORM) })
NAP_FORM_ONCLICK(V_FORM, "button_launchpopup", {|| LAUNCH_POPUP_MENU(O_MENU, V_FORM) })
NAP_FORM_ONCLICK(V_FORM, "button_insert0", {|| INSERT_NEW_MENUITEM(O_MENU, V_FORM) })
NAP_FORM_ONCLICK(V_FORM, "button_remove0", {|| REMOVE_MENUITEM(O_MENU, V_FORM) })

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == 1000
    MOSTRAR("M?????","Botão [OK] pressionado, dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

IF N_RES == NAP_MODAL_ENTER .OR. N_RES == 1000

ENDIF

NAP_FORM_DESTROY(V_FORM)
NAP_DMENU_DESTROY(O_MENU)



********************************
STAT PROC TST_FORM_TABLEVIEW
********************************
LOCAL V_FORM := NAP_FORM_LOAD(DIRET_FORMS() + "TableDBF.nfm")
LOCAL N_RES := 0
// Mapping between Harbour working area and TableView
LOCAL V_DBBIND := { ;
    "tablecell", ;
     { {|| cotacao->cdindx} }, ;
     { {|| cotacao->dtcota} }, ;
     { {|| cotacao->dtcota + 1} }, ;
     { {|| cotacao->dtcota + 2} }, ;
     { {|| TRANSFORM(cotacao->vlcota,"@E 999,999,999,999.99999999")} } ;
}

USE ../dados/cotacao NEW SHARED
SET INDEX TO ../dados/cotacao
GOTO TOP

// Window title
NAP_FORM_TITLE(V_FORM, "Visualizador de dados com formulários GTNAP")

// Map TableView columns with DB
NAP_FORM_DBIND_AREA(V_FORM, V_DBBIND)

// Launch the form
N_RES := NAP_FORM_MODAL(V_FORM, DIRET_FORMS())

IF N_RES == NAP_MODAL_ENTER
    MOSTRAR("M?????","Pressionado [Enter], dados aceitos.")
ELSEIF N_RES == NAP_MODAL_ESC
    MOSTRAR("M?????","ESC pressionado, dados cancelados.")
ELSEIF N_RES == NAP_MODAL_X_BUTTON
    MOSTRAR("M?????","Formulário fechado com [X], dados cancelados.")
ELSE
    MOSTRAR("M?????","Valor de retorno desconhecido.")
ENDIF

NAP_FORM_DESTROY(V_FORM)

CLOSE COTACAO

