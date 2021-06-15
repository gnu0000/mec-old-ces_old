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
 * paramet.c
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

#define MAX_COLS  11
#define V_MAX_COLS  6

static   PSZ apszMIQVTokens [V_MAX_COLS +1];
static   PSZ apszMIQVValues [V_MAX_COLS +1];

static   PSZ apszMIQATokens [MAX_COLS +1];
static   PSZ apszMIQAValues [MAX_COLS +1];

static   PSZ apszMIPVTokens [V_MAX_COLS +1];
static   PSZ apszMIPVValues [V_MAX_COLS +1];

static   PSZ apszMIPATokens [MAX_COLS +1];
static   PSZ apszMIPAValues [MAX_COLS +1];

static   PSZ apszMIQRTokens [MAX_COLS +1];
static   PSZ apszMIQRValues [MAX_COLS +1];

static   PSZ apszMIPRTokens [MAX_COLS +1];
static   PSZ apszMIPRValues [MAX_COLS +1];

static   CHAR szDate[50] = "";
static   PSZ  psz;
static   CHAR szTemp[500] = "";


/*******************************************************************
 * Copies
 *******************************************************************/

USHORT EXPORT ItiImpCopyParametricProfile (HHEAP       hheap,
                                           PIMPORTINFO pii,
                                           PSZ         pszProdDatabase)
   {
   HQRY hqry;
   USHORT uCols, uErr, usRet = 0;
   PSZ  *ppsz, pszTmp;
   CHAR szID [30] = "";
   CHAR szDesc [260] = "";
   CHAR szBuff [260] = "";
   CHAR szQry  [500] = "SELECT ProfileKey, ProfileID, Description "
                       " FROM DSShellImport..ParametricProfile"
                       " WHERE ProfileKey != 0 ";

   hqry = ItiDbExecQuery(szQry, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      printf ("--- ERROR in ItiImpCopyParametricProfile.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
      {
      sprintf (szQry,
         " INSERT INTO %s..ParametricProfile"
         " (ProfileKey,ProfileID,Description) VALUES ( "
         , pszProdDatabase);
      ItiStrCat (szQry, ppsz[0], sizeof szQry);  // profile key

      ItiStrCat (szQry, ", '", sizeof szQry);
      pszTmp = ItiExtract (ppsz[1], "\"");
      if ((pszTmp == NULL) || (*pszTmp == '\0'))
         pszTmp = " Blank ";
      ItiStrCat (szQry, pszTmp, sizeof szQry);   // profile ID
      ItiStrCat (szQry, "', '", sizeof szQry);

      pszTmp = ItiExtract (ppsz[2], "\"");
      if ((pszTmp == NULL) || (*pszTmp == '\0'))
         pszTmp = "No description given.";
      ItiStrCat (szQry, pszTmp, sizeof szQry);   // description
      ItiStrCat (szQry, "' ) ", sizeof szQry);

      usRet = ItiDbExecSQLStatement (hheap, szQry);

      ItiFreeStrArray(hheap, ppsz, uCols); 
      }/* end of while */

   return usRet;

   // return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPARAMETRICPROFILE);
   }

USHORT EXPORT ItiImpCopySigMajorItem (HHEAP       hheap,
                                      PIMPORTINFO pii,
                                      PSZ         pszProdDatabase)
   {
   // needs work   95 NOV
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYSIGMAJORITEM);
   }

USHORT EXPORT ItiImpCopyMIPA (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;
   HHEAP  hhp;

   hhp = ItiMemCreateHeap (8000);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIPA");
      return FALSE;
      }


   /* -- These are used by all sections. */
   apszMIPATokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIPAValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIPAValues[0] = psz;


   apszMIPATokens [1] = "MajorItemKey";              apszMIPAValues [1] = NULL;
   apszMIPATokens [2] = "BaseDate";                  apszMIPAValues [2] = NULL;
   apszMIPATokens [3] = "QualitativeVariable";       apszMIPAValues [3] = NULL;
   apszMIPATokens [4] = "QualitativeValue";          apszMIPAValues [4] = NULL;
   apszMIPATokens [5] = "PriceAdjustment";           apszMIPAValues [5] = NULL;
   apszMIPATokens [6] = NULL;                        apszMIPAValues [6] = NULL;
              
   ItiImpIndividualExec (hhp, hmod, apszMIPATokens, apszMIPAValues, 6,
                         COPYMIPA+INDV_INSERT, COPYMIPA+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemPriceAdjustment "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemPriceAdjustment "
      "(BaseDate, MajorItemKey, QualitativeVariable, "
      " QualitativeValue, PriceAdjustment)"
      " SELECT '"UNDECLARED_BASE_DATE"', "
      " MajorItemKey, QualitativeVariable"
      ", QualitativeValue, PriceAdjustment"
      " FROM %s..MajorItemPriceAdjustment"
      " WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemPriceAdjustment) "
      , psz, psz, psz, psz); 

   usRet = ItiDbExecSQLStatement (hhp, szTemp);

   return usRet;
   }


USHORT EXPORT ItiImpCopyMIPR (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;
   HHEAP  hhpMIPR;

   hhpMIPR = ItiMemCreateHeap (8000);
   if (hhpMIPR == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIPR");
      return FALSE;
      }

   /* -- These are used by all sections. */
   apszMIPRTokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIPRValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIPRValues[0] = psz;


   apszMIPRTokens [1] = "MajorItemKey";              apszMIPRValues [1] = NULL;
   apszMIPRTokens [2] = "BaseDate";                  apszMIPRValues [2] = NULL;
   apszMIPRTokens [3] = "MedianQuantity";            apszMIPRValues [3] = NULL;
   apszMIPRTokens [4] = "MedianQuantityUnitPrice";   apszMIPRValues [4] = NULL;
   apszMIPRTokens [5] = "QuantityAdjustment";        apszMIPRValues [5] = NULL;
   apszMIPRTokens [6] = NULL;                        apszMIPRValues [6] = NULL;
              
   ItiImpIndividualExec (hhpMIPR, hmod, apszMIPRTokens, apszMIPRValues, 6,
                         COPYMIPR+INDV_INSERT, COPYMIPR+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemPriceRegression "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemPriceRegression "
      "(BaseDate, MajorItemKey, "
      "MedianQuantity, MedianQuantityUnitPrice, QuantityAdjustment) "
      "SELECT '"UNDECLARED_BASE_DATE"', MajorItemKey, "
      "MedianQuantity, MedianQuantityUnitPrice, QuantityAdjustment "
      "FROM %s..MajorItemPriceRegression "
      "WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemPriceRegression ) "
      , psz, psz, psz, psz);

   usRet = ItiDbExecSQLStatement (hhpMIPR, szTemp);

   return usRet;
   }


USHORT EXPORT ItiImpCopyMIPV (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;
   HHEAP  hhpMIPV;

   hhpMIPV = ItiMemCreateHeap (8000);
   if (hhpMIPV == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIPV");
      return FALSE;
      }

   /* -- These are used by all sections. */
   apszMIPVTokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIPVValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIPVValues[0] = psz;

   apszMIPVTokens [1] = "MajorItemKey";         apszMIPVValues [1] = NULL; 
   apszMIPVTokens [2] = "BaseDate";             apszMIPVValues [2] = NULL;
   apszMIPVTokens [3] = "QualitativeVariable";  apszMIPVValues [3] = NULL;
   apszMIPVTokens [4] = NULL;                   apszMIPVValues [4] = NULL;
              
   ItiImpIndividualExec
      (hhpMIPV, hmod, apszMIPVTokens,
       apszMIPVValues, 4, COPYMIPV+INDV_INSERT, COPYMIPV+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemPriceVariable "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemPriceVariable "
      "(BaseDate, MajorItemKey, QualitativeVariable) "
      "SELECT '"UNDECLARED_BASE_DATE"', "
      " MajorItemKey, QualitativeVariable "
      "FROM %s..MajorItemPriceVariable "
      "WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemPriceVariable ) "
      , psz, psz, psz, psz); 

   usRet = ItiDbExecSQLStatement (hhpMIPV, szTemp);

   return usRet;
   }



