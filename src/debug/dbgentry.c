/*
 * Debugger entry routine
 *
 * Copyright 2005 Phil Krylov <phil a t newstar.rinet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file LICENSE.txt.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA (or visit https://www.gnu.org/licenses/).
 *
 * As a special exception, the Harbour Project gives permission for
 * additional uses of the text contained in its release of Harbour.
 *
 * The exception is that, if you link the Harbour libraries with other
 * files to produce an executable, this does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * Your use of that executable is in no way restricted on account of
 * linking the Harbour library code into it.
 *
 * This exception does not however invalidate any other reasons why
 * the executable file might be covered by the GNU General Public License.
 *
 * This exception applies only to the code released by the Harbour
 * Project under the name Harbour.  If you copy code from other
 * Harbour Project or Free Software Foundation releases into a copy of
 * Harbour, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for Harbour, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 *
 */

#include "hbapidbg.h"
#include "hbapiitm.h"
#include "hbapicls.h"
#include "hbapirdd.h"
#include "hbstack.h"
#include "hbvm.h"
#include "hbthread.h"

#include "hbdebug.ch"
#include "hbmacro.ch"
#include "set.ch"

/* dummy function declaration */
static HB_BOOL hb_clsSetScope( HB_BOOL fScope )
{
   return fScope;
}

#define HB_DBGINFO_DISABLE  ( ( HB_DEBUGINFO * ) ( HB_PTRUINT ) 0x01 )

#if defined( HB_OS_UNIX )
#define FILENAME_EQUAL( s1, s2 )  ( ! strcmp( ( s1 ), ( s2 ) ) )
#else
#define FILENAME_EQUAL( s1, s2 )  ( ! hb_stricmp( ( s1 ), ( s2 ) ) )
#endif

#define ARRAY_ADD( type, array, length ) \
   ( ( ++length == 1 ) ? ( array = ( type * ) hb_xgrab( sizeof( type ) ) ) : \
     ( ( array = ( type * ) hb_xrealloc( array, sizeof( type ) * length ) ) + \
       length - 1 ) )

#define ARRAY_DEL( type, array, length, index ) \
   do { \
      if( ! --length ) \
         hb_xfree( array ); \
      else if( index < length ) \
         memmove( array + index, array + index + 1, sizeof( type ) * ( length - index ) ); \
   } while( 0 )

#define HB_DBGCOMMON_LOCK()       hb_threadEnterCriticalSectionGC( &s_dbgMtx )
#define HB_DBGCOMMON_UNLOCK()     hb_threadLeaveCriticalSection( &s_dbgMtx )
static HB_CRITICAL_NEW( s_dbgMtx );

typedef struct
{
   char * szModule;
   int    nLine;
   char * szFunction;
} HB_BREAKPOINT;

typedef struct
{
   int      nIndex;
   PHB_ITEM xValue;
} HB_TRACEPOINT;

typedef struct
{
   const char * szName;
   char cType;
   union
   {
      int      num;
      PHB_ITEM ptr;
   } frame;
   int nIndex;
} HB_VARINFO;

typedef struct
{
   char *       szExpr;
   PHB_ITEM     pBlock;
   int          nVars;
   char **      aVars;
   HB_VARINFO * aScopes;
} HB_WATCHPOINT;

typedef struct
{
   char *       szModule;
   char *       szFunction;
   int          nLine;
   int          nProcLevel;
   int          nLocals;
   HB_VARINFO * aLocals;
   int          nStatics;
   HB_VARINFO * aStatics;
} HB_CALLSTACKINFO;

typedef struct
{
   char *       szModule;
   int          nStatics;
   HB_VARINFO * aStatics;
   int          nGlobals;
   HB_VARINFO * aGlobals;
   int          nExternGlobals;
   HB_VARINFO * aExternGlobals;
} HB_MODULEINFO;

typedef struct
{
   int nModules;
   HB_MODULEINFO * aModules;
   PHB_ITEM        pStopLines;
} HB_DBGCOMMONINFO;

typedef struct
{
   HB_BOOL bQuit;
   HB_BOOL bGo;
   HB_BOOL bInside;
   int     nBreakPoints;
   HB_BREAKPOINT * aBreak;
   int nTracePoints;
   HB_TRACEPOINT * aTrace;
   int nWatchPoints;
   HB_WATCHPOINT * aWatch;
   HB_BOOL         bTraceOver;
   int     nTraceLevel;
   HB_BOOL bNextRoutine;
   HB_BOOL bCodeBlock;
   HB_BOOL bToCursor;
   int     nToCursorLine;
   char *  szToCursorModule;
   int     nProcLevel;
   int     nCallStackLen;
   HB_CALLSTACKINFO * aCallStack;
   HB_BOOL bCBTrace;
   HB_BOOL ( * pFunInvoke )( void );
   HB_BOOL bInitGlobals;
   HB_BOOL bInitStatics;
   HB_BOOL bInitLines;
   PHB_DYNS pDbgEntry;
} HB_DEBUGINFO;

static HB_DBGCOMMONINFO s_common = { 0, NULL, NULL };

static void     hb_dbgAddLocal( HB_DEBUGINFO * info, const char * szName, int nIndex, int nFrame );
static void     hb_dbgAddModule( const char * szName );
static void     hb_dbgAddStack( HB_DEBUGINFO * info, const char * szName, int nLine, int nProcLevel );
static void     hb_dbgAddStatic( HB_DEBUGINFO * info, const char * szName, int nIndex, PHB_ITEM pFrame );
static void     hb_dbgAddVar( int * nVars, HB_VARINFO ** aVars, const char * szName, char cType, int nIndex, int nFrame, PHB_ITEM pFrame );
static void     hb_dbgAddStopLines( PHB_ITEM pItem );
static void     hb_dbgEndProc( HB_DEBUGINFO * info );
static PHB_ITEM hb_dbgEval( HB_DEBUGINFO * info, HB_WATCHPOINT * watch, HB_BOOL * valid );
static PHB_ITEM hb_dbgEvalMakeBlock( HB_WATCHPOINT * watch );
static PHB_ITEM hb_dbgEvalResolve( HB_DEBUGINFO * info, HB_WATCHPOINT * watch );
static HB_BOOL  hb_dbgIsAltD( void );
static int      hb_dbgIsBreakPoint( HB_DEBUGINFO * info, const char * szModule, int nLine );
static HB_BOOL  hb_dbgEqual( PHB_ITEM pItem1, PHB_ITEM pItem2 );
static void     hb_dbgQuit( HB_DEBUGINFO * info );
static void     hb_dbgRelease( void );
static PHB_ITEM hb_dbgVarGet( HB_VARINFO * scope );
static void     hb_dbgVarSet( HB_VARINFO * scope, PHB_ITEM xNewValue );

static const char * hb_dbgSetName( HB_set_enum setId )
{
   switch( setId )
   {
      case HB_SET_EXACT:         return "Exact";
      case HB_SET_FIXED:         return "Fixed";
      case HB_SET_DECIMALS:      return "Decimals";
      case HB_SET_DATEFORMAT:    return "DateFormat";
      case HB_SET_EPOCH:         return "Epoch";
      case HB_SET_PATH:          return "Path";
      case HB_SET_DEFAULT:       return "Default";

      case HB_SET_EXCLUSIVE:     return "Exclusive";
      case HB_SET_SOFTSEEK:      return "SoftSeek";
      case HB_SET_UNIQUE:        return "Unique";
      case HB_SET_DELETED:       return "Deleted";

      case HB_SET_CANCEL:        return "Cancel";
      case HB_SET_DEBUG:         return "Debug";
      case HB_SET_TYPEAHEAD:     return "TypeAhead";

      case HB_SET_COLOR:         return "Color";
      case HB_SET_CURSOR:        return "Cursor";
      case HB_SET_CONSOLE:       return "Console";
      case HB_SET_ALTERNATE:     return "Alternate";
      case HB_SET_ALTFILE:       return "AltFile";
      case HB_SET_DEVICE:        return "Device";
      case HB_SET_EXTRA:         return "Extra";
      case HB_SET_EXTRAFILE:     return "ExtraFile";
      case HB_SET_PRINTER:       return "Printer";
      case HB_SET_PRINTFILE:     return "PrintFile";
      case HB_SET_MARGIN:        return "Margin";

      case HB_SET_BELL:          return "Bell";
      case HB_SET_CONFIRM:       return "Confirm";
      case HB_SET_ESCAPE:        return "Escape";
      case HB_SET_INSERT:        return "Insert";
      case HB_SET_EXIT:          return "Exit";
      case HB_SET_INTENSITY:     return "Intensity";
      case HB_SET_SCOREBOARD:    return "ScoreBoard";
      case HB_SET_DELIMITERS:    return "Delimeters";
      case HB_SET_DELIMCHARS:    return "DelimChars";

      case HB_SET_WRAP:          return "Wrap";
      case HB_SET_MESSAGE:       return "Message";
      case HB_SET_MCENTER:       return "MCenter";
      case HB_SET_SCROLLBREAK:   return "ScrollBreak";

      case HB_SET_EVENTMASK:     return "EventMask";

      case HB_SET_VIDEOMODE:     return "VideoMode";

      case HB_SET_MBLOCKSIZE:    return "MBlockSize";
      case HB_SET_MFILEEXT:      return "MFileExt";

      case HB_SET_STRICTREAD:    return "StrictRead";
      case HB_SET_OPTIMIZE:      return "Optimize";
      case HB_SET_AUTOPEN:       return "Autopen";
      case HB_SET_AUTORDER:      return "Autorder";
      case HB_SET_AUTOSHARE:     return "AutoShare";

      /* Harbour SET extensions */
      case HB_SET_LANGUAGE:      return "Language";
      case HB_SET_IDLEREPEAT:    return "IdleRepeat";
      case HB_SET_FILECASE:      return "FileCase";
      case HB_SET_DIRCASE:       return "DirCase";
      case HB_SET_DIRSEPARATOR:  return "DirSeparator";
      case HB_SET_EOF:           return "EOF";
      case HB_SET_HARDCOMMIT:    return "HardCommit";
      case HB_SET_FORCEOPT:      return "ForceOpt";
      case HB_SET_DBFLOCKSCHEME: return "DBFLockScheme";
      case HB_SET_DEFEXTENSIONS: return "DefExtensions";
      case HB_SET_EOL:           return "EOL";
      case HB_SET_TRIMFILENAME:  return "TrimFileName";
      case HB_SET_HBOUTLOG:      return "OutLogFile";
      case HB_SET_HBOUTLOGINFO:  return "OutLogInfo";
      case HB_SET_CODEPAGE:      return "CodePage";
      case HB_SET_OSCODEPAGE:    return "OSCodePage";
      case HB_SET_TIMEFORMAT:    return "TimeFormat";
      case HB_SET_DBCODEPAGE:    return "DBCodePage";

      case HB_SET_INVALID_:
         break;
   }

   return NULL;
}

static PHB_ITEM hb_dbgSetArray( void )
{
   PHB_ITEM pArray;
   int iSet, iPos;

   pArray = hb_itemArrayNew( _SET_COUNT + HB_SET_COUNT );
   iPos = iSet = 1;
   while( iPos <= _SET_COUNT + HB_SET_COUNT )
   {
      const char * szName = hb_dbgSetName( ( HB_set_enum ) iSet );
      PHB_ITEM pSet = hb_arrayGetItemPtr( pArray, iPos++ );

      hb_arrayNew( pSet, HB_DBG_SET_LEN );
      hb_arraySetNI( pSet, HB_DBG_SET_POS, iSet );
      hb_arraySetC( pSet, HB_DBG_SET_NAME, szName );
      hb_setGetItem( ( HB_set_enum ) iSet,
                      hb_arrayGetItemPtr( pSet, HB_DBG_SET_VALUE ),
                      NULL, NULL );
      if( iSet == _SET_COUNT )
         iSet = HB_SET_BASE;
      else
         iSet++;
   }

   return pArray;
}


static PHB_ITEM hb_dbgActivateBreakArray( HB_DEBUGINFO * info )
{
   int i;
   PHB_ITEM pArray = hb_itemArrayNew( info->nBreakPoints );

   for( i = 0; i < info->nBreakPoints; i++ )
   {
      PHB_ITEM pBreak = hb_arrayGetItemPtr( pArray, i + 1 );

      hb_arrayNew( pBreak, HB_DBG_BP_LEN );
      if( ! info->aBreak[ i ].szFunction )
      {
         hb_arraySetNI( pBreak, HB_DBG_BP_LINE, info->aBreak[ i ].nLine );
         hb_arraySetC( pBreak, HB_DBG_BP_MODULE, info->aBreak[ i ].szModule );
      }
      else
         hb_arraySetC( pBreak, HB_DBG_BP_FUNC, info->aBreak[ i ].szFunction );
   }
   return pArray;
}


static PHB_ITEM hb_dbgActivateWatchArray( HB_DEBUGINFO * info )
{
   int i, j;
   PHB_ITEM pArray = hb_itemArrayNew( info->nWatchPoints );

   for( i = 0; i < info->nWatchPoints; i++ )
   {
      PHB_ITEM pWatch = hb_arrayGetItemPtr( pArray, i + 1 ), xValue;
      HB_BOOL fValid;

      for( j = 0; j < info->nTracePoints; j++ )
      {
         if( info->aTrace[ j ].nIndex == i )
            break;
      }
      xValue = hb_dbgEval( info, &info->aWatch[ i ], &fValid );
      hb_arrayNew( pWatch, HB_DBG_WP_LEN );
      hb_arraySetC( pWatch, HB_DBG_WP_EXPR, info->aWatch[ i ].szExpr );
      hb_arraySetL( pWatch, HB_DBG_WP_ISTRACE, j < info->nTracePoints );
      hb_arraySetL( pWatch, HB_DBG_WP_VALID, fValid );
      if( xValue )
      {
         hb_arraySetForward( pWatch, HB_DBG_WP_RESULT, xValue );
         hb_itemRelease( xValue );
      }
   }
   return pArray;
}


static PHB_ITEM hb_dbgActivateVarArray( PHB_ITEM pArray, int nVars, HB_VARINFO * aVars )
{
   int i;

   hb_arrayNew( pArray, nVars );
   for( i = 0; i < nVars; i++ )
   {
      PHB_ITEM aVar = hb_arrayGetItemPtr( pArray, i + 1 );

      hb_arrayNew( aVar, HB_DBG_VAR_LEN );

      hb_arraySetC( aVar, HB_DBG_VAR_NAME, aVars[ i ].szName );
      hb_arraySetNL( aVar, HB_DBG_VAR_INDEX, aVars[ i ].nIndex );
      hb_arraySetCL( aVar, HB_DBG_VAR_TYPE, &aVars[ i ].cType, 1 );
      if( aVars[ i ].cType == 'S' )
         hb_arraySet( aVar, HB_DBG_VAR_FRAME, aVars[ i ].frame.ptr );
      else
         hb_arraySetNL( aVar, HB_DBG_VAR_FRAME, aVars[ i ].frame.num );
   }
   return pArray;
}


static PHB_ITEM hb_dbgActivateModuleArray( void )
{
   PHB_ITEM pArray;
   int i;

   HB_DBGCOMMON_LOCK();

   pArray = hb_itemArrayNew( s_common.nModules );

   for( i = 0; i < s_common.nModules; i++ )
   {
      PHB_ITEM pModule = hb_arrayGetItemPtr( pArray, i + 1 );

      hb_arrayNew( pModule, HB_DBG_MOD_LEN );
      hb_arraySetC( pModule, HB_DBG_MOD_NAME, s_common.aModules[ i ].szModule );
      hb_dbgActivateVarArray( hb_arrayGetItemPtr( pModule, HB_DBG_MOD_STATICS ),
                              s_common.aModules[ i ].nStatics,
                              s_common.aModules[ i ].aStatics );
      hb_dbgActivateVarArray( hb_arrayGetItemPtr( pModule, HB_DBG_MOD_GLOBALS ),
                              s_common.aModules[ i ].nGlobals,
                              s_common.aModules[ i ].aGlobals );
      hb_dbgActivateVarArray( hb_arrayGetItemPtr( pModule, HB_DBG_MOD_EXTGLOBALS ),
                              s_common.aModules[ i ].nExternGlobals,
                              s_common.aModules[ i ].aExternGlobals );
   }

   HB_DBGCOMMON_UNLOCK();

   return pArray;
}


static PHB_ITEM hb_dbgActivateCallStackArray( HB_DEBUGINFO * info )
{
   PHB_ITEM aCallStack = hb_itemArrayNew( info->nCallStackLen );
   int i;

   for( i = 0; i < info->nCallStackLen; i++ )
   {
      HB_CALLSTACKINFO * pEntry = &info->aCallStack[ i ];
      PHB_ITEM aEntry;

      aEntry = hb_arrayGetItemPtr( aCallStack, info->nCallStackLen - i );
      hb_arrayNew( aEntry, HB_DBG_CS_LEN );

      hb_arraySetC( aEntry, HB_DBG_CS_MODULE, pEntry->szModule );
      hb_arraySetC( aEntry, HB_DBG_CS_FUNCTION, pEntry->szFunction );
      hb_arraySetNL( aEntry, HB_DBG_CS_LINE, pEntry->nLine );
      hb_arraySetNL( aEntry, HB_DBG_CS_LEVEL, pEntry->nProcLevel );
      hb_dbgActivateVarArray( hb_arrayGetItemPtr( aEntry, HB_DBG_CS_LOCALS ),
                              pEntry->nLocals, pEntry->aLocals );
      hb_dbgActivateVarArray( hb_arrayGetItemPtr( aEntry, HB_DBG_CS_STATICS ),
                              pEntry->nStatics, pEntry->aStatics );
   }

   return aCallStack;
}


static void hb_dbgActivate( HB_DEBUGINFO * info )
{
   if( ! info->pDbgEntry )
   {
      info->pDbgEntry = hb_dynsymFind( "__DBGENTRY" );
      if( info->pDbgEntry && ! hb_dynsymIsFunction( info->pDbgEntry ) )
         info->pDbgEntry = NULL;
   }

   if( info->pDbgEntry )
   {
      PHB_ITEM aCallStack, aModules, aBreak;
      HB_BOOL bInside = info->bInside;

      aCallStack = hb_dbgActivateCallStackArray( info );
      aModules = hb_dbgActivateModuleArray();
      aBreak = hb_dbgActivateBreakArray( info );

      hb_vmPushDynSym( info->pDbgEntry );
      hb_vmPushNil();
      hb_vmPushLong( HB_DBG_ACTIVATE );
      hb_vmPushPointer( info );
      hb_vmPushLong( info->nProcLevel );
      hb_vmPush( aCallStack );
      hb_vmPush( aModules );
      hb_vmPush( aBreak );

      hb_itemRelease( aCallStack );
      hb_itemRelease( aModules );
      hb_itemRelease( aBreak );

      info->bInside = HB_TRUE;
      hb_vmDo( 6 );
      info->bInside = bInside;
   }
}


void hb_dbgEntry( int nMode, int nLine, const char * szName, int nIndex, PHB_ITEM pFrame )
{
   int i;
   char szProcName[ HB_SYMBOL_NAME_LEN + HB_SYMBOL_NAME_LEN + 5 ];
   HB_DEBUGINFO ** infoPtr = ( HB_DEBUGINFO ** ) hb_stackDebugInfo();
   HB_DEBUGINFO * info = *infoPtr;
   HB_USHORT uiLine;

//    {
//     char prbuff[1024];
//     FILE *f = hb_fopen( "C:\\Users\\Fran\\Desktop\\hb_dbgEntry.log", "a" );
//     fprintf(f, "hb_dbgEntry: Mode: %d Line: %d SZName: %s INdex: %d\n", nMode, nLine, szName, nIndex);
//     fclose( f );
//    }

   //snprintf(prbuff, sizeof(prbuff), "hb_dbgEntry: Mode: %d Line: %d SZName: %s INdex: %d\n", nMode, nLine, szName, nIndex);

   if( info == HB_DBGINFO_DISABLE )
      return;
   else if( nMode != HB_DBG_VMQUIT )
   {
      if( ! info )
      {
         info = *infoPtr = ( HB_DEBUGINFO * ) hb_xgrabz( sizeof( HB_DEBUGINFO ) );
         info->bCBTrace = HB_TRUE;
      }
      else if( info->bInside || info->bQuit )
         return;
   }

   switch( nMode )
   {
      case HB_DBG_MODULENAME:
         HB_TRACE( HB_TR_DEBUG, ( "MODULENAME %s", szName ) );

         if( szName[ strlen( szName ) - 1 ] == ':' )
            return;

         hb_procinfo( 0, szProcName, &uiLine, NULL );
         if( ! strncmp( szProcName, "(_INITSTATICS", 13 ) )
            info->bInitStatics = HB_TRUE;
         else if( ! strncmp( szProcName, "(_INITGLOBALS", 13 ) )
            info->bInitGlobals = HB_TRUE;
         else if( ! strncmp( szProcName, "(_INITLINES", 11 ) )
            info->bInitLines = HB_TRUE;

         if( info->bInitStatics || info->bInitGlobals )
            hb_dbgAddModule( szName );
         else if( ! strncmp( szProcName, "(b)", 3 ) )
            info->bCodeBlock = HB_TRUE;
         else if( info->bNextRoutine )
            info->bNextRoutine = HB_FALSE;

         hb_dbgAddStack( info, szName, uiLine, hb_dbg_ProcLevel() );
         for( i = 0; i < info->nBreakPoints; i++ )
         {
            if( info->aBreak[ i ].szFunction &&
                ! strcmp( info->aBreak[ i ].szFunction, szProcName ) )
            {
               hb_dbg_InvokeDebug( HB_TRUE );
               break;
            }
         }
         return;

      case HB_DBG_LOCALNAME:
         HB_TRACE( HB_TR_DEBUG, ( "LOCALNAME %s index %d", szName, nIndex ) );

         hb_dbgAddLocal( info, szName, nIndex, hb_dbg_ProcLevel() );
         return;

      case HB_DBG_STATICNAME:
         HB_TRACE( HB_TR_DEBUG, ( "STATICNAME %s index %d frame %p", szName, nIndex, ( void * ) pFrame ) );

         hb_dbgAddStatic( info, szName, nIndex, pFrame );
         return;

      case HB_DBG_SHOWLINE:
      {
         HB_CALLSTACKINFO * pTop = &info->aCallStack[ info->nCallStackLen - 1 ];

         HB_TRACE( HB_TR_DEBUG, ( "SHOWLINE %d", nLine ) );

         /* Check if we've hit a tracepoint */
         for( i = 0; i < info->nTracePoints; i++ )
         {
            HB_TRACEPOINT * tp = &info->aTrace[ i ];
            HB_BOOL bOldClsScope;
            PHB_ITEM xValue;

            bOldClsScope = hb_clsSetScope( HB_FALSE );
            xValue = hb_dbgEval( info, &info->aWatch[ tp->nIndex ], NULL );
            hb_clsSetScope( bOldClsScope );

            if( xValue != tp->xValue &&
                ( xValue == NULL || tp->xValue == NULL ||
                  HB_ITEM_TYPE( xValue ) != HB_ITEM_TYPE( tp->xValue ) ||
                  ! hb_dbgEqual( xValue, tp->xValue ) ) )
            {
               if( tp->xValue )
                  hb_itemRelease( tp->xValue );
               tp->xValue = xValue;

               info->bCodeBlock = HB_FALSE;
               info->bTraceOver = HB_FALSE;
               info->bNextRoutine = HB_FALSE;
               info->bGo = HB_FALSE;
               if( info->bToCursor )
               {
                  info->bToCursor = HB_FALSE;
                  hb_xfree( info->szToCursorModule );
               }
               break;
            }
            if( xValue )
               hb_itemRelease( xValue );
         }

         if( i >= info->nTracePoints &&
             ( hb_dbgIsBreakPoint( info, pTop->szModule, nLine ) >= 0 ||
               hb_dbg_InvokeDebug( HB_FALSE ) ||
               ( info->pFunInvoke && info->pFunInvoke() ) ) )
         {
            info->bTraceOver = HB_FALSE;
            info->bNextRoutine = HB_FALSE;
            info->bGo = HB_FALSE;
            if( info->bToCursor )
            {
               info->bToCursor = HB_FALSE;
               hb_xfree( info->szToCursorModule );
            }
         }
         /* Check if we must skip every level above info->nTraceLevel */
         else if( info->bTraceOver )
         {
            if( info->nTraceLevel < info->nCallStackLen )
               return;
            info->bTraceOver = HB_FALSE;
         }

         /* Check if we're skipping to a specific line of source */
         if( info->bToCursor )
         {
            if( nLine == info->nToCursorLine
                && FILENAME_EQUAL( pTop->szModule, info->szToCursorModule ) )
            {
               hb_xfree( info->szToCursorModule );
               info->bToCursor = HB_FALSE;
            }
            else
               return;
         }

         /* Check if we're skipping to the end of current routine */
         if( info->bNextRoutine )
            return;

         if( info->bCodeBlock )
         {
            info->bCodeBlock = HB_FALSE;
            if( ! info->bCBTrace )
               return;
         }

         pTop->nLine = nLine;
         if( ! info->bGo )
         {
            info->nProcLevel = hb_dbg_ProcLevel() - ( hb_dbgIsAltD() ? 2 : 0 );
            hb_dbgActivate( info );
         }
         return;
      }

      case HB_DBG_ENDPROC:
         if( info->bQuit )
            return;

         HB_TRACE( HB_TR_DEBUG, ( "ENDPROC %d", nLine ) );

         if( info->bInitLines )
            hb_dbgAddStopLines( hb_stackReturnItem() );

         info->bCodeBlock   = HB_FALSE;
         info->bInitStatics = HB_FALSE;
         info->bInitGlobals = HB_FALSE;
         info->bInitLines   = HB_FALSE;
         hb_dbgEndProc( info );
         return;

      case HB_DBG_VMQUIT:
         if( info )
         {
            hb_dbgQuit( info );
            hb_xfree( info );
            *infoPtr = HB_DBGINFO_DISABLE;
         }
         if( nIndex != 0 )
         {
            /* main thread exit and HVM cleanup, release common module info */
            hb_dbgRelease();
         }
         return;
   }
}


