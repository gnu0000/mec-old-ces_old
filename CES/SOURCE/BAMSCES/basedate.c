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
 * BaseDate.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the declare base date and update base date commands.
 */


#define INCL_WIN
#define INCL_DOSPROCESS
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\colid.h"
#include "..\include\cesutil.h"
#include "bamsces.h"
#include "basedate.h"
#include "dialog.h"
#include <stdio.h>
#include <process.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include "job.h"

static HHEAP hheapTemp = NULL;
static PSZ   pszBaseDate;
static PSZ   pszLongTextKey;

#define BASEDATELENGTH 25
#define NUM_OF_BDS     30
static  CHAR aszBaseDates   [ NUM_OF_BDS+1 ] [BASEDATELENGTH];
static  BOOL abBaseDateFlag [ NUM_OF_BDS+1 ];


MRESULT EXPORT UpdateBaseDateProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpParent;

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (WinQueryWindow (hwnd, QW_PARENT, 0),
                            &swpParent);

         swpMe.x = swpParent.x + swpParent.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpParent.y + swpParent.cy / 2 - swpMe.cy / 2;

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               PSZ      pszJobKey;
               MRESULT  mr;
               HHEAP    hheap;

               pszJobKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
               hheap   = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               ItiWndSetHourGlass (TRUE);
//             930120-4101 1/25/93 mdh: added the next line to recalc zones.
               PopulateJobZoneTable (hheap, pszJobKey);

               /* reprice cost sheets - update labor, equip, material prices */
               DoJobCostSheetReCalc (pszJobKey);

/**************^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^********/
/**************|||||||||||||||||recalc cost sht|||||||||||||||||||********/
/*************************************************************************/

               NewRecalcJob (pszJobKey);
               ItiWndSetHourGlass (FALSE);

               ItiMemFree (hheap, pszJobKey);

               return mr;
               }
               break;
            }
         break;

      case WM_QUERYDONE:
         WinSetWindowText (hwnd, "Update Job Base Date");
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


void EXPORT DoUpdateBaseDate (HWND  hwnd)
   {
   if (hwnd == NULL || !ItiCesIsJobWindow (hwnd, 0))
      return;

   ItiWndSetDlgMode (ITI_CHANGE);
   ItiWndBuildWindow (hwnd, UpdateBaseDate);
   ItiWndSetDlgMode (ITI_OTHER);
   }





MRESULT EXPORT DeclareBaseDateProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpParent;

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (WinQueryWindow (hwnd, QW_PARENT, 0),
                            &swpParent);

         swpMe.x = swpParent.x + swpParent.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpParent.y + swpParent.cy / 2 - swpMe.cy / 2;

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         WinSetDlgItemText (hwnd, DID_BASEDATE, pszBaseDate);
         }
         break;

      case WM_NEWKEY:
         {
         if (ItiStrCmp (PVOIDFROMMP (mp1), "LongText") == 0)
            {
            pszLongTextKey = ItiStrDup (hheapTemp, PVOIDFROMMP (mp2));
            }
         }
         break;

      case WM_QUERYDONE:
         WinSetWindowText (hwnd, "Declare Base Date");
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT DoDeclareBaseDateStatusProc (HWND   hwnd,
                                            USHORT usMessage,
                                            MPARAM mp1,
                                            MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         char szTemp [60];
         char szTemp2 [60];
         SWP   swpMe;
         SWP   swpParent;

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (WinQueryWindow (hwnd, QW_PARENT, 0),
                            &swpParent);

         swpMe.x = swpParent.x + swpParent.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpParent.y + swpParent.cy / 2 - swpMe.cy / 2;

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);

         WinQueryDlgItemText (hwnd, DID_BASEDATE, sizeof szTemp, szTemp);
         sprintf (szTemp2, szTemp, (PSZ) PVOIDFROMMP (mp2));
         WinSetDlgItemText (hwnd, DID_BASEDATE, szTemp2);
         }
         break;

      case WM_CLOSE:
         WinDestroyWindow (hwnd);
         return 0;
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





