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


#define INCL_DOSFILEMGR 
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
// # include "util.h"


/* define the characters that we check */

#define CHAR_EOF        '\x1a'
#define CHAR_CR         '\x0d'
#define CHAR_LF         '\x0a'
#define CHAR_COMMENT    ';'
#define CHAR_COMMA      ','
#define CHAR_QUOTE      '"'
#define CHAR_BACKSLASH  '\\'

/* UNGET moves the file pointer backwards n characters */
#define UNGET(pf,n) ItiFileSetPos ((pf), - (long) (sizeof (CHAR)) * (long) (n), FILE_CURRENT);


#define min(a,b) ((a) > (b) ? (b) : (a))

/*
 * returns TRUE if buffer filled, FALSE on error or EOF.
 */

static BOOL FillBuffer (PFILE pf)
   {
   USHORT usError;

   if (pf->usBufferPos < pf->usBufferEnd)
      /* buffer does not need to be refilled */
      return TRUE;

   usError = DosRead (pf->hf, pf->szBuffer, pf->usBufferSize, 
                      &(pf->usBufferEnd));
   pf->usBufferPos = 0;
   return usError == 0 && pf->usBufferEnd != 0;
   }




/*
 * ItiFileQueryExist returns TRUE if the given file exists.
 *
 * Parameters: pszFileName       The name of the file.
 *
 * Return Value: TRUE if the file exists, FALSE if not.
 */

BOOL EXPORT ItiFileQueryExist (PSZ pszFileName)
   {
   HFILE hf;
   USHORT usAction;
   USHORT usError;

   usError = DosOpen2 (pszFileName, &hf, &usAction, 0, FILE_NORMAL, 
                       FILE_OPEN, 
                       OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE |
                       OPEN_FLAGS_FAIL_ON_ERROR, NULL, 0);
   if (usError)
      return FALSE;
   DosClose (hf);
   return TRUE;
   }



#define BUFFER_SIZE        32768
#define MIN_BUFFER_SIZE    1024

/*
 * ItiFileOpenRead opens the given file for read access.  The file is
 * locked against write access.  The file must exist, or this 
 * function will return 0.
 *
 * Parameters: pszFileName       The file to open.
 *
 * Return Value: A pointer to the file, or 0 on error.
 */

PFILE EXPORT ItiFileOpenRead (PSZ pszFileName)
   {
   PFILE    pf;
   USHORT   usAction;
   USHORT   usError;
   HFILE    hf;
   USHORT   usSize;

   usError = DosOpen2 (pszFileName, &hf, &usAction, 0, FILE_NORMAL, 
                       FILE_OPEN, 
                       OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE /* |
                       bSequential ? OPEN_FLAGS_SEQUENTIAL : OPEN_FLAGS_RANDOM */,
                       NULL, 0);
   if (usError)
      return NULL;

   usSize = BUFFER_SIZE;
   pf = NULL;
   while (pf == NULL && usSize >= MIN_BUFFER_SIZE)
      {
      pf = (PFILE) ItiMemAllocSeg (usSize, 0, SEG_NONSHARED);
      if (pf == NULL)
         usSize -= 1024;
      }

   if (pf == NULL)
      {
      DosClose (hf);
      return NULL;
      }

   pf->usBufferSize = usSize - sizeof *pf;
   pf->usBufferPos = 0;
   pf->usBufferEnd = 0;
   pf->hf = hf;

   /* fill buffer */
   FillBuffer (pf);

   return pf;
   }



/*
 * ItiFileClose closes the given file.
 *
 * Parameters: pf            A handle to a file to close.
 *
 * Return Value: None.
 */

void EXPORT ItiFileClose (PFILE pf)
   {
   DosClose (pf->hf);
   ItiMemFreeSeg (pf);
   }


/*
 * ItiFileRead reads usNumBytes from the file pf.
 * TRUE on success, FALSE on read error or EOF.
 */

