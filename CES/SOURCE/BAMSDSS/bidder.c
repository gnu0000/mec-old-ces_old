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
 * Bidder.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Bidder window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "bamsdss.h"
#include "bidder.h"
#include "menu.h"
#include "dialog.h"

#include <stdio.h>

/*
 * Window procedure for Bidder window.
 */


MRESULT EXPORT BidderSProc (HWND   hwnd,
                           USHORT usMessage,
                           MPARAM mp1,
                           MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_VIEW_MENU:
//               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Vendor ~Facility List", 
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Vendor", 
                                      IDM_VIEW_USER_FIRST + 2);

               return 0;
            }

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               break;

         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, FacilityForVendorS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, VendorS);
               return 0;
            }
         break;
     }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }
