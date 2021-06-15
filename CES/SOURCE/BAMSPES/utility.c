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
 * Utility.c
 * Mark Hampton
 *
 * This module provides the Utilities Find and Utilities Sort commands.
 */

#include "..\include\iti.h"
#include "BAMSPES.h"
#include "..\include\winid.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "utility.h"
#include "dialog.h"


MRESULT EXPORT SortDialogProc (HWND   hwnd,
                               USHORT usCommand,
                               MPARAM mp1,
                               MPARAM mp2)
   {
   return WinDefDlgProc (hwnd, usCommand, mp1, mp2);
   }


USHORT DoSortCommand (HWND hwnd)
   {
   return WinDlgBox (pglobals->hwndDesktop, hwnd, SortDialogProc, 0, 
                     IDD_SORT, NULL);
   }




MRESULT EXPORT FindDialogProc (HWND   hwnd,
                               USHORT usCommand,
                               MPARAM mp1,
                               MPARAM mp2)
   {
   return WinDefDlgProc (hwnd, usCommand, mp1, mp2);
   }



USHORT DoFindCommand (HWND hwnd)
   {
   return WinDlgBox (pglobals->hwndDesktop, hwnd, FindDialogProc, 0, 
                     IDD_FIND, NULL);
   }



