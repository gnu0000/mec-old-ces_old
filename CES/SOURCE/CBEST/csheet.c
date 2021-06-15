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
 * Csheet.c
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\cesutil.h"
#include <stdio.h>
#include <string.h>
#include "cbest.h"
#include "initcat.h"
#include "dialog.h"
#include "calcfn.h"


#define JCSTRLEN  25


typedef struct
   {
   USHORT uSel;

   /*--- These are label displays when Show Unit Price is selected ---*/
   char szUnitMaterial [JCSTRLEN];
   char szUnitEquipment [JCSTRLEN];
   char szUnitLabor [JCSTRLEN];
   char szUnitOverheadMaterial [JCSTRLEN];
   char szUnitOverheadEquipment [JCSTRLEN];
   char szUnitOverheadLabor [JCSTRLEN];
   char szUnitOverheadTotal [JCSTRLEN];
   char szUnitMarkup [JCSTRLEN];
   char szUnitPrice0 [JCSTRLEN];
   char szExtention0 [JCSTRLEN];

   /*--- These are label displays when Show Extention is selected ---*/
   char szMaterial [JCSTRLEN];
   char szEquipment [JCSTRLEN];
   char szLabor [JCSTRLEN];
   char szOverheadMaterial [JCSTRLEN];
   char szOverheadEquipment [JCSTRLEN];
   char szOverheadLabor [JCSTRLEN];
   char szOverheadTotal [JCSTRLEN];
   char szMarkup [JCSTRLEN];
   char szExtention1 [JCSTRLEN];
   char szUnitPrice1 [JCSTRLEN];
   } JOBCOSTDAT;

typedef JOBCOSTDAT *PJOBCOSTDAT;







void ItiFixIt (HBUF  hbuf,
               HHEAP hheap,
               PSZ  pszDest,
               PSZ  pszStr,
               PSZ  pszFormat,
               PSZ  pszP1,
               PSZ  pszP2,
               PSZ  pszP3,
               PSZ  pszP4)
   {
   char szTmp [255];
   char szTmp2 [255];
   PSZ  *ppszRow, psz, psz2;
   USHORT   uNumCols, uError, uState;
   HQRY     hqry;

   /*--- Replace the %colid parameters, turn %%s to %s ---*/  
   ItiDbReplaceParams (szTmp, pszStr, hbuf, NULL, 0, sizeof szTmp);

   /*--- Replace %s with replacement strings ---*/
   sprintf (szTmp2, szTmp, pszP1, pszP2, pszP3, pszP4);

   /*--- Turn string into a query ---*/
   sprintf (szTmp, "SELECT (%s)", szTmp2);

   /*--- remove any commas ---*/
   ItiExtract (szTmp, ",");

   for (psz2=szTmp2, psz=szTmp; *psz;)
      {
      if (*psz == ',')  psz++;
      if (*psz == '\0') break;
      *psz2++ = *psz++;
      }
   *psz2 = '\0';

   /*--- Exec the query ---*/
   if (!(hqry = ItiDbExecQuery (szTmp2, hheap, 0, 0, 0, &uNumCols, &uState)))
      return;

   /*--- Get the string ---*/
   while (ItiDbGetQueryRow (hqry, &ppszRow, &uError))
      {
      ItiFmtFormatString (ppszRow[0], pszDest, JCSTRLEN, pszFormat, NULL);
      ItiFreeStrArray (hheap, ppszRow, uNumCols);
      }
//   pszDest = "Scum!";
   }



 

void FillLabelData (HWND hwnd, HBUF hbuf)
   {
   PJOBCOSTDAT p;
   HHEAP       hheap;
   PSZ         pszQuan;


   ItiWndSetHourGlass (TRUE);
   p = (PJOBCOSTDAT) ItiWndGetExtra (hwnd, ITI_STATIC);
   p->uSel = 2;

   hheap = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY, (MPARAM) ITIWND_HEAP, 0);

//  pszQuan = QW (hwnd, ITIWND_DATA, 0, 0, 0x8009);
   pszQuan = QW (hwnd, ITIWND_DATA, 0, 0, cQuantityPerItemUnit);

   ItiFixIt (hbuf, hheap, p->szUnitMaterial,          "%MaterialsUnitCost",                               "Number,$,....", 0, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitEquipment,         "%EquipmentDailyCost / %DisplayedProductionRate ",  "Number,$,....", 0, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitLabor,             "%LaborDailyCost / %DisplayedProductionRate ",      "Number,$,....", 0, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitOverheadMaterial,  "%%s + %%s * ( %MaterialsOverheadPct / 100.00)",    "Number,$,....", p->szUnitMaterial,  p->szUnitMaterial,  0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitOverheadEquipment, "%%s + %%s * ( %EquipmentOverheadPct / 100.00)",    "Number,$,....", p->szUnitEquipment, p->szUnitEquipment, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitOverheadLabor,     "%%s + (%%s * %LaborOverheadPct    ) / 100.00",     "Number,$,....", p->szUnitLabor,     p->szUnitLabor,     0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitOverheadTotal,     "%%s + %%s + %%s",                                  "Number,$,....", p->szUnitOverheadMaterial, p->szUnitOverheadEquipment, p->szUnitOverheadLabor, 0);
   ItiFixIt (hbuf, hheap, p->szUnitMarkup,            "%%s * ( %MarkupPct /100.0000)",           "Number,$,....", p->szUnitOverheadTotal, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitPrice0,            " %%s * (1.0 + ( %MarkupPct /100.00) )",   "Number,$,....", p->szUnitOverheadTotal, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szExtention0,            "%%s * %%s ",                              "Number,$,....", p->szUnitPrice0, pszQuan, 0, 0);

// ItiFixIt (hbuf, hheap, p->szExtention0,            "%%s * %QuantityPerItemUnit",        "Number,$,....", p->szUnitPrice0, 0, 0, 0);

   ItiFixIt (hbuf, hheap, p->szMaterial,              "%MaterialsUnitCost * %%s",                       "Number,$,..", pszQuan, 0, 0, 0 );
   ItiFixIt (hbuf, hheap, p->szEquipment,             "%%s * ( %EquipmentDailyCost / %ProductionRate )", "Number,$,....", pszQuan, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szLabor,                 "%%s * ( %LaborDailyCost / %ProductionRate )",     "Number,$,....", pszQuan, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szOverheadMaterial ,     "%%s * (1 + ( %MaterialsOverheadPct /100))",       "Number,$,..", p->szMaterial, 0, 0, 0 );
   ItiFixIt (hbuf, hheap, p->szOverheadEquipment,     "%%s * (1 + ( %EquipmentOverheadPct /100))",       "Number,$,..", p->szEquipment, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szOverheadLabor,         "%%s * (1 + ( %LaborOverheadPct /100))",           "Number,$,..", p->szLabor, 0, 0, 0    );
   ItiFixIt (hbuf, hheap, p->szOverheadTotal,         "%%s + %%s + %%s",                                "Number,$,..", p->szOverheadMaterial, p->szOverheadEquipment, p->szOverheadLabor, 0);
   ItiFixIt (hbuf, hheap, p->szMarkup,                "%%s * ( %MarkupPct / 100.0)",                    "Number,$,..", p->szOverheadTotal, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szExtention1,            "%%s * (1.0 + ( %MarkupPct /100.00) )",           "Number,$,..", p->szOverheadTotal, 0, 0, 0);
   ItiFixIt (hbuf, hheap, p->szUnitPrice1,            "%%s / %%s",                                      "Number,$,..", p->szExtention1, pszQuan, 0, 0);


