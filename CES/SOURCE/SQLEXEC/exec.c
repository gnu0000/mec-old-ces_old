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


#define INCL_DOSMISC
#include "..\include\iti.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "exec.h"
#include "dbqry.h"


extern BOOL bVerbose;



static BOOL IsBlankLine (PSZ pszLine)
   {
   if (*pszLine == ';')
      return TRUE;

   while (isspace (*pszLine))
      pszLine++;

   return *pszLine == '\0';
   }


/* returns FALSE on ERROR */

static BOOL ExecLine (PSZ   pszLine, 
                      BOOL  bIgnoreError, 
                      BOOL  bPrint,
                      PBOOL pbResult,
                      FILE  *pfOut)
   {
   BOOL   bError;
   HQRY   hqry;
   USHORT usNumCols, usState, i, usColToPrint, usNumRows;
   PSZ    *ppsz;

   usNumRows = 0;
   if (!bPrint && pbResult == NULL)
      bError = !DbExecSQLStatement (pszLine);
   else
      {
      if (bPrint && *pszLine >= '1' && *pszLine <= '9')
         {
         usColToPrint = *pszLine - '1';
         pszLine++;
         }
      else
         usColToPrint = -1;
      hqry = DbExecQuery (pszLine, &usNumCols, &usState);
      if (!(bError = (hqry == NULL)))
         {
         while (DbGetQueryRow (hqry, &ppsz, &usState))
            {
            usNumRows++;
            for (i = 0; i < usNumCols; i++)
               {
               if (bPrint && (usColToPrint == -1 || i == usColToPrint))
                  {
                  fputs (ppsz [i], pfOut);
                  if (usColToPrint == -1 && i < usNumCols - 1)
                     fputc ('\t', pfOut);
                  }
               }
            if (bPrint)
               fputc ('\n', pfOut);

            if (ppsz)
               {
               if (pbResult && !*pbResult)
                  *pbResult = strcmp (ppsz [0], "0");
               for (i=0; i < usNumCols; i++)
                  {
                  if (ppsz [i])
                     free (ppsz [i]);
                  }
               free (ppsz);
               }
            }
         }
      }
   if (pbResult && !*pbResult)
      *pbResult = usNumRows != 0;

   return bIgnoreError || (!bError);
   }




int SQLExec (PSZ     pszInputFile, 
             FILE    *pfIn, 
             PSZ     pszLogFile, 
             FILE    *pfLog,
             USHORT  usSkipToLine,
             FILE    *pfOut,
             USHORT  usDots)
   {
   char     szLine [10240];
   USHORT   usLine, usLen, i;
   BOOL     bIgnoreError, bError, bPrint, bSendOnly;
   BOOL     bResult, bGetResult, bDoTrue, bDoFalse;
   USHORT   usLineOffset;

   bError = FALSE;
   usLine = 0;
   while (!bError && fgets (szLine, sizeof szLine, pfIn))
      {
      if (usDots && ((usLine % usDots) == 0))
         putchar ('.');

      usLine++;
      if (usSkipToLine > usLine)
         continue;

      usLen = strlen (szLine);
      if (usLen > 0 && (szLine [usLen - 1] == '\n' || szLine [usLen - 1] == '\r'))
         szLine [usLen - 1] = '\0';

      if (IsBlankLine (szLine))
         continue;

      usLineOffset = 0;
      bIgnoreError = FALSE;
      bSendOnly    = FALSE;
      bPrint       = FALSE;
      bGetResult   = FALSE;
      bDoTrue      = FALSE;
      bDoFalse     = FALSE;

      i = 0;
      if (szLine [i] == '+')
         {
         bSendOnly = TRUE;
         szLine [i] = ' ';
         }
      if (szLine [i] == ':')
         {
         bDoTrue = TRUE;
         i++;
         usLineOffset++;
         }
      if (szLine [i] == '!')
         {
         bDoFalse = TRUE;
         i++;
         usLineOffset++;
         }
      if (szLine [i] == '?')
         {
         bResult    = FALSE;
         bGetResult = TRUE;
         i++;
         usLineOffset++;
         }
      if (szLine [i] == '*')
         {
         bIgnoreError = TRUE;
         usLineOffset++;
         }
      if (szLine [i] == '"')
         {
         bPrint = TRUE;
         usLineOffset++;
         }
      if (szLine [i] == '#')
         {
         bPrint = TRUE;
         bIgnoreError = TRUE;
         usLineOffset++;
         }

      if (bSendOnly)
         {
         if (bVerbose)
            fprintf (pfLog, "Send line %.4u: %s\n", usLine, szLine);

         strcat (szLine, " ");
         if (!DbSendQueryPart (szLine + usLineOffset, &i))
            {
            bError = TRUE;
            fprintf (stderr,"ERROR: Error encountered sending line %u of file %s.\n"
                 "Refer to the file %s for detailed error information.\n",
                  usLine, pszInputFile, pszLogFile);
            fprintf (pfLog, "Error on line %u of file %s.\n", usLine, pszInputFile);
            }
         }
      else /* !bSendOnly */
         {
         if (!((bDoTrue  && !bResult) ||
               (bDoFalse &&  bResult)))
            {
            if (bVerbose)
               fprintf (pfLog, "Exec line %.4u: %s\n", usLine, szLine);

            if (!ExecLine (szLine + usLineOffset, bIgnoreError, bPrint,
                           bGetResult ? &bResult : NULL, pfOut))
               {
               bError = TRUE;
               fprintf (stderr, 
                        "ERROR: Error encountered executing line %u of file %s.\n"
                        "Refer to the file %s for detailed error information.\n",
                        usLine, pszInputFile, pszLogFile);
               fprintf (pfLog, "Error on line %u of file %s.\n", usLine, pszInputFile);
               }
            }
         }
      }

   if (!bError && bVerbose)
      {
      fprintf (pfOut, "%u Lines executed successfully", usLine);
      }

   fprintf (pfLog, "%u Lines executed successfully", usLine);

   return bError;
   }



