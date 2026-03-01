//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
STAT PROC ITEM_CLICKED(O_FORM, O_ITEM)
********************************
LOCAL C_TEXT := HBNAP_MENUITEM_GET_TEXT(O_ITEM)
HBNAP_FORMS_INSERT_TEXT(O_FORM, "textview", "Clicked: " + C_TEXT + hb_eol())

*******************************************
STAT FUNCTION EXAMPLE_APP_DYN_MENU(O_FORM)
*******************************************
// Main menu
LOCAL O_MAINMENU := HBNAP_MENU_CREATE()
// Submenus
LOCAL O_FILEMENU := HBNAP_MENU_CREATE()
LOCAL O_NAVMENU := HBNAP_MENU_CREATE()
LOCAL O_SERVMENU := HBNAP_MENU_CREATE()
LOCAL O_LANGMENU := HBNAP_MENU_CREATE()
LOCAL O_HELPMENU := HBNAP_MENU_CREATE()
// Items (clickable)
LOCAL O_MAINITEM1 := HBNAP_MENUITEM_CREATE("File", NIL, NIL)
LOCAL O_MAINITEM2 := HBNAP_MENUITEM_CREATE("Navigate", NIL, NIL)
LOCAL O_MAINITEM3 := HBNAP_MENUITEM_CREATE("Server", NIL, NIL)
LOCAL O_MAINITEM4 := HBNAP_MENUITEM_CREATE("Language", NIL, NIL)
LOCAL O_MAINITEM5 := HBNAP_MENUITEM_CREATE("Help", NIL, NIL)
LOCAL O_FILEITEM1 := HBNAP_MENUITEM_CREATE("Open", DIRET_FORMS() + "icons/open.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_FILEITEM2 := HBNAP_MENUITEM_CREATE("Save", DIRET_FORMS() + "icons/save.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_FILEITEM3 := HBNAP_MENUITEM_CREATE("Exit", DIRET_FORMS() + "icons/exit.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM1 := HBNAP_MENUITEM_CREATE("First", DIRET_FORMS() + "icons/first.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM2 := HBNAP_MENUITEM_CREATE("Back", DIRET_FORMS() + "icons/back.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM3 := HBNAP_MENUITEM_CREATE("Next", DIRET_FORMS() + "icons/next.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM4 := HBNAP_MENUITEM_CREATE("Last", DIRET_FORMS() + "icons/last.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM1 := HBNAP_MENUITEM_CREATE("Login", DIRET_FORMS() + "icons/login.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM2 := HBNAP_MENUITEM_CREATE("Logout", DIRET_FORMS() + "icons/logout.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM3 := HBNAP_MENUITEM_CREATE("Settings", DIRET_FORMS() + "icons/settings.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM1 := HBNAP_MENUITEM_CREATE("English", DIRET_FORMS() + "icons/usa.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM2 := HBNAP_MENUITEM_CREATE("Portuguese", DIRET_FORMS() + "icons/portugal.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM3 := HBNAP_MENUITEM_CREATE("Spanish", DIRET_FORMS() + "icons/spain.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM4 := HBNAP_MENUITEM_CREATE("Italian", DIRET_FORMS() + "icons/italy.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM5 := HBNAP_MENUITEM_CREATE("Japanese", DIRET_FORMS() + "icons/japan.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM6 := HBNAP_MENUITEM_CREATE("Russian", DIRET_FORMS() + "icons/russia.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM7 := HBNAP_MENUITEM_CREATE("Vietnamese", DIRET_FORMS() + "icons/vietnam.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_HELPITEM1 := HBNAP_MENUITEM_CREATE("About", DIRET_FORMS() + "icons/about.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})

// Add items to each menu
HBNAP_MENU_ADD_ITEM(O_FILEMENU, O_FILEITEM1)
HBNAP_MENU_ADD_ITEM(O_FILEMENU, O_FILEITEM2)
HBNAP_MENU_ADD_ITEM(O_FILEMENU, HBNAP_MENUITEM_SEPARATOR())
HBNAP_MENU_ADD_ITEM(O_FILEMENU, O_FILEITEM3)

HBNAP_MENU_ADD_ITEM(O_NAVMENU, O_NAVITEM1)
HBNAP_MENU_ADD_ITEM(O_NAVMENU, O_NAVITEM2)
HBNAP_MENU_ADD_ITEM(O_NAVMENU, O_NAVITEM3)
HBNAP_MENU_ADD_ITEM(O_NAVMENU, O_NAVITEM4)

HBNAP_MENU_ADD_ITEM(O_SERVMENU, O_SERVITEM1)
HBNAP_MENU_ADD_ITEM(O_SERVMENU, O_SERVITEM2)
HBNAP_MENU_ADD_ITEM(O_SERVMENU, HBNAP_MENUITEM_SEPARATOR())
HBNAP_MENU_ADD_ITEM(O_SERVMENU, O_SERVITEM3)

HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM1)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM2)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, HBNAP_MENUITEM_SEPARATOR())
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM3)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM4)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM5)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM6)
HBNAP_MENU_ADD_ITEM(O_LANGMENU, O_LANGITEM7)

HBNAP_MENU_ADD_ITEM(O_HELPMENU, O_HELPITEM1)

// Link submenus with main menu items
HBNAP_MENUITEM_SUBMENU(O_MAINITEM1, O_FILEMENU)
HBNAP_MENUITEM_SUBMENU(O_MAINITEM2, O_NAVMENU)
HBNAP_MENUITEM_SUBMENU(O_MAINITEM3, O_SERVMENU)
HBNAP_MENUITEM_SUBMENU(O_MAINITEM4, O_LANGMENU)
HBNAP_MENUITEM_SUBMENU(O_MAINITEM5, O_HELPMENU)

// Add main menu items to main menu
HBNAP_MENU_ADD_ITEM(O_MAINMENU, O_MAINITEM1)
HBNAP_MENU_ADD_ITEM(O_MAINMENU, O_MAINITEM2)
HBNAP_MENU_ADD_ITEM(O_MAINMENU, O_MAINITEM3)
HBNAP_MENU_ADD_ITEM(O_MAINMENU, O_MAINITEM4)
HBNAP_MENU_ADD_ITEM(O_MAINMENU, O_MAINITEM5)

RETURN O_MAINMENU

*******************************************
STAT PROC LAUNCH_POPUP_MENU(O_MENU, O_FORM)
*******************************************
// Get the button frame in screen coordinates {x, y, width, height}
LOCAL V_FRAME := HBNAP_FORMS_CONTROL_FRAME(O_FORM, "button_launchpopup")

// Unset the menu as menubar rol (if has this role)
HBNAP_MENU_BAR(NIL, O_FORM)

// Launch the menu as popup, at the top-left corner of 'button_launchpopup'
HBNAP_MENU_POPUP(O_MENU, O_FORM, V_FRAME[1], V_FRAME[2])

********************************
STAT FUNCTION FORM_NEW_ITEM(C_NAME, N_ITEMS, O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "NewItem.nfm", DIRET_FORMS(), HBNAP_FORMS_CLOSE_ON_ESC)
LOCAL L_OK := .F.

// Mapping between Harbour variables and form control names
LOCAL V_BIND := { ;
                    {"edit_name", @C_NAME }, ;
                    {"edit_num_subitems", @N_ITEMS } ;
                }

LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
HBNAP_FORMS_TITLE(O_FORM, "Add new menu items")

// Write the variable values into the form controls (Edit, Buttons, etc)
HBNAP_FORMS_BIND(O_FORM, V_BIND)

// Buttons callback
HBNAP_FORMS_ONCLICK(O_FORM, "button_ok", {|| HBNAP_FORMS_STOP_MODAL(O_FORM, 1000) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_cancel", {|| HBNAP_FORMS_STOP_MODAL(O_FORM, 1001) })

// Launch the form
N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_RETURN
    INFO_MESSAGE_BOX("Pressionado [Enter], dados aceitos.", O_PARENT_FORM)
ELSEIF N_RES == 1000
    INFO_MESSAGE_BOX("Botão [OK] pressionado, dados aceitos.", O_PARENT_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_ESC
    INFO_MESSAGE_BOX("ESC pressionado, dados cancelados.", O_PARENT_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
    INFO_MESSAGE_BOX("Formulário fechado com [X], dados cancelados.", O_PARENT_FORM)
ELSEIF N_RES == 1001
    INFO_MESSAGE_BOX("Botão [Cancelar] pressionado, dados cancelados.", O_PARENT_FORM)
ELSE
    INFO_MESSAGE_BOX("Valor de retorno desconhecido.", O_PARENT_FORM)
ENDIF

IF N_RES == HBNAP_CLOSED_BY_RETURN .OR. N_RES == 1000

    // Write the values from the GUI controls to Harbour variables
    HBNAP_FORMS_BIND_STORE(O_FORM)
    C_MESSAGE := "C_NAME: " + C_NAME + hb_eol() + "N_ITEMS: " + hb_ntos(N_ITEMS)
    INFO_MESSAGE_BOX(C_Message, O_PARENT_FORM)
    L_OK := .T.

ENDIF

HBNAP_FORMS_DESTROY(O_FORM)
RETURN L_OK

***********************************************
STAT PROC INSERT_NEW_MENUITEM(O_MENU, O_FORM)
***********************************************
// Add a new submenu to main menu bar at runtime
LOCAL C_NAME := "NewItem"
LOCAL N_ITEMS := 3
LOCAL O_SUBMENU := NIL
LOCAL O_ITEM := NIL
LOCAL N_CONT := 0

// A form to get the name and items of new menu option
LOCAL L_OK := FORM_NEW_ITEM(@C_NAME, @N_ITEMS, O_FORM)

IF L_OK == .T.
    // Create the submenu
    O_SUBMENU := HBNAP_MENU_CREATE()
    FOR N_CONT := 1 TO N_ITEMS
        O_ITEM := HBNAP_MENUITEM_CREATE(C_NAME + "-" + hb_ntos(N_CONT), NIL, {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
        HBNAP_MENU_ADD_ITEM(O_SUBMENU, O_ITEM)
    NEXT

    // Create the new item, attach its submenu and insert the item in first position of main menu
    O_ITEM = HBNAP_MENUITEM_CREATE(C_NAME, NIL, NIL)
    HBNAP_MENUITEM_SUBMENU(O_ITEM, O_SUBMENU)
    HBNAP_MENU_INS_ITEM(O_MENU, 0, O_ITEM)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF HBNAP_MENU_IS_MENUBAR(O_MENU) == .T.
        HBNAP_FORMS_UPDATE(O_FORM)
    ENDIF

ENDIF

******************************************
STAT PROC REMOVE_MENUITEM(O_MENU, O_FORM)
******************************************
LOCAL N_ITEMS := HBNAP_MENU_COUNT(O_MENU)

IF N_ITEMS > 0
    // Remove the first item in main menu
    HBNAP_MENU_DEL_ITEM(O_MENU, 0)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF HBNAP_MENU_IS_MENUBAR(O_MENU) == .T.
        HBNAP_FORMS_UPDATE(O_FORM)
    ENDIF
ENDIF


********************************
PROC TST_FORM_DYNMENU(O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "DynMenus.nfm", DIRET_FORMS(), HBNAP_FORMS_CLOSE_ON_ESC)
LOCAL O_MENU := EXAMPLE_APP_DYN_MENU(O_FORM)
LOCAL N_RES := 0
LOCAL C_MESSAGE := ""

// Window title
HBNAP_FORMS_TITLE(O_FORM, "Exemplo de Menus Dinàmicos")

// Buttons callback
HBNAP_FORMS_ONCLICK(O_FORM, "button_setmenubar", {|| HBNAP_MENU_BAR(O_MENU, O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_unsetmenubar", {|| HBNAP_MENU_BAR(NIL, O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_launchpopup", {|| LAUNCH_POPUP_MENU(O_MENU, O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_insert0", {|| INSERT_NEW_MENUITEM(O_MENU, O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_remove0", {|| REMOVE_MENUITEM(O_MENU, O_FORM) })

// Launch the form
N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_RETURN
    INFO_MESSAGE_BOX("Pressionado [Enter], dados aceitos.", O_PARENT_FORM)
ELSEIF N_RES == 1000
    INFO_MESSAGE_BOX("Botão [OK] pressionado, dados aceitos.", O_PARENT_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_ESC
    INFO_MESSAGE_BOX("ESC pressionado, dados cancelados.", O_PARENT_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
    INFO_MESSAGE_BOX("Formulário fechado com [X], dados cancelados.", O_PARENT_FORM)
ELSE
    INFO_MESSAGE_BOX("Valor de retorno desconhecido.", O_PARENT_FORM)
ENDIF

IF N_RES == HBNAP_CLOSED_BY_RETURN .OR. N_RES == 1000

ENDIF

HBNAP_FORMS_DESTROY(O_FORM)
HBNAP_MENU_DESTROY(O_MENU)

