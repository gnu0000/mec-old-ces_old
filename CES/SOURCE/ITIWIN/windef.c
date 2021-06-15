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
 * windef.c                                                             *
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
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "..\include\itiperm.h"
#include "initdll.h"
#include "winmain.h"
#include "swmain.h"
#include "lwmain.h"
#include "windef.h"
#include "lwutil.h"
#include "winutil.h"
#include "winupdat.h"
#include "swframe.h"
#include "defwndpr.h"
#include <stdio.h>
#include <string.h>


#define DB_DEFBUFFERSIZE  0x2000


USHORT okDefErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "ITIWIN: windef.c : %s%d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "Def Proc Error", 0, MB_OK | MB_APPLMODAL);
   return 1;
   }



char  szDefStr[] = "<blank>";
char  szInvStr[] = "<invalid>";

/*
 * This procedure gets a string from the buffer area if a buffer
 * has been created, or a blank string otherwise.
 * 0 is returned if a buffer is created, 1 otherwise.
 */

USHORT BufferString (EDT EDTData, HBUF hbuf, USHORT uRow, PSZ pszCol)
   {
   *pszCol = '\0';

   if (hbuf == NULL)
      {
      strcpy (pszCol, szDefStr);
      return 1;
      }

   if (ItiDbGetBufferRowCol (hbuf, uRow, EDTData.uIndex, pszCol) == ITIDB_VALID)
      return 0;

   strcpy (pszCol, szInvStr);
   return 1;
   }



/*
 * This procedure paints the labels and data elements in static windows.
 *
 */

USHORT PaintStaticWnd (HWND hwnd)
   {
   HPS      hps;
   RECTL    rectl, rcl;
   USHORT   i, uCoordType;
   PSWDAT   pswd;
   char     szData [255];

   if ((hps    = WinBeginPaint (hwnd, (HPS)NULL, &rectl)) == NULL)
      okDefErr ("Unable to paint Static Window", 0);
   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);

   WinFillRect (hps, &rectl, pswd->lBColor);

   /* Draw Labels */
   for (i = 0; i < pswd->uLabels; i++)
      {
      rcl         = pswd->pedtLabel[i].rcl;
      uCoordType  = pswd->pedtLabel[i].uCoordType;
      ItiWndPrepRcl (hwnd, uCoordType, &rcl);

      ItiWndDrawText (hps, -1,
                      pswd->ppszLabels[pswd->pedtLabel[i].uIndex],
                      &rcl,
                      pswd->pedtLabel[i].lFColor,
                      pswd->lBColor,
                      pswd->pedtLabel[i].lFlags);
      }

   /* Draw Data */
   for (i = 0; i < pswd->uDatas; i++)
      {
      BufferString (pswd->pedtData[i], pswd->hbuf, 0, szData);

      rcl = pswd->pedtData[i].rcl;
      uCoordType  = pswd->pedtData[i].uCoordType;
      ItiWndPrepRcl (hwnd, uCoordType, &rcl);

      ItiWndDrawText (hps, -1, szData, &rcl,    
                      pswd->pedtData[i].lFColor,
                      pswd->lBColor,
                      pswd->pedtData[i].lFlags);
      }
   WinEndPaint (hps);

   return 0;
   }




USHORT SWColIdToRelCol (HWND hwnd, USHORT u)
   {
   PSWDAT   pswd;
   USHORT   i;

   if (u >= 0x8000)
      return u & 0x7FFF;

   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);

   for (i=0; i < pswd->uDatas; i++)
      if (pswd->pedtData [i].uIndex == u)
         return i;
   return 0;
   }





