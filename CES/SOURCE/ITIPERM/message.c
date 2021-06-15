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
 * Mark Hampton
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include <stdio.h>
#include "init.h"
#include "job.h"
#include "message.h"



MRESULT EXPORT ShowMessageSProc (HWND   hwnd,
                                 USHORT usMessage, 
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (mp2, IDM_DELETE, FALSE);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ShowMessageL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }


void ShowMessages (void)
   {
   char szQuery [512];
   USHORT i, usNumCols, usError;

   sprintf (szQuery, 
            "SELECT UM.ToUserKey "
            "FROM UserMessage UM "
            "WHERE UM.ToUserKey = %s ",
            pglobals->pszUserKey);

   i = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);

   if (i)
      ItiWndBuildWindow (NULL, ShowMessageS);

   }
