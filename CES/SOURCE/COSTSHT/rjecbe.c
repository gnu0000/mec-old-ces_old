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


/*  CES Report DLL Source Code
 *
 *  Timothy Blake (tlb)
 *
 */


#define     INCL_BASE
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI
#define     INCL_WIN


#include    "..\include\iti.h"     
#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"
#include    "..\include\itimbase.h"
#include    "..\include\itidbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\colid.h"
#include    "..\include\winid.h"
#include    "..\include\itiwin.h"
#include    "..\include\dialog.h"
#include    "..\include\itimenu.h"
#include    "dialog.h"

#include    "..\include\itirptdg.h"
#include    "..\include\itirpt.h"

#include    <stdio.h>


/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif


#include    "rindljec.h"


#include "rJECbe.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "JE: CostBased"

#define  QUERY_ARRAY_SIZE       1279



#define SETQERRMSG ItiStrCpy (szErrorMsg,                                    \
                              "ERROR: Query failed for ",                    \
                              sizeof szErrorMsg);                            \
                                                                             \
                   ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);         \
                   ItiStrCat (szErrorMsg,", Error Code: ",sizeof szErrorMsg);\
                                                                             \
                   ItiStrUSHORTToString                                      \
                      (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],            \
                       sizeof szErrorMsg);                                   \
                                                                             \
                   ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);   \
                   ItiStrCat (szErrorMsg, szQuery, sizeof szErrorMsg);       \
                                                                             \
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg)




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rjecbe.dll";

static  CHAR  szCurrentJobID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


/* -- Current keys to be inserted into the queries: */
static  CHAR  szCurJobKey         [SMARRAY + 1] = "";
static  CHAR  szCurJobCstShtKey   [SMARRAY + 1] = "";
static  CHAR  szCurCostSheetID [SMARRAY + 1] = "";


static  PP_KEYSIDS ppkisJobBrkDwnKeys  = NULL;
static  USHORT usJobBrkDwnCnt = 0;
static  CHAR   szCurJobBrkDwnKey [SZKEY_LEN +1] = "";
static  CHAR   szCurJobBrkDwnID  [SMARRAY +1] = "";

static  PP_KEYSIDS ppkisJobItemKeys    = NULL;
static  USHORT usJobItemCnt = 0;
static  CHAR   szCurJobItemKey [SZKEY_LEN +1] = "";
static  CHAR   szCurJobItemID  [SMARRAY +1] = "";

static  PP_KEYSIDS ppkisJICBEKeys = NULL;
static  USHORT usJICBEKCnt = 0;
static  CHAR   szCurJICBEKey [SZKEY_LEN +1] = "";
static  CHAR   szCurJICBEID  [SMARRAY +1] = "";

static  PP_KEYSIDS ppkisTaskKeys = NULL;
static  USHORT usTaskCnt = 0;
/* -- Misnamed, is actually the JobCostSheetKey */
static  CHAR   szCurTaskKey [SZKEY_LEN +1] = ""; 
static  CHAR   szCurTaskID  [SMARRAY +1] = "";



/* -- Queries: */
static  CHAR  szBaseQuery    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szQueryJobInfo [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szQueryJobBrkDwnKeys [SIZE_OF_QUERY_JBDK + 1] = "";

static  CHAR  szQueryJobItemKeys   [SIZE_OF_QUERY_JIK + 1] = "";
static  CHAR  szConjJobItemKeys    [SMARRAY + 1] = "";

static  CHAR  szQueryJICBEKeys     [SIZE_OF_QUERY_JICBEK + 1] = "";
static  CHAR  szConjJICBEKeys      [SMARRAY + 1] = "";

static  CHAR  szQryJobBrkDwnItem   [SIZE_OF_QRY_JOB_BRKDWN_ITEM + 1] = "";
static  CHAR  szConjJobBrkDwnItemJBDK   [2*SMARRAY + 1] = "";
static  CHAR  szConjJobBrkDwnItemJIK    [2*SMARRAY + 1] = "";
static  CHAR  szConjJobBrkDwnItemJICBEK [2*SMARRAY + 1] = "";


static  CHAR  szQryJobTaskItem        [SIZE_OF_QRY_JOB_TASK_ITEM + 1] = "";
static  CHAR  szQryJobTaskItemTTL     [SIZE_OF_QRY_JOB_TASK_ITEM_TTL + 1] = "";
static  CHAR  szQryJobTaskItemQUP     [SIZE_OF_QRY_JOB_TASK_ITEM_TTL + 1] = "";
static  CHAR  szQryJobTaskItemFM      [SIZE_OF_QRY_JOB_TASK_ITEM_FW + 1] = "";
static  CHAR  szConjJobTaskItemJIK    [2*SMARRAY + 1] = "";
static  CHAR  szConjJobTaskItemJICBEK [2*SMARRAY + 1] = "";


static  CHAR  szQryJobTaskProd        [SIZE_OF_QRY_JOB_TASK_PROD + 1] = "";
static  CHAR  szConjJobTaskProdJIK    [2*SMARRAY + 1] = "";
static  CHAR  szConjJobTaskProdJICBEK [2*SMARRAY + 1] = "";
static  CHAR  szConjJobTaskProdJCSK   [2*SMARRAY + 1] = "";
static  CHAR  szConjJobTaskProdTID    [2*SMARRAY + 1] = "";


static  CHAR  szQryCstMEL  [SIZE_OF_QRY_CST_MEL + 1] = "";
static  CHAR  szQryCstTTL  [SIZE_OF_QRY_CST_TTL + 1] = "";
static  CHAR  szQryCstMarkUp    [SIZE_OF_QRY_CST_MARKUP + 1] = "";
static  CHAR  szQryCstUnitPrice [SIZE_OF_QRY_CST_UNITPRICE + 1] = "";

static  CHAR  szQueryMat    [SIZE_OF_QUERY_MAT + 1] = "";
static  CHAR  szConjMat     [SMARRAY + 1] = "";
static  CHAR  szQueryMatSum [SIZE_OF_QUERY_MAT_SUM + 1] = "";


static  CHAR  szQueryCrew    [SIZE_OF_QUERY_CREW + 1] = "";
static  CHAR  szQueryCrewKey  [SIZE_OF_QUERY_CREW + 1] = "";

static  CHAR  szQueryCrewEqp [SIZE_OF_QUERY_CREW_EQP + 1] = "";
static  CHAR  szConjCrewEqpJCSK [2*SMARRAY + 1] = "";
static  CHAR  szConjCrewEqpJCK  [2*SMARRAY + 1] = "";
static  CHAR  szQueryCrewEqpTtl [SIZE_OF_QUERY_CREW_EQP_TTL + 1] = "";
static  CHAR  szConjCrewEqpTtl  [2*SMARRAY + 1] = "";

static  CHAR  szQueryCrewLbr [SIZE_OF_QUERY_CREW_LBR + 1] = "";
static  CHAR  szConjCrewLbrJCSK [2*SMARRAY + 1] = "";
static  CHAR  szConjCrewLbrJCK  [2*SMARRAY + 1] = "";
static  CHAR  szQueryCrewLbrTtl [SIZE_OF_QUERY_CREW_LBR_TTL + 1] = "";
static  CHAR  szConjCrewLbrTtl  [2*SMARRAY + 1] = "";


static  CHAR  szQueryCrewEqpProd [SIZE_OF_QUERY_CREW_EQP_PROD + 1] = "";
static  CHAR  szQueryCrewLbrProd [SIZE_OF_QUERY_CREW_LBR_PROD + 1] = "";

static  CHAR  szQueryUnitDay [6*SMARRAY + 1] = "";


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprJBDK   = NULL;
static  HHEAP   hhprJIK    = NULL;
static  HHEAP   hhprJICBEK = NULL;
static  HHEAP   hhprITBD   = NULL;

static  HHEAP   hhprJECbe  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz    = NULL;
static  PSZ   * ppszQUP = NULL;

           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;

static  COLTTLS  acttl[RPT_TTL_NUM + 1];
                



USHORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
   {
   USHORT us = 1;
   CHAR szOpt[FOOTER_LEN + 1];
   PSZ  pszOpt = szOpt;

   if (preptitles == NULL)
      return (ITIRPT_INVALID_PARM);

   ItiStrCpy(preptitles->szTitle,   TITLE,    sizeof(preptitles->szTitle)   );
   ItiStrCpy(preptitles->szSubTitle,SUB_TITLE,sizeof(preptitles->szSubTitle));
   ItiStrCpy(preptitles->szFooter,  FOOTER,   sizeof(preptitles->szFooter)  );

   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle[0] = '\0';


   /* -- Check for use of footer override option. */
   ItiRptUtGetOption (ITIRPTUT_FOOTER_SWITCH,
                      preptitles->szFooter, sizeof(preptitles->szFooter),
                      argcnt, argvars);

   return 0;
   }/* END OF FUNCTION SetTitles */







USHORT  AlterHeader (USHORT usSelect, PSZ pszNewStr)
   {
   CHAR   szNewTitle[TITLE_LEN +1] = "";
   USHORT us = 0;

   ItiStrCpy(szNewTitle, TITLE_LEAD,  sizeof(szNewTitle) );
   ItiStrCat(szNewTitle, pszNewStr, sizeof(szNewTitle) );

   /* -- Remove trailing blanks. */
   us = (ItiStrLen(szNewTitle)) - 1;
   while ((us > 1) && (szNewTitle[us] == ' ') && (szNewTitle[us-1] == ' ') )
      {
      szNewTitle[us] = '\0';
      us--;
      }

   ItiStrCat(szNewTitle, TITLE_TAIL,  sizeof(szNewTitle) );
   us = ItiPrtChangeHeaderLn (usSelect, szNewTitle);

   return (us);
   }/* END OF FUNCTION AlterHeader */







USHORT SetColumnTitles (USHORT usTitle, USHORT usSection)
   /* -- Print the column titles. */
   {
   CHAR   szTmp[80] = "";
   USHORT us;
   USHORT usFirst;
   USHORT usLast;
   CHAR   szSubHeader[80] = "Cost Sheet ";



   switch (usTitle)
      {
      case ID_QRY_JOB_TASK_ITEM:
         ItiStrCpy (szTmp, acttl[RPT_TTL_6].szTitle, sizeof szTmp);
         ItiStrCat (szTmp, szCurJobItemID, sizeof szTmp);
         ItiStrCat (szTmp, acttl[RPT_TTL_7].szTitle, sizeof szTmp);
         ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE, szTmp);
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                          LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                          LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_9].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                          LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_11].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                          LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                          LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_13].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                          LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_14].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usTtlCol,
                          LEFT, (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_15].szTitle);
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usTtlCol,
                          LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_16].szTitle);
         break;


//      case :
//         break;

      default:
         /* Subsection title header line. */
         ItiStrCat(szSubHeader, szCurCostSheetID, sizeof szSubHeader);
         ItiStrCat(szSubHeader,
                   acttl[(SUBHEADER_INX + usSection)].szTitle, sizeof szSubHeader);
         ItiPrtPutTextAt(CURRENT_LN, 0, CENTER, USE_RT_EDGE, szSubHeader);
         ItiPrtIncCurrentLn();

         switch (usSection)
            {
            case BRKDWN:
               usLast = SUBHEADER_INX + usSection + BRKDWN_OFF;
               break;

            case MATERIAL:
               usLast = SUBHEADER_INX + usSection + MATERIAL_OFF;
               break;

            case CREW:
               usLast = SUBHEADER_INX + usSection + CREW_OFF;
               break;

            default:
               return 13;
               break;
            }

         usFirst = SUBHEADER_INX + usSection + 1;

         for (us=usFirst; us<=usLast; us++)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                             LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                             acttl[us].szTitle);

            if((us == RPT_TTL_39) || /* Extension        */
               (us == RPT_TTL_43) || /* Crew Description */
               (us == RPT_TTL_47))   /* Daily work       */
               {
               ItiPrtIncCurrentLn (); //line feed
               }

            }/* end for loop */


         break;
      }/*  end switch usTitle */







   ItiPrtIncCurrentLn (); //line feed
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   us = ItiPrtIncCurrentLn (); //line feed

   return us;
   }/* END OF FUNCTION SetColumnTitles */






USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case 'x':
      case 'X':
      default:
      ItiStrCat(pszOrderingStr, " order by 1 ", sizeof szOrderingStr);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */





USHORT MakeList(HMODULE hmodML,            HHEAP hhpML,
                        PSZ  pszQry,       USHORT usQryID,
                        BOOL bQuotedKey,   BOOL bQuotedID,
                        PUSHORT pusKeyCnt, PPP_KEYSIDS pppkisML)
/* -- This function builds a Key&ID list from the given query.  */
/* -- Assumes query column 0 is the KEY and column 1 is the ID. */
   {
   USHORT  us, usLen, usRes;
   HQRY    hqryML   = NULL;
   USHORT  usColsML = 0;
   USHORT  usErrML  = 0;
   PSZ   * ppszML   = NULL;
   PSZ     pszIDCol, pszKeyCol;
   CHAR    szTmp[2 * SMARRAY] = "";


   if ((pszQry == NULL) || (pppkisML == NULL) || (pusKeyCnt == NULL))
      return ITIRPTUT_PARM_ERROR;

   /* -- Find out how many keys (rows) we have. */
   *pusKeyCnt = ItiDbGetQueryCount(pszQry, &usColsML, &usErrML);
   usRes = ItiRptUtEstablishKeyIDList (hhpML, (*pusKeyCnt + 1),
                                       sizeof(P_KEYID), pppkisML);

   /* -- Do the database query. */
   hqryML = ItiDbExecQuery (pszQry, hhpML, hmodML, ITIRID_RPT,
                            usQryID, &usColsML, &usErrML);
   if (hqryML == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, " MakeList query failed.");
      return ITIRPTUT_ERROR;
      }
   else
      {/* -- Build a list of category keys. */
      us = 0;
      while( ItiDbGetQueryRow (hqryML, &ppszML, &usErrML) )
         {
         if ((ppszML == NULL) || (*ppszML == NULL))
            break;

         pszKeyCol = (ppszML[0] != NULL) ? ppszML[0] : " 0 ";
         pszIDCol  = (ppszML[1] != NULL) ? ppszML[1] : " N/A ";
         ItiRptUtAddToKeyIDList(hhpML, pszIDCol, pszKeyCol,
                                us, *pppkisML, *pusKeyCnt);
         if (bQuotedID)
            {
            ItiStrCpy(szTmp, " '", sizeof szTmp);
            ItiStrCat(szTmp, (*pppkisML)[us]->pszID, sizeof szTmp);
            ItiStrCat(szTmp, "' ", sizeof szTmp);
            usLen = ItiStrLen(szTmp) +1;
            ItiStrCpy((*pppkisML)[us]->pszID, szTmp, usLen);
            }

         if (bQuotedKey)
            {
            ItiStrCpy(szTmp, " '", sizeof szTmp);
            ItiStrCat(szTmp, (*pppkisML)[us]->szKey, sizeof szTmp);
            ItiStrCat(szTmp, "' ", sizeof szTmp);
            usLen = ItiStrLen(szTmp) +1;
            ItiStrCpy((*pppkisML)[us]->szKey, szTmp, usLen);
            }

         us++;   /* the variable us keeps count of the query row number */

         ItiFreeStrArray (hhpML, ppszML, usColsML);
         }
      }/* end if (hqry... else clause */


   return usRes;

   }/* END OF FUNCTION MakeList */








USHORT JobItemBrkDwnSection (void)
   {
   USHORT usLn;
   PSZ  * ppsz = NULL;
   CHAR   szBrkDwnID [SMARRAY + 1] = "";


   if (szCurJobKey[0] == '\0')
      return 13;


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryJobInfo, " ", szCurJobKey);

   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QUERY_JOB_INFO, &usNumCols);
   if (ppsz != NULL)
      {
      if (usCurrentPg != 0)
         ItiPrtNewPg();
      usCurrentPg = ItiPrtQueryCurrentPgNum();

      /* -- Insert Job ID into header title. */
      AlterHeader (ITIPRT_TITLE, ppsz[JOB_ID]);
      ItiStrCpy(szCurrentJobID, ppsz[JOB_ID], sizeof szCurrentJobID);
      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   




   /* -- Build the section query. */
   ItiStrCpy(szQuery, szQryJobBrkDwnItem,        sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey,               sizeof szQuery);
   ItiStrCat(szQuery, szConjJobBrkDwnItemJBDK,   sizeof szQuery);
   ItiStrCat(szQuery, szCurJobBrkDwnKey,         sizeof szQuery);
   ItiStrCat(szQuery, szConjJobBrkDwnItemJIK,    sizeof szQuery);
   ItiStrCat(szQuery, szCurJobItemKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobBrkDwnItemJICBEK, sizeof szQuery);
   ItiStrCat(szQuery, szCurJICBEKey, sizeof szQuery);

   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_JOB_BRKDWN_ITEM, &usNumCols);

   if (ppsz != NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiStrCpy(szBrkDwnID, ppsz[JOB_BDK_ID], sizeof szBrkDwnID);
      ItiStrCat(szBrkDwnID, ", ", sizeof szBrkDwnID);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       szBrkDwnID);
      /* Item Num */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[JOB_BDK_SIN]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[JOB_BDK_DESC]);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[JOB_BDK_CBE_ID]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1,
                       (LEFT|WRAP),
                       (2 | FROM_RT_EDGE),
                       ppsz[JOB_BDK_CBE_DESC]);
      ItiPrtIncCurrentLn();

      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      usLn = ItiPrtIncCurrentLn (); //line feed

      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   else
      return 13;

   return 0;
   }/* End of Function JobItemBrkDwnSection */





USHORT ItemTaskBrkDwnSection (void)
/* ===== The Item 9999... Task Breakdown Section */
   {
   USHORT usLn, us;
   BOOL   bNewID = TRUE;
   BOOL   bLabeled   = FALSE;
   CHAR   szTmp[2 * SMARRAY] = "/ ";



   /* -- Build the query. */
   ItiStrCpy(szQuery, szQryJobTaskItem,        sizeof szQuery);
   ItiStrCat(szQuery, szQryJobTaskItemFM,      sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey,             sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskItemJIK,    sizeof szQuery);
   ItiStrCat(szQuery, szCurJobItemKey,         sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskItemJICBEK, sizeof szQuery);
   ItiStrCat(szQuery, szCurJICBEKey,           sizeof szQuery);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                          ID_QRY_JOB_TASK_ITEM, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         usLn = 6+ItiStrLen(ppsz[JOB_TASK_DESC])/acttl[RPT_TTL_16].usDtaColWth;
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

            SetColumnTitles (ID_QRY_JOB_TASK_ITEM, 0);

            bLabeled = TRUE;
            }


         if (!bLabeled)
            {
            SetColumnTitles (ID_QRY_JOB_TASK_ITEM, 0);
            bLabeled = TRUE;
            }



         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                         LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_ID]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_QTY]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                         LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_10].szTitle);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_UNIT_PRICE]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                         LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_13].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_UNIT_CODE_ID]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                         LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_14].szTitle);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_EXT]);
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                          (LEFT|WRAP),
                          (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                          ppsz[JOB_TASK_DESC]);

         ItiFreeStrArray(hhprJECbe, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */





      /* -- Writeout the total subsection for the item's task list. */
      /* -- Build the total query. */
      ItiStrCpy(szQuery, szQryJobTaskItemTTL,     sizeof szQuery);
      ItiStrCat(szQuery, szQryJobTaskItemFM,      sizeof szQuery);
      ItiStrCat(szQuery, szCurJobKey,             sizeof szQuery);
      ItiStrCat(szQuery, szConjJobTaskItemJIK,    sizeof szQuery);
      ItiStrCat(szQuery, szCurJobItemKey,         sizeof szQuery);
      ItiStrCat(szQuery, szConjJobTaskItemJICBEK, sizeof szQuery);
      ItiStrCat(szQuery, szCurJICBEKey,           sizeof szQuery);

      ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                           ID_QRY_JOB_TASK_ITEM_TTL, &usNumCols);
      if ((ppsz != NULL) && (bLabeled))
         {
         ItiPrtDrawSeparatorLn (CURRENT_LN,
                                acttl[RPT_TTL_15].usDtaCol, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed


         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                         LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_17].szTitle);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                         LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_18].szTitle);
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                         ppsz[JOB_TASK_TTL_EXT]);
         ItiPrtIncCurrentLn (); //line feed


         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                         LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_19].szTitle);


         ItiStrCpy(szQuery, szQryJobTaskItemQUP,     sizeof szQuery);
         ItiStrCat(szQuery, szQryJobTaskItemFM,      sizeof szQuery);
         ItiStrCat(szQuery, szCurJobKey,             sizeof szQuery);
         ItiStrCat(szQuery, szConjJobTaskItemJIK,    sizeof szQuery);
         ItiStrCat(szQuery, szCurJobItemKey,         sizeof szQuery);
         ItiStrCat(szQuery, szConjJobTaskItemJICBEK, sizeof szQuery);
         ItiStrCat(szQuery, szCurJICBEKey,           sizeof szQuery);

         ppszQUP = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                              ID_QRY_JOB_TASK_ITEM_QUP, &usNumCols);
         if (ppszQUP != NULL)
            {                     
            ItiStrCpy(szTmp, "/ ", sizeof szTmp);
            ItiStrCat(szTmp, ppszQUP[JOB_TASK_QUP_QTY], sizeof szTmp);
            ItiStrCat(szTmp, " =", sizeof szTmp);
            us = (acttl[RPT_TTL_14].usTtlCol) - ItiStrLen(szTmp);
            ItiPrtPutTextAt(CURRENT_LN, us,
                            RIGHT, (acttl[RPT_TTL_14].usTtlCol + 1),
                            szTmp);

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                            ppszQUP[JOB_TASK_QUP_UNIT_PRICE]);

            if (ppszQUP[0])      /* Recover memory. */
               ItiMemFree (hhprJECbe, ppszQUP[0]);
            if (ppszQUP[1])      /* Recover memory. */
               ItiMemFree (hhprJECbe, ppszQUP[1]);
            }

         ItiPrtIncCurrentLn (); //line feed

         if (ppsz[0])      /* Recover memory. */
            ItiMemFree (hhprJECbe, ppsz[0]);

         /* -- Finishout this section. */
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         usLn = ItiPrtIncCurrentLn (); //line feed
         }
//      else
//         {
//         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
//                         LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
//                         acttl[RPT_TTL_17].szTitle);
//         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
//                         LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
//                         acttl[RPT_TTL_18].szTitle);
//         ItiPrtIncCurrentLn (); //line feed
//
//         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
//                         LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
//                         acttl[RPT_TTL_19].szTitle);
//         ItiPrtIncCurrentLn (); //line feed
//         }

      }/* else clause */
   


   return usLn;
   }/* End of Function ItemTaskBrkDwnSection */






USHORT TaskProductivitySection (void)
   {
   USHORT usLn;


   /* With the current TaskID & JobCostSheetKey ofthe ppkisTaskKeys list... */
   ItiStrCpy(szQuery, szQryJobTaskProd,   sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey,        sizeof szQuery);

   ItiStrCat(szQuery, szConjJobTaskProdJIK, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobItemKey,      sizeof szQuery);

   ItiStrCat(szQuery, szConjJobTaskProdJICBEK, sizeof szQuery);
   ItiStrCat(szQuery, szCurJICBEKey,           sizeof szQuery);

   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

   ItiStrCat(szQuery, szConjJobTaskProdTID, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskID,          sizeof szQuery);



   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_JOB_TASK_PROD, &usNumCols);
   if (ppsz != NULL)
      {
      /* -- Check for new page. */
      usLn = 7+ItiStrLen(ppsz[JOB_TASK_PROD_DESC])/acttl[RPT_TTL_20].usDtaColWth;
      if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                      LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_20].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                      LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_TASKID]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_21].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_CSTSHT_ID]);
      /* -- Set the new cost sheet ID for use by section titles. */
      ItiStrCpy(szCurCostSheetID, ppsz[JOB_TASK_PROD_CSTSHT_ID], sizeof szCurCostSheetID);
      ItiPrtIncCurrentLn (); //line feed

      /* Task description: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                       (LEFT|WRAP),
                       (5 | FROM_RT_EDGE),
                       ppsz[JOB_TASK_PROD_DESC]);
      ItiPrtIncCurrentLn (); //blank line

      /* Productivity: */
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_22].usTtlCol,
                      LEFT, (acttl[RPT_TTL_22].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_22].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_RATE]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_23].szTitle);
      ItiPrtIncCurrentLn (); //line feed


      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                      LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_24].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_24].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_HRS_PER_DAY]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_25].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_25].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_OT_HRS]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_26].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_27].usTtlCol,
                      LEFT, (acttl[RPT_TTL_27].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_27].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_27].usDtaColWth | REL_LEFT),
                      ppsz[JOB_TASK_PROD_GRAN]);
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (2 | REL_LEFT),
                      "s.");
      ItiPrtIncCurrentLn (); //line feed

      ItiFreeStrArray(hhprJECbe, ppsz, usNumCols);
      }
   else
      {
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                      LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_20].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                      LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                      szCurTaskID);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                      " data not found or is incomplete. ");
      ItiPrtIncCurrentLn (); //line feed
      }

   /* -- Finishout this section. */
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   usLn = ItiPrtIncCurrentLn (); //line feed

   return usLn;
   }/* End of Function TaskProductivitySection */






USHORT BrkDwnSection (void)
   {
   USHORT us, usStart, usStop, usStartCol, usStopCol;
   CHAR   szTmp [2 * SMARRAY] = "";



   if (ItiPrtKeepNLns(12) == PG_WAS_INC)
      {
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
      }
   SetColumnTitles(0, BRKDWN);


   ItiStrCpy(szQuery, szQryCstMEL, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);


   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_CST_MEL, &usNumCols);
   if (ppsz != NULL)
      {
      /* -- Materials line: */
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_58].usTtlCol,
                      LEFT, (acttl[RPT_TTL_58].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_58].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_MAT_UNIT_COST]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                      LEFT, (2 | REL_LEFT),
                      "+ ");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_MAT_OVRHD_AMT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (1 | REL_LEFT),
                      "(");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_MAT_OVRHD_PCT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (3 | REL_LEFT),
                      ") =");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_MAT_EXT]);
//      ItiStrCpy(szMaterialTtl, ppsz[MAT_EXT_RAW], sizeof szMaterialTtl);
      ItiPrtIncCurrentLn(); //line feed


      /* -- Equipment line: */
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_59].usTtlCol,
                      LEFT, (acttl[RPT_TTL_59].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_59].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_EQU_UNIT_COST]);     /* @ */

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                      LEFT, (2 | REL_LEFT),
                      "+ ");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_EQU_OVRHD_AMT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (1 | REL_LEFT),
                      "(");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_EQU_OVRHD_PCT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (3 | REL_LEFT),
                      ") =");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_EQU_EXT]);
//      ItiStrCpy(szMaterialTtl, ppsz[EQU_EXT_RAW], sizeof szMaterialTtl);
      ItiPrtIncCurrentLn(); //line feed


      /* -- Labor line: */
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_60].usTtlCol,
                      LEFT, (acttl[RPT_TTL_60].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_60].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_LBR_UNIT_COST]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                      LEFT, (2 | REL_LEFT),
                      "+ ");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_LBR_OVRHD_AMT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (1 | REL_LEFT),
                      "(");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_LBR_OVRHD_PCT]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (3 | REL_LEFT),
                      ") =");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                       ppsz[CST_MEL_LBR_EXT]);
//      ItiStrCpy(szMaterialTtl, ppsz[_LBR_EXT_RAW], sizeof szMaterialTtl);
      ItiPrtIncCurrentLn(); //line feed


      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's query.");
      SETQERRMSG;
      }



   /* === Material Equipment Labor Total: */
   ItiStrCpy(szQuery, szQryCstTTL, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_CST_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      us = ItiStrLen(ppsz[0]);
      usStart = (acttl[RPT_TTL_52].usDtaCol 
                 + acttl[RPT_TTL_52].usDtaColWth)
                 - us - 1;
      usStop = us + usStart;

      usStartCol = usStart;
      usStopCol = usStop;

      ItiPrtDrawSeparatorLn (CURRENT_LN, usStartCol, usStopCol);
      ItiPrtIncCurrentLn (); //line feed

      usStart = (usStart - acttl[RPT_TTL_52].usTtlColWth);

      ItiPrtPutTextAt(CURRENT_LN, usStart,
                      LEFT, (acttl[RPT_TTL_52].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_52].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_52].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_52].usDtaColWth | REL_LEFT),
                       ppsz[0]);
      ItiPrtIncCurrentLn(); //blank line

      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's MEL total query.");
      SETQERRMSG;
      }
      


   /* === Mark-Up line: */
   ItiStrCpy(szQuery, szQryCstMarkUp, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_CST_MARKUP, &usNumCols);
   if (ppsz != NULL)
      {
      ItiStrCpy(szTmp, "+ ", sizeof szTmp);
      ItiStrCat(szTmp, ppsz[CST_MARKUP_PCT], sizeof szTmp);
      ItiStrCat(szTmp, acttl[RPT_TTL_53].szTitle, sizeof szTmp);

      us = ItiStrLen(szTmp);
      ItiPrtPutTextAt(CURRENT_LN, (usStartCol - us), LEFT,
                      (usStartCol),
                      szTmp);

      ItiPrtPutTextAt (CURRENT_LN,
                       acttl[RPT_TTL_53].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_53].usDtaColWth | REL_LEFT),
                       ppsz[CST_MARKUP_TTL]);
      ItiPrtIncCurrentLn(); //blank line

      ItiPrtDrawSeparatorLn (CURRENT_LN, usStartCol, usStopCol);
      ItiPrtIncCurrentLn (); //line feed

      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's Markup total query.");
      SETQERRMSG;
      }


   
   /* === Unit Price line: */
   ItiStrCpy(szQuery, szQryCstUnitPrice, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

   ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                        ID_QRY_CST_UNITPRICE, &usNumCols);
   if (ppsz != NULL)
      {

      ItiPrtPutTextAt(CURRENT_LN, (usStartCol - acttl[RPT_TTL_54].usTtlColWth),
                      LEFT, (acttl[RPT_TTL_54].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_54].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_54].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_54].usDtaColWth | REL_LEFT),
                       ppsz[0]);
      ItiPrtIncCurrentLn(); 

      ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's UnitPrice query.");
      SETQERRMSG;
      }


   /* -- Finishout this section. */
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   us = ItiPrtIncCurrentLn (); //line feed

   return us;
   }/* End of Function BrkDwnSection */






USHORT MaterialSection (void)
   {
   USHORT usLength, us = 0;
   USHORT usLn, usStart, usStop;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled;
   CHAR   szTmp[SMARRAY] = "";
   CHAR   szQty[30] = "";



   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryMat, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 ",  sizeof szQuery);



   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                          ID_QUERY_MAT, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         szTmp[0] = '\0';

         usLn = ItiStrLen(ppsz[MAT_DESC]) + 1;
         usLn = 8 + (usLn / acttl[RPT_TTL_40].usDtaColWth);
         if (!bLabeled)
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               }
            SetColumnTitles(0, MATERIAL);
            bLabeled = TRUE;
            }
         else
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               SetColumnTitles(0, MATERIAL);
               bLabeled = TRUE;
               }
            }/* end if(!bLab... else clause */


         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol,
                          LEFT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                          ppsz[MAT_ID]);

         /* BSR 950090 start \/ */
         ItiStrCpy(szQty, ppsz[QUANTITY], sizeof szQty);
         if (ItiStrLen(szQty) > 10)
            { /* reformat without any decimals. */
            ItiFmtFormatString (ppsz[QUANTITY], &szQty[0], sizeof szQty,
                                "Number,,", &usLength);
            }
         else
            {
             ; /* NOP */
            }

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                          szQty);
                          // ppsz[QUANTITY]);
         /* BSR 950090   end /\ */

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                          LEFT, (2 | REL_LEFT),
                          "@ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                          ppsz[UNIT_PRICE]);


         /* -- "/ Unit = " */
         ItiStrCpy(szTmp, "/ ", sizeof szTmp);
         ItiStrCat(szTmp, ppsz[MAT_UNIT], sizeof szTmp);
         ItiStrCat(szTmp, " =", sizeof szTmp);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_38].usTtlCol,
                          RIGHT, acttl[RPT_TTL_39].usDtaCol - 1,
                          szTmp);


         /* -- Quantity * Price = Extension */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                          ppsz[EXT]);
         ItiPrtIncCurrentLn(); //blank line




         /* -- Description */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                          (LEFT|WRAP),
                          (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                          ppsz[MAT_DESC]);
//         ItiPrtIncCurrentLn(); //blank line


         ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      /* -- Finishout this section, iff we had any data. */
      if (bLabeled)
         {
         /* -- Do the materials totals: */
         ItiStrCpy(szQuery, szQueryMatSum, sizeof szQuery);
         ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
         ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
         ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);


         ppsz = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                              ID_QUERY_MAT_SUM, &usNumCols);

         if (ppsz != NULL)
            {
            usStart = (acttl[RPT_TTL_39].usDtaCol 
                       + acttl[RPT_TTL_39].usDtaColWth)
                       - ItiStrLen(ppsz[MAT_SUM_FMT]) - 1;
            usStop = ItiStrLen(ppsz[MAT_SUM_FMT]) + usStart;

            ItiPrtDrawSeparatorLn (CURRENT_LN,
                                   usStart, usStop);
            ItiPrtIncCurrentLn (); //line feed


            ItiPrtPutTextAt(CURRENT_LN,
                            (usStart - (acttl[RPT_TTL_51].usTtlColWth + 1)),
                            LEFT, (acttl[RPT_TTL_51].usTtlColWth | REL_LEFT),
                            acttl[RPT_TTL_51].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                             ppsz[MAT_SUM_FMT]);
            ItiPrtIncCurrentLn(); //blank line

            ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);
            }/* end if (ppsz... then clause */

         /* -- Finishout this section. */
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         us = ItiPrtIncCurrentLn (); //line feed

         }/* end if (bLabeled... then clause */

      }/* else clause */

   return us;
   }/* End of Function MaterialSection */








USHORT CrewSection (void)
   {
   USHORT usLn = 0;
   USHORT us;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled = FALSE;
   CHAR   szJobCrewKey [SMARRAY] = "";
   CHAR   szLbrTtlRaw  [SMARRAY] = "";
   CHAR   szEqpTtlRaw  [SMARRAY] = "";
   CHAR   szSubQryJobCrewKeys [LGARRAY] = "";
   CHAR   szTmp [SMARRAY + 1] = "";
   CHAR   szProdRate [SMARRAY + 1] = "";
   PSZ    pszUnitCost = NULL;
   CHAR    szUnitCost [SMARRAY + 1] = "";

           /* Used by DbGetQueryRow function: */
   HQRY    hqryLoc   = NULL;
   USHORT  usResIdLoc   = ITIRID_RPT;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc  = 0;
   PSZ   * ppszLoc   = NULL;





   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryCrew, sizeof szQuery);
   ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
   ItiStrCat(szQuery, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

   /* -- Use the current keys in the Eqp & Lbr totals subquery. */
   ItiStrCpy(szSubQryJobCrewKeys, " (", sizeof szSubQryJobCrewKeys);
   ItiStrCat(szSubQryJobCrewKeys, szQueryCrewKey, sizeof szQuery);
   ItiStrCat(szSubQryJobCrewKeys, szCurJobKey, sizeof szQuery);
   ItiStrCat(szSubQryJobCrewKeys, szConjJobTaskProdJCSK, sizeof szQuery);
   ItiStrCat(szSubQryJobCrewKeys, szCurTaskKey,          sizeof szQuery);
   ItiStrCat(szSubQryJobCrewKeys, ") ", sizeof szSubQryJobCrewKeys);

   hqry = ItiDbExecQuery (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                          ID_QUERY_CREW, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         usLn = ItiStrLen(ppsz[CREW_DESC]) + 1;
         usLn = 12 + (usLn / acttl[RPT_TTL_43].usDtaColWth);
         if (!bLabeled)
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               }
            SetColumnTitles(0, CREW);
            bLabeled = TRUE;
            }
         else
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               SetColumnTitles(0, CREW);
               bLabeled = TRUE;
               }
            }/* end if(!bLab... else clause */


         /* -- Now, with each CrewKey... */ 
         /* -- ID and description text. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_42].usDtaCol,
                       LEFT, (acttl[RPT_TTL_42].usDtaColWth | REL_LEFT),
                       ppsz[CREW_ID]);

         /* -- Crew description */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_43].usDtaCol,
                       (LEFT|WRAP),
                       (acttl[RPT_TTL_43].usDtaColWth | REL_LEFT),
                       ppsz[CREW_DESC]);
         //ItiPrtIncCurrentLn (); //line feed

         ItiStrCpy(szJobCrewKey, ppsz[CREW_KEY], sizeof szJobCrewKey);



         /* === The crew's equipment: */
         ItiStrCpy(szQuery, szQueryCrewEqp, sizeof szQuery);
         ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewEqpJCSK, sizeof szQuery);
         ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewEqpJCK, sizeof szQuery);
         ItiStrCat(szQuery, szJobCrewKey, sizeof szQuery);


         hqryLoc = ItiDbExecQuery (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                                   ID_QUERY_CREW_EQP, &usNumColsLoc, &usErrLoc);
       
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG;
            }
         else
            {
            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               usLn = ItiStrLen(ppszLoc[EQP_DESC]) + 2;
               usLn = 10 + (usLn / acttl[RPT_TTL_48].usDtaColWth);
               if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
                  {
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
                  SetColumnTitles(0, CREW);
                  }


               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_44].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_44].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_QUAN]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_45].usDtaCol,
                            LEFT, (acttl[RPT_TTL_45].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_ID]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " hr/day =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_49].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed


               /* -- Overtime line */
               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_OT_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_OT_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " ovr-tm =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_49].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_OT_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed



               /* -- Equipment description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_48].usDtaColWth | REL_LEFT),
                             ppszLoc[EQP_DESC]);

               ItiFreeStrArray (hhprJECbe, ppszLoc, usNumColsLoc);
               }
            }/* end of if (hqryLoc... else clause */



            /* -- Now the crew's labor: */
         ItiStrCpy(szQuery, szQueryCrewLbr, sizeof szQuery);
         ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewLbrJCSK, sizeof szQuery);
         ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewLbrJCK, sizeof szQuery);
         ItiStrCat(szQuery, szJobCrewKey, sizeof szQuery);

         hqryLoc = ItiDbExecQuery (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                                ID_QUERY_CREW_LBR, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG;
            }
         else
            {
            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               usLn = ItiStrLen(ppszLoc[LBR_DESC]) + 2;
               usLn = 10 + (usLn / acttl[RPT_TTL_48].usDtaColWth);
               if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
                  {
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
                  SetColumnTitles(0, CREW);
                  }


               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_44].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_44].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_QUAN]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_45].usDtaCol,
                            LEFT, (acttl[RPT_TTL_45].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_ID]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " hr/day =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed


               /* -- Overtime line */
               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_OT_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_OT_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " ovr-tm =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_OT_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed


               /* -- Labor description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_48].usDtaColWth | REL_LEFT),
                             ppszLoc[LBR_DESC]);

               ItiFreeStrArray (hhprJECbe, ppszLoc, usNumColsLoc);
               }

            ItiPrtIncCurrentLn (); //blank line

            }/* end of if (hqryLoc... else clause */



         /* recover memory */
         ItiFreeStrArray (hhprJECbe, ppsz, usNumCols);

         }/* End of WHILE GetQueryRow */


      /* == Quit function if no data was found. */
      if (!bLabeled)
         return 0;


      /* === Almost finally, the totals of equipment and labor: */
      if (ItiPrtKeepNLns(6) == PG_WAS_INC)
         {
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         SetColumnTitles(0, CREW);
         }

      ItiPrtDrawSeparatorLn(CURRENT_LN,
                            acttl[RPT_TTL_49].usDtaCol+1,
                            acttl[RPT_TTL_49].usDtaCol+acttl[RPT_TTL_49].usDtaColWth+1);
      ItiPrtDrawSeparatorLn(CURRENT_LN,
                            acttl[RPT_TTL_50].usDtaCol+1,
                            acttl[RPT_TTL_50].usDtaCol+acttl[RPT_TTL_50].usDtaColWth+1);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol - 6,
                      LEFT, (8 | REL_LEFT), "Total: ");

      /* -- Equipment sub total. */
      ItiStrCpy(szQuery, szQueryCrewEqpTtl, sizeof szQuery);
      ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewEqpJCSK, sizeof szQuery);
      ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

      ItiStrCat(szQuery, szConjCrewEqpTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryJobCrewKeys, sizeof szQuery);


      ppszLoc = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                              ID_QUERY_CREW_EQP_TTL, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_49].usDtaColWth | REL_LEFT),
                         ppszLoc[EQP_TTL]);
         ItiStrCpy(szEqpTtlRaw, ppszLoc[EQP_TTL_RAW], sizeof szEqpTtlRaw);

         ItiFreeStrArray (hhprJECbe, ppszLoc, usNumColsLoc);
         }


      /* -- Labor sub total. */
      ItiStrCpy(szQuery, szQueryCrewLbrTtl, sizeof szQuery);
      ItiStrCat(szQuery, szCurJobKey, sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewLbrJCSK, sizeof szQuery);
      ItiStrCat(szQuery, szCurTaskKey,          sizeof szQuery);

      ItiStrCat(szQuery, szConjCrewLbrTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryJobCrewKeys, sizeof szQuery);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                              ID_QUERY_CREW_LBR_TTL, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
                         ppszLoc[LBR_TTL]);
         ItiStrCpy(szLbrTtlRaw, ppszLoc[LBR_TTL_RAW], sizeof szLbrTtlRaw);

         ItiFreeStrArray (hhprJECbe, ppszLoc, usNumColsLoc);
         }

      ItiPrtIncCurrentLn (); //line feed



      /* unit/day =  sub total. */
      sprintf (szQuery, szQueryUnitDay, 
               szCurJobKey, szCurJobCstShtKey);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprJECbe, hmodrJECbe, ITIRID_RPT,
                              ID_QUERY_UNIT_DAY, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiStrCpy(szTmp, "/ ", sizeof szTmp);
         ItiStrCat(szTmp, ppszLoc[0], sizeof szTmp);

         ItiFmtFormatString (ppszLoc[0], szProdRate, sizeof szProdRate,
                             "Number,....", &us);

         ItiStrCat(szTmp, " ", sizeof szTmp);
         ItiStrCat(szTmp, ppszLoc[1], sizeof szTmp);
         ItiStrCat(szTmp, "/day = ", sizeof szTmp);

         us = ItiStrLen(szTmp);
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol - us,
                         LEFT, (us | REL_LEFT), szTmp);

         ItiFreeStrArray (hhprJECbe, ppszLoc, usNumColsLoc);
         }



      sprintf (szQuery, "SELECT %s / %s ", szEqpTtlRaw, szProdRate);

      pszUnitCost = ItiDbGetRow1Col1 (szQuery, hhprJECbe, 0, 0, 0);
      if (pszUnitCost != NULL)
         {
         ItiFmtFormatString (pszUnitCost, szUnitCost, sizeof szUnitCost,
                             "Number,$,....", &us);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_49].usDtaColWth+2) | REL_LEFT),
                         szUnitCost);

         ItiPrtDrawSeparatorLn(CURRENT_LN + 1,
                               acttl[RPT_TTL_49].usDtaCol+1,
                               acttl[RPT_TTL_49].usDtaCol+acttl[RPT_TTL_49].usDtaColWth+1);

         ItiPrtPutTextAt(CURRENT_LN + 2,
                         acttl[RPT_TTL_49].usDtaCol - acttl[RPT_TTL_56].usTtlColWth ,
                         LEFT, (acttl[RPT_TTL_56].usTtlColWth  | REL_LEFT),
                         acttl[RPT_TTL_56].szTitle );

//         ItiFmtFormatString (pszUnitCost, szUnitCost, sizeof szUnitCost,
//                             "Number,$,....", &us);

         ItiPrtPutTextAt(CURRENT_LN + 2, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_49].usDtaColWth+2) | REL_LEFT),
                         szUnitCost);

         if (pszUnitCost)      /* Recover memory. */
            ItiMemFree (hhprJECbe, pszUnitCost);
         }




      /* -- Now do the labor column. */
      sprintf (szQuery, "SELECT %s / %s ", szLbrTtlRaw, szProdRate);

      pszUnitCost = ItiDbGetRow1Col1 (szQuery, hhprJECbe, 0, 0, 0);
      if (pszUnitCost != NULL)
         {
         ItiFmtFormatString (pszUnitCost, szUnitCost, sizeof szUnitCost,
                             "Number,$,....", &us);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_50].usDtaColWth+2) | REL_LEFT),
                         szUnitCost);

         ItiPrtDrawSeparatorLn(CURRENT_LN + 1,
                               acttl[RPT_TTL_50].usDtaCol+1,
                               acttl[RPT_TTL_50].usDtaCol+acttl[RPT_TTL_50].usDtaColWth+1);

         ItiPrtPutTextAt(CURRENT_LN + 2, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_50].usDtaColWth+2) | REL_LEFT),
                         szUnitCost);

         if (pszUnitCost)      /* Recover memory. */
            ItiMemFree (hhprJECbe, pszUnitCost);
         }


      ItiPrtIncCurrentLn (); //line feed  current_ln
      ItiPrtIncCurrentLn (); //line feed  current_ln + 1
      ItiPrtIncCurrentLn (); //line feed  current_ln + 2

      }/* end of if (hqry... else clause  ID_QUERY_CREW */



      /* -- Finishout this section. */
   if (bLabeled)
      {
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      usLn = ItiPrtIncCurrentLn (); //line feed
      }

   return usLn;
   }/* End of Function CrewSectionFor */












USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szCmdLn  [2*SMARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY] = "";



   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   /* -- Get the job key. */
   WinQueryDlgItemText (hwnd, DID_KEY, sizeof szKeyStr, szKeyStr);

   if (ItiStrLen(szKeyStr) == 0)
      {
      ItiStrCpy (szErrorMsg,
                 "ERROR: No KEY returned from ",
                 sizeof szErrorMsg);

      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, " dialog box query.", sizeof szErrorMsg);

      ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
      return (ITIRPTUT_ERROR);
      }
   else
      {
      ItiStrCat (szCmdLn, szKeyStr, sizeof szCmdLn);
      }

   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_JECBE_SESSION);

   return (us);

   }/* End of PrintProc Function */










/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   DBGMSG(" *** In DLL version check function.");

   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);

   return (FALSE);
   }








VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,   
                                 USHORT  usMaxSize,   
                                 PUSHORT pusWindowID) 
   {                                                  
   ItiStrCpy (pszBuffer, "Job Estimate: Cost Based", usMaxSize);   
   *pusWindowID = rJECbe_RptDlgBox;                       
   }/* END OF FUNCTION */


BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rJECbe_RptDlgBox)                     
       && (usActiveWindowID == JobS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrJECbeRptDlgProc (HWND     hwnd,     
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY+1] = "";

   szKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_JECBE_SESSION, 0, NULL);
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_PRINT:
               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

               us = PrintProc(hwnd, mp1, mp2);

               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));

               if (us == 0)
                  {
                  WinDismissDlg (hwnd, TRUE);
                  return 0;
                  }
               break;


            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            }/* end of switch(SHORT1... */

         break;

      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDll...RptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszCurJobKey = szCurJobKey;
   USHORT  us, usJBDC, usJIC, usJICBEK, usT, usRes;
   BOOL    bActiveOnly = FALSE;
   CHAR    szStr[8] = "";


   /* ------------------------------------------------------------------- *\
    *  Create a memory heaps.                                             *
   \* ------------------------------------------------------------------- */
   hhprJECbe  = ItiMemCreateHeap (MIN_HEAP_SIZE);
   hhprJBDK   = ItiMemCreateHeap (MIN_HEAP_SIZE);
   hhprJIK    = ItiMemCreateHeap (MIN_HEAP_SIZE);
   hhprJICBEK = ItiMemCreateHeap (MIN_HEAP_SIZE);
   hhprITBD   = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentJobID, "JOB ID", sizeof(szCurrentJobID));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   /* -- Check if we are to use only ACTIVE cost sheets. */
   ItiRptUtGetOption(ACTIVEONLY_SWITCH, szStr, sizeof szStr, argcnt, argvars);
   if (szStr[0] != '\0')
      bActiveOnly = TRUE;
   else
      bActiveOnly = FALSE;


