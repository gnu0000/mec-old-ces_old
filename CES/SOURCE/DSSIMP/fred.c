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
 * Fred.c
 *
 * This stuff is responsible for populating the ProjectItem and
 * JobItemProjectItem tables so that the Job Item to proj item
 * window has a chance to work.
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\itifmt.h"
#include <stdio.h>
#include <stdlib.h>



#define TMPSIZE 1024
char    sz [TMPSIZE];


PSZ  MapJobItemProjItemErr = NULL;

char *szErrorStr[] = {/* 0*/ "None",
                      /* 1*/ "Unable to create temp Heap",
                      /* 2*/ "Unable to get JobItemKey (s)",
                      /* 3*/ "Unable to get Project & Category key (s)",
                      /* 4*/ "Unable to create ProjectItem Entry",
                      /* 5*/ "Unable to create JobItemProjectItem Entry",
                      /* 6*/ "Unable to create a Unique PCN",
                      /* 7*/ "",
                      /* 8*/ "",
                      /* 9*/ "",
                      /*10*/ ""};


/***************************/
/*                         */
/*  Select Queries         */
/*                         */
/***************************/

/*--- Gets JobItemKey (s) | Needs JobKey ---*/
char szJI[] = " SELECT JobItemKey, UnitPrice"
              " FROM JobItem"
              " WHERE JobKey = %s";
#define IDX_JI 0


/*--- Gets PCN, Category (s) | Needs JobKey ---*/
char szPC[] = " SELECT PC.ProjectControlNumber, PC.Category"
              " FROM JobProject JP, ProjectCategory PC"
              " WHERE JP.JobKey = %s"
              " AND JP.ProjectControlNumber = PC.ProjectControlNumber";
#define IDX_P 0
#define IDX_C 1


/*--- Gets SequenceNumber (s) | Needs JobKey, JobItemKey, PCN, Category ---*/
char szJIPI[] = " SELECT SequenceNumber"
                " FROM   JobItemProjectItem JIPI"
                " WHERE  JIPI.JobKey = %s"
                " AND    JIPI.JobItemKey = %s"
                " AND    JIPI.ProjectControlNumber = '%s'"
                " AND    JIPI.Category = '%s'";
#define IDX_S 0



/*--- Gets SequenceNumber (s) | Needs JobKey, JobItemKey, PCN, Category ---*/
char szJIPIAndPI[] = " SELECT JIPI.SequenceNumber"
                     " FROM   JobItemProjectItem JIPI, ProjectItem PI"
                     " WHERE  JIPI.JobKey = %s"
                     " AND    JIPI.JobItemKey = %s"
                     " AND    JIPI.ProjectControlNumber = '%s'"
                     " AND    JIPI.Category = '%s'"
                     " AND    PI.ProjectControlNumber = JIPI.ProjectControlNumber"
                     " AND    PI.Category       = JIPI.Category"
                     " AND    PI.SequenceNumber = JIPI.SequenceNumber";


/*--- Get a particular project item | Needs PCN, Category, SequenceNumber ---*/
char szPI[] = " SELECT StandardItemKey"
              " FROM   ProjectItem"
              " WHERE  ProjectControlNumber = '%s'"
              " AND    Category = '%s'"
              " AND    SequenceNumber = '%s'";


/*--- Gets Max SequenceNumber | Needs ProjectItemKey, Category ---*/
char szMaxSeq[] = " SELECT MAX(SequenceNumber)"
                  " FROM ProjectItem"
                  " WHERE ProjectControlNumber = '%s'"
                  " AND Category = '%s'";



/*--- Sees if there are any (JobItemProjectItem & Project Item) for a given JobItem ---*/
char szAnyPI[] = " SELECT JIPI.SequenceNumber"
                 " FROM   JobItemProjectItem JIPI, ProjectItem PI"
                 " WHERE  JIPI.JobKey = %s"
                 " AND    JIPI.JobItemKey = %s"
                 " AND    PI.ProjectControlNumber = JIPI.ProjectControlNumber"
                 " AND    PI.Category             = JIPI.Category"
                 " AND    PI.SequenceNumber       = JIPI.SequenceNumber";


