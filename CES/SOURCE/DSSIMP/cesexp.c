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
 * cesexp.c
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




BOOL EXPORT ItiExpJob (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOB, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBUSER, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCOMMENT, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBALTGROUP, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBALT, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBBREAK, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBBREAKFUND, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBITEM, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBITEMDEP       , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_WORKINGDAYCAL    , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_WORKINGDAYPRE    , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBBREAKPROF     , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBBREAKMAJORITEM, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCBE           , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCOSTSHEET     , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCREWUSED      , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCREWLABORUSED , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBCREWEQUIPMENTUSED , pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_JOBMATERIALUSED  , pszWhere))
      return FALSE;


   return TRUE;
   }


BOOL EXPORT ItiExpProgram (HHEAP hheap, FILE *pf, PSZ pszWhere)
   {
   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PROGRAM, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PROGRAMFUND, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PROGRAMUSER, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PROGRAMJOB, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_PROGRAMJOBDEPEND, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_INFLATIONPRED, pszWhere))
      return FALSE;

   if (!ItiExpExportTable (hheap, hmod, pf, EXP_INFLATIONCHANGE, pszWhere))
      return FALSE;

   return TRUE;
   }
