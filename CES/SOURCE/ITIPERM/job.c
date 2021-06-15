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
 * job.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itidbase.h"
#include "..\include\dialog.h"
#include "..\include\colid.h"
#include "init.h"
#include <stdio.h>
#include "job.h"



typedef struct
   {
   PSZ      pszJobKey;
   USHORT   usPerm;
   } JOBPERM;

typedef JOBPERM *PJOBPERM;


#define MAX_ENTRIES  100


/* global variables */

static JOBPERM jp [MAX_ENTRIES];





static BOOL CheckPerm (PSZ pszJobKey)
   {
   USHORT i;

   if (pszJobKey == NULL)
      return FALSE;

   for (i = 0; i < MAX_ENTRIES; i++)
      if (jp [i].pszJobKey && 
          0 == ItiStrICmp (pszJobKey, jp [i].pszJobKey))
         {
         return jp [i].usPerm >= USER_USER;
         }

   return TRUE;
   }




BOOL CanEditJob (HWND hwnd, BOOL bJob)
   {
   HHEAP hheapWnd;
   PSZ   pszJobKey;
   BOOL  bOK;

   hheapWnd = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   if (bJob)
      pszJobKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   else
      pszJobKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cProgramKey);

   bOK = CheckPerm (pszJobKey);

   ItiMemFree (hheapWnd, pszJobKey);
   return bOK;
   }




BOOL EXPORT ItiPermQueryDelete (PSZ pszKey, BOOL bJob)
   {
   HQRY     hqry;
   USHORT   usNumCols, usError, usPerm;
   PSZ      *ppsz;
   char     szQuery [1024];
   BOOL     bTerminate;

   if (pglobals->usUserType >= USER_SUPER)
      return TRUE;

   sprintf (szQuery, 
            "SELECT JU.UserID, JU.AccessPrivilege "
            "FROM %sUser JU "
            "WHERE JU.%sKey = %s "
            "ORDER BY JU.AccessPrivilege DESC ",
            bJob ? "Job" : "Program", 
            bJob ? "Job" : "Program",
            pszKey);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   bTerminate = FALSE;
   usPerm = 0;
   /* do not reorder the while statement -- bTerminate needs to be evaluated
      before ItiDbGetQueryRow */
   while (!bTerminate && ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      if (ItiStrMatchWildCard (ppsz [0], pglobals->pszUserID))
         {
         ItiStrToUSHORT (ppsz [1], &usPerm);
         bTerminate = usPerm != 0;
         }
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (bTerminate)
      ItiDbTerminateQuery (hqry);

   return usPerm >= USER_OWNER;
   }



BOOL EXPORT ItiPermJobOpened (PSZ pszJobKey, BOOL bJob)
   {
   HQRY     hqry;
   USHORT   usNumCols, usError, i, usPerm;
   PSZ      *ppsz;
   char     szQuery [1024];
   BOOL     bTerminate;


   // added 4/8/93 clf
   // if user wasn't >= USER_SUPER
   // his permission was determined by the whim 
   // of the stack
   //
   usPerm = 0;

   sprintf (szQuery, 
            "SELECT JU.UserID, JU.AccessPrivilege "
            "FROM %sUser JU "
            "WHERE JU.%sKey = %s "
            "ORDER BY JU.AccessPrivilege DESC ",
            bJob ? "Job" : "Program", 
            bJob ? "Job" : "Program",
            pszJobKey);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   bTerminate = TRUE;
   if (pglobals->usUserType >= USER_SUPER)
      usPerm = USER_OWNER;
   else
      {
      bTerminate = FALSE;
      /* do not reorder the while statement -- bTerminate needs to be evaluated
         before ItiDbGetQueryRow */
      while (!bTerminate && ItiDbGetQueryRow (hqry, &ppsz, &usError))
         {
         if (ItiStrMatchWildCard (ppsz [0], pglobals->pszUserID))
            {
            ItiStrToUSHORT (ppsz [1], &usPerm);
            bTerminate = usPerm != 0;
            }
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }
      }
   
   if (bTerminate)
      ItiDbTerminateQuery (hqry);

   if (usPerm == 0)
      return FALSE;

   /* insert the permission in the table */
   for (i=0; i < MAX_ENTRIES; i++)
      if (jp [i].pszJobKey == NULL)
         {
         jp [i].pszJobKey = ItiStrDup (hheap, pszJobKey);
         jp [i].usPerm = usPerm;
         break;
         }

   return TRUE;
   }


BOOL EXPORT ItiPermJobClosed (PSZ pszJobKey)
   {
   USHORT i;

   for (i=0; i < MAX_ENTRIES; i++)
      if (jp [i].pszJobKey && ItiStrICmp (pszJobKey, jp [i].pszJobKey) == 0)
         {
         ItiMemFree (hheap, jp [i].pszJobKey);
         jp [i].pszJobKey = NULL;
         jp [i].usPerm = 0;
         return TRUE;
         }

   return FALSE;
   }




BOOL InitJob (void)
   {
   USHORT i;

   for (i=0; i < MAX_ENTRIES; i++)
      {
      jp [i].pszJobKey = NULL;
      jp [i].usPerm = 0;
      }
   return TRUE;
   }

