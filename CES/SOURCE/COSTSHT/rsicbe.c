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


/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif


#include    "rindlsic.h"


#include "rSICbe.h"                                       

// ADDED 3 JUNE 93
#include "rCstSh.h"
#include "rindlcst.h"
//

/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "StdItm: CBEst"

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

static  CHAR  szCurrentStdItmNum   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


/* -- Current keys to be inserted into the queries: */
static  CHAR  szCurStdItmKey   [SMARRAY + 1] = "";
static  CHAR  szCurCostSheetID [SMARRAY + 1] = "";


static  CHAR   szTtlFmt  [SMARRAY +1] = "";
static  CHAR   szTtlRaw  [SMARRAY +1] = "";

// ADDED 3 JUNE 93
static HMODULE hmod;
static USHORT (*pfnFunc) (PSZ, BOOL);
static USHORT (*pfnUPFunc) (PSZ, PSZ, USHORT);
////

static  PP_KEYSIDS ppkisCBEKeys = NULL;
static  USHORT usCBECnt = 0;
static  CHAR   szCurCBEKey [SZKEY_LEN +1] = "";
static  CHAR   szCurCBEID  [SMARRAY +1] = "";


static  PP_KEYSIDS ppkisTaskKeys = NULL;
static  USHORT usTaskCnt = 0;
static  CHAR   szCurCstShtKey [SZKEY_LEN +1] = "";
static  CHAR   szCurTaskKey   [SMARRAY + 1] = "";

//static  CHAR   szCurTaskID  [SMARRAY +1] = "";


/* -- Queries: */
static  CHAR  szBaseQuery    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery2       [QUERY_ARRAY_SIZE + 1] = "";

/* -- ItemSection Function Queries */
static  CHAR  szQueryStdItmInfo [SIZE_OF_QUERY_STDITM_INFO + 1] = "";
static  CHAR  szQueryCBEstsCnt  [SIZE_OF_QUERY_CBESTS_CNT + 1]  = "";
static  CHAR  szQueryCBEInfo    [SIZE_OF_QUERY_CBE_INFO + 1]    = "";


/* -- ItemTaskSection Function */
static  CHAR  szQryItemCBETask      [SIZE_OF_QRY_ITEM_CBE_TASK + 1] = "";


/* -- TaskProductivitySection Function */
static  CHAR  szQryItemTaskProd     [SIZE_OF_QRY_ITEM_TASK_PROD + 1] = "";
static  CHAR  szConjItemTaskProdCSK [2*SMARRAY + 1] = "";
static  CHAR  szConjItemTaskProdTK  [2*SMARRAY + 1] = "";



/* -- BrkDwnSection Function Queries;  "Cost Sheet XXX... Cost Breakdown"  */
static  CHAR  szQryOvrhdMEL [SIZE_OF_QRY_OVRHD_MEL + 1] = "";

static  CHAR  szQryCstMatTtl    [SIZE_OF_QRY_CST_MAT_TTL + 1] = "";
static  CHAR  szSlcCstMatTtl    [SIZE_OF_SLC_CST_MAT_TTL + 1] = "";
static  CHAR  szQryCstMatTtlFW  [SIZE_OF_QRY_CST_MAT_TTL_FW + 1] = "";

static  CHAR  szQryCstEqpTtl    [SIZE_OF_QRY_CST_EQP_TTL + 1] = "";
static  CHAR  szSlcCstEqpTtl    [SIZE_OF_SLC_CST_EQP_TTL + 1] = "";
static  CHAR  szQryCstEqpTtlFW  [SIZE_OF_QRY_CST_EQP_TTL_FW + 1] = "";

static  CHAR  szQryCstLbrTtl    [SIZE_OF_QRY_CST_LBR_TTL + 1] = "";
static  CHAR  szSlcCstLbrTtl    [SIZE_OF_SLC_CST_LBR_TTL + 1] = "";
static  CHAR  szQryCstLbrTtlFW  [SIZE_OF_QRY_CST_LBR_TTL_FW + 1] = "";



//static  CHAR  szQryCstMEL       [SIZE_OF_QRY_CST_MEL + 1] = "";
//
//static  CHAR  szQryCstMarkUp    [SIZE_OF_QRY_CST_MARKUP + 1] = "";
//static  CHAR  szQryCstUnitPrice [SIZE_OF_QRY_CST_UNITPRICE + 1] = "";

static  CHAR  szQueryMat    [SIZE_OF_QUERY_MAT + 1] = "";
static  CHAR  szConjMat     [SMARRAY + 1] = "";
static  CHAR  szQueryMatSum [SIZE_OF_QUERY_MAT_SUM + 1] = "";




static  CHAR  szQueryCrew    [SIZE_OF_QUERY_CREW + 1] = "";
static  CHAR  szQueryCrewKey  [SIZE_OF_QUERY_CREW + 1] = "";

static  CHAR  szQueryCrewEqp [SIZE_OF_QUERY_CREW_EQP + 1] = "";
static  CHAR  szConjCrewEqpJCSK [2*SMARRAY + 1] = "";
static  CHAR  szConjCrewEqpCrewKey  [2*SMARRAY + 1] = "";
static  CHAR  szQueryCrewEqpTtl [SIZE_OF_QUERY_CREW_EQP_TTL + 1] = "";
static  CHAR  szConjCrewEqpTtl  [2*SMARRAY + 1] = "";

static  CHAR  szQueryCrewLbr [SIZE_OF_QUERY_CREW_LBR + 1] = "";
static  CHAR  szConjCrewLbrJCSK [2*SMARRAY + 1] = "";
static  CHAR  szConjCrewLbrCrewKey  [2*SMARRAY + 1] = "";
static  CHAR  szQueryCrewLbrTtl [SIZE_OF_QUERY_CREW_LBR_TTL + 1] = "";
static  CHAR  szConjCrewLbrTtl  [2*SMARRAY + 1] = "";


static  CHAR  szQueryCrewEqpProd [SIZE_OF_QUERY_CREW_EQP_PROD + 1] = "";
static  CHAR  szConjCrewEqpProd  [2*SMARRAY + 1] = "";

static  CHAR  szQueryCrewLbrProd [SIZE_OF_QUERY_CREW_LBR_PROD + 1] = "";
static  CHAR  szConjCrewLbrProd  [2*SMARRAY + 1] = "";



           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprJBDK   = NULL;
static  HHEAP   hhprJIK    = NULL;
static  HHEAP   hhprJICBEK = NULL;
static  HHEAP   hhprITBD   = NULL;

static  HHEAP   hhprSICbe  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

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
      case ID_QRY_ITEM_CBE_TASK:
         ItiStrCpy (szTmp, acttl[RPT_TTL_6].szTitle, sizeof szTmp);
         ItiStrCat (szTmp, szCurCBEID, sizeof szTmp);
         ItiStrCat (szTmp, acttl[RPT_TTL_7].szTitle, sizeof szTmp);
         ItiStrCat (szTmp, szCurrentStdItmNum, sizeof szTmp);
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








USHORT StdItemSection (void)
   {
   USHORT usRes;
   PSZ  * ppsz = NULL;


   if (szCurStdItmKey[0] == '\0')
      {
      return 13;
      }


   if (usCurrentPg != 0) /* ie. iff this is not the first page. */
      {
//      ItiPrtNewPg();
      }
   usCurrentPg = ItiPrtQueryCurrentPgNum();
   ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");




   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryStdItmInfo, sizeof szQuery); 
   ItiStrCat(szQuery, szCurStdItmKey,    sizeof szQuery);


   ppsz = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QUERY_STDITM_INFO, &usNumCols);
   if (ppsz != NULL)
      {
      ItiStrCpy(szCurrentStdItmNum, ppsz[STDITM_NUM], sizeof szCurrentStdItmNum);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[STDITM_NUM]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[STDITM_DESC]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn();

      ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);
      }
   else
      {
      SETQERRMSG;
      }
   

   /* === Item's CBE count line. */
   ItiStrCpy(szQuery, szQueryCBEstsCnt, sizeof szQuery);
   ItiStrCat(szQuery, szCurStdItmKey, sizeof szQuery);

   ppsz = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QUERY_CBESTS_CNT, &usNumCols);
   if (ppsz != NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[COUNT]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);
      ItiPrtIncCurrentLn();

      ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);
      }
   else
      {
      SETQERRMSG;
      }




   /* === Now make a list of all of the Cost-Based Estimates *
    * === for the current standard item key.                 */

   /* -- Build a list of Cost-Based Estimate Keys for global use; */
   /* -- but first recover memory from any previously built list. */
   if ((ppkisCBEKeys != NULL) && (usCBECnt > 0))
      {
      ItiRptUtDestroyKeyIDList(hhprSICbe, &ppkisCBEKeys, usCBECnt);
      usCBECnt = 0;
      }

   ItiStrCpy(szQuery, szQueryCBEInfo, sizeof szQuery);
   ItiStrCat(szQuery, szCurStdItmKey, sizeof szQuery);


   usRes = MakeList(hmodrSICbe, hhprSICbe, szQuery,
                    ID_QUERY_CBE_INFO,
                    FALSE,  FALSE,
                    &usCBECnt, &ppkisCBEKeys);


   ItiStrCat(szQuery, " order by 2 ", sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                          ID_QUERY_CBE_INFO, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      return 13;
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         ItiPrtPutTextAt (CURRENT_LN, 0,
                          LEFT, USE_RT_EDGE,
                          ppsz[CBE_PREF]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, USE_RT_EDGE,
                          ppsz[CBE_ID]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_5].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1,
                          (LEFT|WRAP),
                          (2 | FROM_RT_EDGE),
                          ppsz[CBE_DESC]);
//         ItiPrtIncCurrentLn();

         ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);
         }/* end while */

      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      }

   return 0;
   }/* End of Function StdItemSection */








