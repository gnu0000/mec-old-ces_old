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
 * JobItem.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Item windows.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\dialog.h"
#include "..\include\itiest.h"
#include "..\include\itiutil.h"
#include "..\include\cesutil.h"
#include "..\include\cbest.h"
#include "..\include\itierror.h"
#include "bamsces.h"
#include "jobitem.h"
#include "menu.h"
#include "dialog.h"
#include "calcfn.h"
#include <stdio.h>
#include <string.h>


/*
 *
 * Window procedure procedure for Job Item List window.
 */

MRESULT EXPORT JobItemsSProc (HWND   hwnd,
                              USHORT usMessage,
                              MPARAM mp1,
                              MPARAM mp2)
   {
   static CHAR szJobKeyLoc[10];
   static CHAR szJobItemKeyLoc[10];

   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobItemsL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               BOOL     bSelected;
               PSZ      pszJobItemKey, pszJobKey;
               HHEAP    hheap;
               USHORT   uRow;
               HWND     hwndList;

               hwndList = WinWindowFromID (hwnd, JobItemsL);

               bSelected = ItiWndQueryActive (hwnd, JobItemsL, NULL);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);

               // ------------------------------------------------------
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Reprice...",
                                      IDM_VIEW_USER_FIRST + 1);
           
               ItiMenuGrayMenuItem   (HWNDFROMMP (mp2),
                                      IDM_VIEW_USER_FIRST + 1, FALSE); 

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST + 2);
               // ------------------------------------------------------

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Item", 
                                      IDM_VIEW_USER_FIRST + 3);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST + 4); //tlb 94dec21

               ItiMenuGrayMenuItem   (HWNDFROMMP (mp2),
                                      IDM_VIEW_USER_FIRST + 3, !bSelected);


               hheap        = (HHEAP)QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               uRow         = (UM)   QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               pszJobItemKey= (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cJobItemKey);
               pszJobKey    = (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cJobKey);

               ItiStrCpy (szJobKeyLoc, pszJobKey, sizeof szJobKeyLoc);
               ItiStrCpy (szJobItemKeyLoc, pszJobItemKey, sizeof szJobItemKeyLoc);

               ItiEstChangeViewMenu (HWNDFROMMP (mp2), hwndList, 
                                     IDM_VIEW_USER_FIRST + 4, 
                                     szJobKeyLoc, szJobItemKeyLoc, hheap);

               ItiMemFree (hheap, pszJobKey);
               ItiMemFree (hheap, pszJobItemKey);
               return 0;
               }
               break;

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_BASE_DATE, 
                                    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_SPEC_YEAR, 
                                    FALSE);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         {
         HWND hwndList;

         hwndList = WinWindowFromID (hwnd, JobItemsL);
         if (SHORT1FROMMP (mp1) >= IDM_VIEW_USER_FIRST + 4)
            {
            ItiEstProcessViewCommand (WinWindowFromID (hwnd, JobItemsL),
                                      IDM_VIEW_USER_FIRST + 4,
                                      SHORT1FROMMP (mp1));
            }
         else
            switch (SHORT1FROMMP (mp1))
               {
               case IDM_ADD:
                 ItiWndDoDialog (hwnd, JobItemsL, TRUE);
                  return 0;
                  break;

               case IDM_CHANGE:
                  ItiWndDoDialog (hwnd, JobItemsL, FALSE);
                  return 0;
                  break;

               case IDM_DELETE:
                  {
                  HWND     hwndList;
                  HHEAP    hheapKeys, hheap;
                  PSZ      *ppszSelKeys;
                  PSZ      pszJobKey, pszJobBreakdnKey, pszJIKey;
                  PSZ      *ppszLocalPNames, *ppszLocalParams;
                  USHORT   uKeys [] = {cJobKey, cJobItemKey, cJobAlternateGroupKey, cJobAlternateKey, 0};
                  USHORT   uRet, usNumRows, uRow;
                  CHAR     szJobKey[10] = "";
                  CHAR     szJobBrkdwnKey[10] = "";

                  ItiWndSetHourGlass (TRUE);

                  hwndList = WinWindowFromID(hwnd, JobItemsL);

                  hheapKeys = (HHEAP)QW (hwndList, ITIWND_HEAP, 0, 0, 0);
                  hheap     = (HHEAP)QW (hwndList, ITIWND_HEAP, 0, 0, 0);

                  ItiDbBuildSelectedKeyArray(hwndList, hheapKeys, &ppszSelKeys, uKeys);
                  uRet = ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobItemsL), ITI_DELETE);
                  if (uRet != 666)                          /* User canceled the delete */
                     {
                     uRow         = (UM)   QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
                     pszJobKey    = (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cJobKey);
                     ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);
                     pszJobBreakdnKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cJobBreakdownKey);
                     ItiStrCpy(szJobBrkdwnKey, pszJobBreakdnKey, sizeof szJobBrkdwnKey);

                     for (usNumRows =0; ppszSelKeys[usNumRows] != NULL; usNumRows++)
                        {
                        pszJIKey = ItiDbGetZStr(ppszSelKeys[usNumRows],1);
                        DelCBEForJobItem (hheapKeys, szJobKey, pszJIKey);
                        }
                     DoJobReCalcForJIDelete(hwnd, ppszSelKeys);
                     ItiFreeStrArray(hheapKeys, ppszSelKeys, usNumRows);
                     DoReCalcJobBreakJobFunds (hwnd);

                     if ((szJobKey[0] != '\0') && (szJobBrkdwnKey[0] != '\0'))
                        {
                        ppszLocalPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
                        ppszLocalParams = ItiStrMakePPSZ (hheap, 3, szJobKey, szJobBrkdwnKey,  NULL);

                        ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszLocalPNames, ppszLocalParams, 0);
                        ItiDBDoCalc (CalcJobItemChgJob, ppszLocalPNames, ppszLocalParams, 0);
                        ItiDbUpdateBuffers ("Job");
                        ItiDbUpdateBuffers ("JobBreakdown");
                        }
                     }
                  ItiWndSetHourGlass (FALSE);

                  return 0;
                  break;
                  }

               case (IDM_VIEW_USER_FIRST + 3):
                  ItiWndBuildWindow (hwndList, JobItemS);
                  return 0;
                  break;

               case (IDM_VIEW_USER_FIRST + 1):  //94jun20
                  {
                  HWND   hwndList;
                  PSZ    pszJobKey, pszJobBreakdnKey;
                  PSZ    *ppszPNames, *ppszParams;
                  CHAR   szJobKey[10] = "";
                  CHAR   szJobBrkdwnKey[10] = "";
                  HHEAP  hheap;
                  USHORT usRes;

                  hwndList = WinWindowFromID(hwnd, JobItemsL);
                  hheap    = (HHEAP)QW (hwndList, ITIWND_HEAP, 0, 0, 0);

                  pszJobKey    = (PSZ)  QW (hwndList, ITIWND_DATA, 0, 1, cJobKey);
                  ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);
                  pszJobBreakdnKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, 1, cJobBreakdownKey);
                  ItiStrCpy(szJobBrkdwnKey, pszJobBreakdnKey, sizeof szJobBrkdwnKey);

                  if ((szJobKey[0] != '\0') && (szJobBrkdwnKey[0] != '\0'))
                     {
                     ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
                     ppszParams = ItiStrMakePPSZ (hheap, 3, szJobKey, szJobBrkdwnKey,  NULL);

                     usRes = WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "CAUTION: All Job Items for the current job breakdown will"
                                " have their estimation methods reset by the system,"
                                " and thus reset their unit prices, is that what you want done?",
                                "Auto Selection of Esimation Methods",
                                0, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION);

                     if (usRes == MBID_YES)
                        {
                        ItiWndSetHourGlass (TRUE);

                        CalcAutoSelectEstimateMethods
                           (hwnd, ppszPNames, ppszParams);
                        ItiDbUpdateBuffers ("JobItem");
                        ItiDbUpdateBuffers ("JobBreakdown");
                        ItiDbUpdateBuffers ("Job");

                        ItiWndSetHourGlass (FALSE);
                        }
                     }

                  return 0;
                  break;
                  }
               }
            }  
         break;

      case WM_ITILWMOUSE:
         {
         USHORT uRow;
         HWND   hwndList;
         HHEAP  hheapList;
         PSZ    pszMethod;

         uRow   = SHORT1FROMMP (mp1);

         if (!(SHORT1FROMMP (mp2) == JobItemsL) || 
             ItiWndIsSingleClick (mp2) ||
             !ItiWndIsLeftClick (mp2))
            break;

         hwndList = WinWindowFromID (hwnd, JobItemsL);
         hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
         pszMethod = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cEstimationMethodAbbreviation);

         ItiEstBuildMethodWnd (hwndList, pszMethod);
         break;
         }
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



/*
 * Window procedure for the Job Item window.
 */

MRESULT EXPORT JobItemSProc (HWND   hwnd,
                            USHORT usMessage,
                            MPARAM mp1,
                            MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               return 0;
               break;

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_BASE_DATE, 
                                    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_SPEC_YEAR, 
                                    FALSE);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




static void EXPORT DoUnitPriceRecalc (HWND hwndDlg)
   {
   HBUF     hbuf, hbufList;
   HWND     hwndList;
   HWND     hwndCombo;
   char     szQuantity [60];
   char     szJobKey [30];
   char     szJobBreakdownKey [30];
   char     szBaseDate [60];
   char     szStandardItemKey [30];
   char     szBasis [63];
   USHORT   uActive;
   HHEAP    hheap;
   PSZ      pszPrice;

   hwndList = WinWindowFromID (hwndDlg, StandardItemCatL);
   hwndCombo = WinWindowFromID (hwndDlg, DID_METHOD);
   if (hwndList == NULL || hwndCombo == NULL)
      return;

   uActive = SHORT1FROMMR (WinSendMsg (hwndCombo, LM_QUERYSELECTION, 
                                        MPFROMSHORT (LIT_FIRST), 0));
   if (uActive == LIT_NONE)
      return;

   WinSendMsg (hwndCombo, LM_QUERYITEMTEXT, 
               MPFROM2SHORT (uActive, sizeof szBasis),
               MPFROMP (szBasis));

   hbuf     = (HBUF)  QW (hwndDlg,  ITIWND_OWNERSTATICBUFFER, 0, 0, 0);
   hbufList = (HBUF)  QW (hwndList, ITIWND_BUFFER, 0, 0, 0);
   hheap    = (HHEAP) QW (hwndDlg,  ITIWND_HEAP, 0, 0, 0);

   if (hbuf == NULL || hbufList == NULL || hheap == NULL)
      return;

   WinQueryDlgItemText (hwndDlg, DID_QUANTITY, sizeof szQuantity, szQuantity);

   if (ITIDB_VALID != ItiDbGetBufferRowCol (hbuf, 0, cJobKey, szJobKey))
      return;
   if (ITIDB_VALID != ItiDbGetBufferRowCol (hbuf, 0, cJobBreakdownKey, szJobBreakdownKey))
      return;
   if (ITIDB_VALID != ItiDbGetBufferRowCol (hbuf, 0, cBaseDate, szBaseDate))
      return;
   
   if ((uActive = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0)) == -1)
      return;

   if (ITIDB_VALID != ItiDbGetBufferRowCol (hbufList, uActive, 
                                            cStandardItemKey,
                                            szStandardItemKey))
      return;

   /* if we've gotten this far, it's a miracle */
   pszPrice = ItiEstEstimateItem (hheap, szBasis, szBaseDate, szJobKey, 
                                  szJobBreakdownKey, szStandardItemKey, 
                                  szQuantity);
   if (pszPrice != NULL)
      {
      WinSetDlgItemText (hwndDlg, DID_UNIT_PRICE, pszPrice);
      ItiMemFree (hheap, pszPrice);
      }
   }



