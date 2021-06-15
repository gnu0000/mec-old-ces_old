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
 * profile.c
 * Mark Hampton
 *
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itimodel.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itirptut.h"
#include <stdio.h>
#include <process.h>
#include <share.h>

#include "init.h"
#include "dialog.h"
#include "profile.h"
#include "local.h"

BOOL BuildWhereClause (HWND     hwnd, 
                       FILE     *pf)
   {
   char szTemp [257];

   /* check to see if we're to limit the number of bids */
   if (GetField (hwnd, DID_NUMBEROFBIDS1, szTemp, sizeof szTemp))
      {
      fprintf (pf, "AND HP.NumberOfBids >= %s ", szTemp);
      if (GetField (hwnd, DID_NUMBEROFBIDS2, szTemp, sizeof szTemp))
         fprintf (pf, "AND HP.NumberOfBids <= %s ", szTemp);
      }

   /* check to see if we're to limit awarded amounts */
   if (GetField (hwnd, DID_AWARDEDTOTAL1, szTemp, sizeof szTemp))
      {
      fprintf (pf, "AND HP.AwardedBidTotal >= $%s ", szTemp);
      if (GetField (hwnd, DID_AWARDEDTOTAL2, szTemp, sizeof szTemp))
         fprintf (pf, "AND HP.AwardedBidTotal <= $%s ", szTemp);
      }

   /* check to see if we're to limit letting dates */
   if (GetField (hwnd, DID_LETTINGDATE1, szTemp, sizeof szTemp))
      {
      fprintf (pf, "AND HP.LettingDate >= '%s' ", szTemp);
      if (GetField (hwnd, DID_LETTINGDATE2, szTemp, sizeof szTemp))
         fprintf (pf, "AND HP.LettingDate <= '%s' ", szTemp);
      }

   /* check to see if we're to limit work types */
   if (IsSomethingSelected (hwnd, ModelWorkTypeL))
      {
      fprintf (pf, "AND HP.WorkType IN (");
      BuildKeyList (WinWindowFromID (hwnd, ModelWorkTypeL),
                    pf, cCodeValueKey);
      fprintf (pf, ") ");
      }   

   /* check to see if we're to limit road types */
   if (IsSomethingSelected (hwnd, ModelRoadTypeL))
      {
      fprintf (pf, "AND HP.RoadType IN (");
      BuildKeyList (WinWindowFromID (hwnd, ModelRoadTypeL),
                    pf, cCodeValueKey);
      fprintf (pf, ") ");
      }   

   /* check to see if we're to limit districts */
   if (IsSomethingSelected (hwnd, ModelDistrictL))
      {
      fprintf (pf, "AND HP.District IN (");
      BuildKeyList (WinWindowFromID (hwnd, ModelDistrictL),
                    pf, cCodeValueKey);
      fprintf (pf, ") ");
      }

   /* check to see if we're to limit statuses */
   if (IsSomethingSelected (hwnd, ModelStatusL))
      {
      fprintf (pf, "AND HP.Status IN (");
      BuildKeyList (WinWindowFromID (hwnd, ModelStatusL),
                    pf, cCodeValueKey);
      fprintf (pf, ") ");
      }

   /* check to see if we're to major items */
   if (IsSomethingSelected (hwnd, ModelMajorItemL))
      {
      fprintf (pf, "AND HPMI.MajorItemKey IN (");
      BuildKeyList (WinWindowFromID (hwnd, ModelMajorItemL),
                    pf, cMajorItemKey);
      fprintf (pf, ") ");
      }
   return TRUE;
   }




MRESULT EXPORT ProfileDlgProc (HWND    hwnd,
                               USHORT  usMessage,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   int i;

   switch (usMessage)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               char  szFileName [257];
               FILE  *pf;
               char  szTemp [257];

               ItiRptUtUniqueFile (szFileName, sizeof szFileName, TRUE);
               pf = _fsopen (szFileName, "wt", SH_DENYWR);
               if (pf == NULL)
                  {
                  DosBeep (1000, 100);
                  DosBeep (500, 100);
                  break;
                  }

               WinQueryDlgItemText (hwnd, DID_PROFILEID, sizeof szTemp, szTemp);
               fputs (szTemp, pf);
               fputc ('\n', pf);

               WinQueryDlgItemText (hwnd, DID_DESCRIPTION, sizeof szTemp, szTemp);
               fputs (szTemp, pf);
               fputc ('\n', pf);
               
               BuildWhereClause (hwnd, pf);
               fputc ('\n', pf);
               fclose (pf);

               spawnl (P_NOWAIT, "itirpt.exe", "itirpt.exe", "dprof", 
                       "-modelinfo", szFileName, 
                       IsButtonChecked (hwnd, DID_MAKEWORKTYPE) 
                          ? "-worktype" 
                          : NULL, NULL);

               /* -- Now, if chosen do the quantity and prices too. */
               if (IsButtonChecked (hwnd, DID_ALSO_Q_P))
                  {
                  for (i=0; i<30000; i++)
                     i=i;
                  DosBeep (1000, 100);
                  spawnl (P_NOWAIT, "itirpt.exe", "itirpt.exe", "dqty", 
                       "-modelinfo", szFileName, 
                       IsButtonChecked (hwnd, DID_MAKEWORKTYPE) 
                          ? "-worktype" 
                          : NULL, NULL);

                  for (i=0; i<30000; i++)
                     i=i;
                  DosBeep (1300, 100);
                  spawnl (P_NOWAIT, "itirpt.exe", "itirpt.exe", "dprice", 
                       "-modelinfo", szFileName, 
                       IsButtonChecked (hwnd, DID_MAKEWORKTYPE) 
                          ? "-worktype" 
                          : NULL, NULL);
                  } /* end if */
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }

