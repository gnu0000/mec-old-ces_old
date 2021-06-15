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
 * dssimp.c
 */

#define INCL_DOSMISC
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
#include "..\include\itierror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "initdll.h"

static CHAR szDllVersion[] = "1.1a0 DSSImp.DLL";

static CHAR aszHPKeys [NUMBER_OF_KEYS + 1] [3] [SZLEN];
static CHAR aszMIKeys [NUMBER_OF_MIKEYS + 1] [5] [SZLEN];
static CHAR szTmpQry  [300] = "";


PSZ pszImportDatabase;

PSZ pszDatabase;


USHORT EXPORT ResetCalcHPMIItem (PSZ pszProdDatabase)
   {
   HQRY   hqry;
   PSZ    pszTmp,  *ppszTmp;
   USHORT us, usCnt, usHPInx, uCols, uErr;
   CHAR   szQty     [20] = "";
   CHAR   szQuery [700] = "";
   CHAR   szTemp  [700] = "";
   PSZ apszLocTokens [3], apszLocValues [3];
   HHEAP  hhpReset;

   hhpReset = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpReset == NULL)
      {
      ItiErrWriteDebugMessage
         ("ERROR: NO heap could be created in ResetCalcHPMIItem");
      return 13;
      }

   DosBeep (1600, 5);
   ItiErrWriteDebugMessage ("MSG: Entering ResetCalcHPMIItem.");

   apszLocTokens [0] = "ProductionDatabase";
   apszLocValues [0] = pszProdDatabase;
   apszLocTokens [1] = "HPProposalKey";
   apszLocValues [1] = NULL;
   apszLocTokens [2] = NULL;
   apszLocValues [2] = NULL;

   sprintf(szQuery,
             "SELECT DISTINCT ProposalKey, ProposalID, SpecYear "
             " FROM %s..HistoricalProposal ORDER BY 1 "
           , pszProdDatabase);

   /* -- Initialize the historical proposal key array. */
   hqry = ItiDbExecQuery (szQuery, hhpReset, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("Init HP array failed ExecQuery.");
      return 13;
      }

   aszHPKeys[0][HPKEY][0] = '\0';
   aszHPKeys[0][HP_ID][0] = '\0';
   aszHPKeys[0][HP_SPECYR][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_KEYS)
         {
         /* -- ProposalKey */
         ItiStrCpy(aszHPKeys[usCnt][HPKEY], ppszTmp[HPKEY], SZLEN);
         ItiStrCpy(aszHPKeys[usCnt][HP_ID], ppszTmp[HP_ID], SZLEN);
         ItiStrCpy(aszHPKeys[usCnt][HP_SPECYR], ppszTmp[HP_SPECYR], SZLEN);
         aszHPKeys[usCnt+1][HPKEY][0] = '\0';
         aszHPKeys[usCnt+1][HP_ID][0] = '\0';
         aszHPKeys[usCnt+1][HP_SPECYR][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.ItiImpCalcProposalItem exceeded array key space.");

      ItiFreeStrArray(hhpReset, ppszTmp, uCols); 
      usCnt++;
      }



   /* -- Initialize the major item key array. */
   sprintf(szTmpQry,
           "SELECT DISTINCT MajorItemKey, MajorItemID, LTRIM(STR(SpecYear))"
           " FROM %s..MajorItem ORDER BY 1 "
           , pszProdDatabase);
   hqry = ItiDbExecQuery(szTmpQry, hhpReset, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpCalcProposalItem, failed ExecQuery.");
      return 13;
      }

   aszMIKeys[0][MIKEY ][0] = '\0';
   aszMIKeys[0][QTY   ][0] = '\0';
   aszMIKeys[0][EXTAMT][0] = '\0';
   aszMIKeys[0][MI_ID ][0] = '\0';
   aszMIKeys[0][MI_SPECYR][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_MIKEYS)
         {
         /* -- MajorItemKey */
         ItiStrCpy(aszMIKeys[usCnt][MIKEY], ppszTmp[MIKEY], SZLEN);
         ItiStrCpy(aszMIKeys[usCnt][MI_ID], ppszTmp[1], SZLEN);
         ItiStrCpy(aszMIKeys[usCnt][MI_SPECYR], ppszTmp[2], SZLEN);
         aszMIKeys[usCnt+1][MIKEY ][0] = '\0';
         aszMIKeys[usCnt+1][QTY   ][0] = '\0';
         aszMIKeys[usCnt+1][EXTAMT][0] = '\0';
         aszMIKeys[usCnt+1][MI_ID ][0] = '\0';
         aszMIKeys[usCnt+1][MI_SPECYR][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.c.ItiImpCalcProposalItem exceeded array key space.");

      ItiFreeStrArray(hhpReset, ppszTmp, uCols); 
      usCnt++;
      }

/* -- With each proposal key... */
usHPInx = 0;
while ((usHPInx<=NUMBER_OF_KEYS) && (aszHPKeys[usHPInx][HPKEY][0] != '\0'))
   {
   /* -- Set the value of the proposal key to use. */
   apszLocValues [1] = aszHPKeys[usHPInx][HPKEY];

   /* -- Zero out the qtys & extamts for each major item key. */
   usCnt = 0;
   while (usCnt <= NUMBER_OF_MIKEYS)
      {
      aszMIKeys[usCnt][EXTAMT][0] =  '0';
      aszMIKeys[usCnt][EXTAMT][1] = '\0';
      aszMIKeys[usCnt][QTY ][0] =  '0';
      aszMIKeys[usCnt][QTY ][1] = '\0';
      usCnt++;
      }


   /* -- Get the sum(quantity) for each major item  */
   /* -- in the current proposal key.               */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, CALCHPMIQUANTITY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszLocTokens, 
                        apszLocValues, sizeof apszLocValues / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hhpReset, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("Init of HPMI Qty array failed ExecQuery.");
      return 13;
      }

   usCnt = 0;
   while ((usCnt<=NUMBER_OF_MIKEYS) && ItiDbGetQueryRow(hqry,&ppszTmp,&uErr))
      {
      us = 0;
      while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
         {
         if (0 == ItiStrCmp(aszMIKeys[us][MIKEY], ppszTmp[MIKEY]) )
            {
            /* -- Quantity */
            pszTmp = ItiExtract (ppszTmp[QTY], ",");
            ItiStrCpy(aszMIKeys[us][QTY], pszTmp, SZLEN);
            break;
            }
         us++;
         }
      ItiFreeStrArray(hhpReset, ppszTmp, uCols); 
      usCnt++;
      }


   /* -- Get the sum(extension) for each major item */
   /* -- in the current proposal key.               */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, CALCHPMIEXTENSION, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszLocTokens, 
                        apszLocValues, sizeof apszLocValues / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hhpReset, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("ItiImpCalcProposalItem Ext, failed ExecQuery.");
      return 13;
      }

   usCnt = 0;
   while ((usCnt<=NUMBER_OF_MIKEYS) && ItiDbGetQueryRow(hqry,&ppszTmp,&uErr))
      {
      us = 0;
      while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
         {
         if (0 == ItiStrCmp(aszMIKeys[us][MIKEY], ppszTmp[MIKEY]) )
            {
            /* -- ExtendedAmount */
            pszTmp = ItiExtract (ppszTmp[EXTAMT], ",");
            ItiStrCpy(aszMIKeys[us][EXTAMT], pszTmp, SZLEN);
            break;
            }
         us++;
         }

      ItiFreeStrArray(hhpReset, ppszTmp, uCols); 
      usCnt++;
      } /* end of while ((usCnt<=NUMBER_OF_MIKEYS... */


   /* -- Now insert the array of data into the import HistPropMajItm */
   us = 0;
   while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
      {
      if ('0' != aszMIKeys[us][QTY][0])
         {
         sprintf (szQuery,
            " /* Reset HPMI table */ "
            "UPDATE HistoricalProposalMajorItem"
            " SET ExtendedAmount = %s "
            " , WeightedAverageUnitPrice = ( %s / %s ) "
            " WHERE ProposalKey = %s "
            " AND MajorItemKey = %s "
            , aszMIKeys[us][EXTAMT]
            , aszMIKeys[us][EXTAMT]
            , aszMIKeys[us][QTY]
            , aszHPKeys[usHPInx][HPKEY]
            , aszMIKeys[us][MIKEY] 
            );

         ItiDbExecSQLStatement (hhpReset, szQuery);
         }
      us++;
      }

   usHPInx++;
   } /* end of while ((usHPInx... */

   DosBeep (1600, 5);
   DosBeep (1600, 5);
   ItiErrWriteDebugMessage ("MSG: Exit from ResetCalcHPMIItem.");

   } /* End of Function ResetCalcHPMIItem */






USHORT EXPORT IndividualCopyProposal (PIMPORTINFO pii, PSZ pszProdDatabase);


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {
//   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
//   return FALSE;
   }



/*******************************************************************
 * Copies
 *******************************************************************/

USHORT EXPORT ItiImpCopyBidder (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYBIDDER);
#else

   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyFacilityMaterial (HHEAP       hheap,
                                          PIMPORTINFO pii,
                                          PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYFACILITYMATERIAL);
   }


USHORT EXPORT ItiImpCopyItemBid (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYITEMBID);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyProject (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECT);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyProjectCategory (HHEAP       hheap,
                                         PIMPORTINFO pii,
                                         PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECTCATEGORY);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyProjectItem (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECTITEM);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyProposal (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;

   /* -- The following will individually copy the proposals. */
   usRet = IndividualCopyProposal (pii, pszProdDatabase);

   return usRet;
   }


USHORT EXPORT ItiImpCopyProposalItem (HHEAP       hheap,
                                      PIMPORTINFO pii,
                                      PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROPOSALITEM);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyProposalMajorItem (HHEAP       hheap,
                                           PIMPORTINFO pii,
                                           PSZ         pszProdDatabase)
   {
#if !defined (ITIIMP_USE_MODIFICATIONS)
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROPOSALMAJORITEM);
#else
   return 0;
#endif
   }


USHORT EXPORT ItiImpCopyVendorFacility (HHEAP       hheap,
                                        PIMPORTINFO pii,
                                        PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYVENDORFACILITY);
   }


USHORT EXPORT ItiImpCopyVendor (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PSZ         pszProdDatabase)
   {
   USHORT usRet;

   ItiImpExec (hheap, pii, pszProdDatabase, hmod, UPDATEVENDOR);
   usRet = ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYVENDOR);
   ItiImpExec (hheap, pii, pszProdDatabase, hmod, SETKMVENDOR);
                 
   return usRet;
   }






/*******************************************************************
 * Foreign keys
 *******************************************************************/

USHORT EXPORT ItiImpDistrict (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;



    return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "District",
                           "DISTRIC", NULL, NULL, NULL, 
                           ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                           ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   //return usRet;
   }/* End of Function ItiImpDistrict */

