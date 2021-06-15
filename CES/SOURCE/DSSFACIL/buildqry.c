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
 * buildqry.c
 * Mark Hampton
 * Copyright (C) 1991 AASHTO
 *
 */

#define INCL_WIN
#include <stdio.h>
#include <ctype.h>
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "dssfacil.h"
#include "dialog.h"
#include "replace.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"


static char szQuery [10240];


BOOL IsButtonChecked (HWND    hwnd,
                      USHORT  usControl)
   {
   return 1 == SHORT1FROMMR (WinSendDlgItemMsg (hwnd, usControl, 
                                                BM_QUERYCHECK, 0, 0));
   }

BOOL IsSomethingSelected (HWND    hwnd,
                          USHORT  usControl)
   {
   return -1 != SHORT1FROMMR (WinSendDlgItemMsg (hwnd, usControl, 
                              WM_ITIWNDQUERY, 
                              MPFROM2SHORT (ITIWND_SELECTION, 0), 0));
   }



PSZ BuildQuery (HWND hwnd)
   {
   ItiMbQueryQueryText (0, ITIRID_WND, FacilityCustomQuery, szQuery, 
                        sizeof szQuery);
   
   /* check to see if we're to limit to only portable facilities */
   if (IsButtonChecked (hwnd, DID_PORTABLEONLY))
      {
      ItiStrCat (szQuery, " AND VF.PortableFlag=1 ", sizeof szQuery);
      }

   /* check to see if we're to limit to only state approved facilities */
   if (IsButtonChecked (hwnd, DID_APPROVEDONLY))
      {
      ItiStrCat (szQuery, " AND FM.StateApproved=1 ", sizeof szQuery);
      }

   /* check to see if we're to limit counties */
   if (IsSomethingSelected (hwnd, FacilityBrowserCountyL))
      {
      ItiStrCat (szQuery, " AND VF.CountyKey IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, FacilityBrowserCountyL),
                    szQuery + ItiStrLen (szQuery),
                    cCountyKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }   

   /* check to see if we're to limit materials */
   if (IsSomethingSelected (hwnd, FacilityBrowserMaterialL))
      {
      ItiStrCat (szQuery, " AND FM.MaterialKey IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, FacilityBrowserMaterialL),
                    szQuery + ItiStrLen (szQuery),
                    cMaterialKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }   

   /* check to see if we're to limit vendors */
   if (IsSomethingSelected (hwnd, FacilityBrowserVendorL))
      {
      ItiStrCat (szQuery, " AND V.VendorKey IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, FacilityBrowserVendorL),
                    szQuery + ItiStrLen (szQuery),
                    cVendorKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }

   /* add the order by clause */
   ItiStrCat (szQuery, " ORDER BY VF.FacilityNumber, V.VendorID, M.MaterialID ",
              sizeof szQuery);

   return szQuery;
   }


