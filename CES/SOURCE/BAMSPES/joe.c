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


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\itifmt.h"

#include <stdio.h>

#include "joe.h"


/*
 * this function will update the JobItemProjectItem and ProjectItem tables
 * for a given category.  
 */
static BOOL UpdateCategoryItems (HHEAP hheap, 
                                 PSZ   pszJobKey,
                                 PSZ   pszPCN,
                                 PSZ   pszCategory,
                                 BOOL  bFirstTime);

static BOOL FirstUseOfJobItem (HHEAP hheap, 
                               PSZ   pszJobKey, 
                               PSZ   pszJobItem);

USHORT QueryNumPCNs (PSZ pszJobKey);
USHORT QueryNumCategories (PSZ pszJobKey);


/*
 * this function performs magic, and returns the number of 
 * categories updated.
 */


USHORT Joe (HHEAP hheap, PSZ pszJobKey)
   {
   HQRY     hqry;
   USHORT   usNumCols, i, usError, usNumCategories;
   char     szQuery [2048];
   PSZ      *ppsz, *ppszCategories, *ppszPCNs;

   usNumCategories = QueryNumCategories (pszJobKey);
   if (usNumCategories == 0)
      return 0;

   ppszCategories = ItiMemAlloc (hheap, sizeof (PSZ) * usNumCategories,
                                 MEM_ZERO_INIT);
   ppszPCNs = ItiMemAlloc (hheap, sizeof (PSZ) * usNumCategories,
                           MEM_ZERO_INIT);

   sprintf (szQuery, 
            "SELECT PC.ProjectControlNumber, PC.Category "
            "FROM JobProject JP, ProjectCategory PC "
            "WHERE JP.JobKey = %s "
            "AND JP.ProjectControlNumber = PC.ProjectControlNumber ",
            pszJobKey);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return 0;

   i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError) )
      {
      if (i < usNumCategories)
         {
         ppszPCNs [i] = ppsz [0];
         ppszCategories [i] = ppsz [1];
         }
      i++;
      ItiMemFree (hheap, ppsz);
      }

   for (i=0; i < usNumCategories; i++)
      {
      UpdateCategoryItems (hheap, pszJobKey, ppszPCNs [i], 
                           ppszCategories [i], i == 0); 
      }


   ItiFreeStrArray (hheap, ppszPCNs, usNumCategories);
   ItiFreeStrArray (hheap, ppszCategories, usNumCategories);

   return usNumCategories;
   }




static BOOL UpdateCategoryItems (HHEAP hheap, 
                                 PSZ   pszJobKey,
                                 PSZ   pszPCN,
                                 PSZ   pszCategory,
                                 BOOL  bFirstTime)
   {
   HQRY     hqry;
   USHORT   usNumCols, i, usError, usMaxSeq;
   char     szQuery [2048];
   PSZ      *ppsz, psz;
   BOOL     bGetQuantity;

   sprintf (szQuery, "SELECT MAX(SequenceNumber) "
                     "FROM JobItemProjectItem "
                     "WHERE JobKey = %s "
                     "AND ProjectControlNumber = '%s' "
                     "AND Category = '%s' ",
                     pszJobKey, pszPCN, pszCategory);
   
   psz = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
   if (psz == NULL || *psz == '\0' || !ItiStrToUSHORT (psz, &usMaxSeq))
      usMaxSeq = 1;
      
   sprintf (szQuery, "SELECT JobItemKey "
                     "FROM JobItem "
                     "WHERE JobKey = %s "
                     "AND JobItemKey NOT IN "
                     "(SELECT JobItemKey "
                     " FROM JobItemProjectItem "
                     " WHERE JobKey = %s "
                     " AND ProjectControlNumber = '%s' "
                     " AND Category = '%s') ",
                     pszJobKey, pszJobKey, pszPCN, pszCategory);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return 0;

   i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      bGetQuantity = bFirstTime && 
                     FirstUseOfJobItem (hheap, pszJobKey, ppsz [0]);
      sprintf (szQuery, 
               "INSERT INTO JobItemProjectItem "
               "(JobKey, JobItemKey, ProjectControlNumber, Category, "
               "SequenceNumber) "
               "VALUES "
               "(%s, %s, '%s', '%s', '%.4u') ",
               pszJobKey, ppsz [0], pszPCN, pszCategory, usMaxSeq);

      ItiDbExecSQLStatement (hheap, szQuery);

      sprintf (szQuery, 
               "INSERT INTO ProjectItem "
               "(ProjectControlNumber, Category, SequenceNumber,"
               "Quantity, UnitPrice, SupplementalDescription, "
               "ExtendedAmount, StandardItemKey) "
               "SELECT '%s', '%s', '%.4u', "
               "%s, JI.UnitPrice, JI.SupplementalDescription, %s, "
               "JI.StandardItemKey "
               "FROM JobItem JI "
               "WHERE JI.JobKey = %s "
               "AND JI.JobItemKey = %s ",
               pszPCN, pszCategory, usMaxSeq,
               bGetQuantity ? "JI.Quantity" : "0",
               bGetQuantity ? "JI.ExtendedAmount" : "0",
               pszJobKey, ppsz [0]);
      ItiDbExecSQLStatement (hheap, szQuery);

      i++;
      if (usMaxSeq == 1)
         usMaxSeq = 5;
      else
         usMaxSeq += 5;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   return TRUE;
   }


