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
 * process.c
 * Mark Hampton
 * Copyright (C) 1991 AASHTO
 *
 */


#define INCL_WIN
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "SQLBrow.h"
#include "init.h"
#include "dialog.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"


//HSEM hsemDone = NULL;
BOOL bCancel = FALSE;
HWND hwnd;

static void ExecQuery (PSZ pszQuery)
   {
   HHEAP    hheap;
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, ulNumRows, usState, i;

   hheap = ItiMemCreateHeap (4096);
   if (hheap == NULL)
      {
//      ItiSemSetEvent (hsemDone);
      return;
      }

   hqry = ItiDbExecQuery (pszQuery, hheap, 0, 0, 0, &i, &usState);
   if (hqry == NULL)
      {
//      ItiSemSetEvent (hsemDone);
      ItiMemDestroyHeap (hheap);
      return;
      }

   ItiDbTerminateQuery (hqry);

   hqry = ItiDbExecQuery (pszQuery, hheap, 0, ITIRID_RPT, i, &usNumCols, &usState);
   if (hqry == NULL)
      {
//      ItiSemSetEvent (hsemDone);
      ItiMemDestroyHeap (hheap);
      return;
      }

   ulNumRows = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usState) && !bCancel)
      {
      for (i=0; i < usNumCols; i++)
         {
         WinSendMsg (hwnd, MLM_INSERT, MPFROMP (ppsz [i]), 0);
         WinSendMsg (hwnd, MLM_INSERT, MPFROMP ("\t"), 0);
         }

      WinSendMsg (hwnd, MLM_INSERT, MPFROMP ("\n"), 0);
      ulNumRows++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   ItiMemDestroyHeap (hheap);
//   ItiSemSetEvent (hsemDone);
   }




void ProcessQuery (HWND hwndDest, PSZ pszQuery)
   {
//   if (hsemAccess == NULL)
//      {
//      if (ItiSemCreateMutex (NULL, 0, &hsemAccess))
//         return;
//      if (ItiSemCreateEvent (NULL, 0, &hsemDone))
//         return;
//      ItiSemSetEvent (hsemDone);
//      }
//
//   if (hsemAccess == NULL || hsemDone == NULL)
//      return
//
//   bCancel = TRUE;
//   ItiSemWaitEvent (hsemDone, SEM_INDEFINITE_WAIT);
//
//   bCancel = FALSE;

   hwnd = hwndDest;
//   _beginthread (ExecQuery, NULL, 32000, pszQuery);
   ExecQuery (pszQuery);
   }


