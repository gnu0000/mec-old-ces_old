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
 * doexp.c
 */
#define INCL_WIN
#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\colid.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "initdll.h"
#include "..\include\itiimp.h"
#include "..\include\message.h"



USHORT BuildKeyList (HWND    hwndList,
                     PSZ     pszDest,
                     USHORT  usBufferColumn,
                     USHORT  usDestMax)
   {
   USHORT   usRow;
   BOOL     bMultiSel;
   PSZ      pszData;
   HHEAP    hheap;

//   bMultiSel = !!(WinQueryWindowULong (hwndList, QWL_STYLE) & LWS_MULTISELECT);
   bMultiSel = TRUE;
   *pszDest = '\0';
   hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                     MPFROMSHORT (bMultiSel ? 
                                                  ITIWND_SELECTION : 
                                                  ITIWND_ACTIVE), 0));
   while (usRow != -1)
      {
      pszData = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, usBufferColumn);

      ItiStrCat (pszDest, pszData, usDestMax);
      ItiMemFree (hheap, pszData);

      if (!bMultiSel)
         usRow = -1;
      else
         usRow = (UM) QW (hwndList, ITIWND_SELECTION, usRow + 1, 0, 0);
      if (usRow != -1)
         ItiStrCat (pszDest, ",", usDestMax);
      } 

   return ItiStrLen (pszDest);
   }





