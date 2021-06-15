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



#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "replace.h"

USHORT BuildKeyList (HWND    hwndList,
                     PSZ     pszDest,
                     USHORT  usBufferColumn,
                     USHORT  usDestMax)
   {
   USHORT   usRow;
   BOOL     bMultiSel;
   PSZ      pszData;
   HHEAP    hheap;

//   bMultiSel = !!(WinQueryWindowULong (hwndList, QWL_STYLE) & LWS_MULTISELECT);
   bMultiSel = TRUE;
   *pszDest = '\0';
   hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

   usRow = (UM) QW (hwndList, (bMultiSel ? ITIWND_SELECTION : ITIWND_ACTIVE), 0, 0, 0);
   while (usRow != -1)
      {
      pszData = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, usBufferColumn);

      ItiStrCat (pszDest, pszData, usDestMax);
      ItiMemFree (hheap, pszData);

      if (!bMultiSel)
         usRow = -1;
      else
         usRow = (UM) QW (hwndList, ITIWND_SELECTION, usRow + 1, 0, 0);
      if (usRow != -1)
         ItiStrCat (pszDest, ",", usDestMax);
      } 

   return ItiStrLen (pszDest);
   }

