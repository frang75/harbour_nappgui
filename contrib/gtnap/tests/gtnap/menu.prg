/* Generic F10-activated horizontal menu bar with pulldown submenus.

   aMenu is an array of top-level items: { cLabel, xAction }
   - xAction is a code block: runs directly on Enter (no dropdown), e.g. Exit.
     Returning .T. stops MENUBAR_RUN().
   - xAction is an array of { cLabel, bAction } pairs: opens a dropdown box
     on Enter/Down; bAction runs on Enter inside the dropdown.
*/

#INCLUDE "inkey.ch"
#INCLUDE "box.ch"

#DEFINE MENUBAR_ROW            0
#DEFINE MENUBAR_COLOR_BAR      "N/W"
#DEFINE MENUBAR_COLOR_ACTIVE   "W+/N"
#DEFINE MENUBAR_COLOR_BOX      "N/W"
#DEFINE MENUBAR_COLOR_SELECTED "W+/N"

/*---------------------------------------------------------------------------*/

FUNCTION MENUBAR_RUN(aMenu)
    LOCAL nKey
    LOCAL nActive := 1
    LOCAL lActive := .F.
    LOCAL lRunning := .T.

    i_menubar_draw(aMenu, nActive, lActive)

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
                lActive := .F.
                i_menubar_draw(aMenu, nActive, lActive)

            CASE nKey == K_LEFT
                nActive := IIF(nActive == 1, LEN(aMenu), nActive - 1)
                i_menubar_draw(aMenu, nActive, lActive)

            CASE nKey == K_RIGHT
                nActive := IIF(nActive == LEN(aMenu), 1, nActive + 1)
                i_menubar_draw(aMenu, nActive, lActive)

            CASE nKey == K_DOWN .OR. nKey == K_ENTER
                lRunning := i_menubar_activate(aMenu, nActive) != .T.
                lActive := .F.
                i_menubar_draw(aMenu, nActive, lActive)

            ENDCASE
        ENDIF
    ENDDO

    RETURN NIL

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
// Screen column where item 'nItem' starts, as laid out by i_menubar_draw().

STATIC FUNCTION i_menubar_col(aMenu, nItem)
    LOCAL nCol := 0
    LOCAL n

    FOR n := 1 TO nItem - 1
        nCol += LEN(aMenu[n][1]) + 2
    NEXT

    RETURN nCol

/*---------------------------------------------------------------------------*/
// Enter/Down on top-level item 'nActive'. Returns .T. when the app must exit.

STATIC FUNCTION i_menubar_activate(aMenu, nActive)
    LOCAL xAction := aMenu[nActive][2]

    IF HB_ISBLOCK(xAction)
        RETURN EVAL(xAction)
    ENDIF

    RETURN i_dropdown_run(xAction, i_menubar_col(aMenu, nActive))

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_dropdown_run(aItems, nCol)
    LOCAL nWidth := 0
    LOCAL nItem
    LOCAL nSel := 1
    LOCAL nKey
    LOCAL lStop := .F.
    LOCAL lDone := .F.
    LOCAL nTop := MENUBAR_ROW + 1
    LOCAL nBottom
    LOCAL nRight
    LOCAL cScreen

    FOR nItem := 1 TO LEN(aItems)
        nWidth := MAX(nWidth, LEN(aItems[nItem][1]))
    NEXT
    nWidth += 2
    nBottom := nTop + LEN(aItems) + 1
    nRight := nCol + nWidth + 1

    cScreen := SAVESCREEN(nTop, nCol, nBottom, nRight)
    i_dropdown_draw(aItems, nCol, nTop, nWidth, nSel)

    DO WHILE !lDone
        nKey := INKEY(0)

        DO CASE
        CASE nKey == K_ESC
            RESTSCREEN(nTop, nCol, nBottom, nRight, cScreen)
            lDone := .T.

        CASE nKey == K_UP
            nSel := IIF(nSel == 1, LEN(aItems), nSel - 1)
            i_dropdown_draw(aItems, nCol, nTop, nWidth, nSel)

        CASE nKey == K_DOWN
            nSel := IIF(nSel == LEN(aItems), 1, nSel + 1)
            i_dropdown_draw(aItems, nCol, nTop, nWidth, nSel)

        CASE nKey == K_ENTER
            RESTSCREEN(nTop, nCol, nBottom, nRight, cScreen)
            lStop := EVAL(aItems[nSel][2]) == .T.
            lDone := .T.

        ENDCASE
    ENDDO

    RETURN lStop

/*---------------------------------------------------------------------------*/

STATIC FUNCTION i_dropdown_draw(aItems, nCol, nTop, nWidth, nSel)
    LOCAL nItem
    LOCAL cText
    LOCAL cColor
    LOCAL nBottom := nTop + LEN(aItems) + 1
    LOCAL nRight := nCol + nWidth + 1

    @ nTop, nCol, nBottom, nRight BOX B_SINGLE COLOR MENUBAR_COLOR_BOX

    FOR nItem := 1 TO LEN(aItems)
        cText := PADR(" " + aItems[nItem][1], nWidth)
        cColor := IIF(nItem == nSel, MENUBAR_COLOR_SELECTED, MENUBAR_COLOR_BOX)
        @ nTop + nItem, nCol + 1 SAY cText COLOR cColor
    NEXT

    RETURN NIL
