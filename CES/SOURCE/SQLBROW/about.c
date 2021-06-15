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
 * About.c
 * Mark Hampton
 * Copyright (C) 1991 AASHTO.
 *
 * This module contains the window procedure for the about box for
 * BAMS/CES
 */

#define INCL_WIN
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\dialog.h"
#include "sqlbrow.h"
#include "about.h"
#include "dialog.h"

MRESULT EXPENTRY AboutDlgProc (HWND    hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         char szBuffer [50];
         SWP   swpMe;
         SWP   swpParent;

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (WinQueryWindow (hwnd, QW_PARENT, 0),
                            &swpParent);

         swpMe.x = swpParent.x + swpParent.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpParent.y + swpParent.cy / 2 - swpMe.cy / 2;

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);

         sprintf (szBuffer, "Release %s", VERSION_STRING);
         WinSetDlgItemText (hwnd, DID_VERSION, szBuffer);
         }
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               WinDismissDlg (hwnd, TRUE);
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



