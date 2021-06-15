/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * AreaType.c
 * Mark Hampton
 *
 * This module provides the AreaType catalog window.
 */



#define  INCL_WIN
#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itibase.h"
#include "..\include\itimenu.h"
#include "..\include\itierror.h"
#include "..\include\dialog.h"
#include "areatype.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR szDllVersion[] = "1.1a0 AreaType.dll";


MRESULT EXPORT AreaTypeCatSProc (HWND     hwnd,
                             USHORT   usMessage,
                             MPARAM   mp1,
                             MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, AreaTypeCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }

            case IDM_VIEW_MENU:
               {
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Area",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, AreaTypeCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, AreaTypeCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, AreaTypeCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, AreaTypeCatL);
               ItiWndBuildWindow (hwndChild, AreaTypeS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






/* This is the window proc for the Area_Static window
 * All window procs must call ItiWndDefStaticWndProc as the
 * default window message handler.
 * This proc handles the child-window special-case as well as the
 * usual menu functions normally handled by the window proc
 */

WNDPROC EXPORT AreaTypeSProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      /* --- initializing Area_List2 is a special case. This window's --- */
      /* ---  keys come from the windows list sibling rather than the --- */
      /* ---  frame windows parent list window                        --- */

      /* --- mp1 = parent hbuf               --- */
      /* --- mp2l=parent row                 --- */
      /* --- mp2h=no autostart children querys? --- */
      case WM_INITQUERY:
         {
         HWND     hwndL1;

         /* --- manually init normal list window --- */
         hwndL1 = WinWindowFromID (hwnd, AreaTypeL1);
         WinSendMsg (hwndL1, WM_INITQUERY, mp1, mp2);

         /* --- other list window isn't yet ready--- */

         /* --- turn off auto child init --- */
         mp2 = MPFROM2SHORT (SHORT1FROMMP (mp2), TRUE);

         /* --- allow msg to continue slightly modified --- */
         return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
         }

      case WM_ITILWMOUSE:
         {
         MPARAM   MP1, MP2;
         HBUF     hbufL1;
         HWND     hwndL1, hwndL2;
         char     szArea [64];
         HHEAP    hheap;

         if (!(SHORT1FROMMP (mp2) == AreaTypeL1) ||
             !ItiWndIsSingleClick (mp2) ||
             !ItiWndIsLeftClick (mp2))
            break;

         /* --- O.K. the Area_List1 window was clicked in.         --- */
         /* --- so now we must update the query in the AreaTypeL2  --- */
         /* --- window                                             --- */

         hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         hwndL1 = WinWindowFromID (hwnd, AreaTypeL1);
         hwndL2 = WinWindowFromID (hwnd, AreaTypeL2);
         hbufL1 = (HBUF) QW (hwndL1, ITIWND_BUFFER, 0, 0, 0);

         ItiDbGetBufferRowCol (hbufL1, SHORT1FROMMP (mp1), cAreaID, szArea);
         SWL (hwnd, ITIWND_LABEL, 1, szArea);

         MP1 = MPFROMP (hbufL1);  /* --- buffer to use  --- */
         MP2 = mp1;               /* --- row clicked on in low word--- */
         WinSendMsg (hwndL2, WM_CHANGEQUERY, MP1, MP2);
         return 0;
         }

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, AreaTypeL1, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, AreaTypeL1, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, AreaTypeL1, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, AreaTypeL1), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
   }






MRESULT EXPORT AreaTypeCatDProc (HWND     hwnd,
                                 USHORT   usMessage,
                                 MPARAM   mp1,
                                 MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT AreaTypeD1Proc (HWND    hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITQUERY:
         WinShowWindow (WinWindowFromID (hwnd, DID_MODIFYCOUNTIES),
                        ItiWndGetDlgMode () != ITI_ADD);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_MODIFYCOUNTIES:
               ItiWndDoNextDialog (hwnd, 0, AreaTypeD2, ITI_CHANGE);
               break;
            }
         }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