static const char * hb_dbgStripModuleName( const char * szName )
{
   const char * ptr;

   if( ( ptr = strrchr( szName, '/' ) ) != NULL )
      szName = ptr + 1;

   if( ( ptr = strrchr( szName, '\\' ) ) != NULL )
      szName = ptr + 1;

   return szName;
}


void hb_dbgAddBreak( void * handle, const char * szModule, int nLine, const char * szFunction )
{
   if( szModule || szFunction )
   {
      HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
      HB_BREAKPOINT * pBreak;

      pBreak = ARRAY_ADD( HB_BREAKPOINT, info->aBreak, info->nBreakPoints );
      if( szModule )
         pBreak->szModule = hb_strdup( hb_dbgStripModuleName( szModule ) );
      else
         pBreak->szModule = NULL;
      pBreak->nLine = nLine;

      if( szFunction )
         pBreak->szFunction = hb_strdup( szFunction );
      else
         pBreak->szFunction = NULL;
   }
}


static void hb_dbgAddLocal( HB_DEBUGINFO * info, const char * szName, int nIndex, int nFrame )
{
   if( info->bInitGlobals )
   {
      HB_MODULEINFO * module;

      HB_DBGCOMMON_LOCK();
      module = &s_common.aModules[ s_common.nModules - 1 ];
      hb_dbgAddVar( &module->nGlobals, &module->aGlobals, szName,
                    'G', nIndex, hb_dbg_vmVarGCount(), NULL );
      HB_DBGCOMMON_UNLOCK();
   }
   else
   {
      HB_CALLSTACKINFO * top = &info->aCallStack[ info->nCallStackLen - 1 ];

      hb_dbgAddVar( &top->nLocals, &top->aLocals, szName, 'L', nIndex, nFrame, NULL );
   }
}


static void hb_dbgAddModule( const char * szName )
{
   char * szModuleName;
   const char * szFuncName;
   int iLen;

   szName = hb_dbgStripModuleName( szName );
   szFuncName = strrchr( szName, ':' );
   iLen = szFuncName ? ( int ) ( szFuncName - szName ) : ( int ) strlen( szName );
   szModuleName = hb_strndup( szName, iLen );

   HB_DBGCOMMON_LOCK();
   if( ! s_common.nModules || !FILENAME_EQUAL( s_common.aModules[ s_common.nModules - 1 ].szModule, szModuleName ) )
   {
      HB_MODULEINFO * pModule;

      pModule = ARRAY_ADD( HB_MODULEINFO, s_common.aModules, s_common.nModules );
      pModule->szModule = szModuleName;
      pModule->nStatics = 0;
      pModule->nGlobals = 0;
      pModule->nExternGlobals = 0;

      szModuleName = NULL;
   }
   HB_DBGCOMMON_UNLOCK();

   if( szModuleName )
      hb_xfree( szModuleName );
}


static void hb_dbgAddStack( HB_DEBUGINFO * info, const char * szName, int nLine, int nProcLevel )
{
   char szBuff[ HB_SYMBOL_NAME_LEN + HB_SYMBOL_NAME_LEN + 5 ];
   HB_CALLSTACKINFO * top;
   const char * szFunction;

   szName = hb_dbgStripModuleName( szName );

   szFunction = strrchr( szName, ':' );
   if( szFunction )
      szFunction++;

   top = ARRAY_ADD( HB_CALLSTACKINFO, info->aCallStack, info->nCallStackLen );
   if( info->bCodeBlock )
   {
      memcpy( szBuff, "(b)", 3 );
      hb_strncpy( szBuff + 3, szFunction, sizeof( szBuff ) - 4 );
      top->szFunction = hb_strdup( szBuff );
   }
   else
   {
      if( szFunction )
      {
         top->szFunction = hb_strdup( szFunction );
      }
      else
      {
         /* We're in an (_INITSTATICSnnnnn) pseudo-function */
         hb_procinfo( 0, szBuff, NULL, NULL );
         top->szFunction = hb_strdup( szBuff );
      }
   }

   if( szFunction )
      top->szModule = hb_strndup( szName, szFunction - szName - 1 );
   else
      top->szModule = hb_strdup( szName );

   top->nProcLevel = nProcLevel;
   top->nLine = nLine;
   top->nLocals = 0;
   top->nStatics = 0;
}


static void hb_dbgAddStatic( HB_DEBUGINFO * info, const char * szName, int nIndex, PHB_ITEM pFrame )
{
   if( info->bInitGlobals )
   {
      HB_MODULEINFO * module;

      HB_DBGCOMMON_LOCK();
      module = &s_common.aModules[ s_common.nModules - 1 ];
      hb_dbgAddVar( &module->nExternGlobals, &module->aExternGlobals, szName,
                    'G', nIndex, hb_dbg_vmVarGCount(), NULL );
      HB_DBGCOMMON_UNLOCK();
   }
   else if( info->bInitStatics )
   {
      HB_MODULEINFO * module;

      HB_DBGCOMMON_LOCK();
      module = &s_common.aModules[ s_common.nModules - 1 ];
      hb_dbgAddVar( &module->nStatics, &module->aStatics, szName,
                    'S', nIndex, 0, pFrame );
      HB_DBGCOMMON_UNLOCK();
   }
   else
   {
      HB_CALLSTACKINFO * top = &info->aCallStack[ info->nCallStackLen - 1 ];

      hb_dbgAddVar( &top->nStatics, &top->aStatics, szName, 'S', nIndex, 0, pFrame );
   }
}


static void hb_dbgAddStopLines( PHB_ITEM pItem )
{
   HB_ISIZ i, nLinesLen;

   HB_DBGCOMMON_LOCK();

   if( ! s_common.pStopLines )
   {
      s_common.pStopLines = hb_itemNew( pItem );
   }
   else
   {
      HB_ISIZ j;
      HB_ISIZ nItemLen = hb_itemSize( pItem );

      nLinesLen = hb_itemSize( s_common.pStopLines );

      for( i = 1; i <= nItemLen; i++ )
      {
         PHB_ITEM pEntry = hb_arrayGetItemPtr( pItem, i );
         const char * szModule = hb_arrayGetCPtr( pEntry, 1 );
         HB_BOOL bFound = HB_FALSE;

         szModule = hb_dbgStripModuleName( szModule );
         for( j = 1; j <= nLinesLen; j++ )
         {
            PHB_ITEM pLines = hb_arrayGetItemPtr( s_common.pStopLines, j );

            if( FILENAME_EQUAL( hb_arrayGetCPtr( pLines, 1 ), szModule ) )
            {
               /* Merge stopline info */
               HB_ISIZ nOrigMin = hb_arrayGetNS( pLines, 2 );
               HB_ISIZ nNewMin = hb_arrayGetNS( pEntry, 2 );
               HB_ISIZ nOrigLen = hb_arrayGetCLen( pLines, 3 );
               HB_ISIZ nNewLen = hb_arrayGetCLen( pEntry, 3 );
               HB_ISIZ nMin = HB_MIN( nNewMin, nOrigMin );
               HB_ISIZ nMax = HB_MAX( nNewMin + ( nNewLen << 3 ) - 1,
                                      nOrigMin + ( nOrigLen << 3 ) - 1 );
               const char * pOrigBuffer = hb_arrayGetCPtr( pLines, 3 );
               const char * pNewBuffer = hb_arrayGetCPtr( pEntry, 3 );
               HB_ISIZ nLen = ( ( nMax - nMin ) >> 3 ) + 1;
               HB_ISIZ k;
               char * pBuffer = ( char * ) hb_xgrabz( nLen + 1 );

               /* the bitfields with line numbers should use
                * 8bit alignment so it's safe to use byte copy
                */
               memmove( &pBuffer[ ( nNewMin - nMin ) >> 3 ], pNewBuffer, nNewLen );
               nOrigMin = ( nOrigMin - nMin ) >> 3;
               for( k = 0; k < nOrigLen; k++ )
                  pBuffer[ nOrigMin + k ] |= pOrigBuffer[ k ];

               hb_arraySetNS( pLines, 2, nMin );
               if( ! hb_arraySetCLPtr( pLines, 3, pBuffer, nLen ) )
                  hb_xfree( pBuffer );
               bFound = HB_TRUE;
               break;
            }
         }

         if( ! bFound )
            hb_arrayAddForward( s_common.pStopLines, pEntry );
      }
   }
   nLinesLen = hb_itemSize( s_common.pStopLines );
   for( i = 1; i <= nLinesLen; i++ )
   {
      PHB_ITEM pEntry = hb_arrayGetItemPtr( s_common.pStopLines, i );
      const char * szModule = hb_arrayGetCPtr( pEntry, 1 );

      if( szModule )
      {
         const char * szName = hb_dbgStripModuleName( szModule );

         if( szName != szModule )
            hb_arraySetCLPtr( pEntry, 1, hb_strdup( szName ), strlen( szName ) );
      }
   }

   HB_DBGCOMMON_UNLOCK();
}


static void hb_dbgAddVar( int * nVars, HB_VARINFO ** aVars, const char * szName, char cType, int nIndex, int nFrame, PHB_ITEM pFrame )
{
   HB_VARINFO * var;

   var = ARRAY_ADD( HB_VARINFO, *aVars, *nVars );
   var->szName = szName;
   var->cType = cType;
   var->nIndex = nIndex;
   if( cType == 'S' )
      var->frame.ptr = pFrame;
   else
      var->frame.num = nFrame;
}


void hb_dbgAddWatch( void * handle, const char * szExpr, HB_BOOL bTrace )
{
   if( szExpr )
   {
      HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
      HB_WATCHPOINT * pWatch;

      pWatch = ARRAY_ADD( HB_WATCHPOINT, info->aWatch, info->nWatchPoints );
      pWatch->szExpr = hb_strdup( szExpr );
      pWatch->pBlock = NULL;
      pWatch->nVars = 0;

      if( bTrace )
      {
         HB_TRACEPOINT * pTrace = ARRAY_ADD( HB_TRACEPOINT, info->aTrace, info->nTracePoints );

         pTrace->nIndex = info->nWatchPoints - 1;
         pTrace->xValue = hb_dbgEval( info, pWatch, NULL );
      }
   }
}


static void hb_dbgClearWatch( HB_WATCHPOINT * pWatch )
{
   hb_xfree( pWatch->szExpr );

   if( pWatch->pBlock )
      hb_itemRelease( pWatch->pBlock );

   if( pWatch->nVars )
   {
      int i;

      for( i = 0; i < pWatch->nVars; i++ )
         hb_xfree( pWatch->aVars[ i ] );

      hb_xfree( pWatch->aVars );
   }
}


static int hb_dbgCountWatch( void * handle )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   return info->nWatchPoints;
}


void hb_dbgDelBreak( void * handle, int nBreak )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   if( nBreak >= 0 && nBreak < info->nBreakPoints )
   {
      HB_BREAKPOINT * pBreak = &info->aBreak[ nBreak ];

      if( pBreak->szModule )
         hb_xfree( pBreak->szModule );
      if( pBreak->szFunction )
         hb_xfree( pBreak->szFunction );

      ARRAY_DEL( HB_BREAKPOINT, info->aBreak, info->nBreakPoints, nBreak );
   }
}


void hb_dbgDelWatch( void * handle, int nWatch )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   if( nWatch >= 0 && nWatch < info->nWatchPoints )
   {
      HB_WATCHPOINT * pWatch = &info->aWatch[ nWatch ];
      int i;

      hb_dbgClearWatch( pWatch );
      ARRAY_DEL( HB_WATCHPOINT, info->aWatch, info->nWatchPoints, nWatch );

      for( i = 0; i < info->nTracePoints; i++ )
      {
         HB_TRACEPOINT * pTrace = &info->aTrace[ i ];

         if( pTrace->nIndex == nWatch )
         {
            if( pTrace->xValue )
               hb_itemRelease( pTrace->xValue );

            ARRAY_DEL( HB_TRACEPOINT, info->aTrace, info->nTracePoints, i );
            i--;
         }
         else if( pTrace->nIndex > nWatch )
            pTrace->nIndex--;
      }
   }
}

static void hb_dbgEndProc( HB_DEBUGINFO * info )
{
   HB_CALLSTACKINFO * top;

   if( ! info->nCallStackLen )
      return;

   top = &info->aCallStack[ --info->nCallStackLen ];
   hb_xfree( top->szFunction );
   hb_xfree( top->szModule );

   if( top->nLocals )
      hb_xfree( top->aLocals );

   if( top->nStatics )
      hb_xfree( top->aStatics );

   if( ! info->nCallStackLen )
   {
      hb_xfree( info->aCallStack );
      info->aCallStack = NULL;
   }
}


static HB_BOOL hb_dbgEqual( PHB_ITEM pItem1, PHB_ITEM pItem2 )
{
   if( HB_ITEM_TYPE( pItem1 ) != HB_ITEM_TYPE( pItem2 ) )
      return HB_FALSE;
   if( HB_IS_NIL( pItem1 ) )
      return HB_IS_NIL( pItem2 );
   if( HB_IS_LOGICAL( pItem1 ) )
      return hb_itemGetL( pItem1 ) == hb_itemGetL( pItem2 );
   if( HB_IS_POINTER( pItem1 ) )
      return hb_itemGetPtr( pItem1 ) == hb_itemGetPtr( pItem2 );
   if( HB_IS_SYMBOL( pItem1 ) )
      return hb_itemGetSymbol( pItem1 ) == hb_itemGetSymbol( pItem2 );
   if( HB_IS_STRING( pItem1 ) )
      return ! hb_itemStrCmp( pItem1, pItem2, HB_TRUE );
   if( HB_IS_NUMINT( pItem1 ) )
      return hb_itemGetNInt( pItem1 ) == hb_itemGetNInt( pItem2 );
   if( HB_IS_NUMERIC( pItem1 ) )
      return hb_itemGetND( pItem1 ) == hb_itemGetND( pItem2 );
   if( HB_IS_DATE( pItem1 ) )
      return hb_itemGetDL( pItem1 ) == hb_itemGetDL( pItem2 );
   if( HB_IS_TIMESTAMP( pItem1 ) )
      return hb_itemGetTD( pItem1 ) == hb_itemGetTD( pItem2 );
   if( HB_IS_ARRAY( pItem1 ) )
      return hb_arrayId( pItem1 ) == hb_arrayId( pItem2 );
   if( HB_IS_HASH( pItem1 ) )
      return hb_hashId( pItem1 ) == hb_hashId( pItem2 );
   return HB_FALSE;
}


static PHB_ITEM hb_dbgEval( HB_DEBUGINFO * info, HB_WATCHPOINT * watch, HB_BOOL * valid )
{
   PHB_ITEM xResult = NULL;

   HB_TRACE( HB_TR_DEBUG, ( "expr %s", watch->szExpr ) );

   /* Check if we have a cached pBlock */
   if( ! watch->pBlock )
      watch->pBlock = hb_dbgEvalMakeBlock( watch );

   if( valid != NULL )
      * valid = HB_FALSE;

   if( watch->pBlock )
   {
      PHB_ITEM aVars = hb_dbgEvalResolve( info, watch );
      PHB_ITEM aNewVars = hb_itemClone( aVars );
      HB_BOOL bInside = info->bInside;
      int i;

      info->bInside = HB_TRUE;

      if( hb_vmTryEval( &xResult, watch->pBlock, 1, aNewVars ) )
      {
         if( valid != NULL )
            * valid = HB_TRUE;
      }
      else if( valid == NULL )
      {
         hb_itemRelease( xResult );
         xResult = NULL;
      }

      info->bInside = bInside;

      for( i = 0; i < watch->nVars; i++ )
      {
         PHB_ITEM xOldValue = hb_arrayGetItemPtr( aVars, i + 1 );
         PHB_ITEM xNewValue = hb_arrayGetItemPtr( aNewVars, i + 1 );

         if( ! hb_dbgEqual( xOldValue, xNewValue ) )
            hb_dbgVarSet( &watch->aScopes[ i ], xNewValue );
      }

      hb_itemRelease( aVars );
      hb_itemRelease( aNewVars );
      if( watch->nVars )
         hb_xfree( watch->aScopes );
   }
   return xResult;
}


static PHB_ITEM hb_dbgEvalMacro( const char * szExpr, PHB_ITEM pItem )
{
   PHB_ITEM pStr;
   const char * type;

   pStr = hb_itemPutC( NULL, szExpr );
   type = hb_macroGetType( pStr );
   hb_itemRelease( pStr );
   if( ! strcmp( type, "U" ) || ! strcmp( type, "UE" ) )
      return NULL;

   hb_vmPushString( szExpr, strlen( szExpr ) );
   hb_macroGetValue( hb_stackItemFromTop( -1 ), 0, HB_SM_RT_MACRO );
   hb_itemMove( pItem, hb_stackItemFromTop( -1 ) );
   hb_stackPop();

   return pItem;
}


static int hb_dbgEvalSubstituteVar( HB_WATCHPOINT * watch,
                                    char * szWord, int nStart, int nLen,
                                    char ** pszOrig )
{
   char buf[ 16 ];
   char * szExpr;
   HB_SIZE n;
   int j;

   for( j = 0; j < watch->nVars; j++ )
   {
      if( ! strcmp( szWord, watch->aVars[ j ] ) )
         break;
   }

   if( j == watch->nVars )
      *ARRAY_ADD( char *, watch->aVars, watch->nVars ) = szWord;
   else
      hb_xfree( szWord );

   n = strlen( watch->szExpr );
   j = hb_snprintf( buf, sizeof( buf ), "__dbg[%d]", j + 1 );
   szExpr = ( char * ) hb_xgrab( n - nLen + j + 1 );
   memcpy( szExpr, watch->szExpr, nStart );
   memcpy( szExpr + nStart, buf, j );
   memcpy( szExpr + nStart + j, watch->szExpr + nStart + nLen, n - nLen - nStart );
   szExpr[ n + j - nLen ] = '\0';
   if( * pszOrig == NULL )
      * pszOrig = watch->szExpr;
   else
      hb_xfree( watch->szExpr );
   watch->szExpr = szExpr;

   return nStart + j;
}


static PHB_ITEM hb_dbgEvalMakeBlock( HB_WATCHPOINT * watch )
{
   int i = 0;
   PHB_ITEM pBlock;
   HB_BOOL bAfterId = HB_FALSE;
   char * szBlock, * szOrig = NULL;
   HB_ISIZ buffsize;

   watch->nVars = 0;
   while( watch->szExpr[ i ] )
   {
      char c = watch->szExpr[ i ];

      if( HB_ISFIRSTIDCHAR( c ) )
      {
         int nStart = i, nLen;
         int j = i;
         char * szWord;

         while( c && HB_ISNEXTIDCHAR( c ) )
            c = watch->szExpr[ ++j ];

         nLen = j - i;
         i = j;
         if( c )
         {
            while( watch->szExpr[ i ] == ' ' )
               i++;

            if( watch->szExpr[ i ] == '(' ||
                ( nLen == 1 && i == j && watch->szExpr[ i ] == '"' ) )
               continue;

            if( watch->szExpr[ i ] == '-' && watch->szExpr[ i + 1 ] == '>' )
            {
               i += 2;

               while( ( c = watch->szExpr[ i ] ) != '\0' && HB_ISNEXTIDCHAR( c ) )
                  i++;

               continue;
            }
         }
         szWord = hb_strupr( hb_strndup( watch->szExpr + nStart, nLen ) );
         i = hb_dbgEvalSubstituteVar( watch, szWord, nStart, nLen, &szOrig );
         bAfterId = HB_TRUE;
         continue;
      }
      if( c == '.' )
      {
         if( watch->szExpr[ i + 1 ] &&
             strchr( "TtFf", watch->szExpr[ i + 1 ] ) &&
             watch->szExpr[ i + 2 ] == '.' )
         {
            i += 3;
         }
         else if( ! hb_strnicmp( watch->szExpr + i + 1, "OR.", 3 ) )
         {
            i += 4;
         }
         else if( ! hb_strnicmp( watch->szExpr + i + 1, "AND.", 4 ) ||
                  ! hb_strnicmp( watch->szExpr + i + 1, "NOT.", 4 ) )
         {
            i += 5;
         }
         else
         {
            i++;
         }
         bAfterId = HB_FALSE;
         continue;
      }
      if( c == ':' ||
          ( c == '-' && watch->szExpr[ i + 1 ] == '>' &&
            HB_ISFIRSTIDCHAR( watch->szExpr[ i + 2 ] ) ) )
      {
         if( c == ':' && watch->szExpr[ i + 1 ] == ':' )
         {
            i = hb_dbgEvalSubstituteVar( watch, hb_strdup( "SELF" ), i, 1, &szOrig );
            bAfterId = HB_TRUE;
            continue;
         }

         if( c == '-' )
            i++;

         i++;

         while( watch->szExpr[ i ] && HB_ISNEXTIDCHAR( watch->szExpr[ i ] ) )
            i++;

         bAfterId = HB_TRUE;
         continue;
      }
      if( strchr( " !#$=<>(+-*/%^|,{&", c ) )
      {
         i++;
         bAfterId = HB_FALSE;
         continue;
      }
      if( c == '\'' || c == '\"' )
      {
         i++;

         while( watch->szExpr[ i ] && watch->szExpr[ i ] != c )
            i++;

         if( watch->szExpr[ i ] )
            i++;

         bAfterId = HB_TRUE;
         continue;
      }
      if( c == '[' )
      {
         i++;
         if( bAfterId )
            bAfterId = HB_FALSE;
         else
         {
            while( watch->szExpr[ i ] && watch->szExpr[ i ] != ']' )
               i++;

            if( watch->szExpr[ i ] )
               i++;

            bAfterId = HB_TRUE;
         }
         continue;
      }
      i++;
   }

   buffsize = 8 + strlen( watch->szExpr ) + 1;

   szBlock = ( char * ) hb_xgrab( buffsize + 1 );
   hb_strncpy( szBlock, "{|__dbg|", buffsize );
   hb_strncat( szBlock, watch->szExpr, buffsize );
   hb_strncat( szBlock, "}", buffsize );
   pBlock = hb_itemNew( NULL );

   if( ! hb_dbgEvalMacro( szBlock, pBlock ) )
   {
      hb_itemRelease( pBlock );
      pBlock = NULL;
   }
   hb_xfree( szBlock );

   if( szOrig != NULL )
   {
      hb_xfree( watch->szExpr );
      watch->szExpr = szOrig;
   }

   return pBlock;
}


