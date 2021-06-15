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
 * regimp.c
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
#include "regimp.h"
#include "initdll.h"



#define  NUM_OF_SI  1000
#define  SZLEN        14

static CHAR  szStar[10] = "-\\|/+   ";
static CHAR  aszResults [NUM_OF_SI + 2] [3] [SZLEN];



/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/* Stuff for import                                                       */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


USHORT EXPORT ItiImpCopyRegression (HHEAP       hheap, 
                                    PIMPORTINFO pii, 
                                    PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char  szQuery [2020] = "";
   char  szTemp [2020] = "";
   PSZ   apszTokens [3], apszValues [3];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "KeyValue";
   apszValues [2] = UNDECLARED_BASE_DATE;

   /* copy PEMETHRegression to the production database */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYREGRESSION,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   /* clean up the mapping table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, DELETEMAPPING,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   /* build the mapping table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, MAKEMAPPING,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   return pii->usError;
   }



USHORT EXPORT ItiImpCopyAreaAdj (HHEAP       hheap, 
                                 PIMPORTINFO pii, 
                                 PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char  szQuery [2020] = "";
   char  szTemp [2020] = "";
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYAREAADJ,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpCopySeasonAdj (HHEAP       hheap, 
                                   PIMPORTINFO pii, 
                                   PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char  szQuery [2020] = "";
   char  szTemp [2020] = "";
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYSEASONADJ,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }


USHORT EXPORT ItiImpCopyWorkTypeAdj (HHEAP       hheap, 
                                     PIMPORTINFO pii, 
                                     PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char  szQuery [2020] = "";
   char  szTemp [2020] = "";
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYWORKTYPEADJ,
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

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREATYPEKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   if (pii->usError)
      return pii->usError;
   apszValues [0] = pszProdDatabase;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREATYPEKEY,
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

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   if (pii->usError)
      return pii->usError;
   apszValues [0] = pszProdDatabase;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpWorkType (HHEAP       hheap, 
                             PIMPORTINFO pii, 
                             PCOLINFO    pci,
                             PSZ         pszProdDatabase)
   {
//   char     szQuery [1024];
//   char     szTemp [1024];
   char  szQuery [2020] = "";
   char  szTemp [2020] = "";
   PSZ      apszTokens [6], apszValues [6];
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone;

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "CodeTableName";
   apszValues [4] = "WRKTYP";
   apszTokens [5] = "TargetColumn";
   apszValues [5] = "WorkType";

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_GETDISTINCTFKS,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_UPDATECODEVALUE,
                        szTemp, sizeof szTemp);
   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = *ppsz;
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }





USHORT EXPORT ItiImpStandardItemKey (HHEAP       hheap, 
                                     PIMPORTINFO pii, 
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase)
   {
   HQRY     hqry;
   HHEAP    hhpLocSIK = NULL;
   PSZ      *ppsz;
   USHORT   usNumCols, usState, usCnt;
   PSZ      apszTokens [8+1], apszValues [8+1];
   CHAR     szQuery [300] = "";
   CHAR     szTemp  [300] = "";
   CHAR     szMark[3] = "";

   hhpLocSIK = ItiMemCreateHeap (MAX_HEAP_SIZE);

#define SI_KEY     0  
#define SI_ID      1  
#define SI_SPYR    2  

   apszTokens [0] = "ProductionDatabase"; apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";      apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";     apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";   apszValues [3] = "StandardItemKey";
   apszTokens [4] = "TargetSpecYear"; apszValues [4] = ""; //ItiDbGetDbValue (hhpLocSIK, ITIDB_MAX_SPEC_YEAR);
   apszTokens [5] = "TargetUnitType";
        //  apszValues [5] = (pii->  ) ? " 1 " : " 0 ";
            apszValues [5] = " 0 ";
   apszTokens [6] = "StdItemNumber";   apszValues [6] = "StandardItemNumber";
   apszTokens [7] = "StdItemKey";      apszValues [7] = "";
   apszTokens [8] = ""; apszValues [8] = "";

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTDITEMKEY_SUBQRY,
                         szTemp, sizeof szTemp);

   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                         apszTokens, apszValues,
                         sizeof apszTokens / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hhpLocSIK, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   aszResults[0][0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppsz, &usState)) 
      {
      if (usCnt <= NUM_OF_SI)
         {
         ItiStrCpy(aszResults[usCnt][SI_KEY ], ppsz[SI_KEY ], SZLEN);  // StandardItemKey
         ItiStrCpy(aszResults[usCnt][SI_ID  ], ppsz[SI_ID  ], SZLEN);  // StandardItemNumber
         ItiStrCpy(aszResults[usCnt][SI_SPYR], ppsz[SI_SPYR], SZLEN);  // StandardItem SpecYear
   
         aszResults[usCnt+1][0][0] = '\0';
         }
   
      ItiFreeStrArray (hhpLocSIK, ppsz, usNumCols);
      usCnt++;
      }

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTANDARDITEMKEY,
                        szTemp, sizeof szTemp);

   //while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
   //       pii->usError == 0)

   usCnt = 0;
   while (aszResults[usCnt][0][0] != '\0')
      {
      apszValues [7] = aszResults [usCnt] [SI_KEY ];  /* Key  */
      apszValues [6] = aszResults [usCnt] [SI_ID  ];  /* ID   */
      apszValues [4] = aszResults [usCnt] [SI_SPYR];  /* SpecYear */

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                           apszTokens, apszValues,
                           sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hhpLocSIK, szQuery);

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (usCnt % 6) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

      //if (usCnt % 32  == 0)
      //   puts (".");

      usCnt++;
      }

   // puts(" \n");
   //if (!bDone)
   //   ItiDbTerminateQuery (hqry);
   //
   //ItiMemFree (hhpLocSIK, apszValues [4]);

   if (NULL != hhpLocSIK)
      ItiMemDestroyHeap (hhpLocSIK);

   return pii->usError;

//    BOOL     bDone;
//    HQRY     hqry;
//    PSZ      *ppsz;
//    USHORT   usNumCols, usState;
//    PSZ      apszTokens [8+1], apszValues [8+1];
//    CHAR     szQuery [500];
//    CHAR     szTemp  [500];
// 
// 
//    apszTokens [0] = "ProductionDatabase"; apszValues [0] = pszProdDatabase;
//    apszTokens [1] = "TableName";      apszValues [1] = pii->pti->pszTableName;
//    apszTokens [2] = "ColumnName";     apszValues [2] = pci->pszColumnName;
//    apszTokens [3] = "TargetColumn";   apszValues [3] = "StandardItemKey";
//    apszTokens [4] = "TargetSpecYear"; apszValues [4] = ""; //ItiDbGetDbValue (hheap, ITIDB_MAX_SPEC_YEAR);
// // use the above spec year as a default if no match is found.
//    apszTokens [5] = "TargetUnitType";
//  //  apszValues [5] = (pii->  ) ? " 1 " : " 0 ";
//    apszValues [5] = " 0 ";
//    apszTokens [6] = "StdItemNumber";   apszValues [6] = "StandardItemNumber";
//    apszTokens [7] = "StdItemKey";      apszValues [7] = "";
// 
//    apszTokens [8] = ""; apszValues [8] = "";
// 
// 
//     ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTDITEMKEY_SUBQRY,
//                          szTemp, sizeof szTemp);
// 
//     ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
//                          apszTokens, apszValues,
//                          sizeof apszTokens / sizeof (PSZ));
// 
//    hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
//    if (hqry == NULL)
//       return pii->usError = -2;
// 
//    ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTANDARDITEMKEY,
//                         szTemp, sizeof szTemp);
// 
//    while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
//           pii->usError == 0)
//       {
//       apszValues [7] = ppsz[0];  /* Key */
//       apszValues [6] = ppsz[1];  /* ID  */
//       apszValues [4] = ppsz[2];  /* SpecYear */
// 
//       ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
//                            apszTokens, apszValues,
//                            sizeof apszTokens / sizeof (PSZ));
//       pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
//       ItiFreeStrArray (hheap, ppsz, usNumCols);
//       }
// 
//    if (!bDone)
//       ItiDbTerminateQuery (hqry);
// 
// 
//    // ItiMemFree (hheap, apszValues [4]);
//    return pii->usError;
   }