/***************************/
/*                         */
/*  Update Queries         */
/*                         */
/***************************/

/*--- Makes Project Item | Needs PCN, Category, SequenceNumber, JobKey, JobItemKey ---*/
char szMakePI[] = " INSERT INTO ProjectItem"
                  " (ProjectControlNumber, Category, SequenceNumber,"
                  " Quantity, UnitPrice, SupplementalDescription,"
                  " ExtendedAmount, StandardItemKey)"
                  " SELECT '%s', '%s', '%s', %s, JI.UnitPrice,"
                  " JI.SupplementalDescription,"
                  " JI.UnitPrice * %s, JI.StandardItemKey"
                  " FROM JobItem JI"
                  " WHERE JI.JobKey = %s"
                  " AND JI.JobItemKey = %s";






/*--- Makes JobItemProjectItem | Needs JobKey, JobItemKey, PCN, Category, SequenceNumber ---*/
char szMakeJIPI[] = " INSERT INTO JobItemProjectItem"
                    " (JobKey,JobItemKey,ProjectControlNumber,"
                    " Category,SequenceNumber)"
                    " VALUES (%s, %s, '%s', '%s', '%s')";



USHORT MapCleanup (HHEAP hheap, USHORT uErr, PPSZ ppszJIKeys, PPSZ ppszPCKeys)
   {
   if (ppszJIKeys)
      ItiDbFreeZStrArray (hheap, ppszJIKeys);
   if (ppszPCKeys)
      ItiDbFreeZStrArray (hheap, ppszPCKeys);

   ItiMemDestroyHeap (hheap);
   MapJobItemProjItemErr = szErrorStr[uErr];
   return uErr;
   }




                     
BOOL DoItUp (HHEAP hheap, PSZ psz)
   {
   if (ItiDbExecSQLStatement (hheap, psz))
      {
      ItiDbRollbackTransaction (hheap);
      return FALSE;
      }
   ItiDbCommitTransaction (hheap);
   return TRUE;
   }



PSZ GenerateUniqueSeq (HHEAP hheap, PSZ pszPCN, PSZ pszCategory, PSZ pszMaxSeq)
   {
   PSZ   psz;

   sprintf (sz, szMaxSeq, pszPCN, pszCategory);

   if (!(psz = ItiDbGetRow1Col1(sz, hheap, 0, 0, 0)))
		psz = ItiStrDup (hheap, "0000");
	else
   if (!*psz)
		{
      ItiMemFree (hheap, psz);
		psz = ItiStrDup (hheap, "0000");
		}
   sprintf (pszMaxSeq, "%.4u", atoi (psz) + 5);
   ItiMemFree (hheap, psz);

   return pszMaxSeq;
   }




/*
 * This fn create a project item entry
 * and then a JobItemProjectItemEntry
 */
USHORT MakePIAndJIPI (HHEAP  hheap,
                      PSZ    pszJobKey,
                      PPSZ   ppszJIKeys,
                      PPSZ   ppszPCKeys,
                      USHORT uPCIndex,
                      USHORT uJIIndex)
   {
   char szSeq [40];
   BOOL bIsFirstOne, bIsDuplicate;

   GenerateUniqueSeq (hheap, ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_P),
                      ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_C), szSeq);
   if (!*szSeq)
      return 6;

   /*--- Create a project item ---*/
     /*-- First make sure this isn't a duplicate via szPI  --*/
     /*-- This should not really be needed if the seq # is --*/
     /*-- truly unique                                     --*/
   sprintf (sz, szPI, ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_P),
            ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_C), szSeq);
   bIsDuplicate = !!ItiDbGetRow1Col1 (sz, hheap, 0, 0, 0);

   if (!bIsDuplicate)
      {
      /*--- see if its the 1st one (1st proj item gets full quan) ---*/
      sprintf (sz, szAnyPI, pszJobKey, ItiDbGetZStr (ppszJIKeys [uJIIndex], IDX_JI));
      bIsFirstOne = !ItiDbGetRow1Col1 (sz, hheap, 0, 0, 0);

      sprintf (sz, szMakePI, ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_P),
                             ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_C),
                             szSeq,
                             (bIsFirstOne ? "JI.Quantity" : "0"),
                             (bIsFirstOne ? "JI.Quantity" : "0"),
                             pszJobKey,
                             ItiDbGetZStr (ppszJIKeys [uJIIndex], IDX_JI));
      if (!DoItUp (hheap, sz))
         return 4;
      }

   /*--- Create a Job item project item ---*/

   /*--- If 1st one pass it the full quan ---*/
   sprintf (sz, szMakeJIPI, pszJobKey,
                            ItiDbGetZStr (ppszJIKeys [uJIIndex], 0),
                            ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_P),
                            ItiDbGetZStr (ppszPCKeys [uPCIndex], IDX_C),
                            szSeq);
   if (!DoItUp (hheap, sz))
      return 5;

   return 0;
   }