static BOOL FirstUseOfJobItem (HHEAP hheap, 
                               PSZ   pszJobKey, 
                               PSZ   pszJobItem)
   {
   USHORT   i;
   char     szQuery [512];
   PSZ      psz;

   sprintf (szQuery, 
            "SELECT COUNT (*) "
            "FROM JobItemProjectItem "
            "WHERE JobKey = %s "
            "AND JobItemKey = %s ",
            pszJobKey, pszJobItem);

   psz = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
   if (psz == NULL || psz == NULL || !ItiStrToUSHORT (psz, &i))
      i = 0;

   return i == 0;
   }


/*
 * call this function whenever a new project category is created
 */

BOOL AddProjectItems (HHEAP hheap,
                      PSZ   pszJobKey,
                      PSZ   pszPCN,
                      PSZ   pszCategory)
   {
   char     szQuery [2048], szQuantity [30], szPrice [30], szExtension [30];
   HQRY     hqry;
   USHORT   i, usNumCols, usError;
   PSZ      *ppsz;
   BOOL     bFirst;

   bFirst = 1 == QueryNumCategories (pszJobKey);

   sprintf (szQuery,
            "SELECT JobItemKey, StandardItemKey, Quantity, UnitPrice, "
            "SupplementalDescription, ExtendedAmount "
            "FROM JobItem "
            "WHERE JobKey = %s ",
            pszJobKey);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   i = 1;
   while (hqry != NULL && ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      ItiFmtFormatString (ppsz [2], szQuantity, sizeof szQuantity,
                          "Number,$", NULL);
      ItiFmtFormatString (ppsz [3], szPrice, sizeof szPrice,
                          "Number,$", NULL);
      ItiFmtFormatString (ppsz [5], szExtension, sizeof szExtension,
                          "Number,$", NULL);
      sprintf (szQuery,
               "INSERT INTO JobItemProjectItem "
               "(JobKey, JobItemKey, ProjectControlNumber, "
               "Category, SequenceNumber) "
               "VALUES "
               "(%s, %s, '%s', "
               "'%s', '%.4d') ",
               pszJobKey, ppsz [0], pszPCN, pszCategory, i);
      ItiDbExecSQLStatement (hheap, szQuery);

      sprintf (szQuery,
               "INSERT INTO ProjectItem "
               "(ProjectControlNumber, Category, SequenceNumber, "
               "Quantity, UnitPrice, SupplementalDescription, "
               "ExtendedAmount, StandardItemKey) "
               "VALUES "
               "('%s', '%s', '%.4d', %s, %s, '%s', %s, %s)",
               pszPCN, pszCategory, i,
               bFirst ? szQuantity : "0",
               szPrice, ppsz [4], 
               bFirst ? szExtension : "0",
               ppsz [1]);
      ItiDbExecSQLStatement (hheap, szQuery);

      if (i == 1)
         i = 5;
      else
         i += 5;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   return hqry != NULL;
   }





USHORT QueryNumPCNs (PSZ pszJobKey)
   {
   char     szQuery [512];
   USHORT   usNumCols, usError;

   sprintf (szQuery,
            "SELECT JP.ProjectControlNumber "
            "FROM JobProject JP "
            "WHERE JP.JobKey = %s ",
            pszJobKey);
   return ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   }



USHORT QueryNumCategories (PSZ pszJobKey)
   {
   char     szQuery [512];
   USHORT   usNumCols, usError;

   sprintf (szQuery,
            "SELECT PC.Category "
            "FROM JobProject JP, ProjectCategory PC "
            "WHERE JP.JobKey = %s "
            "AND JP.ProjectControlNumber = PC.ProjectControlNumber ",
            pszJobKey);
   return ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   }
