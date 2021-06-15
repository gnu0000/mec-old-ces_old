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


/************************************************************************
 *                                                                      *
 * winupdat.c                                                           *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itilock.h"
#include "..\include\colid.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include "swmain.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "initdll.h"
#include "winupdat.h"
#include "winutil.h"
#include "dlgdef.h"
#include "lwmain.h"
#include "lwutil.h"


static   CHAR  szTmpUE      [400] = "";
static   CHAR  szTmpColName [255] = "";
static   CHAR  szTmpMsg     [128] = "";
static   CHAR  szTmpGDBD    [512] = "";  /* used in GetDlgBufferData       */
static   CHAR  szColValLoc  [256] = "";  /* used in ItiWndReplaceDlgParams */
static   CHAR  szWSVMessage [256] = "";

static   CHAR  szPreRAP    [2040] = "";
static   CHAR  szTmpRAP    [2040] = "";
static   CHAR  szPostRAP   [2040] = "";

static   CHAR  szQryAMD    [4000] = "";  /* used in ItiWndAutoModifyDb     */
static   char  szNewQryM   [4000] = "";  /* used in ItiWndModifyDb         */


// static void WriteStackValues (HWND hwnd)
//    {
//    PCHAR    pcStack;
//    SEL      selStack;
//    USHORT   usStackMax;
//    BOOL     bDone;
//    USHORT   usUsed;
// //   char     szMessage [256];
//    PVOID    pvBottom;
// 
//    pvBottom = &pvBottom;
//    pcStack = (PCHAR) &pcStack;
//    selStack = SELECTOROF (pcStack);
//    pcStack = MAKEP (selStack, 0);
//    usStackMax = ItiMemQuerySeg (&selStack);
// 
//    /* search for bottom of stack */
//    bDone = FALSE;
//    while (OFFSETOF (pcStack) < usStackMax && !bDone)
//       {
//       bDone = *pcStack != '\0';
//       pcStack++;
//       }
//    pcStack--;
// 
//    usUsed = usStackMax - OFFSETOF (pcStack);
//    sprintf (szWSVMessage, "Stack size = %u (0x%x)\n"
//                        "Stack Bottom = %u (0x%x)\n"
//                        "Current SP = %u:%u (0x%x:0x%x)\n"
//                        "Bytes Used = %u (0x%x)",
//             usStackMax, usStackMax, 
//             OFFSETOF (pcStack), OFFSETOF (pcStack),
//             selStack, OFFSETOF (pvBottom), selStack, OFFSETOF (pvBottom), 
//             usUsed, usUsed);
// 
//    ItiErrWriteDebugMessage (szWSVMessage);
//    // WinMessageBox (HWND_DESKTOP,hwnd,szWSVMessage,"StackValues",0,MB_OK);
// 
//    } /* End of Function WriteStackValues */



HBUF UpdErr (PSZ psz, HWND hwndOwner)
   {
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (pglobals->hwndDesktop, hwndOwner, psz,
                  "Update Error", 0, MB_MOVEABLE  | MB_OK |
                                     MB_APPLMODAL | MB_ICONHAND);
   return NULL;
   }


HBUF UpdErr2 (PSZ psz, PSZ psz2, PSZ psz3, HWND hwndOwner)
   {
//   char  szTmpUE [400];

   sprintf (szTmpUE, psz, psz2, psz3);
   ItiErrWriteDebugMessage (szTmpUE);
   WinMessageBox (pglobals->hwndDesktop, hwndOwner, szTmpUE,
                  "Update Error", 0, MB_MOVEABLE  | MB_OK |
                                     MB_APPLMODAL | MB_ICONHAND);
   return NULL;
   }



MRESULT EXPENTRY DeletingProc (HWND hwnd, USHORT uMessage, MPARAM mp1, MPARAM mp2)
   {
   switch (uMessage)
      {
      case WM_CLOSE:
         WinDismissDlg (hwnd, TRUE);
         break;
      }
   return WinDefDlgProc (hwnd, uMessage, mp1, mp2);
   }





/* strip # from @#Str
 * return Str in string
 * return # as return val
 */
USHORT GetCtlId (PSZ *ppszSrcCpy)
   {
   USHORT uId;

   ItiStrip (*ppszSrcCpy, "@ \t");
   uId = atoi (*ppszSrcCpy);
   ItiStrip (*ppszSrcCpy, "0123456789 \t");
   return uId;
   }






/* this proc gets the active row for a buffer.
 * since a buffer does not know which row is considered active,
 * we must search for a ctl using that buffer.
 */
