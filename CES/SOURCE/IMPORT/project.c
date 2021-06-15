/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
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


#define INCL_DOS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itifmt.h"
#include "..\include\itiimp.h"
#include "..\include\itiglob.h"
#include "import.h"
// #include "util.h"
#include "load.h"
#include "project.h"


static PSZ apszInfoNames [4] =
   {
   "COSTSHEET",
   "BIDHISTORY",
   "REFPRICE",
   "ITEMLIST",
   };



static USHORT EXPORT SkipSection (PCSVFILEINFO   pcfi)
   {
   USHORT   i;
   char     szTemp [256] = "";

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
      return ERROR_HIGHEST_CSV_ERROR;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
      !ItiStrToUSHORT (szTemp, &i))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiFileSkipCSVLine (pcfi);

   while (i)
      {
      ItiFileSkipCSVLine (pcfi);
      i--;
      }
   }



static USHORT EXPORT ProcessPriceInfo (PCSVFILEINFO   pcfi)
   {
   USHORT   i, j;
   char     szTemp [256] = "";

   for (i=0; i < 4; i++)
      {
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
          ItiStrICmp (szTemp, apszInfoNames [i]))
         return ERROR_HIGHEST_CSV_ERROR;

      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
         !ItiStrToUSHORT (szTemp, &j))
         return ERROR_HIGHEST_CSV_ERROR;

      ItiFileSkipCSVLine (pcfi);

      while (j)
         {
         ItiFileSkipCSVLine (pcfi);
         j--;
         }
      }
   }


static USHORT EXPORT ProcessItems (PCSVFILEINFO   pcfi,
                            FILE           *pfHeader,
                            FILE           *pfItems,
                            PSZ            pszJobID,
                            PSZ            pszWorkType)
   {
   USHORT   i, usError;
   char     szTemp [256];

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
       !ItiStrToUSHORT (szTemp, &i))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiFileSkipCSVLine (pcfi);

   /* write out headers */
   ItiCsvWriteLine (pfItems,
      "JobItem", "JobKey", "JobBreakdownKey", "JobAlternateGroupKey",
      "JobAlternateKey",
      "UnitPrice", "StandardItemKey", "Quantity", "ExtendedAmount", NULL);

   ItiCsvWriteLine (pfHeader,
      "JobBreakdown", "JobKey", "JobBreakdownID", "Description",
      "DetailedEstimate", "WorkType", NULL);
   ItiCsvWriteLine (pfHeader,
      "JobBreakdown", pszJobID, "Main", "Main Job Breakdown",
      "1", pszWorkType, NULL);
   ItiCsvWriteLine (pfHeader,
      "JobAlternateGroup", "JobKey", "JobAlternateGroupID", "Description",
      "ChosenAlternateKey", NULL);
   ItiCsvWriteLine (pfHeader,
      "JobAlternateGroup", pszJobID, "Non-Alt", "Non Alternate Group",
      "Non-Alt", NULL);
   ItiCsvWriteLine (pfHeader,
      "JobAlternate", "JobKey", "JobAlternateGroupKey", "JobAlternateID",
      "Description", NULL);
   ItiCsvWriteLine (pfHeader,
      "JobAlternate", pszJobID, "Non-Alt", "Non-Alt", 
      "Non Alternate", NULL);

   /* skip the first item, since it is junk */
   ItiFileSkipCSVLine (pcfi);
   ProcessPriceInfo (pcfi);
   i -= 1;

   for (usError = 0; i > 0 && usError == 0; i--)
      {
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
          ItiStrICmp (szTemp, "ITEMLISTELEMENT") != 0)
         return ERROR_HIGHEST_CSV_ERROR;

      /* write out the header */
      ItiCsvWriteField (pfItems, "JobItem", ',');
      ItiCsvWriteField (pfItems, pszJobID, ',');
      ItiCsvWriteField (pfItems, "Main", ',');
      ItiCsvWriteField (pfItems, "Non-Alt", ',');
      ItiCsvWriteField (pfItems, "Non-Alt", ',');

      /* write out the item junk */
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      ItiCsvWriteField (pfItems, szTemp, ',');

      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      ItiCsvWriteField (pfItems, szTemp, ',');

      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;

      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      ItiCsvWriteField (pfItems, szTemp, ',');

      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      ItiCsvWriteField (pfItems, szTemp, '\n');

      ItiFileSkipCSVLine (pcfi);

      /* process the pricing info */
      ProcessPriceInfo (pcfi);
      }

   return 0;
   }