BOOL EXPORT ItiFileRead (PFILE    pf, 
                         PSZ      pszBuffer, 
                         USHORT   usBufferSize, 
                         PUSHORT  pusBytesRead)
   {
   USHORT usTemp, usBytesRead, usError;

   usBytesRead = 0;
   usError = 0;

   /* copy the current contents of the buffer into the output buffer */
   if (pf->usBufferPos < pf->usBufferEnd)
      {
      usBytesRead = min (usBufferSize, pf->usBufferEnd - pf->usBufferPos);
      ItiMemCpy (pszBuffer, pf->szBuffer + pf->usBufferPos, usBytesRead);
      pf->usBufferPos += usBytesRead;
      pszBuffer += usBytesRead;
      usBufferSize -= usBytesRead;
      }

   /* fill the read request */
   if (usBufferSize)
      {
      usError = DosRead (pf->hf, pszBuffer, usBufferSize, &usTemp);
      usBytesRead += usTemp;
      }

   /* force buffer to fill if its empty */
   if (pf->usBufferPos >= pf->usBufferEnd)
      usError = !FillBuffer (pf);

   if (pusBytesRead)
      *pusBytesRead = usBytesRead;

   return usError == 0 && usBytesRead != 0;
   }



/*
 * ItiFileQueryPos returns the current file position of the given file.
 *
 * Parameters: pf            The file to check.
 *
 * Return value: A ULONG denoting the current file position.  0 is returned
 * on error.
 */
ULONG EXPORT ItiFileQueryPos (PFILE pf)
   {
   ULONG ulPos;

   if (DosChgFilePtr (pf->hf, 0, FROM_CURRENT, &ulPos))
      return 0;
   else
      return (ulPos - ((ULONG) pf->usBufferEnd - (ULONG) pf->usBufferPos));
   }





/*
 * ItiFileSetPos sets the file position for the given file.
 *
 * Parameters: pf            The file to set.
 *
 *             lPos              The new position.
 *
 *             usFromWhere       Use FROM_CURRENT, FROM_END, or
 *                               FROM_START.
 *
 * Return value: A ULONG denoting the current file position.  0 is returned
 * on error.
 */
ULONG EXPORT ItiFileSetPos (PFILE pf, LONG lPos, USHORT usFromWhere)
   {
   ULONG ulPos;

   if (DosChgFilePtr (pf->hf, 0, FROM_CURRENT, &ulPos))
       return 0;

   ulPos = ulPos - ((ULONG) pf->usBufferEnd - (ULONG) pf->usBufferPos);

   if (usFromWhere == FROM_CURRENT && 
       (lPos < 0 && pf->usBufferPos >= (USHORT) (-lPos)))
      {
      /* move buffer pointer backward */
      pf->usBufferPos -= (USHORT) (-lPos);
//      DosChgFilePtr (pf->hf, 0, FROM_CURRENT, &ulPos);
      }
   else if (usFromWhere == FROM_CURRENT && 
            (lPos >= 0 && pf->usBufferEnd < pf->usBufferPos + (USHORT) lPos))
      {
      /* move buffer pointer forward */
      pf->usBufferPos += (USHORT) lPos;
//      DosChgFilePtr (pf->hf, 0, FROM_CURRENT, &ulPos);
      }
   else
      {
      /* flush buffer */
      pf->usBufferPos = pf->usBufferSize;
      DosChgFilePtr (pf->hf, lPos, usFromWhere, &ulPos);
      }
   return ulPos;
   }




/*
 * ItiFileSkipLine skips to the next line.
 *
 * Parameters: pf            The file to use.
 *
 * Return value: TRUE if an error or the end of the file has been
 * reached.
 */

BOOL EXPORT ItiFileSkipLine (PFILE pf)
   {
   BOOL     bDone;

   bDone = FALSE;

   if (pf->usBufferPos >= pf->usBufferPos)
      if (!FillBuffer (pf))
         bDone = TRUE;

   while (!bDone)
      {
      bDone = pf->szBuffer [pf->usBufferPos] == '\n';
      pf->usBufferPos++;
      if (pf->usBufferPos >= pf->usBufferSize)
         if (!FillBuffer (pf))
            bDone = TRUE;
      }
   return pf->usBufferPos >= pf->usBufferSize;
   }





