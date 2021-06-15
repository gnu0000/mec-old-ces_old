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
 * swmain.c                                                             *
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
#define INCL_GPI
#include <stdio.h>
#include <string.h>
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itimenu.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itiutil.h"
#include "..\include\message.h"
#include "winmain.h"
#include "lwmain.h"
#include "swmain.h"
#include "lwutil.h"
#include "winutil.h"
#include "..\include\winid.h"
#include "swframe.h"
#include "windef.h"
#include "initdll.h"



HWND WndErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "ITIWIN: swmain.c : %s %d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "Win Create Error", 0, MB_OK | MB_SYSTEMMODAL);
   return NULL;
   }




HWND EXPORT ItiWndBuildWindow (HWND hwnd, USHORT uId)
   {
   RECTL    rcl = {0,0,1,1};
   USHORT   uCoordType = BL | PIX | X0ABS | Y0ABS | X1ABS | Y1ABS;
   HWND     hwndReturn;

   if (!uId)
      return NULL;

   WndSetPtr (SPTR_WAIT);
   switch (ItiMbQueryWindowType (uId))
      {
      case ITI_STATIC:
         hwndReturn = ItiWndBuildStaticWindow (hwnd, uId);
         break;
      case ITI_LIST:
         hwndReturn = ItiWndBuildListWindow (hwnd, uId, rcl, uCoordType);
         break;
      case ITI_DIALOG:
         hwndReturn = (HWND) MPFROMLONG (ItiWndBuildDialogWindow (hwnd, uId));
         break;
      default:
         hwndReturn = WndErr ("Window Id has not been registered Id=", uId);
      }
   WndSetPtr (SPTR_ARROW);
   return hwndReturn;
   }





HWND WndExists (HWND    hwndParent,
                USHORT  uWinId,
                HMODULE hmodRes,
                HBUF    hbufParent,
                USHORT  uRowParent)
   {
   HENUM  henum;
   HWND   hwndFrame, hwndClient;
   USHORT uId;
   CHAR   szStr  [1524];
   CHAR   szQuery [1524];
   PSZ    pszQ;
   HBUF   hbuf;


   /*--- Browser kludge ... By SUPERKLUDGE (tm) ---*/
   if (pglobals->hwndAppClient == HWND_DESKTOP)
      return NULL;

   if (ItiMbQueryQueryText (hmodRes, ITIRID_WND, uWinId,
                            szStr, sizeof szStr))
      szQuery[0] = '\0';
   else
      ItiDbReplaceParams (szQuery,    szStr,
                          hbufParent, (HBUF)NULL,
                          uRowParent, sizeof szQuery);
   ItiStrip (szQuery, " \t\n");
   henum = WinBeginEnumWindows (pglobals->hwndAppClient);
   while ((hwndFrame = WinGetNextWindow (henum)) != NULL)
      {
      hwndClient = WinWindowFromID (hwndFrame, FID_CLIENT);
      if (hwndClient == NULL)
         continue;

      uId  = (UM) QW (hwndClient, ITIWND_ID, 0, 0, 0);
      if (uId != uWinId)
         continue;

      hbuf = (HBUF) QW (hwndClient, ITIWND_BUFFER, 0, 0, 0);
      pszQ = (PSZ)ItiDbQueryBuffer (hbuf, ITIDB_QUERY, 0);
      if (((pszQ == NULL ||
           *pszQ == '\0') && szQuery[0] == '\0') ||
          !stricmp (pszQ, szQuery))
         {
         WinEndEnumWindows (henum);
         return hwndClient;
         }
      }
   WinEndEnumWindows (henum);
   return NULL;
   }





HWND GoAway (PSZ pszErr, USHORT uId, HHEAP hheap)
   {
   if (pszErr != NULL)
      WndErr (pszErr, uId);
   ItiMemDestroyHeap (hheap);
   return NULL;
   }




/*--- return value is used as a bool ---*/
/*--- null = error, non null = ok but has no other meaning ---*/

