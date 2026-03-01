//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

********************************
STAT PROC ITEM_CLICKED(O_FORM, O_ITEM)
********************************
LOCAL C_TEXT := NAP_DMENUITEM_GET_TEXT(O_ITEM)
HBNAP_FORMS_INSERT_TEXT(O_FORM, "textview", "Clicked: " + C_TEXT + hb_eol())

*******************************************
STAT FUNCTION EXAMPLE_APP_DYN_MENU(O_FORM)
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
LOCAL O_FILEITEM1 := NAP_DMENUITEM_CREATE("Open", DIRET_FORMS() + "icons/open.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_FILEITEM2 := NAP_DMENUITEM_CREATE("Save", DIRET_FORMS() + "icons/save.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_FILEITEM3 := NAP_DMENUITEM_CREATE("Exit", DIRET_FORMS() + "icons/exit.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM1 := NAP_DMENUITEM_CREATE("First", DIRET_FORMS() + "icons/first.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM2 := NAP_DMENUITEM_CREATE("Back", DIRET_FORMS() + "icons/back.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM3 := NAP_DMENUITEM_CREATE("Next", DIRET_FORMS() + "icons/next.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_NAVITEM4 := NAP_DMENUITEM_CREATE("Last", DIRET_FORMS() + "icons/last.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM1 := NAP_DMENUITEM_CREATE("Login", DIRET_FORMS() + "icons/login.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM2 := NAP_DMENUITEM_CREATE("Logout", DIRET_FORMS() + "icons/logout.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_SERVITEM3 := NAP_DMENUITEM_CREATE("Settings", DIRET_FORMS() + "icons/settings.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM1 := NAP_DMENUITEM_CREATE("English", DIRET_FORMS() + "icons/usa.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM2 := NAP_DMENUITEM_CREATE("Portuguese", DIRET_FORMS() + "icons/portugal.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM3 := NAP_DMENUITEM_CREATE("Spanish", DIRET_FORMS() + "icons/spain.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM4 := NAP_DMENUITEM_CREATE("Italian", DIRET_FORMS() + "icons/italy.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM5 := NAP_DMENUITEM_CREATE("Japanese", DIRET_FORMS() + "icons/japan.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM6 := NAP_DMENUITEM_CREATE("Russian", DIRET_FORMS() + "icons/russia.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_LANGITEM7 := NAP_DMENUITEM_CREATE("Vietnamese", DIRET_FORMS() + "icons/vietnam.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
LOCAL O_HELPITEM1 := NAP_DMENUITEM_CREATE("About", DIRET_FORMS() + "icons/about.png", {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})

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
STAT PROC LAUNCH_POPUP_MENU(O_MENU, O_FORM)
*******************************************
// Get the button frame in screen coordinates {x, y, width, height}
LOCAL V_FRAME := HBNAP_FORMS_CONTROL_FRAME(O_FORM, "button_launchpopup")

// Unset the menu as menubar rol (if has this role)
NAP_DMENU_BAR(NIL, O_FORM)

// Launch the menu as popup, at the top-left corner of 'button_launchpopup'
NAP_DMENU_POPUP(O_MENU, O_FORM, V_FRAME[1], V_FRAME[2])

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
    O_SUBMENU := NAP_DMENU_CREATE()
    FOR N_CONT := 1 TO N_ITEMS
        O_ITEM := NAP_DMENUITEM_CREATE(C_NAME + "-" + hb_ntos(N_CONT), NIL, {| O_ITEM | ITEM_CLICKED(O_FORM, O_ITEM)})
        NAP_DMENU_ADD_ITEM(O_SUBMENU, O_ITEM)
    NEXT

    // Create the new item, attach its submenu and insert the item in first position of main menu
    O_ITEM = NAP_DMENUITEM_CREATE(C_NAME, NIL, NIL)
    NAP_DMENUITEM_SUBMENU(O_ITEM, O_SUBMENU)
    NAP_DMENU_INS_ITEM(O_MENU, 0, O_ITEM)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF NAP_DMENU_IS_MENUBAR(O_MENU) == .T.
        HBNAP_FORMS_UPDATE(O_FORM)
    ENDIF

ENDIF

******************************************
STAT PROC REMOVE_MENUITEM(O_MENU, O_FORM)
******************************************
LOCAL N_ITEMS := NAP_DMENU_COUNT(O_MENU)

IF N_ITEMS > 0
    // Remove the first item in main menu
    NAP_DMENU_DEL_ITEM(O_MENU, 0)

    // Recompute the window size (in Windows/Linux the menubar is attached to the window)
    IF NAP_DMENU_IS_MENUBAR(O_MENU) == .T.
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
HBNAP_FORMS_ONCLICK(O_FORM, "button_setmenubar", {|| NAP_DMENU_BAR(O_MENU, O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "button_unsetmenubar", {|| NAP_DMENU_BAR(NIL, O_FORM) })
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
NAP_DMENU_DESTROY(O_MENU)