//   ItiFixIt (hbuf, hheap, p->szMaterial,              "%MaterialsUnitCost * %%s",                 "Number,$,..", pszQuan, 0, 0, 0 );
//   ItiFixIt (hbuf, hheap, p->szEquipment,             "%EquipmentDailyCost * %Duration",          "Number,$,..", 0, 0, 0, 0);
//   ItiFixIt (hbuf, hheap, p->szLabor,                 "%LaborDailyCost * %Duration",              "Number,$,..", 0, 0, 0, 0    );
//   ItiFixIt (hbuf, hheap, p->szOverheadMaterial ,     "%%s * (1 + (%MaterialsOverheadPct /100))", "Number,$,..", p->szMaterial, 0, 0, 0 );
//   ItiFixIt (hbuf, hheap, p->szOverheadEquipment,     "%%s * (1 + (%EquipmentOverheadPct /100))", "Number,$,..", p->szEquipment, 0, 0, 0);
//   ItiFixIt (hbuf, hheap, p->szOverheadLabor,         "%%s * (1 + (%LaborOverheadPct /100))",     "Number,$,..", p->szLabor, 0, 0, 0    );
//   ItiFixIt (hbuf, hheap, p->szOverheadTotal,         "%%s + %%s + %%s",                          "Number,$,..", p->szOverheadMaterial, p->szOverheadEquipment, p->szOverheadLabor, 0);
//   ItiFixIt (hbuf, hheap, p->szMarkup,                "%%s * (%MarkupPct /100)",                  "Number,$,..", p->szOverheadTotal, 0, 0, 0);
//   ItiFixIt (hbuf, hheap, p->szExtention1,            "%%s + %%s",                                "Number,$,..", p->szMarkup, p->szOverheadTotal, 0, 0);
//   ItiFixIt (hbuf, hheap, p->szUnitPrice1,            "%%s / %%s",                                "Number,$,..", p->szExtention1, pszQuan, 0, 0);
//// ItiFixIt (hbuf, hheap, p->szUnitPrice1,            "%%s / %QuantityPerItemUnit",             "Number,$,..", p->szExtention1, 0, 0, 0);

   ItiMemFree (QW (hwnd, ITIWND_HEAP, 0, 0, 0), pszQuan);
   ItiWndSetHourGlass (FALSE);
   }




void ItiSetLbl (HWND hwnd, USHORT uLabelIndex, PSZ psz)
   {
   WinSendMsg (hwnd, WM_ITIWNDSET,
               MPFROM2SHORT (ITIWND_LABEL, uLabelIndex),
               MPFROMP (psz));
   }



void SetLabels (HWND hwnd, USHORT uMode)
   {
   PJOBCOSTDAT p;
   USHORT      i;

   p = (PJOBCOSTDAT) ItiWndGetExtra (hwnd, ITI_STATIC);

   if (p->uSel == uMode)
      return;

   for (i=13; i<22; i++)
      SW (hwnd, ITIWND_LABELHIDDEN, i, uMode, 0);
   for (i=0; i<10; i++)
      SW (hwnd, ITIWND_DATAHIDDEN, 0x8001 + i, uMode, 0);

   switch (uMode)
      {
      case 0: /*--- Show Unit Price ---*/
         {
         ItiSetLbl (hwnd, 0, p->szUnitMaterial);
         ItiSetLbl (hwnd, 1, p->szUnitEquipment);
         ItiSetLbl (hwnd, 2, p->szUnitLabor);
         ItiSetLbl (hwnd, 3, p->szUnitOverheadMaterial);
         ItiSetLbl (hwnd, 4, p->szUnitOverheadEquipment);
         ItiSetLbl (hwnd, 5, p->szUnitOverheadLabor);
         ItiSetLbl (hwnd, 6, p->szUnitOverheadTotal);
         ItiSetLbl (hwnd, 7, p->szUnitMarkup);
         ItiSetLbl (hwnd, 8, p->szUnitPrice0);
         ItiSetLbl (hwnd, 9, p->szExtention0);

         ItiSetLbl (hwnd, 10, "Unit Price:");
         ItiSetLbl (hwnd, 11, "Extension:");
         ItiSetLbl (hwnd, 12, "* Quantity:");

         break;
         }
      case 1: /*--- Show Extention ---*/
         {
         ItiSetLbl (hwnd, 0, p->szMaterial);
         ItiSetLbl (hwnd, 1, p->szEquipment);
         ItiSetLbl (hwnd, 2, p->szLabor);
         ItiSetLbl (hwnd, 3, p->szOverheadMaterial );
         ItiSetLbl (hwnd, 4, p->szOverheadEquipment);
         ItiSetLbl (hwnd, 5, p->szOverheadLabor);
         ItiSetLbl (hwnd, 6, p->szOverheadTotal);
         ItiSetLbl (hwnd, 7, p->szMarkup);
         ItiSetLbl (hwnd, 8, p->szExtention1);
         ItiSetLbl (hwnd, 9, p->szUnitPrice1);

         ItiSetLbl (hwnd, 10, "Extension:");
         ItiSetLbl (hwnd, 11, "Unit Price:");
         ItiSetLbl (hwnd, 12, "/ Quantity:");
         break;
         }
      }
   }