/*
 * ItiFileSkipBlankLines skips from the current line to the first
 * non-blank line.
 *
 * Parameters: pf            The file to use.
 *
 * Return value: The number of lines skipped.
 */

USHORT EXPORT ItiFileSkipBlankLines (PFILE pf)
   {
   USHORT usNumLines;
   USHORT usCharsOnLine;
   BOOL   bDone;

   usNumLines = 0;
   usCharsOnLine = 0;
   bDone = FALSE;
   do 
      {
      if (pf->usBufferPos >= pf->usBufferEnd)
         bDone = !FillBuffer (pf);
      else if (pf->szBuffer [pf->usBufferPos] == CHAR_LF)
         {
         usNumLines++;
         usCharsOnLine = 0;
         pf->usBufferPos++;
         }
      else if (pf->szBuffer [pf->usBufferPos] == CHAR_CR)
         {
         usCharsOnLine++;
         pf->usBufferPos++;
         }
      else if (!ItiCharIsSpace (pf->szBuffer [pf->usBufferPos]))
         bDone = TRUE;
      else
         {
         pf->usBufferPos++;
         usCharsOnLine++;
         }
      } while (!bDone);
   if (usCharsOnLine)
      ItiFileSetPos (pf, - ((LONG) usCharsOnLine), FROM_CURRENT);

   return usNumLines;
   }





#define FIRST_LINE_COUNT (sizeof apszFirstLine / sizeof (PSZ))

#define HIGHEST_IMPORT_FILE_WORD "HIGHEST"
#define DSSHELL_IMPORT_FILE_WORD "DS/Shell Import File"
#define ECSV_WORD                "ECSV"

/*
 * ItiFileOpenCSV opens a DS/Shell CSV File.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 *             pszFileName       The name of the file to open.
 *
 *             pszDSShellVersion A pointer to a DSShell version string.
 *
 *             pszHighEstVersion A pointer to a HighEst version string.
 * 
 * Return Value: 0 if the file was opened, or one of the following
 * errors:
 *    ERROR_COULD_NOT_OPEN
 *    ERROR_NOT_CSV
 *    ERROR_WRONG_VERSION
 *
 */

BOOL EXPORT ItiFileOpenCSV (PCSVFILEINFO pcfi,
                            PSZ          pszFileName,
                            PSZ          pszDSShellVersion,
                            PSZ          pszHighEstVersion)
   {
   char   szWord [256] = "";
   double dVersion = 0.0;
   double dValid = 0.0;
   double dDSShellVersion = 0.0;

   pcfi->ulLine   = 1;
   pcfi->ulColumn = 0;
   pcfi->bECSV    = FALSE;
   pcfi->bHighEst = FALSE;
   pcfi->bUnitSys = FALSE;
   pcfi->bMetric = FALSE;
   pcfi->pf       = ItiFileOpenRead (pszFileName);
   if (pcfi->pf == NULL)
      return ERROR_COULD_NOT_OPEN;

   if (!ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord))
      return ERROR_NOT_CSV;

   if (ItiStrICmp (szWord, DSSHELL_IMPORT_FILE_WORD) == 0)
      {
      /* a DS/Shell import file */
      //if (!ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord) ||
      //    ItiStrICmp (szWord, pszDSShellVersion))
      //   return ERROR_WRONG_VERSION;

      if (ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord))
         {
         dValid = ItiStrToDouble ("1.10");
         dDSShellVersion = ItiStrToDouble (pszDSShellVersion);
         dVersion = ItiStrToDouble (szWord);

         if (dVersion == dValid)
            ItiErrWriteDebugMessage ("Recognized file version 1.10 ");
         else if (dVersion == dDSShellVersion)
            ItiErrWriteDebugMessage ("Recognized file version 1.00 ");
         else
            {
            ItiErrWriteDebugMessage ("ERROR: Unrecognized file version.");
            return ERROR_WRONG_VERSION;
            }
            
         }

      }
   else if (ItiStrICmp (szWord, HIGHEST_IMPORT_FILE_WORD) == 0)
      {
      /* a HighEst import file */
      pcfi->bHighEst = TRUE;
      if (ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord))
         {             /* -- need to look in highest.c line 688  */
         dVersion = ItiStrToDouble (szWord);
         if (dVersion >= 1.02)
            pcfi->bUnitSys = TRUE;
         else if (ItiStrICmp (szWord, pszHighEstVersion))
            {
            ItiErrWriteDebugMessage ("ERROR: Unrecognized file version.");
            return ERROR_WRONG_VERSION;
            }
         else
            ;
         }
      }
   else
      return ERROR_NOT_CSV;

   if (ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord))
      pcfi->bECSV = ItiStrICmp (szWord, ECSV_WORD) == 0;

   ItiFileSkipCSVLine (pcfi); /* Skip rest of 1st line. */
   if (pcfi->bUnitSys)
      {
      ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord);
      if (ItiStrICmp (szWord, "UNITSYS")) /* the 'UNITSYS' is a literal */
         return ERROR_WRONG_VERSION;

      ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord);
      switch (szWord[0])
         {
         case 'N':
            ItiErrWriteDebugMessage ("Unit System specified is NIETHER.");
            break;
         case 'E':
            ItiErrWriteDebugMessage ("Unit System specified is ENGLISH.");
            break;
         case 'M':
            pcfi->bMetric = TRUE;
            ItiErrWriteDebugMessage ("Unit System specified is METRIC.");
            break;
         default:
            ItiErrWriteDebugMessage
               ("ERROR: Unit System specified is NOT RECOGNIZED.");
            return ERROR_WRONG_VERSION;
            break;
         }/* end switch */
      ItiFileSkipCSVLine (pcfi); /* Skip rest of 2nd line. */
      }/* end if (pcfi->bUnitSys */

   return 0;
   }



/*
 * ItiFileCloseCSV closes a DS/Shell CSV File.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 * Return Value: nothing.
 *
 */

void EXPORT ItiFileCloseCSV (PCSVFILEINFO pcfi)
   {
   ItiFileClose (pcfi->pf);
   pcfi->pf = NULL;
   }






/* 
 * ItiFileGetCSVWord gets the next CSV parameter from the file pf.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 *             pszWord           The word read from the file.
 *
 *             pusWordLen        A pointer to a USHORT that will contain
 *                               the number of characters in the word.
 *
 *             usWordMax         The maximum number of characters to 
 *                               store in pszWord.
 *
 * Return Value: FALSE if no more CSV words are on the line.  You
 * should call ItiFileSkipCSVLine to advance to the next line.
 *
 * Comments: If you buffer is not big enough to hold the contents of
 * the field, the field is truncated, and the remaining information  
 * for that field is lost.
 */

