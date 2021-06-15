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
#include "..\include\itiutil.h"
#include "DSSITEM.h"
#include "..\include\dialog.h"
#include "dialog.h"
#include "..\include\itifmt.h"
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
   char szTemp [257];

   ItiStrCpy (szQuery,
              "SELECT HP.ProposalID, HP.Description, "
              "SI.StandardItemNumber, HPI.Quantity, CV.CodeValueID, "
              "EST.UnitPrice, AWARD.UnitPrice, LOW.UnitPrice, "
              "SI.ShortDescription "
              
              "FROM HistoricalProposal HP, StandardItem SI, "
              "HistoricalProposalItem HPI, HistoricalItemBid AWARD, "
              "HistoricalItemBid EST, HistoricalItemBid LOW, "
              "CodeValue CV "
              
              "WHERE HPI.ItemKey=SI.StandardItemKey "
              "AND CV.CodeValueKey = SI.Unit "
              "AND HP.ProposalKey = HPI.ProposalKey "
              "AND HP.ProposalKey = EST.ProposalKey "
              "AND HP.ProposalKey = AWARD.ProposalKey "
              "AND HP.ProposalKey = LOW.ProposalKey "
              "AND HPI.ProposalItemKey = EST.ProposalItemKey "
              "AND HPI.ProposalItemKey = AWARD.ProposalItemKey "
              "AND HPI.ProposalItemKey = LOW.ProposalItemKey "
              "AND HP.LowBidderKey = LOW.VendorKey "
              "AND HP.AwardedBidderKey = AWARD.VendorKey "
              "AND 1 = EST.VendorKey "
              , sizeof szQuery);

   /* check to see if we're to limit StandardItemNumbers */
   if (GetField (hwnd, DID_ITEMNUMBER, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, "AND SI.StandardItemNumber LIKE '", sizeof szQuery);
      TranslateWildCards (szTemp);
      ItiStrCat (szQuery, szTemp, sizeof szQuery);
      ItiStrCat (szQuery, "' ", sizeof szQuery);
      }

   /* check to see if we're to limit standard item descriptions */
   if (GetField (hwnd, DID_DESCRIPTION, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, "AND SI.ShortDescription LIKE '", sizeof szQuery);
      TranslateWildCards (szTemp);
      ItiStrCat (szQuery, szTemp, sizeof szQuery);
      ItiStrCat (szQuery, "' ", sizeof szQuery);
      }

   /* check to see if we're to limit quantities */
   switch (GetNumericField (hwnd, DID_QUANTITY1, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_QUANTITY1));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HPI.Quantity >= ", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, " ", sizeof szQuery);
         break;
      }

   switch (GetNumericField (hwnd, DID_QUANTITY2, szTemp, sizeof szTemp))
      {
      case FMT_BAD:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_QUANTITY2));
         DosBeep (300, 250);
         return NULL;

      case FMT_EMPTY:
         break;

      case FMT_OK:
         ItiStrCat (szQuery, "AND HPI.Quantity <= ", sizeof szQuery);
         ItiStrCat (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, " ", sizeof szQuery);
         break;
      }

   /* check to see if we're to limit proposal ids */
   if (GetField (hwnd, DID_PROPID, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, "AND HP.ProposalID LIKE '", sizeof szQuery);
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

   /* check to see if we're to limit location descriptions */
   if (GetField (hwnd, DID_LOCATION, szTemp, sizeof szTemp))
      {
      ItiStrCat (szQuery, "AND HP.LocationDescription LIKE '", sizeof szQuery);
      TranslateWildCards (szTemp);
      ItiStrCat (szQuery, szTemp, sizeof szQuery);
      ItiStrCat (szQuery, "' ", sizeof szQuery);
      }

   /* check to see if we're to limit to only awarded proposals */
   if (IsButtonChecked (hwnd, DID_AWARDED_PROP))
      {
      ItiStrCat (szQuery, "AND HP.RejectedFlag=0 ", sizeof szQuery);
      }

   /* check to see if we're to limit to only rejected proposals */
   if (IsButtonChecked (hwnd, DID_REJECTED_PROP))
      {
      ItiStrCat (szQuery, "AND HP.RejectedFlag=1 ", sizeof szQuery);
      }

   /* check to see if we're to limit work types */
   if (IsSomethingSelected (hwnd, IBWorkTypeL))
      {
      ItiStrCat (szQuery, "AND HP.WorkType IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, IBWorkTypeL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }   

   /* check to see if we're to limit road types */
   if (IsSomethingSelected (hwnd, IBRoadTypeL))
      {
      ItiStrCat (szQuery, "AND HP.RoadType IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, IBRoadTypeL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }   

   /* check to see if we're to limit districts */
   if (IsSomethingSelected (hwnd, IBDistrictsL))
      {
      ItiStrCat (szQuery, "AND HP.District IN (", sizeof szQuery);
      BuildKeyList (WinWindowFromID (hwnd, IBDistrictsL),
                    szQuery + ItiStrLen (szQuery),
                    cCodeValueKey, sizeof (szQuery) - ItiStrLen (szQuery));
      ItiStrCat (szQuery, ") ", sizeof szQuery);
      }

//   /* add the order by clause */
//   ItiStrCat (szQuery, "ORDER BY SI.StandardItemNumber, HP.ProposalID ",
//              sizeof szQuery);

   return szQuery;
   }


