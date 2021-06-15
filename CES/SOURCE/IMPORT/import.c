/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.         |
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
 * Import.c
 * Mark Hampton
 * This file is part of DS/Shell.
 *
 * This module imports files into the BAMS/PC database.
 */

#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiimp.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include "import.h"
// #include "util.h"
#include "columns.h"
#include "load.h"
#include "foreign.h"
#include "purge.h"
#include "primary.h"
#include "copy.h"
#include "calc.h"
#include "highest.h"
#include "..\include\itiglob.h"

#define MAX_BUFFER_SIZE 65500U
#define MIN_BUFFER_SIZE 16383U


#define IMP_VERSION_STRING  "1.1b4" 

/* Global variables */
extern PSZ pszBuffer = NULL;
USHORT usBufferSize = 0;
PGLOBALS pglobals;

/* pointer to string that has the name of the production database */
PSZ pszProdDatabase = NULL;

/* number of characters pointed to by pszProdDatabase */
USHORT usProdDatabaseLen = 0;


/* pointer to string that has the name of the import database */
PSZ pszImportDatabase = NULL;

/* number of characters pointed to by pszImportDatabase */
USHORT usImportDatabaseLen = 0;

PIMPORTINFO *apiiTempIDF;
PIMPORTINFO *apImpInfo;
PIMPORTINFO piiIDF;

IMPORTINFO  iiIDF;
IMPORTINFO  iiIDF1;
IMPORTINFO  iiIDF2;

PIMPORTINFO *apImpInfo2;

CHAR szNone[] = "  ";

static BOOL bTestImport = FALSE;

static void EXPORT Usage (void);
static BOOL EXPORT InitImport (HHEAP hheap);
static BOOL  EXPORT ImportFiles (HHEAP hheap, PSZ apszFileNames [], USHORT usNumFiles);

static BOOL EXPORT InitImpCodeTables (HHEAP hheap);
static BOOL EXPORT  InitImpVendorTable (HHEAP hheap);
static BOOL EXPORT  InitImpMajorItemTable (HHEAP hheap);
static BOOL EXPORT  InitImpStdItmMajorItemTable (HHEAP hheap);

int _cdecl main (int argc, char *argv [])
   {
   HHEAP hheap;

   if ((argc <= 1)
       || ( (argc > 1) && ((argv[1][0] == '/') || (argv[1][0] == '-')) && (argv[1][1] == '?')) )
      {
      Usage ();
      return 1;
      }


#ifdef PRERELEASE
   ItiErrWriteDebugMessage ("DS/Shell Import Release "IMP_VERSION_STRING" "PRERELEASE);
   puts ("DS/Shell Import Release "IMP_VERSION_STRING" "PRERELEASE);
#else
   ItiErrWriteDebugMessage ("DS/Shell Import Release "IMP_VERSION_STRING);
   puts ("DS/Shell Import Release "IMP_VERSION_STRING);
#endif
   puts ("Copyright (c) "COPYRIGHT" Info Tech, Inc.");
   printf ("Log file is: %s.  (Note this in case of error)\n",
           ItiErrQueryLogFileName ());

   hheap = ItiMemCreateHeap (0);
   if (hheap == NULL)
      {
      printf ("Could not create a heap!\n");
      return 1;
      }

   if (!InitImport (hheap))
      {
      printf ("Could not initialize the import program!\n");
      return 4;
      }

   if ( (argc > 1) && ((argv[1][0] == '/') || (argv[1][0] == '-'))
       && ((argv[1][1] == 't') || (argv[1][1] == 'T')) ) 
      {
      /* This is reset globally for other DLL import functions. */
      ItiGlobResetImportTestFlag (TRUE);

      /* This flag was initialized in InitImport; reset here if option set. */
      bTestImport = TRUE;
      }


   if (bTestImport)
      ImportFiles (hheap, argv + 2, argc - 2);
   else
      ImportFiles (hheap, argv + 1, argc - 1);

   ItiMemDestroyHeap (hheap);
   ItiMemFreeSeg (pszBuffer);
   ItiDbTerminate ();
   return 0;
   }

static BOOL EXPORT  InitImport (HHEAP hheap)
   {
   USHORT   usState, usNumCols;
   char     szTemp [255] = "";
   HQRY     hqry;

   pglobals = ItiGlobInitGlobals (NULL, NULL, NULL, NULL, "Import", "Import", VERSION_STRING);
   if (pglobals == NULL)
      {
      printf ("Could not initialize the globals module!\n");
      return FALSE;
      }
   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;

   bTestImport = ItiGlobGetImportTestFlag ();

   if (!ItiFmtInitialize ())
      {
      printf ("Could not initialize the format module!\n");
      return FALSE;
      }

   if (!ItiDbInit (2, 3))
      {
      printf ("Could not initialize the database module!\n");
      return FALSE;
      }

   if (!ItiBcpInit ())
      {
      printf ("Could not initialize the BCP module!\n");
      return FALSE;
      }

   /* set global database names */
   if (DosScanEnv ("IMPORTDATABASE", &pszImportDatabase))
      pszImportDatabase = "DSShellImport";
   usImportDatabaseLen = ItiStrLen (pszImportDatabase);

   if (DosScanEnv ("DATABASE", &pszProdDatabase))
      pszProdDatabase = "DSShell";
   usProdDatabaseLen = ItiStrLen (pszProdDatabase);

   ItiStrCpy (szTemp, "USE master", sizeof szTemp);
   usState = ItiDbExecSQLStatement (hheap, szTemp);
   if (usState != 0)
      {
      printf ("Could not USE the master database!\n");
      return FALSE;
      }

   /* -- -------------------------------------------------------------- */
   /* -- tlb FEB 95 */
   /* -- Now clearout the log space before enable of bulkcopy. */
   ItiErrWriteDebugMessage ("About to purge the logs from within InitImport.c ... ");
   printf ("Purging %s log space.\n", pszImportDatabase);

   sprintf (szTemp, 
            "DUMP TRANSACTION %s TO diskdump WITH NO_LOG ",
            pszImportDatabase);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   printf ("Purging tempdb log space.\n");
   ItiStrCpy (szTemp,
              "DUMP TRANSACTION tempdb WITH NO_LOG ",
              sizeof szTemp);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   printf ("Purging %s log space.\n", pszProdDatabase);
   sprintf (szTemp, 
            "DUMP TRANSACTION %s TO diskdump WITH NO_LOG ",
            pszProdDatabase);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;
   /* -- -------------------------------------------------------------- */

   sprintf (szTemp, "exec sp_dboption %s,'select into/bulkcopy', 'true' ",
            pszImportDatabase);
   usState = ItiDbExecSQLStatement (hheap, szTemp);
   if (usState != 0)
      {
      printf ("Could not set the select into/bulkcopy option for database %s!\n",
              pszImportDatabase);
      return FALSE;
      }


   ItiStrCpy (szTemp, "USE ", sizeof szTemp);
   ItiStrCat (szTemp, pszImportDatabase, sizeof szTemp);

   hqry = ItiDbExecQuery (szTemp, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      {
      printf ("Could not USE the import database %s!\n", pszImportDatabase);
      return FALSE;
      }

   usState = ItiDbExecSQLStatement (hheap, szTemp);
   if (usState != 0)
      {
      printf ("Could not USE the import database %s!\n", pszImportDatabase);
      return FALSE;
      }
   ItiDbTerminateQuery (hqry);

   usState = ItiDbExecSQLStatement (hheap, "CHECKPOINT");
   if (usState != 0)
      {
      printf ("Could not CHECKPOINT database %s!\n",
              pszImportDatabase);
      return FALSE;
      }

   usBufferSize = MAX_BUFFER_SIZE;
   pszBuffer = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, MEM_ZERO_INIT);
   while (pszBuffer == NULL && usBufferSize < MIN_BUFFER_SIZE)
      {
      usBufferSize = usBufferSize - usBufferSize / 4;
      pszBuffer = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, MEM_ZERO_INIT);
      }

   if (pszBuffer == NULL)
      {
      printf ("Could not allocate enough memory for buffers!\n");
      return FALSE;
      }

   /* purge tables */
   printf ("Purging import database %s tables of previous data.\n", pszImportDatabase);
   if (!PurgeTables (hheap))
      {
      printf ("ERROR: Could not purge tables!\n");
      return FALSE;
      }

   /* Now init code tables   95 tlb */
   printf ("Initializing import database %s tables...\n", pszImportDatabase);
   if (!InitImpCodeTables (hheap))
      {
      printf ("ERROR: Could not initialize import code tables!\n");
      return FALSE;
      }

   /* Now init vendor tables   95 tlb */
   if (!InitImpVendorTable (hheap))
      {
      printf ("ERROR: Could not initialize import vendor table!\n");
      return FALSE;
      }

   /* Now init major item table   95 tlb */
   if (!InitImpMajorItemTable (hheap))
      {
      printf ("ERROR: Could not initialize import major item table!\n");
      return FALSE;
      }

   /* Now init std item major item table   95 tlb */
   if (!InitImpStdItmMajorItemTable (hheap))
      {
      printf ("ERROR: Could not initialize import StdItmMajorItem table!\n");
      return FALSE;
      }



 ItiStrCpy (szTemp, "USE ", sizeof szTemp);
 ItiStrCat (szTemp, pszImportDatabase, sizeof szTemp);

 hqry = ItiDbExecQuery (szTemp, hheap, 0, 0, 0, &usNumCols, &usState);
 if (hqry == NULL)
    {
    printf ("Could not USE the import database %s!\n", pszImportDatabase);
    return FALSE;
    }

 usState = ItiDbExecSQLStatement (hheap, szTemp);
 if (usState != 0)
    {
    printf ("Could not USE the import database %s!\n", pszImportDatabase);
    return FALSE;
    }
 ItiDbTerminateQuery (hqry);

 usState = ItiDbExecSQLStatement (hheap, "CHECKPOINT");
 if (usState != 0)
    {
    printf ("Could not CHECKPOINT database %s!\n",
            pszImportDatabase);
    return FALSE;
    }

   return TRUE;
   }



static void EXPORT Usage (void)
   {
   DosBeep(1500,10);
   DosBeep(1600,10);

   fprintf (stderr, "\nCES Import program, version "IMP_VERSION_STRING" \n");
   fprintf (stderr, "Copyright (c) 1991-1997 by Info Tech, Inc. \n\n");
   fprintf (stderr, " Usage:   import [ -? | -t ] filename \n\n");
   fprintf (stderr, "      -?   This usage message is displayed, no other action is done. \n");
   fprintf (stderr, "      -t   Does all operations EXCEPT copying to database. \n");
   fprintf (stderr, "           With -t switch you get to override this just before copying. \n\n");
   fprintf (stderr, " Environment option(s) that may be SET are:\n");
   fprintf (stderr, "    TESTIMPORT=Yes   Does all operations EXCEPT copying \n");
   fprintf (stderr, "                     to database (no chance to override). \n\n");
   fprintf (stderr, "    This may be done from the commandline by:\n ");
   fprintf (stderr, "     SET TESTIMPORT=Yes \n");
   fprintf (stderr, "      IMPORT filename \n\n");
   }



USHORT EXPORT DoesTableExist (PIMPORTINFO    pii, 
                       PIMPORTINFO    *apii,
                       USHORT         usNumTables)
   {
   USHORT i;

   for (i=0; i < usNumTables; i++)
      if (ItiStrICmp (pii->pti->pszTableName, apii [i]->pti->pszTableName) == 0)
         return i + 1;
   return 0;
   }

static USHORT ImportDSShellFile (PCSVFILEINFO pcfi, 
                                 HHEAP        hheap, 
                                 PSZ          pszFileName, 
                                 PIMPORTINFO  **papii, 
                                 PUSHORT      pusNumTables)
   {
//   static PIMPORTINFO    piiIDF, *apiiTempIDF;
   USHORT         usError;
   BOOL           bDone, bReintDone = FALSE;
   char           szTemp [250] = "";
   char           szFileName [40] = "";
   CSVFILEINFO    cfiSave;
   USHORT         usMerge = 0;

   bDone = FALSE;
   while (!bDone)
      {
      /* next line should be a tables/columns line */
      usError = GetColumns (hheap, pcfi, &piiIDF);
      switch (usError)
         {
         case ERROR_BAD_TABLE:
            ItiFileSaveCSVPos (&cfiSave, pcfi);
            ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
            ItiFileRestoreCSVPos (pcfi, &cfiSave);

            printf ("Expected a Table/Column line on line %lu\n"
                    "Skiping section starting with field '%s'...\n", 
                    pcfi->ulLine, szTemp);
            bDone = ItiFileSkipCSVSection (pcfi, FALSE);
            if (bDone)
               usError = END_OF_FILE;
            break;

         case ERROR_BAD_COLUMN:
            ItiFileSaveCSVPos (&cfiSave, pcfi);
            ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
            ItiFileRestoreCSVPos (pcfi, &cfiSave);

            printf ("Line %lu contains an invalid column name.\n"
                    "Skiping section starting with field '%s'...\n", 
                    pcfi->ulLine, szTemp);
            bDone = ItiFileSkipCSVSection (pcfi, FALSE);
            if (bDone)
               usError = END_OF_FILE;
            break;

         case 0:
            break;

         case END_OF_FILE:
            bDone = TRUE;
            break;

         case ERROR_MUST_HAVE_DLL:
         case ERROR_DLL_LOAD_ERROR:
            ItiFileSaveCSVPos (&cfiSave, pcfi);
            ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
            ItiFileRestoreCSVPos (pcfi, &cfiSave);
            printf ("Error loading a dll for table %s.\nThis indicates a meta-data problem.  Section '%s' skipped.\n",
                    szTemp, szTemp);
            bDone = ItiFileSkipCSVSection (pcfi, FALSE);
            if (bDone)
               usError = END_OF_FILE;
            break;

         case ERROR_DLL_FUNCTION_NOT_FOUND:
            ItiFileSaveCSVPos (&cfiSave, pcfi);
            ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
            ItiFileRestoreCSVPos (pcfi, &cfiSave);
            printf ("Error loading a dll function for table %s.\nThis indicates a meta-data problem. Section '%s' skipped.\n",
                    szTemp, szTemp);
            bDone = ItiFileSkipCSVSection (pcfi, FALSE);
            if (bDone)
               usError = END_OF_FILE;
            break;

         default:
            printf ("Unknown CSV error (%u) on line %lu, Near column %lu.\n"
                    "Contact Info Tech Technical Support\n",
                    usError, pcfi->ulLine, pcfi->ulColumn);
            break;
         }

      if (usError == 0)
         {
         /* -- If the imported table is one that we initilized, */
         /* -- undo the initilization.                   NOV 95 */
         if (bReintDone == FALSE)
            {
            if (0 == ItiStrICmp (piiIDF->pti->pszTableName, "Vendor"))
               {
               printf (" (Resetting the import db Vendor table.)\n");
               ItiDbExecSQLStatement (hheap,
                  "TRUNCATE TABLE DSShellImport..Vendor ");
               }

            if (0 == ItiStrICmp (piiIDF->pti->pszTableName, "MajorItem"))
               {
               printf (" (Resetting the import db MajorItem table.)\n");
               ItiDbExecSQLStatement (hheap,
                  "TRUNCATE TABLE DSShellImport..MajorItem ");

               printf (" (Resetting the import db StandardItemMajorItem table.)\n");
               ItiDbExecSQLStatement (hheap,
                  "TRUNCATE TABLE DSShellImport..StandardItemMajorItem ");
               }

            bReintDone = TRUE;
            }

         printf ("Importing Table %s", piiIDF->pti->pszTableName);

         ItiStrCpy (szFileName, piiIDF->pti->pszTableName, sizeof szFileName);

         usMerge = DoesTableExist (piiIDF, *papii, *pusNumTables);
         if (usMerge)
            {
            piiIDF->ulRows = (*papii) [usMerge - 1]->ulRows;
            }

         /* process datalines */
         usError = LoadTable (pcfi, piiIDF);
         switch (usError)
            {
            case ERROR_BCP_DONE:
               printf ("BcpDone failed for table %s.  "
                       "Check for duplicate keys in import file.\n",
                       piiIDF->pti->pszTableName);
               FreeColumns (hheap, piiIDF);
               break;

            case ERROR_BCP_SET_TABLE:
               printf ("BcpSetTable failed for table %s.\n", 
                       piiIDF->pti->pszTableName);
               FreeColumns (hheap, piiIDF);
               break;

            case ERROR_BCP_BIND:
               printf ("BcpBind failed for table %s.\n", 
                       piiIDF->pti->pszTableName);
               FreeColumns (hheap, piiIDF);
               break;

            case 0:
               printf ("%lu records loaded\n", piiIDF->ulRows);
               if (usMerge)
                  {
                  (*papii) [usMerge - 1]->ulRows = piiIDF->ulRows;
                  FreeColumns (hheap, piiIDF);
                  piiIDF = NULL;
                  }
               else
                  {
                  *pusNumTables += 1;
                  apiiTempIDF = ItiMemReallocSeg (*papii, sizeof (PIMPORTINFO) * 
                                                       *pusNumTables, 0);
                  if (apiiTempIDF == NULL)
                     {
                     printf ("Out of memory storing table %s\n", 
                              piiIDF->pti->pszTableName);
                     ItiFileCloseCSV (pcfi);
                     usError = ERROR_NO_MEMORY;
                     break;
                     }
                  *papii = apiiTempIDF;
                  (*papii) [*pusNumTables-1] = piiIDF;
                  }
               break;

            default:
               printf ("Error %u loading table %s from file %s.\n", 
                       usError, piiIDF->pti->pszTableName, pszFileName);
               FreeColumns (hheap, piiIDF);
               break;
            }/* end switch */

//-- Quick patch, this will have to be delt with later!
if ((0 == ItiStrICmp (szFileName, "HistoricalProposal"))
    || (0 == ItiStrICmp (szFileName, "Historical-Proposal"))
    || (0 == ItiStrICmp (szFileName, "HistoricalProject"))
    || (0 == ItiStrICmp (szFileName, "Historical-Project")))
   {
   printf("  ---  UnitSystem/UnitType quick patch applied for %s \n ",szFileName); 
   sprintf (szTemp, "UPDATE DSShellImport..%s SET UnitType = UnitSystem ", szFileName);
   ItiDbExecSQLStatement (hheap, szTemp);
   }

         } /* end if (usError == 0... */
      } /*  end of  while (!bDone... */


   return usError;
   } //apiiTempIDF  piiIDF