static PHB_ITEM hb_dbgEvalResolve( HB_DEBUGINFO * info, HB_WATCHPOINT * watch )
{
   int i;
   HB_CALLSTACKINFO * top = &info->aCallStack[ info->nCallStackLen - 1 ];
   PHB_ITEM aVars = hb_itemArrayNew( watch->nVars );
   HB_VARINFO * scopes;
   HB_MODULEINFO * module = NULL;
   int nProcLevel;

   if( ! watch->nVars )
      return aVars;

   scopes = ( HB_VARINFO * ) hb_xgrab( watch->nVars * sizeof( HB_VARINFO ) );
   nProcLevel = hb_dbg_ProcLevel();

   HB_DBGCOMMON_LOCK();

   for( i = 0; i < s_common.nModules; i++ )
   {
      if( FILENAME_EQUAL( s_common.aModules[ i ].szModule, top->szModule ) )
      {
         module = &s_common.aModules[ i ];
         break;
      }
   }

   for( i = 0; i < watch->nVars; i++ )
   {
      char * name = watch->aVars[ i ];
      HB_VARINFO * var;
      int j;
      PHB_ITEM pItem;

      for( j = 0; j < top->nLocals; j++ )
      {
         var = &top->aLocals[ j ];
         if( ! strcmp( name, var->szName ) )
         {
            scopes[ i ].cType = 'L';
            scopes[ i ].frame.num = nProcLevel - var->frame.num;
            scopes[ i ].nIndex = var->nIndex;
            hb_itemArrayPut( aVars, i + 1, hb_dbgVarGet( &scopes[ i ] ) );
            break;
         }
      }
      if( j < top->nLocals )
         continue;

      for( j = 0; j < top->nStatics; j++ )
      {
         var = &top->aStatics[ j ];
         if( ! strcmp( name, var->szName ) )
         {
            scopes[ i ].cType = 'S';
            scopes[ i ].frame.ptr = var->frame.ptr;
            scopes[ i ].nIndex = var->nIndex;
            hb_itemArrayPut( aVars, i + 1, hb_dbgVarGet( &scopes[ i ] ) );
            break;
         }
      }
      if( j < top->nStatics )
         continue;

      if( module )
      {
         for( j = 0; j < module->nStatics; j++ )
         {
            var = &module->aStatics[ j ];
            if( ! strcmp( name, var->szName ) )
            {
               scopes[ i ].cType = 'S';
               scopes[ i ].frame.ptr = var->frame.ptr;
               scopes[ i ].nIndex = var->nIndex;
               hb_itemArrayPut( aVars, i + 1, hb_dbgVarGet( &scopes[ i ] ) );
               break;
            }
         }
         if( j < module->nStatics )
            continue;

         for( j = 0; j < module->nGlobals; j++ )
         {
            var = &module->aGlobals[ j ];
            if( ! strcmp( name, var->szName ) )
            {
               scopes[ i ].cType = 'G';
               scopes[ i ].frame.num = var->frame.num;
               scopes[ i ].nIndex = var->nIndex;
               hb_itemArrayPut( aVars, i + 1, hb_dbgVarGet( &scopes[ i ] ) );
               break;
            }
         }
         if( j < module->nGlobals )
            continue;

         for( j = 0; j < module->nExternGlobals; j++ )
         {
            var = &module->aExternGlobals[ j ];
            if( ! strcmp( name, var->szName ) )
            {
               scopes[ i ].cType = 'G';
               scopes[ i ].frame.num = var->frame.num;
               scopes[ i ].nIndex = var->nIndex;
               hb_itemArrayPut( aVars, i + 1, hb_dbgVarGet( &scopes[ i ] ) );
               break;
            }
         }
         if( j < module->nExternGlobals )
            continue;
      }

      scopes[ i ].cType  = 'M';
      scopes[ i ].szName = hb_dynsymGetSymbol( name )->szName;

      pItem = hb_dbgVarGet( &scopes[ i ] );

      if( pItem )
         hb_itemArrayPut( aVars, i + 1, pItem );

      if( scopes[ i ].cType == 'F' )
         hb_itemRelease( pItem );
   }
   watch->aScopes = scopes;

   HB_DBGCOMMON_UNLOCK();

   return aVars;
}


PHB_ITEM hb_dbgGetExpressionValue( void * handle, const char * expression )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
   PHB_ITEM result;
   HB_WATCHPOINT point;

   point.szExpr = hb_strdup( expression );
   point.pBlock = NULL;
   point.nVars = 0;

   result = hb_dbgEval( info, &point, NULL );

   hb_dbgClearWatch( &point );

   return result;
}


PHB_ITEM hb_dbgGetWatchValue( void * handle, int nWatch )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   if( nWatch >= 0 && nWatch < info->nWatchPoints )
      return hb_dbgEval( info, &info->aWatch[ nWatch ], NULL );
   else
      return NULL;
}


PHB_ITEM hb_dbgGetSourceFiles( void * handle )
{
   PHB_ITEM ret;
   HB_ISIZ nModules;
   HB_ISIZ i;

#if 0
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
#endif
   HB_SYMBOL_UNUSED( handle );

   HB_DBGCOMMON_LOCK();
   nModules = hb_itemSize( s_common.pStopLines );
   ret = hb_itemArrayNew( nModules );
   for( i = 1; i <= nModules; i++ )
      hb_arraySet( ret, i, hb_arrayGetItemPtr( hb_arrayGetItemPtr( s_common.pStopLines, i ), 1 ) );
   HB_DBGCOMMON_UNLOCK();

   return ret;
}


static HB_BOOL hb_dbgIsAltD( void )
{
   HB_ISIZ nOffset = hb_stackBaseProcOffset( 1 );

   return nOffset > 0 &&
          ! strcmp( hb_itemGetSymbol( hb_stackItem( nOffset ) )->szName, "ALTD" );
}


static int hb_dbgIsBreakPoint( HB_DEBUGINFO * info, const char * szModule, int nLine )
{
   int i;

   /* szModule has stripped path here */

   for( i = 0; i < info->nBreakPoints; i++ )
   {
      HB_BREAKPOINT * point = &info->aBreak[ i ];

      if( point->nLine == nLine && point->szModule &&
          FILENAME_EQUAL( szModule, point->szModule ) )
         return i;
   }
   return -1;
}


HB_BOOL hb_dbgIsValidStopLine( void * handle, const char * szModule, int nLine )
{
   HB_BOOL fResult = HB_FALSE;

   if( szModule )
   {
      HB_ISIZ nModules;
      HB_ISIZ i;

#if 0
      HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
#endif
      HB_SYMBOL_UNUSED( handle );

      szModule = hb_dbgStripModuleName( szModule );

      HB_DBGCOMMON_LOCK();
      nModules = hb_itemSize( s_common.pStopLines );
      for( i = 1; i <= nModules; i++ )
      {
         PHB_ITEM pEntry = hb_arrayGetItemPtr( s_common.pStopLines, i );

         if( FILENAME_EQUAL( hb_arrayGetCPtr( pEntry, 1 ), szModule ) )
         {
            int nMin = hb_arrayGetNL( pEntry, 2 );
            int nOfs = nLine - nMin;

            if( nOfs >= 0 && ( HB_SIZE ) ( nOfs >> 3 ) < hb_arrayGetCLen( pEntry, 3 ) )
               fResult = ( hb_arrayGetCPtr( pEntry, 3 )[ nOfs >> 3 ] & ( 1 << ( nOfs & 0x07 ) ) ) != 0;

            break;
         }
      }
      HB_DBGCOMMON_UNLOCK();
   }
   return fResult;
}


const char * hb_dbgGetModuleName( void * handle, const char * szName )
{
   #if 0
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;
   #endif
   HB_SYMBOL_UNUSED( handle );

   if( szName )
      szName = hb_dbgStripModuleName( szName );

   return szName;
}


static void hb_dbgQuit( HB_DEBUGINFO * info )
{
   while( info->nWatchPoints )
   {
      hb_dbgDelWatch( info, info->nWatchPoints - 1 );
   }
   while( info->nBreakPoints )
   {
      hb_dbgDelBreak( info, info->nBreakPoints - 1 );
   }
   while( info->nCallStackLen )
   {
      hb_dbgEndProc( info );
   }
   if( info->bToCursor )
   {
      info->bToCursor = HB_FALSE;
      hb_xfree( info->szToCursorModule );
   }
}


static void hb_dbgRelease( void )
{
   if( s_common.pStopLines )
   {
      hb_itemRelease( s_common.pStopLines );
      s_common.pStopLines = NULL;
   }
   while( s_common.nModules )
   {
      int nModules = s_common.nModules - 1;
      HB_MODULEINFO * module = &s_common.aModules[ nModules ];
      if( module->nStatics )
      {
         hb_xfree( module->aStatics );
      }
      if( module->nGlobals )
      {
         hb_xfree( module->aGlobals );
      }
      if( module->nExternGlobals )
      {
         hb_xfree( module->aExternGlobals );
      }
      if( module->szModule )
      {
         hb_xfree( module->szModule );
      }
      ARRAY_DEL( HB_MODULEINFO, s_common.aModules, s_common.nModules, nModules );
   }
}


void hb_dbgSetCBTrace( void * handle, HB_BOOL bCBTrace )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->bCBTrace = bCBTrace;
}


void hb_dbgSetGo( void * handle )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->bGo = HB_TRUE;
}


void hb_dbgSetInvoke( void * handle, HB_BOOL ( * pFunInvoke )( void ) )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->pFunInvoke = pFunInvoke;
}


void hb_dbgSetNextRoutine( void * handle )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->bNextRoutine = HB_TRUE;
}


void hb_dbgSetQuit( void * handle )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->bQuit = HB_TRUE;
}


void hb_dbgSetToCursor( void * handle, const char * szModule, int nLine )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   if( szModule )
   {
      szModule = hb_dbgStripModuleName( szModule );

      info->bToCursor = HB_TRUE;
      info->szToCursorModule = hb_strdup( szModule );
      info->nToCursorLine = nLine;
   }
}


void hb_dbgSetTrace( void * handle )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   info->bTraceOver = HB_TRUE;
   info->nTraceLevel = info->nCallStackLen;
}


void hb_dbgSetWatch( void * handle, int nWatch, const char * szExpr, HB_BOOL bTrace )
{
   HB_DEBUGINFO * info = ( HB_DEBUGINFO * ) handle;

   if( nWatch >= 0 && nWatch < info->nWatchPoints && szExpr )
   {
      HB_WATCHPOINT * pWatch = &info->aWatch[ nWatch ];
      int i;

      hb_dbgClearWatch( pWatch );
      pWatch->szExpr = hb_strdup( szExpr );
      pWatch->pBlock = NULL;
      for( i = 0; i < info->nTracePoints; i++ )
      {
         HB_TRACEPOINT * pTrace = &info->aTrace[ i ];

         if( pTrace->nIndex == nWatch )
         {
            if( pTrace->xValue )
               hb_itemRelease( pTrace->xValue );

            ARRAY_DEL( HB_TRACEPOINT, info->aTrace, info->nTracePoints, i );
            break;
         }
      }
      if( bTrace )
      {
         HB_TRACEPOINT * pTrace = ARRAY_ADD( HB_TRACEPOINT, info->aTrace, info->nTracePoints );

         pTrace->nIndex = nWatch;
         pTrace->xValue = hb_dbgEval( info, pWatch, NULL );
      }
   }
}


static PHB_ITEM hb_dbgVarGet( HB_VARINFO * scope )
{
   switch( scope->cType )
   {
      case 'G':
         return hb_dbg_vmVarGGet( scope->frame.num, scope->nIndex );
      case 'L':
         return hb_dbg_vmVarLGet( scope->frame.num, scope->nIndex );
      case 'S':
         return hb_dbg_vmVarSGet( scope->frame.ptr, scope->nIndex );
      case 'M':
      {
         PHB_DYNS pDyn;

         pDyn = hb_dynsymFind( scope->szName );
         if( pDyn != NULL )
         {
            PHB_ITEM pItem = hb_memvarGetValueBySym( pDyn );
            if( ! pItem )
            {
               pItem = hb_itemNew( NULL );
               if( hb_rddFieldGet( pItem, hb_dynsymSymbol( pDyn ) ) == HB_SUCCESS )
               {
                  scope->cType = 'F';
               }
               else
               {
                  hb_itemRelease( pItem );
                  pItem = NULL;
               }
            }
            return pItem;
         }
      }
   }
   return NULL;
}


static void hb_dbgVarSet( HB_VARINFO * scope, PHB_ITEM xNewValue )
{
   switch( scope->cType )
   {
      case 'G':
      case 'L':
      case 'S':
         hb_itemCopy( hb_dbgVarGet( scope ), xNewValue );
         break;
      case 'M':
         hb_memvarSetValue( hb_dynsymSymbol( hb_dynsymGet( scope->szName ) ),
                            xNewValue );
         break;
   }
}

/*
 * .prg functions
 */
HB_FUNC( __DBGSETENTRY )
{
   hb_dbg_SetEntry( hb_dbgEntry );
}

HB_FUNC( __DBGSETGO )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetGo( ptr );
}

HB_FUNC( __DBGSETTRACE )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetTrace( ptr );
}

HB_FUNC( __DBGSETCBTRACE )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetCBTrace( ptr, hb_parl( 2 ) );
}

HB_FUNC( __DBGSETNEXTROUTINE )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetNextRoutine( ptr );
}

HB_FUNC( __DBGSETQUIT )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetQuit( ptr );
}

HB_FUNC( __DBGSETTOCURSOR )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetToCursor( ptr, hb_parc( 2 ), hb_parni( 3 ) );
}

HB_FUNC( __DBGGETEXPRVALUE )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
   {
      PHB_ITEM pItem;

      if( HB_ISCHAR( 2 ) )
         pItem = hb_dbgGetExpressionValue( ptr, hb_parc( 2 ) );
      else
         pItem = hb_dbgGetWatchValue( ptr, hb_parni( 2 ) - 1 );

      if( pItem )
      {
         hb_storl( HB_TRUE, 3 );
         hb_itemReturnRelease( pItem );
      }
      else
         hb_storl( HB_FALSE, 3 );
   }
}

HB_FUNC( __DBGGETSOURCEFILES )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_itemReturnRelease( hb_dbgGetSourceFiles( ptr ) );
}

HB_FUNC( __DBGISVALIDSTOPLINE )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_retl( hb_dbgIsValidStopLine( ptr, hb_parc( 2 ), hb_parni( 3 ) ) );
}

HB_FUNC( __DBGADDBREAK )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgAddBreak( ptr, hb_parc( 2 ), hb_parni( 3 ), hb_parc( 4 ) );
}

HB_FUNC( __DBGDELBREAK )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgDelBreak( ptr, hb_parni( 2 ) );
}

HB_FUNC( __DBGISBREAK )
{
   void * ptr = hb_parptr( 1 );
   const char * szModule = hb_parc( 2 );

   if( ptr && szModule )
      hb_retni( hb_dbgIsBreakPoint( ( HB_DEBUGINFO * ) ptr,
                                    hb_dbgStripModuleName( szModule ),
                                    hb_parni( 3 ) ) );
}

HB_FUNC( __DBGGETBREAKPOINTS )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_itemReturnRelease( hb_dbgActivateBreakArray( ( HB_DEBUGINFO * ) ptr ) );
}

HB_FUNC( __DBGADDWATCH )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgAddWatch( ptr, hb_parc( 2 ), hb_parl( 3 ) );
}

HB_FUNC( __DBGDELWATCH )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgDelWatch( ptr, hb_parni( 2 ) );
}

HB_FUNC( __DBGSETWATCH )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_dbgSetWatch( ptr, hb_parni( 2 ), hb_parc( 3 ), hb_parl( 4 ) );
}

HB_FUNC( __DBGCNTWATCH )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_retni( hb_dbgCountWatch( ptr ) );
}

HB_FUNC( __DBGGETWATCHPOINTS )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_itemReturnRelease( hb_dbgActivateWatchArray( ( HB_DEBUGINFO * ) ptr ) );
}

HB_FUNC( __DBGGETSETS )
{
   hb_itemReturnRelease( hb_dbgSetArray() );
}

HB_FUNC( __DBGGETMODULENAME )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
      hb_retc( hb_dbgGetModuleName( ptr, hb_parc( 2 ) ) );
}

HB_FUNC( __DBGMODULEMATCH )
{
   void * ptr = hb_parptr( 1 );

   if( ptr )
   {
      const char * szModule1 = hb_parc( 2 ),
                 * szModule2 = hb_parc( 3 );

      hb_retl( szModule1 && szModule2 &&
               FILENAME_EQUAL( hb_dbgStripModuleName( szModule1 ),
                               hb_dbgStripModuleName( szModule2 ) ) );
   }
}

HB_FUNC( __DBGSENDMSG )
{
   hb_dbg_objSendMessage( hb_parnl( 1 ), hb_param( 2, HB_IT_ANY ),
                          hb_param( 3, HB_IT_ANY ), 4 );
}