static void UpdateAreas (HWND   hwnd,  USHORT uCtlA,
                         USHORT uCtlB, USHORT uMode)
   {
   HBUF   hbufLA, hbufLB, hbufD1;
   USHORT uLARows, uLBRows, i, uRow;
   char   szAreaTypeKey[32], szAreaKey[32], szCtyKey[32];
   char   szName [256], szQry [512];
   PSZ    pszTmpQry;
   HWND   hwndD1;

   hwndD1 = (HWND) QW (hwnd,   ITIWND_OWNERWND, 0, 0, 0);
   hbufD1 = (HBUF) QW (hwndD1, ITIWND_BUFFER,   0, 0, 0);
   hbufLA = (HBUF) QW (hwnd,   ITIWND_BUFFER,   0, uCtlA, 0);
   hbufLB = (HBUF) QW (hwnd,   ITIWND_BUFFER,   0, uCtlB, 0);

   uLARows = (UM) WinSendDlgItemMsg (hwnd, uCtlA, LM_QUERYITEMCOUNT, 0, 0);
   uLBRows = (UM) WinSendDlgItemMsg (hwnd, uCtlB, LM_QUERYITEMCOUNT, 0, 0);

   ItiDbGetBufferRowCol (hbufD1, 0, cAreaTypeKey, szAreaTypeKey);
   ItiDbGetBufferRowCol (hbufD1, 0, cAreaKey,  szAreaKey);

   switch (uMode)
      {
      case ITI_INSERT:
         pszTmpQry = " INSERT INTO AreaCounty"
                     " (AreaTypeKey, AreaKey, CountyKey, BaseDate)"
                     " VALUES"
                     " (%s, %s, %s, \""UNDECLARED_BASE_DATE"\") ";
         break;
      case ITI_DELETE:
         pszTmpQry = " DELETE FROM AreaCounty"
                     " WHERE AreaTypeKey = %s"
                     " AND   AreaKey     = %s"
                     " AND   CountyKey   = %s"
                     " AND   BaseDate    = \""UNDECLARED_BASE_DATE"\" ";
         break;
      }

   for (i=0; i<uLARows; i++)
      {
      if ((ULONG)uCtlB ==  (ULONG)WinSendDlgItemMsg (hwnd, uCtlA,
                                   LM_QUERYITEMHANDLE, MPFROMSHORT (i), 0))
         {
         WinSendDlgItemMsg (hwnd, uCtlA, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof szName), MPFROMP (szName));
         uRow = ItiDbFindRow (hbufLB, szName, cName, 0);
         ItiDbGetBufferRowCol (hbufLB, uRow, cCountyKey, szCtyKey);

         sprintf (szQry, pszTmpQry, szAreaTypeKey, szAreaKey, szCtyKey);
         ItiWndModifyDb (hwnd, szQry, uMode);
         }
      }
   }