static PFNWP NextComboProc;
MRESULT EXPORT ComboHookProc (HWND hwnd, USHORT usMessage, MPARAM mp1, MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_SETFOCUS:
         if (!SHORT1FROMMP (mp2))
            {
            switch (WinQueryWindowUShort (WinQueryWindow (hwnd, QW_PARENT, 
                                          FALSE), QWS_ID))
               {
               case DID_METHOD:
                  DoUnitPriceRecalc (WinQueryWindow (WinQueryWindow (hwnd, 
                                     QW_PARENT, FALSE), QW_PARENT, FALSE));
                  break;
               
               case DID_ALT_GROUP:
                  break;
               }
            }
         break;

      }
   return NextComboProc (hwnd, usMessage, mp1, mp2);
   }


static PFNWP NextQuantityProc;
MRESULT EXPORT QuantityHookProc (HWND hwnd, USHORT usMessage, MPARAM mp1, MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_SETFOCUS:
         if (!SHORT1FROMMP (mp2))
            DoUnitPriceRecalc (WinQueryWindow (hwnd, QW_PARENT, FALSE));
         break;
      }
   return NextQuantityProc (hwnd, usMessage, mp1, mp2);
   }



/*
 * This function:
 * 1> looks at the job alt grp dlg and finds its selection
 * 2> if no selection, job alt combo is cleared >ret
 * 3> the job alt combo is filled with alts corresponding to the group >ret
 *
 */

void EXPORT  RefillJobAltCombo (HWND hwnd)
   {
   HBUF   hbufAltGrp, hbufAlt;
   USHORT i, j;
   USHORT uRows;
   char   szGrpId  [63];
   char   szGrpKey [63];
   char   szTemp    [250];
//   char   szAltId  [65];
//   char   szAltKey [65];

//   WinSendDlgItemMsg (hwnd, DID_ALT, LM_DELETEALL, 0, 0);
//   WinSetDlgItemText (hwnd, DID_ALT, "");

   WinQueryDlgItemText (hwnd, DID_ALT_GROUP, sizeof szGrpId, szGrpId);
   if (!szGrpId[0])
      return;

   WinSendDlgItemMsg (hwnd, DID_ALT, LM_DELETEALL, 0, 0);
   WinSetDlgItemText (hwnd, DID_ALT, "");

   hbufAltGrp = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, JobItemEditAltGroup, 0);

   i = ItiDbFindRow (hbufAltGrp, szGrpId, cJobAlternateGroupID, 0);
   ItiDbGetBufferRowCol (hbufAltGrp, i, cJobAlternateGroupKey, szGrpKey);

   hbufAlt = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, JobItemEditAlt, 0);

   uRows = (UM) ItiDbQueryBuffer (hbufAlt, ITIDB_NUMROWS, 0);

   for (j=i=0; i<uRows; i++)
      {
      ItiDbGetBufferRowCol (hbufAlt, i, cJobAlternateGroupKey, szTemp);
      if (!strcmp (szTemp, szGrpKey))
         {
         ItiDbGetBufferRowCol (hbufAlt, i, cJobAlternateID, szTemp);
         WinSendDlgItemMsg (hwnd, DID_ALT, LM_INSERTITEM, (MPARAM)LIT_END,
                                                          (MPARAM)szTemp);

         /*--- Added 4/17/92.  All listboxes and comboboxes have thier ---*/
         /*--- corresponding buffer rows stored in the item handle     ---*/
         /*--- this is so the shell can find the right row when        ---*/
         /*--- replacing keys                                          ---*/
         WinSendDlgItemMsg (hwnd, DID_ALT, LM_SETITEMHANDLE,
                                     MPFROMSHORT (j++), MPFROMSHORT(i));
         }
      }
   }


void EXPORT  RefillEstMethodCombo (HWND hwnd)
   {
   HWND   hwndList;
   HHEAP  hheapList;
   HBUF   hbufMethod, hbufG;

   USHORT uMethods, uListRow;
   USHORT i, j;
   PSZ    pszSIC;
   static char   szBaseDate[60], szMethod[60], szSIC[15] = " 1 ";

   WinSendDlgItemMsg (hwnd, DID_METHOD, LM_DELETEALL, 0, 0);
   WinSetDlgItemText (hwnd, DID_METHOD, "");

   hbufMethod = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, JobItemEditMethod, 0);
   hbufG      = (HBUF) QW (hwnd, ITIWND_OWNERSTATICBUFFER, 0, 0, 0);
   uMethods   = (UM) ItiDbQueryBuffer (hbufMethod, ITIDB_NUMROWS, 0);


   if (ITIDB_VALID != ItiDbGetBufferRowCol (hbufG, 0, cBaseDate, szBaseDate))
      return;

   if (NULL == (hwndList = WinWindowFromID (hwnd, StandardItemCatL)))
      return;

   if (NULL == (hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0)))
      return;

   uListRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

   pszSIC = (PSZ) QW (hwndList, ITIWND_DATA, 0, uListRow, cStandardItemKey);
   ItiStrCpy(szSIC, pszSIC, sizeof szSIC);

   for (j=i=0; i<uMethods; i++)
      {
      ItiDbGetBufferRowCol (hbufMethod, i, cItemEstimationMethodID, szMethod);

      if (!ItiEstQueryMethod (hheapList, szSIC, szBaseDate, szMethod))
         continue;

      WinSendDlgItemMsg (hwnd, DID_METHOD, LM_INSERTITEM, (MPARAM)LIT_END,
                                                         (MPARAM)szMethod);

      WinSendDlgItemMsg (hwnd, DID_METHOD, LM_SETITEMHANDLE,
                                    MPFROMSHORT (j++), MPFROMSHORT(i));
      }

   if (pszSIC != NULL) 
      ItiMemFree (hheapList, pszSIC);

   WinSendDlgItemMsg (hwnd, DID_METHOD, LM_SELECTITEM, 0, (MPARAM) TRUE);
   }