USHORT EXPORT ItiImpCopyMIQR (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;
   HHEAP  hhp;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIQR");
      return FALSE;
      }


   /* -- These are used by all sections. */
   apszMIQRTokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIQRValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIQRValues[0] = psz;


   apszMIQRTokens [1]  = "MajorItemKey";              apszMIQRValues [1] = NULL;
   apszMIQRTokens [2]  = "BaseDate";                  apszMIQRValues [2] = NULL;
   apszMIQRTokens [3]  = "BaseCoefficient";           apszMIQRValues [3] = NULL;
   apszMIQRTokens [4]  = "DepthAdjustment";           apszMIQRValues [4] = NULL;
   apszMIQRTokens [5]  = "WidthAdjustment";           apszMIQRValues [5] = NULL;
   apszMIQRTokens [6]  = "CrossSectionAdjustment";    apszMIQRValues [6] = NULL;
   apszMIQRTokens [7]  = "StructureBaseAdjustment";   apszMIQRValues [7] = NULL;
   apszMIQRTokens [8]  = "StructureWidthAdjustment";  apszMIQRValues [8] = NULL;
   apszMIQRTokens [9]  = "StructureLengthAdjustment"; apszMIQRValues [9] = NULL;
   apszMIQRTokens [10] = "StructureAreaAdjustment";   apszMIQRValues [10] = NULL;
   apszMIQRTokens [11] = NULL;                        apszMIQRValues [11] = NULL;
              
   ItiImpIndividualExec (hhp, hmod, apszMIQRTokens, apszMIQRValues, 11,
                         COPYMIQR+INDV_INSERT, COPYMIQR+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemQuantityRegression "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemQuantityRegression "
      "(BaseDate, BaseCoefficient, CrossSectionAdjustment, "
      "DepthAdjustment, MajorItemKey, StructureAreaAdjustment, "
      "StructureBaseAdjustment, StructureLengthAdjustment, "
      "StructureWidthAdjustment, WidthAdjustment) "
      "SELECT '"UNDECLARED_BASE_DATE"', BaseCoefficient, CrossSectionAdjustment, "
      "DepthAdjustment, MajorItemKey, StructureAreaAdjustment, "
      "StructureBaseAdjustment, StructureLengthAdjustment, "
      "StructureWidthAdjustment, WidthAdjustment "
      "FROM %s..MajorItemQuantityRegression "
      "WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemQuantityRegression ) "
      , psz, psz, psz, psz);

   usRet = ItiDbExecSQLStatement (hhp, szTemp);

       //usRet = ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYMIQR);
   return usRet;
   }/* End of Function ItiImpCopyMIQR */




