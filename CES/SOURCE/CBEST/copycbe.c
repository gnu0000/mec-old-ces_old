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
 * CopyCBE.c
 */


#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itierror.h"
#include "..\include\itiest.h"
#include "..\include\itiutil.h"
#include "..\include\cbest.h"
#include "..\include\itiglob.h"
#include "..\include\cesutil.h"
#include <stdio.h>
#include <string.h>
#include "CopyCBE.h"
#include "initcat.h"
#include "calcfn.h"


#define NUM_OF_KEYS  51
#define KEYLENGTH    9

static  CHAR szBigTemp     [1600]; /* was 4090 */
static  CHAR szBigTemp2    [1400];
static  CHAR szBigQuery    [1800]; /* was 4200 */
static  CHAR szTemp         [650];
static  CHAR szTemp1        [650];
static  CHAR szTemp2        [125];
static  CHAR szTemp3        [125];
static  CHAR szNewKeyCopy    [16];
static  CHAR szDescription  [256];
static  CHAR szDesc         [256];
// static PPSZ ppszCJMUTemp, ppszCJMUTempDef;
// static PPSZ ppszCJMUKeys, ppszCJMUValues;
//static  CHAR szCStoJCSTemp  [550];
static PPSZ ppszCJMUResults[300];

static CHAR   aszCSKeys[NUM_OF_KEYS+1][KEYLENGTH];
static CHAR   aszTaskKeys[NUM_OF_KEYS+1][KEYLENGTH];


BOOL EXPORT CalcCostSheetDuration (HHEAP  hheap, 
                            PSZ    pszJobKey, 
                            PSZ    pszJobItemKey, 
                            PSZ    pszJobCostSheetKey)
   {
   HHEAP  hhp;
   PSZ    *ppszPNames, *ppszParams;

   hhp = ItiMemCreateHeap (16000);

   ppszPNames = ItiStrMakePPSZ (hhp, 4, "JobKey",  "JobCostSheetKey",  "JobItemKey",   NULL);
   ppszParams = ItiStrMakePPSZ (hhp, 4,  pszJobKey, pszJobCostSheetKey, pszJobItemKey, NULL);

   CalcJobCostShtDuration (hhp, ppszPNames, ppszParams);
   // Added MAR 94
   CalcJobCostShtProductionRate (hhp, ppszPNames, ppszParams);

   ItiMemDestroyHeap(hhp);
   return TRUE;
   }








BOOL EXPORT CreateJobCostSheetForCBE (HHEAP   hheap, 
                                      HWND    hwnd, 
                                      PSZ     pszJobKey, 
                                      PSZ     pszJobItemKey,
                                      PSZ     *ppszCBEKeys)
   {
   USHORT   usNumCol, usErr, usRet, i;
   PPSZ     ppszParams, ppszPNames, ppszTemp;
   PSZ      pszNewKey, pszTaskProdAdj;
   HQRY     hqry;
   HHEAP    hhp;
   CHAR     szTemp2[250] = "";

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);

   ppszPNames = ItiStrMakePPSZ2 (hhp,
                        /* 0 */ "JobKey",
                        /* 1 */ "JobItemKey",
                        /* 2 */ "JobItemCostBasedEstimateKey",
                        /* 3 */ "NJCSHTKey",
                        /* 4 */ "CostSheetKey",
                        /* 5 */ "TaskKey",
                        /* 6 */ "CostBasedEstimateKey",
                        /* 7 */ NULL);

//   if (ItiDbBuildSelectedKeyArray2 (hhp, 0, szBigTemp, &ppszKeys, usKeys))
//      return FALSE;

   for (i=0; ppszCBEKeys[i] != NULL; i++)
      {
      ItiStrCpy(szBigTemp,
                   "/* CreateJobCostSheetForCBE */ "
                   " SELECT CostSheetKey, TaskKey, CostBasedEstimateKey"
                   " FROM Task"
                   " WHERE CostBasedEstimateKey = ", sizeof szBigTemp);
      ItiStrCat(szBigTemp, ItiDbGetZStr(ppszCBEKeys[i],0), sizeof szBigTemp);

      hqry = ItiDbExecQuery (szBigTemp, hhp, 0, 0, 0, &usNumCol, &usErr);

      if (hqry == NULL)
         {
         ItiFreeStrArray2 (hhp, ppszPNames);
         ItiMemDestroyHeap(hhp);
         return FALSE;
         }

      /* For each CostSheet associated with this CBE, get a new key for */
      /* the JobCostSheet and insert the row in the JobCostSheet table  */

      while (ItiDbGetQueryRow (hqry, &ppszTemp, &usErr))
         {
         pszNewKey = ItiDbGetKey (hhp, "JobCostSheet", 1);

         if (pszNewKey == NULL)
            {
            ItiMemDestroyHeap(hhp);
            return FALSE;
            }

         ppszParams = ItiStrMakePPSZ2 (hhp, pszJobKey, pszJobItemKey,
                                       ItiDbGetZStr (ppszCBEKeys [i], 4),
                                       pszNewKey, ppszTemp [0],
                                       ppszTemp [1], ppszTemp [2],
                                       NULL);

         /* -- Check the task production adjustment for an unusable 0 value. */
         ItiStrCpy (szTemp2,
                  " SELECT ProductionAdjustment "
                  " FROM Task "
                  " WHERE TaskKey = ", sizeof szTemp2);
         ItiStrCat (szTemp2, ppszParams[5], sizeof szTemp2);
         ItiStrCat (szTemp2, " AND CostSheetKey = ", sizeof szTemp2);
         ItiStrCat (szTemp2, ppszParams[4], sizeof szTemp2); 
         ItiStrCat (szTemp2, " AND CostBasedEstimateKey = ", sizeof szTemp2);
         ItiStrCat (szTemp2, ppszParams[6], sizeof szTemp2);

         pszTaskProdAdj = ItiDbGetRow1Col1 (szTemp2, hhp, 0, 0, 0);                     \
         if ((pszTaskProdAdj != NULL) && (pszTaskProdAdj[0] == '0'))
            {   
            /* -- Task's production adjustment is zero so do NOT use it *
             * -- as a multiplier of the cost sheet's production rate.  */
            ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_COST_SHEET_TPA1,
                                 szBigTemp, sizeof szBigTemp);
            }
         else
            {
            ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_COST_SHEET,
                                 szBigTemp, sizeof szBigTemp);
            }

         usRet = ItiStrReplaceParams (szBigQuery, szBigTemp, sizeof szBigQuery,
                                      ppszPNames, ppszParams, 100);
         if (usRet)
            {
            ItiDbTerminateQuery(hqry);
            ItiMemDestroyHeap(hhp);
            return FALSE;
            }

         ItiDbBeginTransaction (hhp);

         if (ItiDbExecSQLStatement (hhp, szBigQuery)) /* New JobCostSheet made. */
            {
            ItiDbRollbackTransaction (hhp);
            ItiDbTerminateQuery(hqry);
            ItiMemDestroyHeap(hhp);
            return FALSE;
            }

         ItiDbCommitTransaction (hhp);


         CalcCostSheetDuration (hhp, pszJobKey, pszJobItemKey, pszNewKey);