USHORT EXPORT ItiImpMaterialKey (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "MaterialKey",
                          NULL, "Material", "MaterialKey", "MaterialID",
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATE1, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpProductionMethod (HHEAP       hheap,
                                      PIMPORTINFO pii,
                                      PCOLINFO    pci,
                                      PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "ProductionMethod",
                          "PRODMETH", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpProjectKey (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PCOLINFO    pci,
                                PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "ProjectKey",
                          NULL, "HistoricalProject", "ProjectKey", "ProjectControlNumber",
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATE1, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpProposalItemKey (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase)
   {
   PSZ   apszTokens [3], apszValues [3];
   char  szQuery [700] = "";
   char  szTemp [700] = "";

   /* first time arround, check the import database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;

   /* delete the bad proposal item keys */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELBADKEYTEXT, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   /* update the database */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPOSALITEMKEY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   /* second time arround, check the production database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPOSALITEMKEY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }

USHORT EXPORT ItiImpBidderItemKey (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PCOLINFO    pci,
                                   PSZ         pszProdDatabase)
   {
   char szQuery [500] = "";
   char szTemp [500] = "";
   PSZ apszTokens [3], apszValues [3];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;

   /* delete the bad bidders */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELBADKEYTEXT, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   /* update the database */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        BIDDERITEMKEY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   /* second time arround, check the production database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        BIDDERITEMKEY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }

USHORT EXPORT ItiImpProposalKey (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "ProposalKey",
                          NULL, "HistoricalProposal", "ProposalKey", "ProposalID",
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATE1, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpReason (HHEAP       hheap,
                            PIMPORTINFO pii,
                            PCOLINFO    pci,
                            PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Reason",
                          "CNAWDRS", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpRoadType (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "RoadType",
                          "HWYTYP", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpPropStandardItemKey (HHEAP       hheap,
                                         PIMPORTINFO pii,
                                         PCOLINFO    pci,
                                         PSZ         pszProdDatabase)
   {
   PSZ apszTokens [6], apszValues [6];
   char     szQuery [500] = "";
   char     szTemp [500] = "";
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone;

   /* first time arround, check the import database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "SpecYear";
   apszValues [4] = NULL;
   apszTokens [5] = "ProposalKey";
   apszValues [5] = NULL;

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   /* make the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPMAKESTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   if (ItiDbExecSQLStatement (hheap, szQuery))
      return pii->usError = -3;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPSTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPSTDITEM, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [4] = ppsz [1];
      apszValues [5] = ppsz [2];
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* second time arround, check the production database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszValues [3] = NULL;
   apszValues [4] = NULL;
   apszValues [5] = NULL;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPSTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROPSTDITEM, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [4] = ppsz [1];
      apszValues [5] = ppsz [2];
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



/*
 * This function parses pszStandardItemNumber, and returns a pointer to the
 * spec year string.  It also null terminates the item number.
 */ 

PSZ GetSpecYear (PSZ pszStdItemNumber)
   {
   /* skip leading space */
   while (ItiCharIsSpace (*pszStdItemNumber))
      pszStdItemNumber++;

   /* skip over item number -- NOTE: this assumes that item numbers
      may not have blanks!  Charles Engelke says this is OK. */
   while (*pszStdItemNumber != ' ' && *pszStdItemNumber)
      pszStdItemNumber++;
   
   /* we're at the end of the item # -- check for spec year */
   if (*pszStdItemNumber == '\0')
      return NULL;

   /* null terminate the item number string */
   *pszStdItemNumber = '\0';

   /* return a pointer to the spec year */
   return pszStdItemNumber + 1;
   }

USHORT EXPORT ItiImpProjStandardItemKey (HHEAP       hheap,
                                         PIMPORTINFO pii,
                                         PCOLINFO    pci,
                                         PSZ         pszProdDatabase)
   {
   PSZ apszTokens [6], apszValues [6];
   char     szQuery [500] = "";
   char     szTemp [500] = "";
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone;

   /* first time arround, check the import database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszImportDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "SpecYear";
   apszValues [4] = NULL;
   apszTokens [5] = "StandardItemNumber";
   apszValues [5] = NULL;

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   /* make the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        MAKEPROJSTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   if (ItiDbExecSQLStatement (hheap, szQuery))
      return pii->usError = -3;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEM, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [5] = ItiStrDup (hheap, ppsz [0]);
      apszValues [4] = GetSpecYear (apszValues [5]);

      // 10/7/1992 mdh: added next 15 lines to fix problem with import when
      // no spec year given along with standard item key.  This will cause
      // the offending item to be rejected.  No BSR for this fix.
      if (apszValues [4] == NULL || *apszValues [4] == '\0')
         {
         sprintf (szTemp,
                  "Item '%s', Spec Year '%s', Key = '%s' rejected because of "
                  "missing spec year.",
                  apszValues [5],
                  apszValues [4],
                  apszValues [3]);
         puts (szTemp);
         ItiErrWriteDebugMessage (szTemp);

         ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                              PROJSTDITEM, szTemp, sizeof szTemp);
         continue;
         }

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiMemFree (hheap, apszValues [5]);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* second time arround, check the production database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszValues [3] = NULL;
   apszValues [4] = NULL;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEMLIST, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEM, szTemp, sizeof szTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [5] = ItiStrDup (hheap, ppsz [0]);
      apszValues [4] = GetSpecYear (apszValues [5]);

      // 10/7/1992 mdh: added next 15 lines to fix problem with import when
      // no spec year given along with standard item key.  This will cause
      // the offending item to be rejected.  No BSR for this fix.
      if (apszValues [4] == NULL || *apszValues [4] == '\0')
         {
         sprintf (szTemp,
                  "Item '%s', Spec Year '%s', Key = '%s' rejected because of "
                  "missing spec year.",
                  apszValues [5],
                  apszValues [4],
                  apszValues [3]);
         puts (szTemp);
         ItiErrWriteDebugMessage (szTemp);
         
         ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                              PROJSTDITEM, szTemp, sizeof szTemp);
         continue;
         }

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiMemFree (hheap, apszValues [5]);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }


USHORT EXPORT ItiImpCountyID (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {

   return 0;

   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "CountyName",
                          NULL, "County", "Name", "CountyID", 
                          ITIIMP_GETDISTINCTFKSNONULL, ITIIMP_UPDATE1, 
                          ITIIMP_DOIMPORTDBFIRST );
   }


USHORT EXPORT ItiImpStatus (HHEAP       hheap,
                            PIMPORTINFO pii,
                            PCOLINFO    pci,
                            PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Status",
                          "CNSTAT", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpStructureType (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PCOLINFO    pci,
                                   PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "StructureType",
                          "BRDGTYP", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpUrbanRuralClass (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "UrbanRuralClass",
                          "URBRUR", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpVendorKey (HHEAP       hheap,
                               PIMPORTINFO pii,
                               PCOLINFO    pci,
                               PSZ         pszProdDatabase)
   {
   char szDestColumn [256] = "";

   ItiStrCpy (szDestColumn, pci->pszColumnName, sizeof szDestColumn);
   szDestColumn [ItiStrLen (szDestColumn) - 4] = '\0';

   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, szDestColumn,
                          NULL, "Vendor", "VendorKey", "VendorID",
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATE1, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpWorkType (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;

   usRet = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "WorkType",
                          "WRKTYP", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   return usRet;
   }



USHORT EXPORT ItiImpDBEClass (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   USHORT usRV;

   if (0 == ItiStrCmp(pii->pti->pszTableName, "Vendor") )
      {    /* NO unit-type column in the Vendor table! */
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "DBEClassification",
                           "DBETYPE", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE_CUR, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
      }
   else
      {
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "DBEClassification",
                           "DBETYPE", NULL, NULL, NULL, 
                           ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                           ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
       }

   return usRV;
   }

USHORT EXPORT ItiImpType (HHEAP       hheap,
                          PIMPORTINFO pii,
                          PCOLINFO    pci,
                          PSZ         pszProdDatabase)
   {
   USHORT usRV;

   if (0 == ItiStrCmp(pii->pti->pszTableName, "Vendor") )
      {    /* NO unit-type column in the Vendor table! */
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Type",
                          "VENDTYP", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE_CUR, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
      }
   else
      {
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Type",
                          "VENDTYP", NULL, NULL, NULL, 
                           ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                           ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
       }

   return usRV;
   }


USHORT EXPORT ItiImpCertification (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PCOLINFO    pci,
                                   PSZ         pszProdDatabase)
   {
   USHORT usRV;

   if (0 == ItiStrCmp(pii->pti->pszTableName, "Vendor") )
      {    /* NO unit-type column in the Vendor table! */
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Certification",
                          "CERTTYP", NULL, NULL, NULL, 
                          ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE_CUR, 
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
      }
   else
      {
      usRV = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, "Certification",
                           "CERTTYP", NULL, NULL, NULL, 
                           ITIIMP_GETDISTINCTFKS, ITIIMP_UPDATECODEVALUE, 
                           ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
       }

   return usRV;
   }



/*******************************************************************
 * Calcs
 *******************************************************************/

USHORT EXPORT ItiImpCalcHPMI (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   // Yet to be done. JAN 96
   // Currently done (sort of) in ItiImpCalcProposalItem 
   return 0;
   }/* -- End of Function ItiImpCalcHPMI */



USHORT EXPORT ItiImpCalcProposal (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   PSZ    *ppszTmp;
   USHORT uCols, uErr;
   CHAR szQry[125] = "";
   CHAR szFix[] = "/* ItiImpCalcProposal */"
                  " select distinct HIB.ProposalKey, HIB.VendorKey"
                  " from HistoricalProposal HP, HistoricalItemBid HIB"
                  " where HP.ProposalKey = HIB.ProposalKey"
                  " and HP.AwardedBidderKey != HIB.VendorKey ";

   /* -- First, correct the setting of the LowCostBidder flag in HIB table. */
   hqry = ItiDbExecQuery(szFix, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.ItiImpCalcProposal, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if ((ppszTmp[0] != NULL) && (ppszTmp[1] != NULL))
         {
         sprintf (szQry,
            "UPDATE HistoricalItemBid SET LowCostAlternate = 0 "
            " WHERE ProposalKey = %s "
            " AND VendorKey = %s "
            , ppszTmp[0], ppszTmp[1] );

         ItiDbExecSQLStatement (hheap, szQry);
         }
      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }

    

   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALEST);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALESTZERO);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALAWA);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALAWAZERO);
   return pii->usError;

   }/* -- End of Function ItiImpCalcProposal */




USHORT EXPORT ItiImpCalcProject (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PSZ         pszProdDatabase)
   {
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTLOW);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTLOWZERO);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTEST);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTESTZERO);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTAWA);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJECTAWAZERO);
   return pii->usError;
   }


USHORT EXPORT ItiImpCalcBidder (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PSZ         pszProdDatabase)
   {
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCBIDDER);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCLOWBIDDER);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCBIDDERZERO);
   if (pii->usError)
      return pii->usError;

   /* 921209-4103: moved these two execs from ItiImpCalcProposal to here
      so that the low bidder's totals are calculated.  mdh */
  
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALLOW);
   if (pii->usError)
      return pii->usError;
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROPOSALLOWZERO);
   if (pii->usError)
      return pii->usError;

   return pii->usError;
   }


USHORT EXPORT ItiImpCalcProposalItem (HHEAP       hheap,
                                      PIMPORTINFO pii,
                                      PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   PSZ    pszTmp,  *ppszTmp;
   USHORT us, usCnt, usHPInx, uCols, uErr;
   CHAR   szQty     [20] = "";
   CHAR   szQuery [700] = "";
   CHAR   szTemp  [700] = "";
   PSZ apszLocTokens [3], apszLocValues [3];

   apszLocTokens [0] = "ProductionDatabase";
   apszLocValues [0] = pszProdDatabase;
   apszLocTokens [1] = "HPProposalKey";
   apszLocValues [1] = NULL;
   apszLocTokens [2] = NULL;
   apszLocValues [2] = NULL;


//   /* make the HistoricalProposalMajorItem table */   OLD
//   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, MAKEHPMI);
//   if (pii->usError)
//      return pii->usError;


   /* -- Initialize the historical proposal key array. */
   hqry = ItiDbExecQuery(
               "SELECT DISTINCT ProposalKey, ProposalID, SpecYear "
               " FROM DSShellImport..HistoricalProposal ORDER BY 1 "
               ,hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.ItiImpCalcProposalItem, failed ExecQuery.");
      return 13;
      }

   aszHPKeys[0][HPKEY][0] = '\0';
   aszHPKeys[0][HP_ID][0] = '\0';
   aszHPKeys[0][HP_SPECYR][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_KEYS)
         {
         /* -- ProposalKey */
         ItiStrCpy(aszHPKeys[usCnt][HPKEY], ppszTmp[HPKEY], SZLEN);
         ItiStrCpy(aszHPKeys[usCnt][HP_ID], ppszTmp[HP_ID], SZLEN);
         ItiStrCpy(aszHPKeys[usCnt][HP_SPECYR], ppszTmp[HP_SPECYR], SZLEN);
         aszHPKeys[usCnt+1][HPKEY][0] = '\0';
         aszHPKeys[usCnt+1][HP_ID][0] = '\0';
         aszHPKeys[usCnt+1][HP_SPECYR][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.ItiImpCalcProposalItem exceeded array key space.");

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      usCnt++;
      }



   /* -- Initialize the major item key array. */
   sprintf(szTmpQry,
           "SELECT DISTINCT MajorItemKey, MajorItemID, LTRIM(STR(SpecYear))"
           " FROM %s..MajorItem ORDER BY 1 "
           , pszProdDatabase);
   hqry = ItiDbExecQuery(szTmpQry, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpCalcProposalItem, failed ExecQuery.");
      return 13;
      }

   aszMIKeys[0][MIKEY ][0] = '\0';
   aszMIKeys[0][QTY   ][0] = '\0';
   aszMIKeys[0][EXTAMT][0] = '\0';
   aszMIKeys[0][MI_ID ][0] = '\0';
   aszMIKeys[0][MI_SPECYR][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_MIKEYS)
         {
         /* -- MajorItemKey */
         ItiStrCpy(aszMIKeys[usCnt][MIKEY], ppszTmp[MIKEY], SZLEN);
         ItiStrCpy(aszMIKeys[usCnt][MI_ID], ppszTmp[1], SZLEN);
         ItiStrCpy(aszMIKeys[usCnt][MI_SPECYR], ppszTmp[2], SZLEN);
         aszMIKeys[usCnt+1][MIKEY ][0] = '\0';
         aszMIKeys[usCnt+1][QTY   ][0] = '\0';
         aszMIKeys[usCnt+1][EXTAMT][0] = '\0';
         aszMIKeys[usCnt+1][MI_ID ][0] = '\0';
         aszMIKeys[usCnt+1][MI_SPECYR][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.c.ItiImpCalcProposalItem exceeded array key space.");

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      usCnt++;
      }

/* -- With each proposal key... */
usHPInx = 0;
while ((usHPInx<=NUMBER_OF_KEYS) && (aszHPKeys[usHPInx][HPKEY][0] != '\0'))
   {
   /* -- Set the value of the proposal key to use. */
   apszLocValues [1] = aszHPKeys[usHPInx][HPKEY];

   /* -- Zero out the qtys & extamts for each major item key. */
   usCnt = 0;
   while (usCnt <= NUMBER_OF_MIKEYS)
      {
      aszMIKeys[usCnt][EXTAMT][0] =  '0';
      aszMIKeys[usCnt][EXTAMT][1] = '\0';
      aszMIKeys[usCnt][QTY ][0] =  '0';
      aszMIKeys[usCnt][QTY ][1] = '\0';
      usCnt++;
      }


   /* -- Get the sum(quantity) for each major item  */
   /* -- in the current proposal key.               */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, CALCHPMIQUANTITY, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszLocTokens, 
                        apszLocValues, sizeof apszLocValues / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("ItiImpCalcProposalItem Qty, failed ExecQuery.");
      return 13;
      }

   usCnt = 0;
   while ((usCnt<=NUMBER_OF_MIKEYS) && ItiDbGetQueryRow(hqry,&ppszTmp,&uErr))
      {
      us = 0;
      while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
         {
         if (0 == ItiStrCmp(aszMIKeys[us][MIKEY], ppszTmp[MIKEY]) )
            {
            /* -- Quantity */
            pszTmp = ItiExtract (ppszTmp[QTY], ",");
            ItiStrCpy(aszMIKeys[us][QTY], pszTmp, SZLEN);
            break;
            }
         us++;
         }
      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      usCnt++;
      }


   /* -- Get the sum(extension) for each major item */
   /* -- in the current proposal key.               */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, CALCHPMIEXTENSION, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszLocTokens, 
                        apszLocValues, sizeof apszLocValues / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("ItiImpCalcProposalItem Ext, failed ExecQuery.");
      return 13;
      }

   usCnt = 0;
   while ((usCnt<=NUMBER_OF_MIKEYS) && ItiDbGetQueryRow(hqry,&ppszTmp,&uErr))
      {
      us = 0;
      while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
         {
         if (0 == ItiStrCmp(aszMIKeys[us][MIKEY], ppszTmp[MIKEY]) )
            {
            /* -- ExtendedAmount */
            pszTmp = ItiExtract (ppszTmp[EXTAMT], ",");
            ItiStrCpy(aszMIKeys[us][EXTAMT], pszTmp, SZLEN);
            break;
            }
         us++;
         }

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      usCnt++;
      } /* end of while ((usCnt<=NUMBER_OF_MIKEYS... */


   /* -- Now insert the array of data into the import HistPropMajItm */
   us = 0;
   while ((us <= NUMBER_OF_MIKEYS) && (aszMIKeys[us][MIKEY][0] != '\0'))
      {
      if ('0' != aszMIKeys[us][QTY][0])
         {
         sprintf (szQuery,
            " /* Imports HPMI table */ "
            "INSERT INTO HistoricalProposalMajorItem "
            "(ProposalKey, MajorItemKey, Quantity, ExtendedAmount, "
            "ProposalKeyText, MajorItemKeyText,   WeightedAverageUnitPrice) "
            "VALUES (%s, %s, %s, %s, '%s', '%s',  %s / %s ) "
           ,aszHPKeys[usHPInx][HPKEY],
            aszMIKeys[us][MIKEY],
            aszMIKeys[us][QTY],
            aszMIKeys[us][EXTAMT],
            aszHPKeys[usHPInx][HP_ID],
            aszMIKeys[us][MI_ID],

            aszMIKeys[us][EXTAMT],
            aszMIKeys[us][QTY]
            );

         ItiDbExecSQLStatement (hheap, szQuery);
         }
      us++;
      }


   usHPInx++;
   } /* end of while ((usHPInx... */


 //  pii->usError = ItiImpExec
 //     (hheap, pii, pszProdDatabase, hmod, CALCHPMIUNITPRICE);
 //
 //  if (pii->usError)
 //     return pii->usError;
 //
 //  /* copy the table to the production database: done else where. */
 //  pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYHPMI);
 //  if (pii->usError)
 //     return pii->usError;
 
   } /* End of Function */



