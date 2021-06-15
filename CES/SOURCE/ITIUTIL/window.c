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
 * Window.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include <stdio.h>
#include <stdarg.h>


int _far _cdecl _loadds ItiStrPrintfDlgItem (HWND hwndDlg, USHORT usID, ...)
   {
   va_list  va;
   int      i;
   PSZ      pszOld, pszNew;
   USHORT   usOldSize, usNewSize;

   usOldSize = WinQueryDlgItemTextLength (hwndDlg, usID) + 1;

   pszOld = ItiMemAllocSeg (usOldSize, 0, 0);
   if (!pszOld)
      return -1;
   WinQueryDlgItemText (hwndDlg, usID, usOldSize, pszOld);

   usNewSize = usOldSize < 1024 ? 10240 : usOldSize * 10;
   pszNew = ItiMemAllocSeg (usNewSize, 0, 0);
   if (!pszNew)
      {
      ItiMemFreeSeg (pszOld);
      return -2;
      }

   va_start (va, usID);
   i = vsprintf (pszNew, pszOld, va);
   WinSetDlgItemText (hwndDlg, usID, pszNew);

   ItiMemFreeSeg (pszOld);
   ItiMemFreeSeg (pszNew);
   return i;
   }