USHORT BrkDwnSection (BOOL bPrint, PSZ pszTotalFmt, PSZ pszTotalRaw)
                      /* Note: All total PSZs are of SMARRAY size */
   {
   USHORT us = 0;
   USHORT usStart, usStop, usStartCol, usStopCol;
   CHAR   szTmp         [SMARRAY] = "";
   CHAR   szTmp2        [SMARRAY] = "";
   CHAR   szMatOvrhdPct [SMARRAY + 1] = "";
   CHAR   szEqpOvrhdPct [SMARRAY + 1] = "";
   CHAR   szLbrOvrhdPct [SMARRAY + 1] = "";
   CHAR   szMatOvrhdRaw [SMARRAY + 1] = "";
   CHAR   szEqpOvrhdRaw [SMARRAY + 1] = "";
   CHAR   szLbrOvrhdRaw [SMARRAY + 1] = "";
   CHAR   szMarkUpPct   [SMARRAY + 1] = "";
   CHAR   szMarkUpRaw   [SMARRAY + 1] = "";
                                          
   CHAR   szMaterialTtlRaw  [SMARRAY + 1] = " 0 ";
   CHAR   szEquipmentTtlRaw [SMARRAY + 1] = " 0 ";
   CHAR   szLaborTtlRaw     [SMARRAY + 1] = " 0 ";
   CHAR   szMaterialTtlFmt  [SMARRAY + 1] = " 0 ";
   CHAR   szEquipmentTtlFmt [SMARRAY + 1] = " 0 ";
   CHAR   szLaborTtlFmt     [SMARRAY + 1] = " 0 ";

   USHORT  usNumColsLoc = 0;
   PSZ   * ppszLoc   = NULL;



   if (bPrint)
      {
      if (ItiPrtKeepNLns(12) == PG_WAS_INC)
         {
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }
      SetColumnTitles(0, BRKDWN);
      }



   ItiStrCpy(szQuery, szQryOvrhdMEL,  sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);

   ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QRY_OVRHD_MEL, &usNumColsLoc);
   if (ppszLoc != NULL)
      {
      ItiStrCpy(szMatOvrhdPct, ppszLoc[OVRHD_MEL_MAT_PCT],  sizeof szMatOvrhdPct);
      ItiStrCpy(szEqpOvrhdPct, ppszLoc[OVRHD_MEL_EQP_PCT],  sizeof szEqpOvrhdPct);
      ItiStrCpy(szLbrOvrhdPct, ppszLoc[OVRHD_MEL_LBR_PCT],  sizeof szLbrOvrhdPct);
      ItiStrCpy(szMatOvrhdRaw, ppszLoc[OVRHD_MEL_MAT_RAW],  sizeof szMatOvrhdRaw);
      ItiStrCpy(szEqpOvrhdRaw, ppszLoc[OVRHD_MEL_EQP_RAW],  sizeof szEqpOvrhdRaw);
      ItiStrCpy(szLbrOvrhdRaw, ppszLoc[OVRHD_MEL_LBR_RAW],  sizeof szLbrOvrhdRaw);
      ItiStrCpy(szMarkUpPct, ppszLoc[OVRHD_MEL_MARKUP_PCT],  sizeof szMarkUpPct);
      ItiStrCpy(szMarkUpRaw, ppszLoc[OVRHD_MEL_MARKUP_RAW],  sizeof szMarkUpRaw);
      ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
      }
   else
      {
      SETQERRMSG;
      return 13;
      }





   /* -- Material line: */
   ItiStrCpy(szQuery, szQryCstMatTtl, sizeof szQuery);
   ItiStrCat(szQuery, szMatOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstMatTtl, sizeof szQuery);
   ItiStrCat(szQuery, szMatOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstMatTtl, sizeof szQuery);  /* Unformatted, raw */
   ItiStrCat(szQuery, szMatOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szQryCstMatTtlFW, sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);


   ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QRY_CST_MAT_TTL, &usNumColsLoc);
   if (ppszLoc != NULL)
      {
      if (bPrint)
         {
         /* -- Materials line: */
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_58].usTtlCol,
                      LEFT, (acttl[RPT_TTL_58].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_58].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_MAT_UNIT_COST]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                         LEFT, (2 | REL_LEFT),
                         "+ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_MAT_OVRHD_AMT]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (1 | REL_LEFT),
                         "(");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                          szMatOvrhdPct);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (3 | REL_LEFT),
                         ") =");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_MAT_EXT]);
         ItiStrCpy(szMaterialTtlRaw, ppszLoc[CST_MAT_EXT_RAW], sizeof szMaterialTtlRaw);
         ItiStrCpy(szMaterialTtlFmt, ppszLoc[CST_MAT_EXT],     sizeof szMaterialTtlFmt);
         ItiPrtIncCurrentLn(); //line feed

         }/* end if (bPrint... */
      else
         {
         ItiStrCpy(szMaterialTtlRaw, ppszLoc[CST_MAT_EXT_RAW], sizeof szMaterialTtlRaw);
         ItiStrCpy(szMaterialTtlFmt, ppszLoc[CST_MAT_EXT],     sizeof szMaterialTtlFmt);
         }

      }/* end if (ppszLoc... ID_QRY_MAT_TTL */






   ItiStrCpy(szQuery, szQryCstEqpTtl, sizeof szQuery);
   ItiStrCat(szQuery, szEqpOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstEqpTtl, sizeof szQuery);
   ItiStrCat(szQuery, szEqpOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstEqpTtl, sizeof szQuery);  /* Unformatted, raw */
   ItiStrCat(szQuery, szEqpOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szQryCstEqpTtlFW, sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);


   ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QRY_CST_EQP_TTL, &usNumColsLoc);
   if (ppszLoc != NULL)
      {
      if (bPrint)
         {     
         /* -- Equipment line: */
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_59].usTtlCol,
                         LEFT, (acttl[RPT_TTL_59].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_59].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_EQP_UNIT_COST]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                         LEFT, (2 | REL_LEFT),
                         "+ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_EQP_OVRHD_AMT]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (1 | REL_LEFT),
                         "(");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                          szEqpOvrhdPct);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (3 | REL_LEFT),
                         ") =");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_EQP_EXT]);
         ItiStrCpy(szEquipmentTtlRaw, ppszLoc[CST_EQP_EXT_RAW], sizeof szEquipmentTtlRaw);
         ItiStrCpy(szEquipmentTtlFmt, ppszLoc[CST_EQP_EXT],     sizeof szEquipmentTtlFmt);
         ItiPrtIncCurrentLn(); //line feed
         }/* end if (bPrint... */
      else
         {
         ItiStrCpy(szEquipmentTtlRaw, ppszLoc[CST_EQP_EXT_RAW], sizeof szEquipmentTtlRaw);
         ItiStrCpy(szEquipmentTtlFmt, ppszLoc[CST_EQP_EXT],     sizeof szEquipmentTtlFmt);
         }

      }/* end if (ppszLoc... ID_QRY_EQP_TTL */





   ItiStrCpy(szQuery, szQryCstLbrTtl, sizeof szQuery);
   ItiStrCat(szQuery, szLbrOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstLbrTtl, sizeof szQuery);
   ItiStrCat(szQuery, szLbrOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szSlcCstLbrTtl, sizeof szQuery);  /* Unformatted, raw */
   ItiStrCat(szQuery, szLbrOvrhdRaw,       sizeof szQuery);
   ItiStrCat(szQuery, szQryCstLbrTtlFW, sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);

   ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QRY_CST_LBR_TTL, &usNumColsLoc);
   if (ppszLoc != NULL)
      {
      if (bPrint)
         {     
         /* -- Labor line: */
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_60].usTtlCol,
                         LEFT, (acttl[RPT_TTL_60].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_60].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_LBR_UNIT_COST]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                         LEFT, (2 | REL_LEFT),
                         "+ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_LBR_OVRHD_AMT]);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (1 | REL_LEFT),
                         "(");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                          szLbrOvrhdPct);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (3 | REL_LEFT),
                         ") =");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                          ppszLoc[CST_LBR_EXT]);
         ItiStrCpy(szLaborTtlRaw, ppszLoc[CST_LBR_EXT_RAW], sizeof szLaborTtlRaw);
         ItiStrCpy(szLaborTtlFmt, ppszLoc[CST_LBR_EXT],     sizeof szLaborTtlFmt);
         ItiPrtIncCurrentLn(); //line feed

         }/* end if (bPrint... */
      else
         {
         ItiStrCpy(szLaborTtlRaw, ppszLoc[CST_LBR_EXT_RAW], sizeof szLaborTtlRaw);
         ItiStrCpy(szLaborTtlFmt, ppszLoc[CST_LBR_EXT],     sizeof szLaborTtlFmt);
         }

      }/* end if (ppszLoc... ID_QRY_LBR_TTL */





      /* === Material Equipment Labor Total: */
      ItiRptUtMath(ID_EXP_10, SMARRAY, szTmp,
                   szMaterialTtlRaw, " + ", szEquipmentTtlRaw);

      ItiRptUtMath(ID_EXP_10, SMARRAY, pszTotalRaw,
                   szTmp, " + ", szLaborTtlRaw);

      ItiRptUtMath(ID_EXP_4DC, SMARRAY, pszTotalFmt,
                   pszTotalRaw, " + ", " 0 ");       /* reformat with $, */
      szTmp[0] = '\0';

      if (bPrint)
         {
         us = ItiStrLen(pszTotalFmt);

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
                          pszTotalFmt);
         ItiPrtIncCurrentLn(); //blank line

         }/* end if (bPrint... */
      



      /* === Mark-Up line: */
          /* convert raw markup pct% to 1.nn multiplier. */
      ItiRptUtMath(ID_EXP_10, SMARRAY, szTmp2, 
                   szMarkUpRaw, " * ", pszTotalRaw);
      if (bPrint)
         {
         ItiStrCpy(szTmp, "+ ", sizeof szTmp);
         ItiStrCat(szTmp, szMarkUpPct, sizeof szTmp);
         ItiStrCat(szTmp, acttl[RPT_TTL_53].szTitle, sizeof szTmp);

         us = ItiStrLen(szTmp);
         ItiPrtPutTextAt(CURRENT_LN, (usStartCol - us), LEFT,
                         (usStartCol),
                         szTmp);
         szTmp[0] = '\0';

         ItiRptUtMath(ID_EXP_4DC, SMARRAY, szTmp,
                      szTmp2, " + ", " 0 ");       /* Formatted w/ $, */

         ItiPrtPutTextAt (CURRENT_LN,
                          acttl[RPT_TTL_53].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_53].usDtaColWth | REL_LEFT),
                          szTmp);
         ItiPrtIncCurrentLn(); //blank line

         ItiPrtDrawSeparatorLn (CURRENT_LN, usStartCol, usStopCol);
         ItiPrtIncCurrentLn (); //line feed
         }/* end if (bPrint... */


   
     /* === Unit Price line: */
      ItiStrCpy(szTmp, pszTotalRaw, sizeof szTmp);
      ItiRptUtMath(ID_EXP_10, SMARRAY, pszTotalRaw,
                   szTmp2, " + ", szTmp);   /* MarkUp + Total (raw) */

      ItiRptUtMath(ID_EXP_4DC, SMARRAY, pszTotalFmt,
                   szTmp2, " + ", szTmp);   /* MarkUp + Total (formatted) */


      if (bPrint)
         {
         ItiPrtPutTextAt(CURRENT_LN, (usStartCol - acttl[RPT_TTL_54].usTtlColWth),
                      LEFT, (acttl[RPT_TTL_54].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_54].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_54].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_54].usDtaColWth | REL_LEFT),
                          pszTotalFmt);
         ItiPrtIncCurrentLn(); 

         /* -- Finishout this section. */
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         us = ItiPrtIncCurrentLn (); //line feed
         }


   return us;
   }/* End of Function BrkDwnSection */








USHORT ItemTaskBrkDwnSection (void)
/* ===== The Cost-Based Estimate XXXX for Item 99...99 Section */
   {
   USHORT usLn, us;
   BOOL   bNewID = TRUE;
   BOOL   bLabeled;
   CHAR   szTmp [2 * SMARRAY] = "/ ";
   CHAR   szRunningExtTtl[2 * SMARRAY] = " 0 ";

   bLabeled = FALSE;

   /* -- Build the query to get the CBE tasks. */
   ItiStrCpy(szQuery2, szQryItemCBETask, sizeof szQuery2);
   ItiStrCat(szQuery2, szCurCBEKey,      sizeof szQuery2);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery2, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                          ID_QRY_ITEM_CBE_TASK, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         usLn = 6+ItiStrLen(ppsz[ITEM_CBE_TASK_DESC])/acttl[RPT_TTL_16].usDtaColWth;
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

            SetColumnTitles (ID_QRY_ITEM_CBE_TASK, 0);

            bLabeled = TRUE;
            }


         if (!bLabeled)
            {
            SetColumnTitles (ID_QRY_ITEM_CBE_TASK, 0);
            bLabeled = TRUE;
            }



         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                         LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                         ppsz[ITEM_CBE_TASK_ID]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                         ppsz[ITEM_CBE_TASK_QTY]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                         LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_10].szTitle);



         /* -- Must calculate the unit price, with the task's CstShtKey! */
         ItiStrCpy(szCurCstShtKey, ppsz[ITEM_CBE_TASK_JOB_CSTSHT_KEY],sizeof szCurCstShtKey);
//         BrkDwnSection(FALSE, &szTtlFmt[0], &szTtlRaw[0]);
         pfnUPFunc(szCurCstShtKey, &szTtlRaw[0], sizeof szTtlRaw);
         ItiFmtFormatString (szTtlRaw, szTtlFmt, sizeof szTtlFmt,
                             "Number,$,....", &us);

         ItiStrCpy(szCurCstShtKey, " ",sizeof szCurCstShtKey);

         /* update a running extention total. */
         ItiRptUtMath(ID_EXP_10, SMARRAY, szTmp,
                      ppsz[ITEM_CBE_TASK_QTY_RAW], " * ", szTtlRaw);  
         ItiRptUtMath(ID_EXP_10, SMARRAY, szRunningExtTtl,
                      szTmp, " + ", szRunningExtTtl);  


         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                         szTtlFmt);



         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                         LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_13].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                         ppsz[ITEM_CBE_TASK_UNIT_CODE_ID]);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                         LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_14].szTitle);

         /* Extention = Quantity(raw) * TtlRaw(from BrkDwnSection) */
         ItiRptUtMath(ID_EXP_2DC, SMARRAY, szTmp,
                      ppsz[ITEM_CBE_TASK_QTY_RAW], " * ", szTtlRaw);  
         
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                         szTmp);
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                          (LEFT|WRAP),
                          (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                          ppsz[ITEM_CBE_TASK_DESC]);

         ItiFreeStrArray(hhprSICbe, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      /* -- If a CBE does not have any tasks, then no need to do anything. */
      if (!bLabeled)
         return 13;


      /* -- Writeout the total subsection for the item's task list. */
      ItiRptUtMath(ID_EXP_2DC, SMARRAY, szRunningExtTtl,
                   " 0.000 ", " + ", szRunningExtTtl);  /* Format with $, */

      ItiPrtDrawSeparatorLn (CURRENT_LN,
                             acttl[RPT_TTL_15].usDtaCol, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                      LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_18].szTitle);
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                      RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                      szRunningExtTtl);
      ItiPrtIncCurrentLn (); //line feed


      /* -- Finishout this section. */
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      usLn = ItiPrtIncCurrentLn (); //line feed

      }/* else clause */
   


   return usLn;
   }/* End of Function ItemTaskBrkDwnSection */






USHORT TaskProductivitySection (void)
   {
   USHORT usLn;


   /* With the current TaskID & JobCostSheetKey ofthe ppkisTaskKeys list... */
   ItiStrCpy(szQuery, szQryItemTaskProd, sizeof szQuery);
   ItiStrCat(szQuery, szCurCBEKey,   sizeof szQuery);

   ItiStrCat(szQuery, szConjItemTaskProdCSK, sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey,          sizeof szQuery);

   ItiStrCat(szQuery, szConjItemTaskProdTK, sizeof szQuery);
   ItiStrCat(szQuery, szCurTaskKey,         sizeof szQuery);



   ppsz = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                        ID_QRY_ITEM_TASK_PROD, &usNumCols);
   if (ppsz != NULL)
      {
      /* -- Check for new page. */
      usLn = 7+ItiStrLen(ppsz[ITEM_TASK_PROD_DESC])/acttl[RPT_TTL_20].usDtaColWth;
      if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }

      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                      LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_20].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                      LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                      ppsz[ITEM_TASK_PROD_TASKID]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_21].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                      ppsz[ITEM_TASK_PROD_CSTSHT_ID]);
      /* -- Set the new cost sheet ID for use by section titles. */
      ItiStrCpy(szCurCostSheetID, ppsz[ITEM_TASK_PROD_CSTSHT_ID], sizeof szCurCostSheetID);
      ItiPrtIncCurrentLn (); //line feed

      /* Task description: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                       (LEFT|WRAP),
                       (5 | FROM_RT_EDGE),
                       ppsz[ITEM_TASK_PROD_DESC]);
      ItiPrtIncCurrentLn (); //blank line

      /* Productivity: */
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_22].usTtlCol,
                      LEFT, (acttl[RPT_TTL_22].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_22].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                      ppsz[ITEM_TASK_PROD_RATE]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_23].szTitle);
      ItiPrtIncCurrentLn (); //line feed


      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                      LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_24].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_24].usDtaColWth | REL_LEFT),
                      ppsz[ITEM_TASK_PROD_HRS_PER_DAY]);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_25].szTitle);

//      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
//                      LEFT, (acttl[RPT_TTL_25].usDtaColWth | REL_LEFT),
//                      ppsz[ITEM_TASK_PROD_OT_HRS]);
//
//      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
//                      LEFT, (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
//                      acttl[RPT_TTL_26].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_27].usTtlCol,
                      LEFT, (acttl[RPT_TTL_27].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_27].szTitle);

      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                      LEFT, (acttl[RPT_TTL_27].usDtaColWth | REL_LEFT),
                      ppsz[ITEM_TASK_PROD_GRAN]);
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,
                      LEFT, (2 | REL_LEFT),
                      "s.");
      ItiPrtIncCurrentLn (); //line feed

      ItiFreeStrArray(hhprSICbe, ppsz, usNumCols);
      }
   else
      {
      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                      LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_20].szTitle);

//      ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
//                      LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
//                      szCurTaskID);

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




USHORT MaterialSection (void)
   {
   USHORT us = 0;
   USHORT usLn, usStart, usStop;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled;
   CHAR   szTmp[SMARRAY] = "";



   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryMat,     sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 ", sizeof szQuery);



   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
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

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                          ppsz[QUANTITY]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                          LEFT, (2 | REL_LEFT),
                          "@ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                          ppsz[UNIT_PRICE]);


         /* -- "/ Unit = " */
         ItiStrCpy(szTmp, "/ ", sizeof szTmp);
         ItiStrCat(szTmp, ppsz[MAT_UNIT], sizeof szTmp);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_38].usTtlCol,
                          LEFT, acttl[RPT_TTL_39].usDtaCol - 2,
                          szTmp);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol - 3,
                          LEFT, acttl[RPT_TTL_39].usDtaCol - 1,
                          " =");



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


         ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      /* -- Finishout this section, iff we had any data. */
      if (bLabeled)
         {
         /* -- Do the materials totals: */
         ItiStrCpy(szQuery, szQueryMatSum,  sizeof szQuery);
         ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);


         ppsz = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
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

            ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);
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
   CHAR   szCrewKey [SMARRAY] = "";
   CHAR   szLbrTtlRaw  [SMARRAY] = "";
   CHAR   szEqpTtlRaw  [SMARRAY] = "";
   CHAR   szSubQryCrewKeys [LGARRAY] = "";
   CHAR   szTmp [SMARRAY + 1] = "";

           /* Used by DbGetQueryRow function: */
   HQRY    hqryLoc   = NULL;
   USHORT  usResIdLoc   = ITIRID_RPT;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc  = 0;
   PSZ   * ppszLoc   = NULL;





   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryCrew,    sizeof szQuery);
   ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);

   /* -- Use the current keys in the Eqp & Lbr totals subquery. */
   ItiStrCpy(szSubQryCrewKeys, " (", sizeof szSubQryCrewKeys);
   ItiStrCat(szSubQryCrewKeys, szQueryCrewKey, sizeof szQuery);
   ItiStrCat(szSubQryCrewKeys, szCurCstShtKey, sizeof szQuery);
   ItiStrCat(szSubQryCrewKeys, ") ", sizeof szSubQryCrewKeys);


   hqry = ItiDbExecQuery (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
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

         ItiStrCpy(szCrewKey, ppsz[CREW_KEY], sizeof szCrewKey);



         /* === The crew's equipment: */
         ItiStrCpy(szQuery, szQueryCrewEqp, sizeof szQuery);
         ItiStrCat(szQuery, szCurCstShtKey, sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewEqpCrewKey, sizeof szQuery);
         ItiStrCat(szQuery, szCrewKey, sizeof szQuery);


         hqryLoc = ItiDbExecQuery (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
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


//               /* -- Overtime line */
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
//                            ppszLoc[EQP_OT_RATE]);
//
//               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
//                               LEFT, (6 | REL_LEFT),
//                               "/hr *");
//
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
//                            ppszLoc[EQP_OT_HRS]);
//
//               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
//                               LEFT, (9 | REL_LEFT),
//                               " ovr-tm =");
//
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_49].usDtaColWth | REL_LEFT),
//                            ppszLoc[EQP_OT_DAILY_CST]);
//               ItiPrtIncCurrentLn (); //line feed



               /* -- Equipment description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_48].usDtaColWth | REL_LEFT),
                             ppszLoc[EQP_DESC]);

               ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
               }
            }/* end of if (hqryLoc... else clause */



            /* -- Now the crew's labor: */
         ItiStrCpy(szQuery, szQueryCrewLbr, sizeof szQuery);
         ItiStrCat(szQuery, szCurCstShtKey,          sizeof szQuery);
         ItiStrCat(szQuery, szConjCrewLbrCrewKey, sizeof szQuery);
         ItiStrCat(szQuery, szCrewKey, sizeof szQuery);

         hqryLoc = ItiDbExecQuery (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
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


//               /* -- Overtime line */
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
//                            ppszLoc[LBR_OT_RATE]);
//
//               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
//                               LEFT, (6 | REL_LEFT),
//                               "/hr *");
//
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_47].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_47].usDtaColWth | REL_LEFT),
//                            ppszLoc[LBR_OT_HRS]);
//
//               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
//                               LEFT, (9 | REL_LEFT),
//                               " ovr-tm =");
//
//               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
//                            RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
//                            ppszLoc[LBR_OT_DAILY_CST]);
//               ItiPrtIncCurrentLn (); //line feed


               /* -- Labor description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_48].usDtaColWth | REL_LEFT),
                             ppszLoc[LBR_DESC]);

               ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
               }

            ItiPrtIncCurrentLn (); //blank line

            }/* end of if (hqryLoc... else clause */



         /* recover memory */
         ItiFreeStrArray (hhprSICbe, ppsz, usNumCols);

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
      ItiStrCat(szQuery, szCurCstShtKey,    sizeof szQuery);

      ItiStrCat(szQuery, szConjCrewEqpTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryCrewKeys, sizeof szQuery);


      ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                              ID_QUERY_CREW_EQP_TTL, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_49].usDtaColWth | REL_LEFT),
                         ppszLoc[EQP_TTL]);
//         ItiStrCpy(szEqpTtlRaw, ppszLoc[EQP_TTL_RAW], sizeof szEqpTtlRaw);

         ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
         }


      /* -- Labor sub total. */
      ItiStrCpy(szQuery, szQueryCrewLbrTtl, sizeof szQuery);
      ItiStrCat(szQuery, szCurCstShtKey,          sizeof szQuery);

      ItiStrCat(szQuery, szConjCrewLbrTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryCrewKeys, sizeof szQuery);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                              ID_QUERY_CREW_LBR_TTL, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
                         ppszLoc[LBR_TTL]);
//         ItiStrCpy(szLbrTtlRaw, ppszLoc[LBR_TTL_RAW], sizeof szLbrTtlRaw);

         ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
         }

      ItiPrtIncCurrentLn (); //line feed



      /* unit/day =  sub total. */
      ItiStrCpy(szQuery, szQueryCrewEqpProd, sizeof szQuery);
      ItiStrCat(szQuery, szCurCstShtKey,     sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewEqpProd,  sizeof szQuery);
      ItiStrCat(szQuery, szCurTaskKey,       sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewEqpTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryCrewKeys, sizeof szQuery);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                              ID_QUERY_CREW_EQP_PROD, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiStrCpy(szTmp, "/ ", sizeof szTmp);
         ItiStrCat(szTmp, ppszLoc[CREW_EQP_PROD_RATE], sizeof szTmp);
         ItiStrCat(szTmp, " unit/day = ", sizeof szTmp);

         us = ItiStrLen(szTmp);
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol - us,
                         LEFT, (us | REL_LEFT), szTmp);

         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_49].usDtaColWth+2) | REL_LEFT),
                         ppszLoc[CREW_EQP_PROD_TOTAL]);

         ItiPrtDrawSeparatorLn(CURRENT_LN + 1,
                               acttl[RPT_TTL_49].usDtaCol+1,
                               acttl[RPT_TTL_49].usDtaCol+acttl[RPT_TTL_49].usDtaColWth+1);

         ItiPrtPutTextAt(CURRENT_LN + 2,
                         acttl[RPT_TTL_49].usDtaCol - acttl[RPT_TTL_39].usTtlColWth ,
                         LEFT, (acttl[RPT_TTL_39].usTtlColWth  | REL_LEFT),
                         acttl[RPT_TTL_39].szTitle );

         ItiPrtPutTextAt(CURRENT_LN + 2, acttl[RPT_TTL_49].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_49].usDtaColWth+2) | REL_LEFT),
                         ppszLoc[CREW_EQP_PROD_COST]);

         ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
         }



      /* -- Now do the labor column. */
      ItiStrCpy(szQuery, szQueryCrewLbrProd, sizeof szQuery);
      ItiStrCat(szQuery, szCurCstShtKey,     sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewLbrProd,  sizeof szQuery);
      ItiStrCat(szQuery, szCurTaskKey,       sizeof szQuery);
      ItiStrCat(szQuery, szConjCrewLbrTtl, sizeof szQuery);
      ItiStrCat(szQuery, szSubQryCrewKeys, sizeof szQuery);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprSICbe, hmodrSICbe, ITIRID_RPT,
                              ID_QUERY_CREW_LBR_PROD, &usNumColsLoc);
      if (ppszLoc != NULL)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_50].usDtaColWth+2) | REL_LEFT),
                         ppszLoc[CREW_LBR_PROD_TOTAL]);

         ItiPrtDrawSeparatorLn(CURRENT_LN + 1,
                               acttl[RPT_TTL_50].usDtaCol+1,
                               acttl[RPT_TTL_50].usDtaCol+acttl[RPT_TTL_50].usDtaColWth+1);

         ItiPrtPutTextAt(CURRENT_LN + 2, acttl[RPT_TTL_50].usDtaCol,
                         RIGHT, ((acttl[RPT_TTL_50].usDtaColWth+2) | REL_LEFT),
                         ppszLoc[CREW_LBR_PROD_COST]);

         ItiFreeStrArray (hhprSICbe, ppszLoc, usNumColsLoc);
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

   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_RSICBE_SESSION);

   return (us);

   }/* End of PrintProc Function */




USHORT QryInit (void)
   {
   USHORT usRet = 0;

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_STDITM_INFO,
                       szQueryStdItmInfo, sizeof szQueryStdItmInfo );

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CBESTS_CNT,
                       szQueryCBEstsCnt, sizeof szQueryCBEstsCnt );

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CBE_INFO,
                       szQueryCBEInfo, sizeof szQueryCBEInfo );
////////////////

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_ITEM_CBE_TASK,
                       szQryItemCBETask, sizeof szQryItemCBETask);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_ITEM_TASK_PROD,
                       szQryItemTaskProd, sizeof szQryItemTaskProd);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_ITEM_TASK_PROD_CSK,
                       szConjItemTaskProdCSK, sizeof szConjItemTaskProdCSK); 

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_ITEM_TASK_PROD_TK,
                       szConjItemTaskProdTK, sizeof szConjItemTaskProdTK);


   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_OVRHD_MEL,
                       szQryOvrhdMEL, sizeof szQryOvrhdMEL);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_MAT_TTL,
                       szQryCstMatTtl, sizeof szQryCstMatTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_SLC_CST_MAT_TTL,
                       szSlcCstMatTtl, sizeof szSlcCstMatTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_MAT_TTL_FW,
                       szQryCstMatTtlFW, sizeof szQryCstMatTtlFW);


   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_EQP_TTL,
                       szQryCstEqpTtl, sizeof szQryCstEqpTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_SLC_CST_EQP_TTL,
                       szSlcCstEqpTtl, sizeof szSlcCstEqpTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_EQP_TTL_FW,
                       szQryCstEqpTtlFW, sizeof szQryCstEqpTtlFW);


   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_LBR_TTL,
                       szQryCstLbrTtl, sizeof szQryCstLbrTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_SLC_CST_LBR_TTL,
                       szSlcCstLbrTtl, sizeof szSlcCstLbrTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QRY_CST_LBR_TTL_FW,
                       szQryCstLbrTtlFW, sizeof szQryCstLbrTtlFW);



   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_MAT,
                       szQueryMat, sizeof szQueryMat);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_MAT,
                       szConjMat, sizeof szConjMat);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_MAT_SUM,
                       szQueryMatSum, sizeof szQueryMatSum);


   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW,
                       szQueryCrew, sizeof szQueryCrew);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREWKEY,
                       szQueryCrewKey, sizeof szQueryCrewKey);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_EQP,
                       szQueryCrewEqp, sizeof szQueryCrewEqp);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_EQP_CREWKEY,
                       szConjCrewEqpCrewKey, sizeof szConjCrewEqpCrewKey);




   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_EQP_TTL,
                       szQueryCrewEqpTtl, sizeof szQueryCrewEqpTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_EQP_TTL,
                       szConjCrewEqpTtl, sizeof szConjCrewEqpTtl);


   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_LBR,
                       szQueryCrewLbr, sizeof szQueryCrewLbr);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_LBR_JCSK,
                       szConjCrewLbrJCSK, sizeof szConjCrewLbrJCSK);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_LBR_CREWKEY,
                       szConjCrewLbrCrewKey, sizeof szConjCrewLbrCrewKey);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_LBR_TTL,
                       szQueryCrewLbrTtl, sizeof szQueryCrewLbrTtl);

   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_LBR_TTL,
                       szConjCrewLbrTtl, sizeof szConjCrewLbrTtl);

                 
   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_EQP_PROD,
                       szQueryCrewEqpProd, sizeof szQueryCrewEqpProd);
                 
   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_QUERY_CREW_LBR_PROD,
                       szQueryCrewLbrProd, sizeof szQueryCrewLbrProd);

                     
   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_EQP_PROD,
                       szConjCrewEqpProd, sizeof szConjCrewEqpProd);
                     
   usRet = ItiMbQueryQueryText(hmodrSICbe, ITIRID_RPT, ID_CONJ_CREW_LBR_PROD,
                       szConjCrewLbrProd, sizeof szConjCrewLbrProd);

   return usRet;
   }





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

BOOL EXPORT ItiDllQueryCompatibility (USHORT usCallerVersion)

   {
   DBGMSG(" *** In DLL version check function.");

   if (usCallerVersion <= DLL_VERSION)
      return (TRUE);

   return (FALSE);
   }








VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,   
                                 USHORT  usMaxSize,   
                                 PUSHORT pusWindowID) 
   {                                                  
   ItiStrCpy (pszBuffer, "Standard Item: Cost Based Estimate", usMaxSize);   
   *pusWindowID = rSICbe_RptDlgBox;                       
   }/* END OF FUNCTION */


BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
//   if ((usTargetWindowID == rSICbe_RptDlgBox)                     
//       && ( (usActiveWindowID == StandardItemCatL)
//          ||(usActiveWindowID == StandardItemCatS)) )
//
//      return TRUE;                                            
//   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrSICbeRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_RSICBE_SESSION, 0, NULL);
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
   PSZ     pszCurStdItmKey = szCurStdItmKey;
   USHORT  us;
   USHORT  usCBE, usT, usRes;




   /* ------------------------------------------------------------------- *\
    *  Create a memory heaps.                                             *
   \* ------------------------------------------------------------------- */
   hhprSICbe  = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentStdItmNum, "999999-999999", sizeof(szCurrentStdItmNum));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


//   ItiRptUtGetOption (OPT_SW_INCL_XYZ_RPT, szSuppRpt, sizeof szSuppRpt,
//                      argcnt, argvars);
//   if (szSuppRpt[0] == '\0') 
//      bPrtRpt = FALSE;
//   else
//      bPrtRpt = TRUE;



   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrSICbe, &usNumOfTitles, &acttl[0]);

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

   QryInit();


   //////ADDED 3 JUNE 93
   //hmod = ItiDllLoadDll("rCstSh", RPT_EXE_VERSION);
   hmod = ItiDllLoadDll("rCstSh", "1");
   pfnFunc = ItiDllQueryProcAddress(hmod, "COSTSHEETBRKDWNSECTIONS");
   pfnUPFunc = ItiDllQueryProcAddress(hmod, "COSTSHEETUNITPRICE");
   /////  

   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(szCurStdItmKey,sizeof(szCurStdItmKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      /* -- This function does the initial item intro section as well as *
       * -- building the list, ppkisCBEKeys, of Cost-Based Estimate keys *
       * -- for the Item.                                                */
      StdItemSection();


      /* === Using the list of Cost-based Estimates for the current Item... */
      for (usCBE = 0; usCBE < usCBECnt; usCBE++)
         {
         ItiStrCpy(szCurCBEKey,ppkisCBEKeys[usCBE]->szKey,SZKEY_LEN);
         ItiStrCpy(szCurCBEID, ppkisCBEKeys[usCBE]->pszID,SMARRAY);


         /* == Do the "Cost-Based Estimate XXXX for Item 99...99" Section. */
         /* -- Iff the CBE has one or more tasks. */   
         ItemTaskBrkDwnSection();



         /* -- Build the query to make a list *
          *    of current item's Task & CostSheet keys. */
         ItiStrCpy(szQuery, szQryItemCBETask, sizeof szQuery);
         ItiStrCat(szQuery, szCurCBEKey,      sizeof szQuery);


         /* -- Build a list of TaskKeys and their *
          *    associated CostSheetKeys.         */
         usRes = MakeList(hmodrSICbe, hhprSICbe, szQuery,
                          ID_QRY_ITEM_CBE_TASK,
                          FALSE,  FALSE,
                          &usTaskCnt, &ppkisTaskKeys);


         /* === Using the list of tasks... */
         for (usT = 0; usT < usTaskCnt; usT++)
            {/* -- START LOOP FOR TASK KEYS */
            ItiStrCpy(szCurCstShtKey,ppkisTaskKeys[usT]->szKey,SZKEY_LEN);
            ItiStrCpy(szCurTaskKey,  ppkisTaskKeys[usT]->pszID,SMARRAY);

            TaskProductivitySection();

            // MODIFIED 3 JUNE 93   Does the cost sheet sections by using
            //                      the rCstSh dll.
            pfnFunc(szCurCstShtKey, TRUE);
            // BrkDwnSection(TRUE, &szTtlFmt[0], &szTtlRaw[0]);
            // MaterialSection();
            // CrewSection();

            }/* -- END LOOP FOR TASK KEYS */
         ItiRptUtDestroyKeyIDList(hhprSICbe, &ppkisTaskKeys, usTaskCnt);


         }/* end of for (usCBE... */


      }/* End of WHILE GetNext(StandardItem)Key */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   ItiMemDestroyHeap(hhprSICbe);


   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */


