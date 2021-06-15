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
 * areaimp.c
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "areaimp.h"
#include "initcat.h"






/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/* Stuff for import                                                       */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


USHORT EXPORT ItiImpCopyArea (HHEAP       hheap, 
                              PIMPORTINFO pii, 
                              PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   /* copy area to the production database */
   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYAREA,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpCopyAreaType (HHEAP       hheap, 
                                  PIMPORTINFO pii, 
                                  PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYAREATYPE,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpCopyAreaCounty (HHEAP       hheap, 
                                    PIMPORTINFO pii, 
                                    PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [3], apszValues [3];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "KeyValue";
   apszValues [2] = UNDECLARED_BASE_DATE;

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYAREACOUNTY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpAreaTypeKey (HHEAP       hheap, 
                                 PIMPORTINFO pii, 
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [4], apszValues [4];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = ItiImpQueryImportDatabase ();
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";
   apszValues [3] = "AreaTypeKey";

   if (ItiStrCmp (pii->pti->pszTableName, "Area") == 0)
      {
      ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, INSERTSTAR,
                           szTemp, sizeof szTemp);
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      if (pii->usError)
         return pii->usError;
      
      }

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, UPDATEAREATYPEKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   apszValues [0] = pszProdDatabase;
   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, UPDATEAREATYPEKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }





USHORT EXPORT ItiImpAreaKey (HHEAP       hheap, 
                             PIMPORTINFO pii, 
                             PCOLINFO    pci,
                             PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [4], apszValues [4];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = ItiImpQueryImportDatabase ();
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";
   apszValues [3] = "AreaKey";

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   apszValues [0] = pszProdDatabase;
   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpCountyKey (HHEAP       hheap, 
                               PIMPORTINFO pii, 
                               PCOLINFO    pci,
                               PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
//   PSZ   apszTokens [6], apszValues [6];
//
//   apszTokens [0] = "ProductionDatabase";
//   apszValues [0] = pszProdDatabase;
//   apszTokens [1] = "TableName";
//   apszValues [1] = pii->pti->pszTableName;
//   apszTokens [2] = "ColumnName";
//   apszValues [2] = pci->pszColumnName;
//   apszTokens [3] = "KeyValue";
//   apszValues [3] = NULL;
//   apszTokens [4] = "TargetColumn";
//   apszValues [4] = "AreaKey";
//   apszTokens [5] = "AreaTypeKey";
//
//   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, UPDATECOUNTYKEY,
//                        szTemp, sizeof szTemp);
//   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
//                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
//   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
//   return pii->usError;
   return ItiImpFillInFK (hheap, hmodMe, pii, pci, pszProdDatabase, 
                          "CountyKey",             /* target column */
                          NULL,                    /* code table name */
                          "County",                /* prod table name */
                          "CountyKey",             /* prod key name */
                          "CountyID",              /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }



