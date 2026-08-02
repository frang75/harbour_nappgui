/* Generic F10-activated horizontal menu bar with pulldown submenus.

   aMenu is an array of top-level items: { cLabel, xAction }
   - xAction is a code block: runs directly on Enter (no dropdown), e.g. Exit.
     Returning .T. stops MENUBAR_RUN().
   - xAction is an array of { cLabel, bAction } pairs: opens a dropdown box
     on Enter/Down; bAction runs on Enter inside the dropdown.

   Left/Right always move across the top-level bar, even while a dropdown is
   open (the dropdown follows the active item, or closes if it has none).
*/

#INCLUDE "inkey.ch"
#INCLUDE "box.ch"

#DEFINE MENUBAR_ROW            0
#DEFINE MENUBAR_COLOR_BAR      "N/W"
#DEFINE MENUBAR_COLOR_ACTIVE   "W+/N"
#DEFINE MENUBAR_COLOR_BOX      "N/W"
#DEFINE MENUBAR_COLOR_SELECTED "W+/N"
#DEFINE MENUBAR_COLOR_HINT     "N/W"
#DEFINE MENUBAR_HINT           "Press F10 to menu"

#DEFINE DROP_SEL               1
#DEFINE DROP_TOP               2
#DEFINE DROP_BOTTOM            3
#DEFINE DROP_COL               4
#DEFINE DROP_RIGHT             5
#DEFINE DROP_SCREEN            6
#DEFINE DROP_ITEMS             7

/*---------------------------------------------------------------------------*/

FUNCTION MENUBAR_RUN(aMenu)
    LOCAL nKey
    LOCAL nActive := 1
    LOCAL lActive := .F.
    LOCAL lRunning := .T.
    LOCAL aDrop := NIL

    i_menubar_draw(aMenu, nActive, lActive)
    i_hint_draw()

    DO WHILE lRunning
        nKey := INKEY(0)

        IF !lActive
            IF nKey == K_F10
                lActive := .T.
                i_menubar_draw(aMenu, nActive, lActive)
            ENDIF

        ELSE
            DO CASE
            CASE nKey == K_ESC
                IF aDrop != NIL
                    i_dropdown_close(aDrop)
                    aDrop := NIL
                ELSE
                    lActive := .F.
                ENDIF
                i_menubar_draw(aMenu, nActive, lActive)

            CASE nKey == K_LEFT .OR. nKey == K_RIGHT
                IF aDrop != NIL
                    i_dropdown_close(aDrop)
                    aDrop := NIL
                ENDIF
                nActive := i_menubar_neighbour(aMenu, nActive, nKey == K_LEFT)
                i_menubar_draw(aMenu, nActive, lActive)
                aDrop := i_dropdown_open(aMenu, nActive)

            CASE nKey == K_DOWN
                IF aDrop != NIL
                    aDrop[DROP_SEL] := IIF(aDrop[DROP_SEL] == LEN(aDrop[DROP_ITEMS]), 1, aDrop[DROP_SEL] + 1)
                    i_dropdown_draw(aDrop)
                ELSE
                    aDrop := i_dropdown_open(aMenu, nActive)
                ENDIF

            CASE nKey == K_UP
                IF aDrop != NIL
                    aDrop[DROP_SEL] := IIF(aDrop[DROP_SEL] == 1, LEN(aDrop[DROP_ITEMS]), aDrop[DROP_SEL] - 1)
                    i_dropdown_draw(aDrop)
                ENDIF

            CASE nKey == K_ENTER
                IF aDrop != NIL
                    i_dropdown_close(aDrop)
                    lActive := .F.
                    lRunning := i_menubar_launch(aMenu, nActive, lActive, aDrop[DROP_ITEMS][aDrop[DROP_SEL]][2])
                    aDrop := NIL
                ELSEIF HB_ISBLOCK(aMenu[nActive][2])
                    lActive := .F.
                    lRunning := i_menubar_launch(aMenu, nActive, lActive, aMenu[nActive][2])
                ENDIF

            ENDCASE
        ENDIF
    ENDDO

    RETURN NIL

/*---------------------------------------------------------------------------*/
// Clears the screen, redraws the persistent chrome (bar + hint) and runs
// 'bAction'. Returns .T. if MENUBAR_RUN() must keep running.

