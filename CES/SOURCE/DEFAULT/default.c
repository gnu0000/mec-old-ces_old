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
 * default.c
 */

#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\dialog.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include "..\include\colid.h"
#include <stdio.h>
#include "initcat.h"
#include "default.h"
#include "dialog.h"



/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstInstall (PSZ szDatabaseName,
                           PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName,
                             PSZ pszImportDatabase)
   {
   return TRUE;
   }



/*
 * ItiDllQueryVersion
 *
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return 1;
   }


VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Default Unit Price Catalog", usMaxSize);
   *pusWindowID = DefaultUnitPriceCatS;
   }







/*
 * ItiEstTableChanged is used to tell an installable estimation method
 * that a table has changed.
 *
 * Parameters: usTable        The ID of the table that has changed.
 *
 *             ausColumns     An array from 0..usNumColumns, that
 *                            contain the IDs of the columns that
 *                            have changed.  If this parameter is
 *                            NULL, then all columns have changed.
 *
 *             usNumColumns   The number of columns in ausColumns.
 *
 *             pszKey         A pointer to a string that contains
 *                            the unique key.  The string is in the
 *                            form "ColumnName = ColumnValue
 *                            [AND ColumnName = ColumnValue]".
 */

void EXPORT ItiEstTableChanged (USHORT   usTable,
                                USHORT   ausColumns [],
                                USHORT   usNumColumns,
                                PSZ      pszKey)
   {
   }



/*
 * ItiEstItem returns a string to be used as the unit price for
 * a given standard item.  This function is usefull when the item has
 * not yet been added to the data base.
 *
 * Parameters: hheap                A heap to allocate from.
 *
 *             pszBaseDate          The base date for the price.  This
 *                                  parameter may be null iff pszJobKey
 *                                  is not NULL.  If this parameter and
 *                                  pszJobKey are supplied, pszBaseDate
 *                                  overrides the Job's base date.
 *
 *             pszJobKey            The job that the item is in.
 *                                  If the item is not in a job,
 *                                  set this parameter to NULL.
 *
 *             pszJobBreakdownKey   The job breakdown that the
 *                                  item is in.  If the item is not
 *                                  in a job, set this parameter to NULL.
 *
 *             pszStandardItemKey   The standard item to price.  This
 *                                  parameter may not be NULL.
 *
 *             pszQuantity          The quantity for this item.  If the
 *                                  quantity is unknown, use NULL.
 *
 * Return value:  A pointer to a null terminated string containing the
 * unit price.  The string is allocated from hheap.  Special return
 * values:
 *
 *    NULL:          An error occured estimating the item.
 *
 *    empty string:  Not enough information was supplied to estimate
 *                   the given item.
 *
 * Notes:  The caller is responsible for freeing the unit price from the
 * heap.
 *
 * The unit price is rounded according to the RoundingPrecision field
 * in the standard item catalog.
 */

PSZ EXPORT ItiEstItem (HHEAP hheap,
                       PSZ   pszBaseDate,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszStandardItemKey,
                       PSZ   pszQuantity)
   {
   BOOL bGotBaseDate = FALSE;
   char szTemp [1024];
   PSZ  pszPrice;

   if (pszStandardItemKey == NULL ||
       (pszJobKey == NULL && pszBaseDate == NULL))
      return NULL;

   if (pszBaseDate == NULL)
      {
      pszBaseDate = ItiEstGetJobBaseDate (hheap, pszJobKey);
      if (pszBaseDate == NULL)
         return NULL;

      bGotBaseDate = TRUE;
      }

   sprintf (szTemp,
            "SELECT UnitPrice "
            "FROM StdItemDefaultPrices "
            "WHERE BaseDate = '%s' "
            "AND StandardItemKey = %s ",
            pszBaseDate, pszStandardItemKey);

   pszPrice = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

   if (pszPrice == NULL)
      {
      if (bGotBaseDate)
         ItiMemFree (hheap, pszBaseDate);
      return NULL;
      }

   pszPrice = ItiEstRoundUnitPrice (hheap, ItiStrToDouble (pszPrice),
                                    pszStandardItemKey);

   if (bGotBaseDate)
      ItiMemFree (hheap, pszBaseDate);

   return pszPrice;
   }





MRESULT EXPORT DefaultUnitPriceCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, DefaultUnitPriceCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Default Unit Price",
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;

            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, DefaultUnitPriceCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, DefaultUnitPriceCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb
                  (WinWindowFromID (hwnd, DefaultUnitPriceCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, DefaultUnitPriceCatL);
               ItiWndBuildWindow (hwndChild, DefaultUnitPriceS);
               return 0;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT DefaultUnitPriceSProc (HWND     hwnd,
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
              // ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
              ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            //case DID_OK:
            //   break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT DefltPricesDProc (HWND    hwnd,
                                    USHORT  usMessage,
                                    MPARAM  mp1,
                                    MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         /* Disable push buttons until we fill the list box. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), FALSE); /* OK button */
         WinEnableWindow(WinWindowFromID(hwnd, 4), FALSE); /* ADD MORE button */
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, StandardItemCatL, cStandardItemKey, NULL, cStandardItemKey);

         /* Now enable the push button. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), TRUE); /* OK button */
         WinEnableWindow(WinWindowFromID(hwnd, 4), TRUE); /* ADD MORE button */
         break;

      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == StandardItemCatL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, StandardItemCatL);
            if (hwndList == NULL)
               break;

            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cStandardItemNumber);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_STDITEMNUM, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;
            } /* end command switch */
         }/* end case wm_command */
         break;

      }  /*  */
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {
   static BOOL bInited = FALSE;

   if (bInited)
      return TRUE;

   bInited = TRUE;

   if (ItiMbRegisterStaticWnd (DefaultUnitPriceCatS, DefaultUnitPriceCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (DefaultUnitPriceS, DefaultUnitPriceSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (JobDefaultUnitPriceS, DefaultUnitPriceSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (DefaultUnitPriceCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (DefltPricesD, DefltPricesD, DefltPricesDProc, hmodMe))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiEstQueryAvail (HHEAP hheap,
                              PSZ   pszStandardItemKey,
                              PSZ   pszBaseDate)
   {
   char szTemp [512];
   PSZ psz;

   if (pszBaseDate == NULL)
      pszBaseDate = UNDECLARED_BASE_DATE;

   sprintf (szTemp,
            "SELECT SIDP.BaseDate "
            "FROM StdItemDefaultPrices SIDP "
            "WHERE SIDP.StandardItemKey = %s "
            "AND SIDP.BaseDate = '%s' ",
            pszStandardItemKey, pszBaseDate);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (psz == NULL)
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }

BOOL EXPORT ItiEstQueryJobAvail (HHEAP hheap,
                                 PSZ   pszJobKey,
                                 PSZ   pszJobItemKey)
   {
   char szTemp [512];
   PSZ psz;

   sprintf (szTemp,
            "SELECT SIDP.BaseDate "
            "FROM StdItemDefaultPrices SIDP, JobItem JI, Job J "
            "WHERE SIDP.StandardItemKey = JI.StandardItemKey "
            "AND JI.JobItemKey = %s "
            "AND JI.JobKey = %s "
            "AND JI.JobKey = J.JobKey "
            "AND SIDP.BaseDate = J.BaseDate ",
            pszJobItemKey, pszJobKey);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (psz == NULL)
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }

VOID EXPORT ItiEstQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Default Unit Price", usMaxSize);
   *pusWindowID = JobDefaultUnitPriceS;
   }

