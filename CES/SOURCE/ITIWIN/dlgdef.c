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
 * dlgdef.c                                                             *
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
#include "..\include\winid.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\dialog.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include "..\include\itiutil.h"
#include "winmain.h"
#include "swmain.h"
#include "lwmain.h"
#include "windef.h"
#include "lwutil.h"
#include "winutil.h"
#include "winupdat.h"
#include "dlgdef.h"
#include "initdll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlfront.h>
#include <limits.h>



HBUF DlgErr (PSZ psz)
   {
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, psz,
                  "Update Error", 0, MB_MOVEABLE | MB_OK | MB_APPLMODAL);
   return NULL;
   }


USHORT QueryId (PDWDAT pdwdat, USHORT i)
   {
   if (pdwdat->uDlgMode == ITI_CHANGE)
      return pdwdat->pdgi[i].uChangeQuery;
   else
      return pdwdat->pdgi[i].uChangeQuery;
   }

USHORT QueryId2 (PDWDAT pdwdat, PDGI pdgi)
   {
   if (pdwdat->uDlgMode == ITI_CHANGE)
      return pdgi->uChangeQuery;
   else
      return pdgi->uChangeQuery;
   }



/* this gets a hbuf associated with the specified control.
 * if the specified ctl is 0, get a buffer that contains
 * the colid.
 *
 * This fn also has the side effect of setting uCtlId if it was
 * passed in as zero and a ctl matching colid was found
 */
HBUF GetDlgHbuf (HWND hwndDlg, PDWDAT pdwdat, USHORT *puCtlId, USHORT uColId)
   {
   USHORT   i;
   HBUF     hbuf;
   PDGI     pdgi;

   /*--- Specific control has been specified ---*/
   if (*puCtlId)
      {
      if ((pdgi = GetMatchingPDGI (pdwdat, puCtlId, NULL, NULL))==NULL)
         return DlgErr ("unable to find control matching control id");
      if (QueryId2 (pdwdat, pdgi) == ITIWND_ENUM)
         return NULL;
      return pdwdat->pbufdat[pdgi->uOffset].hbuf;
      }

   /*--- Specific control has not been specified ---*/
   else /* (!*puCtlId) */
      {
      /*--- first look at controls for column ---*/
      if ((pdgi = GetMatchingPDGI (pdwdat, NULL, &uColId, NULL))!=NULL)
            {
            *puCtlId = pdgi->uCtlId;
            if (QueryId2 (pdwdat, pdgi) == ITIWND_ENUM)
               return NULL;
            return pdwdat->pbufdat[pdgi->uOffset].hbuf;
            }

      /*--- now look anywhere in ctl/listwin buffers ---*/
      for (i = 0; i < pdwdat->uBuffers; i++)
         if (ItiDbColExists (hbuf = pdwdat->pbufdat[i].hbuf, uColId))
            return hbuf;

      }
   return DlgErr ("Internal Error: unable to find column using hbuf");
   }



ULONG CtlClassID2 (HWND hwnd)
   {
   CHAR     szClass[128];

   if (hwnd == NULL)
      return 0;

   WinQueryClassName (hwnd, sizeof szClass, szClass);

   if (!stricmp (szClass, "LISTWINDOW"))
      return (ULONG)ITIWC_LIST;

   return (ULONG)(atol (szClass+1));
   }


ULONG CtlClassID (HWND hwndDlg, USHORT uCtlId)
   {
   return CtlClassID2 (WinWindowFromID (hwndDlg, uCtlId));
   }





USHORT BuffTypeFromCtlId (HWND hwndDlg, USHORT uCtlId)
   {
   switch (CtlClassID (hwndDlg, uCtlId))
      {
      case (ULONG)WC_BUTTON:
      case (ULONG)WC_TITLEBAR:
      case (ULONG)WC_ENTRYFIELD:
      case (ULONG)WC_FRAME:
      case (ULONG)WC_STATIC:
      case (ULONG)WC_SCROLLBAR:
      case (ULONG)WC_MLE:
         return ITI_STATIC;

      case (ULONG)WC_COMBOBOX:
      case (ULONG)WC_LISTBOX:
      case (ULONG)WC_MENU:
      case (ULONG)ITIWC_LIST:
         return ITI_LIST;
      }
   return ITI_LIST;
   }



void SetControlText (HWND hwndDlg, USHORT uCtlId, PSZ pszStr, USHORT uIndex)
   {
   switch (CtlClassID (hwndDlg, uCtlId))
      {
      case (ULONG)WC_BUTTON:
      case (ULONG)WC_TITLEBAR:
      case (ULONG)WC_ENTRYFIELD:
      case (ULONG)WC_FRAME:
      case (ULONG)WC_STATIC:
      case (ULONG)WC_SCROLLBAR:
      case (ULONG)WC_MENU:
         WinSetDlgItemText (hwndDlg, uCtlId, pszStr);
         break;

      case (ULONG)WC_MLE:
         /* this is ugly, but it seems to work. -- mdh */
         WinSendDlgItemMsg (hwndDlg, uCtlId, MLM_SETSEL, MPFROMLONG (0),
                            MPFROMLONG (LONG_MAX));
         WinSendDlgItemMsg (hwndDlg, uCtlId, MLM_CLEAR, 0, 0);
         WinSendDlgItemMsg (hwndDlg, uCtlId, MLM_INSERT, MPFROMP (pszStr), 0);
         break;

      case (ULONG)WC_COMBOBOX:
      case (ULONG)WC_LISTBOX:
         WinSendDlgItemMsg (hwndDlg, uCtlId, LM_INSERTITEM,
                            MPFROMSHORT (LIT_END), MPFROMP (pszStr));

         WinSendDlgItemMsg (hwndDlg, uCtlId, LM_SETITEMHANDLE,
                            MPFROMSHORT (uIndex), MPFROMSHORT (uIndex));
         break;
      }
   }




