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
 * find.c
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


/* I make this data a structure because i am going to store
 * this data in the lw data structure. This will make seatrches
 * for list windows context sensitive, and will save thier last
 * find specs.
 */

typedef struct
   {  HWND     hwnd;
      HBUF     hbuf;
      USHORT   uMode;
      USHORT   uCol;
      CHAR     szMatch [100];
   } MATCHINFO;

typedef MATCHINFO *PMATCHINFO;




void Initpmi (HWND hwnd, PMATCHINFO *pmi)
   {
   HHEAP hheap;
   HBUF  hbuf;

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   hbuf  =  (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);

   *pmi = (PMATCHINFO) ItiMemAlloc (hheap, sizeof (MATCHINFO), 0);

   (*pmi)->hwnd     = hwnd;
   (*pmi)->hbuf     = hbuf;
   (*pmi)->uMode    = 0;
   (*pmi)->uCol     = 0;
   strcpy ((*pmi)->szMatch, szFindPattern);       // csp bsr 930130 10/5/94
   
   ItiWndSetExtra (hwnd, ITI_LIST, *pmi);
   }







const CHAR *ppszModes[] = { "Find Next Match",
                            "Find Previous Match",
                            "Select All Matches"};

/*
 *   make sure you pass hwndList in the DoDlg function call!
 */

MRESULT EXPORT LWFindDlgProc (HWND    hwnd,
                                USHORT  uMsg,
                                MPARAM  mp1,
                                MPARAM  mp2)
   {
   static PMATCHINFO pmi;

   switch (uMsg)
      {
      case WM_INITDLG:
         {
         USHORT   uFlags, uEntrys = 2;
         USHORT   i, uCol;
         HWND     hwndList;
         PSZ      pszCol;
         PEDT     pedt;

         hwndList = (HWND) mp2;

         if (!(pmi = (PMATCHINFO) ItiWndGetExtra (hwndList, ITI_LIST)))
            Initpmi (hwndList, &pmi);

         /*--- Fill Find Mode ComboBox ---*/
         uFlags = (UM) QW (hwndList, ITIWND_FLAGS, 0, 0, 0);
         uEntrys += !!((uFlags & LWS_MULTISELECT) == LWS_MULTISELECT);

         for (i=0; i<uEntrys; i++)
            WinSendDlgItemMsg (hwnd, DID_MODE, LM_INSERTITEM, (MPARAM)LIT_END,
                               MPFROMP (ppszModes[i]));

         /*--- Fill Field Combobox ---*/

// THIS METHOD USES THE DBASE MODULE
//
//         uCol = (UM)ItiDbQueryBuffer (pmi->hbuf, ITIDB_NUMCOL, 0);
//
//         for (i=0; i<uCol; i++)
//            {
//            uColId = (UM) ItiDbQueryBuffer (pmi->hbuf, ITIDB_COLID, i);
//            pszCol = ItiColColIdToString (uColId);
//
//            WinSendDlgItemMsg (hwnd, DID_FIELD, LM_INSERTITEM,
//                                (MPARAM)LIT_END, MPFROMP (pszCol));
//            }

// THIS USES WIN LABELS
//
//         uCol = (UM) WinSendMsg (pmi->hwnd, WM_ITIWNDQUERY, (MPARAM)ITIWND_LABELCOLS, 0);
//         for (i=0; i<uCol; i++)
//            {
//            pszCol = (PSZ)WinSendMsg (pmi->hwnd, WM_ITIWNDQUERY,
//                                     MPFROM2SHORT (ITIWND_LABEL, i), 0);
//            WinSendDlgItemMsg (hwnd, DID_FIELD, LM_INSERTITEM,
//                                     (MPARAM)LIT_END, MPFROMP (pszCol));
//            }


// THIS USES WIN DATA AND DBASE
//
         uCol = (UM) QW (pmi->hwnd, ITIWND_NUMCOLS, 0, 0, 0);
         for (i=0; i<uCol; i++)
            {
            pedt = (PEDT) QW (pmi->hwnd, ITIWND_DATAEDT, i, 0, 0);

            if (!(pszCol = ItiColColIdToString (pedt->uIndex)))
               continue;

            WinSendDlgItemMsg (hwnd, DID_FIELD, LM_INSERTITEM,
                                     (MPARAM)LIT_END, MPFROMP (pszCol));
            }


         /*--- Select Initial data ---*/
         WinSendDlgItemMsg (hwnd, DID_FIELD, LM_SELECTITEM,
                                     (MPARAM)pmi->uCol, MPFROMP (TRUE));

         WinSendDlgItemMsg (hwnd, DID_MODE, LM_SELECTITEM,
                                     (MPARAM)pmi->uMode, MPFROMP (TRUE));

         WinSetDlgItemText (hwnd, DID_MATCH, pmi->szMatch);   
         }
         break;


      case WM_CLOSE:
         WinDismissDlg (hwnd, 0xffff);
         return 0;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               PEDT     pedt;
               USHORT   i, uActive, uRows;
               CHAR     szDat [257];

               pmi->uCol = (UM)WinSendDlgItemMsg (hwnd, DID_FIELD,
                                                  LM_QUERYSELECTION, 0, 0);

               pedt = (PEDT) QW (pmi->hwnd, ITIWND_DATAEDT, pmi->uCol, 0, 0);

               pmi->uMode = (UM)WinSendDlgItemMsg (hwnd, DID_MODE,
                                                   LM_QUERYSELECTION, 0, 0);

               WinQueryWindowText (WinWindowFromID (hwnd, DID_MATCH), 100,
                                                    pmi->szMatch);

               /*--- Here we do the selecting                         ---*/
               /*--- 0) Find Next Match                               ---*/
               /*---  -----------------                               ---*/
               /*---   store old active row                           ---*/
               /*---   deselect all                                   ---*/
               /*---   search for match starting starting at oldpos+1 ---*/
               /*---   if found select/activate and return            ---*/
               /*---   if not found activate oldrow                   ---*/
               /*---                                                  ---*/
               /*---                                                  ---*/
               /*--- 1) Find Previous Match                           ---*/
               /*---  ---------------------                           ---*/
               /*---   same as above but searching up                 ---*/
               /*---                                                  ---*/
               /*---                                                  ---*/
               /*--- 2) Select All Matches                            ---*/
               /*---  --------------------                            ---*/
               /*---   store old active row                           ---*/
               /*---   deselect all                                   ---*/
               /*---   search for matches starting starting at 0      ---*/
               /*---   select all matches                             ---*/
               /*---   activate first match if any found              ---*/
               /*---   if not found activate oldrow                   ---*/
               /*---                                                  ---*/

               uActive = (UM)QW (pmi->hwnd, ITIWND_ACTIVE, 0, 0, 0);
               uRows   = (UM)QW (pmi->hwnd, ITIWND_NUMROWS, 0, 0, 0);

               SW (pmi->hwnd, ITIWND_SELECTION, ITI_UNSELECT, 0, 0xFFFF);

               strcpy (szFindPattern, pmi->szMatch); // csp bsr 930130 10/5/94


               switch (pmi->uMode)
                  {
                  case 0: /*--- Find Next Match---*/
                     {
                     for (i= uActive+1; i < uRows; i++)
                        {
                        ItiDbGetBufferRowCol (pmi->hbuf, i, pedt->uIndex, szDat);

                        if (ItiStrMatchWildCard (pmi->szMatch, szDat))
                           {
                           SW (pmi->hwnd, ITIWND_SELECTION, ITI_SELECT, i, i);
                           SW (pmi->hwnd, ITIWND_ACTIVE, 0, i, 0);
                           WinDismissDlg (hwnd, 0);
                           return 0;
                           }
                        }
                     SW (pmi->hwnd, ITIWND_ACTIVE, 0, uActive, 0);
                     break;

                     }
                  case 1: /*--- Find Prev Match---*/
                     {
                     for (i=uActive-1; i <= uActive; i--)
                        {
                        ItiDbGetBufferRowCol (pmi->hbuf, i, pedt->uIndex, szDat);

                        if (ItiStrMatchWildCard (pmi->szMatch, szDat))
                           {
                           SW (pmi->hwnd, ITIWND_SELECTION, ITI_SELECT, i, i);
                           SW (pmi->hwnd, ITIWND_ACTIVE, 0, i, 0);
                           WinDismissDlg (hwnd, 0);
                           return 0;
                           }
                        }
                     SW (pmi->hwnd, ITIWND_ACTIVE, 0, uActive, 0);

                     break;
                     }

                  case 2: /*--- Find All  Match---*/
                     {
                     USHORT uNewA = 0xFFFF;

                     for (i= 0; i < uRows; i++)
                        {
                        ItiDbGetBufferRowCol (pmi->hbuf, i, pedt->uIndex, szDat);

                        if (ItiStrMatchWildCard (pmi->szMatch, szDat))
                           {
                           SW (pmi->hwnd, ITIWND_SELECTION, ITI_SELECT, i, i);

                           uNewA = (uNewA == 0xFFFF ? i : uNewA);
                           }
                        }
                     SW (pmi->hwnd, ITIWND_ACTIVE, 0, uNewA == 0xFFFF ? uActive : uNewA, 0);

                     WinDismissDlg (hwnd, 0);
                     break;
                     }
                  }
               break;
               }

            case DID_CANCEL:
               WinDismissDlg (hwnd, 0xffff);
               return 0L;
            }
      }
   return WinDefDlgProc (hwnd, uMsg, mp1, mp2);
   }






USHORT EXPORT ItiMenuDoFind (HWND hwndList)
   {
   USHORT uRet;

   if (strlen (szFindPattern) == 0)                  // csp bsr 930130 10/5/94
      strcpy (szFindPattern, "*");                   // csp bsr 930130 10/5/94

   uRet = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppClient,
                     LWFindDlgProc, hmodMe, IDD_LWSEARCH, hwndList);
   return uRet;
   }