static BOOL EXPORT MakeBaseDate (HHEAP hheap, 
                          PSZ   pszBaseDate, 
                          PSZ   pszLongTextKey, 
                          HWND  hwndStatus);


void EXPORT DoPurgeUnRefBaseDates (HWND hwnd)
   {
 //  HHEAP hheapTemp = NULL;
   CHAR szBDQry[440] =
           " select COUNT(*) "
           " from BaseDate "
            UNREF_BD
           ;

   HQRY hqry;
   CHAR szMsg[200] = "";
   CHAR szTemp[550] = "";
   PSZ  pszCount, *ppszBaseDates;
   INT  ixPBD = 0;
   BOOL bResult, bDone = FALSE;
   BOOL bProblem = FALSE;
   USHORT uCols, uErr, usCnt, usCntIdx, usRc = 0;


   hheapTemp = ItiMemCreateHeap (2000);
   if (hheapTemp == NULL)
      {
      WinMessageBox (HWND_DESKTOP, hwnd,
                 //    "ERROR: No more system resources, hheap == NULL in DoPurgeUnRefBaseDates",
                    " Pruge of unreferenced BaseDates currently disabled.",
                     pglobals->pszAppName, 0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
      return;
      }

     ItiWndSetHourGlass (TRUE);
   // if (bBaseDateListMade == FALSE)
   //    {
   //    ItiDbExecSQLStatement (hheapTemp, szBDQry);
   //    bBaseDateListMade = TRUE;
   //    }

   pszCount = ItiDbGetRow1Col1(szBDQry, hheapTemp, 0, 0, 0);

   ItiStrCpy (szMsg,"About to Purge ", sizeof szMsg);
   ItiStrCat (szMsg, pszCount, sizeof szMsg);
   ItiStrCat (szMsg, " unreferenced base dates. ", sizeof szMsg);

   usRc = WinMessageBox (HWND_DESKTOP, hwnd, 
            szMsg,
            pglobals->pszAppName, 0, MB_OKCANCEL | MB_MOVEABLE);
   
   if (usRc == MBID_CANCEL)
      {
      ItiErrWriteDebugMessage("--- Notice: Basedate Purge canceled.");
      return;
      }
   else
      ItiErrWriteDebugMessage(szMsg);
   
   ItiWndSetHourGlass (TRUE);


   /* -- Build basedate list. */
   ItiStrCpy (szTemp,
           " select DISTINCT BaseDate from BaseDate "  UNREF_BD
           , sizeof szTemp);

   if (!(hqry = ItiDbExecQuery(szTemp, hheapTemp, 0, 0, 0, &uCols, &uErr)))
      {
      return;
      }

   for (usCnt=0; usCnt< NUM_OF_BDS ; usCnt++)
      {
      //aszBaseDates[usCnt][0] = '\'';
      aszBaseDates[usCnt][0] = '\0';
      abBaseDateFlag[usCnt] = FALSE;
      }

   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszBaseDates, &uErr)) 
      {
      if (usCnt < NUM_OF_BDS)
         {
         ItiStrCpy (aszBaseDates[usCnt], ppszBaseDates[0], BASEDATELENGTH);
         ItiStrCat (aszBaseDates[usCnt], "' ", BASEDATELENGTH);
         usCnt++;
         aszBaseDates[usCnt][0] = '\0';
         }
      }


   ItiMbQueryQueryText (0, ITIRID_DELETE, PurgeBasedateD, szTemp, sizeof szTemp);
   ItiDbExecSQLStatement (hheapTemp, szTemp);

