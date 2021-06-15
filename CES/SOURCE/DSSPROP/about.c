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
 * Copyright (C) 1991 AASHTO.
 */

#define INCL_WIN
#include <stdio.h>
#include "..\include\iti.h"
#include "dssprop.h"
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



