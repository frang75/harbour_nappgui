//
// Important!!
// HBNAP uses UTF-8. Save file in UTF8 format
//

#INCLUDE "hbnap.ch"

// Shared state between WORKER_CARGA_BASES() (background thread) and
// ATUALIZAR_PROGRESSO_CARGA() (main thread tick, runs every 50ms).
// Only the main thread ever touches HBNAP_FORMS_* - the worker thread only
// writes these STATICs, always under H_MUTEX_CARGA.
STATIC H_MUTEX_CARGA
STATIC N_TASK_ATUAL
STATIC N_SUBFRACAO
STATIC L_CARGA_FIM
STATIC L_CARGA_CANCELADA // Set by I_CANCELAR_CARGA() when the user closes the dialog with [X]

********************************
STAT FUNC VALIDAR_EMPRESA(O_EMPRESA)
********************************
RETURN .T.

********************************
STAT PROC WORKER_CARGA_BASES()
********************************
// Runs on its own thread (hb_threadStart). Simulates a heavy, CPU-bound
// load process (e.g. a real database update) that must not freeze the
// GUI thread - 15 subtasks of 1 second each, in 20 steps of 50ms, so the
// secondary progress bar moves smoothly. Never touches the form - only
// the protected STATICs above.
LOCAL N_TASK, N_STEP
LOCAL N_PASSOS := 20
LOCAL L_CANCEL := .F.

FOR N_TASK := 1 TO 15
    FOR N_STEP := 1 TO N_PASSOS
        hb_idleSleep(0.050)
        hb_mutexLock(H_MUTEX_CARGA)
        L_CANCEL := L_CARGA_CANCELADA
        IF L_CANCEL == .F.
            N_TASK_ATUAL := N_TASK
            N_SUBFRACAO := N_STEP / N_PASSOS
        ENDIF
        hb_mutexUnLock(H_MUTEX_CARGA)
        IF L_CANCEL == .T.
            EXIT // Breaks the N_STEP loop
        ENDIF
    NEXT
    IF L_CANCEL == .T.
        EXIT // Breaks the N_TASK loop
    ENDIF
NEXT

IF L_CANCEL == .F.
    hb_mutexLock(H_MUTEX_CARGA)
    L_CARGA_FIM := .T.
    hb_mutexUnLock(H_MUTEX_CARGA)
ENDIF

RETURN

********************************
STAT FUNC ATUALIZAR_PROGRESSO_CARGA(O_FORM, C_TAREFAS)
********************************
// HBNAP_FORMS_TICK callback (main thread, every 50ms). Reads the shared
// state under the mutex, then pushes it into the form widgets.
LOCAL N_TASK, N_SUBFRAC, L_FIM, N_GERAL

hb_mutexLock(H_MUTEX_CARGA)
N_TASK := N_TASK_ATUAL
N_SUBFRAC := N_SUBFRACAO
L_FIM := L_CARGA_FIM
hb_mutexUnLock(H_MUTEX_CARGA)

N_GERAL := ((N_TASK - 1) + N_SUBFRAC) / LEN(C_TAREFAS)
HBNAP_FORMS_SET_TEXT(O_FORM, "progress_text1", C_TAREFAS[N_TASK])
HBNAP_FORMS_SET_TEXT(O_FORM, "perc_text1", hb_ntos(INT(N_SUBFRAC * 100)) + "%")
HBNAP_FORMS_SET_REAL(O_FORM, "progress1", N_SUBFRAC)
HBNAP_FORMS_SET_TEXT(O_FORM, "progress_text2", "Atualização geral (" + hb_ntos(N_TASK) + "/" + hb_ntos(LEN(C_TAREFAS)) + ")")
HBNAP_FORMS_SET_TEXT(O_FORM, "perc_text2", hb_ntos(INT(N_GERAL * 100)) + "%")
HBNAP_FORMS_SET_REAL(O_FORM, "progress2", N_GERAL)

IF L_FIM == .T.
    HBNAP_FORMS_STOP_MODAL(O_FORM, 0)
    RETURN .F. // Stop ticking
ENDIF

RETURN .T. // Keep ticking

********************************
STAT FUNC I_CANCELAR_CARGA(N_TICK_ID)
********************************
// Close callback, passed as the 3rd arg to HBNAP_FORMS_MODAL(): user
// pressed [X] on the CopyFiles dialog. Signals the worker thread to stop
// (cooperative - no forced kill) and unregisters the tick explicitly.
// The actual hb_threadJoin() happens right after HBNAP_FORMS_MODAL()
// returns, in SIMULAR_ATUALIZACAO_BASES().
hb_mutexLock(H_MUTEX_CARGA)
L_CARGA_CANCELADA := .T.
hb_mutexUnLock(H_MUTEX_CARGA)

HBNAP_FORMS_TICK_STOP(N_TICK_ID)

RETURN .T. // Allow the dialog to actually close

********************************
STAT PROC SIMULAR_ATUALIZACAO_BASES(O_PARENT_FORM)
********************************
// Shows CopyFiles.nfm and drives it for ~15s with a real background
// thread (hb_threadStart) doing the (simulated) work, while the main
// thread's HBNAP_FORMS_TICK() reads the shared progress every 50ms and
// updates the two progress bars/labels. If the user closes it early with
// [X], the load is cancelled, an interruption notice is shown, and the
// Empresas dialog is shown anyway - same as a normal completion.
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "CopyFiles.nfm", DIRET_FORMS(), 0)
LOCAL H_THREAD
LOCAL N_TICK_ID
LOCAL N_RES
LOCAL C_TAREFAS := { ;
    "Inicializando conexão com o banco de dados", ;
    "Carregando tabela de Empresas", ;
    "Carregando tabela de Cidades", ;
    "Carregando tabela de Entidades Gestoras", ;
    "Verificando índices", ;
    "Carregando módulo de Orçamento", ;
    "Carregando módulo Contábil (SIAFIC)", ;
    "Carregando módulo de Licitação", ;
    "Carregando módulo de PPA", ;
    "Carregando módulo Patrimonial", ;
    "Carregando módulo de Almoxarifado", ;
    "Carregando módulo de Frota", ;
    "Carregando módulo de Doações", ;
    "Verificando integridade dos dados", ;
    "Finalizando inicialização" ;
}

H_MUTEX_CARGA := hb_mutexCreate()
N_TASK_ATUAL := 1
N_SUBFRACAO := 0
L_CARGA_FIM := .F.
L_CARGA_CANCELADA := .F.

HBNAP_FORMS_SET_TEXT(O_FORM, "main_text", ;
    "Atualizando as bases de dados do sistema. Este processo pode levar " + ;
    "alguns instantes. Por favor, aguarde até a conclusão antes de continuar.")
HBNAP_FORMS_SET_TEXT(O_FORM, "info_text", "Processo: DB-SYNC")

H_THREAD := hb_threadStart(@WORKER_CARGA_BASES())
N_TICK_ID := HBNAP_FORMS_TICK(O_FORM, 0.050, {|| ATUALIZAR_PROGRESSO_CARGA(O_FORM, C_TAREFAS) })

N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM, {|| I_CANCELAR_CARGA(N_TICK_ID) })

hb_threadJoin(H_THREAD)

IF N_RES == HBNAP_CLOSED_BY_BUTTON
    INFO_MESSAGE_BOX("O processo de atualização das bases de dados foi interrompido.", O_PARENT_FORM)
ENDIF

HBNAP_FORMS_DESTROY(O_FORM)

RETURN

********************************
STAT PROC SALVAR_EMPRESA(O_FORM, O_EMPRESA)
********************************
LOCAL C_MESSAGE

// Recovery data from form to O_EMPRESA vector
HBNAP_FORMS_BIND_STORE(O_FORM)

IF VALIDAR_EMPRESA(O_EMPRESA) == .T.

    IF RLOCK()
        empresas->uf := O_EMPRESA[1]
        empresas->codcid := O_EMPRESA[2]
        empresas->cidade := O_EMPRESA[3]
        empresas->codent := O_EMPRESA[4]
        empresas->gestora := O_EMPRESA[5]
        UNLOCK
    ENDIF

    C_MESSAGE := "C_UF: " + O_EMPRESA[1] + HB_EOL() + ;
                 "C_CODCID: " + O_EMPRESA[2] + HB_EOL() + ;
                 "C_CIDADE: " + O_EMPRESA[3] + HB_EOL() + ;
                 "C_CODENT: " + O_EMPRESA[4] + HB_EOL() + ;
                 "C_GESTORA: " + O_EMPRESA[5]

    INFO_MESSAGE_BOX(C_MESSAGE, O_FORM)

ENDIF

********************************
STAT PROC FORM_EMPRESA_DETAIL(C_FORM_TITLE, O_PARENT_FORM)
********************************
LOCAL O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_detail.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))
LOCAL O_EMPRESA := { empresas->uf, empresas->codcid, empresas->cidade, empresas->codent, empresas->gestora }
LOCAL V_BIND := { ;
                    {"uf_combo", @O_EMPRESA[1] }, ;
                    {"codcid_edit", @O_EMPRESA[2] }, ;
                    {"cidade_combo", @O_EMPRESA[3] }, ;
                    {"codent_edit", @O_EMPRESA[4] }, ;
                    {"gestora_edit", @O_EMPRESA[5] } ;
                }
LOCAL C_UF := { ;
                    "AC", "AL", "AP", "AM", "BA", "CE", "DF", "ES", "GO", "MA", "MT", "MS", "MG", ;
                    "PA", "PB", "PR", "PE", "PI", "RJ", "RN", "RS", "RO", "RR", "SC", "SP", "SE", "TO" ;
              }

LOCAL C_CIDADE := { ;
                    "São Paulo", ;
                    "Rio de Janeiro", ;
                    "Brasília", ;
                    "Salvador", ;
                    "Fortaleza", ;
                    "Belo Horizonte", ;
                    "Manaus", ;
                    "Curitiba", ;
                    "Recife", ;
                    "Porto Alegre", ;
                    "Goiânia", ;
                    "Belém", ;
                    "Guarulhos", ;
                    "Campinas", ;
                    "São Luís" ;
              }

LOCAL N_RES := 0

HBNAP_FORMS_TITLE(O_FORM, C_FORM_TITLE)
HBNAP_FORMS_BIND(O_FORM, V_BIND)
HBNAP_FORMS_ITEM_LIST(O_FORM, "uf_combo", C_UF)
HBNAP_FORMS_ITEM_LIST(O_FORM, "cidade_combo", C_CIDADE)
HBNAP_FORMS_ONCLICK(O_FORM, "salvar_button", {|| SALVAR_EMPRESA(O_FORM, O_EMPRESA) })

N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_ESC
ELSEIF N_RES == HBNAP_CLOSED_BY_RETURN
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
ELSE
ENDIF

HBNAP_FORMS_DESTROY(O_FORM)

********************************
STAT PROC ADD_EMPRESA(O_PARENT_FORM)
********************************
APPEND BLANK
FORM_EMPRESA_DETAIL("Inclusão de empresa", O_PARENT_FORM)
HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)

********************************
STAT PROC EDIT_EMPRESA(O_PARENT_FORM)
********************************
LOCAL SRECNO := HBNAP_FORMS_AREA_RECNO(O_PARENT_FORM)
IF SRECNO != -1
    GO SRECNO
    FORM_EMPRESA_DETAIL("Alterar de empresa", O_PARENT_FORM)
    HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)
ENDIF

********************************
STAT PROC REMOVE_EMPRESA(O_PARENT_FORM)
********************************
LOCAL SRECNO := HBNAP_FORMS_AREA_RECNO(O_PARENT_FORM)
IF SRECNO != -1
    GO SRECNO
    IF RLOCK()
        DELETE
        UNLOCK
    ENDIF
    HBNAP_FORMS_AREA_REFRESH(O_PARENT_FORM)
ENDIF

********************************
PROC TST_FORM_EMPRESAS(O_PARENT_FORM)
********************************
LOCAL O_FORM
LOCAL N_RES := 0
LOCAL C_MESSAGE := ""
LOCAL V_COLS := { ; // table columns
    { HBNAP_LEFT, {|| empresas->uf} }, ;
    { HBNAP_LEFT, {|| empresas->codcid} }, ;
    { HBNAP_LEFT, {|| empresas->cidade} }, ;
    { HBNAP_LEFT, {|| empresas->gestora} }, ;
    { HBNAP_LEFT, {|| "Principal"} } ;
}

// Simulate the database load/update, like the client's production apps do
SIMULAR_ATUALIZACAO_BASES(O_PARENT_FORM)

O_FORM := HBNAP_FORMS_LOAD(DIRET_FORMS() + "Company_list.nfm", DIRET_FORMS(), hb_bitOr(HBNAP_FORMS_RESIZABLE, HBNAP_FORMS_CLOSE_ON_ESC, HBNAP_FORMS_CLOSE_ON_RETURN))

USE ../dados/empresas NEW SHARED
GOTO TOP

HBNAP_FORMS_TITLE(O_FORM, "Empresas")
HBNAP_FORMS_TABLE_BIND(O_FORM, "table", "EMPRESAS", V_COLS)
HBNAP_FORMS_ONCLICK(O_FORM, "add_button", {|| ADD_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "add_tool", {|| ADD_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "edit_button", {|| EDIT_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "edit_tool", {|| EDIT_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "remove_button", {|| REMOVE_EMPRESA(O_FORM) })
HBNAP_FORMS_ONCLICK(O_FORM, "remove_tool", {|| REMOVE_EMPRESA(O_FORM) })

// Launch the form
N_RES := HBNAP_FORMS_MODAL(O_FORM, O_PARENT_FORM)

IF N_RES == HBNAP_CLOSED_BY_ESC
    INFO_MESSAGE_BOX("Form closed by pressing [ESC] key", O_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_RETURN
    INFO_MESSAGE_BOX("Form closed by pressing [RETURN] key", O_FORM)
ELSEIF N_RES == HBNAP_CLOSED_BY_BUTTON
    INFO_MESSAGE_BOX("Form closed by pressing [X] button", O_FORM)
ELSE
    INFO_MESSAGE_BOX("Form closed by user func with code: " + hb_ntos(N_RES), O_FORM)
ENDIF

HBNAP_FORMS_DESTROY(O_FORM)

CLOSE EMPRESAS
