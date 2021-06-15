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


#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include <stdio.h>
#include <stdarg.h>
#include "..\include\itiimp.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include "..\dssimp\dssimp.h"
#include "..\include\dssimp.h"
#include "..\include\itifmt.h"

void EXPORT ItiCSVWriteField (FILE *pf, PSZ psz, int cTrail)
   {
   BOOL     bNeedsQuotes;

   bNeedsQuotes = (ItiStrChr (psz, ',') != NULL) ||
                  (ItiStrChr (psz, '"') != NULL);

   if (!bNeedsQuotes)
      {
      fputs (psz, pf);
      if (cTrail)
         fputc (cTrail, pf);
      return;
      }

   /* quote the string, and double the internal quotes */
   fputc ('"', pf);
   while ('\0' != *psz)
      {
      fputc (*psz, pf);
      if ('"' == *psz)
         fputc (*psz, pf);
      psz++;
      }
   fputc ('"', pf);
   if (cTrail)
      fputc (cTrail, pf);
   }


void CEXPORT ItiCSVWriteLine (FILE *pf, ...)
   {
   va_list  va;
   PSZ      psz;

   va_start (va, pf);
   psz = va_arg (va, PSZ);

   while (psz)
      {
      ItiCSVWriteField (pf, psz, 0);
      psz = va_arg (va, PSZ);
      if (psz)
         fputc (',', pf);
      }

   fputc ('\n', pf);
   }

/* csp - BSR 930154 12oct94 - Do any necessary conversions before
   writing the data
*/
void EXPORT ItiCSVDoConversion(USHORT usID,
                               PPSZ   ppsz)
   {
   switch (usID)
      {
      case EXP_MAPVERTEX:
         {
         CHAR szLongLat [56], szFormatted [56];
         if (ItiStrCmp (ppsz[0], "MapVertex"))
            return;

         sprintf(szLongLat, "Position,long <long><decimal> lat <lat><decimal>");
         ItiFmtFormatString (ppsz [3], szFormatted, sizeof szFormatted,
                             szLongLat, NULL);
         ItiStrCpy (ppsz [3], szFormatted, sizeof szFormatted);

         break;
         }
      }
   }




BOOL EXPORT ItiExpExportTable (HHEAP    hheap,
                               HMODULE  hmod,
                               FILE     *pf, 
                               USHORT   usID,
                               PSZ      pszWhere)
   {
   char     szTemp [2040] = "";
   char     szQuery [4090] = "";
   USHORT   usError, usNumCols, i;
   PSZ      apszValues [2], apszTokens [2];
   PPSZ     ppsz;
   HQRY     hqry;

   apszTokens [0] = "Where";
   apszValues [0] = pszWhere;
   apszTokens [1] = NULL;
   apszValues [1] = NULL;

   /* write out the header line */
   ItiMbQueryQueryText (hmod, ITIRID_EXPORTHDR, usID, szTemp, sizeof szTemp);
   fputs (szTemp, pf);
   fputc ('\n', pf);

   ItiMbQueryQueryText (hmod, ITIRID_EXPORT, usID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, apszTokens, apszValues, 100);
   
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      for (i=0; i < usNumCols - 1; i++)
         ItiCSVWriteField (pf, ppsz [i], ',');
      ItiCSVDoConversion(usID, ppsz);
      ItiCSVWriteField (pf, ppsz [usNumCols - 1], '\n');
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   return TRUE;
   }