/* ========================== New import functions */
BOOL EXPORT CopyHistProp (PSZ pszKey)
   {
   static   PSZ apszTokens [MAX_COLUMNS +1];
   static   PSZ apszValues [MAX_COLUMNS +1];
   static   CHAR szDate[50];
   static   PSZ  psz;
   USHORT usi;
   HHEAP  hheap;
   CHAR   szKey[15] = "";  /* ProposalKey */

   if ((pszKey == NULL) || (*pszKey == '\0'))
      return FALSE;
   else
      ItiStrCpy (szKey, pszKey, sizeof szKey);


   /* START HERE to make this stuff work */
   ItiErrWriteDebugMessage("\n =========== Start of CopyHistProp ==== ");
   ItiErrWriteDebugMessage(pszKey);

   hheap = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hheap == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in dssimp.c.CopyHistProp");
      return FALSE;
      }

   for(usi=0; usi < MAX_COLUMNS; usi++)
      {
      apszValues[usi] = NULL;
      }


   /* -- These are used by all sections. */
   apszTokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszValues[0] = "DSShell";
   else
      apszValues[0] = psz;
   //  apszValues [0] = "DSShell";


   /* -- Historical Proposals */
   ItiDbExecSQLStatement (hheap, 
      "/* Fix */ UPDATE DSShellImport..HistoricalProposal"
      " SET LowBidderKey = AwardedBidderKey"
      " WHERE LowBidderKey IS NULL " );

   /* -- Set the ImportDate value from pglobals only! */           
   sprintf (szDate, "'%.2d/%.2d/%.4d %.2d:%.2d:%.2d'",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour,
            (USHORT) pglobals->pgis->minutes,
            (USHORT) pglobals->pgis->seconds);

   apszTokens [1]  = "TodaysDate";              apszValues [1] = szDate;
   apszTokens [2]  = "ProposalKey";             apszValues [2] = szKey;
   apszTokens [3]  = "ProposalID";              apszValues [3] = NULL;
   apszTokens [4]  = "Description";             apszValues [4] = NULL;
   apszTokens [5]  = "ProjectNumber";           apszValues [5] = NULL;
   apszTokens [6]  = "Location";                apszValues [6] = NULL; // apszValues [6] = "#Position,<long><decimal> <lat><decimal>";
   apszTokens [7]  = "LocationDescription";     apszValues [7] = NULL;
   apszTokens [8]  = "SpecYear";                apszValues [8] = NULL;
   apszTokens [9]  = "District";                apszValues [9] = NULL;
   apszTokens [10] = "CountyID";                apszValues [10] = NULL;
   apszTokens [11] = "CountyName";              apszValues [11] = NULL;
   apszTokens [12] = "WorkType";                apszValues [12] = NULL;
   apszTokens [13] = "RoadType";                apszValues [13] = NULL;
   apszTokens [14] = "LettingDate";             apszValues [14] = NULL;
   apszTokens [15] = "LowBidderKey";            apszValues [15] = NULL;
   apszTokens [16] = "AwardedBidderKey";        apszValues [16] = NULL;
   apszTokens [17] = "NumberOfBids";            apszValues [17] = NULL;
   apszTokens [18] = "LowBidTotal";             apszValues [18] = "#Number,....";
   apszTokens [19] = "AwardedBidTotal";         apszValues [19] = "#Number,....";
   apszTokens [20] = "EngineersEstimate";       apszValues [20] = "#Number,..";
   apszTokens [21] = "DateComplete";            apszValues [21] = NULL;
   apszTokens [22] = "Status";                  apszValues [22] = NULL;
   apszTokens [23] = "PercentComplete";         apszValues [23] = NULL;
   apszTokens [24] = "RejectedFlag";            apszValues [24] = NULL;
   apszTokens [25] = "Reason";                  apszValues [25] = NULL;
   apszTokens [26] = "AveragePavementDepth";    apszValues [26] = NULL;
   apszTokens [27] = "AveragePavementWidth";    apszValues [27] = NULL;
   apszTokens [28] = "ProposalLength";          apszValues [28] = NULL;
   apszTokens [29] = NULL;                      apszValues [29] = NULL;
              
   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 29,
                         COPYPROPOSAL+INDV_INSERT, COPYPROPOSAL+INDV_SELECT);

   /* -- Handle 'Location' data here. */
   ItiStrCpy (szTmpQry, "UPDATE ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, apszValues[0], sizeof szTmpQry);
   ItiStrCat (szTmpQry,
                "..HistoricalProposal SET Location = "
                "(SELECT Location FROM DSShellImport..HistoricalProposal"
                " WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiStrCat (szTmpQry, ") WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiDbExecSQLStatement (hheap, szTmpQry);


   /* -- Handle 'LocationDescription' data here. */
   ItiStrCpy (szTmpQry, "UPDATE ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, apszValues[0], sizeof szTmpQry);
   ItiStrCat (szTmpQry,
         "..HistoricalProposal SET LocationDescription = "
         "(SELECT LocationDescription FROM DSShellImport..HistoricalProposal"
         " WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiStrCat (szTmpQry, ") WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiDbExecSQLStatement (hheap, szTmpQry);


   /* -- Handle 'Description' text (with imbedded quote marks) here. */
   ItiStrCpy (szTmpQry, "UPDATE ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, apszValues[0], sizeof szTmpQry);
   ItiStrCat (szTmpQry,
         "..HistoricalProposal SET Description = "
         "(SELECT Description FROM DSShellImport..HistoricalProposal"
         " WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiStrCat (szTmpQry, ") WHERE ProposalKey = ", sizeof szTmpQry);
   ItiStrCat (szTmpQry, szKey, sizeof szTmpQry);
   ItiDbExecSQLStatement (hheap, szTmpQry);




   /* -- Historical Proposal Items */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROPOSALITEM);
   apszTokens [1] = "ProposalKey";       apszValues [1] = szKey;
   apszTokens [2] = "ProposalItemKey";   apszValues [2] = NULL;
   apszTokens [3] = "ItemKey";           apszValues [3] = NULL;
   apszTokens [4] = "Quantity";          apszValues [4] = "#Number,....";
   apszTokens [5] = "AlternateGroup";    apszValues [5] = "#String";
   apszTokens [6] = "Alternate";         apszValues [6] = "#String";
   apszTokens [7] = "LineNumber";        apszValues [7] = "#String";
   apszTokens [8] = NULL;                apszValues [8] = NULL;

   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 8,
                         COPYPROPOSALITEM+INDV_INSERT, COPYPROPOSALITEM+INDV_SELECT);



   /* -- Historical Bidders */
   apszTokens [1] = "ProposalKey";   apszValues [1] = szKey;
   apszTokens [2] = "VendorKey";     apszValues [2] = NULL;
   apszTokens [3] = "BidTotal";      apszValues [3] = "#Number,....";
   apszTokens [4] = NULL;            apszValues [4] = NULL;
   
   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 4,
                         COPYBIDDER+INDV_INSERT, COPYBIDDER+INDV_SELECT);


   /* -- Historical Item Bids */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYITEMBID);
   apszTokens [1] = "ProposalKey";       apszValues [1] = szKey;
   apszTokens [2] = "ProposalItemKey";   apszValues [2] = NULL;
   apszTokens [3] = "VendorKey";         apszValues [3] = NULL;
   apszTokens [4] = "UnitPrice";         apszValues [4] = "#Number,....";
   apszTokens [5] = "LowCostAlternate";  apszValues [5] = NULL;
   apszTokens [6] = NULL;                apszValues [6] = NULL;

   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 6,
                         COPYITEMBID+INDV_INSERT, COPYITEMBID+INDV_SELECT);


   /* -- Historical Projects */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECT);

   apszTokens [1]  = "ProposalKey";          apszValues [1] = szKey;
   apszTokens [2]  = "ProjectControlNumber"; apszValues [2] = NULL;
   apszTokens [3]  = "Description";          apszValues [3] = NULL;
   apszTokens [4]  = "ProjectKey";           apszValues [4] = NULL;
   apszTokens [5]  = "SpecYear";             apszValues [5] = NULL;
   apszTokens [6]  = "WorkType";             apszValues [6] = NULL;
   apszTokens [7]  = "District";             apszValues [7] = NULL;
   apszTokens [8]  = "UrbanRuralClass";      apszValues [8] = NULL;
   apszTokens [9]  = "Location";             apszValues [9] = NULL;
   apszTokens [10] = "AwardedAmount";        apszValues [10] = "#Number,....";
   apszTokens [11] = "LowBidAmount";         apszValues [11] = "#Number,....";
   apszTokens [12] = "EstimatedAmount";      apszValues [12] = "#Number,....";
   apszTokens [13] = "Length";               apszValues [13] = "#Number,...";
   apszTokens [14] = "Width";                apszValues [14] = "#Number,...";
   apszTokens [15] = NULL;                   apszValues [15] = NULL;

   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 15,
                         COPYPROJECT+INDV_INSERT, COPYPROJECT+INDV_SELECT);
