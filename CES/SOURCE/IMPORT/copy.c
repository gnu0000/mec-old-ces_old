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
 * Copy.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This module is part of DS/Shell.
 *
 * This module copies tables from the import database to the production
 * database.
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
#include "copy.h"





BOOL CopyDatabase (HHEAP         hheap, 
                   PIMPORTINFO   apii [], 
                   USHORT        usNumTables)
   {
   USHORT i;
   BOOL   bHadErrors = FALSE;
   CHAR   szTemp [256] = "";

   /* first, check to see if any errors occurred */
   for (i=0; i < usNumTables; i++)
      if (apii [i]->usError != 0)
         {
         //printf ("The imported data will not be copied to the production\n"
         //        "database because of previous errors.\n");
         sprintf (szTemp, "\n *** Data error found in table %s.",
                  apii [i]->pti->pszTableName);
         puts (szTemp);
         ItiErrWriteDebugMessage (szTemp);
         bHadErrors = TRUE;
         }


   if (bHadErrors)
      return FALSE;

   for (i=0; i < usNumTables; i++)
      {
      if (apii [i]->pti->pfnCopy == NULL)
         apii [i]->usError = ERROR_MUST_HAVE_DLL; 
      else
         {
         sprintf (szTemp,
                  "\n Copying table %s to production database %s ",
                  apii [i]->pti->pszTableName, pszProdDatabase);
         if (0 != ItiStrNCmp(apii [i]->pti->pszTableName,"Historical",10) )
            {
            puts (szTemp);
            ItiErrWriteDebugMessage (szTemp);
            }
         else if (0 == ItiStrCmp(apii [i]->pti->pszTableName,"HistoricalProposal") )
            {
            sprintf (szTemp,
               "\n Copying imported historical data to production database %s ",
               pszProdDatabase);
            puts (szTemp);
            ItiErrWriteDebugMessage (szTemp);
            }

         apii [i]->usError = apii [i]->pti->pfnCopy (hheap, apii [i], 
                                                     pszProdDatabase);
         }
      }
      
         /* -- The following tables are EXEMPT from returning errors */
         /* -- at this point due to the new import modifications.    */
       //  if ( 0 != ItiStrNICmp(apii[i]->pti->pszTableName, "Historical", 9) )
       //     bHadErrors = TRUE;

   return !bHadErrors;
   }


