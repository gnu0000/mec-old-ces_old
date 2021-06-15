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
#include "dssprop.h"
#include "dialog.h"
#include "replace.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itifmt.h"

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



BOOL GetField (HWND     hwnd, 
               USHORT   usControl, 
               PSZ      pszBuffer, 
               USHORT   usBufferSize)
   {
   USHORT usLen;

   usLen = WinQueryDlgItemText (hwnd, usControl, usBufferSize, pszBuffer);
   if (usLen != 0)
      {
      while (isspace (*pszBuffer) && usLen > 0)
         {
         pszBuffer++;
         usLen--;
         }
      }
   return usLen != 0 && *pszBuffer != '\0';
   }




#define FMT_OK       1
#define FMT_BAD      2
#define FMT_EMPTY    0

USHORT GetNumericField (HWND     hwnd, 
                        USHORT   usControl, 
                        PSZ      pszBuffer, 
                        USHORT   usBufferSize)
   {
   char szTemp [256];

   if (GetField (hwnd, usControl, szTemp, sizeof szTemp))
      {
      if (0 == ItiFmtFormatString (szTemp, pszBuffer, usBufferSize,
                                   "Number,$....", NULL))
         return FMT_OK;
      return FMT_BAD;
      }
   return FMT_EMPTY;
   }

USHORT GetDateField (HWND     hwnd, 
                     USHORT   usControl, 
                     PSZ      pszBuffer, 
                     USHORT   usBufferSize)
   {
   char szTemp [256];

   if (GetField (hwnd, usControl, szTemp, sizeof szTemp))
      {
      if (0 == ItiFmtFormatString (szTemp, pszBuffer, usBufferSize,
                                   "DateTime,mm/dd/yyyy", NULL))
         return FMT_OK;
      return FMT_BAD;
      }
   return FMT_EMPTY;
   }





void TranslateWildCards (PSZ psz)
   {
   while (*psz)
      {
      if (*psz == '*')
         *psz = '%';
      else if (*psz == '?')
         *psz = '_';
      psz++;
      }
   }


PSZ BuildQuery (HWND hwnd)
   {
   char szTemp [1024];

   ItiMbQueryQueryText (0, ITIRID_WND, PropCustomQuery, szQuery, 
                        sizeof szQuery);
   
   /* check to see if we're to limit proposal IDs */
   if (GetField (hwnd, DID_PROPID, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.ProposalID LIKE '", sizeof szQuery);
      TranslateWildCards (szTemp);
      ItiStrCat (szQuery, szTemp, sizeof szQuery);
      ItiStrCat (szQuery, "' ", sizeof szQuery);
      }

   /* check to see if we're to limit location descriptions */
   if (GetField (hwnd, DID_LOCATION, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.LocationDescription LIKE '", sizeof szQuery);
      TranslateWildCards (szTemp);
      ItiStrCat (szQuery, szTemp, sizeof szQuery);
      ItiStrCat (szQuery, "' ", sizeof szQuery);
      }

   /* check to see if we're to limit estimates */
   switch (GetNumericField (hwnd, DID_ESTIMATE1, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_ESTIMATE1));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HP.EngineersEstimate >= ", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, " ", sizeof szQuery);
         break;
      }

   switch (GetNumericField (hwnd, DID_ESTIMATE2, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_ESTIMATE2));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HP.EngineersEstimate <= ", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, " ", sizeof szQuery);
         break;
      }

   /* check to see if we're to limit letting dates */
   switch (GetDateField (hwnd, DID_LETTING1, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_LETTING1));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HP.LettingDate >= '", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, "' ", sizeof szQuery);
         break;
      }

   switch (GetDateField (hwnd, DID_LETTING2, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_LETTING2));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HP.LettingDate <= '", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, "' ", sizeof szQuery);
         break;
      }

   /* check to see if we're to limit to only awarded proposals */
   if (IsButtonChecked (hwnd, DID_AWARDED_PROP))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.RejectedFlag=0 ", sizeof szQuery);
      }

   /* check to see if we're to limit to only rejected proposals */
   if (IsButtonChecked (hwnd, DID_REJECTED_PROP))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.RejectedFlag=1 ", sizeof szQuery);
      }

   /* check to see if we're to limit work types */
   if (IsSomethingSelected (hwnd, PropBrowserWorkL))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.WorkType IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, PropBrowserWorkL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      if (!ItiStrCmp ((szQuery + (ItiStrLen (szQuery) - 4)), "(1) "))
         ItiMemSet ((szQuery + (ItiStrLen (szQuery) - 40)), '\0', 1);
      }   

   /* check to see if we're to limit road types */
   if (IsSomethingSelected (hwnd, PropBrowserRoadL))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.RoadType IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, PropBrowserRoadL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }   

   /* check to see if we're to limit districts */
   if (IsSomethingSelected (hwnd, PropBrowserDistrictL))
      {
      ItiStrCat (szQuery, " AND HistoricalProposal.District IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, PropBrowserDistrictL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }

   /* check to see if we're to limit bidders */
   if (IsSomethingSelected (hwnd, PropBrowserBidderL))
      {
      ItiStrCat (szQuery, 
                 " AND HistoricalProposal.ProposalKey IN "
                 "(SELECT DISTINCT HistoricalProposal.ProposalKey "
                 " FROM HistoricalProposal p1, HistoricalBidder hb"
                 " WHERE p1.ProposalKey = hb.ProposalKey "
                 " AND hb.VendorKey IN (", 
                 sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, PropBrowserBidderL),
                    szQuery + ItiStrLen (szQuery),
                    cVendorKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ")) ", sizeof szQuery);
      }

   /* add the order by clause */
   ItiStrCat (szQuery, " ORDER BY ProposalID ",
              sizeof szQuery);

   if (ItiStrLen (szQuery) > BIGSTRLEN)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, "Search criteria is to large. "
                     "Please select a smaller search criteria.",
                     "DSS/Proposal Browser",
                      666, MB_ICONHAND | MB_MOVEABLE | MB_OK);
      return NULL;
      }

   return szQuery;
   }