MRESULT EXPORT AreaTypeD2Proc (HWND     hwnd,
                               USHORT   usMessage,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         ItiWndSetLBHandles (hwnd, AreaTypeD2L1, (ULONG)AreaTypeD2L1);
         ItiWndSetLBHandles (hwnd, AreaTypeD2L2, (ULONG)AreaTypeD2L2);
         break;
         }

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               UpdateAreas (hwnd, AreaTypeD2L1, AreaTypeD2L2, ITI_INSERT);
               UpdateAreas (hwnd, AreaTypeD2L2, AreaTypeD2L1, ITI_DELETE);
               WinDismissDlg (hwnd, FALSE);
               break;
               }
            case DID_DO:     /*--- <--- button (add area)    ---*/
               ItiWndMoveSelectedItem (hwnd, AreaTypeD2L1, AreaTypeD2L2);
               return 0;

            case DID_UNDO:   /*--- ---> button (remove area) ---*/
               ItiWndMoveSelectedItem (hwnd, AreaTypeD2L2, AreaTypeD2L1);
               return 0;
            }
         }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {

// -------------------------------------------------------------- 94 NOV tlb
#define  NUMBER_OF_AT_KEYS    32
#define  KEYLEN               10

   HHEAP  hhp;
   HQRY   hqry;
   PSZ    *ppszTmp, pszDatabase, pszCnt = NULL;
   USHORT usCols, usErr;
   INT    iCnt;
   CHAR   aszATKeys  [NUMBER_OF_AT_KEYS + 3] [KEYLEN];
   CHAR   szQry[256] = "";
   CHAR   szChk[128] =
            "IF EXISTS (select Deleted from %s..Area where AreaKey = 1) "
            "UPDATE %s..Area SET Deleted = NULL where AreaKey = 1 ";
   CHAR   szAT[256] =
            " SELECT DISTINCT AreaTypeKey"
            " FROM %s..AreaType"
            " WHERE AreaTypeKey"
            " NOT IN (SELECT DISTINCT AreaTypeKey"
                    " FROM %s..Area WHERE AreaKey = 1) ";
   CHAR   szFix[180] =
            "INSERT INTO"
            " %s..Area (AreaTypeKey,AreaKey,AreaID,Description)"
            " VALUES ( %s, 1, '*', 'Default all Areas') " ;

   CHAR   szACT[128] =
            " SELECT DISTINCT AreaTypeKey"
            " FROM %s..AreaCounty"
            " WHERE AreaKey != 1";

   CHAR   szAC[80] =
            " SELECT DISTINCT CountyKey"
            " FROM %s..County"
            " WHERE Deleted = NULL";

   CHAR   szACFix[256] =
            "INSERT INTO"
            " %s..AreaCounty (AreaKey,AreaTypeKey,CountyKey,BaseDate)"
            " VALUES ( 1, %s, %s, \""UNDECLARED_BASE_DATE"\" ) ";

   CHAR   szDefAT[380] =
            "IF NOT EXISTS (select AreaTypeKey from %s..AreaType where AreaTypeKey = 1) "
            " INSERT INTO"
            " %s..AreaType (AreaTypeKey,AreaTypeID,Description)"
            " VALUES (1, 'Statewide', 'Statewide default AreaType') " ;


   if (bInited)
      return TRUE;

   hhp = ItiMemCreateHeap (1024);

   /* Find which database to use. */
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

   /* -- First reset any deleted special Keys, '1', from older versions. */
   sprintf(szQry, szChk, pszDatabase, pszDatabase);
   ItiDbExecSQLStatement (hhp, szQry);

   /* -- Check for default AreaTypeKey */
   sprintf(szQry, szDefAT, pszDatabase, pszDatabase);
   ItiDbExecSQLStatement (hhp, szQry);

   /* -- Next get any missing default keys from older versions... */
   sprintf(szQry, szAT, pszDatabase, pszDatabase);
   if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &usCols, &usErr)))
      {
		ItiErrWriteDebugMessage ("areatype.c.ItiDllInitDll, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return FALSE;
      }
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &usErr)) 
      {
      /* -- ...then fill in any missing default keys from older versions. */
      sprintf (szQry, szFix, pszDatabase, ppszTmp[0]);
      ItiDbExecSQLStatement (hhp, szQry);
      ItiFreeStrArray(hhp, ppszTmp, usCols); 
      }


   /* -- Finally take care of the AreaCounty keys. */
   pszCnt=ItiDbGetRow1Col1
        ("select (SELECT count(CountyKey) FROM AreaCounty WHERE AreaKey=1 "
         "AND BaseDate = \""UNDECLARED_BASE_DATE"\" )"
         " - (SELECT count(CountyKey) FROM County ) "
         ,hhp,0,0,0);
   if (pszCnt != NULL)
      ItiStrToSHORT (pszCnt, &iCnt);

   if (iCnt <= 0)
      {
      ItiWndSetHourGlass (TRUE);

      sprintf(szQry, szACT, pszDatabase);
      if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &usCols, &usErr)))
         {
   		ItiErrWriteDebugMessage ("areatype.c.ItiDllInitDll, failed szACT ExecQuery.");
         ItiMemDestroyHeap (hhp);
         return FALSE;
         }
      aszATKeys[0][0] = '\0';
      iCnt = -1;
      while (ItiDbGetQueryRow(hqry, &ppszTmp, &usErr)) 
         {                                   /* Fill the AreaType key array. */
         if (iCnt <= NUMBER_OF_AT_KEYS)
            {
            iCnt++;
            ItiStrCpy(aszATKeys[iCnt], ppszTmp[0], KEYLEN); 
            aszATKeys[iCnt+1][0] = '\0';
            }
         ItiFreeStrArray(hhp, ppszTmp, usCols);
         }
   
      while (iCnt >= 0)
         {
         sprintf(szQry, szAC, pszDatabase);
         if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &usCols, &usErr)))
            {
   			ItiErrWriteDebugMessage ("areatype.c.ItiDllInitDll, failed szAC ExecQuery.");
            ItiMemDestroyHeap (hhp);
            return FALSE;
            }
         while (ItiDbGetQueryRow(hqry, &ppszTmp, &usErr)) 
            {
            sprintf (szQry, szACFix, pszDatabase,  aszATKeys[iCnt], ppszTmp[0] /*, pszBaseDate */ );
            ItiDbExecSQLStatement (hhp, szQry);
            ItiFreeStrArray(hhp, ppszTmp, usCols); 
            }
   
         iCnt--;
         } /* end of while (iCnt... */

      ItiWndSetHourGlass (FALSE);

      }/* end if (iCnt... */

   ItiMemDestroyHeap (hhp);
// --------------------------------------------------------------


   if (ItiMbRegisterStaticWnd (AreaTypeCatS, AreaTypeCatSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (AreaTypeS,    AreaTypeSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd   (AreaTypeCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd   (AreaTypeL1,   ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd   (AreaTypeL2,   ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd   (AreaTypeD2L1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd   (AreaTypeD2L2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (AreaTypeCatD, AreaTypeCatD,  AreaTypeCatDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (AreaTypeD1,   AreaTypeD1,    AreaTypeD1Proc,   hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (AreaTypeD2,   AreaTypeD2,    AreaTypeD2Proc,   hmodMe))
      return FALSE;

   bInited = TRUE;

   return TRUE;
   }




/*
 * ItiDllQueryVersion returns the version number of this DLL.
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return VERSION;
   }





/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "A~rea Type Catalog", usMaxSize);
   *pusWindowID = AreaTypeCatS;
   }