/* selects row in method combo box
 * only call during initialization of dlg box 
 * in change mode.
 *
 */

void EXPORT  SelectEstMethodRow (HWND hwnd)
   {
   HHEAP hheapList;
   HWND  hwndList;
   PSZ   pszMethod;
   USHORT uListRow, i;
   char  szTxt[60];

   if (NULL == (hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 0, 0, 0)))
      return;

   if (NULL == (hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0)))
      return;

   uListRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   pszMethod = (PSZ) QW (hwndList, ITIWND_DATA, 0, uListRow, cItemEstimationMethodID);

   i = (UM) WinSendDlgItemMsg (hwnd, DID_METHOD, LM_QUERYITEMCOUNT, 0, 0);
   for (; i > 0; i--)
      {
      WinSendDlgItemMsg (hwnd, DID_METHOD, LM_QUERYITEMTEXT, MPFROM2SHORT (i-1, 256), szTxt);
      if (strcmp (szTxt, pszMethod))
         continue;

      WinSendDlgItemMsg (hwnd, DID_METHOD, LM_SELECTITEM, MPFROMSHORT (i-1), (MPARAM) TRUE);

      if (strcmp (pszMethod, "Ad-Hoc"))
         {
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE + DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE), FALSE);
         }
      else
         {
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE + DID_STOFFSET), TRUE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE), TRUE);
         }

      break;
      }

   if (pszMethod != NULL)
      ItiMemFree (hheapList, pszMethod);
   }



MRESULT EXPORT JobItemDProc (HWND     hwnd,
                             USHORT   usMessage,
                             MPARAM   mp1,
                             MPARAM   mp2)
   {
   static PSZ pszKey;
   static BOOL bDoCalc = FALSE;
   static CHAR szInitEstMethod[30];
   static CHAR szJobItemKey[10];
   static CHAR szItemNumber [14] = "";

   switch (usMessage)
      {
      case WM_ITICALC:
    //     if (bDoCalc)
    //        {
    //        /* if the user clicked OK and a new item is being added */
    //        if (pszKey != NULL)
    //           {
    //           CopyCBEInfoForNewJobItem(hwnd, pszKey);
    //           pszKey = NULL;
    //           }
    //        }
    //     // test
    //     {
    //     HWND  hwndCalc = NULL;
    //     HHEAP hheap;
    //     CHAR  szJBKey[32] = "";
    //     CHAR  szJKey[32] = "";
    //     PSZ   pszJobBreakdnKey, pszJKey;
    //     PSZ  *ppszLocalPNames, *ppszLocalParams;
    //
    //
    //     hheap = ItiMemCreateHeap (1024);
    //     hwndCalc = QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
    //
    //     pszJKey =        (PSZ) QW (hwndCalc, ITIWND_DATA, 0, 0, cJobKey);
    //     ItiStrCpy(szJKey, pszJKey, sizeof szJKey);
    //
    //     pszJobBreakdnKey = (PSZ) QW (hwndCalc, ITIWND_DATA, 0, 0, cJobBreakdownKey);
    //     ItiStrCpy(szJBKey, pszJobBreakdnKey, sizeof szJBKey);
    //
    //     ppszLocalPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
    //     ppszLocalParams = ItiStrMakePPSZ (hheap, 3, szJKey, szJBKey,  NULL);
//  //       ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszLocalPNames, ppszLocalParams, 0);
//  //       ItiDBDoCalc (CalcJobItemChgJob, ppszLocalPNames, ppszLocalParams, 0);
    //
    //     ItiMemDestroyHeap (hheap);
    //
         ItiDbUpdateBuffers ("Job");
         ItiDbUpdateBuffers ("JobBreakdown");
    //     }
    
         bDoCalc = FALSE;
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "JobItem") == 0)
            {
            /* This takes care of setting the JobItemKey for an Edit/Add. */
            hheap = QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            ItiStrCpy(szJobItemKey, pszKey, sizeof szJobItemKey);
            }
         return 0;
         break;
         }

      case WM_INITDLG:
         {
         szJobItemKey[0] = '\0';
         szJobItemKey[1] = '\0';
         szItemNumber[0] = '\0';

         bDoCalc = FALSE;
         WinEnableWindow (WinWindowFromID (hwnd, DID_ITEM_NUMBER+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_DESCRIPTION+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_ALT_GROUP+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_ALT+DID_STOFFSET), FALSE);
//csp e-01         WinSendDlgItemMsg (hwnd, DID_ITEM_NUMBER, EM_SETREADONLY, 
//csp e-01                            MPFROMSHORT (1), 0);
         WinSendDlgItemMsg (hwnd, DID_DESCRIPTION, EM_SETREADONLY, 
                            MPFROMSHORT (1), 0);
         WinSendDlgItemMsg (hwnd, DID_UNIT, EM_SETREADONLY, 
                            MPFROMSHORT (1), 0);

         /* ADDED 17 Aug 93 */
         /* Disable push buttons until we fill the list box. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), FALSE); /* OK button       */
         WinEnableWindow(WinWindowFromID(hwnd, 4), FALSE); /* Add More button */
         }
         break;

      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == StandardItemCatL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT uRow;

            hwndList = WinWindowFromID (hwnd, StandardItemCatL);
            if (hwndList == NULL)
               break;
            
            uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemNumber);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_ITEM_NUMBER, pszTemp);
               ItiStrCpy (szItemNumber, pszTemp, sizeof szItemNumber);
               ItiMemFree (hheap, pszTemp);
               }

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cShortDescription);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_DESCRIPTION, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cCodeValueID);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_UNIT, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }

            RefillEstMethodCombo (hwnd);

            }
         break;

      case WM_CHAR:
         {
         HWND    hwndFocus, hwndList;
         HBUF    hbufList;
         HHEAP   hheap;
         USHORT  usId, i, usRows, usMatchCol;
         PEDT    pedt;
         PSZ     pszTmp;
         CHAR    szItemMask [15], szTmp [255];
         USHORT  usVKey;

         hwndFocus = WinQueryFocus (HWND_DESKTOP, FALSE);
         usId      = WinQueryWindowUShort (hwndFocus, QWS_ID);

         if (usId != DID_ITEM_NUMBER)
            break;
          
         usVKey = (USHORT) SHORT2FROMMP (mp2);

         switch (usVKey)
            {
            case VK_SHIFT:
            case VK_ALT:
            case VK_CTRL:
               return 0;

            case VK_TAB:
            case VK_BACKTAB:
               if (ItiStrLen (szItemNumber) > 0)
                  {
                  WinSetDlgItemText (hwnd, DID_ITEM_NUMBER, szItemNumber);
                  WinSetFocus (HWND_DESKTOP,
                               WinWindowFromID (hwnd, DID_QUANTITY));
                  return 0;
                  }
               else
                  {
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "An Item Number has not been selected.",
                                "Dialog Data Error", 0, MB_OK | MB_APPLMODAL);
                  WinSetFocus (HWND_DESKTOP,
                               WinWindowFromID (hwnd, DID_ITEM_NUMBER));
                  return 0;
                  }

            default:
               break;
            }

         hwndList  = WinWindowFromID (hwnd, StandardItemCatL);
         hbufList  =  (HBUF) QW (hwndList, ITIWND_BUFFER, 0, 0, 0);
         usMatchCol = 0;

         pedt = (PEDT) QW (hwndList, ITIWND_DATAEDT, usMatchCol, 0, 0);

         WinQueryWindowText (hwndFocus, sizeof szItemMask, szItemMask);

         if (ItiStrLen (szItemMask) == 0)
            break;

         ItiStrCat (szItemMask, "*", sizeof szItemMask);

         usRows = (UM) QW (hwndList, ITIWND_NUMROWS, 0, 0, 0);

         for (i = 0; i < usRows; i++)
            {
            ItiDbGetBufferRowCol (hbufList, i, pedt->uIndex, szTmp);

            if (ItiStrMatchWildCard (szItemMask, szTmp))
               {
               SW (hwndList, ITIWND_SELECTION, ITI_UNSELECT, 0, 0xFFFF);
               SW (hwndList, ITIWND_SELECTION, ITI_SELECT, i, i);
               SW (hwndList, ITIWND_ACTIVE, 0, i, 0);

               hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               if (hheap == NULL)
                  break;

               pszTmp = (PSZ) QW (hwndList, ITIWND_DATA, 0, i, cShortDescription);
               if (pszTmp != NULL)
                  {
                  WinSetDlgItemText (hwnd, DID_DESCRIPTION, pszTmp);
                  ItiMemFree (hheap, pszTmp);
                  }

               pszTmp = (PSZ) QW (hwndList, ITIWND_DATA, 0, i, cCodeValueID);
               if (pszTmp != NULL)
                  {
                  WinSetDlgItemText (hwnd, DID_UNIT, pszTmp);
                  ItiMemFree (hheap, pszTmp);
                  }

               pszTmp = (PSZ) QW (hwndList, ITIWND_DATA, 0, i, cStandardItemNumber);
               if (pszTmp != NULL)
                  {
                  if (ItiStrCmp (pszTmp, szItemNumber))
                     RefillEstMethodCombo (hwnd);

                  ItiStrCpy (szItemNumber, pszTmp, sizeof szItemNumber);
                  ItiMemFree (hheap, pszTmp);
                  }

               break;
               }
            }
         }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT  mr;
               HBUF     hbuf;
               HHEAP    hheap, hheapList, hheapList2;
               HWND     hwndList, hwndList2, hwndList3, hwndCombo, hwnd2 = NULL; 
               USHORT   uRow, usUpDateID, uActive, uSameKey, uRet=0;
               USHORT   uKeys [] = {cJobKey, cJobItemKey, 0};
               PSZ      pszWinStandardItemKey, pszJobKey, pszJobItemKey;
               PSZ      pszDlgStandardItemKey, psz, pszItemEstMethodKey;
               PSZ      pszQuantity;
               CHAR     szQty[31];
               CHAR     szEstMethod[20], szTemp [250], szTmp[250] = "";
               DOUBLE   dPrice;
               CHAR szJobKey[10];
               CHAR szJBKey[10];
               CHAR szDlgStdItemKey[10];
               PSZ  pszJCBEKey = NULL;
               PSZ  pszJobBreakdnKey = NULL;
               PSZ  *ppszLocalPNames, *ppszLocalParams;

               WinSetDlgItemText (hwnd, DID_ITEM_NUMBER, szItemNumber);

               hwndList = WinWindowFromID(hwnd, StandardItemCatL);
               pszKey = NULL;
               if (hwndList == NULL)
                  break;

               // hheap = ItiMemCreateHeap (MAX_HEAP_SIZE);
               hheap = QW (hwnd, ITIWND_HEAP, 0, 0, 0);


               /* -- Get the JobItem's QUANTITY. */
               WinQueryDlgItemText (hwnd, DID_QUANTITY, sizeof szQty, szQty);
               pszQuantity = &szQty[0];

               /* -- Test to see if an item number has been selected. */
               if (ItiStrLen (szItemNumber) == 0)
                  {
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "An Item Number has not been selected.",
                                "Dialog Data Error", 0, MB_OK | MB_APPLMODAL);
                  WinSetFocus (HWND_DESKTOP,
                               WinWindowFromID (hwnd, DID_ITEM_NUMBER));
                  return 0;
                  }

               /* -- Test if an ESTIMATION METHOD was selected. */
               hwndCombo = WinWindowFromID (hwnd, DID_METHOD);
               uActive = SHORT1FROMMR (WinSendMsg (hwndCombo, LM_QUERYSELECTION, 
                                        MPFROMSHORT (LIT_FIRST), 0));
               if (uActive == LIT_NONE)
                  {
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "You must first select an estimation method.",
                                "Dialog Data Error", 0, MB_OK | MB_APPLMODAL);
                  return 0;
                  }
               else
                  {
                  WinSendMsg (hwndCombo, LM_QUERYITEMTEXT, 
                              MPFROM2SHORT (uActive, sizeof szEstMethod),
                              MPFROMP (szEstMethod));

                  /* -- Set the Method key value. */
                  if (0 == ItiStrCmp (szEstMethod, "Ad-Hoc"))
                     pszItemEstMethodKey = "1000000";
                  else if (0 == ItiStrCmp (szEstMethod, "Regress"))
                     pszItemEstMethodKey = "1000002";
                  else if (0 == ItiStrCmp (szEstMethod, "Average"))
                     pszItemEstMethodKey = "1000003";
                  else if (0 == ItiStrCmp (szEstMethod, "CBEst"))
                     pszItemEstMethodKey = "1000001";
                  else   /* if (0 == ItiStrCmp (szEstMethod, "Default")) */
                     pszItemEstMethodKey = "1000004";

                  }/* end of else clause IF(uActive... */



            // BSR 930065 start
               WinQueryDlgItemText (hwnd, DID_ALT_GROUP, sizeof szTemp, szTemp);
               if (szTemp[0] == '\0')
                  {
                  //WinSetDlgItemText(hwnd, DID_ALT_GROUP, "Non-Alt");
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "You must select an Alternate Group, or Non-Alt.",
                                "Dialog Data Error", 0, MB_OK | MB_APPLMODAL);
                  return 0;
                  }
   
               WinQueryDlgItemText (hwnd, DID_ALT, sizeof szTemp, szTemp);
               if (szTemp[0] == '\0')
                  {
                  //WinSetDlgItemText(hwnd, DID_ALT, "Non-Alt");
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                                "You must select an Alternate, or Non-Alt.",
                                "Dialog Data Error", 0, MB_OK | MB_APPLMODAL);
                  return 0;
                  }
            // BSR 930065 end


               ItiWndSetHourGlass (TRUE);

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               pszJobItemKey = &szJobItemKey[0];

               hwndList3 = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
               pszJobKey = (PSZ) QW (hwndList3, ITIWND_DATA, 0, 0, cJobKey);
               ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);


               /* Code by super kludge */
               /* BSR 920923-4101: round the unit price. mdh */
               hwndList2 = (HWND) QW(hwnd, ITIWND_OWNERWND, 0, 0, 0);
               hheapList2 = (HHEAP) QW (hwndList2, ITIWND_HEAP, 0, 0, 0);
               uRow = (UM) QW (hwndList2, ITIWND_ACTIVE, 0, 0, 0);
               pszWinStandardItemKey = (PSZ) QW (hwndList2, ITIWND_DATA, 0, uRow, cStandardItemKey);

               if (szJobItemKey[0] == '\0')
                  {
                  pszJobItemKey = (PSZ) QW (hwndList2, ITIWND_DATA, 0, uRow, cJobItemKey);
                  if (pszJobItemKey == NULL)
                     {
                     hbuf = (HBUF) QW (hwnd, ITIWND_OWNERSTATICBUFFER, 0, 0, 0);
                     if ((hbuf != NULL) &&
                         (ITIDB_VALID != ItiDbGetBufferRowCol (hbuf, 0, cJobItemKey, szJobItemKey)) )
                        {
                        ItiErrWriteDebugMessage("MISSING JobItemKey in JobItem dialog box.");
                        }
                     }
                  else
                     ItiStrCpy(szJobItemKey, pszJobItemKey, sizeof szJobItemKey);
                  }

		sprintf(szTemp,
              "**** In JobItem.c  JobItemKey: %s, JobKey: %s, pszJobItemKey: %s ",
			     szJobItemKey, szJobKey, pszJobItemKey );
		ItiErrWriteDebugMessage(szTemp);


               uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               pszDlgStandardItemKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemKey);
               ItiStrCpy(szDlgStdItemKey, pszDlgStandardItemKey, sizeof szDlgStdItemKey);

               WinQueryDlgItemText (hwnd, DID_UNIT_PRICE, sizeof szTemp, szTemp);
               dPrice = ItiStrToDouble (szTemp);
               psz = ItiEstRoundUnitPrice (hheap, dPrice, szDlgStdItemKey);
               WinSetDlgItemText (hwnd, DID_UNIT_PRICE, psz);
               if (NULL != psz)
                  ItiMemFree (hheap, psz);
               psz = NULL;


               /* if this is a change in Job Items we need the JobItemKey,  */
               /* StandardItemKey, JobKey, in order to delete the associated*/
               /* CostBasedEstimates, CostSheets, JobCrewUsed, Laborers,    */
               /* Equipment, Material, etc                                  */

               usUpDateID =  ItiWndGetDlgMode();

               bDoCalc = TRUE;

               if (usUpDateID == ITI_CHANGE)
                  {
                  /* Compare the StandardItemKey (selected in JobItemsL) with */
                  /* the StandardItemKey (selected in the StandardItemCatL)   */
                  /* This indicates whether the user is changing the Standard */
                  /* Item to another StandardItem, meaning the CBE's etc.     */
                  /* should be copied.                                        */

                  uSameKey = ItiStrCmp(pszWinStandardItemKey, pszDlgStandardItemKey);

                  if ((uSameKey != 0)
                      || (0 != ItiStrCmp(szEstMethod, szInitEstMethod)) ) 
                     {         /* ie. the StdItem or method changed. */

// ////////////////////////////////////////
// Replace the next block of code with something like the following:
//   IF StdItemKey item has CBEs
//     Copy SICBEs to JICBEs where no name confict exists
//   ELSE
//     UnMark any existing JobItemCBEs 
// 
// ////////////////////////////////////////
                     DelCBEForJobItem (hheap, szJobKey, szJobItemKey);

                     if (0 == ItiStrCmp(szEstMethod, "CBEst") )
                        CopyCBEForJobItem (hheap, szJobKey, szJobItemKey, szDlgStdItemKey);
// ////////////////////////////////////////
                     }
                  }/* endif usUpDateID == ITI_CHANGE */

               if (usUpDateID == ITI_ADD)       /* NEW SECTION */
                  {
                  if (pszKey != NULL)
                     {  /* ie. new JobItemKey value */
                     ItiErrWriteDebugMessage("Comment: Next, add CBE For JobItem: ");
                     // if (0 == ItiStrCmp(szEstMethod, "CBEst") )
                     if (0 == ItiStrCmp(pszItemEstMethodKey, "1000001") )
                        CopyCBEForJobItem (hheap, szJobKey, szJobItemKey, szDlgStdItemKey);
                     pszKey = NULL;
                     }
                  }/* endif usUpDateID == ITI_ADD */


               if (hwnd2 == NULL)
                  {
                  hwnd2 = QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
                  pszJobBreakdnKey = (PSZ) QW (hwnd2, ITIWND_DATA, 0, 0, cJobBreakdownKey);
                  ItiStrCpy(szJBKey, pszJobBreakdnKey, sizeof szJBKey);
                  }

               RecalcJobItem (hheap,
                              szJobKey,
                              szJBKey,
                              szJobItemKey,
                              pszQuantity,
                              szDlgStdItemKey,
                              pszItemEstMethodKey);

               ppszLocalPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
               ppszLocalParams = ItiStrMakePPSZ (hheap, 3, szJobKey, szJBKey,  NULL);

               CalcJobItemChgJobBrkdwn (hheap, ppszLocalPNames, ppszLocalParams);
               CalcJobItemChgJob (hheap, ppszLocalPNames, ppszLocalParams);

               ItiDbUpdateBuffers ("Job");
               ItiDbUpdateBuffers ("JobBreakdown");

