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
 * Primary.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module fills in primary keys of loaded tables.
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
#include "primary.h"



BOOL EXPORT FillInPrmKeysFunc (HHEAP       hheap, 
                        PIMPORTINFO pii,
                        PCOLINFO    pci);


BOOL EXPORT FillInPrimaryKeys (HHEAP hheap, PIMPORTINFO apii [], USHORT usNumTables)
   {
   USHORT   i;
   PCOLINFO pciKey;

   for (i=0; i < usNumTables; i++)
      {
      if (apii [i]->usError == 0 && (pciKey = FindKeyColumn (apii [i])))
         {
         if (apii [i]->pti->pfnMerge != NULL)
            if (apii [i]->pti->pfnMerge (hheap, apii [i], pszProdDatabase))
               continue;
         if (!FillInPrmKeysFunc (hheap, apii [i], pciKey))
            printf ("Could not fill in keys for table %s. Table not processed.\n",
                    apii [i]->pti->pszTableName);
         }
      }
   return TRUE;
   }




BOOL EXPORT FillInPrmKeysFunc (HHEAP       hheap, 
                        PIMPORTINFO pii,
                        PCOLINFO    pciKey)
   {
   char   szTemp [256];
   PSZ    pszKeyBase;
   PSZ    pszTemp;
   ULONG  ulRows;

   sprintf (szTemp, "Filling in primary key for table %s.", 
            pii->pti->pszTableName);
   puts (szTemp);
   ItiErrWriteDebugMessage (szTemp);

   ItiStrCpy (szTemp, "SELECT COUNT (*) FROM ", sizeof szTemp);
   ItiStrCat (szTemp, pii->pti->pszTableName, sizeof szTemp);
   ItiStrCat (szTemp, " WHERE ", sizeof szTemp);
   ItiStrCat (szTemp, pciKey->pszColumnName, sizeof szTemp);
   ItiStrCat (szTemp, " < 1000000 ", sizeof szTemp);

   pszTemp = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (pszTemp == NULL || *pszTemp == '\0')
      return FALSE;

   if (!ItiStrToULONG (pszTemp, &ulRows))
      return FALSE;

   if (ulRows == 0)
      return TRUE;

   pszKeyBase = ItiDbGetKey (hheap, pii->pti->pszTableName, ulRows);

   if (pszKeyBase == NULL)
      {
      printf ("Could not get %lu keys for table %s. Table not processed.\n",
              pii->ulRows, pii->pti->pszTableName);
      pii->usError = ERROR_NO_KEYS;

      return FALSE;
      }

   ItiStrCpy (szTemp, "UPDATE ", sizeof szTemp);
   ItiStrCat (szTemp, pii->pti->pszTableName, sizeof szTemp);
   ItiStrCat (szTemp, " SET ", sizeof szTemp);
   ItiStrCat (szTemp, pciKey->pszColumnName, sizeof szTemp);
   ItiStrCat (szTemp, "=", sizeof szTemp);
   ItiStrCat (szTemp, pciKey->pszColumnName, sizeof szTemp);
   ItiStrCat (szTemp, "+", sizeof szTemp);
   ItiStrCat (szTemp, pszKeyBase, sizeof szTemp);
   ItiStrCat (szTemp, "-1 WHERE ", sizeof szTemp);
   ItiStrCat (szTemp, pciKey->pszColumnName, sizeof szTemp);
   ItiStrCat (szTemp, " < 1000000 ", sizeof szTemp);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return pii->usError = ERROR_CANT_FILL_IN_KEYS;

   return TRUE;
   }


// =========================================

#include <limits.h>





USHORT EXPORT FillInPFgnKeys (HHEAP       hheap, 
                             PIMPORTINFO piiFIK, 
                             PCOLINFO    pci)
   {
   CHAR   szTemp [250] = "";

   if (pci->pfnFillInForeignKeys == NULL)
      return ERROR_MUST_HAVE_DLL;

   sprintf (szTemp, "Filling in Foreign keys for %s.%s.", 
            piiFIK->pti->pszTableName, pci->pszImportName);
   puts (szTemp);
   ItiErrWriteDebugMessage (szTemp);

   pci->pfnFillInForeignKeys (hheap, piiFIK, pci, pszProdDatabase);

   return piiFIK->usError;
   }





BOOL EXPORT FillInForeignFrgnKeys (HHEAP hheap, PIMPORTINFO apiiFK [], USHORT usNumTables)
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
               apiiFK [i]->usError = FillInPFgnKeys (hheap, apiiFK [i], 
                                               apiiFK [i]->pci + j);
            } /* end inner for loop */
         } /* end if apiiFK [i]... */
      } /* end outer for loop */

   return TRUE;
   } /* End of Function FillInForeignFrgnKeys */