//   ItiRptUtGetOption (OPT_SW_INCL_XYZ_RPT, szSuppRpt, sizeof szSuppRpt,
//                      argcnt, argvars);
//   if (szSuppRpt[0] == '\0') 
//      bPrtRpt = FALSE;
//   else
//      bPrtRpt = TRUE;



   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrJECbe, &usNumOfTitles, &acttl[0]);

   /* -- Set the report's titles for this DLL. */
   SetTitles(prept, argcnt, argvars);



   


   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */
   us =  ItiPrtLoadDLLInfo(hab, hmq, prept, argcnt, argvars);
   if (us != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return (ITIPRT_NOT_INITIALIZED);
      }

   ItiPrtBeginReport (PRT_Q_NAME);




   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_JBDK,
                       szQueryJobBrkDwnKeys, sizeof szQueryJobBrkDwnKeys);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_JIK,
                       szQueryJobItemKeys, sizeof szQueryJobItemKeys);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JIK,
                       szConjJobItemKeys, sizeof szConjJobItemKeys);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_JICBEK,
                       szQueryJICBEKeys, sizeof szQueryJICBEKeys);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JICBEK,
                       szConjJICBEKeys, sizeof szConjJICBEKeys);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_JOB_INFO,
                       szQueryJobInfo, sizeof szQueryJobInfo );


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_BRKDWN_ITEM,
                       szQryJobBrkDwnItem, sizeof szQryJobBrkDwnItem);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_BRKDWN_ITEM_JBDK,
                       szConjJobBrkDwnItemJBDK, sizeof szConjJobBrkDwnItemJBDK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_BRKDWN_ITEM_JIK,
                       szConjJobBrkDwnItemJIK, sizeof szConjJobBrkDwnItemJIK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_BRKDWN_ITEM_JICBEK,
                       szConjJobBrkDwnItemJICBEK, sizeof szConjJobBrkDwnItemJICBEK);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_TASK_ITEM,
                       szQryJobTaskItem, sizeof szQryJobTaskItem);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_TASK_ITEM_TTL,
                       szQryJobTaskItemTTL, sizeof szQryJobTaskItemTTL);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_TASK_ITEM_QUP,
                       szQryJobTaskItemQUP, sizeof szQryJobTaskItemQUP);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_TASK_ITEM_FW,
                       szQryJobTaskItemFM, sizeof szQryJobTaskItemFM);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_ITEM_JIK,
                       szConjJobTaskItemJIK, sizeof szConjJobTaskItemJIK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_ITEM_JICBEK,
                       szConjJobTaskItemJICBEK, sizeof szConjJobTaskItemJICBEK);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_JOB_TASK_PROD,
                       szQryJobTaskProd, sizeof szQryJobTaskProd);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_PROD_JIK,
                       szConjJobTaskProdJIK, sizeof szConjJobTaskProdJIK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_PROD_JICBEK,
                       szConjJobTaskProdJICBEK, sizeof szConjJobTaskProdJICBEK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_PROD_JCSK,
                       szConjJobTaskProdJCSK, sizeof szConjJobTaskProdJCSK); 

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_JOB_TASK_PROD_TID,
                       szConjJobTaskProdTID, sizeof szConjJobTaskProdTID);  


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_CST_MEL,
                       szQryCstMEL, sizeof szQryCstMEL);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_CST_TTL,
                       szQryCstTTL, sizeof szQryCstTTL);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_CST_MARKUP,
                       szQryCstMarkUp, sizeof szQryCstMarkUp);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QRY_CST_UNITPRICE,
                       szQryCstUnitPrice, sizeof szQryCstUnitPrice);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_MAT,
                       szQueryMat, sizeof szQueryMat);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_MAT,
                       szConjMat, sizeof szConjMat);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_MAT_SUM,
                       szQueryMatSum, sizeof szQueryMatSum);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW,
                       szQueryCrew, sizeof szQueryCrew);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREWKEY,
                       szQueryCrewKey, sizeof szQueryCrewKey);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_EQP,
                       szQueryCrewEqp, sizeof szQueryCrewEqp);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_EQP_JCSK,
                       szConjCrewEqpJCSK, sizeof szConjCrewEqpJCSK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_EQP_JCK,
                       szConjCrewEqpJCK, sizeof szConjCrewEqpJCK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_EQP_TTL,
                       szQueryCrewEqpTtl, sizeof szQueryCrewEqpTtl);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_EQP_TTL,
                       szConjCrewEqpTtl, sizeof szConjCrewEqpTtl);


   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_LBR,
                       szQueryCrewLbr, sizeof szQueryCrewLbr);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_LBR_JCSK,
                       szConjCrewLbrJCSK, sizeof szConjCrewLbrJCSK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_LBR_JCK,
                       szConjCrewLbrJCK, sizeof szConjCrewLbrJCK);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_LBR_TTL,
                       szQueryCrewLbrTtl, sizeof szQueryCrewLbrTtl);

   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_CONJ_CREW_LBR_TTL,
                       szConjCrewLbrTtl, sizeof szConjCrewLbrTtl);

                 
   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_EQP_PROD,
                       szQueryCrewEqpProd, sizeof szQueryCrewEqpProd);
                 
   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_CREW_LBR_PROD,
                       szQueryCrewLbrProd, sizeof szQueryCrewLbrProd);
                 
   ItiMbQueryQueryText(hmodrJECbe, ITIRID_RPT, ID_QUERY_UNIT_DAY,
                       szQueryUnitDay, sizeof szQueryUnitDay);
 

   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(szCurJobKey,sizeof(szCurJobKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      /* === Make a list of the Job/Brkdwn keys. ========================= */
      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryJobBrkDwnKeys, " ", szCurJobKey);
 

      usRes = MakeList(hmodrJECbe, hhprJBDK, szQuery, ID_QUERY_JBDK,
                       FALSE,  FALSE,
                       &usJobBrkDwnCnt, &ppkisJobBrkDwnKeys);
      if (usRes != 0)
         {
         ItiRptUtErrorMsgBox (NULL, "MakeList for JobBreakdowns failed.");
         return (13);
         }

      for (usJBDC = 0; usJBDC < usJobBrkDwnCnt; usJBDC++)
         {/* -- START LOOP FOR JOB BRKDWN KEYS */
         ItiStrCpy(szCurJobBrkDwnKey,ppkisJobBrkDwnKeys[usJBDC]->szKey,SZKEY_LEN);
         ItiStrCpy(szCurJobBrkDwnID, ppkisJobBrkDwnKeys[usJBDC]->pszID,SMARRAY);


         /* === Make a list of the Job/Brkdwn/Item keys. ================= */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryJobItemKeys, szCurJobKey, szConjJobItemKeys);
         ItiStrCat(szQuery, szCurJobBrkDwnKey, sizeof szQuery);


         usRes = MakeList(hmodrJECbe, hhprJIK, szQuery, ID_QUERY_JIK, 
                          FALSE,  FALSE,
                          &usJobItemCnt, &ppkisJobItemKeys);
         if (usRes != 0)
            {
            ItiRptUtErrorMsgBox (NULL, "MakeList for JobItems failed.");
            return (13);
            }



         for (usJIC = 0; usJIC < usJobItemCnt; usJIC++)
            {/* -- START LOOP FOR JOB ITEM KEYS */
            ItiStrCpy(szCurJobItemKey,ppkisJobItemKeys[usJIC]->szKey,SZKEY_LEN);
            ItiStrCpy(szCurJobItemID, ppkisJobItemKeys[usJIC]->pszID,SMARRAY);


            /* === Make a list of the Job/Brkdwn/Item/CostBased Est keys. ================= */
            ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                              szQueryJICBEKeys, szCurJobKey, szConjJICBEKeys );
            ItiStrCat(szQuery, szCurJobItemKey, sizeof szQuery);
            

            /* -- Add the "use only Active cost sheet" restriction. */
            if (bActiveOnly)
               ItiStrCat(szQuery, " AND Active > 0 ", sizeof szQuery);
            /* -------------------------------------------------- */


            usRes = MakeList(hmodrJECbe, hhprJICBEK, szQuery, ID_QUERY_JICBEK, 
                             FALSE,  FALSE,
                             &usJICBEKCnt, &ppkisJICBEKeys);
            if (usRes != 0)
               {
               ItiRptUtErrorMsgBox (NULL, "MakeList for JobItemCostBasedEst failed.");
               return (13);
               }

         


            for (usJICBEK = 0; usJICBEK < usJICBEKCnt; usJICBEK++)
               {/* -- START LOOP FOR ITEM'S COSTBASED EST KEYS */
               ItiStrCpy(szCurJICBEKey,ppkisJICBEKeys[usJICBEK]->szKey,SZKEY_LEN);
               ItiStrCpy(szCurJICBEID, ppkisJICBEKeys[usJICBEK]->pszID,SMARRAY);


               JobItemBrkDwnSection();

               /* == Do the Task Xxxx using CostSheet Xxxx Section. */
               ItemTaskBrkDwnSection();



               /* -- Build the query to make a list *
                *    of current job item task keys. */
               ItiStrCpy(szQuery, szQryJobTaskItem,        sizeof szQuery);
               ItiStrCat(szQuery, szQryJobTaskItemFM,      sizeof szQuery);
               ItiStrCat(szQuery, szCurJobKey,             sizeof szQuery);
               ItiStrCat(szQuery, szConjJobTaskItemJIK,    sizeof szQuery);
               ItiStrCat(szQuery, szCurJobItemKey,         sizeof szQuery);
               ItiStrCat(szQuery, szConjJobTaskItemJICBEK, sizeof szQuery);
               ItiStrCat(szQuery, szCurJICBEKey,           sizeof szQuery);

               /* -- Build a list of TaskIDs and their *
                *    associated JobCostSheetKeys.      */
               usRes = MakeList(hmodrJECbe, hhprJECbe, szQuery,
                                ID_QRY_JOB_TASK_ITEM,
                                FALSE,  FALSE,
                                &usTaskCnt, &ppkisTaskKeys);


               /* === Using the list of job cost sheets and task IDs... */
               for (usT = 0; usT < usTaskCnt; usT++)
                  {/* -- START LOOP FOR TASK KEYS */
                  ItiStrCpy(szCurTaskKey,ppkisTaskKeys[usT]->szKey,SZKEY_LEN);
                  /* misnamed, what is called the task key is really the JCSKey */
                  ItiStrCpy(szCurJobCstShtKey,ppkisTaskKeys[usT]->szKey,SZKEY_LEN);
                  ItiStrCpy(szCurTaskID, " '",SMARRAY);
                  ItiStrCat(szCurTaskID, ppkisTaskKeys[usT]->pszID,SMARRAY);
                  ItiStrCat(szCurTaskID, "' ",SMARRAY);

                  TaskProductivitySection();
                  BrkDwnSection();
                  MaterialSection();
                  CrewSection();
                  }/* -- END LOOP FOR TASK KEYS */
               ItiRptUtDestroyKeyIDList(hhprJECbe, &ppkisTaskKeys, usTaskCnt);


               }/* -- END LOOP FOR JOB ITEM COST BASED EST KEYS */
            ItiRptUtDestroyKeyIDList(hhprJICBEK, &ppkisJICBEKeys, usJICBEKCnt);


            }/* -- END LOOP FOR JOB ITEM KEYS */
         ItiRptUtDestroyKeyIDList(hhprJIK, &ppkisJobItemKeys, usJobItemCnt);


         }/* -- END LOOP FOR JOB BRKDWN KEYS */
      ItiRptUtDestroyKeyIDList(hhprJBDK, &ppkisJobBrkDwnKeys, usJobBrkDwnCnt);


      }/* End of WHILE GetNext(Job)Key */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   ItiMemDestroyHeap(hhprJECbe);
   ItiMemDestroyHeap(hhprJBDK);  
   ItiMemDestroyHeap(hhprJIK);   
   ItiMemDestroyHeap(hhprJICBEK);
   ItiMemDestroyHeap(hhprITBD);  


   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */

