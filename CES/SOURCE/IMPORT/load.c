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


/*
 * load.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This module is part of DS/Shell.
 *
 * This module loads data from an import file into a database table.
 */



#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "..\include\colid.h"
#include "..\include\itiimp.h"
// #include "util.h"
#include "load.h"
#include "import.h"


#include <stdio.h>
/*
 * ItiMemDupSeg duplicates a segment.  If usNumBytes is 0, then the size
 * of pvSeg is duplicated.
 */

PVOID EXPORT ItiMemDupSeg (PVOID pvSeg, USHORT usNumBytes)
   {
   PVOID pvNew;
   USHORT usSegSize;

   usSegSize = ItiMemQuerySeg (pvSeg);
   if (usNumBytes == 0)
      {
      usNumBytes = usSegSize;
      }
   pvNew = ItiMemAllocSeg (usNumBytes, SEG_NONSHARED, 0);
   if (pvNew == NULL)
      return NULL;

   if (usNumBytes == 0)
      {
      *(((PSZ) pvNew) + 65535U)= *(((PSZ) pvSeg) + 65535U);
      usNumBytes = 65535U;
      }
   ItiMemCpy (pvNew, pvSeg, usNumBytes);
   return pvNew;
   }







/*
 * BindColumns 
 *
 * TRUE if OK, FALSE if not.
 */

static BOOL EXPORT BindColumns (PIMPORTINFO pii)
   {
   USHORT i;
   BOOL   bOK;

   for (bOK = TRUE, i=0; i < pii->usTotalColumns && bOK; i++)
      {
      if (i < pii->usFileColumns || pii->pci [i].bSysGenKey ||
          pii->pci [i].pszColumnData [0])
         bOK = ItiBcpBindColumn (pii->pci [i].usColumnOrder,
                                 pii->pci [i].pszColumnData);
      }
   return bOK;
   }


PCOLINFO EXPORT FindKeyColumn (PIMPORTINFO pii)
   {
   USHORT i;

   for (i=0; i < pii->usTotalColumns; i++)
      if (pii->pci [i].bSysGenKey)
         return &(pii->pci [i]);
   return NULL;
   }


BOOL EXPORT IsNull (PSZ pszString)
   {
   while (*pszString && ItiCharIsSpace (*pszString))
      pszString++;
   return *pszString == '\0';
   }


/* 
 * LoadTable loads a table from a file into SQL server using the 
 * BCP load facility.  
 *
 * Return value: 0 if table was loaded, otherwise an error code.
 * If an error occured, then the offending section is skipped.
 */

USHORT EXPORT LoadTable (PCSVFILEINFO   pcfi,
                  PIMPORTINFO    pii)
   {
   BOOL        bDone, bError;
   USHORT      usLen;
   USHORT      i, j;
   CSVFILEINFO cfiSave;
   ULONG       ulLine, ulCount, ulTotalLines;
   PCOLINFO    pciKey;

//   12/14/1992 mdh: commented this out
//   pii->ulRows = 0;
   if (!ItiBcpSetTable (pii->pti->pszTableName))
      {
      printf ("\nBCP Bind failed on line %lu. Table %s skipped.\n", 
              pcfi->ulLine, pii->pti->pszTableName);
      ItiFileSkipCSVSection (pcfi, FALSE);
      return ERROR_BCP_SET_TABLE;
      }

   /* bind the columns */
   if (!BindColumns (pii))
      {
      printf ("\nColumn binding failed on line %lu.  Table %s skipped.\n",
              pcfi->ulLine, pii->pti->pszTableName);
      ItiFileSkipCSVSection (pcfi, FALSE);
      return ERROR_BCP_BIND;
      }

   pciKey = FindKeyColumn (pii);

   /* read the data rows */
   bDone = FALSE;
   ItiFileSaveCSVPos (&cfiSave, pcfi);
   ulLine = 0;

   // 12/14/1992 mdh: changed 0 to pii->ulRows to keep key count correct 
   ulCount = pii->ulRows;

   ulTotalLines = 0;
   while (!bDone && ItiFileGetCSVWord (pcfi, pszBuffer, &usLen, usBufferSize))
      {
      if (pii->pti->usTableID != ItiColStringToTableId (pszBuffer))
         {
         bDone = TRUE;
         }
      else
         {
         bError = FALSE;
         for (i=0; i < pii->usFileColumns; i++)
            {
            /* -- init the current column data field for ItiFmtCheckString. */
            for (j=0; j < pii->pci [i].usColumnMax; j++)
               pii->pci [i].pszColumnData[j] = ' ';
            pii->pci [i].pszColumnData[j] = '\0';

            ItiFileGetCSVWord (pcfi, pszBuffer, &usLen, usBufferSize);
            if (IsNull (pszBuffer) && pii->pci [i].pszDefault)
               ItiStrCpy (pii->pci [i].pszColumnData, 
                          pii->pci [i].pszDefault,
                          pii->pci [i].usColumnMax);
            else if (ItiFmtCheckString (pszBuffer, 
                                        pii->pci [i].pszColumnData,
                                        pii->pci [i].usColumnMax,
                                        pii->pci [i].pszFormat,
                                        NULL))
               {
               printf ("\nLine %lu, Column %lu, Field %s: Bad data: '%s'\n"
                       "Record skipped.",
                       pcfi->ulLine, pcfi->ulColumn, 
                       pii->pci [i].pszImportName, pszBuffer);
               bError = TRUE;
               break;
               }
            }
         if (!bError)
            {
            ulCount++;
            if (pciKey)
               ItiStrULONGToString (ulCount, pciKey->pszColumnData, 
                                    pciKey->usColumnMax);
            
            if (!ItiBcpSendRow ())
               {
               printf ("\nLine %lu, Column %lu: BCP Send Row failed.\n"
                       "Record skipped.",
                       pcfi->ulLine, pcfi->ulColumn);
               bError = TRUE;
               }
            }

         bDone = ItiFileSkipCSVLine (pcfi);
         ItiFileSaveCSVPos (&cfiSave, pcfi);
         ulLine++;
         if (ulLine % 100 == 0)
            {
            if (ulLine % 1000 == 0)
               {   
               ulTotalLines = ItiBcpBatch ();
               if (ulTotalLines == -1)
                  {
                  bDone = TRUE;
                  bError = TRUE;
                  printf ("\nbcp_batch failed.  refer to error log for more info.");
                  }
               else
                  {
                  printf (".");
                  pii->ulRows += ulTotalLines;
                  }
               } /* if (ulLine % 1000... then clause */ 
            else
               printf (".");
            } /* if (ulLine % 100... then clause */
         } /* if (pii->pti->usTableID... else clause */ 

      } /* while (!bDone... */ 

   if (bDone)
      ItiFileRestoreCSVPos (pcfi, &cfiSave);

   printf ("\n");
   ulTotalLines = ItiBcpTableDone ();

   /* free up memory taken up by buffers */
   for (i=0; i < pii->usTotalColumns; i++)
      {
      ItiMemFreeSeg (pii->pci [i].pszColumnData);
      pii->pci [i].pszColumnData = NULL;
      }

   if (ulTotalLines == -1)
      return ERROR_BCP_DONE;

   pii->ulRows += ulTotalLines;
   return 0;
   }

  
