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
 * ItiImp.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * 
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include "initdll.h"

#define QRY_SIZE  2000

PSZ  pszImportDatabase;
BOOL bLogMsg;


static   PPSZ ppszRes;

static   CHAR szSelectQuery [QRY_SIZE + 1];
static   CHAR szInsertQuery [QRY_SIZE + 1];
static   CHAR szQry         [QRY_SIZE + 1];

static   CHAR szFmtPat [32] = "";

static   CHAR aszFmtValues [MAX_COLUMNS] [ITIIMP_STR1_LEN + 1];
static   CHAR aszFmt       [MAX_COLUMNS] [ITIIMP_STR1_LEN];

static   CHAR szDate [50] = "";
static   BOOL bDateSet = FALSE;
static   CHAR szNull [] = " NULL "; /* Null as a text specifier */
static   CHAR sz1900 [] = " 1900 "; /* Default spec year to use */
static   CHAR szID [32] = "";

USHORT EXPORT ItiImpExec (HHEAP       hheap,
                          PIMPORTINFO pii,
                          PSZ         pszProdDatabase,
                          HMODULE     hmod,
                          USHORT      usQueryID)
   {
   char szQuery [2040] = "";
   char szTemp [2040] = "";
   //char szDate [50] = "";
   PSZ  apszTokens [3], apszValues [3];

   if (bDateSet == FALSE)
      {
      sprintf (szDate, "'%.2d/%.2d/%.4d %.2d:%.2d:%.2d'",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour,
            (USHORT) pglobals->pgis->minutes,
            (USHORT) pglobals->pgis->seconds);
      bDateSet = TRUE;
      }

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "TodaysDate";
   apszValues [2] = szDate;


   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usQueryID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   return pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   }





static USHORT EXPORT  DoKeyTextFKs (HHEAP       hheap,
                                  HMODULE     hmod,
                                  PIMPORTINFO pii,
                                  PSZ         apszTokens [],
                                  PSZ         apszValues [],
                                  USHORT      usNumValues,
                                  USHORT      usQueryID,
                                  USHORT      usUpdateID)
   {
   char     szQuery [2040] = "";
   char     szTemp [2040] = "";
   HQRY     hqry;
   PSZ      *ppsz, pszTmp, pszYear;
   USHORT   i, usNumCols, usState;
   BOOL     bDone = FALSE;

   ItiMbQueryQueryText (usQueryID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usQueryID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, usNumValues);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (usUpdateID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usUpdateID, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      /* -- Find the year value. */
      pszTmp = ppsz[0];
      i = 0;
      while ( ((*pszTmp) != '\0')  &&  ((*pszTmp) != ' ') )
         {
         szID[i] = (*pszTmp);
         i++;
         pszTmp++;
         }
      szID[i] = '\0';
      pszYear = (pszTmp + 1);
         
      /* -- Assign key value (use ID portion of key text). */
      apszValues[11] = szID;
      apszValues [3] = ppsz[0];

      /* -- Set the SpecYear from the key text. */
      if (pszYear != NULL)
         switch (*pszYear)
            {
            case '1':
            case '2':
               apszValues [9] = pszYear;
               break;

            default:
               apszValues [9] = sz1900;
               break;
            }
     else
        apszValues [9] = sz1900;

          

      /* -- Check for a null unit type, ie. both english & metric. */
      if ((*ppsz[2]) == '\0')
         apszValues[10] = szNull;
      else
         apszValues[10] = ppsz[2];


      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, usNumValues);
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }/* End of Function DoKeyTextFKs */


static USHORT EXPORT  DoMultiSpecYearFKs (HHEAP       hheap,
                                  HMODULE     hmod,
                                  PIMPORTINFO pii,
                                  PSZ         apszTokens [],
                                  PSZ         apszValues [],
                                  USHORT      usNumValues,
                                  USHORT      usQueryID,
                                  USHORT      usUpdateID)
   {
   char     szQuery [2040] = "";
   char     szTemp [2040] = "";
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone = FALSE;

   ItiMbQueryQueryText (usQueryID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usQueryID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, usNumValues);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (usUpdateID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usUpdateID, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz[0];
      apszValues [9] = ppsz[1];
      apszValues[10] = ppsz[2];
      apszValues[11] = ppsz[0]; /* JAN 96 */

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, usNumValues);

      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }/* End of Function DoMultiSpecYearFKs */



////


static USHORT EXPORT DoParametricFKs (HHEAP       hheap,
                               HMODULE     hmod,
                               PIMPORTINFO pii,
                               PSZ         apszTokens [],
                               PSZ         apszValues [],
                               USHORT      usNumValues,
                               USHORT      usQueryID,
                               USHORT      usUpdateID)
   {
   char     szQuery [2000] = "";
   char     szTemp [2000] = "";
   HQRY     hqryPFK;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone = FALSE;

   ItiMbQueryQueryText (usQueryID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usQueryID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, usNumValues);
   hqryPFK = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqryPFK == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (usUpdateID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usUpdateID, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqryPFK, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz[0];
      apszValues [11] = ppsz[0];
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, usNumValues);
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqryPFK);

   return pii->usError;
   }/* End of Function DoParametricFKs */