/* This does not yet clean up unused entrys in the JIPI & PI tables yet
 *
 * when bScratch is false:1> the mapping is done
 *
 * mapping means:
 *
 * Enum Job Items In Job
 *   Enum Projects in Job
 *     Enum Categories in project
 *       if (! Is JobItem ProjItem Table Entry for this job item & project?)
 *         {
 *         Create JobItem ProjItem Table Entry
 *         if (! JobItem exists)
 *           Create Project Item
 *         }
 *
 * (I.E. fully populate the JobItem/ProjectItem window)
 *
 */

USHORT MapJobItemProjItem (PSZ pszJobKey)
   {
   USHORT uPCIndex,
          uJIIndex,
          uRet;
   HHEAP  hheap, hhp;
   PPSZ   ppszJIKeys = NULL,
          ppszPCKeys = NULL;
   USHORT puJIKeys [] = {1,0};
   USHORT puPCKeys [] = {1,2,0};


   /*--- Things could get hairy, so make sure we'll have sufficient mem ---*/
   if (!(hheap = ItiMemCreateHeap (MAX_HEAP_SIZE)))
      return MapCleanup (hheap, 1, NULL, NULL);
                
   /*--- Get Job Item Keys ---*/
   sprintf (sz, szJI, pszJobKey);
   if (ItiDbBuildSelectedKeyArray2 (hheap, 0, sz, &ppszJIKeys, puJIKeys))
      return MapCleanup (hheap, 2, NULL, NULL);

   /*--- Get Project & Category Keys ---*/
   sprintf (sz, szPC, pszJobKey);
   if (ItiDbBuildSelectedKeyArray2 (hheap, 0, sz, &ppszPCKeys, puPCKeys))
      return MapCleanup (hheap, 3, ppszJIKeys, NULL);

   /*--- loop through all project/Category combinations ---*/
   for (uPCIndex = 0; ppszPCKeys[uPCIndex]; uPCIndex++)
      {
      // CreateHeap here?
      hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
      for (uJIIndex = 0; ppszJIKeys[uJIIndex]; uJIIndex++)
         {
         /*--- See if Job Item has JIPI tuple ---*/
         sprintf (sz, szJIPIAndPI, pszJobKey,
                  ItiDbGetZStr (ppszJIKeys [uJIIndex],  IDX_JI),
                  ItiDbGetZStr (ppszPCKeys [uPCIndex],  IDX_P),
                  ItiDbGetZStr (ppszPCKeys [uPCIndex],  IDX_C));

         if (!ItiDbGetRow1Col1(sz, hhp, 0, 0, 0))
            {
            /*--- make PI and JIPI ---*/
            if (uRet = MakePIAndJIPI (hhp,       pszJobKey,
                                      ppszJIKeys,  ppszPCKeys,
                                      uPCIndex,    uJIIndex))
               return MapCleanup (hhp, uRet, ppszJIKeys, ppszPCKeys);

            /*--- This is not really needed ---*/
            }
         }
      // DestroyHeap here?
      ItiMemDestroyHeap (hhp);
      }
   return MapCleanup (hheap, 0, ppszJIKeys, ppszPCKeys);
   }