for (usCntIdx = 0; usCntIdx < usCnt; usCntIdx++)
   {
   ItiWndSetHourGlass (FALSE);  //blink
   ItiWndSetHourGlass (TRUE);

   if (usCntIdx < NUM_OF_BDS)
      {
      for (ixPBD = PurgeBasedateD_MIN; ixPBD <= PurgeBasedateD_MAX; ixPBD++)
         {
         ItiMbQueryQueryText (0, ITIRID_DELETE, ixPBD, szTemp, sizeof szTemp);
         ItiStrCat (szTemp, aszBaseDates[usCntIdx], sizeof szTemp);

         if ((ixPBD == PurgeBasedateD_08) || (ixPBD == PurgeBasedateD_09))
            ItiStrCat (szTemp, " ) ", sizeof szTemp);


         if ( (bProblem == TRUE) && (ixPBD == PurgeBasedateD_18) )
            {
            /* Skip the BaseDate table marking, query PurgeBasedateD_18. */
            }
         else
            {
   if ((ixPBD != PurgeBasedateD_08) && (ixPBD != PurgeBasedateD_09)
      && (!abBaseDateFlag[usCntIdx]) )
     /* BUG, ALL data in PEMETHAvg and PEMETHReg is deleted in purge */
     /* if (!abBaseDateFlag[usCntIdx]) */
               bResult = ItiDbExecSQLStatement (hheapTemp, szTemp);
            }

         if (bResult && (ixPBD < PurgeBasedateD_18))
            {
            if (abBaseDateFlag[usCntIdx])
               {
               if (ixPBD < PurgeBasedateD_12)
                  { /* -- No Msg for empty parametric tables. */
                  ItiStrCpy (szMsg,
                     "Could not Purge basedate with: \n ", sizeof szMsg);
                  ItiStrCat (szMsg, aszBaseDates[usCntIdx], sizeof szMsg);
                  WinMessageBox (HWND_DESKTOP, hwnd, szMsg,
                     pglobals->pszAppName,0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
                  ItiWndSetHourGlass (TRUE);
                  }
               abBaseDateFlag[usCntIdx] = TRUE;
               }
            bProblem = TRUE;
            } /* end if (!bResult... */

         } /* end of for (ixPBD...*/
      }/* close of if (usCntIdx... then clause */
   else
      bProblem = TRUE;

   }/* end of for (usCntIdx...*/

   ItiMbQueryQueryText (0, ITIRID_UPDATE, BasedateStat, szTemp, sizeof szTemp);
   if (!ItiDbExecSQLStatement (hheapTemp, szTemp))
     {
     bDone = TRUE;
     } /* end if (!ItiDbExec... */

   ItiMbQueryQueryText (0, ITIRID_UPDATE, BasedateStat2, szTemp, sizeof szTemp);
   if (!ItiDbExecSQLStatement (hheapTemp, szTemp))
     {
     bDone = bDone && TRUE;
     } /* end if (!ItiDbExec... */

   if (bDone && !bProblem)
     {
     ItiStrCpy (szMsg, "Purge of the ", sizeof szMsg);
     ItiStrCat (szMsg, pszCount, sizeof szMsg);
     ItiStrCat (szMsg, " unreferenced base dates finished. ", sizeof szMsg);
     WinMessageBox (HWND_DESKTOP, hwnd, szMsg,
                    pglobals->pszAppName, 0, MB_OK | MB_MOVEABLE);
     ItiWndSetHourGlass (FALSE);
     }/* if (bDone */

    //  }/* end else clause */


   /////////////////////////
   if (NULL != pszCount)
      ItiMemFree(hheapTemp, pszCount);


   if (NULL != hheapTemp)
      ItiMemDestroyHeap (hheapTemp);

   ItiWndSetHourGlass (FALSE);
   return;
   }/* End of Function DoPurgeUnRefBaseDates */





void EXPORT DoDeclareBaseDate (HWND hwnd)
   {
   char   szDate [50] = "";
   HWND   hwndModeless;
   QMSG   qmsg;

   sprintf (szDate, "%.2d/%.2d/%.4d %.2d:%.2d", 
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour,
            (USHORT) pglobals->pgis->minutes);

   pszLongTextKey = NULL;
   pszBaseDate = szDate;
   hheapTemp = ItiMemCreateHeap (2048);

   if (hheapTemp == NULL)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "Bad error -- hheap == NULL in DoDeclareBaseDate",
                     pglobals->pszAppName, 0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
      return;
      }

   ItiWndSetDlgMode (ITI_ADD);
   ItiWndBuildWindow (hwnd, DeclareBaseDate);
   ItiWndSetDlgMode (ITI_OTHER);

   if (pszLongTextKey == NULL)
      {
      if (NULL != hheapTemp)
         ItiMemDestroyHeap (hheapTemp);
      return;
      }

   hwndModeless = WinLoadDlg (HWND_DESKTOP, hwnd, DoDeclareBaseDateStatusProc, 
                              0, DeclareBaseDateStatus, szDate);
   
   if (hwndModeless == NULL)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "Not enough memory to declare base date."
                     "  Try the command again after closing some applications.",
                     pglobals->pszAppName, 0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
      if (NULL != hheapTemp)
         ItiMemDestroyHeap (hheapTemp);
      return;
      }
                                 
   WinUpdateWindow (hwndModeless);
   ItiWndSetHourGlass (TRUE);

   if (!MakeBaseDate (hheapTemp, szDate, pszLongTextKey, hwndModeless))
      {
      ItiWndSetHourGlass (FALSE);
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "Not enough memory to declare base date."
                     "  Try the command again after closing some applications.",
                     pglobals->pszAppName, 0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
      WinDestroyWindow (hwndModeless);
      if (NULL != hheapTemp)
         ItiMemDestroyHeap (hheapTemp);
      return;
      }

   /* Get and dispatch messages. */
   while (WinIsWindow (pglobals->habMainThread, hwndModeless) && 
          WinGetMsg (pglobals->habMainThread, &qmsg, NULL, 0, 0))
      {
      if (qmsg.hwnd != NULL)
         WinDispatchMsg (pglobals->habMainThread, &qmsg);
      else
         {
         /* process WM_SEMx messages */
         }
      }

   ItiWndSetHourGlass (FALSE);
   if (NULL != hheapTemp)
      ItiMemDestroyHeap (hheapTemp);
   }




typedef struct
   {
   HHEAP hheap;
   PSZ   pszBaseDate;
   PSZ   pszLongTextKey;
   HWND  hwndStatus;
   } BASEDATEINFO; /* bdi */

typedef BASEDATEINFO FAR *PBASEDATEINFO; /* pbdi */
   

void FAR _cdecl _loadds MakeBaseDateThread (PBASEDATEINFO pbdi);

#define BASEDATE_STACKSIZE 16386



static BOOL EXPORT MakeBaseDate (HHEAP hheap, 
                          PSZ   pszBaseDate, 
                          PSZ   pszLongTextKey, 
                          HWND  hwndStatus)
   {
   PBASEDATEINFO  pbdi;

   pbdi = ItiMemAlloc (hheap, sizeof (BASEDATEINFO), 0);
   if (pbdi == NULL)
      return FALSE;

   pbdi->hheap = hheap;
   pbdi->pszBaseDate = ItiStrDup (hheap, pszBaseDate);
   pbdi->pszLongTextKey = ItiStrDup (hheap, pszLongTextKey);
   pbdi->hwndStatus = hwndStatus;

   if (-1 == _beginthread (MakeBaseDateThread, NULL, BASEDATE_STACKSIZE, pbdi))
      return FALSE;
//   MakeBaseDateThread (pbdi);
   return TRUE;
   }


static USHORT EXPORT DoIt (PBASEDATEINFO pbdi)
   {
   char   szTemp [800] = "";
   HQRY   hqry = NULL;
   PSZ    *ppszMIK;
   USHORT usCnt, uErr, uCols, usErrorCount = 0;
#define NUM_OF_MIKS 40
#define KEY_SIZE     9
   CHAR   aszMIK [NUM_OF_MIKS + 2] [KEY_SIZE + 1];

   usCnt = 0;
   while (usCnt <= NUM_OF_MIKS)
      {
      aszMIK[usCnt][0] = '\0';
      aszMIK[usCnt][1] = '\0';
      usCnt++;
      }

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO BaseDate (BaseDate, Comment) VALUES ('"
                    , sizeof szTemp);
   ItiStrCat (szTemp, pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp,  "', ", sizeof szTemp);    
   ItiStrCat (szTemp,  pbdi->pszLongTextKey, sizeof szTemp);
   ItiStrCat (szTemp, " ) ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      {
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame,
                     "Could not insert into the BaseDate table.",
                     "Create Base Date", 0, MB_OK | MB_MOVEABLE);
      return 1;
      }

   /* copy all records with an undeclared base date to the new base date */
   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO AreaCounty "
                    "(BaseDate, AreaKey, AreaTypeKey, CountyKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp,  "', AreaKey, AreaTypeKey, CountyKey FROM AreaCounty "
                    " WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);
           
   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;



   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO EquipmentRate "
                    "(BaseDate, Deleted, EquipmentKey, OvertimeRate, Rate, ZoneKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', Deleted, EquipmentKey, OvertimeRate, Rate, ZoneKey "
                    "FROM EquipmentRate "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;


   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO LaborerWageAndBenefits "
                    "(BaseDate, DavisBaconOvertimeRate, DavisBaconRate, "
                    "Deleted, LaborerKey, NonDavisBaconOvertimeRate, "
                    "NonDavisBaconRate, ZoneKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', DavisBaconOvertimeRate, DavisBaconRate, "
                    "Deleted, LaborerKey, NonDavisBaconOvertimeRate, "
                    "NonDavisBaconRate, ZoneKey "
                    "FROM LaborerWageAndBenefits "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;



   /* -- Build MajorItemKey list. */
   ItiStrCpy (szTemp,
              "/* Declaring BaseDate, prep MajItm */ "
              " SELECT DISTINCT MajorItemKey"
              " FROM MajorItemPriceRegression"
              " ORDER BY MajorItemKey "
              , sizeof szTemp);

   hqry = ItiDbExecQuery(szTemp, pbdi->hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      // skip this table, write error msg.
      ItiErrWriteDebugMessage("Failed to get HQRY in basedate.c.MajorItemPriceRegression");
      }
   else
      {              /* Fill the MIK array */
      usCnt = 0;
      while (ItiDbGetQueryRow(hqry, &ppszMIK, &uErr)) 
         {
         if (usCnt < NUM_OF_MIKS)
            {
            ItiStrCpy (aszMIK[usCnt], ppszMIK[0], KEY_SIZE);
            usCnt++;
            aszMIK[usCnt][0] = '\0';
            ItiFreeStrArray (pbdi->hheap, ppszMIK, uCols);
            }
         }/* end while */

      usCnt = 0;
      while ((usCnt < NUM_OF_MIKS) && (aszMIK[usCnt][0] != '\0')) 
         {
         ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                 "INSERT INTO MajorItemPriceRegression "
                 "(BaseDate, MajorItemKey, MedianQuantity, MedianQuantityUnitPrice, QuantityAdjustment) "
                 "SELECT '", sizeof szTemp);
         ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
         ItiStrCat (szTemp, "', MajorItemKey, MedianQuantity, MedianQuantityUnitPrice, QuantityAdjustment "
                 " FROM MajorItemPriceRegression "
                 " WHERE MajorItemKey = ", sizeof szTemp);
         ItiStrCat (szTemp, aszMIK[usCnt], sizeof szTemp);
         ItiStrCat (szTemp, " AND BaseDate=(select MAX(BaseDate)"
                                 " from MajorItemPriceRegression"
                                 " where MajorItemKey = ", sizeof szTemp);
         ItiStrCat (szTemp, aszMIK[usCnt], sizeof szTemp);
         ItiStrCat (szTemp, " ) ", sizeof szTemp);

         if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
            usErrorCount++;
         usCnt++;
         }/* end while... */

      /* -- Now set the reserved unDeclared base date. */
      ItiStrCpy (szTemp,
                 " /* Declaring BaseDate */"
                 " DELETE FROM MajorItemPriceRegression"
                 " WHERE BaseDate='"UNDECLARED_BASE_DATE"' "
                 , sizeof szTemp);
      if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
         usErrorCount++;

      ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
              " INSERT INTO MajorItemPriceRegression"
              " (BaseDate, MajorItemKey, MedianQuantity,"
              " MedianQuantityUnitPrice, QuantityAdjustment)"
              " SELECT DISTINCT '"UNDECLARED_BASE_DATE"', MajorItemKey,"
              " MedianQuantity, MedianQuantityUnitPrice, QuantityAdjustment"
              " FROM MajorItemPriceRegression "
              " WHERE BaseDate=(select MAX(BaseDate)"
                               " from MajorItemPriceRegression) ", sizeof szTemp);
      if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
         usErrorCount++;

      }/* end else clause */
         /////////////////////////////////

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemPriceVariable "
                    "(BaseDate, MajorItemKey, QualitativeVariable) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', MajorItemKey, QualitativeVariable "
                    "FROM MajorItemPriceVariable "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ////// Special non-null list insurance
   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemPriceVariable "
                    "(BaseDate, MajorItemKey, QualitativeVariable) "
                    "SELECT DISTINCT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', MajorItemKey, 0 FROM MajorItem ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;
   //////

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemPriceAdjustment "
                    "(BaseDate, MajorItemKey, PriceAdjustment, QualitativeValue, QualitativeVariable) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', MajorItemKey, PriceAdjustment, QualitativeValue, QualitativeVariable "
                    "FROM MajorItemPriceAdjustment "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;


   ////// Special non-null list insurance
   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemPriceAdjustment "
                    "(BaseDate, MajorItemKey, PriceAdjustment, QualitativeValue, QualitativeVariable) "
                    "SELECT DISTINCT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', MajorItemKey, 0.0, '0', 0 FROM MajorItem ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;
   /////

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemQuantityAdjustment "
                    "(BaseDate, MajorItemKey, QualitativeVariable, QuantityAdjustment, VariableValue) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', MajorItemKey, QualitativeVariable, QuantityAdjustment, VariableValue "
                    "FROM MajorItemQuantityAdjustment "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemQuantityRegression "
                    "(BaseDate, BaseCoefficient, CrossSectionAdjustment, "
                    "DepthAdjustment, MajorItemKey, StructureAreaAdjustment, "
                    "StructureBaseAdjustment, StructureLengthAdjustment, "
                    "StructureWidthAdjustment, WidthAdjustment) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', BaseCoefficient, CrossSectionAdjustment, "
                    "DepthAdjustment, MajorItemKey, StructureAreaAdjustment, "
                    "StructureBaseAdjustment, StructureLengthAdjustment, "
                    "StructureWidthAdjustment, WidthAdjustment "
                    "FROM MajorItemQuantityRegression "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO MajorItemQuantityVariable "
                    "(BaseDate,MajorItemKey,QualitativeVariable) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',MajorItemKey,QualitativeVariable "
                    "FROM MajorItemQuantityVariable "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);
   
   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;


   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
            "INSERT INTO MaterialPrice "
            "(BaseDate, Deleted,MaterialKey,UnitPrice,ZoneKey,UnitType) "
            "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "', Deleted,MaterialKey,UnitPrice,ZoneKey,UnitType "
            "FROM MaterialPrice "
            "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);
   
   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO StandardItemPEMETHAverage "
                    "(BaseDate, ImportDate,StandardItemKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',ImportDate,StandardItemKey "
                    "FROM StandardItemPEMETHAverage "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO StandardItemPEMETHRegression "
                    "(BaseDate,ImportDate,StandardItemKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',ImportDate,StandardItemKey "
                    "FROM StandardItemPEMETHRegression "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
            "INSERT INTO StdItemDefaultPrices "
            "(BaseDate,Deleted,Description,StandardItemKey,UnitPrice,UnitType) "
            "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',Deleted,Description,StandardItemKey,UnitPrice,UnitType "
            " FROM StdItemDefaultPrices "
            " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO WorkingDayPrediction "
                    "(BaseDate,Month,WorkingDayCalendarKey,WorkingDays,Year) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',Month,WorkingDayCalendarKey,WorkingDays,Year "
                    "FROM WorkingDayPrediction "
                    "WHERE BaseDate = '"UNDECLARED_BASE_DATE"' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "/* Declaring BaseDate */ "
                    "INSERT INTO ZoneMap "
                    "(BaseDate,MapKey,ZoneKey,ZoneTypeKey) "
                    "SELECT '", sizeof szTemp);
   ItiStrCat (szTemp,  pbdi->pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "',MapKey,ZoneKey,ZoneTypeKey "
                    "FROM ZoneMap "
                    "WHERE BaseDate='"UNDECLARED_BASE_DATE"' ",sizeof szTemp);

   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      usErrorCount++;

   /* send the update messages */
   ItiDbUpdateBuffers ("BaseDate");
   }


