/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * sort.c
 * Copyright (C) 1990 Info Tech, Inc.
 *
 */

#define INCL_WIN
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "window.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "dialog.h"
#include "init.h"




/* returns TRUE if the query actually changes
 *
 *
 */

BOOL FixQuery (PSZ pszDest, PSZ pszSrc,
               HWND hwnd,   HWND hwndList,
               USHORT uD1,  USHORT uD2)
   {
   CHAR  szFirst [40], szSecond [40];
   PSZ   pszOStart = NULL;
   PSZ   pszOEnd   = NULL;

   WinQueryWindowText (WinWindowFromID (hwnd, DID_FIRSTC),  sizeof szFirst, szFirst);
   WinQueryWindowText (WinWindowFromID (hwnd, DID_SECONDC), sizeof szSecond, szSecond);
   if (!stricmp (szSecond, "<none>"))
      *szSecond = '\0';

   strcpy (pszDest, pszSrc);
   if (pszOStart = ItiDbFindOrderByClause (pszSrc))
      {
      for (pszOEnd = pszOStart + 8; *pszOEnd; pszOEnd++)

         if (!ItiStrNICmp (pszOEnd, "compute", 7) ||
             !ItiStrNICmp (pszOEnd, "for", 3))
            break;

      pszDest[pszOStart - pszSrc] = '\0';
      }
   strcat (pszDest, " ORDER BY ");
   strcat (pszDest, szFirst);
   strcat (pszDest, (uD1 ? " DESC" : " ASC "));

   if (*szSecond)
      {
      strcat (pszDest, ",");
      strcat (pszDest, szSecond);
      strcat (pszDest, (uD2 ? " DESC " : " ASC "));
      }

   if (pszOStart)
      strcat (pszDest, pszOEnd);

   return stricmp (pszSrc, pszDest);
   }













CHAR *ppszDir[] = { "~Ascending",
                    "~Descending"};

/*
 *   make sure you pass hwndList in the DoDlg function call!
 */

MRESULT EXPORT LWSortDlgProc (HWND    hwnd,
                                USHORT  uMsg,
                                MPARAM  mp1,
                                MPARAM  mp2)
   {
   static HWND   hwndList;
   static USHORT uD1;
   static USHORT uD2;

   switch (uMsg)
      {
      case WM_INITDLG:
         {
         USHORT   i, uCol;
         PSZ      pszCol;
         PEDT     pedt;

         hwndList = (HWND) mp2;

         /*--- Fill Field Comboboxes ---*/
         WinSendDlgItemMsg (hwnd, DID_SECONDC, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP ("<None>"));

         uCol = (UM) QW (hwndList, ITIWND_NUMCOLS, 0, 0, 0);
         for (i=0; i<uCol; i++)
            {
            pedt = (PEDT) QW (hwndList, ITIWND_DATAEDT, i, 0, 0);
            if (!(pszCol = ItiColColIdToString (pedt->uIndex)))
               continue;

            WinSendDlgItemMsg (hwnd, DID_FIRSTC, LM_INSERTITEM,
                                     (MPARAM)LIT_END, MPFROMP (pszCol));
            WinSendDlgItemMsg (hwnd, DID_SECONDC, LM_INSERTITEM,
                                     (MPARAM)LIT_END, MPFROMP (pszCol));
            }

         /*--- Select Initial data ---*/
         WinSendDlgItemMsg (hwnd, DID_FIRSTC, LM_SELECTITEM, 0, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, DID_SECONDC, LM_SELECTITEM, 0, MPFROMP (TRUE));

         uD1 = uD2 = 0;
         break;
         }

      case WM_CLOSE:
         WinDismissDlg (hwnd, 0xffff);
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_FIRSTD:
               WinSetWindowText (WinWindowFromID (hwnd, DID_FIRSTD),
                                 ppszDir[uD1 = 1-uD1]);
               return 0L;

            case DID_SECONDD:
               WinSetWindowText (WinWindowFromID (hwnd, DID_SECONDD),
                                 ppszDir[uD2 = 1-uD2]);
               return 0L;

            case DID_OK:
               {
               HBUF   hbuf;
               HHEAP  hheap;
               PSZ    pszQuery, pszTmp;
               
               hbuf     = (HBUF)  QW (hwndList, ITIWND_BUFFER, 0, 0, 0);
               hheap    = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               pszTmp   = (PSZ)   ItiMemAlloc (hheap, 4096, 0);
               pszQuery = (PSZ)   ItiDbQueryBuffer (hbuf, ITIDB_QUERY, 0);

               if (FixQuery (pszTmp, pszQuery, hwnd, hwndList, uD1, uD2))
                  WinSendMsg (hwndList, WM_CHANGEQUERY, MPFROMLONG (-1), 
                              MPFROMP (pszTmp));

               ItiMemFree (hheap, pszTmp);

               WinDismissDlg (hwnd, 0xffff);
               return 0L;
               }

            case DID_CANCEL:
               WinDismissDlg (hwnd, 0xffff);
               return 0L;
            }
      }
   return WinDefDlgProc (hwnd, uMsg, mp1, mp2);
   }




USHORT EXPORT ItiMenuDoSort (HWND hwndList)
   {
   USHORT uRet;

   uRet = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppClient,
                     LWSortDlgProc, hmodMe, IDD_LWSORT, hwndList);
   return uRet;
   }