static USHORT EXPORT DoFKs (HHEAP       hheap,
                            HMODULE     hmod,
                            PIMPORTINFO pii,
                            PSZ         apszTokens [],
                            PSZ         apszValues [],
                            USHORT      usNumValues,
                            USHORT      usQueryID,
                            USHORT      usUpdateID)
   {
   char     szQuery [2000] = "";
   char     szTemp  [2000] = "";
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone;

   if ((0 == usUpdateID) || (0 == usQueryID))
     {
     ItiErrWriteDebugMessage("-- MSG: ItiImp.c DoFKs function received a zero value for a query ID number. ");
     return 0;
     }

   ItiMbQueryQueryText (usQueryID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usQueryID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, usNumValues);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (usUpdateID >= ITIIMP_BASE ? 0 : hmod, ITIRID_IMPORT, 
                        usUpdateID, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = *ppsz;
      apszValues [11] = *ppsz;
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, usNumValues);
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }


USHORT EXPORT ItiImpFillInNA (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszTargetColumn)
   {
   char szQuery [2048];
   char szTemp [2048];
   PSZ  apszTokens [4], apszValues [4];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";
   apszValues [3] = pszTargetColumn;

   ItiMbQueryQueryText (0, ITIRID_IMPORT, 
                        ITIIMP_UPDATENAS, szTemp, sizeof szTemp);

   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszTokens / sizeof (PSZ));
   return pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   }



USHORT EXPORT ItiImpFillInFK (HHEAP       hheap,
                              HMODULE     hmod,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase,
                              PSZ         pszTargetColumn,
                              PSZ         pszCodeTableName,
                              PSZ         pszProdTableName,
                              PSZ         pszProdKeyName,
                              PSZ         pszProdIDName,
                              USHORT      usQueryID,
                              USHORT      usUpdateID,
                              USHORT      usFlags)
    {
   PSZ apszTokens [12], apszValues [12];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "TargetColumn";
   apszValues [4] = pszTargetColumn;
   apszTokens [5] = "CodeTableName";
   apszValues [5] = pszCodeTableName;
   apszTokens [6] = "ProdTableName";
   apszValues [6] = pszProdTableName;
   apszTokens [7] = "ProdKeyName";
   apszValues [7] = pszProdKeyName;
   apszTokens [8] = "ProdIDName";
   apszValues [8] = pszProdIDName;
   apszTokens [9] = "SpecYearValue";
   apszValues [9] = NULL;
   apszTokens[10] = "UnitTypeValue";
   apszValues[10] = NULL;
   apszTokens[11] = "AltKeyValue";
   apszValues[11] = NULL;    /* apt to be the apszValues [3] id portion. */


   if (pglobals->bUseMetric)
      apszValues[10] = " 1 ";
   else
      apszValues[10] = " 0 ";


   if (usFlags & ITIIMP_PARAMETRIC)
      {
      /* -- Use the import database as the ProdDatabase first. */
      DoParametricFKs (hheap, hmod, pii, apszTokens, apszValues, 
                 sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);

      if (usQueryID != ITIIMP_PARA_GETDISTINCTFKS)
         {
         /* -- Now use the production database as the ProdDatabase. */
         apszValues [0] = pszProdDatabase;
         DoParametricFKs (hheap, hmod, pii, apszTokens, apszValues, 
                    sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);
         }                                              

      return pii->usError;
      }



   if (usFlags & ITIIMP_KEYTEXT)
      {
      /* -- Use the import database as the ProdDatabase. */
      DoKeyTextFKs (hheap, hmod, pii, apszTokens, apszValues, 
                    sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);

      /* -- Now use the production database as the ProdDatabase. */
      apszValues [0] = pszProdDatabase;
      DoKeyTextFKs (hheap, hmod, pii, apszTokens, apszValues, 
                    sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);
      return pii->usError;
      }


   if (usFlags & ITIIMP_MULTISPECYRS)
      {
      /* -- Use the import database as the ProdDatabase first. */
      DoMultiSpecYearFKs (hheap, hmod, pii, apszTokens, apszValues, 
                 sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);

      /* -- Now use the production database as the ProdDatabase. */
      apszValues [0] = pszProdDatabase;
      DoMultiSpecYearFKs (hheap, hmod, pii, apszTokens, apszValues, 
                 sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID);
      return pii->usError;
      }


   if (!(usFlags & ITIIMP_NONAS))
      if (ItiImpFillInNA (hheap, pii, pci, pszTargetColumn))
         return pii->usError;

   if (usFlags & ITIIMP_DOIMPORTDBFIRST)
      {
      if (DoFKs (hheap, hmod, pii, apszTokens, apszValues, 
                 sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID))
         {
         return pii->usError;
         }
      }      
   apszValues [0] = pszProdDatabase;
   if (DoFKs (hheap, hmod, pii, apszTokens, apszValues, 
              sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID))
      {
      return pii->usError;
      }

   if (usFlags & ITIIMP_DOIMPORTDBLAST)
      {
      apszValues [0] = pszImportDatabase;
      if (DoFKs (hheap, hmod, pii, apszTokens, apszValues, 
                 sizeof apszTokens / sizeof (PSZ), usQueryID, usUpdateID))
         {
         return pii->usError;
         }
      }
   if (usFlags & ITIIMP_ZERONULLS)
      {
      char szQuery [2000] = "";
      char szTemp  [2000] = "";

      ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_ZERONULLSQRY, 
                           szTemp, sizeof szTemp);
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      if (pii->usError)
         {
         return pii->usError;
         }
      }
   return pii->usError;
   }/* End of Function ItiImpFillInFK */


PSZ EXPORT ItiImpQueryImportDatabase (void)
   {
   return pszImportDatabase;
   }



USHORT EXPORT ItiImpIndividualExec (HHEAP   hheap,
                                    HMODULE hmod,
                                    PSZ     apszTokens [],
                                    PSZ     apszValues [],
                                    USHORT  usNumValues,
                                    USHORT  usInsert,
                                    USHORT  usSelect)
   {
   USHORT   usError = 0;
   USHORT   usNumCols, usRow;
   USHORT    us, usDestLength, usDestMax = ITIIMP_STR1_LEN;
   HQRY     hqry;
   CHAR     szMsg[1500] = "";


   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usSelect, szSelectQuery, sizeof szSelectQuery);
   ItiStrReplaceParams (szQry, szSelectQuery, sizeof szQry, apszTokens, apszValues, usNumValues);
ItiErrWriteDebugMessage("----------------------------------------- ");
// ItiErrWriteDebugMessage(szQry);

   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return 13;
   else
      {
      szQry[0] = '\0';
      ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usInsert, szInsertQuery, sizeof szInsertQuery);
      if (usNumCols > usNumValues)
         ItiErrWriteDebugMessage("ERROR: (ItiImp.c) Number of query columns greater than number of apsTokens.\n");


      for (us=0; (us<usNumValues); us++)
         {
         aszFmtValues[us][0] = '\0';
         ItiStrCpy (&aszFmtValues[us][0], &apszValues[us][0], ITIIMP_STR1_LEN);
         }
      }

   usRow = 0;
   while (ItiDbGetQueryRow (hqry, &ppszRes, &usError))
      {
      /* need to format the results. */
      for (us=0; (us<usNumValues); us++)
         {
         if ((aszFmtValues[us] != NULL) && (aszFmtValues[us][0] == '#'))
            {
            /* test for null  */
            if ((ppszRes[us] != NULL) && ((*ppszRes[us]) != '\0') )
               ItiFmtFormatString
                  (ppszRes[us], aszFmt[us], usDestMax,
                   &aszFmtValues[us][1], &usDestLength);
            else
               ItiStrCpy(aszFmt[us], " NULL ", ITIIMP_STR1_LEN);

            apszValues[us] = aszFmt[us];

            if (bLogMsg)
               {
               sprintf(szMsg,"~Row: %u, Idx: %u, QryCol: %s  Format: %s,  new Value: %s.",
                       usRow, us, ppszRes[us], &aszFmtValues[us][1], apszValues[us]);
               ItiErrWriteDebugMessage(szMsg);
               }
            }
         else
            {
            /* test for null */
            if ((ppszRes[us] != NULL) && ((*ppszRes[us]) != '\0') )
               ItiStrCpy(aszFmt[us], ppszRes[us], ITIIMP_STR1_LEN);
            else
               ItiStrCpy(aszFmt[us], " NULL ", ITIIMP_STR1_LEN);

            /* Strip out single quotes, and commas. */
            ItiExtract (aszFmt[us], "',");

            apszValues[us] = aszFmt[us];
            if (bLogMsg)
               {
               sprintf(szMsg,"-Row: %u, Idx: %u, QryCol: %s  Value: %s.",
                       usRow, us, ppszRes[us], apszValues[us]);
               ItiErrWriteDebugMessage(szMsg);
               }
            }
         }/* end for */

      ItiStrReplaceParams (szQry, szInsertQuery, QRY_SIZE, 
                           apszTokens, apszValues, usNumCols);


      usError = ItiDbExecSQLStatement (hheap, szQry);
//ItiErrWriteDebugMessage(szQry);
      if (usRow % 50 == 0 )
         printf (".");

      usRow++;
      ItiFreeStrArray (hheap, ppszRes, usNumCols);
      } /* end of while (ItiDbGet... */

   return usError;
   } /* End of Function ItiImpIndividualExec */