STATIC FUNCTION i_menubar_launch(aMenu, nActive, lActive, bAction)
    CLS
    i_menubar_draw(aMenu, nActive, lActive)
    i_hint_draw()
    RETURN EVAL(bAction) != .T.

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_menubar_draw(aMenu, nActive, lActive)
    LOCAL nCol := 0
    LOCAL nItem
    LOCAL cText
    LOCAL cColor

    @ MENUBAR_ROW, 0 SAY PADR("", MAXCOL() + 1) COLOR MENUBAR_COLOR_BAR

    FOR nItem := 1 TO LEN(aMenu)
        cText := " " + aMenu[nItem][1] + " "
        cColor := IIF(lActive .AND. nItem == nActive, MENUBAR_COLOR_ACTIVE, MENUBAR_COLOR_BAR)
        @ MENUBAR_ROW, nCol SAY cText COLOR cColor
        nCol += LEN(cText)
    NEXT

    RETURN NIL

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_hint_draw()
    @ MAXROW(), MAXCOL() - LEN(MENUBAR_HINT) + 1 SAY MENUBAR_HINT COLOR MENUBAR_COLOR_HINT
    RETURN NIL

/*---------------------------------------------------------------------------*/
// Screen column where item 'nItem' starts, as laid out by i_menubar_draw().

STATIC FUNCTION i_menubar_col(aMenu, nItem)
    LOCAL nCol := 0
    LOCAL n

    FOR n := 1 TO nItem - 1
        nCol += LEN(aMenu[n][1]) + 2
    NEXT

    RETURN nCol

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_menubar_neighbour(aMenu, nActive, lLeft)
    IF lLeft
        RETURN IIF(nActive == 1, LEN(aMenu), nActive - 1)
    ENDIF
    RETURN IIF(nActive == LEN(aMenu), 1, nActive + 1)

/*---------------------------------------------------------------------------*/
// Opens (saves screen + draws) the dropdown of top-level item 'nActive', if
// it has one. Returns the dropdown state, or NIL when the item has none.

STATIC FUNCTION i_dropdown_open(aMenu, nActive)
    LOCAL aItems := aMenu[nActive][2]
    LOCAL nTop := MENUBAR_ROW + 1
    LOCAL nCol
    LOCAL nBottom
    LOCAL nRight
    LOCAL aDrop

    IF !HB_ISARRAY(aItems)
        RETURN NIL
    ENDIF

    nCol := i_menubar_col(aMenu, nActive)
    nBottom := nTop + LEN(aItems) + 1
    nRight := nCol + i_dropdown_width(aItems) + 1

    aDrop := { 1, nTop, nBottom, nCol, nRight, SAVESCREEN(nTop, nCol, nBottom, nRight), aItems }
    i_dropdown_draw(aDrop)

    RETURN aDrop

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_dropdown_close(aDrop)
    RESTSCREEN(aDrop[DROP_TOP], aDrop[DROP_COL], aDrop[DROP_BOTTOM], aDrop[DROP_RIGHT], aDrop[DROP_SCREEN])
    RETURN NIL

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_dropdown_width(aItems)
    LOCAL nWidth := 0
    LOCAL nItem

    FOR nItem := 1 TO LEN(aItems)
        nWidth := MAX(nWidth, LEN(aItems[nItem][1]))
    NEXT

    RETURN nWidth + 2

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_dropdown_draw(aDrop)
    LOCAL aItems := aDrop[DROP_ITEMS]
    LOCAL nSel := aDrop[DROP_SEL]
    LOCAL nTop := aDrop[DROP_TOP]
    LOCAL nCol := aDrop[DROP_COL]
    LOCAL nWidth := aDrop[DROP_RIGHT] - aDrop[DROP_COL] - 1
    LOCAL nItem
    LOCAL cText
    LOCAL cColor

    @ nTop, nCol, aDrop[DROP_BOTTOM], aDrop[DROP_RIGHT] BOX B_SINGLE COLOR MENUBAR_COLOR_BOX

    FOR nItem := 1 TO LEN(aItems)
        cText := PADR(" " + aItems[nItem][1], nWidth)
        cColor := IIF(nItem == nSel, MENUBAR_COLOR_SELECTED, MENUBAR_COLOR_BOX)
        @ nTop + nItem, nCol + 1 SAY cText COLOR cColor
    NEXT

    RETURN NIL
