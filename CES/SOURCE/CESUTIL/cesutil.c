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
 * cesutil.c
 * Mark Hampton
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "init.h"




BOOL EXPORT ItiCesIsJobWindow (HWND hwnd, USHORT usID)
   {
   if (hwnd != NULL && usID == 0)
      usID = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);

   return (usID == CBEstForJobItemS          ||
           usID == CrewForJobS               ||
           usID == JobAlternateGroupListS    ||
           usID == JobAlternateGroupS        ||
           usID == JobBreakdownFundS         ||
           usID == JobBreakdownListS         ||
           usID == JobBreakdownMajorItemS    ||
           usID == JobBreakdownProfileS      ||
           usID == JobBreakdownS             ||
           usID == JobCommentS               ||
           usID == JobCostShtS               ||
           usID == JobCrewS                  ||
           usID == JobFundS                  ||
           usID == JobItemS                  ||
           usID == JobItemScheduleS          ||
           usID == JobItemsS                 ||
           usID == JobMajItemPriceRegrS      ||
           usID == JobMajItemQuantityRegrS   ||
           usID == JobMaterialsS             ||
           usID == JobPemethAvgS             ||
           usID == JobS                      ||
           usID == JobTaskS                  ||
           usID == JobUserS                  ||
           usID == JobZoneS                  ||
           usID == PemethAvgS);
//
// These are not part of a job! 
//  12/04/92 -cf
//           ||
//           usID == PemethAvgSA               ||
//           usID == PemethAvgSB               );
   }




BOOL EXPORT ItiCesIsProgramWindow (HWND hwnd, USHORT usID)
   {
   if (hwnd != NULL && usID == 0)
      usID = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);

   return (usID == ProgramFundS              ||
           usID == ProgramJobS               ||
           usID == ProgramS                  ||
           usID == ProgramUserS              );
   }



BOOL EXPORT ItiCesIsCatalogWindow (HWND hwnd, USHORT usID)
   {
   if (hwnd != NULL && usID == 0)
      usID = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);

   return (usID == ApplicationS              ||
           usID == AreaTypeCatS              ||
           usID == AreaTypeS                 ||
           usID == BidderS                   ||
           usID == CBEstForStdItemS          ||
           usID == CategoryS                 ||
           usID == CodeTableS                ||
           usID == CodeValueS                ||
           usID == CostSheetCatS             ||
           usID == CostSheetS                ||
           usID == CostShtCrewS              ||
           usID == CountyCatS                ||
           usID == CrewCatS                  ||
           usID == CrewForCostShtS           ||
           usID == CrewS                     ||
           usID == DefaultUnitPriceCatS      ||
           usID == DefaultUnitPriceS         ||
           usID == DefaultUserS              ||
           usID == EquipmentCatS             ||
           usID == EquipmentS                ||
           usID == FacilityBrowserResultsS   ||
           usID == FacilityCatS              ||
           usID == FacilityForMaterialS      ||
           usID == FacilityForVendorS        ||
           usID == FacilityS                 ||
           usID == FundCatS                  ||
           usID == InflationPredictionCatS   ||
           usID == InflationPredictionS      ||
           usID == ItemBrowserResultsS       ||
           usID == LaborCatS                 ||
           usID == LaborerS                  ||
           usID == MajItemPriceRegrS         ||
           usID == MajItemQuantityRegrS      ||
           usID == MajorItemCatS             ||
           usID == MajorItemS                ||
           usID == MapCatS                   ||
           usID == MaterialCatS              ||
           usID == MaterialPriceS            ||
           usID == MaterialS                 ||
           usID == ParametricProfileCatS     ||
           usID == ProjectCategoryS          ||
           usID == ProjectItemS              ||
           usID == ProjectS                  ||
           usID == ProjectsForJobS           ||
           usID == PropBrowserResultsS       ||
           usID == ProposalItemC1            ||
           usID == ProposalItemC2            ||
           usID == ProposalItemC3            ||
           usID == ProposalItemC4            ||
           usID == ProposalItemS             ||
           usID == ProposalMajItemS          ||
           usID == ProposalS                 ||
           usID == SignifMajItemS            ||
           usID == StandardItemCatS          ||
           usID == StandardItemS             ||
           usID == TaskCostShtS              ||
           usID == TaskS                     ||
           usID == UserCatS                  ||
           usID == VendorCatS                ||
           usID == VendorS                   ||
           usID == WorkingDayCalendarCat     ||
           usID == WorkingDayCalendarCatS    ||
           usID == WorkingDayCalendarS       ||
           usID == ZoneTypeCatS              ||
           usID == ZonesS                    );
   }





/*
 * This file contains two separate types of functions:
 *
 * 1> High level functions, such as DoJobRecalc, which get keys
 *     and call the ItiDBDoCalc function.
 *    These are called by you and return immediately 
 *
 * 2> Low Level functions, such as CalcDetailedEstimateCost, which
 *     are called by the recalc thread to do the actual calculation.
 *    These block.
 *
 *
 */



