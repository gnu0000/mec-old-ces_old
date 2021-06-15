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
 * estimate.c
 *
 * this module calculates cost based estimates.
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include "..\include\itirptut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cbest.h"
#include <math.h>


/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstInstall (PSZ szDatabaseName, 
                           PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName, 
                             PSZ pszImportDatabase)
   {
   return TRUE;
   }




/*
 * ItiEstTableChanged is used to tell an installable estimation method
 * that a table has changed.  
 *
 * Parameters: usTable        The ID of the table that has changed.
 *
 *             ausColumns     An array from 0..usNumColumns, that 
 *                            contain the IDs of the columns that
 *                            have changed.  If this parameter is
 *                            NULL, then all columns have changed.
 *
 *             usNumColumns   The number of columns in ausColumns.
 *
 *             pszKey         A pointer to a string that contains
 *                            the unique key.  The string is in the 
 *                            form "ColumnName = ColumnValue 
 *                            [AND ColumnName = ColumnValue]".
 */

void EXPORT ItiEstTableChanged (USHORT   usTable, 
                                USHORT   ausColumns [],
                                USHORT   usNumColumns,
                                PSZ      pszKey)
   {
//   if (usTable == tJob && 
//       (ItiEstSearchCol (ausColumns, usNumColumns, cLettingMonth) ||
//        ItiEstSearchCol (ausColumns, usNumColumns, cBaseDate) ||
//        ItiEstSearchCol (ausColumns, usNumColumns, cWorkType))
//      {
//
//      }
   }




//static PSZ CalculateCostSheet (HHEAP   hheap, 
//                               PSZ     pszStandardItemKey, 
//                               PSZ     pszBaseDate, 
//                               PSZ     pszQuantity)
//   {
//   return NULL;
//   }
//
//
//
//
PSZ EXPORT ItiEstItem (HHEAP hheap,
                       PSZ   pszBaseDate,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszStandardItemKey,
                       PSZ   pszQuantity)
   {
//   PSZ      pszPrice, pszTempBaseDate;
//
//   if (pszStandardItemKey == NULL || 
//       (pszJobKey == NULL && pszBaseDate == NULL))
//      return NULL;
//
//   pszTempBaseDate = ItiEstGetJobBaseDate (hheap, pszJobKey);
//   
//   if (pszBaseDate == NULL)
//      {
//      pszBaseDate = pszTempBaseDate;
//      }
//
//   if (pszQuantity == NULL)
//      return ItiStrDup (hheap, "");
//
//   pszPrice = NULL;
//
//   pszPrice = CalculateCostSheet (hheap, pszStandardItemKey, pszBaseDate, 
//                                  pszQuantity);
//
//   ItiMemFree (hheap, pszTempBaseDate);
//   return pszPrice;
   return ItiStrDup (hheap, "0");
   }



VOID EXPORT ItiEstQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Cost based", usMaxSize);
   *pusWindowID = CBEstForJobItemS;
   }






BOOL EXPORT ItiEstQueryJobAvail (HHEAP hheap, 
                                 PSZ   pszJobKey,
                                 PSZ   pszJobItemKey)
   {
//   char szTemp [1024];
//   PSZ psz;
//
//   sprintf (szTemp,
//            "SELECT JCBE.JobItemCostBasedEstimateKey "
//            "FROM JobCostBasedEstimate JCBE "
//            "WHERE JCBE.JobItemKey = %s "
//            "AND JCBE.JobKey = %s "
//            "AND JCBE.Active = 1 ",
//            pszJobItemKey, pszJobKey);
//
//   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
//   if (psz == NULL)
//      return FALSE;
//
//   ItiMemFree (hheap, psz);
   return TRUE;
   }


BOOL EXPORT ItiEstQueryAvail (HHEAP hheap, 
                              PSZ   pszStandardItemKey,
                              PSZ   pszBaseDate)
   {
//   char szTemp [1024];
//   PSZ psz;
//
//   sprintf (szTemp,
//            "SELECT CBE.CostBasedEstimateKey "
//            "FROM CostBasedEstimate CBE "
//            "WHERE CBE.StandardItemKey = %s "
//            "AND CBE.Active = 1 ",
//            pszStandardItemKey);
//
//   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
//   if (psz == NULL)
//      return FALSE;
//
//   ItiMemFree (hheap, psz);
   return TRUE;
   }