BOOL EXPORT ItiExpGetWhereClause (PSZ     pszWhere,
                                  USHORT  usWhereLen,
                                  PSZ     pszTable,
                                  USHORT  usTableLen)
   {
   HWND     hwnd;
   USHORT   usID, i;
   PSZ      pszKey;
   CHAR     szTemp[2048] = "";

   hwnd = WinSendMsg (WinQueryWindow (pglobals->hwndAppClient,
                                            QW_PARENT, FALSE),
                            WM_QUERYACTIVEDOC, 0, 0);

   if (hwnd == NULL)
      return FALSE;

   hwnd = WinWindowFromID (hwnd, FID_CLIENT);
   if (hwnd == NULL)
      return FALSE;

   usID = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);
   if (usID == 0)
      return FALSE;

   pszKey = NULL;
   *pszWhere = '\0';
   *pszTable = '\0';
   switch (usID)
      {
      case DefaultUserS:
      case CBEstForJobItemS:
      case CrewForJobS:
      case JobAlternateGroupListS:
      case JobAlternateGroupS:
      case JobBreakdownFundS:
      case JobBreakdownListS:
      case JobBreakdownMajorItemS:
      case JobBreakdownProfileS:
      case JobBreakdownS:
      case JobCommentS:
      case JobCostShtS:
      case JobCrewS:
      case JobFundS:
      case JobItemS:
      case JobItemScheduleS:
      case JobItemsS:
      case JobMajItemPriceRegrS:
      case JobMajItemQuantityRegrS:
      case JobMaterialsS:
      case JobPemethAvgS:
      case JobS:
      case JobTaskS:
      case JobUserS:
      case JobZoneS:
      case PemethAvgS:
         ItiStrCpy (pszWhere, " AND J.JobKey= ", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No JobKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Job", usTableLen);
         break;

      case ProgramFundS:
      case ProgramJobS:
      case ProgramS:
      case ProgramUserS:
         ItiStrCat (pszWhere, " AND P.ProgramKey=", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cProgramKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No ProgramKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Program", usTableLen);
         break;

      case AreaTypeCatS:
         ItiStrCat (pszWhere, " AND AT.AreaTypeKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, AreaTypeCatL),
                       pszWhere + i, cAreaTypeKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "AreaType", usTableLen);
         break;

      case AreaTypeS:
         ItiStrCat (pszWhere, " AND AT.AreaTypeKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cAreaTypeKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No AreaTypeKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "AreaType", usTableLen);
         break;

      case CBEstForStdItemS:
         ItiStrCat (pszWhere, " AND CBE.CostBasedEstimateKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, CBEstForStdItemL),
                       pszWhere + i, cCostBasedEstimateKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "CostBasedEstimate", usTableLen);
         break;

      case CodeTableS:
         ItiStrCat (pszWhere, " AND CT.CodeTableKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, CodeTableL),
                       pszWhere + i, cCodeTableKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "CodeTable", usTableLen);
         break;

      case CodeValueS:
         ItiStrCat (pszWhere, " AND CT.CodeTableKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cCodeTableKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No CodeTableKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "CodeTable", usTableLen);
         break;

      case CostSheetCatS:
         ItiStrCat (pszWhere, " AND CS.CostSheetKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, CostSheetCatL),
                       pszWhere + i, cCostSheetKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "CostSheet", usTableLen);
         break;

      case CostSheetS:
      case CostShtCrewS:
      case CrewForCostShtS:
         ItiStrCat (pszWhere, " AND CS.CostSheetKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cCostSheetKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No CostSheetKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "CostSheet", usTableLen);
         break;

      case CountyCatS:
         ItiStrCat (pszWhere, " AND C.County IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, CountyCatL),
                       pszWhere + i, cCountyKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "County", usTableLen);
         break;

      case CrewCatS:
         ItiStrCat (pszWhere, " AND C.CrewKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, CrewCatL),
                       pszWhere + i, cCrewKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Crew", usTableLen);
         break;

      case CrewS:
         ItiStrCat (pszWhere, " AND C.CrewKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cCrewKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No CrewKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Crew", usTableLen);
         break;

      case DefaultUnitPriceCatS:
         ItiStrCat (pszWhere, " AND STDP.StandardItemKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, DefaultUnitPriceCatL),
                       pszWhere + i, cStandardItemKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "StdItemDefaultPrices", usTableLen);
         break;

      case DefaultUnitPriceS:
         ItiStrCat (pszWhere, " AND STDP.StandardItemKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No StdItemKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "StdItemDefaultPrices", usTableLen);
         break;

      case UserCatS:
         ItiStrCat (pszWhere, " AND U.UserKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, UserCatL),
                       pszWhere + i, cUserKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "User", usTableLen);
         break;

      case VendorCatS:
         ItiStrCat (pszWhere, " AND V.VendorKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, VendorCatL),
                       pszWhere + i, cVendorKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Vendor", usTableLen);
         break;

      case VendorS:
         ItiStrCat (pszWhere, " AND V.VendorKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cVendorKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No VendorKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Vendor", usTableLen);
         break;

      case ZoneTypeCatS:
         ItiStrCat (pszWhere, " AND ZT.ZoneTypeKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, ZoneTypeCatL),
                       pszWhere + i, cZoneTypeKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "ZoneType", usTableLen);
         break;

      case ZonesS:
         ItiStrCat (pszWhere, " AND ZT.ZoneTypeKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cZoneTypeKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No ZoneTypeKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "ZoneType", usTableLen);
         break;

      case ApplicationS:
         return FALSE;
         break;

      case EquipmentCatS:
         ItiStrCat (pszWhere, " AND E.EquipmentKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, EquipmentCatL),
                       pszWhere + i, cEquipmentKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Equipment", usTableLen);
         break;

      case EquipmentS:
         ItiStrCat (pszWhere, " AND E.EquipmentKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cEquipmentKey);
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Equipment", usTableLen);
         break;

      case LaborCatS:
         ItiStrCat (pszWhere, " AND L.LaborerKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, LaborCatL),
                       pszWhere + i, cLaborerKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Labor", usTableLen);
         break;

      case LaborerS:
         ItiStrCat (pszWhere, " AND L.LaborerKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cLaborerKey);
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Labor", usTableLen);
         break;

      case MaterialCatS:
         ItiStrCat (pszWhere, " AND M.MaterialKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, MaterialCatL),
                       pszWhere + i, cMaterialKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Material", usTableLen);
         break;

      case MaterialPriceS:
         ItiStrCat (pszWhere, " AND M.MaterialKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cMaterialKey);
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "Material", usTableLen);
         break;

      case FundCatS:
         ItiStrCat (pszWhere, " AND F.FundKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, FundCatL),
                       pszWhere + i, cFundKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Fund", usTableLen);
         break;

      case MajorItemCatS:
         ItiStrCat (pszWhere, " AND MI.MajorItemKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, MajorItemCatL),
                       pszWhere + i, cFundKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "MajorItem", usTableLen);
         break;

      case MajorItemS:
         ////
         {
         PSZ pszReset;

         if (DosScanEnv("RESETHPMI", &pszReset))
            ResetCalcHPMIItem ("DSShell");
            /* -- Recalculate the HistPropMajorItem extensions. */
         }
         ////
         ItiStrCat (pszWhere, " AND MI.MajorItemKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cMajorItemKey);
         if (pszKey == NULL)
            {
            sprintf (szTemp, "No MajoritemKey given for: %s", pszWhere);
            ItiErrWriteDebugMessage (szTemp);
            return FALSE;
            }
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "MajorItem", usTableLen);
         break;

      case StandardItemCatS:
         ItiStrCat (pszWhere, " AND SI.StandardItemKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, StandardItemCatL),
                       pszWhere + i, cStandardItemKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ") ORDER BY SI.StandardItemNumber ", usWhereLen);
         ItiStrCpy (pszTable, "StandardItem", usTableLen);
         break;

      case StandardItemS:
         ItiStrCat (pszWhere, " AND SI.StandardItemKey =", usWhereLen);
         pszKey = QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey);
         ItiStrCat (pszWhere, pszKey, usWhereLen);
         ItiStrCpy (pszTable, "StandardItem", usTableLen);
         break;

      case MapCatS:
         ItiStrCat (pszWhere, " AND M.MapKey IN (", usWhereLen);
         i = ItiStrLen (pszWhere);
         BuildKeyList (WinWindowFromID (hwnd, MapCatL),
                       pszWhere + i, cMapKey, usWhereLen -  i);
         ItiStrCat (pszWhere, ")", usWhereLen);
         ItiStrCpy (pszTable, "Map", usTableLen);
         break;

      case BidderS:
      case FacilityBrowserResultsS:
      case FacilityCatS:
      case FacilityForMaterialS:
      case FacilityForVendorS:
      case FacilityS:
//         break;

      case InflationPredictionCatS:
      case InflationPredictionS:
//         break;

      case MajItemPriceRegrS:
      case MajItemQuantityRegrS:
//         break;

      case CategoryS:
//         break;

      case ParametricProfileCatS:
      case SignifMajItemS:
      case ItemBrowserResultsS:
      case ProjectCategoryS:
      case ProjectItemS:
      case ProjectS:
      case ProjectsForJobS:
      case PropBrowserResultsS:
      case ProposalItemC1:
      case ProposalItemC2:
      case ProposalItemC3:
      case ProposalItemC4:
      case ProposalItemS:
      case ProposalMajItemS:
      case ProposalS:
//         break;

      case TaskCostShtS:
      case TaskS:
//         break;

      case WorkingDayCalendarCat:
      case WorkingDayCalendarCatS:
      case WorkingDayCalendarS:
//         break;

      default:
         WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame,
                        "I don't know how to export this window!",
                        pglobals->pszAppName, 0, 
                        MB_OK | MB_MOVEABLE | MB_OK);
         return FALSE;
      }

   if (strstr (pszWhere, "()"))
      *pszWhere = '\0';
   return TRUE;
   }