//   /* -- Handle 'Location' data here. */
//   ItiStrCpy (szTmpQry, "UPDATE ", sizeof szTmpQry);
//   ItiStrCat (szTmpQry, apszValues[0], sizeof szTmpQry);
//   ItiStrCat (szTmpQry,
//                "..HistoricalProject SET Location = "
//                "(SELECT Location FROM DSShellImport..HistoricalProject"
//                " WHERE ProjectKey = ", sizeof szTmpQry);
//   ItiStrCat (szTmpQry, apszValues[4], sizeof szTmpQry);
//   ItiStrCat (szTmpQry, ") WHERE ProjectKey = ", sizeof szTmpQry);
//   ItiStrCat (szTmpQry, apszValues[4], sizeof szTmpQry);
//   ItiDbExecSQLStatement (hheap, szTmpQry);




   /* -- Historical Project Categories */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECTCATEGORY);

   apszTokens [1]  = "ProjectKey";          apszValues [1]  = NULL;
   apszTokens [2]  = "CategoryNumber";      apszValues [2]  = NULL;
   apszTokens [3]  = "Description";         apszValues [3]  = NULL;
   apszTokens [4]  = "Length";              apszValues [4]  = "#Number,...";
   apszTokens [5]  = "Width";               apszValues [5]  = "#Number,...";
   apszTokens [6]  = "Structure";           apszValues [6]  = NULL;
   apszTokens [7]  = "StructureID";         apszValues [7]  = NULL;
   apszTokens [8]  = "StructureLength";     apszValues [8]  = "#Number,...";
   apszTokens [9]  = "StructureWidth";      apszValues [9]  = "#Number,...";
   apszTokens [10] = "StructureType";       apszValues [10] = NULL;
   apszTokens [11] = "ProposalKey";         apszValues [11]  = szKey;
   apszTokens [12] = NULL;                  apszValues [12] = NULL;
                                                       
   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 12,
                         COPYPROJECTCATEGORY+INDV_INSERT, COPYPROJECTCATEGORY+INDV_SELECT);


   /* -- Historical Project Items */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJECTITEM);
   apszTokens [1] = "ProposalKey";       apszValues [1] = szKey;
   apszTokens [2] = "ProjectKey";        apszValues [2] = NULL;
   apszTokens [3] = "CategoryNumber";    apszValues [3] = NULL;
   apszTokens [4] = "SequenceNumber";    apszValues [4] = NULL;
   apszTokens [5] = "ProposalItemKey";   apszValues [5] = NULL;
   apszTokens [6] = "ItemKey";           apszValues [6] = NULL;
   apszTokens [7] = "Quantity";          apszValues [7] = "#Number,....";
   apszTokens [8] = NULL;                apszValues [8] = NULL;
 
   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 8,
                         COPYPROJECTITEM+INDV_INSERT, COPYPROJECTITEM+INDV_SELECT);



   /* -- Historical Proposal Major Items */
//   ImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROPOSALMAJORITEM);
   apszTokens [1] = "ProposalKey";               apszValues [1] = szKey;
   apszTokens [2] = "MajorItemKey";              apszValues [2] = NULL;
   apszTokens [3] = "Quantity";                  apszValues [3] = "#Number,....";
   apszTokens [4] = "ExtendedAmount";            apszValues [4] = "#Number,....";
   apszTokens [5] = "WeightedAverageUnitPrice";  apszValues [5] = "#Number,....";
   apszTokens [6] = NULL;                        apszValues [6] = NULL;

   ItiImpIndividualExec (hheap, hmod, apszTokens, apszValues, 6,
                         COPYPROPOSALMAJORITEM+INDV_INSERT, COPYPROPOSALMAJORITEM+INDV_SELECT);


   ItiErrWriteDebugMessage(" ============= End of CopyHistProp ==== ");
   if (hheap != NULL)
      ItiMemDestroyHeap (hheap);

   return TRUE;   
   } /* End of Function CopyHistProp */





USHORT EXPORT IndividualCopyProposal (PIMPORTINFO pii,
                                      PSZ         pszProdDatabase)
   {
   BOOL   bOK;
   USHORT i, usCount, usRet, usCnt, uCols, uErr;
   PSZ    pszCount, *ppszTmp;
   CHAR   aszKeys[NUMBER_OF_KEYS + 1][KEY_LEN + 1];
   CHAR   szTmp[256] = "";
   HHEAP  hheap;
   HQRY   hqry;


   hheap = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hheap == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in dssimp.c");
      return 13;
      }

   sprintf(szTmp,"/* Enter dssimp.c.IndividualCopyProposal */ USE  %s ", pszImportDatabase);
   usRet = ItiDbExecSQLStatement (hheap, szTmp);

   /* -- Get the total number of proposals. */
   sprintf(szTmp,"SELECT count(*) FROM %s..HistoricalProposal "
                 "WHERE ProposalKey != NULL ", pszImportDatabase);
   pszCount = ItiDbGetRow1Col1(szTmp, hheap, 0, 0, 0);
   if (pszCount != NULL)
      {
      ItiStrToSHORT (pszCount, &usCount);
      ItiMemFree(hheap, pszCount);
      }
   else
      usCount = 0;

   /* -- Initialize the key array. */
   sprintf(szTmp,"SELECT ProposalKey FROM %s..HistoricalProposal "
                 "WHERE ProposalKey != NULL ", pszImportDatabase);
   if (!(hqry = ItiDbExecQuery(szTmp, hheap, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("dssimp.c.IndividualCopyProposal, failed ExecQuery.");
      ItiMemDestroyHeap (hheap);
      return 13;
      }

   aszKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_KEYS)
         {
         ItiStrCpy(aszKeys[usCnt], ppszTmp[0], KEY_LEN); 
         aszKeys[usCnt+1][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.c.IndividualCopyProposal exceeded array key space.");

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      usCnt++;
      }/* end of while */


   /* -- ================= Now do the copying of the individual proposals. */
   for (i = 0; i < usCount; i++)
      {
      usRet = ItiDbExecSQLStatement (hheap, "BEGIN TRANSACTION ");
      printf ("ú");
      bOK = CopyHistProp (aszKeys[i]);

      if (bOK)
         {
         usRet = ItiDbExecSQLStatement (hheap, "COMMIT TRANSACTION ");
         }
      else
         {
         usRet = ItiDbExecSQLStatement (hheap, "ROLLBACK TRANSACTION ");
         sprintf (szTmp,"*** DSShellImport HistoricalProposal key %s failed.", aszKeys[i] );
         ItiErrWriteDebugMessage(szTmp);
         printf (szTmp);
         }

      }/* end of for loop */

   if (hheap != NULL)
      ItiMemDestroyHeap (hheap);

   return usRet;
   } /* End of Function IndividualCopyProposal */