// /                  }/* endif usUpDateID == ITI_CHANGE */

              // mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
              // ItiWndSetHourGlass (TRUE);

// /               if (pszKey != NULL)
// /                  {
// /                  if (0 == ItiStrCmp(szEstMethod, "CBEst") )
// /                     CopyCBEForJobItem (hheap, szJobKey, szJobItemKey, szDlgStdItemKey);
// /           //  CopyCBEInfoForNewJobItem(hwnd, pszKey);
// /               //   ItiMemFree (hheap, pszKey);
// /                  pszKey = NULL;
// /                  }

               if (SHORT1FROMMP (mp1) == DID_ADDMORE)
                  {
                  WinSetDlgItemText (hwnd, DID_DURATION, "1");
                  WinSetDlgItemText (hwnd, DID_DELAY, "0");
// BSR 930065 
                  WinSetDlgItemText(hwnd, DID_ALT_GROUP, "Non-Alt");
                  WinSetDlgItemText(hwnd, DID_ALT, "Non-Alt");
// BSR 930065 
                  if (!mr)
                     {
                     szItemNumber[0] = '\0';
                     SW (hwndList, ITIWND_SELECTION, ITI_UNSELECT, 0, 0xFFFF);
                     RefillEstMethodCombo (hwnd);
                     }
                  }

            //   if (NULL != pszJobKey)
            //      ItiMemFree(hheapList2, pszJobKey);
               if (NULL != pszDlgStandardItemKey)
                  ItiMemFree(hheapList,  pszDlgStandardItemKey);
               if (NULL != pszWinStandardItemKey)
                  ItiMemFree(hheapList2, pszWinStandardItemKey);

               ItiWndSetHourGlass (SHORT1FROMMP (mp1) == DID_OK);

            //   if (NULL != hheap)
            //      ItiMemDestroyHeap(hheap);

               return mr;
               }
               break;


            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;

      case WM_QUERYDONE:
         {
         HWND  hwndCtl;
         HWND   hwndCombo;
         USHORT uActive;

         /* set hooks */
         hwndCtl = WinWindowFromID (hwnd, DID_METHOD);
         ItiWndAddHook (hwndCtl, ComboHookProc, &NextComboProc);
         hwndCtl = WinWindowFromID (hwnd, DID_QUANTITY);
         ItiWndAddHook (hwndCtl, QuantityHookProc, &NextQuantityProc);

         if (ITI_ADD == ItiWndGetDlgMode ())
            {
            USHORT uItem;

            RefillEstMethodCombo (hwnd);
            uItem = SHORT1FROMMR (WinSendDlgItemMsg (hwnd, DID_METHOD, LM_SEARCHSTRING,
                                  MPFROM2SHORT (LSS_CASESENSITIVE, LIT_FIRST),
                                  MPFROMP ("Default")));
                                        
            WinSendDlgItemMsg (hwnd, DID_METHOD, LM_SELECTITEM, 
                               MPFROMSHORT (uItem), 
                               MPFROMSHORT (TRUE));
            WinSendDlgItemMsg (hwnd, DID_ALT_GROUP, LM_SELECTITEM,
                               MPFROMSHORT (0), 
                               MPFROMSHORT (TRUE));
            WinSendDlgItemMsg (hwnd, DID_ALT, LM_SELECTITEM,
                               MPFROMSHORT (0), 
                               MPFROMSHORT (TRUE));
            ItiWndActivateRow (hwnd, StandardItemCatL, cStandardItemKey, hwnd,  0);
            WinSetDlgItemText (hwnd, DID_DURATION, "1");
            WinSetDlgItemText (hwnd, DID_DELAY, "0");
            WinSetDlgItemText (hwnd, DID_UNIT_PRICE, "0");
// BSR 930062
            ItiWndActivateRow (hwnd, DID_ALT_GROUP, cJobAlternateGroupKey, hwnd, 0);
   // test  RefillJobAltCombo (hwnd);
            ItiWndActivateRow (hwnd, DID_ALT,       cJobAlternateKey,      hwnd, 0);
// BSR 930062
            }
         else
            {
            ItiWndActivateRow (hwnd, DID_ALT_GROUP,  cJobAlternateGroupKey,    hwnd,  0);
            RefillJobAltCombo (hwnd);
            ItiWndActivateRow (hwnd, DID_ALT,        cJobAlternateKey,         hwnd,  0);
            ItiWndActivateRow (hwnd, StandardItemCatL, cStandardItemKey, hwnd,  0);
            RefillEstMethodCombo (hwnd);
//            ItiWndActivateRow (hwnd, DID_METHOD,     cItemEstimationMethodKey, hwnd,  0);
            SelectEstMethodRow (hwnd);
            WinQueryDlgItemText (hwnd, DID_ITEM_NUMBER,
                                 sizeof szItemNumber, szItemNumber);
            }

         WinEnableWindow (WinWindowFromID (hwnd, DID_ITEM_NUMBER+DID_STOFFSET), TRUE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_DESCRIPTION+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT+DID_STOFFSET), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_ITEM_NUMBER), TRUE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_DESCRIPTION), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT), FALSE);

         /* -- Set value for the initial Est Method. */
         hwndCombo = WinWindowFromID (hwnd, DID_METHOD);
         uActive = SHORT1FROMMR (WinSendMsg (hwndCombo, LM_QUERYSELECTION, 
                                 MPFROMSHORT (LIT_FIRST), 0));
         WinSendMsg (hwndCombo, LM_QUERYITEMTEXT, 
                     MPFROM2SHORT (uActive, sizeof szInitEstMethod),
                     MPFROMP (szInitEstMethod));
         if (szInitEstMethod[0] == '\0')
            {
            }

         /* ADDED 17 Aug 93 */
         /* Now enable the push buttons. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), TRUE);
         WinEnableWindow(WinWindowFromID(hwnd, 4), TRUE);
         break;
         }
      case WM_CONTROL:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ALT_GROUP:
               RefillJobAltCombo (hwnd);
               break;

            case DID_METHOD:
               {
               char szMethod [20];

               WinQueryDlgItemText (hwnd, DID_METHOD, sizeof szMethod, szMethod);

               if (strcmp (szMethod, "Ad-Hoc"))
                  {
                  WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE + DID_STOFFSET), FALSE);
                  WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE), FALSE);
                  }
               else
                  {
                  WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE + DID_STOFFSET), TRUE);
                  WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT_PRICE), TRUE);
                  }
               }
               break;
            }
         }
         break;
      case WM_CHILDQUERYDONE:
         WinEnableWindow (WinWindowFromID (hwnd, DID_DESCRIPTION), FALSE);
         WinEnableWindow (WinWindowFromID (hwnd, DID_UNIT), FALSE);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }/* End of Function JobItemDProc */





int EXPORT  CopyCBEInfoForNewJobItem (HWND hwnd, PSZ pszJobItemKey)
   {
   PSZ          pszStandardItemKey, pszJobKey;
   HHEAP        hheap, hheapList, hheapStatic;
   HWND         hwndStatic, hwndList;
   USHORT       uRow;
	CHAR         szTemp[512] = "";

   hwndList = WinWindowFromID(hwnd, StandardItemCatL);
   if (hwndList == NULL)
      return 1;

   uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
   pszStandardItemKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemKey);

	if (pszJobItemKey == NULL)
		{
		sprintf(szTemp,
			     "Error Condition: NULL JobItemKey in CopyCBEInfoForNewJobItem; "
              "StdItem: %s, JobKey: %s ",
			     pszStandardItemKey, pszJobKey );
		ItiErrWriteDebugMessage(szTemp);
      return 1;
		}

   hwndStatic = QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
   hheapStatic = (HHEAP) QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);
   pszJobKey = (PSZ) QW (hwndStatic, ITIWND_DATA, 0, 0, cJobKey);
   
   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

 //  CreateCBEForNewJobItem (hheap, hwnd, pszJobKey, pszJobItemKey, pszStandardItemKey);
   CopyCBEForJobItem (hheap, pszJobKey, pszJobItemKey, pszStandardItemKey);

   if (ItiEstIsActive (hheap, pszJobKey, pszJobItemKey, EST_COST_BASED))
      DoJobItemCalc(hheap, pszJobKey, pszJobItemKey);

   ItiMemFree (hheapList, pszStandardItemKey);
   ItiMemFree (hheapStatic, pszJobKey);

   return 0;
   }