USHORT EXPORT ItiImpCopyMIQV (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
//   static   PSZ apszMIQVTokens [5 +1];
//   static   PSZ apszMIQVValues [5 +1];
//   static   CHAR szDate[50] = "";
//   static   PSZ  psz;
//   static   CHAR szTemp[500] = "";
   USHORT usRet = 0;
   HHEAP  hhp;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIQV");
      return FALSE;
      }


   /* -- These are used by all sections. */
   apszMIQVTokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIQVValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIQVValues[0] = psz;


   apszMIQVTokens [1] = "MajorItemKey";         apszMIQVValues [1] = NULL;
   apszMIQVTokens [2] = "BaseDate";             apszMIQVValues [2] = NULL;
   apszMIQVTokens [3] = "QualitativeVariable";  apszMIQVValues [3] = NULL;
   apszMIQVTokens [4] = NULL;                   apszMIQVValues [4] = NULL;
              
   ItiImpIndividualExec (hhp, hmod, apszMIQVTokens, apszMIQVValues, 4,
                         COPYMIQV+INDV_INSERT, COPYMIQV+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemQuantityVariable "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemQuantityVariable "
      "(BaseDate, MajorItemKey, QualitativeVariable) "
      "SELECT '"UNDECLARED_BASE_DATE"', "
      " MajorItemKey, QualitativeVariable "
      "FROM %s..MajorItemQuantityVariable "
      "WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemQuantityVariable ) "
      , psz, psz, psz, psz); 

   usRet = ItiDbExecSQLStatement (hhp, szTemp);

   return usRet;
   } /* End of Function ItiImpCopyMIQV */



USHORT EXPORT ItiImpCopyMIQA (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
//   static   PSZ apszMIQATokens [6 +1];
//   static   PSZ apszMIQAValues [6 +1];
//   static   CHAR szDate[50] = "";
//   static   PSZ  psz;
//   static   CHAR szTemp[500] = "";
   USHORT usRet = 0;
   HHEAP  hhp;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in paramet.c.ItiImpCopyMIQA");
      return FALSE;
      }


   /* -- These are used by all sections. */
   apszMIQATokens [0] = "ProductionDatabase";
   if (DosScanEnv ("DATABASE", &psz ))
      apszMIQAValues[0] = "DSShell"; //pszProdDatabase
   else
      apszMIQAValues[0] = psz;


   apszMIQATokens [1] = "MajorItemKey";              apszMIQAValues [1] = NULL;
   apszMIQATokens [2] = "BaseDate";                  apszMIQAValues [2] = NULL;
   apszMIQATokens [3] = "QualitativeVariable";       apszMIQAValues [3] = NULL;
   apszMIQATokens [4] = "VariableValue";             apszMIQAValues [4] = NULL;
   apszMIQATokens [5] = "QuantityAdjustment";        apszMIQAValues [5] = NULL;
   apszMIQATokens [6] = NULL;                        apszMIQAValues [6] = NULL;
              
   ItiImpIndividualExec (hhp, hmod, apszMIQATokens, apszMIQAValues, 6,
                         COPYMIQA+INDV_INSERT, COPYMIQA+INDV_SELECT);

   /* -- Base date setting. */
   sprintf (szTemp,
      "/* Resetting Undecl. BaseDate values */ "
      " DELETE FROM %s..MajorItemQuantityAdjustment "
      " WHERE BaseDate = '"UNDECLARED_BASE_DATE"' "
      "INSERT INTO %s..MajorItemQuantityAdjustment "
      "(BaseDate, MajorItemKey, QualitativeVariable, "
      " VariableValue, QuantityAdjustment)"
      " SELECT '"UNDECLARED_BASE_DATE"', "
      " MajorItemKey, QualitativeVariable"
      ", VariableValue, QuantityAdjustment"
      " FROM %s..MajorItemQuantityAdjustment"
      " WHERE BaseDate=(select MAX(BaseDate)"
                     " from %s..MajorItemQuantityAdjustment) "
      , psz, psz, psz, psz); 

   usRet = ItiDbExecSQLStatement (hhp, szTemp);

   return usRet;
 //  return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYMIQA);
   }

/*******************************************************************
 * Foreign keys
 *******************************************************************/




/*******************************************************************
 * Merges
 *******************************************************************/
 
/* -- The following function will:
   --    Fill in the KeyText column
   --    Match and populate the primary key in the import DB
 */
