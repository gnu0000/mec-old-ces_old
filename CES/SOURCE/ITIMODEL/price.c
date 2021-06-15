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
 * price.c
 * Mark Hampton
 *
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itimodel.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itirptut.h"
#include <stdio.h>
#include <process.h>
#include <share.h>

#include "init.h"
#include "dialog.h"
#include "price.h"
#include "profile.h"
#include "local.h"



MRESULT EXPORT PriceDlgProc (HWND    hwnd,
                             USHORT  usMessage,
                             MPARAM  mp1,
                             MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               char  szFileName [257];
               FILE  *pf;

               ItiRptUtUniqueFile (szFileName, sizeof szFileName, TRUE);
               pf = _fsopen (szFileName, "wt", SH_DENYWR);
               if (pf == NULL)
                  {
                  DosBeep (1000, 100);
                  DosBeep (500, 100);
                  break;
                  }

               fputc ('\n', pf);
               fputc ('\n', pf);
               
               BuildWhereClause (hwnd, pf);
               fputc ('\n', pf);
               fclose (pf);

               spawnl (P_NOWAIT, "itirpt.exe", "itirpt.exe", "dprice", 
                       "-modelinfo", szFileName, NULL);
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }

