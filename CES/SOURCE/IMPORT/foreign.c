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
 * foreign.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module fills in foreign keys of loaded tables.
 */

#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiimp.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include "import.h"
// #include "util.h"
#include "load.h"
#include "foreign.h"
#include <limits.h>



USHORT EXPORT FillInFgnKeysFunc (HHEAP       hheap, 
                          PIMPORTINFO piiFIK, 
                          PCOLINFO    pci);



BOOL EXPORT FillInForeignKeys (HHEAP hheap, PIMPORTINFO apiiFK [], USHORT usNumTables)
   {
   USHORT i, j;

   for (i=0; i < usNumTables; i++)
      {
      if (apiiFK [i]->usError == 0)
         {
         /* process each column that is a foreign key */
         for (j=0; j < apiiFK [i]->usFileColumns && apiiFK [i]->usError == 0; j++)
            {
            if (apiiFK [i]->pci [j].kForeignKey)
               apiiFK [i]->usError = FillInFgnKeysFunc (hheap, apiiFK [i], 
                                               apiiFK [i]->pci + j);
            }
         }
      }

   return TRUE;
   }





USHORT EXPORT FillInFgnKeysFunc (HHEAP       hheap, 
                          PIMPORTINFO piiFIK, 
                          PCOLINFO    pci)
   {
   CHAR   szTemp [250] = "";
//BOOL   bFound;

   if (pci->pfnFillInForeignKeys == NULL)
      return ERROR_MUST_HAVE_DLL;

   sprintf (szTemp, "Filling in foreign keys for %s.%s.", 
            piiFIK->pti->pszTableName, pci->pszImportName);
   puts (szTemp);
   ItiErrWriteDebugMessage (szTemp);

//if ( 0 == ItiStrNICmp(piiFIK->pti->pszTableName, "Historical", 9) )
//   bFound = TRUE;

   pci->pfnFillInForeignKeys (hheap, piiFIK, pci, pszProdDatabase);


   return piiFIK->usError;
   }