USHORT EXPORT ItiImpMergeMajorItem (HHEAP       hheap,
                                    PIMPORTINFO pii,
                                    PSZ         pszProdDatabase)
   {
   HHEAP  hhp;
   HQRY   hqry;
   USHORT usRetMMI = 0;
   USHORT usInx, usCnt, uCols, uErr;
   PSZ    *ppszTmp;
#define KEYTEXT_LEN  16
#define KEY_TXT       0 
#define ID_TXT        1
#define SPECYR        2
#define MI_KEY        3
   CHAR   aszKeys[NUMBER_OF_KEYS + 1] [4] [KEYTEXT_LEN + 1];
   CHAR szMsg[250] = "";
   CHAR szTmp[250] = "";
   CHAR szLst[200] = "/* merge */ "
                     "select RTRIM(MajorItemID) +CHAR(32)+ LTRIM(STR(SpecYear))"
                     " , MajorItemID, SpecYear, MajorItemKey "
                     "from %s..MajorItem where SpecYear > 0 ";

   CHAR szNLst[200] = " /* merge */ select RTRIM(MajorItemID) + ' NULL' "
                     " , MajorItemID, 'NULL', MajorItemKey  "
                     " from %s..MajorItem "
                     " where SpecYear = 0 OR SpecYear = NULL ";


   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in dssimp.c.ItiImpMergeMajorItem");
      return 13;
      }

   /* -- Initialize the key array from import DB first, non-null spec year. */
   sprintf (szTmp, szLst, pszImportDatabase);
   
   if (!(hqry = ItiDbExecQuery(szTmp, hhp, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpMergeMajorItem, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   aszKeys[0] [KEY_TXT] [0] = '\0';
   aszKeys[0] [ID_TXT ] [0] = '\0';
   aszKeys[0] [SPECYR ] [0] = '\0';
   aszKeys[0] [MI_KEY ] [0] = '\0';
   usCnt = 1;
   aszKeys[usCnt] [KEY_TXT] [0] = '\0';
   aszKeys[usCnt] [ID_TXT ] [0] = '\0';
   aszKeys[usCnt] [SPECYR ] [0] = '\0';
   aszKeys[usCnt] [MI_KEY ] [0] = '\0';

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_KEYS)
         {
         ItiStrCpy(aszKeys[usCnt][KEY_TXT], ppszTmp[KEY_TXT], KEYTEXT_LEN); 
         ItiStrCpy(aszKeys[usCnt][ID_TXT ], ppszTmp[ID_TXT ], KEYTEXT_LEN); 
         ItiStrCpy(aszKeys[usCnt][SPECYR ], ppszTmp[SPECYR ], KEYTEXT_LEN); 
       // ItiStrCpy (aszKeys[usCnt][MI_KEY ], ppszTmp[MI_KEY ], KEYTEXT_LEN); 
         aszKeys[usCnt+1][KEY_TXT][0] = '\0';
         aszKeys[usCnt+1][ID_TXT ][0] = '\0';
         aszKeys[usCnt+1][SPECYR ][0] = '\0';
         aszKeys[usCnt+1][MI_KEY  ][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.c.ItiImpMergeMajorItem exceeded array key space.");

      ItiFreeStrArray(hhp, ppszTmp, uCols); 
      usCnt++;
      }/* end of while */


   /* -- Initialize the key array from import DB second with null spec year. */
   sprintf (szTmp, szNLst, pszImportDatabase);
   
   if (!(hqry = ItiDbExecQuery(szTmp, hhp, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpMergeMajorItem, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_KEYS)
         {
         ItiStrCpy(aszKeys[usCnt][KEY_TXT], ppszTmp[KEY_TXT], KEYTEXT_LEN); 
         ItiStrCpy(aszKeys[usCnt][ID_TXT ], ppszTmp[ID_TXT ], KEYTEXT_LEN); 
         ItiStrCpy(aszKeys[usCnt][SPECYR ], ppszTmp[SPECYR ], KEYTEXT_LEN); 
     //   ItiStrCpy(aszKeys[usCnt][MI_KEY ], ppszTmp[MI_KEY ], KEYTEXT_LEN); 
         aszKeys[usCnt+1][KEY_TXT][0] = '\0';
         aszKeys[usCnt+1][ID_TXT ][0] = '\0';
         aszKeys[usCnt+1][SPECYR ][0] = '\0';
         aszKeys[usCnt+1][MI_KEY  ][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.c.ItiImpMergeMajorItem exceeded array key space.");

      ItiFreeStrArray(hhp, ppszTmp, uCols); 
      usCnt++;
      }/* end of while */


   /* -- Now go thru the production DB MajorItem table  */
   /* -- to check for existing ID&SpecYrs in the array. */
   /* -- Merge the key array with the prod DB, non-null spec year. */
   sprintf (szTmp, szLst, pszProdDatabase);
   ItiStrCat (szTmp, " AND MajorItemKey > 999999 ", sizeof szTmp);
   
   if (!(hqry = ItiDbExecQuery(szTmp, hhp, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpMergeMajorItem, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      /* -- Loop thru array to chk for already existing IDs&SpecYrs. */
      usInx = 1;
      while (usInx < usCnt)
         {
         if (0 == ItiStrICmp(aszKeys[usInx][KEY_TXT], ppszTmp[KEY_TXT]) )
            { /* then copy the primary key value into the array. */
            ItiStrCpy(aszKeys[usInx][MI_KEY], ppszTmp[MI_KEY], KEYTEXT_LEN);
            }
         usInx++;
         }/* end while */

      ItiFreeStrArray(hhp, ppszTmp, uCols); 
      }/* end of while */



   /* -- Merge the key array with the prod DB, null spec year. */
   sprintf (szTmp, szNLst, pszProdDatabase);
   ItiStrCat (szTmp, " AND MajorItemKey > 999999 ", sizeof szTmp);
   
   if (!(hqry = ItiDbExecQuery(szTmp, hhp, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("dssimp.c.ItiImpMergeMajorItem, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      /* -- Loop thru array to chk for already existing IDs&SpecYrs. */
      usInx = 1;
      while (usInx < usCnt)
         {
         if ((0 == ItiStrICmp(aszKeys[usInx][KEY_TXT], ppszTmp[KEY_TXT]))
             || (0 == ItiStrICmp(aszKeys[usInx][ID_TXT], ppszTmp[ID_TXT])) )
            { /* then copy the primary key value into the array. */
            ItiStrCpy(aszKeys[usInx][MI_KEY], ppszTmp[MI_KEY], KEYTEXT_LEN);
            }
         usInx++;
         }/* end while */

      ItiFreeStrArray(hhp, ppszTmp, uCols); 
      }/* end of while */


   /* -- Now loop thru the array to update the import's MajorItem table. */
   while (usCnt > 0)
      {
      if (aszKeys[usCnt][KEY_TXT][0] != '\0')
         {
         sprintf (szTmp,
            "/* merge */ "
            "UPDATE %s..MajorItem "
            " SET MajorItemKeyText = '%s' "
            " WHERE MajorItemID = '%s' "
            " AND (SpecYear = %s OR SpecYear = 0) "
            , pszImportDatabase
            , aszKeys[usCnt][KEY_TXT]
            , aszKeys[usCnt][ID_TXT ]
            , aszKeys[usCnt][SPECYR ] );

         usRetMMI = ItiDbExecSQLStatement (hhp, szTmp);
         } /* end if  */

      if (aszKeys[usCnt][MI_KEY][0] != '\0')
         {
         sprintf (szTmp,
            "/* merge */ "
            "UPDATE %s..MajorItem "
            " SET MajorItemKey = %s "
            " WHERE MajorItemKeyText = '%s' "
            , pszImportDatabase
            , aszKeys[usCnt][MI_KEY]
            , aszKeys[usCnt][KEY_TXT]);

         usRetMMI = ItiDbExecSQLStatement (hhp, szTmp);
         } /* end if  */

      usCnt--;
      } /* end of while (usCnt... */

/////   write key value array.
 usInx = 0;
 while (usInx < usCnt)
    {
    ItiStrCpy(szMsg, aszKeys[usInx][KEY_TXT], KEYTEXT_LEN);
    ItiStrCat(szMsg, " : ", KEYTEXT_LEN);
    ItiStrCat(szMsg, aszKeys[usInx][ID_TXT ], KEYTEXT_LEN);
    ItiStrCat(szMsg, " : ", KEYTEXT_LEN);
    ItiStrCat(szMsg, aszKeys[usInx][SPECYR ], KEYTEXT_LEN);
    ItiStrCat(szMsg, " : ", KEYTEXT_LEN);
    ItiStrCat(szMsg, aszKeys[usInx][MI_KEY ], KEYTEXT_LEN);
    ItiErrWriteDebugMessage (szMsg);

    usInx++;
    }/* end while */
/////

   return usRetMMI;
   } /* End of Function ItiImpMergeMajorItem */


/*******************************************************************
 * Calcs
 *******************************************************************/


USHORT EXPORT ItiImpCalcMajorItem (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   /* -- This function sets the MajorItemKeyText field of the MajorItem */
   /* -- table. */
   HQRY   hqry;
   PSZ    *ppszTmp;
   USHORT uCols, uErr;
   CHAR szQry[250] = "";
   CHAR szQLst[250] = "/* ItiImpCalcMajorItem */"
      " select distinct "
      " RTRIM(MajorItemID) + CHAR(32) "
      " + LTRIM(STR(SpecYear)) "
      " ,MajorItemID, SpecYear "
      " from DSShellImport..MajorItem "
      " where SpecYear != NULL "
      " and SpecYear != 0 ";

   CHAR szQNLst[250] = "/* ItiImpCalcMajorItem */"
      " select distinct "
      " RTRIM(MajorItemID) + ' NULL' "
      " ,MajorItemID "
      " from DSShellImport..MajorItem "
      " where SpecYear = NULL "
      " or SpecYear = 0 ";

   /* -- Do the non-null spec years first. */
   hqry = ItiDbExecQuery(szQLst, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.ItiImpCalcMajorItem, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if ((ppszTmp[0] != NULL) && (ppszTmp[1] != NULL) && (ppszTmp[2] != NULL))
         {
         sprintf (szQry,
            "/* ItiImpCalcMajorItem */ "
            "UPDATE DSShellImport..MajorItem "
            " SET MajorItemKeyText = '%s' "
            " WHERE MajorItemID = '%s' "
            " AND SpecYear = %s "
            , ppszTmp[0], ppszTmp[1], ppszTmp[2] );

         ItiDbExecSQLStatement (hheap, szQry);
         }
      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }

   /* -- Now do the null spec years. */
   hqry = ItiDbExecQuery(szQNLst, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.ItiImpCalcMajorItem, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if ((ppszTmp[0] != NULL) && (ppszTmp[1] != NULL))
         {
         sprintf (szQry,
            "/* ItiImpCalcMajorItem */ "
            "UPDATE DSShellImport..MajorItem "
            " SET MajorItemKeyText = '%s' "
            " , SpecYear = NULL "
            " WHERE MajorItemID = '%s' "
            " AND (SpecYear = NULL "
                 " OR SpecYear = 0) "
            , ppszTmp[0], ppszTmp[1] );

         ItiDbExecSQLStatement (hheap, szQry);
         }
      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }


   return pii->usError;
   }/* -- End of Function ItiImpCalcMajorItem */




USHORT EXPORT ItiImpCalcSIMI (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   PSZ    *ppszTmp, pszYr;
   USHORT uCols, uErr;
   CHAR szKeyText[50] = "";
   CHAR szUpdate[250] = "";
   CHAR szQry[250] =
                     "/* ItiImpCalcSIMI */ "
                     "SELECT DISTINCT StandardItemKeyText "
                     "FROM DSShellImport..StandardItemMajorItem "
                     "WHERE SpecYear = 0 OR SpecYear = NULL ";

/////////////////////////////////
   /* -- Get the null and 0 spec years. */
   hqry = ItiDbExecQuery(szQry, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("ItiImpCalcSIMI, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (ppszTmp[0] != NULL) 
         {
         ItiStrCpy (szKeyText, ppszTmp[0], sizeof szKeyText);
         pszYr = szKeyText;
         while ((pszYr != NULL) && (*pszYr != ' ') && (*pszYr != '\0'))
            {
            pszYr++;
            }

         if ((pszYr != NULL) && (*pszYr != '\0'))
            {
            sprintf (szUpdate,
               "/* ItiImpCalcSIMI */ "
               "UPDATE DSShellImport..StandardItemMajorItem "
               " SET SpecYear = %s "
               " WHERE StandardItemKeyText = '%s' "
               " AND (SpecYear = NULL "
                    " OR SpecYear = 0) "
               , pszYr, ppszTmp[0]);

            pii->usError = ItiDbExecSQLStatement (hheap, szUpdate);
            }

         } /* end of if (ppszTmp... */

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }

/////
   ItiStrCpy (szQry,
              "/* ItiImpCalcSIMI */"
              " SELECT MajorItemKeyText, SpecYear, UnitType"
              " FROM DSShellImport..MajorItem "
              , sizeof szQry);
   hqry = ItiDbExecQuery(szQry, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("ItiImpCalcSIMI, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (ppszTmp[0] != NULL) 
         {
         if ((ppszTmp[2] != NULL) && (*ppszTmp[2] != '\0'))
            {
            sprintf (szUpdate,
               "/* ItiImpCalcSIMI */ "
               "UPDATE DSShellImport..StandardItemMajorItem "
               " SET UnitType = %s "
               " WHERE MajorItemKeyText = '%s' "
               " AND SpecYear <= %s "
               , ppszTmp[2], ppszTmp[0], ppszTmp[1]);

            pii->usError = ItiDbExecSQLStatement (hheap, szUpdate);
            }

         } /* end of if (ppszTmp... */

      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }
/////

   return pii->usError;
   }/* -- End of Function ItiImpCalcSIMI */





////
USHORT EXPORT ItiImpCalcMIQVA (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   PSZ    *ppszTmp;
   USHORT uCols, uErr;
   CHAR szQry[250] = "";
   CHAR szQLst[] = "/* ItiImpCalcMIQVA */"
                   " select distinct MajorItemKeyText, BaseDate"
                   " from DSShellImport..MajorItemQuantityRegression ";

   hqry = ItiDbExecQuery(szQLst, hheap, 0, 0, 0, &uCols, &uErr);
   if (hqry == NULL)
      {
      ItiErrWriteDebugMessage ("dssimp.ItiImpCalcMIQV, failed ExecQuery.");
      return 13;
      }

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if ((ppszTmp[0] != NULL) && (ppszTmp[1] != NULL))
         {
         sprintf (szQry,
            "/* ItiImpCalcMIQVA */ "
            "UPDATE DSShellImport..MajorItemQuantityVariable "
            " SET BaseDate = '%s' "
            " WHERE MajorItemKeyText = '%s' "
            , ppszTmp[1], ppszTmp[0] );

         ItiDbExecSQLStatement (hheap, szQry);

         sprintf (szQry,
            "/* ItiImpCalcMIQVA */ "
            "UPDATE DSShellImport..MajorItemQuantityAdjustment "
            " SET BaseDate = '%s' "
            " WHERE MajorItemKeyText = '%s' "
            , ppszTmp[1], ppszTmp[0] );

         ItiDbExecSQLStatement (hheap, szQry);
         }
      ItiFreeStrArray(hheap, ppszTmp, uCols); 
      }


   return pii->usError;
   }/* -- End of Function ItiImpCalcMIQVA */