HWND GetMetaData (PSWDAT pswd, HMODULE hmod, USHORT uId, USHORT *puOptions)
   {
   SWMET       swmet;
   PSZ         *ppszTmp;
   HHEAP       hheap;

   /*--- Fill In All The MetaData ---*/
   pswd->uId     = uId;
   pswd->hbuf    = NULL;
   hheap = pswd->hheap;

   if (ItiMbQueryStaticMetrics (hmod, ITIRID_WND, uId, &swmet))
      return GoAway (NULL, uId, hheap);
   pswd->rclPos      = swmet.rcl;
   pswd->uCoordType  = swmet.uCoordType;
   pswd->lBColor     = swmet.lBColor;
   pswd->uAddId      = swmet.uAddId;
   pswd->uChangeId   = swmet.uChangeId;

   *puOptions          = swmet.uOptions;


   if (ItiMbGetLabelInfo (hmod, ITIRID_WND, uId, hheap, &pswd->pedtLabel,
                          &pswd->ppszLabels, &pswd->uLabels))
      return GoAway (NULL, uId, hheap);

   if (ItiMbGetDataInfo (hmod, ITIRID_WND, uId, hheap, &pswd->pedtData,
                          &ppszTmp,  &pswd->uDatas))
      return GoAway (NULL, uId, hheap);
   ItiFreeStrArray (hheap, ppszTmp, pswd->uDatas);

   if (ItiMbGetChildInfo (hmod, ITIRID_WND, uId, hheap, &pswd->pchdChildren,
                               &pswd->uChildren))
      return GoAway (NULL, uId, hheap);

   return (HWND) TRUE;
   }






/*
 * This fn creates a window from the window's ID
 * This fn also creates child (list and clt) windows of the window.
 *
 * This is the proc to kick off a new static window.
 * Currently it's error checking is insufficient.
 * this proc calls the procedure for building the child list
 * windows. pglobals->hwndAppClient
 */
    /* client class has memory for 1 pointer:
     *          ptr to SWDAT,
     *  class proc should be the default static win proc
     *  since the winproc is always subclassed
     */
HWND EXPORT ItiWndBuildStaticWindow (HWND hwnd, USHORT uId)
   {
   PSWDAT      pswd;
   PFNWP       wpfnWndProc;
   USHORT      i, uOptions, uRowParent;
   HWND        hwndFrame, hwndClient;
   RECTL       rcl;
   HHEAP       hheap;
   HMODULE     hmod;
   CHD         chd;
   CHAR        szTitle [255];
   CHAR        szTmp [255];
   HBUF        hbufParent;


   /*--- Create The required memory for the new window ---*/
   if ((hheap  = ItiMemCreateHeap (SW_INITIALHEAPSIZE)) == NULL)
      return WndErr ("Unable To Create Heap For Window. Id:", uId);

   /*--- Get The window proc and hmod for the window ---*/
   if ((wpfnWndProc = ItiMbStaticProc (uId, &hmod)) == NULL)
      return GoAway ("Unable to obtain proc for static window, Id:", uId, hheap);

   hbufParent = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);
   uRowParent = (UM)   QW (hwnd, ITIWND_ACTIVE, 0, 0, 0);

   ItiMbQueryTitleText (hmod, ITIRID_WND, uId, szTmp, sizeof szTmp);
   ItiDbReplaceParams (szTitle, szTmp, (HBUF)-1L, (HBUF)"<working>", 0, sizeof szTitle);

   if ((hwndClient = WndExists (hwnd, uId, hmod, hbufParent, uRowParent)) != NULL)
      {
      ItiMemDestroyHeap (hheap);
      WinSetActiveWindow (HWND_DESKTOP, hwndClient);
      return hwndClient;
      }


   /*--- Allocate the info structure that all static windows use ---*/
   if ((pswd = (PSWDAT) ItiMemAlloc (hheap, sizeof (SWDAT), 0)) == NULL)
      return GoAway ("Unable to alloc mem for static window (PSWDAT), Id:", uId, hheap);

   pswd->hheap     = hheap;
   pswd->hwndOwner = (hwnd == pglobals->hwndAppClient ? NULL : hwnd);
   pswd->pExtra    = NULL;

   if (GetMetaData (pswd, hmod, uId, &uOptions) == NULL)
      return NULL;

   hwndFrame = WinCreateStdWindow(pglobals->hwndAppClient,
                                  WS_CLIPCHILDREN|WS_VISIBLE|FS_BORDER|FS_NOBYTEALIGN,
                                  (PULONG) &uOptions, ITIWND_STATICCLASS,
                                  "", WS_VISIBLE|WS_CLIPCHILDREN, hmod, 0, &hwndClient);
   if (hwndFrame == NULL)
      {
      ItiErrDisplayWindowError (WinQueryAnchorBlock(pglobals->hwndAppClient), NULL, 0);
      return GoAway (NULL, uId, hheap);
      }

   /*---this must be done immediately after window creation ---*/
   if ((WinSubclassWindow (hwndClient, wpfnWndProc)) == 0L)
      return GoAway ("Unable to subclass client window. Id:", uId, hheap);

   /*--- make statics start out gray until they get thier data ---*/
   pswd->lBColor = CLR_PALEGRAY;

   /*--- put static info struct in window extra bytes ---*/
   if ((WinSetWindowPtr (hwndClient, WNDPTR_SWDAT, pswd)) == FALSE)
      return GoAway ("Unable to set window ptr (WNDPR_SWDAT). Id:", uId, hheap);


   /*--- subclass the frame to make the child behave ---*/
   if (pglobals->hwndAppFrame != HWND_DESKTOP)
      if ((ItiWndSetMDI (hwndFrame)) == FALSE)
         return GoAway ("Unable to subclass frame window. Id:", uId, hheap);

//   /*--- fix sys menu for NormalSize ---*/
//   ItiWndFixSysMenu (hwndFrame);

   /*--- create child (list and ctl) windows ---*/
   for (i = 0; i < pswd->uChildren; i++)
      {
      chd = pswd->pchdChildren[i];
      if (pswd->pchdChildren[i].ulKind == ITIWC_LIST)
         ItiWndBuildListWindow (hwndClient, chd.uId, chd.rcl,chd.uCoordType);
      else
         ItiWndBuildOtherWindow (hwndClient,chd.uId,chd.ulKind,chd.rcl,chd.uCoordType);
      }

   rcl = pswd->rclPos;
   ItiWndPrepRcl (pglobals->hwndAppClient, pswd->uCoordType, &rcl);

   /*--- size static window to what its metadata says it should be ---*/
   WinSetWindowPos (hwndFrame, HWND_TOP, 
                    (SHORT)rcl.xLeft, (SHORT)rcl.yBottom,
                    (SHORT)rcl.xRight - (SHORT)rcl.xLeft,
                    (SHORT)rcl.yTop   - (SHORT)rcl.yBottom,
                    SWP_MOVE | SWP_SIZE | SWP_SHOW | SWP_ACTIVATE);

   WinSetWindowText (hwndFrame, szTitle);

   WinSendMsg (hwndClient, WM_INITQUERY, MPFROMP (hbufParent),
                    MPFROM2SHORT (uRowParent, FALSE));

   return hwndClient;
   }




/*
 * This guy creates all those cute little list windows.
 * It doesn't error check worth a nickel though.
//   but it's getting better at it!
 */
HWND EXPORT ItiWndBuildListWindow (HWND   hwnd,
                                   USHORT uId,
                                   RECTL  rcl,
                                   USHORT uCoordType)
   {
   HWND       hwndList;

//clf WS_CLIPCHILDREN added 2/12

   ItiWndPrepRcl (hwnd, uCoordType, &rcl);
   hwndList = WinCreateWindow (hwnd, ITIWND_LISTCLASS, "", WS_VISIBLE|WS_CLIPCHILDREN,
                               (USHORT) rcl.xLeft,  (USHORT) rcl.yBottom,
                               (USHORT) rcl.xRight - (USHORT) rcl.xLeft,
                               (USHORT) rcl.yTop   - (USHORT) rcl.yBottom,
                               hwnd, HWND_BOTTOM, uId, NULL, NULL);
   if (hwndList == NULL)
      {
      ItiErrDisplayWindowError (WinQueryAnchorBlock(hwnd), NULL, 0);
      return NULL;
      }

   return ItiWndKickListWindow (hwndList, uId);
   }




HWND EXPORT ItiWndKickListWindow (HWND   hwndList,
                                  USHORT uId)
   {
   PFNTXT     pfnTxt;
   LWMET      lwmet;
   PSZ        *ppszLabels, *ppszTmp;
   PEDT       pedtData, pedtLabel;
   LWINIT     lwInit;
   HHEAP      hheap;
   HMODULE    hmod;

   if (hwndList == NULL)
      return WndErr ("ListWindow is null. Did you forget to register it? Id:", uId);

   if ((pfnTxt = ItiMbListEditProc (uId, &hmod)) == NULL)
//      return WndErr ("Unable to get List win txt proc. Id:", uId);
      ;

   if (ItiMbQueryListMetrics (hmod, ITIRID_WND, uId, &lwmet))
      return WndErr ("Unable to obtain list metrics. Id:", uId);

   if ((hheap = ((PLWDAT)LWGetPLWDAT (hwndList, FALSE))->hheap) == NULL)
      return WndErr ("Things are looking really bad. Id:", uId);

   lwInit.uDataRows    = 0;                         /* defined later */
   if (ItiMbGetDataInfo (hmod, ITIRID_WND, uId, hheap, &pedtData, &ppszTmp, &lwInit.uDataCols))
//      return WndErr ("Unable To Get LW Data Info Id:", uId);
      ;

   /*--- list windows don't care about the data formats ---*/
   ItiFreeStrArray (hheap, ppszTmp, lwInit.uDataCols);

   if (ItiMbGetLabelInfo (hmod, ITIRID_WND, uId, hheap, &pedtLabel, &ppszLabels, &lwInit.uLabelCols))
//      return WndErr ("Unable To Get LW Label Info. ID:", uId);
      ;

   lwInit.uDataYSize   = lwmet.uDataYSize;
   lwInit.uLabelYSize  = lwmet.uLabelYSize;
   lwInit.uOptions    =  lwmet.uOptions;
   lwInit.uXScrollInc  = 0;                         /* use default */

   if (LWInit (hwndList,     // handle to window to initialize
               hmod,         // module containing metadata for lw
               &lwInit,      // lwinit struct
               pedtData,     // ptr to array to data element descriptors
               pedtLabel,    // ptr to array to label element descriptors
               ppszLabels,   // ptr to array of label strings
               pfnTxt))      // ptr to get text proc
      return WndErr ("Unable To init List Window Id:", uId);


   /*--- free data because it is copied ---*/
   ItiMemFree (hheap, pedtLabel);
   ItiMemFree (hheap, pedtData);
   ItiFreeStrArray (hheap, ppszLabels, lwInit.uLabelCols);

   SWL (hwndList, ITIWND_COLOR, LWC_CLIENT, CLR_PALEGRAY); 
// SWL (hwndList, ITIWND_COLOR, LWC_CLIENT, lwmet.lBColor); 
   SWL (hwndList, ITIWND_COLOR, LWC_LABEL,  lwmet.lLBColor);
   WinInvalidateRect (hwndList, NULL, TRUE);
   SW (hwndList, ITIWND_BUTTON, 0, lwmet.uButtonId, 0); 
   return hwndList;
   }






ULONG EXPORT ItiWndBuildDialogWindow (HWND hwndOwner, USHORT uWinId)
   {
   PFNWP    pfnDlgProc;
   USHORT   uResId, uCtls, i, j, uBuffers, uQuery;
   ULONG    uRet;
   HMODULE  hmod;
   PDWDAT   pdwdat;
   PSZ      *ppszTmp;
   HHEAP    hheap;
   PDGI     pdgi;
   BOOL     bLoaded;
   PBUFDAT  pbufdat;
   HBUF     hbufParent;
   BOOL     bChangeMode;

WndSetPtr (SPTR_WAIT);

   pfnDlgProc = ItiMbDialogProc (uWinId, &uResId, &hmod);
   if (pfnDlgProc == NULL)
      {
      WndErr ("Unable to obtain pointer to dlg proc. uId=",uWinId);
   WndSetPtr (SPTR_ARROW);
      return DID_ERROR;
      }

   hheap = ItiMemCreateHeap (4096);
   pdwdat = (PDWDAT) ItiMemAlloc (hheap, sizeof (DWDAT), 0);

   ItiMbGetDialogInfo  (hmod, ITIRID_WND, uResId, hheap,
                        &pdgi, &ppszTmp, &uCtls);
   /*--- dialog windows don't care about the data formats ---*/
   ItiFreeStrArray (hheap, ppszTmp, uCtls);

   uBuffers = 0;
   pbufdat = NULL;
   hbufParent = (HBUF) QW (hwndOwner, ITIWND_BUFFER, 0, 0, 0);

   bChangeMode = (ItiWndGetDlgMode () == ITI_CHANGE);

   for (i = 0; i < uCtls; i++)
      {
      /*--- Check and see if buffdat is already created  ---*/
      /*--- if its a list window, alwayscreate a new one ---*/
      /*--- (listwindows have a uQuery==0)               ---*/
      bLoaded = FALSE;

      uQuery = (bChangeMode ? pdgi[i].uChangeQuery : pdgi[i].uAddQuery);

      if (uQuery == ITIWND_ENUM)
         {
         pdgi[i].uOffset = 0xFFFF;
         continue;
         }

      for (j = 0; j < uBuffers; j++)
         if (uQuery && uQuery == pbufdat[j].uBuffId)
            {
            bLoaded = TRUE;
            pdgi[i].uOffset = j;
            break;
            }

      if (!bLoaded)
         {
         uBuffers++;
         pbufdat = ItiMemRealloc (hheap, pbufdat, sizeof(BUFDAT)*uBuffers, 0);
         pbufdat[uBuffers-1].hbuf = NULL;
         pbufdat[uBuffers-1].hbufParent = hbufParent;
         pbufdat[uBuffers-1].uBuffId = uQuery;
         pdgi[i].uOffset = uBuffers-1;
         }
      }
   pdwdat->uBuffers = uBuffers;
   pdwdat->pbufdat  = pbufdat;
   pdwdat->pdgi     = pdgi;
   pdwdat->uCtls    = uCtls;
   pdwdat->hheap    = hheap;
   pdwdat->uResId   = uResId;
   pdwdat->hwndOwner = hwndOwner;
   pdwdat->hmod      = hmod;
   pdwdat->pExtra   = NULL;

   pdwdat->uDlgMode = ItiWndGetDlgMode ();
   pdwdat->uDlgId          = uWinId;
   pdwdat->uDoneQueryCount = 0;
   pdwdat->ppszLockRow = NULL;

   if (uWinId == ShowUsersD)
      {

      uRet = LONGFROMMP ( MPFROMHWND (WinLoadDlg (HWND_DESKTOP, hwndOwner, pfnDlgProc,
                          hmod, uWinId, (PVOID) pdwdat)));
      ItiWndSetDlgMode (ITI_OTHER);
   WndSetPtr (SPTR_ARROW);
      return uRet;
      }
   else
      uRet = (ULONG) WinDlgBox (HWND_DESKTOP, hwndOwner, pfnDlgProc,
                                hmod, uWinId, (PVOID) pdwdat);

   ItiMemDestroyHeap (hheap);
   ItiWndSetDlgMode (ITI_OTHER);
 WndSetPtr (SPTR_ARROW);
   return uRet;
   }








PFNWP pfnCombo = NULL;


WNDPROC EXPORT PaintComboProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   HPS      hps;
   RECTL    rcl;

   switch (umsg)
      {
//      case WM_PAINT:
//         {
////       hps = WinBeginPaint (hwnd, (HPS)NULL, &rectl));
//         hps = WinGetPS (hwnd);
//
//         WinQueryWindowRect (hwnd, &rcl);
//         WinFillRect (hps, &rcl, CLR_WHITE);
//
////       WinEndPaint (hps);
//         WinReleasePS (hps);
//         }

//      case WM_FOCUSCHANGE:
//      case EN_SELECT:
      case LN_SELECT:
      case LN_KILLFOCUS:
         {
         hps = WinGetPS (hwnd);
         WinQueryWindowRect (hwnd, &rcl);
         WinFillRect (hps, &rcl, CLR_WHITE);
         WinReleasePS (hps);
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;
         }


      }
   return pfnCombo (hwnd, umsg, mp1, mp2);
   }





/*
 * This procedure creates control windows
 * such as buttons ...
 */

HWND EXPORT ItiWndBuildOtherWindow (HWND    hwnd,
                                    USHORT  uId,
                                    ULONG   ulKind,
                                    RECTL   rcl,
                                    USHORT  uCoordType)
   {
   HWND  hwndRet;

//   ItiWndPrepRcl (hwnd, uCoordType, &rcl);
//   hwndRet WinCreateWindow (hwnd, (PSZ)ulKind, "", WS_VISIBLE,
//                           (USHORT) rcl.xLeft,  (USHORT) rcl.yBottom,
//                           (USHORT) rcl.xRight, (USHORT) rcl.yTop,
//                           hwnd, HWND_TOP, uId, NULL, NULL);
   hwndRet = WinCreateWindow (hwnd, (PSZ)ulKind, "",
                              WS_VISIBLE | CBS_DROPDOWNLIST | WS_SAVEBITS | WS_SYNCPAINT,
                              0,  0, 10, 10,
                              hwnd, HWND_TOP, uId, NULL, NULL);
   if (!hwndRet)
      ItiErrDisplayWindowError (WinQueryAnchorBlock(pglobals->hwndAppClient), NULL, 0);

   pfnCombo = (PFNWP) WinQueryWindowPtr (hwndRet, QWP_PFNWP);
   WinSetWindowPtr (hwndRet, QWP_PFNWP, (PVOID)PaintComboProc);

   return hwndRet;
   }



USHORT ItiWndFadeWindow (HWND hwnd, ULONG lTopColor, ULONG lBottomColor)
   {
   HPS    hps;
   RECTL  rclUpdate;
//   RECTL  rclWnd;
//   POINTL ptlTL, ptlBR;
//   AREABUNDLE abnd;
//   ULONG      l;

   /*--- Try a bkg fade here ---*/
   hps = WinBeginPaint (hwnd, NULL, &rclUpdate); /* start painting  */

//   WinQueryWindowRect (hwnd, &rclWnd);
//   GpiCreateLogColorTable(hps, 0L, LCOLF_RGB, 0L, 0L, NULL);
//
//   ptlTL.x  = rclUpdate.xLeft;
//   ptlBR.x  = rclUpdate.xRight;
//   for (l=0; l<64; l++)
//      {
//      ptlTL.y  = ((rclWnd.yTop - rclWnd.yBottom) * (l+1L)) / 64L;
//      if (ptlTL.y < rclUpdate.yBottom) continue;
//      ptlBR.y  = ((rclWnd.yTop - rclWnd.yBottom) * l) / 64L;
//      if (ptlBR.y > rclUpdate.yTop) continue;
//
//// this will change
////
////      abnd.lColor = l << 2;
//
//      abnd.lColor =  0x00000080l;
//
//      GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0L, (PBUNDLE)&abnd);
//      GpiMove (hps, &ptlTL);
//      GpiBox (hps, DRO_FILL, &ptlBR, 0L, 0L);
//      }


   WinFillRect (hps, &rclUpdate, SYSCLR_APPWORKSPACE);     /* fill background */

   WinEndPaint (hps);                      /* end painting  */
   return 0;
   }







/*
 * This procedure registers the classes for
 * the static and list windows. It returns
 * 0 if all is ok
 *
 */

USHORT EXPORT ItiWndRegister (HAB hab)
   {
   if (WinRegisterClass (hab, ITIWND_STATICCLASS, WinDefWindowProc, CS_SIZEREDRAW, 
                         sizeof (PSWDAT)) == FALSE)
      return 1;

//   ItiMbRegisterStaticWnd (ITI_DEFAULTWND, ItiWndFixStaticWndProc, 0);
     ItiMbRegisterStaticWnd (ITI_DEFAULTWND, ItiWndDefStaticWndProc, 0);

   if (LWRegister (hab))
      return 1;

   return 0;
   }

