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
 * catexp.c
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "initdll.h"
#include "..\include\itiimp.h"



BOOL EXPORT ItiExpAreaType (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_AREATYPE, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_AREA, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_AREACOUNTY, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpZoneType (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_ZONETYPE, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_ZONE, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_ZONEMAP, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpCounty (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_COUNTY, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_COUNTYMAP, pszWhere))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiExpMap (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAP, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAPVERTEX, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpDefault (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_DEFAULT, pszWhere))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiExpFund (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_FUND, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpCodeTable (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CODETABLE, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CODEVALUE, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpUser (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_USER, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_USERCATALOG, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_USERAPPLICATION, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_DEFAULTJOBUSER, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_DEFAULTPROGRAMUSER, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpCBE (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBE                 , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_TASK                , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBECOSTSHEET        , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBECREWUSED         , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBECREWEQUIPMENTUSED, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBECREWLABORUSED    , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CBEMATERIALUSED     , pszWhere))
      return FALSE;


   return TRUE;
   }

BOOL EXPORT ItiExpCrew (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREW         , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREWEQUIPMENT, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREWLABOR    , pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpLabor (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_LABOR      , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_LABORERWAGE, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpEquipment (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_EQUIPMENT    , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_EQUIPMENTRATE, pszWhere))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiExpMaterial (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MATERIAL     , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MATERIALPRICE, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpCostSheet (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_COSTSHEET        , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREWUSED         , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREWEQUIPMENTUSED, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_CREWLABORUSED    , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MATERIALUSED     , pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpMajorItem (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   CHAR szNewWhere[2000] = "";
   INT  inx = 0;
   PSZ  pszTmp;

   pszTmp = pszWhere;

   if (pszWhere != NULL)
      {
      ItiStrCpy (szNewWhere, pszWhere, sizeof szNewWhere);
      ItiStrCat (szNewWhere, " ORDER BY MajorItemID ", sizeof szNewWhere);
      }
   else
      ItiStrCpy (szNewWhere, " ORDER BY MajorItemID ", sizeof szNewWhere);

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEM , szNewWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEM_NULSY , szNewWhere))
      return FALSE;


   while ( (pszWhere != NULL) && (*(pszTmp+2) != '\0') ) 
      {
      if ( (*pszTmp == 'M') && (*(pszTmp+1) == 'I') && (*(pszTmp+2) == '.') )
         {
         szNewWhere[inx] = 'S';
         inx++;
         szNewWhere[inx] = 'I';
         inx++;
         }/* end if */

      szNewWhere[inx] = (*pszTmp);
      inx++;
      pszTmp++;
      }/* end while */

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMSTDITEM, szNewWhere))
      return FALSE;

   ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMSTDITEM_NULSY, pszWhere);

   return TRUE;
   }


BOOL EXPORT ItiExpMajorItemPriceReg (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMPRICEREG, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMPRICEVAR, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMPRICEADJ, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpMajorItemQuantityReg (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMQTYREG, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMQTYVAR, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_MAJORITEMQTYADJ, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpParametricProfile (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PARAMETRICPROFILE, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_SIGMAJORITEM, pszWhere))
      return FALSE;

   return TRUE;
   }

BOOL EXPORT ItiExpPEMETHAverage (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PEMETHAVG, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_WAUP, pszWhere))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiExpPEMETHRegression (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PEMETHREG, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_AREAADJ, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_WORKTYPEADJ, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_SEASONADJ, pszWhere))
      return FALSE;

   return TRUE;
   }


BOOL EXPORT ItiExpVendor (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_VENDOR, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_VENDORADDRESS, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_VENDORCLASS, pszWhere))
      return FALSE;

   return TRUE;
   }



BOOL EXPORT ItiExpStandardItem (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_STANDARDITEM, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_STANDARDITEMDEPEND, pszWhere))
      return FALSE;

   return TRUE;
   }
