// HBAWS Interface
#INCLUDE "cua.ch"
#INCLUDE "inkey.ch"
#include "hbaws.ch"

****************************
PROC EXEMPLO_HBAWS()
****************************
LOCAL V_Janela
*
CUA20 @ 10,20,26,80 JANELA V_Janela ;
     TITULO "Escolha o exemplo HBAWS" SUBTITULO "%T";
     AJUDA "T?????"

ESPECIALIZE V_Janela MENU

ADDOPCAO V_Janela TEXTO "#HBAWS Listall" ;
    ACAO EXEMPLO_LISTALL() AJUDA "P06683"

ATIVE(V_Janela)


****************************
PROC EXEMPLO_LISTALL()
****************************
LOCAL V_OBJS := {}
LOCAL V_ITEM := {}
LOCAL C_TEXT := ""
LOCAL C_ERR := ""
LOCAL C_PREFIX := ""
LOCAL N_CONT := 0
LOCAL V_Janela, L_OK

INIT_HBAWS()
V_OBJS := HBAWS_S3_LIST_ALL(@C_ERR, Exemplo_AWS_Bucket(), C_PREFIX)

IF Len(V_OBJS) != 0
    C_TEXT += "Num Files found: " + hb_ntos(LEN(V_OBJS)) + hb_eol()
    FOR N_Cont := 1 TO LEN(V_OBJS)
        V_Item := V_OBJS[N_Cont]
        C_TEXT += "ITEM: " + hb_ntos(N_Cont) + hb_eol()
        C_TEXT += " * S3Key: " + V_Item[OBJ_S3KEY] + hb_eol()
        // ? " * ContentSize: " + hb_ntos(V_Item[OBJ_CONTENT_SIZE])
        // ? " * ContentType: " + V_Item[OBJ_CONTENT_TYPE]
        // ? " * Date: " + DToC(V_Item[OBJ_DATE])
        // ? " * Time: " + V_Item[OBJ_TIME]
        // ? " * TimeZone: " + V_Item[OBJ_TIMEZONE]
        // ? " * StorageClass: " + V_Item[OBJ_STORAGE_CLASS]
        // ? " * IsRestore: " + hb_ValToStr(V_Item[OBJ_IS_RESTORE])
        // ? " * RestoreDate: " + DToC(V_Item[OBJ_RESTORE_DATE])
        // ? " * RestoreTime: " + V_Item[OBJ_RESTORE_TIME]
        // ? " * RestoreTimeZone: " + V_Item[OBJ_RESTORE_TIMEZONE]
        // ? " * ChecksumAlgorithm: " + V_Item[OBJ_CHECKSUM_ALGORITHM]
        // ? " * ETag: " + V_Item[OBJ_ETAG]
    NEXT
ELSE
    IF LEN(C_ERR)==0
        C_TEXT := "No files found"
    ELSE
        C_TEXT := "Error: " + C_ERR
    ENDIF
ENDIF

@ 5,15,MAXROW()-4,MAXCOL()-10 JANELA V_Janela ;
  TITU "Exibe texto em memória" SUBTITULO "%T;Read-only" ;
  TECLAS {"F2=fecha texto"} ;
  AJUDA "T?????"

ESPECIALIZE V_Janela TEXTO C_TEXT TERMINAR {K_F2} EDITA .F.

ATIVE(V_Janela)
DESTRUA V_Janela


***********************
FUNCTION INIT_HBAWS()
***********************
STATIC L_INIT_AWS := .F.
LOCAL C_ERR := NIL

IF L_INIT_AWS == .F.
    L_INIT_AWS := HBAWS_INIT(@C_ERR, Exemplo_AWS_AccessKey(), Exemplo_AWS_Secret())
ENDIF

RETURN L_INIT_AWS
