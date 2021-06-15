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
 * catalog.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itidbase.h"
#include "init.h"
#include <stdio.h>
#include "..\include\dialog.h"
#include "catalog.h"

typedef struct
   {
   PSZ      pszCatalogID;
   USHORT   usPerm;
   } CATPERM;

typedef CATPERM *PCATPERM;



static USHORT usNumCatalogs = 0;
static PCATPERM pcp = NULL;


static BOOL CheckPerm (PSZ pszCatalogID)
   {
   USHORT i;

   if (pglobals->usUserType >= USER_SUPER)
      return TRUE;

   if (pcp == NULL)
      return TRUE;

   for (i = 0; i < usNumCatalogs; i++)
      if (pcp [i].pszCatalogID && 
          0 == ItiStrICmp (pszCatalogID, pcp [i].pszCatalogID))
         {
         return pcp [i].usPerm >= USER_USER;
         }

   return TRUE;
   }






BOOL CanEditCatalog (USHORT usID)
   {
   switch (usID)
      {
      case ApplicationS            :
         return CheckPerm ("Applicat");

      case AreaTypeCatS            :
      case AreaTypeS               :
         return CheckPerm ("AreaType");

      case CBEstForStdItemS        :
      case TaskCostShtS            :
      case TaskS                   :
         return CheckPerm ("CBEST");

      case CodeTableS              :
      case CodeValueS              :
         return CheckPerm ("CodeTab");

      case CostSheetCatS           :
      case CostSheetS              :
      case CostShtCrewS            :
         return CheckPerm ("CostSht");

      case CountyCatS              :
         return CheckPerm ("County");

      case CrewCatS                :
      case CrewForCostShtS         :
      case CrewS                   :
         return CheckPerm ("Crew");

      case DefaultUnitPriceCatS    :
      case DefaultUnitPriceS       :
         return CheckPerm ("Default");

      case EquipmentCatS           :
      case EquipmentS              :
         return CheckPerm ("Equip");

      case FacilityCatS            :
      case FacilityForMaterialS    :
      case FacilityForVendorS      :
      case FacilityS               :
         return CheckPerm ("Facility");

      case FundCatS                :
         return CheckPerm ("Fund");

      case InflationPredictionCatS :
      case InflationPredictionS    :
         return CheckPerm ("InflPred");

      case LaborCatS               :
      case LaborerS                :
         return CheckPerm ("Labor");

      case ParametricProfileCatS   :
      case MajItemPriceRegrS       :
      case MajItemQuantityRegrS    :
         return CheckPerm ("Paramet");

      case MajorItemCatS           :
      case MajorItemS              :
         return CheckPerm ("MajItem");

      case MapCatS                 :
         return CheckPerm ("Map");

      case MaterialCatS            :
      case MaterialPriceS          :
      case MaterialS               :
         return CheckPerm ("Material");

      case StandardItemCatS        :
      case StandardItemS           :
         return CheckPerm ("StdItem");

      case UserCatS                :
         return CheckPerm ("User");

      case VendorCatS              :
      case VendorS                 :
         return CheckPerm ("Venor");

      case WorkingDayCalendarCat   :
      case WorkingDayCalendarCatS  :
      case WorkingDayCalendarS     :
         return CheckPerm ("WorkDay");

      case ZoneTypeCatS            :
      case ZonesS                  :
         return CheckPerm ("ZoneType");


      case DefaultUserS            :
      case FacilityBrowserResultsS :
      case PropBrowserResultsS     :
      case ItemBrowserResultsS     :
      case BidderS                 :
      case CategoryS               :
      case ProjectCategoryS        :
      case ProjectItemS            :
      case ProjectS                :
      case ProjectsForJobS         :
      case ProposalItemC1          :
      case ProposalItemC2          :
      case ProposalItemC3          :
      case ProposalItemC4          :
      case ProposalItemS           :
      case ProposalMajItemS        :
      case ProposalS               :
      case SignifMajItemS          :
         return TRUE;

      default:
         return TRUE;
      }
   }



BOOL InitCatalogForUser (PSZ pszUserKey)
   {
   HQRY     hqry;
   USHORT   usNumCols, usError, i;
   PSZ      *ppsz;
   char     szQuery [1024];

   if (pcp)
      {
      for (i = 0; i < usNumCatalogs; i++)
         ItiMemFree (hheap, pcp[i].pszCatalogID);
      ItiMemFree (hheap, pcp);
      pcp = NULL;
      }
      
   sprintf (szQuery, 
            "SELECT C.CatalogID, UC.Permission "
            "FROM Catalog C, UserCatalog UC "
            "WHERE UC.UserKey = %s "
            "AND UC.CatalogKey = C.CatalogKey "
            "ORDER BY C.CatalogID ",
            pszUserKey);

   usNumCatalogs = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   if (usNumCatalogs == 0)
      return FALSE;

   pcp = ItiMemAlloc (hheap, sizeof (CATPERM) * usNumCatalogs, MEM_ZERO_INIT);
   if (pcp == NULL)
      return FALSE;

   sprintf (szQuery, 
            "SELECT C.CatalogID, UC.Permission "
            "FROM Catalog C, UserCatalog UC "
            "WHERE UC.UserKey = %s "
            "AND UC.CatalogKey = C.CatalogKey "
            "ORDER BY C.CatalogID ",
            pszUserKey);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      if (i < usNumCatalogs)
         {
         pcp [i].pszCatalogID = ItiStrDup (hheap, ppsz [0]);
         ItiStrToUSHORT (ppsz [1], &(pcp [i].usPerm));
         i++;
         }
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   return TRUE;
   }


BOOL EXPORT ItiPermQueryUserCatAvail (void)
   {
   return pglobals->usUserType >= USER_SUPER;
   }

 

