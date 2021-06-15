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
 * ZoneType.c
 * Mark Hampton
 *
 * This module provides the Zone Type catalog.
 */



#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itibase.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "ZoneType.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR szDllVersion[] = "1.1a0 ZoneType.dll";

/* This is the window proc for the ZonesS window
 * All window procs must call ItiWndDefStaticWndProc as the
 * default window message handler.
 * This proc handles the child-window special-case as well as the
 * usual menu functions normally handled by the window proc
 */


WNDPROC EXPORT ZonesSProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      /* --- initializing ZonesL2 is a special case. This window's --- */
      /* ---  keys come from the windows list sibling rather than the --- */
      /* ---  frame windows parent list window                        --- */


      /* --- mp1 = parent hbuf               --- */
      /* --- mp2l=parent row                 --- */
      /* --- mp2h=no autostart children querys? --- */
      case WM_INITQUERY:
         {
         HWND     hwndL1;

         /* --- manually init normal list window --- */
         hwndL1 = WinWindowFromID (hwnd, ZonesL1);
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
         char     szZones [64];
         HHEAP    hheap;

         if (!(SHORT1FROMMP (mp2) == ZonesL1) ||
             !ItiWndIsSingleClick (mp2) ||
             !ItiWndIsLeftClick (mp2))
            return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);

         /* --- O.K. the ZonesL1 window was clicked in.         --- */
         /* --- so now we must update the query in the ZonesL2  --- */
         /* --- window                                             --- */

         hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         hwndL1 = WinWindowFromID (hwnd, ZonesL1);
         hwndL2 = WinWindowFromID (hwnd, ZonesL2);
         hbufL1 = (HBUF) QW (hwndL1, ITIWND_BUFFER, 0, 0, 0);

         ItiDbGetBufferRowCol (hbufL1, SHORT1FROMMP (mp1), cZoneID, szZones);
         SWL (hwnd, ITIWND_LABEL, 1, szZones);

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

               bSelected = ItiWndQueryActive (hwnd, ZonesL1, NULL);
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
               ItiWndDoDialog (hwnd, ZonesL1, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ZonesL1, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ZonesL1), ITI_DELETE);
               break;
            }
         break;

      }
   return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
   }


MRESULT EXPORT ZoneTypeCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, ZoneTypeCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Zone",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ZoneTypeCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ZoneTypeCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ZoneTypeCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, ZoneTypeCatL);
               ItiWndBuildWindow (hwndChild, ZonesS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }











static void CreateChildRelations (HHEAP hheap,
                                  PSZ   pszZoneTypeKey)
   {
   char szTemp [1024] = "";


   sprintf (szTemp, "IF NOT EXISTS (select ZoneTypeKey from ZoneType "
                                  " where ZoneTypeKey = 1) "
                    "INSERT INTO ZoneType "
                    "(ZoneTypeKey, ZoneTypeID, Description, Deleted) "
                    "VALUES "
                    "(1,' Statewide','Default zone type.', NULL)");
   ItiDbExecSQLStatement (hheap, szTemp);



   sprintf (szTemp, "IF NOT EXISTS (select ZoneTypeKey from Zone "
                                  " where ZoneKey = 1 and ZoneTypeKey = %s ) "
                    "INSERT INTO Zone "
                    "(ZoneTypeKey, ZoneKey, ZoneID, Description, Deleted) "
                    "VALUES "
                    "(%s,1,' Default','Default zone.', NULL)",
                    pszZoneTypeKey, pszZoneTypeKey);

   ItiDbExecSQLStatement (hheap, szTemp);


   sprintf (szTemp, "INSERT INTO ZoneMap "
                    "(ZoneTypeKey, ZoneKey, MapKey, BaseDate) "
                    "VALUES "
                    "(%s,1,1,'%s')",
                    pszZoneTypeKey, UNDECLARED_BASE_DATE);

   ItiDbExecSQLStatement (hheap, szTemp);

   }/* End of Function CreateChildRelations */




MRESULT EXPORT ZoneTypeDProc (HWND    hwnd,
                                       USHORT   usMessage,
                                       MPARAM   mp1,
                                       MPARAM   mp2)


   {
   static PSZ pszZoneTypeKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "ZoneType") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszZoneTypeKey = ItiStrDup (hheap, (PSZ) mp2);
            }

         return 0;
         }
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_ADDMORE:
            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap;

               pszZoneTypeKey = NULL;
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               if (pszZoneTypeKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  CreateChildRelations (hheap, pszZoneTypeKey);
                  ItiMemFree (hheap, pszZoneTypeKey);
                  }
               return mr;
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }















MRESULT EXPORT ZoneDProc (HWND     hwnd,
                                   USHORT   usMessage,
                                   MPARAM   mp1,
                                   MPARAM   mp2)
   {
   static PSZ pszZoneTypeKey;

   switch (usMessage)
      {
      case WM_INITQUERY:
         WinShowWindow (WinWindowFromID (hwnd, DID_MODIFYZONEMAPS),
                        ItiWndGetDlgMode () != ITI_ADD);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_MODIFYZONEMAPS:
               ItiWndDoNextDialog (hwnd, 0, ZoneMapD, ITI_CHANGE);
               break;

            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap;
    CHAR szTemp[200] = "SELECT 'In the spot.' ";

               pszZoneTypeKey = NULL;
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               if (pszZoneTypeKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               //   CreateChildRelations (hheap, pszZoneTypeKey);

   ItiDbExecSQLStatement (hheap, szTemp);

                  ItiMemFree (hheap, pszZoneTypeKey);
                  }
               return mr;
               }
               break;
            }
         }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }












static void UpdateZones (HWND   hwnd,  USHORT uCtlA,
                         USHORT uCtlB, USHORT uMode)
   {
   HBUF   hbufLA, hbufLB, hbufD1;
   USHORT uLARows, uLBRows, i, uRow;
   char   szZoneTypeKey[32], szZoneKey[32], szMapKey[32];
   char   szMapID [256], szQry [512];
   PSZ    pszTmpQry;
   HWND   hwndD1;

   hwndD1 = (HWND) QW (hwnd,  ITIWND_OWNERWND, 0, 0, 0);
   hbufD1 = (HBUF) QW (hwndD1,ITIWND_BUFFER,   0, 0, 0);
   hbufLA = (HBUF) QW (hwnd,  ITIWND_BUFFER,   0, uCtlA, 0);
   hbufLB = (HBUF) QW (hwnd,  ITIWND_BUFFER,   0, uCtlB, 0);

   uLARows = (UM) WinSendDlgItemMsg (hwnd, uCtlA, LM_QUERYITEMCOUNT, 0, 0);
   uLBRows = (UM) WinSendDlgItemMsg (hwnd, uCtlB, LM_QUERYITEMCOUNT, 0, 0);

   ItiDbGetBufferRowCol (hbufD1, 0, cZoneTypeKey, szZoneTypeKey);
   ItiDbGetBufferRowCol (hbufD1, 0, cZoneKey,  szZoneKey);

   switch (uMode)
      {
      case ITI_INSERT:
         pszTmpQry = " INSERT INTO ZoneMap"
                     " (ZoneTypeKey, ZoneKey, MapKey, BaseDate)"
                     " VALUES"
                     " (%s, %s, %s, \""UNDECLARED_BASE_DATE"\") ";
         break;
      case ITI_DELETE:
         pszTmpQry = " DELETE FROM ZoneMap"
                     " WHERE ZoneTypeKey = %s"
                     " AND   ZoneKey     = %s"
                     " AND   ZoneKey != 1"
                     " AND   MapKey   = %s"
                     " AND   BaseDate    = \""UNDECLARED_BASE_DATE"\" ";
         break;
      }

   for (i=0; i<uLARows; i++)
      {
      if ((ULONG)uCtlB ==  (ULONG)WinSendDlgItemMsg (hwnd, uCtlA,
                                   LM_QUERYITEMHANDLE, MPFROMSHORT (i), 0))
         {
         WinSendDlgItemMsg (hwnd, uCtlA, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof szMapID), MPFROMP (szMapID));
         uRow = ItiDbFindRow (hbufLB, szMapID, cMapID, 0);
         ItiDbGetBufferRowCol (hbufLB, uRow, cMapKey, szMapKey);

         sprintf (szQry, pszTmpQry, szZoneTypeKey, szZoneKey, szMapKey);
         ItiWndModifyDb (hwnd, szQry, uMode);
         }
      }
   }












MRESULT EXPORT ZoneMapDProc (HWND    hwnd,
                                        USHORT   usMessage,
                                        MPARAM   mp1,
                                        MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         ItiWndSetLBHandles (hwnd, MapCountyL, (ULONG)MapCountyL);
         ItiWndSetLBHandles (hwnd, MapCat2L, (ULONG)MapCat2L);
         break;
         }

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               UpdateZones (hwnd, MapCountyL, MapCat2L, ITI_INSERT);
               UpdateZones (hwnd, MapCat2L, MapCountyL, ITI_DELETE);
               WinDismissDlg (hwnd, FALSE);
               break;
               }
            case DID_DO:     /*--- <--- button (add area)    ---*/
               ItiWndMoveSelectedItem (hwnd, MapCountyL, MapCat2L);
               return 0;

            case DID_UNDO:   /*--- ---> button (remove area) ---*/
               ItiWndMoveSelectedItem (hwnd, MapCat2L, MapCountyL);
               return 0;
            }
         break;
         }
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
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
   ItiStrCpy (pszBuffer, "~Zone Type Catalog", usMaxSize);
   *pusWindowID = ZoneTypeCatS;
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (ZoneTypeCatS, ZoneTypeCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (ZonesS, ZonesSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (ZonesL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (ZonesL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (ZoneTypeCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MapCountyL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MapCat2L, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (ZoneTypeD, ZoneTypeD, ZoneTypeDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (ZoneD, ZoneD, ZoneDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (ZoneMapD, ZoneMapD, ZoneMapDProc, hmodMe))
      return FALSE;


   return TRUE;
   }




