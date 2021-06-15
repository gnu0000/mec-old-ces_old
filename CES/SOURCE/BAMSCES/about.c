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
 * About.c
 * Mark Hampton
 *
 * This module contains the window procedure for the about box for
 * BAMS/CES
 */

#define INCL_WIN
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "bamsces.h"
#include "about.h"
#include "dialog.h"
#include "..\include\dialog.h"

MRESULT EXPENTRY AboutDlgProc (HWND    hwnd,
                               USHORT   usMessage,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
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

#ifdef PRERELEASE
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, PRERELEASE);
#else
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, "");
#endif
         ItiStrPrintfDlgItem (hwnd, DID_COPY1, COPYRIGHT);
         ItiStrPrintfDlgItem (hwnd, DID_COPY2, COPYRIGHT);
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