void EnableDlgItem (HWND hwndDlg, USHORT uCtlId, BOOL bEnable)
   {
   HWND  hwndCtl, hwndStCtl;
   ULONG ulClass;

   hwndCtl = WinWindowFromID (hwndDlg, uCtlId);
   WinEnableWindow (hwndCtl, TRUE);
   WinSendMsg (hwndCtl, EM_SETREADONLY, MPFROMSHORT (FALSE), 0L);

   if ((hwndStCtl = WinWindowFromID (hwndDlg, uCtlId + DID_STOFFSET)) != NULL)
      WinEnableWindow (hwndStCtl, TRUE);

   if (bEnable)
      return;

   if ((ulClass = CtlClassID (hwndDlg, uCtlId)) == (ULONG)WC_ENTRYFIELD ||
       ulClass == (ULONG)WC_MLE)
      SetControlText (hwndDlg, uCtlId, "<Working>", 0);

   WinSendMsg (hwndCtl, EM_SETREADONLY, MPFROMSHORT (TRUE), 0L);
   WinEnableWindow (hwndCtl, FALSE);
   if (hwndStCtl != NULL)
      WinEnableWindow (hwndStCtl, FALSE);
   }





PDGI GetMatchingPDGI (PDWDAT pdwdat,
                      USHORT *puCtlId,
                      USHORT *puColId,
                      HBUF   hbuf)
   {
   USHORT   i;
   DGI      dgi;

   for (i = 0; i < pdwdat->uCtls; i++)
      {
      dgi = pdwdat->pdgi[i];

      if (puCtlId  != NULL && *puCtlId != 0 && dgi.uCtlId != *puCtlId)
         continue;
      if (puColId  != NULL && dgi.uColId != *puColId)
         continue;
      if (hbuf     != NULL && pdwdat->pbufdat[dgi.uOffset].hbuf != hbuf)
         continue;
      return pdwdat->pdgi + i;
      }
   return NULL;
   }





void DisableQueryItems (HWND hwndDlg, PDWDAT pdwdat, USHORT uDlgMode)
   {
   USHORT i;
   ULONG  ulClass;

   if (pdwdat == NULL)
      return;

   if (!(uDlgMode == ITI_CHANGE || uDlgMode == ITI_ADD))
      return;

   for (i = 0; i < pdwdat->uCtls; i++)
      {

      if (QueryId(pdwdat, i) == ITIWND_LIST) /*--not listwindows--*/
         continue;

      if (uDlgMode == ITI_CHANGE)
         {
         EnableDlgItem (hwndDlg, pdwdat->pdgi[i].uCtlId, FALSE);
         continue;
         }

      ulClass = CtlClassID (hwndDlg, pdwdat->pdgi[i].uCtlId);
      if (ulClass == (long)WC_COMBOBOX || ulClass == (long)WC_LISTBOX)
         EnableDlgItem (hwndDlg, pdwdat->pdgi[i].uCtlId, FALSE);
      }
   }



/* returns row index of match or 0xFFFF if
 * no match or 0xFFFE if bad ColId
 */
USHORT MatchingBufferRow (HBUF hbuf, USHORT uColId, PSZ pszDat)
   {
   USHORT  i, uRows;
   char    szColDat [512];

   uRows = (USHORT) (ULONG)ItiDbQueryBuffer (hbuf, ITIDB_NUMROWS, 0);
   if (!ItiDbColExists (hbuf, uColId))
      return 0xFFFE;

   for (i=0; i < uRows; i++)
      {
      ItiDbGetBufferRowCol (hbuf, i, uColId, szColDat);

      if (!stricmp (pszDat, szColDat))
         return i;
      }
   return 0xFFFF;
   }


USHORT MatchingEnumRow (HWND hwndDlg, PDGI pdgi, PSZ pszDat)
   {
   HWND   hwndCtl;
   USHORT i, uLen, uDatVal;
   char   szMatch [256];
   char   szItem  [256];

   uDatVal = atoi (pszDat);
   hwndCtl = WinWindowFromID (hwndDlg, pdgi->uCtlId);

   for (i = 0; i < pdgi->uOffset; i++)
      {
      WinSendMsg (hwndCtl, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof szItem),
                                                          MPFROMP (szItem));
      ItiFmtCheckString  (szItem, szMatch, sizeof szMatch, pdgi->pszInputFmt, &uLen);

      if (uDatVal == (USHORT)atoi (szMatch))
         return i;
      }
   return 0xFFFF;
   }







void SetControl (HWND hwndDlg, PDWDAT pdwdat, USHORT iCtlIndex, USHORT uCount)
   {
   HBUF     hbuf;
   CHAR     szStr [256];
   DGI      dgi;
   USHORT   i, uRowParent;

   dgi = pdwdat->pdgi[iCtlIndex];
   hbuf = pdwdat->pbufdat[dgi.uOffset].hbuf;
   switch (CtlClassID (hwndDlg, dgi.uCtlId))
      {
      case (ULONG)WC_COMBOBOX:
      case (ULONG)WC_LISTBOX:
         WinSendDlgItemMsg (hwndDlg, dgi.uCtlId, LM_DELETEALL, 0L, 0L);

         for (i=0; i < uCount; i++)
            {
            ItiDbGetBufferRowCol (hbuf, i, dgi.uColId, szStr);
            SetControlText (hwndDlg, dgi.uCtlId, szStr, i);
            }
         uRowParent = (UM) QW (pdwdat->hwndOwner, ITIWND_ACTIVE, 0, 0, 0);

         break;
      case (ULONG)WC_BUTTON:
         {

         ItiDbGetBufferRowCol (hbuf, 0, dgi.uColId, szStr);
         if (szStr[0] == '1')
            WinSendDlgItemMsg (hwndDlg, dgi.uCtlId, BM_SETCHECK, (MPARAM)1L, 0);
         break;
         }

      default:
         ItiDbGetBufferRowCol (hbuf, 0, dgi.uColId, szStr);
         SetControlText (hwndDlg, dgi.uCtlId, szStr, 0);
      }
   }


/*
 * With this fn you can tell if the user has changed the text in an entry
 * field.  This fn compares the text in the field with the buffer text
 * that was initially put in the field and returns:
 *   FALSE - no change or an error
 *   TRUE  - change
 */
BOOL EXPORT ItiWndEntryChanged (HWND hwndDlg, USHORT uCtl)
   {
   PDWDAT  pdwdat;
   USHORT  i;
   PDGI    pdgi;
   HBUF    hbuf;
   char    szTmp1 [256];
   char    szTmp2 [256];

   pdwdat = (PDWDAT) WinQueryWindowPtr (hwndDlg, 0);
   for (i = 0; i < pdwdat->uCtls; i++)
      if (pdwdat->pdgi[i].uCtlId == uCtl)
         break;
   if (i >= pdwdat->uCtls)
      return FALSE;            /*--- ctl not found in meta data ---*/
   
   pdgi = pdwdat->pdgi + i;
   hbuf = pdwdat->pbufdat[pdgi->uOffset].hbuf;

   ItiDbGetBufferRowCol (hbuf, 0, pdgi->uColId, szTmp1);

   WinQueryDlgItemText (hwndDlg, uCtl, sizeof szTmp2, szTmp2);

   return !!strcmp (szTmp1, szTmp2);
   }
                              






void LimitControlInputLength (HWND hwndDlg, PDWDAT pdwdat,
                              USHORT iCtlIndex, LONG ulClassId)
   {
   DGI      dgi;
   HBUF     hbuf;
   USHORT   uColType, uMaxLen;

   if (ulClassId != (ULONG)WC_ENTRYFIELD && ulClassId != (ULONG)WC_MLE)
      return;

   dgi    = pdwdat->pdgi[iCtlIndex];
   hbuf   = pdwdat->pbufdat[dgi.uOffset].hbuf;

   uColType = (USHORT)(ULONG)ItiDbQueryBuffer (hbuf, ITIDB_COLTYPE,
                                               dgi.uColId);
   uMaxLen  = (USHORT)(ULONG)ItiDbQueryBuffer (hbuf, ITIDB_COLMAXLEN,
                                               dgi.uColId);

   WinSendDlgItemMsg (hwndDlg, dgi.uCtlId,
      (ulClassId == (ULONG)WC_ENTRYFIELD ? EM_SETTEXTLIMIT : MLM_SETTEXTLIMIT),
      MPFROMSHORT ((uColType == SQLCHAR ? uMaxLen : 128)), 0);
   }


/*
 * This function returns false if a control is in error
 */
BOOL VerifyControls (HWND  hwndDlg, PDWDAT pdwdat)
   {
   USHORT   i, uRet;
   DGI      dgi;
   HWND     hwndCtl, hwndTxt;
   LONG     ulClass;
   char     szTmp [512];
   char     szStr [512];
   PSZ      pszErr, pszType;

   for (i = 0; i < pdwdat->uCtls; i++)
      {
      dgi = pdwdat->pdgi[i];
      ulClass = CtlClassID (hwndDlg, dgi.uCtlId);
      hwndCtl = WinWindowFromID (hwndDlg, dgi.uCtlId);
      if (ulClass != (ULONG)WC_ENTRYFIELD &&
          ulClass != (ULONG)WC_MLE &&
          ulClass != (ULONG)WC_COMBOBOX)
         continue;

      WinQueryWindowText (hwndCtl, sizeof szStr, szStr);

      /*--- check combos manually ---*/
      if (ulClass == (ULONG)WC_COMBOBOX)
         {
         uRet = 0;
         ItiClip (szStr, " ");
         if (szStr[0] == '\0' && (dgi.pszInputFmt == NULL || dgi.pszInputFmt[0] != '*'))
            uRet = ITIFMT_NULL;
         }
      else
         {
         uRet = ItiFmtCheckString (szStr, szTmp, sizeof szTmp, dgi.pszInputFmt, NULL);
         }
      if (!uRet)
         continue;

      if (uRet == ITIFMT_FORMAT_ERROR)
         sprintf (szTmp, "Invalid input format %s for Control # %d",
                     dgi.pszInputFmt, dgi.uCtlId);
      else
         {
         if (uRet == ITIFMT_NULL)
            pszErr = "Invalid blank";
         else
            pszErr = "Invalid Input";

         switch (ulClass)
            {
            case (ULONG)WC_ENTRYFIELD:
               pszType = "Entry Field ";
               break;

            case (ULONG)WC_MLE:
               pszType = "Multi Line Entry";
               break;

            case (ULONG)WC_COMBOBOX:
               pszType = "Combobox";
               break;

            case (ULONG)WC_LISTBOX:
               pszType = "ListBox";
               break;

            case (ULONG)ITIWC_LIST:
               pszType = "List Window";
               break;

            default:
               pszType = "Dialog Control";
               break;
            }
         if((hwndTxt = WinWindowFromID (hwndDlg, dgi.uCtlId+DID_STOFFSET)) == NULL ||
            !WinQueryWindowText (hwndTxt, sizeof szStr, szStr))
            strcpy (szStr, "");
         else
            ItiExtract (szStr, "~");

         sprintf (szTmp, "%s in\n%s - %s.\nCursor will be placed at control",
                  pszErr, pszType, szStr);
         }

      WinMessageBox (HWND_DESKTOP, hwndDlg, szTmp, "Dialog Verification Error",
                     0, MB_MOVEABLE | MB_OK| MB_APPLMODAL | MB_ICONHAND);

      WinSetFocus (HWND_DESKTOP, hwndCtl);
         return FALSE;
      }
   return TRUE;
   }



HWND InitFocus (HWND hwndDlg, PDWDAT pdwdat)
   {
   USHORT   i;
   DGI      dgi;
   HWND     hwndCtl;
   LONG     ulClass;

   for (i = 0; i < pdwdat->uCtls; i++)
      {
      dgi = pdwdat->pdgi[i];
      ulClass = CtlClassID (hwndDlg, dgi.uCtlId);
      if (ulClass == (ULONG)WC_ENTRYFIELD || ulClass == (ULONG)WC_MLE)
         {
         hwndCtl = WinWindowFromID (hwndDlg, dgi.uCtlId);
         WinSetFocus (HWND_DESKTOP, hwndCtl);
         return hwndCtl;
         }
      }
   return NULL;
   }




/* clears the text in edit controls
 * also sets focus to first encountered edit control
 *
 *
 */
void ClearControls (HWND  hwndDlg, PDWDAT pdwdat)
   {
   USHORT   i;
   DGI      dgi;
   HWND     hwndCtl;
   LONG     ulClass;

   for (i = 0; i < pdwdat->uCtls; i++)
      {
      dgi = pdwdat->pdgi[i];
      ulClass = CtlClassID (hwndDlg, dgi.uCtlId);
      if (ulClass == (ULONG)WC_ENTRYFIELD || ulClass == (ULONG)WC_MLE)
         {
         hwndCtl = WinWindowFromID (hwndDlg, dgi.uCtlId);
         WinSetWindowText (hwndCtl, "");
         }
      }
   }



/*
 * This function will activate a row in a dialogs listwindow
 * or combobox
 *
 * for rigth now
 * if the hwnd is another dialog window,
 * the query to be used will be the main query for that dialog window only
 *
 * hwndDlg  - current dialog window
 * uCtl     - ctl id to select row of
 * uCtlCol  - ctl buffer col to match              0=col in metadata
 * hwnd     - window with hbuf to match on         Null=parents window
 * uBuffCol - col in hbuf to match on              0=same as uctlcol
 *
 * returns:  0: ok
 *           1: No match
 *           2: bad dlg mode         only works in change mode
 *           3: bad control          puts up error
 *           4: bad matching hwnd    puts up error
 *
 */

USHORT EXPORT ItiWndActivateRow (HWND   hwndDlg,  // dialog window
                                 USHORT uCtl,     // ctl id to select row in
                                 USHORT uCtlCol,  // col to match

                                 HWND   hwnd,     // window with hbuf to match
                                 USHORT uBuffCol) // col in hbuf to match
   {
   PDWDAT   pdwdat;     //
   HBUF     hbufCtl,    // buffer of control Null if an enumeration
            hbuf;       // buffer to match with
   USHORT   uBuffRow,   // row in match buffer
            uState,     //
            uMatch;     // The matching Row
   PDGI     pdgi;       //
   char     szColDat [512];


   pdwdat = (PDWDAT)WinSendMsg (hwndDlg, WM_GETDLGDAT, 0L, 0L);

   /*---Make sure we are in change mode---*/
   if (pdwdat->uDlgMode != ITI_CHANGE)
      return 2;

   /*---Make sure ctl exists in the metadata---*/
   if ((pdgi = GetMatchingPDGI (pdwdat, &uCtl, NULL, NULL)) == NULL)
      {
      DlgErr ("Bad control id in activate row");
      return 3;
      }

   /*---Get hbuf of control and its column to match on---*/
   if (QueryId2 (pdwdat, pdgi) == ITIWND_ENUM)  /*--- Enumeration ---*/
      {
      hbufCtl = NULL;
      uCtlCol = pdgi->uColId;
      }
   else                                       /*--- Non Enumeration ---*/
      {
      hbufCtl = pdwdat->pbufdat[pdgi->uOffset].hbuf;
      uCtlCol = (uCtlCol ? uCtlCol : pdgi->uColId);
      }

   /*---Get info about the buffer to match with---*/
   hwnd = (hwnd != NULL ? hwnd : pdwdat->hwndOwner);
   hbuf = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);
   uBuffCol = (uBuffCol ? uBuffCol : uCtlCol);
   uBuffRow = (UM) QW (hwnd, ITIWND_ACTIVE, 0, 0, 0);
   if (hbuf == NULL || uBuffRow == 0xFFFF)
      {
      DlgErr ("Null hbuf or no active row to match in ActivateRow");
      return 4;
      }

   /*---Make sure the column is valid for the match buffer---*/

   if (!ItiDbColExists (hbuf, uBuffCol))
      {
      DlgErr ("Bad column id in match buffer in ActivateRow");
      return 4;
      }

   /*---Make sure the column is valid for the control buffer---*/
   if (QueryId2 (pdwdat, pdgi) != ITIWND_ENUM && !ItiDbColExists (hbufCtl, uCtlCol))
      {
      DlgErr ("Bad col id in ctl buffer in ActivateRow");
      return 3;
      }

   /*--- Get Row from Match Buffer at uRow, uQueryCol (busy wait)---*/
   do
      uState = ItiDbGetBufferRowCol (hbuf, uBuffRow, uBuffCol, szColDat);
   while (uState == ITIDB_WAIT || uState == ITIDB_GRAY);

   /*---return if no data to match (deleted reference?)---*/
   if (szColDat[0] == '\0')
      return 1;

   /*---find match in control---*/
   if (QueryId2 (pdwdat, pdgi) == ITIWND_ENUM)
      uMatch = MatchingEnumRow (hwndDlg, pdgi, szColDat);
   else
      uMatch = MatchingBufferRow (hbufCtl, uCtlCol, szColDat);

   /*---Return if no match---*/
   if (uMatch >= 0xFFFEU)
      return 1;

   /*---Select matching row---*/
   if (CtlClassID (hwndDlg, uCtl) != (ULONG)ITIWC_LIST)
      WinSendDlgItemMsg (hwndDlg, uCtl, LM_SELECTITEM,
                           MPFROMSHORT (uMatch), (MPARAM)TRUE);
   else
      WinSendDlgItemMsg (hwndDlg, uCtl, WM_ITIWNDSET,
                        MPFROMSHORT (ITIWND_ACTIVE), MPFROMSHORT (uMatch));
   }





/* This fn determines if the active row has changed since the call to
 * ItiWndActivateRow was called with the same parameter values
 *
 * return: FALSE : no change
 *         TRUE  : change made
 */
BOOL EXPORT ItiWndActiveRowChanged (HWND   hwndDlg,
                                    USHORT uCtl,
                                    USHORT uCtlCol,
                                    HWND   hwnd,
                                    USHORT uBuffCol)
   {
   HWND   hwndCtl;
   USHORT uCurrent, uWas;
   BOOL   bLW;

   /*---Get current row---*/
   if (!(hwndCtl = WinWindowFromID (hwndDlg, uCtl)))
      return FALSE;

   bLW = (CtlClassID (hwndDlg, uCtl) == (ULONG)ITIWC_LIST);

   if (!bLW)
      uCurrent = (UM) WinSendMsg (hwndCtl, LM_QUERYSELECTION,
                                  MPFROMSHORT (LIT_FIRST), (MPARAM)TRUE);
   else
      uCurrent = (UM) QW (hwndCtl, ITIWND_ACTIVE, 0, 0, 0);

   /*---Get would be row---*/
   ItiWndActivateRow (hwndDlg, uCtl, uCtlCol, hwnd, uBuffCol);
   if (!bLW)
      uWas = (UM) WinSendMsg (hwndCtl, LM_QUERYSELECTION,
                                  MPFROMSHORT (LIT_FIRST), (MPARAM)TRUE);
   else
      uWas = (UM) QW (hwndCtl, ITIWND_ACTIVE, 0, 0, 0);

   /*---Re Select the row that it was at fn call---*/
   if (!bLW)
      WinSendMsg (hwndCtl, LM_SELECTITEM,
                  MPFROMSHORT (uCurrent), (MPARAM)TRUE);
   else
      SW (hwndDlg, ITIWND_ACTIVE, 0, uCurrent, 0);

   return (uCurrent != uWas);
   }
                             







USHORT DoEnum (HWND hwndDlg, PDWDAT pdwdat, USHORT uIndex, PSZ pszEnum)
   {
   DGI    dgi;
   ULONG  ulClass;
   BOOL   bOK;
   USHORT i, uCount;
   PSZ    *ppszStr;

   dgi = pdwdat->pdgi[uIndex];

   if ((ulClass = CtlClassID (hwndDlg, dgi.uCtlId)) != (ULONG)WC_COMBOBOX &&
       ulClass != (ULONG)WC_LISTBOX)
      return 1;

   EnableDlgItem (hwndDlg, dgi.uCtlId, TRUE);
   bOK = ItiFmtGetEnumData (pdwdat->hheap, pszEnum, NULL, &ppszStr,
                      NULL, &uCount);
   if (!bOK)
      return 2;

   for (i = 0; i < uCount; i++)
      WinSendDlgItemMsg (hwndDlg, dgi.uCtlId, LM_INSERTITEM,
                         MPFROMSHORT (LIT_END), MPFROMP (ppszStr[i]));

   ItiFmtFreeEnumData (pdwdat->hheap, NULL, ppszStr, NULL, uCount);
   pdwdat->pdgi[uIndex].uOffset = uCount;
   return 0;
   }





MRESULT DoInitQuery (HWND hwnd, PDWDAT pdwdat, MPARAM mp1, MPARAM mp2)
   {
   CHAR    szStr [1024], szQuery [1024];
   USHORT  uBuffId, i, j, uOwnerId;
   USHORT  uCtlId, uBuffType, uNumRows, uCtls;
   USHORT  uRowParent;
   HBUF    hbufParent, hbufStatic, hbuf, hbufTmp;
   HWND    hwndStatic;
   PDGI    pdgi;
   PSZ     *ppszEnum;

   uRowParent = (UM) QW (pdwdat->hwndOwner, ITIWND_ACTIVE, 0, 0, 0);

   /* Get the parent windows static buffer
    * This is different from the parent buffer in most cases.
    * This buffer is used as a fallback buffer if the needed
    * keys cannot be obtained from the parent buffer
    */
   hbufStatic = NULL;
   uOwnerId = WinQueryWindowUShort (pdwdat->hwndOwner, QWS_ID);
   if (ItiMbQueryWindowType (uOwnerId) == ITI_LIST)
      {
      hwndStatic = WinQueryWindow (pdwdat->hwndOwner, QW_PARENT, 0);
      hbufStatic = (HBUF) QW (hwndStatic, ITIWND_BUFFER, 0, 0, 0);
      }

   for (i = 0; i < pdwdat->uBuffers; i++)
      {
      /*--- find a ctl using this buffer ---*/
      uCtlId = 0;
      for (j = 0; j < pdwdat->uCtls; j++)
         if (pdwdat->pdgi[j].uOffset == i)
            uCtlId = pdwdat->pdgi[j].uCtlId;

      hbufParent = pdwdat->pbufdat[i].hbufParent;
      uBuffId = pdwdat->pbufdat[i].uBuffId;

      /*--- This is a listwindow ---*/
      if (uBuffId == ITIWND_LIST)
         {
         HWND hwndCtl = WinWindowFromID (hwnd, uCtlId);

         if (hwndCtl == NULL)
            return DlgErr ("hwndCtl is NULL! Did you remember to register your list window?");
         ItiWndKickListWindow (hwndCtl, uCtlId);
         WinSendMsg (hwndCtl, WM_INITQUERY, MPFROMP (hbufParent),
                     MPFROM2SHORT(uRowParent, 0));
         hbuf = (HBUF) QW (hwndCtl, ITIWND_BUFFER, 0, 0, 0);
         pdwdat->pbufdat[i].hbuf    = hbuf;
         pdwdat->pbufdat[i].uBuffId = uCtlId;
         }

      /*--- This is a normal control ---*/
      else
         {
         ItiMbQueryQueryText (pdwdat->hmod, ITIRID_WND, uBuffId, szStr, sizeof szStr);

         hbufTmp = (pdwdat->uDlgMode == ITI_ADD ? NULL : hbufParent);

         ItiWndReplaceAllParams (szQuery, szStr, NULL, NULL, hbufTmp,
                           hbufStatic, uRowParent, FALSE, sizeof szQuery);

//         ItiDbReplaceParams (szQuery, szStr, hbufTmp,
//                             hbufStatic, uRowParent, sizeof szQuery);
         uBuffType = BuffTypeFromCtlId (hwnd, uCtlId);

         hbuf = ItiDbCreateBuffer (hwnd, uBuffId, uBuffType | ITI_DIALOG, szQuery, &uNumRows);
         pdwdat->pbufdat[i].hbuf = hbuf;
         }
      }
   /*--- check for enums ---*/
   ItiMbGetDialogInfo  (pdwdat->hmod, ITIRID_WND, pdwdat->uResId,
                        pdwdat->hheap, &pdgi, &ppszEnum, &uCtls);
   for (i = 0; i < pdwdat->uCtls; i++)
      {
      if (QueryId(pdwdat, i) == ITIWND_ENUM)
         DoEnum (hwnd, pdwdat, i,ppszEnum[i]);
      }
   ItiFreeStrArray (pdwdat->hheap, ppszEnum, uCtls);
   ItiMemFree (pdwdat->hheap, pdgi);
   WinInvalidateRect (hwnd, NULL, TRUE);
   return 0;
   }


/*WM_QUERYFOCUSCHAIN */
/*WM_QUERYFOCUSCHANGE */
/*WM_FOCUSCHAIN */

PFNWP pfnComboHandler;
BOOL  bComboErr = FALSE;

WNDPROC EXPORT CheckComboProc (HWND hwnd, USHORT uMsg,
                               MPARAM mp1, MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_SETFOCUS:
         {
         USHORT uRows, i, uLen;
         CHAR   szStr[256];
         CHAR   szCmp[256];
         HWND   hwndDlg, hwndCombo;

         /*---hwnd is gaining focus---*/
         if (SHORT1FROMMP (mp2))
            break;

         /*---hwnd is losing focus---*/
         bComboErr = FALSE;
         hwndCombo = WinQueryWindow (hwnd, QW_OWNER, 0);
         uRows = (USHORT)(ULONG)WinSendMsg (hwndCombo, LM_QUERYITEMCOUNT, 0, 0);
         WinQueryWindowText (hwnd, sizeof szStr, szStr);
         if (szStr[0] == '\0' || !uRows)
            return pfnComboHandler (hwnd, uMsg, mp1, mp2);

         /*--- check for exact match ---*/
         for (i = 0; i < uRows; i++)
            {
            WinSendMsg (hwndCombo, LM_QUERYITEMTEXT,
                        MPFROM2SHORT (i, sizeof szCmp),
                        MPFROMP (szCmp));

            if (!stricmp (szCmp, szStr))
               {
               WinSendMsg (hwndCombo, LM_SELECTITEM, MPFROMSHORT (i),
                           (MPARAM)1);

               return pfnComboHandler (hwnd, uMsg, mp1, mp2);
               }
            }
         /*--- check for partial match ---*/
         uLen = strlen (szStr);
         for (i = 0; i < uRows; i++)
            {
            WinSendMsg (hwndCombo, LM_QUERYITEMTEXT,
                        MPFROM2SHORT (i, sizeof szCmp),
                        MPFROMP (szCmp));

            if (!strnicmp (szCmp, szStr, uLen))
               {
//               WinSetWindowText (hwnd, szCmp);
               WinSendMsg (hwndCombo, LM_SELECTITEM, MPFROMSHORT (i),
                           (MPARAM)1);
               return pfnComboHandler (hwnd, uMsg, mp1, mp2);
               }
            }


         DosBeep (275,50);
         bComboErr = TRUE;
         hwndDlg = WinQueryWindow (hwndCombo, QW_PARENT, FALSE);
         WinPostMsg (hwndDlg, WM_ITIWNDRESETFOCUS, MPFROMHWND (hwnd), 0);
         return 0;
         }
      }
   return pfnComboHandler (hwnd, uMsg, mp1, mp2);
   }





static void HookCombos (HWND hwndDlg, PDWDAT pdwdat)
   {
   USHORT i, uCtlId;
   HWND   hwndCombo, hwndComboChild;
   HENUM  henum;

   for (i = 0; i < pdwdat->uCtls; i++)
      {
      uCtlId = pdwdat->pdgi[i].uCtlId;

      if (CtlClassID (hwndDlg, uCtlId) != (LONG)WC_COMBOBOX)
         continue;

      if ((hwndCombo = WinWindowFromID (hwndDlg, uCtlId))==NULL)
         continue;


      henum = WinBeginEnumWindows (hwndCombo);
      while ((hwndComboChild = WinGetNextWindow (henum)) != NULL)
         {
         if (CtlClassID2 (hwndComboChild) == (LONG)WC_ENTRYFIELD)
            break;
         }
      WinEndEnumWindows (henum);

      if (hwndComboChild == NULL)
         continue;

      pfnComboHandler = (PFNWP) WinQueryWindowPtr (hwndComboChild, QWP_PFNWP);
      WinSetWindowPtr (hwndComboChild, QWP_PFNWP, (PVOID)CheckComboProc);
      }
   }



/*
 * This function provides a way to hook ENTRYFIELDS and COMBOBOXES
 * in dialog boxes.  This function must be called to do this because
 * of the nonstandard way these controls are sometimes used.
 *
 * return values:
 *    0 - OK
 *    1 - incorrect ctl type (EDIT and COMBO only)
 *    2 - unable to hook
 *    3 - Incorrect mood setting of the METAPHYSICS environment variable
 *
 *    pfnNextProc - next window procedure to call in message chain
 *        This value should be stored in a static and called in the
 *        default case for the window proc you provide.
 *
 *    If the hwnd is a COMBOBOX control:
 *       1> ALL COMBOBOXES ARE HOOKED.
 *          (Check the hwnd in pfnHookProc to make sure its the one you want)
 *       2> All hooking is done after my validation hooks.
 *       3> subsequent calls are added to the top of the chain (after mine).
 *
 *    If the hwnd is a ENTRYFIELD control:
 *       1> only the specified entryfield is hooked.
 *       2> subsequent calls are added to the top of the chain (after mine).
 *
 *    If you want to hook a combobox before my validation hook:
 *       1> enumerate to find the ENTRYFIELD associated with the combobox.
 *       2> call ItiWndAddHook with that ENTRYFIELD hwnd.
 *          This will only hook that one COMBOBOX.
 *
 */

USHORT EXPORT ItiWndAddHook (HWND hwndCtl,
                             PFNWP pfnHookProc,
                             PFNWP *pfnNextProc)
   {
   if (CtlClassID2 (hwndCtl) == (LONG)WC_ENTRYFIELD)
      {
      *pfnNextProc = (PFNWP) WinQueryWindowPtr (hwndCtl, QWP_PFNWP);
      WinSetWindowPtr (hwndCtl, QWP_PFNWP, (PVOID)pfnHookProc);
      return 0;
      }

   if (CtlClassID2 (hwndCtl) == (LONG)WC_COMBOBOX)
      {
      *pfnNextProc    = pfnComboHandler;
      pfnComboHandler = pfnHookProc;
      return 0;
      }
   return 1;
   }










static MRESULT ProcessDialogQuery (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   PDWDAT  pdwdat;
   USHORT  x, uBuffId;

   pdwdat = (PDWDAT) WinQueryWindowPtr (hwnd, 0);
   switch (SHORT1FROMMP (mp1))
      {
      case ITIWND_BUFFER:
         uBuffId = (mp2 ? SHORT1FROMMP (mp2): pdwdat->uDlgId);
         for (x = 0; x < pdwdat->uBuffers; x++)
            {
            if (pdwdat->pbufdat[x].uBuffId == uBuffId)
               return (MRESULT) (pdwdat->pbufdat[x].hbuf);
            }
         return (MRESULT) (pdwdat->pbufdat[0].hbuf);

      case ITIWND_ACTIVE:
         return 0;

      case ITIWND_HEAP:
         return (MRESULT) (pdwdat->hheap);

      case ITIWND_ID:
         return (MRESULT) (pdwdat->uDlgId);

      case ITIWND_OWNERWND:
         {
         HWND hwndParent;

         hwndParent = (HWND) (pdwdat->hwndOwner);
         if (!(x = SHORT2FROMMP (mp1)))
            return hwndParent;
         return QW (hwndParent, ITIWND_OWNERWND, x-1, 0, 0);
         }

      case ITIWND_PSTRUCT:
         return (MRESULT) pdwdat;

      case ITIWND_TYPE:
         return (MRESULT) ITI_DIALOG;

      case ITIWND_OWNERLISTBUFFER:
         {
         return (MRESULT) QW (pdwdat->hwndOwner, ITIWND_BUFFER, 0, 0, 0);
         }

      case ITIWND_OWNERSTATICBUFFER:
         {
         HWND hwndStatic = WinQueryWindow (pdwdat->hwndOwner, QW_PARENT, 0);
         return (MRESULT) QW (hwndStatic, ITIWND_BUFFER, 0, 0, 0);
         }

      /* mp1l = ITIWND_DATA                  */
      /* mp1h = buffid (0= dlg main query)   */
      /* mp2l = row                          */
      /* mp2h = col                          */
      case ITIWND_DATA:
         {
         USHORT   uRow, uCol, uNumRows;
         CHAR     szTmp [256];
         HBUF     hbuf;

         hbuf = ProcessDialogQuery (hwnd, MPFROMSHORT (ITIWND_BUFFER),
                                          MPFROMSHORT (SHORT2FROMMP (mp1)));
         if (hbuf == NULL)
            return NULL;

         uRow = SHORT1FROMMP (mp2);
         uCol = SHORT2FROMMP (mp2);

         uNumRows = (USHORT)(ULONG)ItiDbQueryBuffer (hbuf, ITIDB_NUMROWS, 0);
         if (uRow >= uNumRows)
            return NULL;

         if (!ItiDbColExists(hbuf, uCol))
            return NULL;

         ItiDbGetBufferRowCol (hbuf, uRow, uCol, szTmp);

         return MRFROMP (ItiStrDup (pdwdat->hheap, szTmp));
         }

      case ITIWND_LOCKROW:
         return (MRESULT) (pdwdat->ppszLockRow);

      default:
         return 0;
      }
   return 0;
   }












/*
 * This procedure is the default window proc for dialog windows.
 * It in turn calls ItiWndDefWndProc.
 * If you further subclass a dialog window, this should be the
 * procedure you call in the default case.
 */

WNDPROC EXPORT ItiWndDefDlgProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   PDWDAT  pdwdat;

   pdwdat = (PDWDAT) WinQueryWindowPtr (hwnd, 0);

   switch (umsg)
      {
      case WM_INITDLG:
         {
         USHORT uTmp, uRet;
         CHAR   szTitle [255];
         CHAR   szNewTitle [255];
         HWND   hwndAddMore;
         HWND   hwndCopy;
         HPOINTER hptr;

         pdwdat = (PDWDAT) PVOIDFROMMP (mp2);
         WinSetWindowPtr (hwnd, 0, pdwdat);

         DisableQueryItems (hwnd, pdwdat, pdwdat->uDlgMode);
         pdwdat->uDlgId = WinQueryWindowUShort (hwnd, QWS_ID);

         /*--- UPDATE DIALOG TITLE IF ADD OR CHANGE ---*/
         if (pdwdat->uDlgMode == ITI_ADD || pdwdat->uDlgMode == ITI_CHANGE)
            {
            WinQueryWindowText (hwnd, sizeof szTitle, szTitle);
            strcpy (szNewTitle, (pdwdat->uDlgMode == ITI_ADD ? "Add " : "Change "));
            strcat (szNewTitle, szTitle);
            WinSetWindowText (hwnd, szNewTitle);
            }

         /*--- this fn starts any querys used in the dialog box ---*/
         pdwdat->uDoneQueryCount = 0;
         HookCombos (hwnd, pdwdat);
         bComboErr = FALSE;
         WinPostMsg (hwnd, WM_INITQUERY, 0, 0);
         hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE);
         WinSetPointer (HWND_DESKTOP, hptr);

         /*--- if in change mode ---*/
         if (pdwdat->uDlgMode == ITI_CHANGE)
            {
            /*--- get buffer row -- not freed until hheap is killed ---*/
            uRet = ItiWndDoQueryLock (hwnd, &(pdwdat->ppszLockRow), &uTmp);
            if (uRet && uRet != ITIERR_NOQUERY)
               DlgErr ("Unable to get lock row!");

            /*--- remove addmode buttons ---*/
            if (hwndAddMore = WinWindowFromID (hwnd, DID_ADDMORE))
               WinShowWindow (hwndAddMore, FALSE);
            }

         /*--- if in add mode, tlb 95MAR ---*/
         if (pdwdat->uDlgMode == ITI_ADD)
            {
            /*--- remove copy buttons ---*/
            if (hwndCopy = WinWindowFromID (hwnd, DID_COPY))
               WinShowWindow (hwndCopy, FALSE);
            }


         if (!pdwdat->uBuffers)
            WinPostMsg (hwnd, WM_QUERYDONE, 0, 0);
         }
         return FALSE;


      /* This message is sent from a post message about 5 lines up.
       * I do these types of things to keep it confusing.
       */
      case WM_INITQUERY:
         return DoInitQuery (hwnd, pdwdat, mp1, mp2);


/* IN the future, dialogs will need to process DBCHANGED to refill */
/* standard listboxes and comboboxes                               */
//      /*
//       * This message is sent to us from the DBBuffer module
//       * It is the result of calling ItiDbCreateBuffer.
//       */
//      case WM_ITIDBAPPENDEDROWS:
//      case WM_ITIDBGRAY:
//      case WM_ITIDBCHANGED:
//         {
//         USHORT i;
//         DGI    dgi;
//
//         for (i = 0; i < pdwdat->uCtls; i++)
//            {
//            dgi = pdwdat->pdgi[i];
//            if (QueryId(pdwdat, i) == ITIWND_LIST &&
//                PVOIDFROMMP (mp2) == pdwdat->pbufdat[dgi.uOffset].hbuf)
//               WinSendDlgItemMsg (hwnd, dgi.uCtlId, umsg, mp1, mp2);
//            }
//         CheckOtherDialogs (hwnd, umsg, mp1, mp2);
//         break;
//         }

      case WM_ITIDBBUFFERDONE:
         {
         USHORT i, uBuffId;
         ULONG  ulClassId;
         HBUF   hbuf /*, hbufParent*/;
         DGI    dgi;

         hbuf = PVOIDFROMMP (mp2);
         for (i = 0; i < pdwdat->uCtls; i++)
            {
            dgi = pdwdat->pdgi[i];
            ulClassId = CtlClassID (hwnd, dgi.uCtlId);

            if (QueryId(pdwdat, i) == ITIWND_ENUM)
               continue;

            if (hbuf != pdwdat->pbufdat[dgi.uOffset].hbuf)
               continue;

            /*--- normal controls ---*/
            EnableDlgItem (hwnd, pdwdat->pdgi[i].uCtlId, TRUE);
            LimitControlInputLength (hwnd, pdwdat, i, ulClassId);
            uBuffId = pdwdat->pbufdat[dgi.uOffset].uBuffId;


            if (pdwdat->uDlgMode  == ITI_ADD &&
               (ulClassId == (ULONG)WC_ENTRYFIELD ||
                ulClassId == (ULONG)WC_MLE        ||
                ulClassId == (ULONG)WC_BUTTON))
               continue;

            SetControl (hwnd, pdwdat, i, SHORT2FROMMP (mp1));
            }

         WinSendMsg (hwnd, WM_CHILDQUERYDONE, MPFROMSHORT (uBuffId), (MPARAM)hbuf);
         return 0;
         }

      /* this message is sent from a lw or control to its parent
       * to let it know its query is done
       * mp1l = id of buffer that finished
       */
      case WM_CHILDQUERYDONE:
         if (++pdwdat->uDoneQueryCount == pdwdat->uBuffers)
            WinPostMsg (hwnd, WM_QUERYDONE, MPFROMSHORT (pdwdat->uBuffers), mp2);
         return 0;


      /* this message is sent to the dialog (itself)
       * to let it know all its child querys are done
       * mp1l = count of buffers that finished
       */
      case WM_QUERYDONE:
         InitFocus (hwnd, pdwdat);
         return 0;

      case WM_ITIWNDQUERY:
         return ProcessDialogQuery (hwnd, mp1, mp2);

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT mr = 0;

               if (bComboErr || !VerifyControls (hwnd, pdwdat))
                  return (MPARAM) 1L;

               if (pdwdat->uDlgMode == ITI_ADD)
                  {
                  if (ItiWndAutoModifyDb (hwnd, ITI_ADD))
                     mr = (MPARAM) 1L;
                  }
               else if (pdwdat->uDlgMode == ITI_CHANGE)
                  if (ItiWndAutoModifyDb (hwnd, ITI_CHANGE))
                     mr = (MPARAM) 1L;

               if (SHORT1FROMMP (mp1) == DID_OK)
                  {
                  ItiWndSetHourGlass (TRUE);
                  WinSendMsg (hwnd, WM_ITICALC, 0, 0);
                  WinDismissDlg (hwnd, TRUE);
                  }
               ClearControls (hwnd, pdwdat);
               InitFocus (hwnd, pdwdat);
               return mr;
               }

            case DID_CANCEL:
               ItiWndSetHourGlass (TRUE);
               WinSendMsg (hwnd, WM_ITICALC, 0, 0);
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
         return 0;
         }

      case WM_HELP:
         {
         USHORT usCommand;

         usCommand = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);

         if (pglobals->hwndHelp != NULL)
            {
            WinPostMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                        MPFROMSHORT (usCommand), 0);
            return 0;
            }
         }
         break;

      case WM_GETDLGDAT:
         return MRFROMP (pdwdat);

      case WM_ITIWNDRESETFOCUS:
         WinSetFocus (HWND_DESKTOP, HWNDFROMMP(mp1));
         return 0;

      case WM_DESTROY:
         {
         USHORT i, j;
         BOOL   bDelete;

         if (pdwdat == NULL)
            break;

         for (i = 0; i < pdwdat->uBuffers; i++)
            {
            bDelete = TRUE;
            for (j = 0; j < pdwdat->uCtls; j++)
               if (pdwdat->pdgi[j].uOffset == i &&
                   QueryId(pdwdat, j) == ITIWND_LIST)
                  bDelete = FALSE;
            if (bDelete)
               ItiDbDeleteBuffer (pdwdat->pbufdat[i].hbuf, hwnd);
            }
         ItiWndSetHourGlass (FALSE);
         return 0;
         }
      }
   return WinDefDlgProc (hwnd, umsg, mp1, mp2);
   }










/* This proc will do the automatic add or change dialog box for
 * the window.
 *
 * hwnd   -- the static window
 * uWinid -- the child id containing the hbuf or 0 if using the static hbuf
 * bAdd   -- boolean add/change dialog box
 */
void EXPORT ItiWndDoDialog (HWND hwnd, USHORT uWinId, BOOL bAdd)
   {
   HWND     hwndMom;
   USHORT   uNewId, uDlgType;

   hwndMom = hwnd;
   if (uWinId > 0)
      hwndMom = WinWindowFromID (hwnd, uWinId);

   ItiWndSetDlgMode (bAdd ? ITI_ADD  : ITI_CHANGE);
   uDlgType = (bAdd ? ITIWND_ADDID : ITIWND_CHANGEID);

   uNewId   = (UM) QW (hwndMom, uDlgType, 0, 0, 0);

   if (uNewId == ITIWND_PARENT)
      {
      hwndMom = hwnd;
      uNewId = (UM) QW (hwndMom, uDlgType, 0, 0, 0);
      }

   if (uNewId)
      ItiWndBuildWindow (hwndMom, uNewId);
   }


/*
 * this function is called by a dialog box to create a new dialog box
 * or anyplace where a dlg should be created where its ID
 * is to be specified rather than looked up in the metadata
 */
void EXPORT ItiWndDoNextDialog (HWND   hwnd,      // current window
                                USHORT uParentId, // 0 - hwnd is parent
                                                  // # - this kid id is parent
                                USHORT uNewId,    // dialog id
                                USHORT uMode)     // new dlg mode
   {
   HWND     hwndParent;
   USHORT   uOldMode;

   uOldMode = ItiWndGetDlgMode ();

   /*------ THIS ------- SHOULD ------- GO ------- AWAY ------*/
   if (uOldMode == ITI_ADD)
      {
      ItiWndPrintBetaMessage (hwnd);
      return;
      }

   ItiWndSetDlgMode (uMode);
   hwndParent = (uParentId ? WinWindowFromID (hwnd, uParentId) : hwnd);
   ItiWndBuildWindow (hwndParent, uNewId);
   ItiWndSetDlgMode (uOldMode);
   }



void EXPORT ItiWndPrintBetaMessage (HWND hwnd)
   {
   WinMessageBox (pglobals->hwndDesktop,
                  pglobals->hwndAppFrame,
                  "For this release, this option is not available for ADD mode.  This option is available for CHANGE mode.",
                  "BETA LIMITATION MESSAGE",
                  0,
                  MB_MOVEABLE | MB_OK | MB_APPLMODAL | MB_ICONHAND);
   }