USHORT GetHbufRow (HWND hwndDlg, HBUF hbuf, PDWDAT pdwdat, USHORT uCtlId)
   {
   USHORT uRow;
   PDGI   pdgi;

   if (!uCtlId)
      {
      if ((pdgi = GetMatchingPDGI (pdwdat, NULL, NULL, hbuf))==NULL)
         return 0xFFFF;
      uCtlId = pdgi->uCtlId;
      }

   switch (CtlClassID (hwndDlg, uCtlId))
      {
      case (ULONG)WC_BUTTON:
      case (ULONG)WC_TITLEBAR:
      case (ULONG)WC_ENTRYFIELD:
      case (ULONG)WC_FRAME:
      case (ULONG)WC_STATIC:
      case (ULONG)WC_SCROLLBAR:
      case (ULONG)WC_MLE:
         uRow = 0;

      case (ULONG)WC_COMBOBOX:
      case (ULONG)WC_LISTBOX:
         uRow = (USHORT) (ULONG) WinSendDlgItemMsg (hwndDlg, uCtlId,
                                 LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0L);
         if (uRow == LIT_NONE)
            return 0xFFFF;

         /*--- as of 4/17/92 we store buffer row in extra bytes      ---*/
         /*--- this is to allow re-ordering and selective inserting  ---*/
         /*--- into list box and still be able to access the correct ---*/
         /*--- buffer row  -CLF                                      ---*/

         uRow = (UM)WinSendDlgItemMsg (hwndDlg, uCtlId,
                          LM_QUERYITEMHANDLE, MPFROMSHORT (uRow), 0);
         return uRow;

      case (ULONG)ITIWC_LIST:
         return (UM) WinSendDlgItemMsg (hwndDlg, uCtlId, WM_ITIWNDQUERY,
                                          MPFROMSHORT (ITIWND_ACTIVE), 0L);
      default:
         UpdErr ("Unable to find control type", hwndDlg);
         return 0xFFFF;
      }
   }








/* This function gets the data from a dialog control or buffer row/col
 *
 * for right now, i only verify/format edit controls
 *
 * returns:
 *   0 --- data from dialog control
 *   1 --- data from buffer with format
 *   2 --- data from buffer without format
 *   3 --- error
 */
USHORT GetDlgBufferData (HWND   hwndDlg,
                         PDWDAT pdwdat,
                         USHORT uCtlId,
                         USHORT uColId,
                         PSZ    pszColDat,
                         USHORT uMaxLen)
   {
   HBUF     hbuf;
   USHORT   usRes, uRow, uState = 0;
   ULONG    ulClass;
   HWND     hwndCtl;
   // char     szTmpGDBD [512];
   BOOL     bMatch;
   PDGI     pdgi;

   *pszColDat = '\0';

   /*--- if uCtlId != 0, a value was       ---*/
   /*--- explicitly specified in @#ColName ---*/
   if (uCtlId)
      {
      if ((pdgi = GetMatchingPDGI (pdwdat, &uCtlId, NULL, NULL)) == NULL)
         {
         // char szTmpMsg [128];

         sprintf (szTmpMsg, "Invald Ctl Id specified in update/insert query uCtlId=%d", uCtlId);
         return 3;
         }
      /*-- if they specify enum ctl, forcem to use it---*/
      bMatch = (QueryId2 (pdwdat, pdgi) ==ITIWND_ENUM || pdgi->uColId == uColId);
      hbuf = GetDlgHbuf (hwndDlg, pdwdat, &uCtlId, uColId);
      /*--- CtlId does not nexessarily have the correct uColId (bMatch)---*/
      }
   else
      {
      hbuf = GetDlgHbuf (hwndDlg, pdwdat, &uCtlId, uColId);

      /*--- if pdgi comes back!=null, then ctl uses hbuf and col ---*/
      pdgi = GetMatchingPDGI (pdwdat, &uCtlId, &uColId, hbuf);
      bMatch = (pdgi != NULL);
      /*--- CtlId  DOES necessarily have the correct uColId (if pdgi!=null)---*/
      }

   ulClass = (uCtlId ? CtlClassID (hwndDlg, uCtlId) : 0L);
   hwndCtl = (uCtlId ? WinWindowFromID (hwndDlg, uCtlId) : NULL);

   /*--- Data from Entry Field ---*/
    if (bMatch                           &&
       (ulClass == (ULONG) WC_ENTRYFIELD ||
        ulClass == (ULONG) WC_MLE        ||
        ulClass == (ULONG) WC_COMBOBOX))
      {
      WinQueryWindowText (hwndCtl, uMaxLen, szTmpGDBD);
      if (ItiFmtCheckString (szTmpGDBD, pszColDat, uMaxLen, pdgi->pszInputFmt, NULL))
         {
         UpdErr2 ("Internal Error Invalid format on verified string.\n Format=%s String=%s ",
                 pdgi->pszInputFmt, pszColDat, hwndDlg);
         return 3;
         }
      return 0;
      }

   /*--- Data from check box ---*/
   if (bMatch &&ulClass == (ULONG)WC_BUTTON)
      {
      pszColDat[0] = (CHAR)(WinSendMsg (hwndCtl, BM_QUERYCHECK, 0, 0) ? '1': '0');
      pszColDat[1] = '\0';
      return 0;
      }

   /*--- data from buffer ---*/
   if ((uRow = GetHbufRow (hwndDlg, hbuf, pdwdat, uCtlId)) == 0xFFFF)
      {
      /* return 3; */

      /*--- 10/10/91 -clf                                        ---*/
      /*--- we cannot simply return here.                        ---*/
      /*--- in the case of an optional control that is blank we  ---*/
      /*--- could end up here                                    ---*/
      /*--- we must return a null type in the correct format     ---*/

      szTmpGDBD[0] = '\0';
      }
   else
      {
      do /*--- busy wait for row to become available ---*/
         {
// WriteStackValues (hwndDlg);
         uState = ItiDbGetBufferRowCol (hbuf, uRow, uColId, szTmpGDBD);
         }
      while (uState == ITIDB_WAIT || uState == ITIDB_GRAY);
      }

   /* we get to here if:
    *   1> the column requested was not in a control
    *   2> the column requested was in a control other than edit/combo/check
    *   3> the column requested didn't match the specified ctl
    */

   /* we do the below if if:
    *   2> the column requested was in a control other than edit/combo/check
    *   3> the column requested didn't match the specified ctl
    */

   if (pdgi != NULL) /*--- there is a format conversion to be done ---*/
      {
// WriteStackValues (hwndDlg);
      // if(ItiFmtCheckString (szTmpGDBD, pszColDat, uMaxLen, pdgi->pszInputFmt, NULL))
      usRes = ItiFmtCheckString (szTmpGDBD, pszColDat, uMaxLen, pdgi->pszInputFmt, NULL);
      if (usRes != 0)
         UpdErr ("Internal Error Invalid format on verified string", hwndDlg);

      return 1;
      }
   else
      {
      strcpy (pszColDat, szTmpGDBD);
      }
   return 2;
   }/* End of Function GetDlgBufferData */






/* This procedure replaces parameters in a string
 * the parameter has the form of
 * @[CtlIdVal]ColumnNameString
 * the string is typically an update or insert command string.
 *
 * --------USAGE:   @ColumnNameString--------
 * --------EXAMPLE: @Description-------------
 *    If the parameter is in this form:
 *    1> a buffer used in the dialog box is arbitrarily selected.
 *    2> the buffer is checked to see if it contains the named column.
 *    3> if no, goto step 1.
 *    4> The data is retrieved from either:
 *       4.1> The dialog ctl if there exists a an entry/mle ctl using this
 *              buffer/column
 *       4.2> The buffer
 *    5> if a list buffer, and the data is being obtained from the buffer,
 *         the row number is obtained as follows:
 *       5.1> arbitrarily select a dialog list/combo/listwin control.
 *       5.2> if it does not use the above selected buffer, goto 4.1
 *       5.3> use this controls selection index as the buffers row index.
 *
 * --------USAGE:   @CtlIdVal ColumnNameString--------
 * --------EXAMPLE: @225 Description------------------
 *    If the parameter is in this form:
 *    1> a control is identified by the number.
 *    2> the buffer associated with this dialog control is identified.
 *    3> The data is retrieved from either:
 *       3.1> The dialog ctl if it is an entry/mle and using the correct column
 *       3.2> The buffer
 *    4> if a list buffer, the row = the selection index of the ctl.
 *
 * This function returns 0 if ok
 */
USHORT EXPORT ItiWndReplaceDlgParams (PSZ    pszDest,
                                      PSZ    pszSrc,
                                      HWND   hwndDlg,
                                      USHORT uDestSize)
   {
   // char     szColName [128];
   // char     szTable   [128];
   // char     szColValLoc  [256];
   char     szColName [64];
   char     szTable   [64];
   PSZ      pszKey, pszSrcCpy;
   USHORT   uColId, uCtlId, uWinType;
   BOOL     bReturn = 0;
   PDWDAT   pdwdatLoc;


   uWinType = (UM) QW (hwndDlg, ITIWND_TYPE, 0, 0, 0);

   pdwdatLoc = (PDWDAT)WinSendMsg (hwndDlg, WM_GETDLGDAT, 0L, 0L);

   *szTable = *pszDest  = '\0';
   pszSrcCpy = pszSrc;
   while (TRUE)
      {
      if (*pszSrcCpy == '\0')
         break;
      if (*pszSrcCpy != '@')
         {
         *pszDest++ = *pszSrcCpy++;
         continue;
         }

      pszSrcCpy++;
      *pszDest = '\0';
      uCtlId = GetCtlId (&pszSrcCpy);
      ItiGetWord (&pszSrcCpy, szColName, " ,\n\t\"'@%()", FALSE);

      /*--- @NewKey keyword ---*/
      if (!stricmp (szColName, "NewKey"))
         {
         if (*szTable == '\0')
            ItiDbGetTableFromInsert (pszSrc, szTable, sizeof szTable);

         if (*szTable == '\0')
            {
            UpdErr ("New key Requested but table not found", hwndDlg);
            bReturn = ITIERR_NOKEY;
            continue;
            }

         /*--- get the key ---*/
         if ((pszKey = ItiDbGetKey (pdwdatLoc->hheap, szTable, 1)) == NULL)
            {
            UpdErr ("Unable to get New key", hwndDlg);
            bReturn = ITIERR_NOKEY;
            continue;
            }

         /* notify the dialog box about the new key, and hope they make copies */
         WinSendMsg (hwndDlg, WM_NEWKEY, MPFROMP (szTable), MPFROMP (pszKey));

         strcat (pszDest, pszKey);
         pszDest = strchr (pszDest, '\0');
         ItiMemFree (pdwdatLoc->hheap, pszKey);
         continue;
         }


      /*--- @CurrentUserID keyword ---*/
      if (!stricmp (szColName, "CURRENTUSERID"))
         {
         strcat (pszDest, pglobals->pszUserID);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentUserName keyword ---*/
      if (!stricmp (szColName, "CURRENTUSERNAME"))
         {
         strcat (pszDest, pglobals->pszUserName);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentUserKey keyword ---*/
      if (!stricmp (szColName, "CURRENTUSERKEY"))
         {
         strcat (pszDest, pglobals->pszUserKey);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentTime keyword ---*/
      if (!stricmp (szColName, "CURRENTTIME"))
         {
         sprintf (szColValLoc, "'%.2d:%.2d:%.2d %cm'",
                  (USHORT) pglobals->pgis->hour == 0 ? 12 : 
                                   pglobals->pgis->hour > 12 ? 
                                     pglobals->pgis->hour - 12 : 
                                     pglobals->pgis->hour ,
                  (USHORT) pglobals->pgis->minutes,
                  (USHORT) pglobals->pgis->seconds,
                  (CHAR) pglobals->pgis->hour >= 12 ? 'p' : 'a');
         strcat (pszDest, szColValLoc);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentDate keyword ---*/
      if (!stricmp (szColName, "CURRENTDATE"))
         {
         sprintf (szColValLoc, "'%.2d/%.2d/%.4d'",
                  (USHORT) pglobals->pgis->month,
                  (USHORT) pglobals->pgis->day,
                  (USHORT) pglobals->pgis->year);
         strcat (pszDest, szColValLoc);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentDate keyword ---*/
      if (!stricmp (szColName, "CURRENTDATETIME"))
         {
         sprintf (szColValLoc, "'%.2d/%.2d/%.4d %.2d:%.2d:%.2d'",
                  (USHORT) pglobals->pgis->month,
                  (USHORT) pglobals->pgis->day,
                  (USHORT) pglobals->pgis->year,
                  (USHORT) pglobals->pgis->hour,
                  (USHORT) pglobals->pgis->minutes,
                  (USHORT) pglobals->pgis->seconds);
         strcat (pszDest, szColValLoc);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @MaxSpecYear KeyWord ---*/
      if (!stricmp (szColName, "MAXSPECYEAR"))
         {
         pszKey = ItiDbGetDbValue (pdwdatLoc->hheap, ITIDB_MAX_SPEC_YEAR);
         strcat (pszDest, pszKey);
         ItiMemFree (pdwdatLoc->hheap, pszKey);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurSpecYear KeyWord ---*/
      if (!stricmp (szColName, "CURSPECYEAR"))
         {
     //    pszKey = ItiDbGetDbValue (pdwdatLoc->hheap, ITIDB_CUR_SPEC_YEAR);
     //    strcat (pszDest, pszKey);
     //    ItiMemFree (pdwdatLoc->hheap, pszKey);

         strcat (pszDest, pglobals->pszCurrentSpecYear);

         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurrentUnitSys keyword; from pglobals --  95 JAN tlb */
      if (!stricmp (szColName, "CURRENTUNITSYS"))
         {
         if (pglobals->bUseMetric)
            strcat (pszDest, "1");
         else
            strcat (pszDest, "0"); 

         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @CurUnitSys KeyWord; */                               
      if (!stricmp (szColName, "CURUNITSYS"))
         {
     //    /* from database query -- 95 MAR tlb */
     //    pszKey = ItiDbGetDbValue (pdwdatLoc->hheap, ITIDB_CUR_UNIT_SYS);
     //    strcat (pszDest, pszKey);
     //    ItiMemFree (pdwdatLoc->hheap, pszKey);
     //    pszDest = strchr (pszDest, '\0');
     //    continue;

         /* from pglobals --  95 JUN tlb */
         if (pglobals->bUseMetric)
            strcat (pszDest, "1");
         else
            strcat (pszDest, "0"); 

         pszDest = strchr (pszDest, '\0');
         continue;
         }



      /*--- @MaxBaseDate KeyWord ---*/
      if (!stricmp (szColName, "MAXBASEDATE"))
         {
         pszKey = ItiDbGetDbValue (pdwdatLoc->hheap, ITIDB_MAX_BASE_DATE);
         strcat (pszDest, pszKey);
         ItiMemFree (pdwdatLoc->hheap, pszKey);
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @UndeclaredBaseDate KeyWord ---*/
      if (!stricmp (szColName, "UNDECLAREDBASEDATE"))
         {
         strcat (pszDest, "'1/1/1900'");
         pszDest = strchr (pszDest, '\0');
         continue;
         }

      /*--- @ColumnName ---*/
      if ((uColId = ItiColStringToColId (szColName)) == 0xFFFF)
         {
         // CHAR szTmpColName[255];

         sprintf (szTmpColName, "Invalid Column name requested : @%s. ColId=%d",
                  szColName, uColId);
         UpdErr (szTmpColName, hwndDlg);
         bReturn = ITIERR_NOCOL;
         continue;
         }

      if (uWinType != ITI_DIALOG)
         {
         UpdErr ("Only Dialog window querys may contain '@' characters !!!", hwndDlg);
         bReturn = ITIERR_BADBUFF;
         }

      else
         {
         GetDlgBufferData (hwndDlg, pdwdatLoc, uCtlId, uColId,
                           szColValLoc, sizeof szColValLoc);
         }

      strcat (pszDest, szColValLoc);
      pszDest = strchr (pszDest, '\0');
      }
   *pszDest = '\0';
   return bReturn;
   }







/*
 * multisel only if hwnd is a listwindow
 *
 *
 * uRowOwner is the row to use if the
 *
 * pszDest.........The new query
 * pszSrc..........The query with %key, @[num]dlgid, and {} elements to fix
 * hwndSel..list hwnd. Needed only when bAllowMultiSel=TRUE
 * hwndDlg......dialog hwnd. Needed only for @ (not needed for special @)
 * hbufOwner.......primary buffer from which to get keys
 * hbufOwner2......secondary buffer from which to get keys (assumed static)
 * uRowOwner.......if bAllowMultiSel=F this is row to get. if this is 0xFFFF
 *                 ask hwndSel for active row.
 * bAllowMultiSel..Allow key rows to be selected rows vs active rows
 * uMax............max len of dest string.
 *
 * Returns 0 of OK
 */
USHORT EXPORT ItiWndReplaceAllParams(PSZ    pszDest,
                               PSZ    pszSrc,
                               HWND   hwndSel,
                               HWND   hwndDlg,
                               HBUF   hbufOwner,
                               HBUF   hbufOwner2,
                               USHORT uRowOwner,
                               BOOL   bAllowMultiSel,
                               USHORT uMax)
   {
//   CHAR  szPreRAP    [2048] = "";
//   CHAR  szTmpRAP    [2048] = "";
//   CHAR  szPostRAP   [2048] = "";
   USHORT   uRow = 0;
   USHORT   uType, uRet;
   USHORT   uError = 0;
   BOOL     bMultiSel = FALSE;

   pszDest[0] = '\0';

   if (!pszSrc || *pszSrc == '\0')
      return ITIERR_BLANKQUERY;

   if (bAllowMultiSel && hbufOwner)
      {
      uType = (UM) QW (hwndSel, ITIWND_TYPE, 0, 0, 0);
      bMultiSel = (uType == ITI_LIST) &&
                  ((LWQuery (hwndSel, LWM_Options, FALSE) & LWS_MULTISELECT) ==
                  LWS_MULTISELECT);
      }

   /*--- NEW 1/10/91 This was added so you could get lock query ---*/
   /*--- in multisel windows with no selection                  ---*/
   if (bMultiSel &&  0xFFFF == (UM) QW (hwndSel, ITIWND_SELECTION, 0, 0, 0))
      bMultiSel = FALSE;



   if (!bMultiSel)
      {
      if ((uRow = uRowOwner) == 0xFFFF && hwndSel)
      uRow = (UM) QW (hwndSel, ITIWND_ACTIVE, 0, 0, 0);
      }

   while (pszSrc[0] != '\0')
      {
      /*--- get non clause ---*/
      ItiGetWord (&pszSrc, szPreRAP,  "{", TRUE);

      if (*szPreRAP)
         {
         if (bMultiSel)
            uRow = (UM) QW (hwndSel, ITIWND_SELECTION, 0, 0, 0);

         if (uRet = ItiDbReplaceParams (szTmpRAP, szPreRAP, hbufOwner,
                                        hbufOwner2, uRow, uMax))
            uError = (uError ? uError : uRet);

         if (uRet = ItiWndReplaceDlgParams (szPostRAP, szTmpRAP, hwndDlg,
                                            sizeof szPostRAP))
            uError = (uError ? uError : uRet);

         strcat (pszDest, szPostRAP);
         }

      /*--- get clause ---*/
      ItiGetWord (&pszSrc, szPreRAP, "}", TRUE);
      while (bMultiSel && szPreRAP[0] &&
         (uRow = (UM) QW (hwndSel, ITIWND_SELECTION, uRow +1, 0, 0)) != 0xFFFF)
         {
         if (uRet = ItiDbReplaceParams (szTmpRAP, szPreRAP, hbufOwner,
                                        hbufOwner2, uRow, uMax))
            uError = (uError ? uError : uRet);
         if (uRet = ItiWndReplaceDlgParams (szPostRAP, szTmpRAP, hwndDlg,
                                            sizeof szPostRAP))
            uError = (uError ? uError : uRet);
         strcat (pszDest, " ");
         strcat (pszDest, szPostRAP);
         strcat (pszDest, " ");
         }
      } /*-end while-*/
   return uError;
   }



/***************************************************************/
/*********** John 3:16 *****************************************/
/***************************************************************/



/*
 * This procedure will do an add, change or delete for a specified
 * window. The querys are obtained from the metadata of the correct
 * type. The query ID's are the same as the ID of hwnd
 * Use ItiWndModifyDb if you already have the query string
 *
 */
USHORT EXPORT ItiWndAutoModifyDb (HWND hwnd, USHORT uMode)
   {
//   char      szQryAMD[4096];
   HMODULE  hmod;
   USHORT   uId, uResType;

   if (!hwnd)
      {
      UpdErr ("NULL HWND in ItiWndAutoModifyDb", hwnd);
      return ITIERR_NULLHWND;
      }

   if (uMode == ITI_DELETE &&
       WinMessageBox (pglobals->hwndDesktop, hwnd,
                     "Are you sure you want to delete all selected rows?",
                     "Delete", 0, MB_YESNO | MB_DEFBUTTON2 | MB_MOVEABLE)
       != MBID_YES)
      return 666;

   switch (uMode)
      {
      case ITI_ADD:
         uResType = ITIRID_INSERT;
         break;

      case ITI_CHANGE:
         uResType = ITIRID_UPDATE;
         break;

      case ITI_DELETE:
         uResType = ITIRID_DELETE;
         break;

      default:
         UpdErr ("Invalid Modify Mode in ItiWndAutoModifyDb", hwnd);
         return ITIERR_BADMODE;
      }

   uId  = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);
   ItiMbQueryHMOD (uId, &hmod);
   if (ItiMbQueryQueryText (hmod, uResType, uId, szQryAMD, sizeof szQryAMD))
      return ITIERR_NOQUERY;

   return ItiWndModifyDb (hwnd, szQryAMD, uMode);

   } /* End of Function ItiWndAutoModifyDb */






/*
 * This procedure will do an add, change or delete for a specified
 * window. pszQuery is assumed to be the query with the replaceable
 * parameters not replaced yet. If the uMode = ITI_CHANGE, the lock
 * query will be obtained from the metadata for hwnd. If the query
 * contains '@' characters, make sure hwnd is a dialog window.  If
 * you want the querys to be obtained from the metadata automatically,
 * use ItiWndAutoModifyDb.
 *
 * if uMode = ITI_DELETE, the verify msg box should be done before
 * calling this fn.
 *
 */
USHORT EXPORT ItiWndModifyDb (HWND hwnd, PSZ pszQuery, USHORT uMode)
   {
//   char     szNewQryM [4096];
   USHORT   uRet;

   if (pszQuery == NULL || !*pszQuery)
      return 0;

   if (uRet = ItiWndPrepQuery (szNewQryM, pszQuery, hwnd, uMode))
      return uRet;

   WndSetPtr (SPTR_WAIT);

   switch (uMode)
      {
      case ITI_ADD:
         uRet = ItiWndAddDb (hwnd, szNewQryM);
         break;

      case ITI_CHANGE:
         uRet = ItiWndChangeDb (hwnd, szNewQryM);
         break;

      case ITI_DELETE:
         uRet = ItiWndDeleteDb (hwnd, szNewQryM);
         break;

      default:
         uRet = ITIERR_BADMODE;
         UpdErr ("invalid modify mode in ItiWndModifyDb (Got That?)", hwnd);
      }
   WndSetPtr (SPTR_ARROW);
   return uRet;
   } /* End of Function ItiWndModifyDb */





/*
 * This function will replace any repleable parameters for query's
 * The characters considered special are '%', '@', and '{'-'}'.
 * The '@' character can only be used when hwnd is a dialog window.
 *
 * When replacing parameters, the query will look at its parent window
 * for data to replace keys with. If the parent is a list window, the
 * parents static window will also be looked at if the parent does not
 * have the required data column.
 *
 * If the current mode is ITI_ADD the query will not be allowed to use
 * its parent windows buffer for data if it is a list window buffer. only
 * static and dialog window buffers may be used for data in this special
 * case. Think about it.
 *
 * Another exception is with deletes. it uMode = ITI_DELETE, the buffers
 * used are the hbuf of the hwnd itself and its parents hbuf
 *
 * uMode may be ITI_ADD    -- query is for an insert
 *              ITI_CHANGE -- query is an update
 *              ITI_DELETE -- query is a delete
 *
 *              ITI_STATIC -- query is for data for static buffer
 *              ITI_LIST   -- query is for data for list buffer
 *              ITI_DIALOG -- use ITI_ADD / ITI_CHANGE for dlg display qrys
 *
 */
USHORT EXPORT ItiWndPrepQuery (PSZ     pszDest,
                               PSZ     pszSrc,
                               HWND    hwnd,
                               USHORT  uMode)
   {
   HWND     hwndOwner, hwndOwner2, hwndBase;
   HBUF     hbufOwner, hbufOwner2;
   USHORT   uOwnerType, uRet;

   hwndOwner2 = hbufOwner2 = NULL;
   hwndBase   = (uMode == ITI_LIST ? WinQueryWindow (hwnd, QW_PARENT, 0): hwnd);
   hwndOwner  = (HWND) QW (hwndBase, ITIWND_OWNERWND, 0, 0, 0);
   hwndOwner  = (uMode == ITI_DELETE ? hwnd : hwndOwner);

   hbufOwner  = (HBUF) QW (hwndOwner, ITIWND_BUFFER, 0, 0, 0);
   uOwnerType = (UM) QW (hwndOwner, ITIWND_TYPE, 0, 0, 0);

   if (uOwnerType == ITI_LIST)
      {
      hwndOwner2  = (HWND) QW (hwndOwner, ITIWND_OWNERWND, 0, 0, 0);
      hbufOwner2  = (HBUF) QW (hwndOwner2, ITIWND_BUFFER, 0, 0, 0);
      }

   if (uMode == ITI_ADD && uOwnerType == ITI_LIST)
      hbufOwner = NULL;

   uRet = ItiWndReplaceAllParams (pszDest, pszSrc, hwndOwner, hwnd,
                            hbufOwner, hbufOwner2, 0xFFFF, TRUE, 4096);
   return uRet;
   }




/***************************************************************/
/***************************************************************/
/***                                                         ***/
/***                    INSERT                               ***/
/***                                                         ***/
/***************************************************************/
/***************************************************************/
/*
 * 0=no error
 */
USHORT EXPORT ItiWndAddDb (HWND hwnd, PSZ pszQuery)
   {
   char  szTable [80];
   HHEAP hheap;

   if (!ItiDbGetTableFromInsert (pszQuery, szTable, sizeof szTable))
      return ITIERR_NOTABLE;

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   if (!ItiDbBeginTransaction (hheap))
      return ITIERR_NOTRANSACTION;

   ItiErrWriteDebugMessage (pszQuery);

   /* execute the insert */
   if (ItiDbExecSQLStatement (hheap, pszQuery))
      {
      ItiDbRollbackTransaction (hheap);
      UpdErr (pszQuery, hwnd);
      return ITIERR_NOSQLEXEC;
      }

   /* commit the transaction */
   if (ItiDbCommitTransaction (hheap))
      ItiDbUpdateBuffers (szTable);
   else
      return ITIERR_NOSQLCOMMIT;

   return 0;
   }





/***************************************************************/
/***************************************************************/
/***                                                         ***/
/***                    CHANGE                               ***/
/***                                                         ***/
/***************************************************************/
/***************************************************************/

/*
 * This fn loads/preps/executes the lock query for hwnd
 * and returns the locked row
 * 0 = no error
 * !0= error code
 */
USHORT ItiWndDoQueryLock (HWND hwnd, PSZ **pppszRow, USHORT *puCols)
   {
   char     szTmp    [4096];
   char     szLockQry[4096];
   USHORT   uId, uRet;
   HMODULE  hmod;
   HHEAP    hheap;

   uId = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);
   ItiMbQueryHMOD (uId, &hmod);
   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   if (ItiMbQueryQueryText (hmod, ITIRID_LOCK, uId, szTmp, sizeof szTmp))
      return ITIERR_NOQUERY;

   if (uRet = ItiWndPrepQuery (szLockQry, szTmp, hwnd, ITI_CHANGE))
      return uRet;

   *pppszRow = ItiDbGetRow1 (szLockQry, hheap, 0, 0, 0, puCols);
   if (*pppszRow == NULL)
      return ITIERR_NOEXEC;

   return 0;
   }




ItiWndUndoXact (HHEAP   hheap,
                HLOCK   hlock,
                PSZ     *ppszLockRow,
                USHORT  uCols,
                PSZ     pszErr,
                USHORT  uRet,
                HWND    hwnd)
   {
   if (hlock)
      ItiLockFree (hlock, FALSE);

   if (hheap)
      {
      ItiFreeStrArray (hheap, ppszLockRow, uCols);
      ItiDbRollbackTransaction (hheap);
      }

   if (pszErr)
      UpdErr (pszErr, hwnd);

   return uRet;
   }


/***************************************************************/

USHORT EXPORT ItiWndChangeDb (HWND hwnd, PSZ pszQuery)
   {
   char     szTable  [80];
   char     szTmp    [160];
   USHORT   uCols, uState, i;
   HLOCK    hlock;
   HHEAP    hheap;
   PSZ      *ppszLockRow;
   PSZ      *ppszOrigRow;

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   ppszOrigRow= (PSZ *) QW (hwnd, ITIWND_LOCKROW, 0, 0, 0);

   if (!ItiDbGetTableFromInsert (pszQuery, szTable, sizeof szTable))
      return ITIERR_NOTABLE;

   if (!(hlock = ItiLockRequest ("SomeTable", NULL, NULL, 0)))
      return ITIERR_NODBLOCK;

   if (!ItiLockLock (hlock))
      return ItiWndUndoXact (NULL, hlock, NULL, 0, "You have lost your update lock!", ITIERR_NODBLOCK, hwnd);

   /*--- begin the transaction ---*/
   if (!ItiDbBeginTransaction (hheap))
      return ItiWndUndoXact (NULL, hlock, NULL, 0, NULL, ITIERR_NOTRANSACTION, hwnd);

   /*--- Lock The row and get its data ---*/
   if (ItiWndDoQueryLock (hwnd, &ppszLockRow, &uCols))
      return ItiWndUndoXact (hheap, hlock, ppszLockRow, uCols, NULL, ITIERR_NOEXEC, hwnd);

   /*--- Compare new and old rows to see if it has changed ---*/
   for (i=0; i<uCols; i++)
      if (strcmp (ppszLockRow[i], ppszOrigRow[i]))
         {
         /*--- THIS NEXT LINE WILL NOT BE NEEDED AFTER IPC IS RUNNING ---*/
         /*==============================================================*/
         /* of course, we will have to wait until the 23rd centry for that */
         ItiDbUpdateBuffers (szTable);

         ItiErrWriteDebugMessage ("\n \/\/\/\/ Aborted query \/\/\/\/ ");

         sprintf (szTmp, "This data has recently been changed by another "
                         "user on the system. "
                         "This change will be aborted. ");
         ItiErrWriteDebugMessage (szTmp);
         ItiErrWriteDebugMessage (pszQuery);
         ItiErrWriteDebugMessage (" /\/\/\/\ Aborted query /\/\/\/\ \n ");

         return ItiWndUndoXact (hheap, hlock, ppszLockRow, uCols, szTmp, ITIERR_ROWCHANGED, hwnd);
         }

   ItiErrWriteDebugMessage (pszQuery);

   /*--- execute the update ---*/
   if (uState = ItiDbExecSQLStatement (hheap, pszQuery))
      return ItiWndUndoXact (hheap, hlock, ppszLockRow, uCols, pszQuery, ITIERR_NOEXEC, hwnd);

   /*--- commit the transaction, release lock, tell lockman row changed---*/
   ItiLockFree (hlock, TRUE);
   if (uState = ItiDbCommitTransaction (hheap))
      {
      ItiDbUpdateBuffers (szTable);
      }

   return 0;
   }




/***************************************************************/
/***************************************************************/
/***                                                         ***/
/***                    DELETE                               ***/
/***                                                         ***/
/***************************************************************/
/***************************************************************/
/* return values:
 *  0 - OK
 */
USHORT EXPORT ItiWndDeleteDb (HWND hwnd, PSZ pszQuery)
   {
   HWND  hwndDel;
   HHEAP hheap;

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   hwndDel = WinLoadDlg (HWND_DESKTOP, hwnd,
                         DeletingProc, hmodModule, 100, NULL);

   if (!ItiDbBeginTransaction (hheap))
      {
      WinDestroyWindow (hwndDel);
      UpdErr ("Delete Failed - Could not begin transaction.", hwnd);
      return ITIERR_NOTRANSACTION;
      }

   if (ItiDbExecSQLStatement (hheap, pszQuery))
      {
      ItiDbRollbackTransaction (hheap);
      WinDestroyWindow (hwndDel);
      UpdErr ("Delete Failed - Could not exec transaction.", hwnd);
      return ITIERR_NOEXEC;
      }

   /* commit the transaction */
   if (ItiDbCommitTransaction (hheap))
      ItiDbUpdateDeletedBuffers (pszQuery);
   else
      {
      ItiDbRollbackTransaction (hheap);
      WinDestroyWindow (hwndDel);
      UpdErr ("Delete Failed - Could not commit transaction.", hwnd);
      return ITIERR_NOSQLCOMMIT;
      }
   WinDestroyWindow (hwndDel);
   return 0;
   }