BOOL EXPORT ItiFileGetCSVWord (PCSVFILEINFO  pcfi,
                               PSZ           pszWord, 
                               PUSHORT       pusWordLen,
                               USHORT        usWordMax)
   {
   BOOL     bEOL, bQuote, bDone, bEOF;
/*   USHORT   usBytesRead, usWordLen; */
   USHORT   usWordLen, usCnt;
   PSZ pszTmp;

   bEOL = FALSE;
   bQuote = FALSE;
   bDone = FALSE;
   usWordLen = 0;
   bEOF = FALSE;

   if (pszWord == NULL)
      return FALSE;

   *pszWord = '\0';

/* -- Added in Jan 96, empty out the whole buffer. */
   pszTmp = pszWord;
   usCnt = 0;
   while ( (usCnt < usWordMax) && (pszTmp != NULL) )
      {
      (*pszTmp) = '\0';
      pszTmp++;
      usCnt++;
      }
/* -- */

   usWordMax--;      /* save space for the null terminator */
   while (usWordLen < usWordMax && !bEOL && !bDone)
      {
      if (pcfi->pf->usBufferPos >= pcfi->pf->usBufferEnd)
         bEOF = !FillBuffer (pcfi->pf);
      if (!bEOF)
         *pszWord = pcfi->pf->szBuffer [pcfi->pf->usBufferPos++];
//      bEOF = !ItiFileRead (pcfi->pf, pszWord, sizeof *pszWord, &usBytesRead);
//      if (bEOF || usBytesRead != sizeof *pszWord)
      if (bEOF)
         {
         bEOL = usWordLen == 0;
         break;
         }
      else switch (*pszWord)
         {
         case CHAR_EOF:
            bDone = TRUE;
            bEOL = usWordLen == 0;
            UNGET (pcfi->pf, 1);
            break;

         case CHAR_COMMENT: 
            if (pcfi->ulColumn == 0)
               {
               pcfi->ulLine += 1;
               ItiFileSkipLine (pcfi->pf);
               pcfi->ulLine += ItiFileSkipBlankLines (pcfi->pf);
               }
            else
               {
               pcfi->ulColumn += 1;
               pszWord++;
               usWordLen++;
               }
            break;

         case CHAR_BACKSLASH:
            pcfi->ulColumn += 1;
            if (pcfi->bECSV)
               {
               if (pcfi->pf->usBufferPos >= pcfi->pf->usBufferEnd)
                  bEOF = !FillBuffer (pcfi->pf);
               if (!bEOF)
                  *pszWord = pcfi->pf->szBuffer [pcfi->pf->usBufferPos++];
//               bEOF = !ItiFileRead (pcfi->pf, &c, sizeof c, &usBytesRead);
//               if (bEOF || usBytesRead != sizeof c)
               if (bEOF)
                  {
                  bDone = TRUE;
                  break;
                  }
               else if (*pszWord == CHAR_CR || *pszWord == CHAR_LF)
                  {
                  /* gobble up the cr or lf */
                  pcfi->ulColumn = 0;
                  if (*pszWord == CHAR_CR)
                     pcfi->ulLine += 1;
                  }
//               else if (c == CHAR_CR || c == CHAR_LF)
//                  {
//                  /* gobble up the cr or lf */
//                  pcfi->ulColumn = 0;
//                  if (c == CHAR_CR)
//                     pcfi->ulLine += 1;
//                  }
               else
                  {
                  pszWord++;
                  usWordLen++;

                  /* unget the character we just got */
                  UNGET (pcfi->pf, 1);
                  }
               }
            else
               {
               pszWord++;
               usWordLen++;
               }
            break;

         case CHAR_LF:
            if (bQuote)
               {
               pszWord++;
               usWordLen++;
               }
            break;

         case CHAR_CR:
            if (bQuote)
               {
               pcfi->ulColumn = 0;
               pcfi->ulLine += 1;
               /* uncomment this code to store carrige returns as well as 
                  line feeds. */
               /* pszWord++;   */
               /* usWordLen++; */
               }
            else
               {
               if (usWordLen == 0)
                  {
                  pcfi->ulColumn = 0;
                  bEOL = TRUE;
                  }
               else
                  bDone = TRUE;

               /* put back the character */
               UNGET (pcfi->pf, 1);
               }
            break;

         case CHAR_QUOTE:
            pcfi->ulColumn += 1;
            if (!bQuote && usWordLen == 0)
               bQuote = TRUE;
            else if (bQuote)
               {
               if (pcfi->pf->usBufferPos >= pcfi->pf->usBufferEnd)
                  bEOF = !FillBuffer (pcfi->pf);
               if (!bEOF)
                  *pszWord = pcfi->pf->szBuffer [pcfi->pf->usBufferPos++];
//               bEOF = !ItiFileRead (pcfi->pf, pszWord, sizeof *pszWord, &usBytesRead);
//               if (bEOF || usBytesRead != sizeof *pszWord)
               if (bEOF)
                  bDone = TRUE;
               else if (*pszWord == CHAR_QUOTE)
                  {
                  pszWord++;
                  usWordLen++;
                  }
               else 
                  {
                  bQuote = FALSE;
                  UNGET (pcfi->pf, 1);
                  }
               }
            else 
               {
               /* should I do an error? */
               pszWord++;
               usWordLen++;
               }
            break;

         case CHAR_COMMA:
            pcfi->ulColumn += 1;
            if (bQuote)
               {
               pszWord++;
               usWordLen++;
               }
            else
               bDone = TRUE;
            break;

         default:
            pcfi->ulColumn += 1;
            pszWord++;
            usWordLen++;
            break;
         }
      }
   if (!bEOL && !bDone && usWordLen == usWordMax)
      {
      /* skip up to the next word */
      ItiFileSkipCSVWord (pcfi, bQuote);
      }

   if (pusWordLen)
      *pusWordLen = usWordLen;
   *pszWord = '\0';
   return !(bEOL || bEOF);
   }





/*
 * ItiFileSkipCSVWord skips to the next CSV word.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 * Return Value: Returns TRUE on EOF or error.
 */

BOOL EXPORT ItiFileSkipCSVWord (PCSVFILEINFO  pcfi,
                                BOOL          bInQuote)
   {
   BOOL bDone, bBackslash, bEOF;
   USHORT usBytesRead;
   UCHAR c;

   bBackslash = FALSE;
   bDone = FALSE;
   bEOF = FALSE;
   while (!bDone && !bEOF)
      {
      bEOF = !ItiFileRead (pcfi->pf, &c, sizeof c, &usBytesRead);
      if (bEOF || usBytesRead != sizeof c)
         {
         bDone = TRUE;
         break;
         }
      else switch (c)
         {
         case CHAR_EOF:
            bDone = TRUE;
            bEOF = TRUE;
            break;

         case CHAR_BACKSLASH:
            pcfi->ulColumn += 1;
            if (pcfi->bECSV)
               bBackslash = !bBackslash;
            break;

         case CHAR_CR:
            pcfi->ulLine += 1;
            pcfi->ulColumn = 0;
            if (bBackslash)
               bBackslash = FALSE;
            else if (!bInQuote)
               {
               bDone = TRUE;
               pcfi->ulLine += ItiFileSkipBlankLines (pcfi->pf);
               }
            break;

         case CHAR_COMMA:
            if (!bInQuote)
               bDone = TRUE;
            break;

         case CHAR_QUOTE:
            pcfi->ulColumn += 1;
            if (bInQuote)
               {
               bEOF = !ItiFileRead (pcfi->pf, &c, sizeof c, &usBytesRead);
               if (bEOF || usBytesRead != sizeof c)
                  bDone = TRUE;
               else if (c != CHAR_QUOTE)
                  {
                  bInQuote = FALSE;
                  UNGET (pcfi->pf, 1);
                  }
               }
            break;

         default:
            pcfi->ulColumn += 1;
            break;
         }
      }
   return bEOF || usBytesRead != sizeof c;
   }



/*
 * ItiFileSkipCSVLine skips to the next CSV line.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 * Return Value: Returns TRUE on EOF or error.
 */

BOOL EXPORT ItiFileSkipCSVLine (PCSVFILEINFO  pcfi)
   {
   BOOL bQuote, bDone, bBackslash, bEOF;
   USHORT usBytesRead;
   UCHAR c;

   bBackslash = FALSE;
   bQuote = FALSE;
   bEOF = FALSE;
   bDone = FALSE;
   while (!bDone && !bEOF)
      {
      bEOF = !ItiFileRead (pcfi->pf, &c, sizeof c, &usBytesRead);
      if (bEOF || usBytesRead != sizeof c)
         {
         bDone = TRUE;
         break;
         }
      else switch (c)
         {
         case CHAR_EOF:
            bDone = TRUE;
            bEOF = TRUE;
            break;

         case CHAR_BACKSLASH:
            if (pcfi->bECSV)
               bBackslash = !bBackslash;
            break;

         case CHAR_CR:
            if (bBackslash)
               {
               bBackslash = FALSE;
               pcfi->ulLine += 1;
               }
            else if (!bQuote)
               {
               bDone = TRUE;
               pcfi->ulLine += ItiFileSkipBlankLines (pcfi->pf);
               }
            break;

         case CHAR_QUOTE:
            bBackslash = FALSE;
            bQuote = !bQuote;
            break;
         }
      }
   pcfi->ulColumn = 0;

   return bEOF || usBytesRead != sizeof c;
   }



/*
 * ItiFileSaveCSVPos saves the current CSV file position.
 *
 * Parameters: pcfiSave          Where to store the file position.
 *
 *             pcfi              The file whose position is to be saved.
 *
 * Return value: none.
 */ 

void EXPORT ItiFileSaveCSVPos (PCSVFILEINFO pcfiSave, PCSVFILEINFO pcfi)
   {
   pcfi->ulFilePos = ItiFileQueryPos (pcfi->pf);
   ItiMemCpy (pcfiSave, pcfi, sizeof (CSVFILEINFO));
   }






/*
 * ItiFileRestoreCSVPos restores a saved CSV file position.
 *
 * Parameters: pcfi              The file whose position is to be restored.
 *
 *             pcfiSaved         The saved file position.
 *
 * Return value: none.
 */ 

VOID EXPORT ItiFileRestoreCSVPos (PCSVFILEINFO pcfi, PCSVFILEINFO pcfiSaved)
   {
   ItiMemCpy (pcfi, pcfiSaved, sizeof (CSVFILEINFO));
   ItiFileSetPos (pcfi->pf, pcfi->ulFilePos, FROM_START);
   }






/*
 * ItiFileSkipCSVSection skips a CSV file section.  The function
 * assumes that it is at the beginning of a line.
 *
 * Parameters: pcfi              The CSV file to process.
 *
 *             bCaseSensitive    TRUE if you want a case sensitive
 *                               string compare, FALSE if not.
 *
 * Return Value: TRUE if at end of file or error.
 *
 * Comments: A CSV section is denoted by a set of lines whose
 * first field is the same.  At least one line (the current) is 
 * always skipped.
 *
 * RESTRICTION: Only the first 255 characters of the first
 * field is used for comparison.
 */

BOOL EXPORT ItiFileSkipCSVSection (PCSVFILEINFO pcfi, BOOL bCaseSensitive)
   {
   char        szWord [256], szTemp [256];
   BOOL        bEOF;
   BOOL        bDone;
   CSVFILEINFO cfiOld;

   if (!ItiFileGetCSVWord (pcfi, szWord, NULL, sizeof szWord))
      return ItiFileSkipCSVLine (pcfi);

   bDone = FALSE;
   bEOF = FALSE;
   while (!(bEOF = ItiFileSkipCSVLine (pcfi)) && !bDone)
      {
      ItiFileSaveCSVPos (&cfiOld, pcfi);
      if (ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         {
         if (bCaseSensitive)
            bDone = ItiStrCmp (szWord, szTemp);
         else 
            bDone = ItiStrICmp (szWord, szTemp);
         }
      }

   if (bDone)
      {
      /* reset file position to start of line */
      ItiFileRestoreCSVPos (pcfi, &cfiOld);
      }

   return bEOF;
   }




BOOL EXPORT ItiUtilXlateKeys (PSZ pszInBuffer, PSZ pszOutBuffer, USHORT usOutBufferSize)
   {
   /* Assumes clause like " AND HP.WorkType IN (1000789, ... AND ... "   */
   CHAR szWord [25], szKey [25];
   PSZ  pszP1, pszP2;

   pszP1 = pszInBuffer;
   while (pszP1 != NULL && ( (*pszP1) != ')' ) )



   return TRUE;
   }
