/* Harbour GTNAP demo */

PROC MAIN

IF HB_GTVERSION()=="NAP"
    NAP_INIT("GTNAP Demo", 35, 110, {|| RUN_MAIN() })
ELSE
    SETMODE(35, 110)
    RUN_MAIN()
ENDIF

RETURN

/*---------------------------------------------------------------------------*/

PROC RUN_MAIN

LOCAL aMenu

IF HB_GTVERSION()=="NAP"
    NAP_TERMINAL()
ENDIF

aMenu := { ;
    { "Text demo", { ;
        { "Sequential Text", {|| SEQUENTIAL_TEXT_DEMO() } }, ;
        { "Located Text",    {|| LOCATED_TEXT_DEMO() } } } }, ;
    { "Exit", {|| EXIT_APP() } } }

MENUBAR_RUN(aMenu)

RETURN

/*---------------------------------------------------------------------------*/

FUNCTION EXIT_APP()

IF HB_GTVERSION()=="NAP"
    NAP_EXIT()
ENDIF

RETURN .T.

/*---------------------------------------------------------------------------*/

FUNCTION SEQUENTIAL_TEXT_DEMO()
    @ 2, 0 SAY "Sequential Text demo (placeholder)"
    RETURN NIL

/*---------------------------------------------------------------------------*/

FUNCTION LOCATED_TEXT_DEMO()
    @ 2, 0 SAY "Located Text demo (placeholder)"
    RETURN NIL