//         CreateJobMaterialsUsed (hhp, pszJobKey, pszNewKey, ppszTemp[0]);
//                                      JobKey         NJCSHTKey      CostSheetKey
         CreateJobMaterialsUsed (hhp, ppszParams[0], ppszParams[3], ppszParams[4]);
         CopyCrewsForNewTask (hhp, pszJobKey, pszNewKey, ppszTemp [0]);
//                                 JobKey     NJCSHTKey  CostSheetKey

         /* calculate the cost sheet */
         DoAddJobCostShtCalc (hhp, pszJobKey, pszNewKey);


         /* clean up */
         ItiMemFree (hhp, pszNewKey);
         ItiFreeStrArray2 (hhp, ppszParams);
         ItiFreeStrArray (hhp, ppszTemp, usNumCol);
         } /* end of while (ItiDbGetQueryRow (hqry, &ppszTemp... loop */

      }/* end of for (i=0; ppszCBEKeys[i] != NULL... loop */
   
   DoJobCostSheetReCalc (pszJobKey);

   ItiFreeStrArray2 (hhp, ppszPNames);
   ItiMemDestroyHeap(hhp);
   return TRUE;
   } /* End of Function */







/*
 * CopyCrewsForNewTask copies the crews for a cost sheet in a catalog
 * to crew in a job.   
 *
 *
 */
BOOL EXPORT CopyCrewsForNewTask (HHEAP   hheap, 
                          PSZ     pszJobKey,
                          PSZ     pszJobCostSheetKey, 
                          PSZ     pszCostSheetKey)
   {
   static HQRY    hqry;
   static HHEAP   hhp;
   static HHEAP   hhpLoop;
   PPSZ    ppszTemp;
   PSZ     pszNewKey, pszBaseDate;
   USHORT  usNumCol, usErr, usCnt;
   BOOL    bDavisBacon;
   CHAR    aszCrewKeys[NUM_OF_KEYS+1][KEYLENGTH];
//   CHAR    szBigTemp [4096], szBigQuery [4150];

   if (!pszJobKey || !pszJobCostSheetKey || !pszCostSheetKey)
      {
      ItiErrWriteDebugMessage ("*** Missing Keys in cbest.copycbe.CopyCrewsForNewTask. ");
      return FALSE;
      }

   hhp = ItiMemCreateHeap (8192);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.copycbe.CopyCrewsForNewTask. ");
      return FALSE;
      }

   ItiDbExecSQLStatement (hhp,"select '>>> ENTERING: CopyCrewsForNewTask function' ");

   /* === Determine whether the Job uses DavisBacon or NonDavisBacon wages  */
   bDavisBacon = ItiEstIsDavisBaconJob (hhp, pszJobKey);

   pszBaseDate = ItiEstGetJobBaseDate (hhp, pszJobKey);



   /* This query obtains all of the crews associated with a costsheet */
   ItiStrCpy (szBigTemp,
            "/* cbest.copycbe.CopyCrewsForNewTask */ "
            " SELECT CostSheetCrewKey"
            " FROM CrewUsed"
            " WHERE CostSheetKey = ", sizeof szBigTemp);
   ItiStrCat (szBigTemp, pszCostSheetKey, sizeof szBigTemp);

   hqry = ItiDbExecQuery (szBigTemp, hhp, 0, 0, 0, &usNumCol, &usErr);
   if (hqry == NULL)
      {
      ItiMemDestroyHeap(hhp);
      ItiErrWriteDebugMessage ("*** ExecQuery failed in cbest.copycbe.CopyCrewsForNewTask. ");
      return FALSE;
      }


   usCnt = 0;
   aszCrewKeys[0][0]   = '\0';
   while (ItiDbGetQueryRow (hqry, &ppszTemp, &usErr))
      {
      if (usCnt < NUM_OF_KEYS)
         {
         ItiStrCpy (aszCrewKeys[usCnt], ppszTemp[0], KEYLENGTH);
         usCnt++;
         aszCrewKeys[usCnt][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("Number of Keys returned exceed array space in CopyCrewsForNewTask.");
      }

   ItiDbBeginTransaction (hhp);

   ItiDbExecSQLStatement (hhp,"select '\n *** Start of CopyCrewsForNewTask loop' ");
   ItiDbExecSQLStatement (hhp,"select '  --- \n ' ");

   /* -- Now with each cost sheet crew key. */
   usCnt = 0;
   while (aszCrewKeys[usCnt][0] != '\0')
      {
      hhpLoop = ItiMemCreateHeap (8000);
      if (hhpLoop == NULL)
         {
         ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.copycbe.CopyCrewsForNewTask. ");
         return FALSE;
         }

      /* -- Get a JobCrewKey for the current crew key to be copied. */
      pszNewKey = ItiDbGetKey (hhpLoop, "JobCrewUsed", 1);
      if (pszNewKey == NULL)
         {
         ItiErrWriteDebugMessage
            ("*** Failed to get a job crew key in cbest.copycbe.CopyCrewsForNewTask. ");
         ItiDbRollbackTransaction (hhpLoop);
         ItiDbTerminateQuery (hqry);
         ItiMemFree (hhpLoop, pszBaseDate);
         ItiMemDestroyHeap(hhpLoop);
         return FALSE;
         }

      /* -- Now with each job crew key insert new job crew used entry... */
      //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_CREW, szBigTemp, sizeof szBigTemp);
      //sprintf(szBigQuery, szBigTemp,
      //        pszJobKey, pszJobCostSheetKey,
      //        pszNewKey, pszCostSheetKey, aszCrewKeys[usCnt]);

      ItiStrCpy (szBigQuery,
                 " /* cbest.copycbe.c COPY_CREW */"                          
                 " INSERT INTO JobCrewUsed"                          
                 " (JobKey, JobCostSheetKey, JobCrewKey,"                          
                 " CrewID, Description) SELECT ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszJobKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszJobCostSheetKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszNewKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, 
                 " CostSheetCrewID, Description FROM CrewUsed"
                 " WHERE CostSheetKey = ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszCostSheetKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, " AND CostSheetCrewKey = ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, aszCrewKeys[usCnt], sizeof szBigQuery);

      ItiDbExecSQLStatement (hhpLoop, szBigQuery);


      /* -- ... next copy the crew's equipment... */
      //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_CREW_EQ, szBigTemp, sizeof szBigTemp);
      //sprintf(szBigQuery, szBigTemp,
      //        pszJobKey, pszJobCostSheetKey, pszNewKey,
      //        pszCostSheetKey, aszCrewKeys[usCnt],
      //        pszJobKey, pszBaseDate );

      ItiStrCpy (szBigTemp,
                    " /* copy the crew's equipment */"                          
                    " INSERT INTO JobCrewEquipmentUsed"                          
                      " (JobKey, JobCostSheetKey, JobCrewKey,"                          
                      " EquipmentKey, Quantity,"                          
                      " Rate, OvertimeRate)"
                   " SELECT ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobCostSheetKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszNewKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp,
                    ", CEU.EquipmentKey,"                          
                    " CEU.Quantity, ER.Rate, ER.OvertimeRate"                          
                  " FROM CrewEquipmentUsed CEU,"                          
                    " EquipmentRate ER, JobZone JZ,"                          
                    " Equipment E"                          
                  " WHERE CEU.CostSheetKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszCostSheetKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, " AND CEU.CostSheetCrewKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, aszCrewKeys[usCnt], sizeof szBigTemp);
      ItiStrCat (szBigTemp,
                 " AND CEU.EquipmentKey = E.EquipmentKey"                          
                 " AND CEU.EquipmentKey = ER.EquipmentKey"                          
                 " AND JZ.JobKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp,
                 " AND JZ.ZoneTypeKey = E.ZoneTypeKey"                          
                 " AND JZ.ZoneKey = ER.ZoneKey"
                 " AND ER.BaseDate = '", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszBaseDate, sizeof szBigTemp);
      ItiStrCat (szBigTemp, "' ", sizeof szBigTemp);
   
      ItiDbExecSQLStatement (hhpLoop, szBigTemp);
  
 

      /* -- ... then fill in any missing equipment with defaults. */
      //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_CREW_EQ_DEF, szBigTemp, sizeof szBigTemp);
      //sprintf(szBigQuery, szBigTemp,
      //        pszJobKey, pszJobCostSheetKey, pszNewKey,
      //        pszCostSheetKey, aszCrewKeys[usCnt],
      //        pszJobKey, pszBaseDate,
      //        pszJobKey, pszJobCostSheetKey, pszNewKey);

      ItiStrCpy (szBigTemp,
               " /* copy rates for the default zone */"
               " INSERT INTO JobCrewEquipmentUsed"
                 " (JobKey, JobCostSheetKey, JobCrewKey,"
                 " EquipmentKey, Quantity, Rate, OvertimeRate)"
               " SELECT ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobCostSheetKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszNewKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp, 
              ", CEU.EquipmentKey, CEU.Quantity, ER.Rate, ER.OvertimeRate"                          
             " FROM CrewEquipmentUsed CEU,"                          
               " EquipmentRate ER, JobZone JZ,"                          
               " Equipment E"                          
             " WHERE CEU.CostSheetKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszCostSheetKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp,   " AND CEU.CostSheetCrewKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, aszCrewKeys[usCnt], sizeof szBigTemp);

      ItiStrCat (szBigTemp,   " AND CEU.EquipmentKey = E.EquipmentKey"                          
             " AND CEU.EquipmentKey = ER.EquipmentKey"                          
             " AND JZ.JobKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp,   " AND JZ.ZoneTypeKey = E.ZoneTypeKey"                          
                 " AND 1 = ER.ZoneKey AND ER.BaseDate = '", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszBaseDate, sizeof szBigTemp);

      ItiStrCat (szBigTemp,   "' AND CEU.EquipmentKey NOT IN"
             " (SELECT JCEU.EquipmentKey"
             " FROM JobCrewEquipmentUsed JCEU"
             " WHERE JCEU.JobKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp,   " AND JCEU.JobCostSheetKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobCostSheetKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp,   " AND JCEU.JobCrewKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszNewKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ") ", sizeof szBigTemp);

      ItiDbExecSQLStatement (hhpLoop, szBigTemp);



      /* -- Now do the laborers for the crews used... */
      //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_CREW_LAB, szBigTemp, sizeof szBigTemp);
      ItiStrCpy (szBigQuery,
           " /* copy the equipment crew's labor */"
           " INSERT INTO JobCrewLaborUsed"
             " (JobKey, JobCostSheetKey, JobCrewKey,"
             " LaborerKey, Quantity, Rate, OvertimeRate)"
           " SELECT ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszJobKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszJobCostSheetKey, sizeof szBigQuery);
      ItiStrCat (szBigQuery, ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszNewKey, sizeof szBigQuery);

      ItiStrCat (szBigQuery, 
           ", CLU.LaborerKey, CLU.Quantity, LWAB.", sizeof szBigQuery);
      ItiStrCat (szBigQuery, bDavisBacon ? "DavisBaconRate" : "NonDavisBaconRate", sizeof szBigQuery);

      ItiStrCat (szBigQuery, ", LWAB.", sizeof szBigQuery);
      ItiStrCat (szBigQuery, bDavisBacon ? "DavisBaconOvertimeRate" : "NonDavisBaconOvertimeRate", sizeof szBigQuery);

      ItiStrCat (szBigQuery,
         " FROM CrewLaborUsed CLU, LaborerWageAndBenefits LWAB,"
              " JobZone JZ, Labor L WHERE CLU.CostSheetKey = ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszCostSheetKey, sizeof szBigQuery);

      ItiStrCat (szBigQuery, " AND CLU.CostSheetCrewKey = ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, aszCrewKeys[usCnt], sizeof szBigQuery);

      ItiStrCat (szBigQuery,
          " AND CLU.LaborerKey = L.LaborerKey"
          " AND CLU.LaborerKey = LWAB.LaborerKey"
          " AND JZ.JobKey = ", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszJobKey, sizeof szBigQuery);

      ItiStrCat (szBigQuery,
          " AND JZ.ZoneTypeKey = L.ZoneTypeKey"
          " AND JZ.ZoneKey = LWAB.ZoneKey"
          " AND LWAB.BaseDate = '", sizeof szBigQuery);
      ItiStrCat (szBigQuery, pszBaseDate, sizeof szBigQuery);
      ItiStrCat (szBigQuery, "' ", sizeof szBigQuery);

      ItiDbExecSQLStatement (hhpLoop, szBigQuery);



      /* -- ...and the laborers default rates for the crews used. */
      //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, COPY_CREW_LAB_DEF, szBigTemp, sizeof szBigTemp);
      ItiStrCpy (szBigTemp,
            " /* copy labor rates for the default zone */"
            " INSERT INTO JobCrewLaborUsed"
              " (JobKey, JobCostSheetKey, JobCrewKey,"
              " LaborerKey, Quantity, Rate, OvertimeRate)"
            " SELECT ", sizeof szBigTemp);

      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobCostSheetKey, sizeof szBigTemp);
      ItiStrCat (szBigTemp, ", ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszNewKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp, ", CLU.LaborerKey,"
                            " CLU.Quantity, LWAB.", sizeof szBigTemp);
      ItiStrCat (szBigTemp, bDavisBacon ? "DavisBaconRate" : "NonDavisBaconRate", sizeof szBigTemp);

      ItiStrCat (szBigTemp, ", LWAB.", sizeof szBigTemp);
      ItiStrCat (szBigTemp, bDavisBacon ? "DavisBaconOvertimeRate" : "NonDavisBaconOvertimeRate", sizeof szBigTemp);

      ItiStrCat (szBigTemp, 
                  " FROM CrewLaborUsed CLU, LaborerWageAndBenefits LWAB,"
                    " JobZone JZ, Labor L"
                  " WHERE CLU.CostSheetKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszCostSheetKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp, " AND CLU.CostSheetCrewKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, aszCrewKeys[usCnt], sizeof szBigTemp);

      ItiStrCat (szBigTemp, 
                     " AND CLU.LaborerKey = L.LaborerKey"
                     " AND CLU.LaborerKey = LWAB.LaborerKey"
                     " AND JZ.JobKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp, 
                     " AND JZ.ZoneTypeKey = L.ZoneTypeKey"
                     " AND 1 = LWAB.ZoneKey"
                     " AND LWAB.BaseDate = '", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszBaseDate, sizeof szBigTemp);
                         
      ItiStrCat (szBigTemp, 
                     "' AND CLU.LaborerKey NOT IN"
                     " (SELECT JCEU.LaborerKey"
                      " FROM JobCrewLaborUsed JCEU"
                      " WHERE JCEU.JobKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp, " AND JCEU.JobCostSheetKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszJobCostSheetKey, sizeof szBigTemp);

      ItiStrCat (szBigTemp,
                     "  AND JCEU.JobCrewKey = ", sizeof szBigTemp);
      ItiStrCat (szBigTemp, pszNewKey, sizeof szBigTemp); 
      ItiStrCat (szBigTemp, " ) ", sizeof szBigTemp); 

      ItiDbExecSQLStatement (hhpLoop, szBigTemp);



      ItiMemFree (hhpLoop, pszNewKey);
      if (NULL != hhpLoop)
         ItiMemDestroyHeap (hhpLoop);

      ItiDbExecSQLStatement (hhp,"select '\n                ---  ' ");

      usCnt++;
      } /* end of while loop */

   ItiDbExecSQLStatement (hhp,"select '<<< EXITING: CopyCrewsForNewTask function' ");
//   ItiDbExecSQLStatement (hhp,"select ' - ' ");

   ItiDbCommitTransaction (hhp);
   ItiMemFree (hhp, pszBaseDate);

   if (NULL != hhp)
      ItiMemDestroyHeap (hhp);

   return TRUE;
   }/* End of Function CopyCrewsForNewTask */




BOOL EXPORT CreateJobMaterialsUsed (HHEAP hheap, 
                                    PSZ   pszJobKey, 
                                    PSZ   pszJobCostSheetKey, 
                                    PSZ   pszCostSheetKey)
   {
   HQRY  hqryLocal;
   HHEAP hhp;
   PSZ  pszCount, psz;
   PPSZ ppszCJMUTemp, ppszCJMUTempDef;
   PPSZ ppszCJMUKeys, ppszCJMUValues;
//   static PPSZ ppszCJMUResults[400];
   // CHAR szBigQuery [800] = "";
   // CHAR szBigTemp2 [1000] = "";
   // CHAR szBigTemp [1000] = "";
   BOOL bRet = TRUE;
   INT    i, j;
   USHORT usNumCol, usErr, us, usCount;
   CHAR szMsgPat[80] = "cbest.copycbe.CreateJobMaterialsUsed COUNT   ";
            //          012345678901234567890123456789012345678901234567890

   if (!pszJobKey || !pszJobCostSheetKey || !pszCostSheetKey)
      {
      ItiErrWriteDebugMessage ("*** Missing Keys in cbest.copycbe.CreateJobMaterialsUsed. ");
      return FALSE;
      }

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
 //  hhp = ItiMemCreateHeap (8000);
 //  if (hhp == NULL)
 //     {
 //     ItiErrWriteDebugMessage ("Failed to create hheap in copycbe.c CreateJobMaterialsUsed.");
 //     return FALSE;
 //     }
   hhp = hheap;

   ppszCJMUKeys = ItiStrMakePPSZ2 (hhp, "JobKey", "JobCostSheetKey",
                               "CostSheetKey", NULL);
   ppszCJMUValues = ItiStrMakePPSZ2 (hhp, pszJobKey, pszJobCostSheetKey,
                                 pszCostSheetKey, NULL);

   /* ------------------------------------------------------------------- *\
    * -- Test if we have any materials to be copied.                      *
   \* ------------------------------------------------------------------- */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, MATERIALS_CPY_CNT, szBigTemp2, sizeof szBigTemp2);
   ItiStrReplaceParams (szBigTemp, szBigTemp2, sizeof szBigTemp, ppszCJMUKeys, ppszCJMUValues, 100);
   pszCount = ItiDbGetRow1Col1 (szBigTemp, hhp, 0, 0, 0);

   if (pszCount != NULL)
      {
      ItiStrToUSHORT (pszCount, &usCount);

      ItiStrCat (szMsgPat, pszCount, sizeof szMsgPat);
      ItiErrWriteDebugMessage (szMsgPat); 

      //ItiMemFree (hhp, pszCount); 
      }

   if ((NULL == pszCount) || (usCount == 0))
      {
      ItiErrWriteDebugMessage
         ("  cbest.copycbe.CreateJobMaterialsUsed found an empty list. ");
      ItiFreeStrArray2 (hhp, ppszCJMUKeys);
      ItiFreeStrArray2 (hhp, ppszCJMUValues);
   //   if (NULL != hhp)
   //      ItiMemDestroyHeap (hhp);

      return FALSE;
      }
   //else
   //   ItiMemFree (hhp, pszCount); 



   /* ------------------------------------------------------------------- *\
    * Do the zones.                                                       *
   \* ------------------------------------------------------------------- */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, MATERIALS, szBigTemp2, sizeof szBigTemp2);
   ItiStrReplaceParams (szBigTemp, szBigTemp2, sizeof szBigTemp, ppszCJMUKeys, ppszCJMUValues, 100);
   hqryLocal = ItiDbExecQuery (szBigTemp, hhp, hmodMe, ITIRID_CALC, MATERIALS, &usNumCol, &usErr);
   if (hqryLocal == NULL)
      {
      ItiErrWriteDebugMessage ("ExecQuery failed in copycbe.c CreateJobMaterialsUsed.");
      return FALSE;
      }
   i = 0;
   while (ItiDbGetQueryRow (hqryLocal, &ppszCJMUTemp, &usErr))
      {
      i++;
      ppszCJMUResults[i] = ItiStrMakePPSZ2 (hhp, ppszCJMUTemp[0], ppszCJMUTemp[1], 
                     ppszCJMUTemp[2], ppszCJMUTemp[3], ppszCJMUTemp[4], ppszCJMUTemp[5], NULL);

      // need to remove any possible commas in the number formats.
      us = 1 + ItiStrLen (ppszCJMUTemp[3]);
      ItiStrCpy(ppszCJMUResults[i][3] ,(ItiExtract (ppszCJMUTemp[3], ",$")), us);
      us = 1 + ItiStrLen (ppszCJMUTemp[5]);
      ItiStrCpy(ppszCJMUResults[i][5] ,(ItiExtract (ppszCJMUTemp[5], ",$")), us);

      ItiFreeStrArray (hhp, ppszCJMUTemp, usNumCol);
      } /* end of while loop */

   j = i;
   while (i != 0)
      {
      ItiStrCpy (szBigQuery,
         "/* cbest.copycbe.CreateJobMaterialsUsed */ "
         "INSERT INTO JobMaterialUsed"
         " (JobKey, JobCostSheetKey, MaterialKey, Quantity, Unit, UnitPrice)"
         " VALUES ( ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][0], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][1], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][2], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][3], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][4], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][5], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ") ", sizeof szBigQuery);

      ItiDbExecSQLStatement (hhp, szBigQuery);
      i--;
      } /* end of while loop */

   while (j != 0)
      {
      ItiFreeStrArray2 (hhp, ppszCJMUResults[j]);
      j--;
      }



   /* ------------------------------------------------------------------- *\
    * Do the default zones.                                               *
   \* ------------------------------------------------------------------- */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, MATERIALS_DEFZONE, szBigTemp2, sizeof szBigTemp2);
   ItiStrReplaceParams (szBigTemp, szBigTemp2, sizeof szBigTemp, ppszCJMUKeys, ppszCJMUValues, 999);
   hqryLocal = ItiDbExecQuery (szBigTemp, hhp, hmodMe, ITIRID_CALC, MATERIALS_DEFZONE, &usNumCol, &usErr);
   if (hqryLocal == NULL)
      {
      ItiErrWriteDebugMessage ("ExecQuery failed in copycbe.c CreateJobMaterialsUsed.");
      return FALSE;
      }
   i = 0;
   while (ItiDbGetQueryRow (hqryLocal, &ppszCJMUTempDef, &usErr))
      {
      i++;
      ppszCJMUResults[i] = ItiStrMakePPSZ2(hhp,
                                       ppszCJMUTempDef[0], ppszCJMUTempDef[1], 
                                       ppszCJMUTempDef[2], ppszCJMUTempDef[3],
                                       ppszCJMUTempDef[4], ppszCJMUTempDef[5], NULL);

      // need to remove any possible commas in the number formats.
      us = 1 + ItiStrLen (ppszCJMUTempDef[3]);
      ItiStrCpy(ppszCJMUResults[i][3] ,(ItiExtract (ppszCJMUTempDef[3], ",$")), us);
      us = 1 + ItiStrLen (ppszCJMUTempDef[5]);
      ItiStrCpy(ppszCJMUResults[i][5] ,(ItiExtract (ppszCJMUTempDef[5], ",$")), us);

      ItiFreeStrArray (hhp, ppszCJMUTempDef, usNumCol);
      } /* end of while loop */

   j = i;
   while (i != 0)
      {
      ItiStrCpy (szBigQuery,
         "/* cbest.copycbe.CreateJobMaterialsUsed */ "
         "INSERT INTO JobMaterialUsed"
         " (JobKey, JobCostSheetKey, MaterialKey, Quantity, Unit, UnitPrice)"
         " VALUES ( ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][0], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][1], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][2], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][3], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][4], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ", ", sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ppszCJMUResults[i][5], sizeof szBigQuery);
      ItiStrCat (szBigQuery,  ") ", sizeof szBigQuery);

      ItiDbExecSQLStatement (hhp, szBigQuery);
      i--;
      } /* end of while loop */

   while (j != 0)
      {
      ItiFreeStrArray2 (hhp, ppszCJMUResults[j]);
      j--;
      }

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiFreeStrArray2 (hhp, ppszCJMUKeys);
   ItiFreeStrArray2 (hhp, ppszCJMUValues);
   //if (NULL != hhp)
   //   ItiMemDestroyHeap (hhp);

   return bRet;
   }/* End of Function CreateJobMaterialsUsed */




/* ------------------------------------------------------------- */
/* --   FUNCTION: CopyCostShtToAJobCostSht                       */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobItemKey                                  */
/* --             pszJobCostSheetKey                             */
/* --             pszJobItemCostBasedEstimateKey                 */
/* --             pszCostBasedEstimateKey                        */
/* --             pszCostSheetKey                                */
/* --             pszTaskKey                                     */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Copy a CostSheet to a JobCostSheets                     */
/* --                                                            */
/* -- RETURN: TRUE if okay                                       */
/* --         FALSE if not                                       */
/* ------------------------------------------------------------- */
BOOL EXPORT CopyCostShtToAJobCostSht (HHEAP  hheap,
                                      PSZ    pszJobKey,
                                      PSZ    pszJobItemKey,
                                      PSZ    pszJobCostSheetKey,
                                      PSZ    pszJobItemCostBasedEstimateKey,
                                      PSZ    pszCostBasedEstimateKey,
                                      PSZ    pszCostSheetKey,
                                      PSZ    pszTaskKey)
   {
   BOOL bRC;
   USHORT usCol;
   PSZ  *ppsz = NULL;
   PSZ  *ppsz2 = NULL;

   CHAR szProductionRate[32] = "";
   CHAR szProductionGranularity[32] = "";
   CHAR szDefaultHoursPerDay[8] = "";
   CHAR szMarkUpPct[8] = "";
   CHAR szLaborOverheadPct[8] = "";
   CHAR szEquipmentOverheadPct[8] = "";
   CHAR szMaterialsOverheadPct[8] = "";
   CHAR szCostSheetID[16] = "";
   CHAR szTaskID[18] = "";
   CHAR szUnit[32] = "";
   CHAR szQPIU[20] = "";
//   CHAR szDesc[256] = "";
//   CHAR szDescription[256] = "";
//   CHAR szCStoJCSTemp  [256]  = "";


   ItiStrCpy (szBigTemp2,
      "/* cbest.copycbe.CopyCostShtToAJobCostSht */"
      " INSERT INTO JobCostSheet"
      " (JobKey, JobCostSheetKey, JobItemKey, JobItemCostBasedEstimateKey,"
      " TaskID, TaskDescription, CostSheetID, Description, ProductionRate,"
      " Duration, ProductionGranularity, HoursPerDay, OvertimeHoursPerDay,"
      " LaborDailyCost, EquipmentDailyCost, MaterialsUnitCost, MarkUpPct,"
      " LaborOverheadPct, EquipmentOverheadPct, MaterialsOverheadPct,"
      " QuantityPerItemUnit, Unit, UnitPrice)"
      " VALUES ( ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, ", ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobCostSheetKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, ", ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobItemKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, ", ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobItemCostBasedEstimateKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2,
      ",'x','x','x','x',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0) ", sizeof szBigTemp2);


   if (ItiDbExecSQLStatement (hheap, szBigTemp2))
      {
      ItiErrWriteDebugMessage ("\n *** Failed ExecSQLStatement: ");
      ItiErrWriteDebugMessage (szBigTemp2);
      return FALSE;
      }
   //ppsz = ItiDbGetRow1(szCStoJCSTemp, hheap, 0, 0, 0, &usCol);
   //if (ppsz != NULL)
   //   ItiFreeStrArray (hheap, ppsz, usCol);
   //   //ItiMemFree (hheap, ppsz);

    ItiStrCpy (szBigTemp2,
       "/* cbest.copycbe.CopyCostShtToAJobCostSht */ "
       "SELECT ProductionRate, ProductionGranularity, DefaultHoursPerDay"
       ", MaterialsOverheadPct, EquipmentOverheadPct, LaborOverheadPct "
       ", MarkUpPct, CostSheetID, Description "
       "FROM CostSheet WHERE CostSheetKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszCostSheetKey, sizeof szBigTemp2);

   ppsz = ItiDbGetRow1(szBigTemp2, hheap, 0, 0, 0, &usCol);
	if (ppsz != NULL)
      {
         /* looks clumbsy but sprintf had pointer/segment */
         /* problems with direct usage of the ppsz.       */

      ItiStrCpy(szProductionRate,        ppsz[0], sizeof szProductionRate);
      ItiStrCpy(szProductionGranularity, ppsz[1], sizeof szProductionGranularity);
      ItiStrCpy(szDefaultHoursPerDay,    ppsz[2], sizeof szDefaultHoursPerDay);
      ItiStrCpy(szMaterialsOverheadPct,  ppsz[3], sizeof szMaterialsOverheadPct);
      ItiStrCpy(szEquipmentOverheadPct,  ppsz[4], sizeof szEquipmentOverheadPct);
      ItiStrCpy(szLaborOverheadPct,      ppsz[5], sizeof szLaborOverheadPct);
      ItiStrCpy(szMarkUpPct,             ppsz[6], sizeof szMarkUpPct);
      ItiStrCpy(szCostSheetID,           ppsz[7], sizeof szCostSheetID);

      /* remove single quotes from the description. */
      ItiStrCpy(szDescription, (ItiExtract (ppsz[8], ",'")), sizeof szDescription);
      // old line: ItiStrCpy(szDescription, ppsz[8], sizeof szDescription);

      ItiStrCpy (szBigTemp2,
         "/* cbest.copycbe.CopyCostShtToAJobCostSht */ "
         "UPDATE JobCostSheet "
         "SET ProductionRate = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szProductionRate, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,ProductionGranularity = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szProductionGranularity, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,HoursPerDay = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szDefaultHoursPerDay, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,MarkUpPct = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szMarkUpPct, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,LaborOverheadPct = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szLaborOverheadPct, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,EquipmentOverheadPct = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szEquipmentOverheadPct, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,MaterialsOverheadPct = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szMaterialsOverheadPct, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " ,CostSheetID = '", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szCostSheetID, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, "' ,Description = '", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, szDescription, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, "' WHERE JobKey = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, pszJobKey, sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, " AND JobCostSheetKey = ", sizeof szBigTemp2);
      ItiStrCat (szBigTemp2, pszJobCostSheetKey, sizeof szBigTemp2);

      if (ItiDbExecSQLStatement (hheap, szBigTemp2) )
         {
         ItiErrWriteDebugMessage ("\n *** Failed ExecSQLStatement: ");
         ItiErrWriteDebugMessage (szBigTemp2);
         return FALSE;
         }
      // ppsz2 = ItiDbGetRow1(szBigTemp2, hheap, 0, 0, 0, &usCol);
      // if (ppsz2 != NULL)
      //    {
      //    ItiFreeStrArray (hheap, ppsz2, usCol);
      //    // ItiMemFree (hheap, ppsz2);
      //    }

      /* Now recover memory. */
      ItiFreeStrArray (hheap, ppsz, usCol);
      }
   else
      { /* -- No data was retrieved, now what? */
      ItiErrWriteDebugMessage
         ("No Cost Sheet data was retrived in CopyCostShtToAJobCostSht to copy.");
      return FALSE;
      }




   /* -- Now get the info from the Task. */ 
   ItiStrCpy (szBigTemp2,
      "/* cbest.copycbe.CopyCostShtToAJobCostSht */ "
      "SELECT TaskID, Description, Unit, QuantityPerItemUnit "
      "FROM Task "
      "WHERE CostSheetKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszCostSheetKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, " AND CostBasedEstimateKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszCostBasedEstimateKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, " AND TaskKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszTaskKey, sizeof szBigTemp2);

   ppsz = ItiDbGetRow1(szBigTemp2, hheap, 0, 0, 0, &usCol);
	if (ppsz != NULL)
      {
      ItiStrCpy(szTaskID, ppsz[0], sizeof szTaskID);
      ItiStrCpy(szDesc,   ppsz[1], sizeof szDesc);
      ItiStrCpy(szUnit,   ppsz[2], sizeof szUnit);
      ItiStrCpy(szQPIU,   ppsz[3], sizeof szQPIU);

      ItiFreeStrArray (hheap, ppsz, usCol);
      }

   ItiStrCpy (szBigTemp2,
      "/* cbest.copycbe.CopyCostShtToAJobCostSht */ "
      "UPDATE JobCostSheet "
      "SET TaskID = '", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, szTaskID, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, "', TaskDescription = '", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, szDesc, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, "', Unit = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, szUnit, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, ", QuantityPerItemUnit = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, szQPIU, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, " WHERE JobKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobKey, sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, " AND JobCostSheetKey = ", sizeof szBigTemp2);
   ItiStrCat (szBigTemp2, pszJobCostSheetKey, sizeof szBigTemp2);

   if (ItiDbExecSQLStatement (hheap, szBigTemp2) )
      {
      ItiErrWriteDebugMessage ("\n *** Failed ExecSQLStatement: ");
      ItiErrWriteDebugMessage (szBigTemp2);
      return FALSE;
      }
   // ppsz = ItiDbGetRow1(szBigTemp2, hheap, 0, 0, 0, &usCol);
   // if (ppsz != NULL)
   //    ItiFreeStrArray (hheap, ppsz, usCol);
   //    //ItiMemFree (hheap, ppsz);



   /* Copy Materials */
   bRC = CreateJobMaterialsUsed
            (hheap, pszJobKey, pszJobCostSheetKey, pszCostSheetKey);

   /* Copy Crews */
   CopyCrewsForNewTask
            (hheap, pszJobKey, pszJobCostSheetKey, pszCostSheetKey);

   return bRC;
   }/* End of Function CopyCostShtToAJobCostSht */







/* ------------------------------------------------------------- */
/* --   FUNCTION: CopyCBEToJobCBE                                */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszCostBasedEstimateKey                        */
/* --             pszJobItemCostBasedEstimateKey                 */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    The given CBE is copied into a new Job CBE              */
/* --    along with all the crews and materials into             */
/* --    new JobCostSheets, etc.                                 */
/* --                                                            */
/* -- RETURN: TRUE if okay                                       */
/* --         FALSE if not                                       */
/* ------------------------------------------------------------- */
BOOL EXPORT CopyCBEToJobCBE (HHEAP hheap,
                      PSZ   pszJobKey,
                      PSZ   pszJobItemKey,
                      PSZ   pszCostBasedEstimateKey,
                      PSZ   pszJobItemCostBasedEstimateKey)
   {
   BOOL bRet;
   HQRY hqry;
   PSZ  *ppszCSKeys = NULL;
   PSZ  pszCostSheetKey = NULL;
   PSZ  pszTaskKey = NULL;
   PSZ  pszNewKey = NULL;
   CHAR szCostSheetKey[16] = "";
   CHAR szTaskKey[16] = "";
   CHAR szNewKey[16] = "";
//   CHAR szTemp[250] = "";
   USHORT  usCnt;
   USHORT  uErr, uCols;
//   CHAR   aszCSKeys[NUM_OF_KEYS+1][KEYLENGTH];
//   CHAR   aszTaskKeys[NUM_OF_KEYS+1][KEYLENGTH];


   /* -- Test the given keys. */
   if ((NULL==pszJobKey) || (NULL==pszJobItemKey)
       ||(NULL==pszJobItemCostBasedEstimateKey)
       ||(NULL==pszCostBasedEstimateKey))
      {
      DosBeep (1000, 10); DosBeep (1502, 22);
      ItiErrWriteDebugMessage
         ("\n ERROR: Missing keys for cbest.copycbe.CopyCBEToJobCBE.");
      ItiErrWriteDebugMessage(" JK:  JIK:  JICBEK:  CBEK:  ");
      ItiErrWriteDebugMessage(pszJobKey);
      ItiErrWriteDebugMessage(pszJobItemKey);
      ItiErrWriteDebugMessage(pszJobItemCostBasedEstimateKey);
      ItiErrWriteDebugMessage(pszCostBasedEstimateKey);
      ItiErrWriteDebugMessage(" .end of key list. \n ");
      return FALSE;
      }

   /* -- This query obtains all of the Tasks   */
   /* -- associated with a CostBasedEstimate.  */
   ItiStrCpy (szTemp,
            "/* cbest.copycbe.CopyCBEToJobCBE */"
            " SELECT CostSheetKey, TaskKey "
            " FROM Task"
            " WHERE CostBasedEstimateKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszCostBasedEstimateKey, sizeof szTemp);

   if (!(hqry = ItiDbExecQuery(szTemp, hheap, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }

   aszCSKeys[0][0]   = '\0';
   aszTaskKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszCSKeys, &uErr)) 
      {
      if (usCnt < NUM_OF_KEYS)
         {
         ItiStrCpy (aszCSKeys[usCnt], ppszCSKeys[0], KEYLENGTH);
         ItiStrCpy (aszTaskKeys[usCnt], ppszCSKeys[1], KEYLENGTH);
         usCnt++;
         aszCSKeys[usCnt][0] = '\0';
         aszTaskKeys[usCnt][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("Number of Keys returned exceed array space in CopyCBEToJobCBE.");
      }

   /* -- For each Cost Sheet key... */
   for (usCnt=0; aszCSKeys[usCnt][0] ; usCnt++)
      {
      /* ... generate a new job cost sheet key... */
      pszNewKey = ItiDbGetKey (hheap, "JobCostSheet", 1);
      if (pszNewKey == NULL)
         {
         return FALSE;
         }
      else
         {
         ItiStrCpy (szNewKey, pszNewKey, sizeof szNewKey);
         ItiMemFree (hheap, pszNewKey);
         }

      /* .. and setup a job cost sheet. */
      CopyCostShtToAJobCostSht
         (hheap, pszJobKey, pszJobItemKey, szNewKey,
         pszJobItemCostBasedEstimateKey,
         pszCostBasedEstimateKey,
         aszCSKeys[usCnt],
         aszTaskKeys[usCnt]);

      
      }/* end of for loop */

   return bRet;
   }/* End of Function CopyCBEToJobCBE */
                          






/* ------------------------------------------------------------- */
/* --   FUNCTION: CopyCBEForJobItem                              */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobItemKey                                  */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    If there exists CostBasedEstimates associated with the  */
/* --    given StandardItemKey, then those CBEs are copied into  */
/* --    a new JobCostBasedEstimate entry, along with all the    */
/* --    crews and materials into new JobCostSheets, etc.        */
/* --                                                            */
/* -- RETURN: TRUE if okay                                       */
/* --         FALSE if not                                       */
/* ------------------------------------------------------------- */
BOOL EXPORT CopyCBEForJobItem (HHEAP hheap, 
                               PSZ   pszJobKey,
                               PSZ   pszJobItemKey, 
                               PSZ   pszStandardItemKey)
   {
   HQRY    hqryLocal     = NULL;
   HHEAP   hhpCBEJI      = NULL;
   PSZ     *ppszCBECols  = NULL;
   PSZ     pszNewKeyCopy = NULL;
   PSZ     pszCBEKey     = NULL;
   PSZ     pszTemp       = NULL;
   CHAR    szCBEKey    [KEYLENGTH] = "";
   CHAR    szCBEID     [KEYLENGTH] = "";
   CHAR    szCBEActive [KEYLENGTH] = "";
   USHORT  i, uCols, uErr;
   CHAR    szUT[4] = "";
   PPSZ ppszResults[NUM_OF_KEYS+1];
//   CHAR    aszCBECols[NUM_OF_CBE_KEYS+1][4][KEYLENGTH];
//   CHAR    aszCBEDesc[NUM_OF_CBE_KEYS+1][256];
//   CHAR    szCBEDesc    [256] = "";
//   CHAR    szTemp       [400] = "";
//   CHAR    szTemp1      [400] = "";
//   CHAR    szNewKeyCopy [16] = "";

     if (pglobals->bUseMetric)
        szUT[0] = '1';
     else
        szUT[0] = '0';

     szUT[1] = '\0';

   /* -- Test the given keys. */
   if ((NULL==pszJobKey)||(NULL==pszJobItemKey)||(NULL==pszStandardItemKey))
      {
      DosBeep (1000, 10); DosBeep (1502, 22);
      ItiErrWriteDebugMessage
         ("ERROR: Missing keys for cbest.copycbe.CopyCBEForJobItem.");
      return FALSE;
      }

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhpCBEJI = ItiMemCreateHeap (8090);
   if (hhpCBEJI == NULL)
      {
      ItiErrWriteDebugMessage
         ("Failed to create hhpCBEJI in copycbe.c CopyCBEForJobItem.");
      return FALSE;
      }


#define CBEK   0
#define CBEID  1
#define ACTV   2
#define DESC   3
   /* -- This query obtains all of the CostBasedEstimates  */
   /* -- association with a Std Item.                      */
   ItiErrWriteDebugMessage(" -- Getting CBEs associated with a Std Item: ");

   ItiMbQueryQueryText
      (hmodMe, ITIRID_CALC, CBE_KEYS, szTemp, sizeof szTemp);
   ItiStrCat (szTemp, szUT, sizeof szTemp);

   ItiMbQueryQueryText
      (hmodMe, ITIRID_CALC, CBE_KEYS_SUFFIX, szTemp2, sizeof szTemp2);
   ItiStrCat (szTemp, szTemp2, sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);

   ItiMbQueryQueryText
      (hmodMe, ITIRID_CALC, CBE_KEYS_2, szTemp1, sizeof szTemp1);
   ItiStrCat (szTemp, szTemp1, sizeof szTemp);
   ItiStrCat (szTemp, szUT, sizeof szTemp);

   ItiMbQueryQueryText
      (hmodMe, ITIRID_CALC, CBE_KEYS_2_SUFFIX, szTemp3, sizeof szTemp3);
   ItiStrCat (szTemp, szTemp3, sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);

   hqryLocal = ItiDbExecQuery
             (szTemp, hhpCBEJI, hmodMe, ITIRID_CALC, CBE_KEYS, &uCols, &uErr);
   if (hqryLocal == NULL)
      {
      ItiErrWriteDebugMessage
           ("ExecQuery failed in cbest.copycbe.CopyCBEForJobItem.");
      return FALSE;
      }

   i = 0;
   while (ItiDbGetQueryRow (hqryLocal, &ppszCBECols, &uErr))
      {
      i++;
      ppszResults[i] = ItiStrMakePPSZ2
                          (hhpCBEJI, ppszCBECols[CBEK], ppszCBECols[CBEID], 
                           ppszCBECols[ACTV], ppszCBECols[DESC],  NULL);
   
      ItiFreeStrArray (hhpCBEJI, ppszCBECols, uCols);
      } /* end of while loop */

   if (i == 0)
      ItiErrWriteDebugMessage(" -- No CBEs Found!");
   

while (i != 0)
   {
   /* -- For each CBE associated with the given StandardItem, get a new  */
   /* -- key for the JobCostBasedEstimate and insert the row into the    */
   /* -- JobCostBasedEstimate table...                                   */
      pszNewKeyCopy = ItiDbGetKey (hhpCBEJI, "JobCostBasedEstimate", 1);
      if (pszNewKeyCopy == NULL)
         {
         return FALSE;
         }
      else
         {
         ItiStrCpy (szNewKeyCopy, pszNewKeyCopy, sizeof szNewKeyCopy);
         ItiMemFree (hhpCBEJI, pszNewKeyCopy);
         }

      ItiStrCpy (szTemp1,
              "/* cbest.copycbe.CopyCBEForJobItem */"
              " INSERT INTO JobCostBasedEstimate"
              " (JobKey, JobItemKey,"
              " JobItemCostBasedEstimateKey,"
              " CostBasedEstimateID, Description, Active)"
              " VALUES (", sizeof szTemp1);
      ItiStrCat (szTemp1,  pszJobKey, sizeof szTemp1);
      ItiStrCat (szTemp1,  ", ", sizeof szTemp1);
      ItiStrCat (szTemp1,  pszJobItemKey, sizeof szTemp1);
      ItiStrCat (szTemp1,  ", ", sizeof szTemp1);
      ItiStrCat (szTemp1,  szNewKeyCopy, sizeof szTemp1);
      ItiStrCat (szTemp1,  ", '", sizeof szTemp1);
      ItiStrCat (szTemp1,  ppszResults[i][CBEID], sizeof szTemp1);
      ItiStrCat (szTemp1,  "', '", sizeof szTemp1);
      ItiStrCat (szTemp1,  ppszResults[i][DESC], sizeof szTemp1);
      ItiStrCat (szTemp1,  "', ", sizeof szTemp1);
      ItiStrCat (szTemp1,  ppszResults[i][ACTV], sizeof szTemp1);
      ItiStrCat (szTemp1,  ") ", sizeof szTemp1);

      if (ItiDbExecSQLStatement (hhpCBEJI, szTemp1))
         {
         return FALSE;
         }
      else /* ...Now that the Job CBE is in place, copy the "tasks." */
         {
         CopyCBEToJobCBE(hhpCBEJI,
                         pszJobKey,
                         pszJobItemKey,
                         ppszResults[i][CBEK],    /* from CBE Key   */
                         szNewKeyCopy);               /*  to  JICBE Key */
         }

   i--;
   } /* end of while loop */

//      ItiDbCommitTransaction (hhpCBEJI);
  }/* End of Function CopyCBEForJobItem */

