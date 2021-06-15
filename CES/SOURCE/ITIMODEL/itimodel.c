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
 * itimodel.c
 * Mark Hampton
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itimodel.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "init.h"
#include <stdio.h>
#include "local.h"

void EXPORT ItiModelRun (HWND hwnd, USHORT usModelID)
   {
   switch (usModelID)
      {
      case ITIMODEL_PROFILE_ANALYSIS:
         ItiWndSetDlgMode (ITI_OTHER);
         ItiWndBuildWindow (pglobals->hwndAppFrame, ModelProfileD);
         break;

      case ITIMODEL_QUANTITY_ANALYSIS:
         ItiWndSetDlgMode (ITI_OTHER);
         ItiWndBuildWindow (pglobals->hwndAppFrame, ModelQuantityD);
         break;

      case ITIMODEL_PRICE_ANALYSIS:
         ItiWndSetDlgMode (ITI_OTHER);
         ItiWndBuildWindow (pglobals->hwndAppFrame, ModelPriceD);
         break;

      default:
         WinMessageBox (HWND_DESKTOP, hwnd, 
                        "This function is not implemented yet.",
                        "ItiModel", 0, 
                        MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
         break;
      }
   }



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
      while (ItiCharIsSpace (*pszBuffer) && usLen > 0)
         {
         pszBuffer++;
         usLen--;
         }
      }
   return usLen != 0 && *pszBuffer != '\0';
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



void BuildKeyList (HWND    hwndList,
                   FILE    *pf,
                   USHORT  usBufferColumn)
   {
   USHORT   usRow;
   BOOL     bMultiSel;
   PSZ      pszData;
   HHEAP    hheap;

//   bMultiSel = !!(WinQueryWindowULong (hwndList, QWL_STYLE) & LWS_MULTISELECT);
   bMultiSel = TRUE;
   hheap = WinSendMsg (hwndList, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_HEAP), 0);

   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                     MPFROMSHORT (bMultiSel ? 
                                                  ITIWND_SELECTION : 
                                                  ITIWND_ACTIVE), 0));
   while (usRow != -1)
      {
      pszData = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_DATA), 
                            MPFROM2SHORT (usRow, usBufferColumn));

      if (pszData != NULL)
         fputs (pszData, pf);
      ItiMemFree (hheap, pszData);

      if (!bMultiSel)
         usRow = -1;
      else
         usRow = SHORT1FROMMP (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                           MPFROM2SHORT (ITIWND_SELECTION, 
                                           usRow + 1), 0));
      if (usRow != -1 && pszData != NULL)
         fputs (", ", pf);
      }
   }

