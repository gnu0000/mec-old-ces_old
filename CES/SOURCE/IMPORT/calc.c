
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
 * calc.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module calculates loaded tables.
 */

#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiimp.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include <limits.h>
#include "import.h"
// #include "util.h"
#include "calc.h"


BOOL CalcTables (HHEAP        hheap, 
                 PIMPORTINFO  apii [], 
                 USHORT       usNumTables)
   {
   USHORT i;
   char   szTemp [250];

   for (i=0; i < usNumTables; i++)
      {
      if (apii [i]->usError == 0 && apii [i]->pti->pfnCalc != NULL)
         {
         sprintf (szTemp, "Performing calculations on table %s.",
                  apii [i]->pti->pszTableName, pszProdDatabase);
         puts (szTemp);
         ItiErrWriteDebugMessage (szTemp);
         apii [i]->usError = apii [i]->pti->pfnCalc (hheap, apii [i], 
                                                     pszProdDatabase);
         }
      }

   return TRUE;
   }


