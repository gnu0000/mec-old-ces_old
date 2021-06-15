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
 * wintext.c                                                            *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
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
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




USHORT EXPORT WinTextBoxRegister (HAB hab)
   {
   if (WinRegisterClass (hab, ITIWND_TEXTBOXCLASS, WndTextBoxProc,
                         CS_SIZEREDRAW, 2 * sizeof (PVOID));
      return 1;
   return 0;
   }

WinSetWindowText


WNDPROC EXPORT WndTextBoxProc (HWND   hwnd,
                               USHORT umsg,
                               MPARAM mp1,
                               MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_CREATE:
         {
         HHEAP hheap;

         hheap = ItiMemCreateHeap (255);
         WinSetWindowPtr (hwnd, 0, hheap);
         WinSetWindowPtr (hwnd, 1, NULL);
         }

      case WM_SETWINDOWPARAMS:
         {
         PWNDPARAMS pwprm;

         pwprm = (PWNDPARAMS) PVOIDFROMMP (mp1);

         if (!(pwprm->fsStatus & WPM_TEXT))




         }

      case WM_PAINT:
         {
         }

      case WM_CLOSE:
         {
         }
      }
   }