/* HERE ! */
MRESULT EXPORT JobCostShtStaticProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITQUERY:
         {
         HHEAP hheap;
         PVOID p;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         p = ItiMemAlloc (hheap, sizeof (JOBCOSTDAT), 0);
         memset (p, '\0', sizeof (JOBCOSTDAT)); // null out data
         ItiWndSetExtra (hwnd, ITI_STATIC, p);
         }
         break;

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

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Job Crew List", 
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Materials List", 
                                      IDM_VIEW_USER_FIRST + 2);
               return 0;
            }
         break;

      case WM_ITIDBBUFFERDONE:
         {
         HWND  hwndCombo;
         ULONG ulStyle;

         ulStyle = WinQueryWindowULong (hwnd, QWL_STYLE);
         ulStyle &= ~WS_CLIPCHILDREN;
         WinSetWindowULong (hwnd, QWL_STYLE, ulStyle);

         FillLabelData (hwnd, mp2);

         SetLabels (hwnd, 0);

         hwndCombo = WinWindowFromID (hwnd, JobCostShtCombo);
         WinSendMsg (hwndCombo, LM_DELETEALL, 0, 0);
         WinSendMsg (hwndCombo, LM_INSERTITEM,
                     (MPARAM)LIT_END, MPFROMP ("Show Unit Price"));
         WinSendMsg (hwndCombo, LM_INSERTITEM,
                     (MPARAM)LIT_END, MPFROMP ("Show Extension"));
         WinSendMsg (hwndCombo, LM_SELECTITEM, 0, MPFROMP (TRUE));

         break;
         }
 
      case WM_CONTROL:
         if ((SHORT2FROMMP (mp1) == LN_ENTER   ||
              SHORT2FROMMP (mp1) == LN_SELECT) &&
             !(UM) WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), (USHORT) CBM_ISLISTSHOWING, 0, 0))
            {
            switch (SHORT1FROMMP (mp1))
               {
               case JobCostShtCombo:
                  {
                  HWND     hwndCombo;
                  USHORT   uSel;
            
//                  /*--- did selection change? ---*/
//                  if (SHORT2FROMMP (mp1) != CBN_LBSELECT)
//                     break;
//            
                  hwndCombo = WinWindowFromID (hwnd, JobCostShtCombo);
                  uSel = (UM) WinSendMsg (hwndCombo, LM_QUERYSELECTION, 0, 0);
            
                  SetLabels (hwnd, uSel);
            
                  WinInvalidateRect (hwnd, NULL, 0);
                  return 0;
                  }
               }
            }
            break;
      
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case IDM_CHANGE:
               {
               HHEAP  hheap;
               PSZ    pszJobKey, pszJobItemKey, pszJobBrkdwnKey, pszJobCstShtKey;
               USHORT us, usCBEst, usActive;
               CHAR   szTemp[128] = "";
               PSZ    pszQuantity;
               CHAR   szQuantity[16] = "";
               PSZ    pszStandardItemKey;
               CHAR   szStandardItemKey[10] = "";
               PSZ    *ppszPNames, *ppszParams;

               hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

               pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobKey));

               pszJobItemKey     = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobItemKey));

               pszJobBrkdwnKey   = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobBreakdownKey));
               
               pszJobCstShtKey   = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobCostSheetKey));

               ItiWndDoDialog (hwnd, 0, FALSE);

               ItiWndSetHourGlass (TRUE);

               /* If the JobCostSheet is changed, and the JobItem's UnitPrice */
               /* is calculated using CostBasedEstimate, and the JCBEst is    */
               /* active, then the JobItem must be recalculated               */

               usCBEst = DetermineJobItemEstMethod(hwnd);
               if (usCBEst == 0)
                  {
                  usActive = DetermineIfCBEActive(hwnd);

                  if (usActive == 0)
                     {
                     sprintf (szTemp,  " SELECT Quantity"
                                       " FROM JobItem"
                                       " WHERE JobItemKey = %s "
                                       " AND JobKey = %s ",
                                       pszJobItemKey, pszJobKey);
                     pszQuantity = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
                                /* We need to remove any possible commas */
                                /* in the number formats.                */
                     us = 1 + ItiStrLen (pszQuantity);
                     ItiStrCpy(szQuantity,
                               (ItiExtract (pszQuantity, ",$")), us);

                     sprintf (szTemp,  " SELECT StandardItemKey"
                                       " FROM JobItem"
                                       " WHERE JobItemKey = %s "
                                       " AND JobKey = %s ",
                                       pszJobItemKey, pszJobKey);
                     pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

                     RecalcCBEST(hheap,
                                 pszJobKey,
                                 pszJobBrkdwnKey,
                                 pszJobItemKey,
                                 szQuantity,
                                 pszStandardItemKey);

                     ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
                     ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);
       
                     CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
                     CalcJobItemChgJob       (hheap, ppszPNames, ppszParams);

                     ItiFreeStrArray (hheap, ppszPNames, 4);
                     ItiFreeStrArray (hheap, ppszParams, 4);

                     } /* endif (usActive == 0) */
                  } /* endif (usCBEst == 0) */


               if (pszQuantity)
                  ItiMemFree(hheap, pszQuantity);
               if (pszJobKey)
                  ItiMemFree(hheap, pszJobKey);
               if (pszStandardItemKey)
                  ItiMemFree(hheap, pszStandardItemKey);
               if (pszJobItemKey)
                  ItiMemFree(hheap, pszJobItemKey);
               if (pszJobBrkdwnKey)
                  ItiMemFree(hheap, pszJobBrkdwnKey);
               if (pszJobCstShtKey)
                  ItiMemFree(hheap, pszJobCstShtKey);

               // ADDED CHG
               SetLabels (hwnd, 0);

               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("Task");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiWndSetHourGlass (FALSE);

               break;
               }

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, JobCrewS);
               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("Task");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, JobMaterialsS);
               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("Task");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               return 0;

            default:
               break;
            }
         break;
      }


   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }









MRESULT EXPORT EditJobCostSheetProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   {
   BOOL bChg;
   PSZ  pszInitialGran;
   static CHAR szInitGran[8];

   switch (usMessage)
      {
      case WM_INITDLG:
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT, cCodeValueID, NULL, 0);
         ItiWndActivateRow (hwnd, DID_PRODGRAN, 0, hwnd, 0);
         pszInitialGran = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cProductionGranularity));
         ItiStrCpy(szInitGran, pszInitialGran, sizeof szInitGran);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_OK:
               /* -- If the ProductionGranularity changed we need to  */
               /* -- reset the ProductionRate to reflect that change. */
               bChg = ItiWndActiveRowChanged (hwnd, DID_PRODGRAN, 0, hwnd, 0);
               if (bChg)
                  {
                  ResetProdRate (hwnd, TRUE, szInitGran);
                  }

// look in the static window procedure for this recalc.
//                RecalcCBEST(hheap,
//                            pszJobKey,
//                            pszJobBreakdownKey,
//                            pszJobItemKey,
//                            pszQuantity,
//                            pszStandardItemKey);
               break;

            default:
               break;
            }/* end switch */
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