/* -- Manually set the unit system in selected import */
/* -- tables AFTER the data is loaded; UnitType can no longer be null. */
static void EXPORT  PostFileLoadTweaks (HHEAP hhp, PSZ pszUnitSys)
   {
   char  szTemp [250] = "";

   if (*pszUnitSys == '\0') /* if unit type is null... */
      pszUnitSys = "0 ";    /* then default to english */

   ItiStrCpy (szTemp, "UPDATE DSShellImport..", sizeof szTemp);
   ItiStrCat (szTemp, "WeightedAverageUnitPrice SET UnitType = ", sizeof szTemp); 
   ItiStrCat (szTemp, pszUnitSys, sizeof szTemp); 
   ItiStrCat (szTemp, " WHERE UnitType = NULL ", sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   ItiStrCpy (szTemp, "UPDATE DSShellImport..", sizeof szTemp);
   ItiStrCat (szTemp, "SeasonAdjustment SET UnitType = ", sizeof szTemp); 
   ItiStrCat (szTemp, pszUnitSys, sizeof szTemp); 
   ItiStrCat (szTemp, " WHERE UnitType = NULL ", sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   ItiStrCpy (szTemp, "UPDATE DSShellImport..", sizeof szTemp);
   ItiStrCat (szTemp, "WorkTypeAdjustment SET UnitType = ", sizeof szTemp); 
   ItiStrCat (szTemp, pszUnitSys, sizeof szTemp); 
   ItiStrCat (szTemp, " WHERE UnitType = NULL ", sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   ItiStrCpy (szTemp, "UPDATE DSShellImport..", sizeof szTemp);
   ItiStrCat (szTemp, "AreaAdjustment SET UnitType = ", sizeof szTemp); 
   ItiStrCat (szTemp, pszUnitSys, sizeof szTemp); 
   ItiStrCat (szTemp, " WHERE UnitType = NULL ", sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   /* -- Now, fix the AreaKeyText fields. */
      /* Assumes AreaID is like '01' and AreaKeyText is like '1' */
      /* The bidhist.csv file was incorrectly generated. */
   ItiStrCpy (szTemp,
    " UPDATE DSShellImport..WeightedAverageUnitPrice"
    " SET AreaKeyText = '0'+AreaKeyText "
    " where AreaKeyText IN (select RIGHT(AreaID,1) from Area "
                  " WHERE AreaID != '*' AND ('0' = SUBSTRING(AreaID,1,1))) "
    , sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   ItiStrCpy (szTemp,
    " UPDATE DSShellImport..AreaAdjustment"
    " SET AreaKeyText = '0'+AreaKeyText "
    " where AreaKeyText IN (select RIGHT(AreaID,1) from Area "
                  " WHERE AreaID != '*' AND ('0' = SUBSTRING(AreaID,1,1))) "
    , sizeof szTemp); 
   ItiDbExecSQLStatement (hhp, szTemp);

   return;
   } /* End of Function PostFileLoadTweaks */


static BOOL EXPORT LoadFile (HHEAP         hheap, 
                      PSZ           pszFileName,
                      PIMPORTINFO   **papii,
                      PUSHORT       pusNumTables)
   {
   static CSVFILEINFO    cfi;
   static CHAR           szFile [250] = "";
   static CHAR           szTemp [510] = "";
   BOOL           bDelete = FALSE;
   USHORT         usError;

   switch (usError = ItiFileOpenCSV (&cfi, pszFileName, "1.00", 
                                     HIGHEST_IMPORT_VERSION))
      {
      case 0:
         break;

      case ERROR_COULD_NOT_OPEN:
         printf ("Could not open file '%s'.\n", pszFileName);
         return FALSE;

      case ERROR_NOT_CSV:
         printf ("File '%s' is not a %s CSV file.\n", pszFileName,
                  cfi.bHighEst ? "HighEst" : "DS/Shell");
         return FALSE;

      case ERROR_WRONG_VERSION:
         printf ("%s import file '%s' is a different CSV file version than expected.\n",
                 cfi.bHighEst ? "HighEst" : "DS/Shell", 
                 pszFileName);
         return FALSE;

      default:
         printf ("Unknown error %u opening file '%s'.\n"
                 "Contact Info Tech Technical Support.\n", 
                 usError, pszFileName);
         return FALSE;
      }

   sprintf (szTemp, "Importing %s import file %s.", 
           cfi.bHighEst ? "HighEst" : "DS/Shell", pszFileName);
   puts (szTemp);
   ItiErrWriteDebugMessage (szTemp);

   if (cfi.bHighEst)
      {
      usError = ConvertHighEstFile (&cfi, hheap, pszFileName, szFile, sizeof szFile);
      if (usError == 0)
         pszFileName = szFile;
      bDelete = TRUE;
      }
   if (usError == 0)
      usError = ImportDSShellFile (&cfi, hheap, pszFileName, papii, pusNumTables);

   /* -- Metrication initialization for HighEst files. */
   if ((cfi.bHighEst) && (cfi.bMetric))
     PostFileLoadTweaks (hheap, "1 "); /* metric */
   else
     PostFileLoadTweaks (hheap, "0 "); /* english */


   ItiFileCloseCSV (&cfi);
//   if (bDelete && (usError == 0 || usError == END_OF_FILE))
//      unlink (szFile);
   return usError == 0 || usError == END_OF_FILE;
   }

/* ImportFile */
static BOOL EXPORT ImportFiles (HHEAP hheap, PSZ apszFileNames [], USHORT usNumFiles)
   {
//   PIMPORTINFO *apImpInfo;
   USHORT  usNumTables;
   USHORT  i;
   INT     ch;
//   CHAR szCh[3] = "";
   PSZ  pszErr;
   BOOL bAns,  bWithErrors = FALSE;
   CHAR szMsg[100] = "";
   PSZ  pszTestEnv = NULL;

   apImpInfo = NULL;
   usNumTables = 0;

   DosScanEnv ("TESTIMPORT", &pszTestEnv);
   if (pszTestEnv != NULL)
      {
      DosBeep(1500,10);
      printf ("\n*** TestImport environment variable is set.\n");
      printf (  "*** Data will NOT be transfered into the production database.\n\n");
      DosBeep(1600,12);
      }
   else if (bTestImport)
      {
      DosBeep(1500,10);
      printf ("\n*** TestImport option switch is set.\n");
      printf (  "*** Data will NOT be transfered into the production database.\n\n");
      DosBeep(1600,12);
      }
   else
      {
      DosBeep(900,10);
      printf ("\n--- Neither TestImport environment variable or option switch is set.\n");
      printf (  "--- Data will be transfered into the production database.\n\n");
      DosBeep(1000,10);
      }

   
   for (i=0; i < usNumFiles; i++)
      {
      if (!LoadFile (hheap, apszFileNames [i], &apImpInfo, &usNumTables))
         {
         DosBeep(1200,10);
         DosBeep(1600,16);
         printf ("Error loading file %s.  File skipped.\n",
                 apszFileNames [i]);
         return FALSE;
         }
      else
         printf ("File %s loaded into import database.\n",
                 apszFileNames [i]);
      }


   apImpInfo2 = apImpInfo;

   printf ("Filling in primary keys.\n");
   ItiErrWriteDebugMessage ("Filling in primary keys.");
   if (!FillInPrimaryKeys (hheap, apImpInfo, usNumTables))
      return FALSE;

//   printf ("Filling in foreign keys.\n");
//   ItiErrWriteDebugMessage ("Filling in foreign keys.");
//   if (!FillInForeignKeys (hheap, apImpInfo, usNumTables))
//      return FALSE;

 /* Test, the FillInForeignFrgnKeys is from primary.c; when it comes  */
 /*       from foreign.c there is a parameter problem with apImpInfo. */
 printf ("Filling in Foreign keys.\n");
 ItiErrWriteDebugMessage ("Filling in Foreign keys.");
 if (!FillInForeignFrgnKeys (hheap, apImpInfo, usNumTables))
    return FALSE;


   printf ("Performing calculations.\n");
   ItiErrWriteDebugMessage ("Performing Calculations.");
   if (!CalcTables (hheap, apImpInfo, usNumTables))
      return FALSE;

   DosScanEnv ("TESTIMPORT", &pszTestEnv);
   if ((pszTestEnv != NULL) || (bTestImport))
      {
      DosBeep(1500,10);
      printf ("\n TestImport option is active, import stopped as commanded.\n");
      printf (" NO data was transfered into the production database.\n\n");
      DosBeep(1600,12);
      DosBeep(1700,10);
      DosBeep(1000,10);

      fprintf (stderr,"       Do you want to LOAD the data INTO the production database anyway (y/n)? \n");
      do{
         ch = fgetc(stdin);
         bAns = ((ch == 'Y') || (ch == 'y') || (ch == 'N') || (ch == 'n')); 
         if (!bAns)
            {
            fprintf (stderr, "\b  \r");
            DosBeep(1800,5);
            }
         else
            {
            if ((ch == 'N') || (ch == 'n'))
               {
               printf ("\n NO data was transfered into the production database.\n");
               printf ("\n    --- Normal Termination of Import Process --- \n");
               return FALSE;
               }
            /* -- Answer was YES so reset itiglob to say so. */
            ItiGlobResetImportTestFlag (FALSE);

            /* This flag was initialized in InitImport; reset here. */
            bTestImport = FALSE;

            printf ("\n\n");
            }
         } while (!bAns);
      } /* end of if ((pszTestEnv... */


   printf ("\n Copying tables to production database...\n");
   ItiErrWriteDebugMessage ("\n Copying tables to production database...");
   if (FALSE == CopyDatabase(hheap, apImpInfo, usNumTables) )
      {
      bWithErrors = TRUE;
      printf ("Attempt to transfer data into production database failed. \n");
      printf ("The following tables were not processed:\n");

      for (i=0; i < usNumTables; i++)
         {
         if ((apImpInfo != NULL) && (apImpInfo [i]->usError != 0))
            {
            switch (apImpInfo [i]->usError)
               {
               case ERROR_NO_MEMORY:
                  pszErr = " no memory left! ";
                  break;
               case ERROR_BAD_TABLE:
                  pszErr = " Bad table name ";
                  break;
               case ERROR_BAD_COLUMN:
                  pszErr = " Bad column name ";
                  break;
               case END_OF_FILE:
                  pszErr = " End of file ";
                  break;
               case ERROR_TOO_MANY_COLUMNS:
                  pszErr = " too many columns were imported ";
                  break;
               case ERROR_BAD_QUERY:
                  pszErr = " bad query sent to server ";
                  break;
               case ERROR_METABASE:
                  pszErr = " metabase data error ";
                  break;
               case ERROR_BCP_SET_TABLE:
                  pszErr = " BcpSetTable failed ";
                  break;
               case ERROR_BCP_BIND:
                  pszErr = " BcpBind failed ";
                  break;
               case ERROR_BCP_DONE:
                  pszErr = " BcpDone failed -- duplicate rows? ";
                  break;
               case ERROR_NO_KEYS:
                  pszErr = " could not get keys ";
                  break;
               case ERROR_CANT_FILL_IN_KEYS:
                  pszErr = " could not fill in keys ";
                  break;
               case ERROR_NO_KEY:
                  pszErr = " table does not have a key! ";
                  break;
               case ERROR_BAD_KEY:
                  pszErr = " bad key value ";
                  break;
               case ERROR_DLL_LOAD_ERROR:
                  pszErr = " dll could not load ";
                  break;
               case ERROR_DLL_FUNCTION_NOT_FOUND:
                  pszErr = " function could not be found in dll ";
                  break;
               case ERROR_MUST_HAVE_DLL:
                  pszErr = " must have foreign key dll ";
                  break;
               case ERROR_CANT_OPEN_TEMP_FILE:
                  pszErr = " could not open temp file for highest conversion ";
                  break;
               case ERROR_HIGHEST_CSV_ERROR:
                  pszErr = " error in HighEst CSV file ";
                  break;
               default:
                  pszErr = "Somekind of query execution failure "; //apImpInfo [i]->usError
                  break;
               } /* end switch */

            sprintf(szMsg," %s (Error: %s)\n", apImpInfo[i]->pti->pszTableName, pszErr);
            printf (szMsg);
            ItiErrWriteDebugMessage (szMsg);
            }
         }  /* end for */
      }/* end if */
   else
      printf
        ("\n ...done copying data to production database.\n Review the %s log file.\n",ItiErrQueryLogFileName ());


   if (bWithErrors)
      printf ("\n --- Termination of Import Process, with data errors. \n");
   else
      printf ("\n    --- Normal Termination of Import Process --- \n");

   return TRUE;
   } /* End of function ImportFiles */


#define A_LEN 15

static BOOL EXPORT  InitImpCodeTables (HHEAP hheap)
   {
   USHORT i, us, usState, usNumCols;
   CHAR szQry[510] = "";
   HQRY hqry;
   PSZ *ppsz, pszDatabase;
   CHAR aszID[50][A_LEN +1];
   CHAR aszKey[50][A_LEN +1];

   /* Find which database to use. */
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

      
   usState += ItiDbExecSQLStatement (hheap, "DELETE FROM DSShellImport..CodeTable ");


   sprintf(szQry,
      "INSERT INTO DSShellImport..County (CountyKey, CountyID, Name)"
      " SELECT CountyKey, CountyID, Name FROM %s..County "
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);

   sprintf(szQry,
      " /* InitImpCodeTables */ "
      "INSERT INTO DSShellImport..CodeTable (CodeTableKey, CodeTableID, MaximumCodeLength, Description)"
      " SELECT CodeTableKey, CodeTableID, MaximumCodeLength, Description "
      " FROM %s..CodeTable "
      , pszDatabase);
   usState += ItiDbExecSQLStatement (hheap, szQry);

   sprintf(szQry,
      " /* InitImpCodeTables */ "
      "INSERT INTO DSShellImport..CodeValue (CodeValueKey, CodeTableKey, CodeValueID, Translation, UnitType)"
      " SELECT CodeValueKey, CodeTableKey, CodeValueID, Translation, UnitType "
      " FROM %s..CodeValue "
      , pszDatabase);
   usState += ItiDbExecSQLStatement (hheap, szQry);


   sprintf(szQry,
      "SELECT CodeTableID, CodeTableKey FROM %s..CodeTable  "
      , pszDatabase);
   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &us);

   if (hqry == NULL)
      {
      printf ("ERROR: Could not init the import DB code table IDs!\n");
      return FALSE;
      }

   i = 0;
   while ( ItiDbGetQueryRow (hqry, &ppsz, &us) )  
      {
      if (i<50)
         {
         ItiStrCpy(aszID[i], ppsz[0], A_LEN);
         ItiStrCpy(aszKey[i], ppsz[1], A_LEN);
         }
      i++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   aszID[i][0] = '\0';
   aszKey[i][0] = '\0';


   i = 0;
   while ( aszID[i][0] != '\0' )  
      {
      sprintf(szQry,"/* import.c */ UPDATE DSShellImport..CodeValue "
                    "SET CodeTableKeyText = '%s' WHERE CodeTableKey = %s "
            , aszID[i], aszKey[i]);
      ItiDbExecSQLStatement (hheap, szQry);
      i++;
      }

   return usState == 0;
   }/* End of function InitImpCodeTables */



static BOOL EXPORT  InitImpVendorTable (HHEAP hheap)
   {
   USHORT i, us, usState, usNumCols;
   CHAR szQry[512] = "";
   HQRY hqry;
   PSZ *ppsz, pszDatabase;
#define ASZ_SIZE  100
   CHAR aszID  [ASZ_SIZE +1] [A_LEN +1];
   CHAR aszKey [ASZ_SIZE +1] [A_LEN +1];

   /* Find which database to use. */
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

   sprintf(szQry,
      " /* InitImpVendorTable */ "
      "INSERT INTO DSShellImport..Vendor"
             " (VendorKey, VendorID, ShortName, Name,"
              " Type, Certification, DBEClassification,"
              " TypeText, CertificationText, DBEClassificationText)"
      " SELECT VendorKey, VendorID, ShortName, Name, "
              " Type, Certification, DBEClassification, "
              " 'type',  'cert',  'dbe' "
      " FROM %s..Vendor WHERE Deleted IS NULL "
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);


   sprintf(szQry,
      " SELECT DISTINCT CodeValueID, CodeValueKey FROM %s..CodeValue "
      " WHERE CodeTableKey = (select CodeTableKey from CodeTable "
                             " where CodeTableID = 'VENDTYP' ) "           
      , pszDatabase);
   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &us);
   if (hqry == NULL)
      {
      printf ("ERROR: Could not init the import DB vendor classifications!\n");
      return FALSE;
      }
   i = 0;
   while ( ItiDbGetQueryRow (hqry, &ppsz, &us) )  
      {
      if (i < ASZ_SIZE)
         {
         ItiStrCpy(aszID[i], ppsz[0], A_LEN);
         ItiStrCpy(aszKey[i], ppsz[1], A_LEN);
         }
      i++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (i > ASZ_SIZE)
      {
      i = ASZ_SIZE +1;
      ItiErrWriteDebugMessage ("Vendor type array size exceeded.");
      }
   aszID[i][0] = '\0';
   aszKey[i][0] = '\0';

   i = 0;
   while ( aszID[i][0] != '\0' )  
      {
      sprintf(szQry,"/* import.c */ UPDATE DSShellImport..Vendor "
         "SET TypeText = '%s' WHERE Type = %s "
         , aszID[i], aszKey[i]);
      ItiDbExecSQLStatement (hheap, szQry);
      i++;
      }


   sprintf(szQry,
      " SELECT DISTINCT CodeValueID, CodeValueKey FROM %s..CodeValue "
      " WHERE CodeTableKey = (select CodeTableKey from CodeTable "
                             " where CodeTableID = 'CERTTYP' ) "           
      , pszDatabase);
   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &us);
   if (hqry == NULL)
      {
      printf ("ERROR: Could not init the import DB vendor classifications!\n");
      return FALSE;
      }
   i = 0;
   while ( ItiDbGetQueryRow (hqry, &ppsz, &us) )  
      {
      if (i < ASZ_SIZE)
         {
         ItiStrCpy(aszID[i], ppsz[0], A_LEN);
         ItiStrCpy(aszKey[i], ppsz[1], A_LEN);
         }
      i++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (i > ASZ_SIZE)
      {
      i = ASZ_SIZE +1;
      ItiErrWriteDebugMessage ("Vendor Certification array size exceeded.");
      }
   aszID[i][0] = '\0';
   aszKey[i][0] = '\0';

   i = 0;
   while ( aszID[i][0] != '\0' )  
      {
      sprintf(szQry,"/* import.c */ UPDATE DSShellImport..Vendor "
         "SET CertificationText = '%s' WHERE Certification = %s "
         , aszID[i], aszKey[i]);
      ItiDbExecSQLStatement (hheap, szQry);
      i++;
      }



   sprintf(szQry,
      " SELECT DISTINCT CodeValueID, CodeValueKey FROM %s..CodeValue "
      " WHERE CodeTableKey = (select CodeTableKey from CodeTable "
                             " where CodeTableID = 'DBETYPE' ) "           
      , pszDatabase);
   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &us);
   if (hqry == NULL)
      {
      printf ("ERROR: Could not init the import DB vendor classifications!\n");
      return FALSE;
      }
   i = 0;
   while ( ItiDbGetQueryRow (hqry, &ppsz, &us) )  
      {
      if (i < ASZ_SIZE)
         {
         ItiStrCpy(aszID[i], ppsz[0], A_LEN);
         ItiStrCpy(aszKey[i], ppsz[1], A_LEN);
         }
      i++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (i > ASZ_SIZE)
      {
      i = ASZ_SIZE +1;
      ItiErrWriteDebugMessage ("Vendor type array size exceeded.");
      }
   aszID[i][0] = '\0';
   aszKey[i][0] = '\0';

   i = 0;
   while ( aszID[i][0] != '\0' )  
      {
      sprintf(szQry,"/* import.c */ UPDATE DSShellImport..Vendor "
         "SET DBEClassificationText = '%s' WHERE DBEClassification = %s "
         , aszID[i], aszKey[i]);
      ItiDbExecSQLStatement (hheap, szQry);
      i++;
      }

   return usState == 0;
   }/* End of function InitImpVendorTable */




////////

static BOOL EXPORT  InitImpMajorItemTable (HHEAP hheap)
   {
   USHORT i, us, usState, usNumCols;
   CHAR szQry[510] = "";
   HQRY hqry;
   PSZ *ppsz, pszDatabase;
#define ASZ_SIZE  100
   CHAR aszID  [ASZ_SIZE +1] [A_LEN +1];
   CHAR aszKey [ASZ_SIZE +1] [A_LEN +1];

   /* Find which database to use. */
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

   sprintf(szQry,
      " /* InitImpMajorItemTable */ "
      "INSERT INTO DSShellImport..MajorItem"
             " (MajorItemKey, MajorItemID,"
             " MajorItemKeyText, "
             " Description, CommonUnit,"
              " Deleted, SpecYear, UnitType, CommonUnitText)"
      " SELECT MajorItemKey, MajorItemID,"
 //   " CHAR(034)+SUBSTRING(MajorItemID,1,4)+SUBSTRING(STR(SpecYear),6,5)+CHAR(034) "
    " SUBSTRING(MajorItemID,1,4)+SUBSTRING(STR(SpecYear),6,5) "
              ", Description, CommonUnit,"
              " Deleted, SpecYear, UnitType, 'CUTX' "
      " FROM %s..MajorItem WHERE Deleted IS NULL "
      " AND SpecYear != NULL "
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);

   sprintf(szQry,
      " /* InitImpMajorItemTable */ "
      "INSERT INTO DSShellImport..MajorItem"
             " (MajorItemKey, MajorItemID,"
             " MajorItemKeyText, "
             " Description, CommonUnit,"
              " Deleted, SpecYear, UnitType, CommonUnitText)"
      " SELECT MajorItemKey, MajorItemID,"
  //  " CHAR(034)+SUBSTRING(MajorItemID,1,4)+' NULL'+CHAR(034) "
    " SUBSTRING(MajorItemID,1,4)+' NULL' "
              ", Description, CommonUnit,"
              " Deleted, SpecYear, UnitType, 'CUTX' "
      " FROM %s..MajorItem WHERE Deleted IS NULL "
      " AND SpecYear = NULL "
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);



   sprintf(szQry,
      " SELECT DISTINCT CodeValueID, CodeValueKey FROM %s..CodeValue "
      " WHERE CodeTableKey = 1000016 "
                     //       " (select CodeTableKey from CodeTable "
                     //        " where CodeTableID = 'UNITS' ) "           
      , pszDatabase);
   hqry = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usNumCols, &us);
   if (hqry == NULL)
      {
      printf ("ERROR: Could not init the import DB MajorItem units!\n");
      return FALSE;
      }
   i = 0;
   while ( ItiDbGetQueryRow (hqry, &ppsz, &us) )  
      {
      if (i < ASZ_SIZE)
         {
         ItiStrCpy(aszID[i], ppsz[0], A_LEN);
         ItiStrCpy(aszKey[i], ppsz[1], A_LEN);
         }
      i++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (i > ASZ_SIZE)
      {
      i = ASZ_SIZE +1;
      ItiErrWriteDebugMessage ("MajorItem units array size exceeded.");
      }
   aszID[i][0] = '\0';
   aszKey[i][0] = '\0';

   i = 0;
   while ( aszID[i][0] != '\0' )  
      {
      sprintf(szQry,"/* import.c */ UPDATE DSShellImport..MajorItem "
         "SET CommonUnitText = '%s' WHERE CommonUnit = %s "
         , aszID[i], aszKey[i]);
      ItiDbExecSQLStatement (hheap, szQry);
      i++;
      }

   return usState == 0;
   }/* End of function InitImpMajorItemTable */




static BOOL EXPORT  InitImpStdItmMajorItemTable (HHEAP hheap)
   {
   USHORT usState;
   CHAR szQry[1020] = "";
   PSZ  pszDatabase;

   /* Find which database to use. */
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

   /* -- Be sure we are in the PRODUCTION database. */
   sprintf(szQry, " USE %s ", pszDatabase);
   ItiDbExecSQLStatement (hheap, szQry);
   ItiDbExecSQLStatement (hheap, szQry);

   sprintf(szQry,
      " USE %s "
      "INSERT INTO DSShellImport..StandardItemMajorItem"
      " (MajorItemKey, StandardItemKey,"
      " MajorItemKeyText,"
      " StandardItemKeyText,"
      " QuantityConversionFactor, PercentofCost, SpecYear)"
      " SELECT SIMI.MajorItemKey,"
      " SIMI.StandardItemKey,"
      " MajorItemID,"
      " RTRIM(StandardItemNumber) +CHAR(32)+ LTRIM(STR(StandardItem.SpecYear)) "
      ", SIMI.QuantityConversionFactor, PercentofCost, StandardItem.SpecYear "
      "FROM  MajorItem MI, StandardItem SI, StandardItemMajorItem SIMI "
      "WHERE MI.MajorItemKey = SIMI.MajorItemKey "
      "AND SI.StandardItemKey = SIMI.StandardItemKey "
 "AND (MI.SpecYear = SIMI.SpecYear OR MI.SpecYear = NULL)"
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);
   if (usState > 0)
      {
      printf ("ERROR: Could not init the import DB StdItmMjrItm SItxt !\n");
      return FALSE;
      }

   /* -- Now do the null spec years. */
   sprintf(szQry,
      " USE %s "
      "INSERT INTO DSShellImport..StandardItemMajorItem"
      " (MajorItemKey, StandardItemKey,"
      " MajorItemKeyText,"
      " StandardItemKeyText,"
      " QuantityConversionFactor, PercentofCost, SpecYear)"
      " SELECT StandardItemMajorItem.MajorItemKey,"
      " StandardItemMajorItem.StandardItemKey,"
      " MajorItemID,"
      " RTRIM(StandardItemNumber) + CHAR(32) + LTRIM(STR(StandardItem.SpecYear)) "
      ", SIMI.QuantityConversionFactor, PercentofCost, StandardItem.SpecYear "
      " FROM  MajorItem MI, StandardItem SI, StandardItemMajorItem SIMI"
      " WHERE MI.MajorItemKey = SIMI.MajorItemKey "
      " AND SI.StandardItemKey = SIMI.StandardItemKey "
      , pszDatabase);
   usState = ItiDbExecSQLStatement (hheap, szQry);
   if (usState > 0)
      {
      printf ("ERROR: Could not init the import DB StdItmMjrItm SItxt !\n");
      return FALSE;
      }


   usState = ItiDbExecSQLStatement (hheap, " USE DSShellImport ");
   if (usState > 0)
      {
      printf
         ("ERROR: Could not switch back to the import DB in StdItmMjrItm!\n");
      return FALSE;
      }

   return usState == 0;
   }/* End of function InitImpStdItmMajorItemTable */