static USHORT EXPORT ProcessPerms (PCSVFILEINFO   pcfi,
                            FILE           *pfHeader,
                            PSZ            pszJobID)
   {
   USHORT   i, usError;
   char     szTemp [256] = "";

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
       !ItiStrToUSHORT (szTemp, &i))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiFileSkipCSVLine (pcfi);

   /* write out header */
   ItiCsvWriteLine (pfHeader, "JobUser", "JobKey" , "UserID",
                    "AccessPrivilege", NULL);

               /* skip last one */
   for (usError = 0; i > 1 && usError == 0; i--)
      {
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
          ItiStrICmp (szTemp, "SECURITYELEMENT") != 0)
         return ERROR_HIGHEST_CSV_ERROR;

      /* write header info */
      ItiCsvWriteField (pfHeader, "JobUser", ',');
      ItiCsvWriteField (pfHeader, pszJobID,  ',');

      /* write out the item junk */
      while (ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         {
         ItiCsvWriteField (pfHeader, szTemp, ',');
         }
      fputc ('\n', pfHeader);

      ItiFileSkipCSVLine (pcfi);
      }

   /* skip the last one, since it is garbage */
   ItiFileSkipCSVLine (pcfi);


   return 0;
   }


USHORT EXPORT ProcessHeader (PCSVFILEINFO   pcfi,
                      FILE           *pfHeader,
                      FILE           *pfItems)
   {
   char   szTemp [256];
   char   szTemp2 [256];
   char   szJobID [17];
   char   szWorkType [10];
   USHORT usError;

   ItiFileSkipCSVLine (pcfi);

   /* write out header lines */
   ItiCsvWriteLine (pfHeader,
                    "Job", "ConstructionEngineeringPercent",
                    "ContingencyPercent", "Duration", "DavisBaconWages",
                    "WorkingDayCalendayKey", "Location", "SpecYear",
                    "BaseDate", "LastUpdate", "DateCreated",
                    "EstimatedCost", "EstimateTotal", "Description", "Estimator",
                    "JobID", "WorkType", "PrimaryCountyKey",
                    "CheckedBy", "DateChecked", "ApprovedBy", "DateApproved",
                    "LettingMonth", "District", NULL);
   fputs ("Job,0,0,0,0,N/A,0 0 0 0 0 0,0,1-1-1900,1-1-1900,1-1-1900,",
          pfHeader);

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // PROJECTHEADERELEMENT
      return ERROR_HIGHEST_CSV_ERROR;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // TotalCost
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // ProjectName
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // Estimator
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szJobID, NULL, sizeof szJobID))  // Contract Number
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szJobID, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // Contract Date
      return ERROR_HIGHEST_CSV_ERROR;

   if (!ItiFileGetCSVWord (pcfi, szWorkType, NULL, sizeof szWorkType))  // WorkType
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szWorkType, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // County
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // Checked by
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // date checked
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // approved by
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // date approved
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // Letting Date
      return ERROR_HIGHEST_CSV_ERROR;
   ItiFmtFormatString (szTemp, szTemp2, sizeof szTemp2, "DateTime,mmmm", NULL);
   ItiCsvWriteField (pfHeader, szTemp2, ',');

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))  // district
      return ERROR_HIGHEST_CSV_ERROR;
   ItiCsvWriteField (pfHeader, szTemp, '\n');

   ItiFileSkipCSVLine (pcfi);

   usError = 0;
   while (ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) &&
          usError == 0)
      {
      if (ItiStrICmp (szTemp, "ITEMLIST") == 0)
         usError = ProcessItems (pcfi, pfHeader, pfItems, szJobID, szWorkType);
      else if (ItiStrICmp (szTemp, "SECURITY") == 0)
         {
         usError = ProcessPerms (pcfi, pfHeader, szJobID);
         }
      else
         return ERROR_HIGHEST_CSV_ERROR;
      }

   return usError;
   }





