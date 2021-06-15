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
 * lwutil.c                                                             *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file contains utility procedures for the other modules          *
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
#include "..\include\itidbase.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include "winmain.h"
#include "lwmain.h"
#include "lwutil.h"
#include "lwsplit.h"
#include <stdio.h>




static SHORT okUtlErr (PSZ psz, USHORT uId)
   {
   char  szTmp [512];

   sprintf (szTmp, "ITIWIN: lwutil.c : %s%d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "List Util Error", 0, MB_OK | MB_APPLMODAL);
   return 1;
   }



USHORT Init (HWND hwndFrame)
   {
   return (LWGetPLWDAT (hwndFrame, FALSE))->Init;
   }



/* valid uCmd values to calculate 
   LWM_YRowSize
   LWM_XRowSize
   LWM_YWindowSize
   LWM_XWindowSize
   LWM_YWindowPos
   LWM_XWindowPos
   LWM_YScrollInc
   LWM_XScrollInc
   LWM_NumRows
   LWM_NumCols
   LWM_NumLabelCols
   LWM_YLabelSize
   LWM_Options
   LWM_FullScreenRows
*/

USHORT LWQuery (HWND hwnd, USHORT uCmd, USHORT bChild)
   {
   PLWDAT  plwd;
   SWP swp;

   if ((plwd = LWGetPLWDAT (hwnd, bChild)) == NULL)
//      return okUtlErr ("Unable to obtain plwd in LWQuery", 0);
   return 0;

   switch (uCmd)
      {
      case LWM_YRowSize:
         return (USHORT)(plwd->rclData.yTop - plwd->rclData.yBottom);
      case LWM_XRowSize:
         return (USHORT)(plwd->rclData.xRight - plwd->rclData.xLeft);
      case LWM_YWindowSize:
         WinQueryWindowPos (hwnd, &swp);
         return swp.cy;
      case LWM_XWindowSize:
         WinQueryWindowPos (hwnd, &swp);
         return swp.cx;
      case LWM_YWindowPos:
         WinQueryWindowPos (hwnd, &swp);
         return swp.y;
      case LWM_XWindowPos:
         WinQueryWindowPos (hwnd, &swp);
         return swp.x;
      case LWM_YScrollInc:
         return (USHORT)(plwd->rclData.yTop - plwd->rclData.yBottom);
      case LWM_XScrollInc:
         return plwd->uXScrollInc;
      case LWM_NumRows:
         return plwd->uDataRows;
      case LWM_NumCols:
         return plwd->uDataCols;
      case LWM_NumLabelCols:
         return plwd->uLabelCols;
      case LWM_YLabelSize:
         return plwd->uYLabel;
      case LWM_Options:
         return plwd->uOptions;
      case LWM_Active:
         return plwd->uActive;
      case LWM_FullScreenRows:
         return LWQuery (hwnd, LWM_YWindowSize, bChild) /
                LWQuery (hwnd, LWM_YRowSize, bChild);

      default:
         return okUtlErr ("Invalid uCmd LWQuery", 0);
      }
   }





PRECTL LWElementRect (HWND hwndChild)
   {
   return &((LWGetPLWDAT (hwndChild, TRUE))->rclData);
   }



PEDT LWGetPEDT (HWND hwnd, USHORT bChild, USHORT bClient)
   {
   if (bClient)
      return (LWGetPLWDAT (hwnd, bChild))->pedtData;
   else
      return (LWGetPLWDAT (hwnd, bChild))->pedtLabel;
   }


PSZ *LWGetPPSZ (HWND hwndChild)
   {
   return (LWGetPLWDAT (hwndChild, TRUE))->ppszLabels;
   }



PLWDAT LWGetPLWDAT (HWND hwnd, USHORT bChild)
   {
   if (bChild)
      return (PLWDAT) WinQueryWindowPtr (WinQueryWindow
                                (hwnd, QW_PARENT, FALSE), QWP_LWDAT);
   else
      return (PLWDAT) WinQueryWindowPtr (hwnd, QWP_LWDAT);
   }


HHEAP LWGetHHEAP  (HWND hwnd, USHORT bChild)
   {
   return (LWGetPLWDAT (hwnd, bChild))->hheap;
   }


HBUF LWGetHBUF (HWND hwnd, USHORT bChild)
   {
   return (LWGetPLWDAT (hwnd, bChild))->hbuf;
   }


                           
USHORT *LWGetSel (HWND hwnd, USHORT bChild)
   {
   return (LWGetPLWDAT (hwnd, bChild))->puSel;
   }


LONG LWColor (HWND hwnd, USHORT uColorType, USHORT bChild)
   {
   PLWDAT     plwd;

   plwd = LWGetPLWDAT (hwnd, bChild);

   if (!plwd->Init)
      return CLR_PALEGRAY;
      
   switch (uColorType)
      {
      case LWC_CLIENT:
         return plwd->lDataColor;
      case LWC_LABEL: 
         return plwd->lLabelColor;
      case LWC_SELECT:
         return plwd->lSelectColor;
      case LWC_ACTIVE:
         return plwd->lActiveColor;
      case LWC_SPLITBORDER:
         return plwd->lSplitBColor;
      case LWC_SPLITMIN:
         return plwd->lSplitMin;
      case LWC_SPLITMAX:
         return plwd->lSplitMax;
      default:
         return CLR_PALEGRAY;
      }
   return CLR_PALEGRAY;
   }





USHORT LWSendActiveMsg (HWND hwnd, BOOL bChild)
   {
   HWND hwndFrame, hwndParent;
   PLWDAT   plwd;


   hwndFrame  = (bChild ? WinQueryWindow (hwnd, QW_PARENT, 0) : hwnd);
   plwd = LWGetPLWDAT (hwndFrame, FALSE);
   hwndParent = WinQueryWindow (hwndFrame, QW_PARENT, 0);

   WinSendMsg (hwndParent, WM_ACTIVEROWCHANGED,
               MPFROM2SHORT (plwd->uActive,
               WinQueryWindowUShort (hwndFrame, QWS_ID)),
               MPFROMP (hwndFrame));

   return 0;
   }



