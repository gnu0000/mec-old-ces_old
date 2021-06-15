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
 * Error.c
 *
 * (C) 1992 Info Tech Inc.
 * This file displays installation errors.
 */


#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "install.h"
#include "error.h"


int InstError (PGNUWIN pgw, BOOL bExit, PSZ pszLine1, PSZ pszLine2, PSZ pszLine3)
   {
   int c;

   ClearWin (pgw);
   PaintText (pgw,  2, 30, 2, 1, "ERROR");
   PaintText (pgw,  3, 30, 2, 1, "-----");
   if (pszLine1)
      PaintText (pgw,  5,  2, 0, 0, pszLine1);
   if (pszLine2)
      PaintText (pgw,  6,  2, 0, 0, pszLine2);
   if (pszLine3)
      PaintText (pgw,  7,  2, 0, 0, pszLine3);

   if (bExit)
      PaintText (pgw,  9,  2, 0, 1, "Press <Enter> to continue, or Escape to exit.");
   else
      PaintText (pgw,  9,  2, 0, 1, "Press <Enter> to continue.");
    
   c = GetKey ("\x1B\x0D");
   if (bExit && c == '\x1b')
      Quit (1);
   return c;
   }