MRESULT ProcessStaticQuery (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT   i, j;
   PSWDAT   pswd;
   MRESULT  Err = MPFROMSHORT (1);

   i = SHORT2FROMMP (mp1);
   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);

   switch (SHORT1FROMMP (mp1))
      {
      case ITIWND_NUMCOLS:   return MRFROMSHORT (pswd->uDatas);
      case ITIWND_LABELCOLS: return MRFROMSHORT (pswd->uLabels);
      case ITIWND_COLOR:     return MRFROMLONG  (pswd->lBColor);
      case ITIWND_ACTIVE:    return 0;
      case ITIWND_ID:        return (MRESULT) pswd->uId;
      case ITIWND_BUFFER:    return (MRESULT) pswd->hbuf;
      case ITIWND_HEAP:      return (MRESULT) pswd->hheap;
      case ITIWND_NUMKIDS:   return MRFROMSHORT (pswd->uChildren);
      case ITIWND_TYPE:      return (MRESULT) ITI_STATIC;
      case ITIWND_PSTRUCT:   return MRFROMP (pswd);



      case ITIWND_OWNERWND:
         {
         HWND     hwndParent;

         if (pswd == NULL)
            return 0;

         hwndParent = (HWND) pswd->hwndOwner;
         if (!i)
            return hwndParent;

         return QW (hwndParent, ITIWND_OWNERWND, i-1, 0, 0);
         }


      case ITIWND_SELECTION:
         return 0;

      case ITIWND_LABELEDT:
         if (i >= pswd->uLabels)
            return Err;
         return (MRESULT) &(pswd->pedtLabel[i]);

      case ITIWND_DATAEDT:
         if (i >= pswd->uDatas)
            return Err;
         return (MRESULT) &(pswd->pedtData[i]);

      case ITIWND_LABELHIDDEN:
         i = i & 0x7FFF;
         if (i >= pswd->uLabels)
            return (MRESULT) 0;
         return (MRESULT) (pswd->pedtLabel [i].lFlags & ITIWND_HIDE);

      case ITIWND_DATAHIDDEN:
         i = SWColIdToRelCol (hwnd, i);
         if (i >= pswd->uDatas)
            return (MRESULT) 0;
         return (MRESULT) (pswd->pedtData [i].lFlags & ITIWND_HIDE);

      case ITIWND_LABEL:
         if (i >= pswd->uLabels)
            return (MRESULT) NULL;
         return (MRESULT) pswd->ppszLabels[i];


      case ITIWND_KIDTYPE:
         if (i >= pswd->uChildren)
            return Err;
         return MRFROMLONG (pswd->pchdChildren[i].ulKind);

      case ITIWND_KIDID:
         if (i >= pswd->uChildren)
            return Err;
         return MRFROMLONG (pswd->pchdChildren[i].uId);

      case ITIWND_KID:
         if (i >= pswd->uChildren)
            return Err;
         return MRFROMP (WinWindowFromID (hwnd, pswd->pchdChildren[i].uId));

      case ITIWND_ADDID:
         if (!i)
            return (MRESULT)pswd->uAddId;

         for (j = 0; j < pswd->uChildren; j++)
            {
            if (pswd->pchdChildren[j].uId == i)
               return (MRESULT)pswd->pchdChildren[j].uAddId;
            }
         return 0;

      case ITIWND_CHANGEID:
         if (!i)
            return (MRESULT)pswd->uChangeId;

         for (j = 0; j < pswd->uChildren; j++)
            {
            if (pswd->pchdChildren[j].uId == i)
               return (MRESULT)pswd->pchdChildren[j].uChangeId;
            }
         return 0;

      case ITIWND_DATA:
         {
         CHAR     szTmp [256];
         USHORT   uCol;

         if (pswd->hbuf == NULL)
            return NULL;
         uCol = SHORT2FROMMP (mp2);
         if (!ItiDbColExists(pswd->hbuf, uCol))
            return NULL;
         ItiDbGetBufferRowCol (pswd->hbuf, 0, uCol, szTmp);
         return MRFROMP (ItiStrDup (pswd->hheap, szTmp));
         }

      default:
         okDefErr ("unknown WM_ITIWNDQUERY Cmd:", SHORT1FROMMP (mp1));
         return 0;
      }
   return 0;
   }





MRESULT ProcessStaticSet (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   MRESULT  Err = MPFROMSHORT (1);
   USHORT   i;
   PSWDAT   pswd;

   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
   i = SHORT2FROMMP (mp1);

   switch (SHORT1FROMMP (mp1))
      {
      case ITIWND_COLOR:
         pswd->lBColor = (long) mp2;
         if (i)
            WinInvalidateRect (hwnd, NULL, FALSE);
         break;

      case ITIWND_LABELEDT:
         if (i >= pswd->uLabels)
            return Err;
         pswd->pedtLabel[i] = *(PEDT)mp2;
//         WinInvalidateRect (hwnd, NULL, TRUE);
         WinInvalidateRect (hwnd, NULL, FALSE);
         break;

      case ITIWND_DATAEDT:
         if (i >= pswd->uDatas)
            return Err;
         pswd->pedtData[i] = *(PEDT)mp2;
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;


      case ITIWND_LABELHIDDEN:
         i = i & 0x7FFF;
         if (i >= pswd->uLabels)
            return Err;

         if (mp2)
            pswd->pedtLabel [i].lFlags |= ITIWND_HIDE;
         else
            pswd->pedtLabel [i].lFlags &= ~ITIWND_HIDE;

         break;

      case ITIWND_DATAHIDDEN:
         i = SWColIdToRelCol (hwnd, i);
         if (i >= pswd->uDatas)
            return Err;
         if (mp2)
            pswd->pedtData [i].lFlags |= ITIWND_HIDE;
         else
            pswd->pedtData [i].lFlags &= ~ITIWND_HIDE;
         break;

      case ITIWND_LABEL:
         if (i >= pswd->uLabels)
            return Err;
         if (!mp2)
            return Err;
         ItiMemFree (pswd->hheap, (pswd->ppszLabels)[i]);
         (pswd->ppszLabels)[i] = ItiStrDup (pswd->hheap, (PSZ) mp2);
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;

      case ITIWND_BUFFER:
         pswd->hbuf = (HBUF) mp2;
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;

      case ITIWND_MAXSIZE:               /* --- not done ---*/
         break;
      }
   return MRFROMSHORT (0);
   }





static MRESULT DoInitQuery (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT   uWinType, i, uJunk;
   char     szQuery[BIGSTRLEN];
   char     szStr[BIGSTRLEN];
   PSWDAT   pswd;
   HWND     hwndChild, hwndFrame, hwndStatic;
   HBUF     hbufParent, hbufStatic;
   USHORT   uRowParent;
   HMODULE  hmodWnd;
   BOOL     bNoInitKids;

   if ((pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT)) == NULL)
      okDefErr ("Unable to obtain pswd Window Ptr", 0);
   uWinType = pswd->uId;
   ItiMbQueryHMOD (uWinType, &hmodWnd);
   hbufParent  = (HBUF) PVOIDFROMMP  (mp1);
   uRowParent  =        SHORT1FROMMP (mp2);
   bNoInitKids =        SHORT2FROMMP (mp2);
   hwndFrame   = WinQueryWindow (hwnd, QW_PARENT, 0);
   hwndStatic  = WinQueryWindow (hwndFrame, QW_OWNER, 0);


   hbufStatic  = (HBUF) QW (hwndStatic, ITIWND_BUFFER, 0, 0, 0);
   
   /*--- Handle the query ---*/
   if (!ItiMbQueryQueryText (hmodWnd, ITIRID_WND,
                              uWinType, szStr, sizeof szStr) && *szStr != '\0')
      {
      if (ItiWndReplaceAllParams (szQuery, szStr, NULL, NULL, 
                                  hbufParent, hbufStatic,
                                  uRowParent, FALSE, sizeof szQuery))
         okDefErr ("Unable to obtain all replacable parameters in static query", 0);

      pswd->hbuf = ItiDbCreateBuffer (hwnd, uWinType, ITI_STATIC,
                                      szQuery, &uJunk);
      }

   /*--- Handle the title ---*/
   ItiMbQueryTitleText (hmodWnd, ITIRID_WND, uWinType, szStr, sizeof szStr);
   if (ItiDbReplaceParams (szQuery, szStr, (HBUF)-1L, (HBUF)"<working>", 0, sizeof szQuery))
      okDefErr ("Unable to obtain all replacable parameters in static title", 0);

   WinSetWindowText (hwndFrame, szQuery);

   /*--- Should we auto init the kids? ---*/
   if (bNoInitKids)
      return 0;

   /*--- now initiate querys in the child windows ---*/
   for (i = 0; i < pswd->uChildren; i++)
      {
      hwndChild = WinWindowFromID (hwnd, pswd->pchdChildren[i].uId);

      /*--- note that the child windows get same buf/row as the static ---*/
      WinSendMsg (hwndChild, WM_INITQUERY, mp1, mp2);
      }
   return 0;
   }





static MRESULT DoBufferDone (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   HMODULE  hmodWnd;
   PSWDAT   pswd;
   USHORT   uWinType;
   char     szTitle[BIGSTRLEN];
   char     szTmp  [BIGSTRLEN];
   HWND     hwndFrame;

   if ((pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT)) == NULL)
      okDefErr ("Unable to obtain pswd Window Ptr", 0);

   if (PVOIDFROMMP (mp2) != pswd->hbuf)
      return NULL;

   uWinType = pswd->uId;
   ItiMbQueryHMOD (uWinType, &hmodWnd);

   /*--- Handle the title ---*/
   ItiMbQueryTitleText (hmodWnd, ITIRID_WND, uWinType, szTmp, sizeof szTmp);
   ItiDbReplaceParams (szTitle, szTmp, pswd->hbuf, NULL, 0, sizeof szTitle);
   hwndFrame = WinQueryWindow (hwnd, QW_PARENT, 0);
   WinSetWindowText (hwndFrame, szTitle);

   SWL (hwnd, ITIWND_COLOR, TRUE, CLR_WHITE);
   }






static MRESULT DoLWMouse (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT uListId;
   HWND   hwndList, hwndMom, hwndStatic;
   USHORT uRow, uCol, uArea, uNewId;
   USHORT uDlgType;
   PEDT   pedt;
   BOOL   bSingle, bLeft;

   /*--- ID of child sending message ---*/
   /*---  uRow = SHORT1FROMMP (mp1);--- */
   uRow     = SHORT1FROMMP (mp1);
   uCol     = SHORT2FROMMP (mp1);
   uListId  = SHORT1FROMMP (mp2);
   uArea    = SHORT2FROMMP (mp2) & 0x00FF;
   bSingle  = ItiWndIsSingleClick (mp2);
   bLeft    = ItiWndIsLeftClick (mp2);
   hwndList = WinWindowFromID (hwnd, uListId);
   hwndMom  = hwndList;
   hwndStatic = WinQueryWindow (hwnd, QW_PARENT, 0);

   if (bSingle)
      return 0;

   if (bLeft)
      {
      if (uArea != LW_DATA && uArea != LW_LABEL)
         return 0;

      pedt = LWGetPEDT (hwndMom, FALSE, (uArea==LW_DATA));
      uNewId = pedt[uCol].uNextId;
      }

   else /*--!bLeft--*/
      {
//      if (!ItiPermQueryEdit (hwndStatic))
//         return 0;
//
//      uDlgType = (uArea==LW_LABELAREA || uArea==LW_LABEL ? ITIWND_ADDID : ITIWND_CHANGEID);
//
//      if (uDlgType == ITIWND_CHANGEID && uRow == 0xFFFF)
//         return 0;
//
//      ItiWndSetDlgMode (uDlgType == ITIWND_ADDID ? ITI_ADD : ITI_CHANGE);
//
//      uNewId   = (UM) QW (hwndMom, uDlgType, 0, 0, 0);
//
//      if (uNewId == ITIWND_PARENT)
//         {
//         hwndMom = hwnd;
//         uNewId   = (UM) QW (hwndMom, uDlgType, 0, 0, 0);
//         }
//
//
      if (!ItiPermQueryEdit (hwndStatic))
         return 0;

      uDlgType = (uArea==LW_LABELAREA || uArea==LW_LABEL ? IDM_ADD : IDM_CHANGE);
      WinSendMsg (hwndStatic, WM_COMMAND, MPFROMSHORT(uDlgType), MPFROMSHORT(uListId));
      return 0;
      }

   if (uNewId == NO_WINDOW)
      return 0;

   if (ItiWndBuildWindow(hwndMom,uNewId)== NULL &&
       ItiMbQueryWindowType (uNewId) != ITI_DIALOG)
      okDefErr ("Unable to create window Id=", uNewId);

   return 0;
   }






/* This fn handles dbl clicking tn the static area.
 * left  button == window link
 * right button == edit
 *
 */
static MRESULT DoStaticMouse (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   RECTL    rcl;
   USHORT   i, j;
   PSWDAT   pswd;
   LONG     ulXMouse, ulYMouse;
   EDT      edt;

   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
   ulXMouse = (LONG) SHORT1FROMMP (mp1);
   ulYMouse = (LONG) SHORT2FROMMP (mp1);

   /*--- right dbl clk on static is edit static window ---*/
   if (umsg == WM_BUTTON2DBLCLK)
      {
      if (!ItiPermQueryEdit (hwnd))
         return 0;

//      ItiWndSetDlgMode (ITI_CHANGE);
//      return ItiWndBuildWindow (hwnd,pswd->uChangeId);
//
// Send change message instead
//
      WinSendMsg (hwnd, WM_COMMAND, MPFROMSHORT(IDM_CHANGE), 0);
      return 0;
      }

   /*--- left dbl clk on static is link to window ---*/
   for (j = 0; j <= 2; j++)
      {
      for (i = 0; i < (j ? pswd->uLabels : pswd->uDatas); i++)
         {
         edt = (j ? pswd->pedtLabel[i] : pswd->pedtData[i]);
         rcl = edt.rcl;
         ItiWndPrepRcl (hwnd, edt.uCoordType, &rcl);
         if (rcl.xLeft   > ulXMouse ||  rcl.xRight  < ulXMouse ||
             rcl.yBottom > ulYMouse ||  rcl.yTop    < ulYMouse ||
             !edt.uNextId)
            continue;

         if (ItiWndBuildWindow(hwnd,edt.uNextId)==NULL &&
             ItiMbQueryWindowType (edt.uNextId) != ITI_DIALOG)
            okDefErr ("Unable to create window Id=", edt.uNextId);

         return 0;
         }
      }
   return 0;
   }









/*
 * This procedure is the default window proc for static windows.
 * It in turn calls WNDefWndProc.
 * If you further subclass a static window, this should be the
 * procedure you call in the default case.
 */

WNDPROC EXPORT ItiWndDefStaticWndProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_CHANGEQUERY:
         {
         PSWDAT   pswd;

         pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
         if (pswd->hbuf != NULL)
            ItiDbDeleteBuffer (pswd->hbuf, hwnd);
         pswd->hbuf = NULL;
         /* --- FALL THROUGH --- */
         /* --- CODE BY SuperKludge (tm) --- */
         }


      case WM_INITQUERY:    /* mp1 = parent hbuf || mp2l=parent row */
         return DoInitQuery (hwnd, mp1, mp2);


      case WM_ITIDBBUFFERDONE:
         return DoBufferDone (hwnd, mp1, mp2);


      case WM_ITIDBGRAY:
         return 0;


      case WM_ITIDBCHANGED:
         {
         PSWDAT   pswd;

         pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
         if (PVOIDFROMMP (mp2) != pswd->hbuf)
            return (MRESULT) DosBeep (234, 50);

         SWL (hwnd, ITIWND_COLOR, 0, CLR_PALEGRAY);
         return 0;
         }

      case WM_PAINT:
         PaintStaticWnd (hwnd);
         return 0;

      case WM_SIZE:
         {
         PSWDAT pswd;
         HWND   hwndChild;
         RECTL  rcl;
         PCHD   pchd;
         USHORT i;

         pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
         for (i = 0; i < pswd->uChildren; i++)
            {
            pchd = (pswd->pchdChildren + i);
            hwndChild = WinWindowFromID (hwnd, pchd->uId);
            rcl = pchd->rcl;
            ItiWndPrepRcl (hwnd, pchd->uCoordType, &rcl);
            WinSetWindowPos (hwndChild, HWND_TOP,
                           (SHORT) rcl.xLeft,   (SHORT) rcl.yBottom,
                           (SHORT) rcl.xRight - (SHORT) rcl.xLeft,
                           (SHORT) rcl.yTop   - (SHORT) rcl.yBottom,
                           SWP_MOVE | SWP_SIZE);
            }
         return 0;
         }


      case WM_BUTTON2DOWN:
         return WinSendMsg (WinQueryWindow (hwnd, QW_PARENT, 0),
                            WM_TRACKFRAME, MPFROMSHORT (TF_MOVE), NULL);

      case WM_BUTTON1DBLCLK:
      case WM_BUTTON2DBLCLK:
         return DoStaticMouse (hwnd, umsg, mp1, mp2);


//
//
//                        -------------------------------------
//                        |DATA    |DATA    |LABEL   |LABEL   |
//                        |ELEMENT |AREA    |ELEMENT |AREA    |
//      -------------------------------------------------------
//      |SingleClickLeft  |   *    |   *    |   *    |   *    |
//      |                 |        |        |        |        |
//      |SingleClickRight |  ------ Adjust Window pos ------  |
//      |                 |        |        |        |        |
//      |DoubleClickLeft  | assoc. |   *    | assoc. |   *    | 
//      |                 |        |        |        |        |
//      |DoubleClickRight | change | change |  add   |  add   |
//      |                 |        |        |        |        |
//      -------------------------------------------------------
      case WM_ITILWMOUSE:
         return DoLWMouse (hwnd, mp1, mp2);

      case WM_ITIWNDQUERY:
         return ProcessStaticQuery (hwnd, mp1, mp2);

      case WM_ITIWNDSET:
         return ProcessStaticSet (hwnd, mp1, mp2);

//      case WM_ITILWQBUTTON:
//         {
//         HWND     hwndList;
//         USHORT   uButtonId, uLWId;
//         PLWDAT   plwd;
//
//         hwndList  = HWNDFROMMP (mp2);
//         uButtonId = SHORT1FROMMP (mp1);
//         uLWId     = SHORT2FROMMP (mp1);
//         plwd      = LWGetPLWDAT (hwndList, FALSE);
//
//         ItiDbQButton (hwndList, plwd->hbuf);
//         WinInvalidateRect (hwnd, NULL, TRUE);
//         return 0;
//         }

      case WM_COMMAND:
         {
         PSWDAT pswd;

         if (SHORT1FROMMP (mp2) != CMDSRC_MENU)
            break;

         switch (SHORT1FROMMP (mp1))
            {
            case IDM_PARENT:
               pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
               if (pswd->hwndOwner);
                  WinSetActiveWindow (pglobals->hwndDesktop, pswd->hwndOwner);
               return 0;

            case IDM_NORMALSIZE:
               {
               RECTL rcl;
               HWND  hwndFrame;

               pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
               rcl = pswd->rclPos;
               hwndFrame = WinQueryWindow (hwnd, QW_PARENT, 0);

               ItiWndPrepRcl (pglobals->hwndAppClient, pswd->uCoordType, &rcl);

               /*--- size static window to what its metadata says it should be ---*/
               WinSetWindowPos (hwndFrame, HWND_TOP, 
                              (SHORT)rcl.xLeft, (SHORT)rcl.yBottom,
                              (SHORT)rcl.xRight - (SHORT)rcl.xLeft,
                              (SHORT)rcl.yTop   - (SHORT)rcl.yBottom,
                              SWP_MOVE | SWP_SIZE | SWP_SHOW |
                              SWP_ACTIVATE | SWP_RESTORE);
               return 0;
               }
               break;
            }
         break;
         }

      case WM_INITMENU:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_VIEW_MENU:
               {
               PSWDAT pswd;
         
               pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, !pswd->hwndOwner);
               return 0;
               }
               break;

            case IDM_EDIT_MENU:
               if (!ItiPermQueryEdit (hwnd))
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
                  }
               break;
            }
         break;
         }


      case WM_ITICOPY:
         ItiWndCopyIt (hwnd);
         return 0;


      case WM_CLOSE:
         WinDestroyWindow (WinQueryWindow (hwnd, QW_PARENT, FALSE));
         return 0;


      case WM_DESTROY:
         {
         PSWDAT pswd;
         
         ItiWndAxeKids (hwnd);
         pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
         if (pswd->hbuf != NULL)
            ItiDbDeleteBuffer (pswd->hbuf, hwnd);
         ItiMemDestroyHeap (pswd->hheap);
         WinSetWindowPtr (hwnd, WNDPTR_SWDAT, NULL);
         break;
         }
      }
   return ItiWndDefWndProc (hwnd, umsg, mp1, mp2);
   }









/*
 * This is the default procedure used by list windows for getting 
 * data elements.  List windows do not need a window procedure.
 * as you can see, this proc does not need to do much.
 *
 */
char  aszCol [255];

int EXPORT ItiWndDefListWndTxtProc (HWND   hwnd,
                                    USHORT uRow,
                                    USHORT uCol,
                                    PSZ    *pszTxt)
   {
   PLWDAT  plwd;

   plwd = LWGetPLWDAT (hwnd, TRUE);
   BufferString (plwd->pedtData[uCol], plwd->hbuf, uRow, aszCol);
   *pszTxt = aszCol;
   return 0;
   }





/*
 * I dont know if this is really needed at all.
 * if it is used it will/should be the sink for both
 * static and list windows.
 * 
 */
WNDPROC EXPORT ItiWndDefWndProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   return ItiWndDocDefWindowProc (hwnd, umsg, mp1, mp2);
   }