void FAR _cdecl _loadds MakeBaseDateThread (PBASEDATEINFO pbdi)
   {
   HMQ   hmq;
   HAB   hab;
   HWND  hwnd;

   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, DEFAULT_QUEUE_SIZE);

   DoIt (pbdi);

   /* finish up */
   hwnd = pbdi->hwndStatus;
   ItiMemFree (pbdi->hheap, pbdi->pszBaseDate);
   ItiMemFree (pbdi->hheap, pbdi->pszLongTextKey);
   ItiMemFree (pbdi->hheap, pbdi);

   WinDestroyMsgQueue (hmq); 
   WinTerminate (hab);
   WinPostMsg (hwnd, WM_CLOSE, 0, 0);
   }



BOOL EXPORT DeleteBaseDate (PSZ pszBaseDate)
   {
   char     szTemp [250] = "";
   USHORT   usErrorCount = 0;
   HHEAP    hheap;

   if ((pszBaseDate == NULL) || ( (*pszBaseDate) == '\0'))
      return FALSE;

   hheap = ItiMemCreateHeap (2048);
   if (hheap == NULL)
      return FALSE;

   ItiStrCpy (szTemp, "DELETE FROM AreaCounty WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ", sizeof szTemp);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;


   ItiStrCpy (szTemp, "DELETE FROM  EquipmentRate WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp,  pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ",  sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;


   ItiStrCpy (szTemp, "DELETE FROM LaborerWageAndBenefits WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp,  pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ",  sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemPriceAdjustment WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemPriceRegression "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ",  sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemPriceVariable "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, "' ",  sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemQuantityAdjustment "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemQuantityRegression "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MajorItemQuantityVariable "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  MaterialPrice "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  StandardItemPEMETHAverage "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  StandardItemPEMETHRegression "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM  StdItemDefaultPrices "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;
                               
   ItiStrCpy (szTemp, "DELETE FROM  WorkingDayPrediction "
                    "WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                    
   ItiStrCat (szTemp, "' ", sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;

   ItiStrCpy (szTemp, "DELETE FROM ZoneMap "
                     " WHERE BaseDate= '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);                     
   ItiStrCat (szTemp,  "' ",sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      usErrorCount++;



   if (usErrorCount == 0)
      {
      /* Delete Jobs with pszBaseDate here! ======= */
// =======================================

      ItiStrCpy (szTemp, "DELETE FROM LongText WHERE LongTextKey IN "
                       "(SELECT Comment FROM BaseDate "
                       "WHERE BaseDate= '", sizeof szTemp);
      ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
      ItiStrCat (szTemp, "' )", sizeof szTemp);

      if (ItiDbExecSQLStatement (hheap, szTemp))
         {
         ItiStrCpy (szTemp, "DELETE FROM BaseDate "
                          "WHERE BaseDate= '", sizeof szTemp);
         ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
         ItiStrCat (szTemp, "' ", sizeof szTemp);
         if (ItiDbExecSQLStatement (hheap, szTemp))
            usErrorCount++;
         }
      }

   if (NULL != hheap)
      ItiMemDestroyHeap (hheap);

   return TRUE;
   }/* End of Function DeleteBaseDate */

