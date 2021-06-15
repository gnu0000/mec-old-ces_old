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
 * buildres.c
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\resource.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "dbqry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define  DATE        __DATE__ 
#define  TIME        __TIME__ 
#define MAXWINDOWS   500
#define MAXLABELS    2000
#define MAXDATAS     2000
#define MAXDLGS      2000
#define MAXLWDATRICS 500
#define MAXSTRINGS   1000
#define MAXCOLS      MAXSTRINGS

#define MAXPATH      258

#define STRSIZE     255        
#define BIGSTRSIZE  1024        

#define CHILD       1
#define DATA        2
#define DIALOG      3
#define END         4
#define FORMAT      5
#define LABEL       6
#define LIST        7
#define QUERY       8
#define STATIC      9
#define TITLE       10
#define TEXT        11
#define COLUMNS     12

#define INVALID     0xFFFF

char     szInFile   [80];
char     szOutFile1 [80];
char     szOutFile2 [80];
char     szOutFile3 [80];
char     szTmpFile  [80];
char     szCurrentLine  [1024];
USHORT   uLineNo;
BOOL     bTraceMode = FALSE;
BOOL     bFirstTime = TRUE;  /* --- init colid flag for data and dialog --- */

PSZ pszAppName;

USHORT CommandLine (PSZ pszLine, PSZ pszF1, PSZ pszF2, PSZ pszF3);





void ErrRes (PSZ psz1, PSZ psz2)
   {
   printf ("Buildres Error (%s:%d): %s %s\n",
            szInFile, uLineNo, psz1, psz2);
   printf ("<< %s >>\n", szCurrentLine);
   exit (1);
   }


typedef BOOL (EXPORT *PFNCOLINIT) (void);
typedef USHORT (EXPORT *PFNCOLSTRTOID) (PSZ pszCol);

PFNCOLSTRTOID pfnItiColStringToColId = NULL;
HMODULE hmodColId = 0;

static void InitColId (void)
   {
   PFNCOLINIT pfnItiColInitialize;
   char szPath [MAXPATH];
   PSZ pszPathEnd;

   strcpy (szPath, pszAppName);
   pszPathEnd = strrchr (szPath, '\\');
   if (pszPathEnd != NULL)
      *(pszPathEnd+1) = '\0';
   strcat (szPath, "colid.dll");
   
   hmodColId = ItiDllLoadDll (szPath, "0");
   if (hmodColId == 0)
      {
      printf ("Could not load dll %s.\n", szPath);
      exit (1);
      }

   pfnItiColInitialize = ItiDllQueryProcAddress (hmodColId, 
                                                 "ITICOLINITIALIZE");
   pfnItiColStringToColId = ItiDllQueryProcAddress (hmodColId, 
                                                    "ITICOLSTRINGTOCOLID");

   if (pfnItiColInitialize == NULL)
      {
      printf ("Could not find function ItiColInitialize in COLID.DLL\n");
      exit (1);
      }

   if (pfnItiColStringToColId == NULL)
      {
      printf ("Could not find function ItiColStringToColId in COLID.DLL\n");
      exit (1);
      }

   if (!pfnItiColInitialize ())
      {
      printf ("ItiColInitialize failed in COLID.DLL\n");
      exit (1);
      }
   }


/*
 * Returns TRUE or FALSE
 *
 */
USHORT IsCommentLine (PSZ pszStr)
   {
   ItiStrip (pszStr, " \t");
   if (strchr ("#;\n", *pszStr) != NULL)
      {
      if (!strncmp (pszStr, "#line ", 6))
         uLineNo = atoi (pszStr+6) - 1;
      return TRUE;
      }
   if (*pszStr == '\0' || (*pszStr == '\\' && pszStr[1] == '\\'))
      return TRUE;
   return FALSE;
   }


/* Reads a line from fpIn
 * returns FALSE at EOF
 */
fpos_t InputFilePos;
USHORT uOldLineNo;

USHORT ReadLine (PSZ psz, USHORT uLen, FILE *fpIn)
   {
   USHORT   i, c;

   do
      {
      if (feof (fpIn))
         return FALSE;
      fgetpos (fpIn, &InputFilePos);
      uOldLineNo = uLineNo;
      i = 0;
      while ((char)(c = fgetc (fpIn)) != '\n' && c != EOF && i < uLen -1)
         psz[i++] = (char) c;
      psz[i] = '\0';
      while ((char)c != '\n' && c != EOF)
         c = fgetc (fpIn);
      uLineNo++;
      }
   while (IsCommentLine (psz));
   strcpy (szCurrentLine, psz);
   return TRUE;
   }


void UnReadLine (FILE *fpIn)
   {
   fsetpos (fpIn, &InputFilePos);
   uLineNo = uOldLineNo;
   }



/*
 * Eats a word from the front of a string and returns
 * that word and the rest of the string.
 *  an or '|' is considered its own word
 * words ending in an L are striped of the offending character
 * returns 0 when there is nothing to return
 */
USHORT StripWord (PSZ *ppsz, PSZ pszWord, BOOL bStripL)
   {
   ItiStrip (*ppsz, " \t");

   /* test comment */
   if (**ppsz != '\0' && strchr ("|+-", **ppsz) != NULL)  /*get operator*/
      {
      *(pszWord++) = *((*ppsz)++);
      *pszWord = '\0';
      return 1;
      }
   else if (**ppsz == '\0')
      {
      return 0;
      }
   else                                   /* get word */
      {
      while (**ppsz != '\0' && strchr (" \t+-|", **ppsz) == NULL)
         *(pszWord++) = *((*ppsz)++);
      if (bStripL && (*(pszWord -1) == 'L' || *(pszWord -1) == 'l'))
         *(pszWord -1) = '\0';
      *pszWord = '\0';
      return 1;
      }
   }



/*
 * This is a modified atol fn.
 * this fn allows the input string to:
 *    expresion enclosed in parenthesis
 *    numbers followed by a L
 *    be hex octal or dec
 *    separated by logical or (|)
 *
 * This proc does Little error checking!
 */

LONG Itiatol (PSZ pszNum)
   {
   LONG lVal = 0L;
   char szWord [STRSIZE];
   int  cOperator;
   PSZ  pszEnd;

   ItiClip  (pszNum, " )\tUuLl");
   ItiStrip (pszNum, " (\t");
   cOperator = '|';
   while (StripWord (&pszNum, szWord, TRUE))
      {
      if (*szWord != '\0' && strchr ("|+-", *szWord) != NULL)
         cOperator = *szWord;
      else
         {
         if (*szWord < '0' || *szWord > '9')
            ErrRes ("Value expected in field :", szWord);

         switch (cOperator)
            {
            case '|': lVal |= strtoul (szWord, &pszEnd, 0); break;
            case '+': lVal += strtoul (szWord, &pszEnd, 0); break;
            case '-': lVal -= strtoul (szWord, &pszEnd, 0); break;
            }
         if (lVal == 0L &&
             ((USHORT) (ULONG)pszEnd <= (USHORT) (ULONG)szWord || *(pszEnd-1) != '0'))
            ErrRes ("Value expected in field :", szWord);
         }
      }
   return lVal;
   }



/*************************************************************/


void ReadCol (PSZ *ppszCSV, PSZ pszCol, USHORT uMax)
   {
   BOOL bQuote;

   *pszCol = '\0';
   *ppszCSV = ItiStrip (*ppszCSV, " ");
   bQuote  = ItiSingleQuotes (ppszCSV);

   do
      {
      if (bQuote && ItiTwoQuotes (ppszCSV))
         *(pszCol++) = '\"';
      else if (ItiSingleQuotes (ppszCSV))
         bQuote = !bQuote;
      else if ((!bQuote && **ppszCSV == ',') || **ppszCSV == '\0')
         break;
      else if (!bQuote && **ppszCSV == ' ')
         (*ppszCSV)++;
      else
         *(pszCol++) = *((*ppszCSV)++);
      }
   while (TRUE);
   *pszCol = '\0';
   *ppszCSV += !!(**ppszCSV == ',');
   }


void GetCol (USHORT uField, PSZ pszCSV, PSZ pszCol, USHORT uMax)
   {
   USHORT   i;

   for (i = 1; i <= uField; i++)
      ReadCol (&pszCSV, pszCol, uMax);
   }



/*************************************************************/


LONG GetCSVLong (SHORT uField, PSZ pszCSV)
   {
   unsigned char  szVal [STRSIZE];
   LONG lVal;

   GetCol (uField, pszCSV, szVal, sizeof szVal);
   if (!strlen (szVal))
      {
      char  sz[STRSIZE];

      sprintf (sz, "Blank CSV field found field: %d", uField);
      ErrRes (sz, "");
      }
   lVal = Itiatol (szVal);
   return lVal;
   }


LONG GetCSVULong (SHORT uField, PSZ pszCSV)
   {
   return (ULONG) GetCSVLong (uField, pszCSV);
   }


SHORT GetCSVShort (SHORT uField, PSZ pszCSV)
   {
   return (SHORT) GetCSVLong (uField, pszCSV);
   }


USHORT GetCSVUShort (SHORT uField, PSZ pszCSV)
   {
   return (USHORT) GetCSVLong (uField, pszCSV);
   }



PSZ GetCSVString  (SHORT uField, PSZ pszCSV, PSZ pszVal, USHORT usValMax)
   {
   GetCol (uField, pszCSV, pszVal, usValMax);
   return pszVal;
   }



/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/



/*
 * CHILDRESOURCEFILE Format:
 *
 *  USHORT --- Count of CD's
 *  { 
 *  CD     --- CD Structures Ordered by uParent field
 *  }            
 */
CD       acd [MAXWINDOWS];

void BuildChildInfo (FILE *fpIn, FILE *fpOut)
   {
   USHORT   i, uCount, uCommand;
   unsigned char szCSV [STRSIZE];
   CD       cd;

   uCount = 0;
   printf ("Building Child Info to %s...\n", szOutFile1);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      cd.uId         = GetCSVUShort (1, szCSV);
      cd.uChildId    = GetCSVUShort (2, szCSV);
      cd.ulKind      = GetCSVULong (3, szCSV);
      cd.uAddId      = GetCSVShort  (4, szCSV);
      cd.uChangeId   = GetCSVShort  (5, szCSV);
      cd.rcs.xLeft   = GetCSVShort  (6, szCSV);
      cd.rcs.yBottom = GetCSVShort  (7, szCSV);
      cd.rcs.xRight  = GetCSVShort  (8, szCSV);
      cd.rcs.yTop    = GetCSVShort  (9, szCSV);
      cd.uCoordType  = GetCSVUShort (10, szCSV);

      /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && cd.uId >= acd[i].uId; i++)
         ;
      if (uCount > i)
         memmove (&(acd[i+1]), acd+ i, (uCount - i) * sizeof (CD));
      acd[i] = cd;
      if (++uCount > MAXWINDOWS)
         ErrRes ("More ChildInfo Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (acd, sizeof (CD), uCount, fpOut);
   fclose (fpOut);
   if (uCommand)
      UnReadLine (fpIn);
   }



/*************************************************************************/



/*
 * DATARESOURCEFILE Format:
 *
 *  USHORT --- Count of DD's
 *  { 
 *  DD     --- DD Structures Ordered by uId field
 *  }            
 *
 * DATATXTRESOURCEFILE Format:
 *  null terminated strings only
 *  pointers maintained by DATARESOURCEFILE
 *
 *
 */
DD       add [MAXDATAS];

void BuildDataInfo  (FILE *fpIn, FILE *fpOut, FILE *fpTxtOut)
   {
   USHORT   i, uCount, uTxtPos, uStrLen, uCommand;
   unsigned char szCSV   [STRSIZE];
   unsigned char szLabel [STRSIZE];
   unsigned char szColName [STRSIZE];
   DD       dd;

   /* -- must init before replacing col names (colid.h)   --
      -- but only if this function or BuildDlg is called. --
      -- This is beacuse colid uses this program but does --
      -- not call this function.                          -- */
   if (bFirstTime)
      {
      bFirstTime = FALSE;
      InitColId ();
      }

   uCount = uTxtPos = 0;

   printf ("Building Data Info to %s and %s...\n", szOutFile1, szOutFile2);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      dd.uId         = GetCSVUShort (1, szCSV);
      dd.uNextId     = GetCSVUShort (2, szCSV);
      dd.rcs.xLeft   = GetCSVShort  (3, szCSV);
      dd.rcs.yBottom = GetCSVShort  (4, szCSV);
      dd.rcs.xRight  = GetCSVShort  (5, szCSV);
      dd.rcs.yTop    = GetCSVShort  (6, szCSV);
      dd.uCoordType  = GetCSVUShort (7, szCSV);
      dd.lFColor     = GetCSVULong  (8, szCSV);
      dd.lFlags      = GetCSVULong  (9, szCSV);
      GetCSVString (10, szCSV, szColName, sizeof szColName);
      GetCSVString (11, szCSV, szLabel,   sizeof szLabel);

      if ((dd.uColumn = pfnItiColStringToColId (szColName)) == INVALID)
         ErrRes ("Unable to Locate Col Id for ", szColName);

                         
      /*--- Write Data Format String & Store Offset ---*/
      dd.uOffset = uTxtPos;
      uTxtPos += (uStrLen = strlen (szLabel) + 1);
      fwrite (szLabel, sizeof (char), uStrLen, fpTxtOut); 

      /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && dd.uId >= add[i].uId; i++)
         ;
      if (uCount > i)
         memmove (add+ i+ 1, add+ i, (uCount - i) * sizeof (DD));
      add[i] = dd;
      if (++uCount > MAXDATAS)
         ErrRes ("More DataInfo Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (add, sizeof (DD), uCount, fpOut);
   fclose (fpOut);
   fclose (fpTxtOut);
   if (uCommand)
      UnReadLine (fpIn);
   }





/*************************************************************************/
/*
 * DIALOGRESOURCEFILE Format:
 *
 *  USHORT --- Count of DG's
 *  { 
 *  DG     --- DD Structures Ordered by uId field
 *  }            
 *
 * DIALOGTXTRESOURCEFILE Format:
 *  null terminated strings only
 *  pointers maintained by DIALOGRESOURCEFILE
 *
 */
DLGI    adlgi [MAXDLGS];

void BuildDialogInfo  (FILE *fpIn, FILE *fpOut, FILE *fpTxtOut)
   {
   USHORT   i, uCount, uTxtPos, uStrLen, uCommand;
   unsigned char szCSV     [STRSIZE];
   unsigned char szFmat    [STRSIZE];
   unsigned char szColName [STRSIZE];
   DLGI     dlgi;

   /* -- must init before replacing col names (colid.h)   --
      -- but only if this function or BuildData is called.--
      -- This is beacuse colid uses this program but does --
      -- not call this function.                          -- */
   if (bFirstTime)
      {
      bFirstTime = FALSE;
      InitColId ();
      }

   uCount = uTxtPos = 0;

   printf ("Building Dialog Info to %s and %s...\n", szOutFile1, szOutFile2);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      dlgi.uId          = GetCSVUShort (1, szCSV);
      dlgi.uAddQuery    = GetCSVUShort (2, szCSV);
      dlgi.uChangeQuery = GetCSVUShort (3, szCSV);
      GetCSVString (4, szCSV, szColName, sizeof szColName);
      dlgi.uCtlId       = GetCSVUShort (5, szCSV);
      GetCSVString (6, szCSV, szFmat, sizeof szFmat);
      uStrLen = strlen (szFmat) + 1;
      GetCSVString (7, szCSV, szFmat + uStrLen, sizeof szFmat - uStrLen);
      uStrLen += strlen (szFmat + uStrLen) + 1;

      if ((dlgi.uColId = pfnItiColStringToColId (szColName)) == INVALID)
         ErrRes ("Unable to Locate Col Id for ", szColName);

      /*--- Write Dialog Format Input and output Strings & Store Offset ---*/
      dlgi.uOffset = uTxtPos;
      uTxtPos += uStrLen;
      fwrite (szFmat, sizeof (char), uStrLen, fpTxtOut); 

      /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && dlgi.uId >= adlgi[i].uId; i++)
         ;
      if (uCount > i)
         memmove (adlgi+ i+ 1, adlgi+ i, (uCount - i) * sizeof (DLGI));
      adlgi[i] = dlgi;
      if (++uCount > MAXDLGS)
         ErrRes ("More DialogInfo Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (adlgi, sizeof (DLGI), uCount, fpOut);
   fclose (fpOut);
   fclose (fpTxtOut);
   if (uCommand)
      UnReadLine (fpIn);
   }






/*************************************************************************/



/*
 * LABELRESOURCEFILE Format:
 *
 *  USHORT --- Count of LD's
 *  { 
 *  LD     --- LD Structures Ordered by uId field
 *  }
 *
 * LABELTXTRESOURCEFILE Format:
 *  null terminated strings only
 *  pointers maintained by LABELRESOURCEFILE
 *
 *
 */
LD       ald [MAXLABELS];

void BuildLabelInfo  (FILE *fpIn, FILE *fpOut, FILE *fpTxtOut)
   {
   USHORT   i, uCount, uTxtPos, uStrLen, uCommand;
   unsigned char szCSV   [STRSIZE];
   unsigned char szLabel [STRSIZE];
   LD       ld;

   uCount = uTxtPos = 0;
   printf ("Building Label Info to %s and %s...\n", szOutFile1, szOutFile2);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      ld.uId         = GetCSVUShort (1, szCSV);
      ld.uNextId     = GetCSVUShort (2, szCSV);
      ld.rcs.xLeft   = GetCSVShort  (3, szCSV);
      ld.rcs.yBottom = GetCSVShort  (4, szCSV);
      ld.rcs.xRight  = GetCSVShort  (5, szCSV);
      ld.rcs.yTop    = GetCSVShort  (6, szCSV);
      ld.uCoordType  = GetCSVUShort (7, szCSV);
      ld.lFColor     = GetCSVULong  (8, szCSV);
      ld.lFlags      = GetCSVULong  (9, szCSV);
      GetCSVString (10, szCSV, szLabel, sizeof szLabel);


      /*--- Write Label String & Store Offset ---*/
      ld.uOffset = uTxtPos;
      uTxtPos += (uStrLen = strlen (szLabel) + 1);
      fwrite (szLabel, sizeof (char), uStrLen, fpTxtOut); 

      /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && ld.uId >= ald[i].uId; i++)
         ;
      if (uCount > i)
         memmove (ald+ i+ 1, ald+ i, (uCount - i) * sizeof (LD));
      ald[i] = ld;
      if (++uCount > MAXLABELS)
         ErrRes ("More LabelInfo Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (ald, sizeof (LD), uCount, fpOut);
   fclose (fpOut);
   fclose (fpTxtOut);
   if (uCommand)
      UnReadLine (fpIn);
   }




/*************************************************************************/

/*
 * STATICLWDATRESFILE Format:
 *
 *  USHORT --- Count of SM's
 *  { 
 *  SM     --- SM Structures Ordered by uId field
 *  }            
 *
 */
SM       asm [MAXLWDATRICS];


void BuildStaticMetrics  (FILE *fpIn, FILE *fpOut)
   {
   USHORT   i, uCount, uCommand;
   unsigned char szCSV [STRSIZE];
   SM       sm;

   uCount = 0;

   printf ("Building Static Window Metrics to %s...\n", szOutFile1);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      sm.uId         = GetCSVUShort (1, szCSV);
      sm.uAddId      = GetCSVShort  (2, szCSV);
      sm.uChangeId   = GetCSVShort  (3, szCSV);
      sm.rcs.xLeft   = GetCSVShort  (4, szCSV);
      sm.rcs.yBottom = GetCSVShort  (5, szCSV);
      sm.rcs.xRight  = GetCSVShort  (6, szCSV);
      sm.rcs.yTop    = GetCSVShort  (7, szCSV);
      sm.uCoordType  = GetCSVUShort (8, szCSV);
      sm.uOptions    = GetCSVUShort (9, szCSV);
      sm.lBColor     = GetCSVLong   (10, szCSV);

      /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && sm.uId > asm[i].uId; i++)
         ;
      if (uCount > i)
         memmove (asm+ i+ 1, asm+ i, (uCount - i) * sizeof (SM));
      asm[i] = sm;
      if (++uCount > MAXLWDATRICS)
         ErrRes ("More Static Met Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (asm, sizeof (SM), uCount, fpOut);
   fclose (fpOut);
   if (uCommand)
      UnReadLine (fpIn);
   }


/*************************************************************************/




/*
 * LISTLWDATRESFILE Format:
 *
 *  USHORT --- Count of LM's
 *  { 
 *  LM     --- LM Structures Ordered by uId field
 *  }            
 *
 */
LM       alm [MAXLWDATRICS];


void BuildListMetrics  (FILE *fpIn, FILE *fpOut)
   {
   USHORT   i, uCount, uCommand;
   unsigned char szCSV [STRSIZE];
   LM       lm;

   uCount = 0;

   printf ("Building List Window Metrics to %s...\n", szOutFile1);

   /*--- Read Into Array ---*/
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      lm.uId         = GetCSVUShort (1, szCSV);
      lm.uOptions    = GetCSVUShort (2, szCSV);
      lm.lBColor     = GetCSVLong  (3, szCSV);
      lm.lLBColor    = GetCSVLong  (4, szCSV);
      lm.uLabelYSize = GetCSVUShort (5, szCSV);
      lm.uDataYSize  = GetCSVUShort (6, szCSV);
      lm.uButtonId   = GetCSVUShort (7, szCSV);

     /*--- Add To Array In Order ---*/
      for (i = 0; i < uCount && lm.uId > alm[i].uId; i++)
         ;
      if (uCount > i)
         memmove (alm+ i+ 1, alm+ i, (uCount - i) * sizeof (LM));
      alm[i] = lm;
      if (++uCount > MAXLWDATRICS)
         ErrRes ("More List Met Structures than array Space", "");
      }

   /*--- Write Array ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (alm, sizeof (LM), uCount, fpOut);
   fclose (fpOut);
   if (uCommand)
      UnReadLine (fpIn);
   }


/*************************************************************************/

int _FAR_ cdecl IDCompare (void _FAR_ *idx1, void _FAR_ *idx2)
   {
   return ((IDX *)idx1)->uId - ((IDX *)idx2)->uId;
   }




/* TXTRESFILE Format:
 *
 * USHORT --- Index Size
 * {
 * IDX    --- Sorted Array of Indices to strings
 * }
 * {
 * null terminated strings (1 per IDX)
 * }
 *
 */
IDX      aidx [MAXSTRINGS];


void BuildText  (FILE *fpIn, FILE *fpOut)
   {
   USHORT   uId, uCount, uIndex, uTxtPos, uStrLen, uOldId, uCommand, uOldLN;
   unsigned char szCSV [STRSIZE];
   unsigned char szStr [BIGSTRSIZE];
   fpos_t   pos;

   uOldId = uCount = 0;

   printf ("Building Text to %s...\n", szOutFile1);

   /*--- read index ---*/
   fgetpos (fpIn, &pos);
   uOldLN = uLineNo;
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record - Add To Array ---*/
      aidx[uCount].uId = uId = GetCSVUShort (1, szCSV);

      /*-- dont add new index entry if continuation ---*/
      if ((uCount += (uId == uOldId ? 0 : 1)) > MAXSTRINGS)
         ErrRes ("More List Met Structures than array Space", "");

      uOldId = uId;
      }


   /*--- rewind infile ---*/
   fsetpos (fpIn, &pos);
   uLineNo = uOldLN;

   /*--- leave space for index ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (aidx, sizeof (IDX), uCount, fpOut);

   /*--- write strings / store offsets ---*/
   uOldId = uIndex =  0;
   uTxtPos = sizeof (USHORT) + sizeof (IDX) * uCount;
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record ---*/
      uId = GetCSVUShort (1, szCSV);
      GetCSVString (2, szCSV, szStr, sizeof szStr);

      /*--- Write Data String & Store Offset ---*/
      if (uId != uOldId)
         {
         fputc ('\0', fpOut);
         aidx[uIndex].uOffset = ++uTxtPos;
         uIndex++;
         }
      uTxtPos += (uStrLen = strlen (szStr));
      fwrite (szStr, sizeof (char), uStrLen, fpOut);
      uOldId = uId;
      }
   fputc ('\0', fpOut);

   /* sort index, rewind outfile & rewrite Index*/
   qsort (aidx, uCount, sizeof (IDX), IDCompare);
   fgetpos (fpOut, &pos);
   rewind (fpOut);
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (aidx, sizeof (IDX), uCount, fpOut);
   fsetpos (fpOut, &pos);
   fclose (fpOut);
   if (uCommand)
      UnReadLine (fpIn);
   }


/*************************************************************************/


int _FAR_ cdecl IDCompare2 (void _FAR_ *idx1, void _FAR_ *idx2)
   {
   return ((IDX2 *)idx1)->uId - ((IDX2 *)idx2)->uId;
   }



/* FORMATRESFILE Format:
 *
 * USHORT --- Index Size
 * {
 * IDX2   --- Sorted Array of Indices to strings
 * }
 * {
 * null terminated strings (n per IDX2)
 * }
 *
 */
IDX2    aidx2 [MAXSTRINGS];


void BuildFormat  (FILE *fpIn, FILE *fpOut)
   {
   USHORT   uId,        uCount,      uCommand,  
            uTxtPos,    uStrLen,     uOldId,
            uOldLN,     uCol;
   unsigned char szCSV [STRSIZE];
   unsigned char szStr [BIGSTRSIZE];
   fpos_t   pos;
   int      iIndex;

   printf ("Building Formats to %s...\n", szOutFile1);
   uOldId = uCount = 0;

   /*--- read index save pos and lineno because we rewind ---*/
   fgetpos (fpIn, &pos);
   uOldLN = uLineNo;

   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Record - Add To Array ---*/
      aidx2[uCount].uId = uId = GetCSVUShort (1, szCSV);

      /*-- dont add new index entry if continuation ---*/
      if ((uCount += (uId == uOldId ? 0 : 1)) > MAXSTRINGS)
         ErrRes ("More List Met Structures than array Space", "");

      uOldId = uId;
      }

   /*--- rewind infile ---*/
   fsetpos (fpIn, &pos);
   uLineNo = uOldLN;

   /*--- leave space for index ---*/
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (aidx2, sizeof (IDX2), uCount, fpOut);

   /*--- write strings / store offsets ---*/
   uOldId = 0;
   iIndex = -1;
   uTxtPos = sizeof (USHORT) + sizeof (IDX2) * uCount;
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (uCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;

      /*--- Read Records ---*/
      if ((uId = GetCSVUShort (1, szCSV)) != uOldId)
         {
         iIndex++;
         aidx2[iIndex].uOffset = uTxtPos;
         aidx2[iIndex].uId     = uId;
         aidx2[iIndex].uCount  = 0;
         }

      /*--- Write all format fields ---*/
      uCol = 2;
      do
         {
         GetCSVString (uCol, szCSV, szStr, sizeof szStr);
         if (szStr[0] == '\0')
            break;

         /*--- Write Data String & Store Offset ---*/
         uTxtPos += (uStrLen = strlen (szStr)) + 1;
         fwrite (szStr, sizeof (char), uStrLen, fpOut);
         fputc ('\0', fpOut);
         aidx2[iIndex].uCount++;
         uCol++;
         }
      while (1);
      uOldId = uId;
      }
   fputc ('\0', fpOut);

   /* sort index, rewind outfile & rewrite Index*/
   qsort (aidx2, uCount, sizeof (IDX2), IDCompare2);
   fgetpos (fpOut, &pos);
   rewind (fpOut);
   fwrite (&uCount, sizeof (USHORT), 1, fpOut);
   fwrite (aidx2, sizeof (IDX2), uCount, fpOut);
   fsetpos (fpOut, &pos);
   fclose (fpOut);
   if (uCommand)
      UnReadLine (fpIn);
   }


/*************************************************************************/
char  txt [60000];

int _FAR_ cdecl IDBCompare (void _FAR_ *idx1, void _FAR_ *idx2)
   {
   return stricmp (txt+ (((IDX *)idx1)->uOffset), txt+ (((IDX *)idx2)->uOffset));
   }

void PrintHeaderTop (FILE *fpOut, PSZ pszFileName)
   {
   fprintf (fpOut, "/*\n * %s \n *\n */\n\n", pszFileName);
   fprintf (fpOut, "/*************************************************************\n");
   fprintf (fpOut, " *!            !!!!!! DO NOT MODIFY THIS FILE !!!!!!    !!!!!*\n");
   fprintf (fpOut, " *!!             THIS FILE IS GENERATED BY BUILDRES      !!!!*\n");
   fprintf (fpOut, " *!!!         THE METADATA FOR THIS FILE IS IN THE SQL    !!!*\n");
   fprintf (fpOut, " *!!!!       SERVER, SO DO NOT TOUCH IT, OR THE CLIENT     !!*\n");
   fprintf (fpOut, " *!!!!!    VERSIONS OF THE SOFTWARE WILL BE OUT OF WACK!!   !*\n");
   fprintf (fpOut, " *************************************************************\n");
   fprintf (fpOut, " *                                                           *\n");
   fprintf (fpOut, " * Id's for column and table names are system generated.     *\n");
   fprintf (fpOut, " * You couldn't change them if your life depended on it.     *\n");
   fprintf (fpOut, " * Don't even try.  If you do, AASHTO will haunt you for     *\n");
   fprintf (fpOut, " * the rest of your life.                                    *\n");
   fprintf (fpOut, " * Therefore, if the column or table ID's have been changed  *\n");
   fprintf (fpOut, " * all resources that use column and table ID's should be    *\n");
   fprintf (fpOut, " * remade, and fresh copies have to be sent to everyone and  *\n");
   fprintf (fpOut, " * their immediate relatives MUST be sent a new copy of all  *\n");
   fprintf (fpOut, " * executables, as well as a sever SQL server data base      *\n");
   fprintf (fpOut, " * update.  See, you didn't really want to change that ID,   *\n");
   fprintf (fpOut, " * did you?                                                  *\n");
   fprintf (fpOut, " *************************************************************/\n");
   fprintf (fpOut, "\n\n\n\n\n");
   }


void PrintColumnHeaderBottom (FILE *fpOut)
   {
   fprintf (fpOut, "\n/* This value is returned when a table or column string cannot\n");
   fprintf (fpOut, "   be found in the metabase. */\n#define COLID_ERROR 0xffff\n");
   fprintf (fpOut, "\n\n/*\n");
   fprintf (fpOut, " * ItiColIsColInTable returns TRUE if the given column is in the\n");
   fprintf (fpOut, " * specified table.\n");
   fprintf (fpOut, " *\n");
   fprintf (fpOut, " * Parameters: pszTableName      The name of the table.  This\n");
   fprintf (fpOut, " *                               parameter may be NULL if usTable is\n");
   fprintf (fpOut, " *                               used.\n");
   fprintf (fpOut, " *\n */\n\n");
   fprintf (fpOut, "extern BOOL EXPORT ItiColIsColInTable (PSZ    pszTableName,\n");
   fprintf (fpOut, "                                       USHORT usTable,\n");
   fprintf (fpOut, "                                       PSZ    pszColumn,\n");
   fprintf (fpOut, "                                       USHORT usColumn);\n");
   fprintf (fpOut, "\n\n/*\n");
   fprintf (fpOut, " * ItiColStringToColId returns a column ID for a given column name.\n");
   fprintf (fpOut, " */\n\n");
   fprintf (fpOut, "extern USHORT EXPORT ItiColStringToColId (PSZ pszCol);\n");
   fprintf (fpOut, "\n\n\n/*\n");
   fprintf (fpOut, " * ItiColColIdToString returns pointer to a string for a given column ID.\n");
   fprintf (fpOut, " */\n\n");
   fprintf (fpOut, "extern PSZ EXPORT ItiColColIdToString (USHORT usCol);\n");
   fprintf (fpOut, "\n\n\n/*\n");
   fprintf (fpOut, " * ItiColStringToTableId returns a table ID for a given table name.\n");
   fprintf (fpOut, " */\n\n");
   fprintf (fpOut, "extern USHORT EXPORT ItiColStringToTableId (PSZ pszTable);\n");
   fprintf (fpOut, "\n\n\n/*\n");
   fprintf (fpOut, " * ItiColTableIdToString returns pointer to a string for a given table ID.\n");
   fprintf (fpOut, " */\n\n");
   fprintf (fpOut, "extern PSZ EXPORT ItiColTableIdToString (USHORT usTable);\n");
   fprintf (fpOut, "\n\n");
   fprintf (fpOut, "extern BOOL EXPORT ItiColInitialize (void); \n\n");
   fprintf (fpOut, "\n\n\n/*\n");
   fprintf (fpOut, " * ItiColGetDefaultValue returns a pointer to a string that\n");
   fprintf (fpOut, " * is the default value of the specified field.  If an invalid\n");
   fprintf (fpOut, " * table or column is given, NULL is returned.\n");
   fprintf (fpOut, " */\n\n");
   fprintf (fpOut, "extern PSZ EXPORT ItiColGetDefaultValue (PSZ    pszTableName,\n");
   fprintf (fpOut, "                                         USHORT usTable,\n");
   fprintf (fpOut, "                                         PSZ    pszColumn,\n");
   fprintf (fpOut, "                                         USHORT usColumn);\n");
   }

TABCOL atabcol [MAXSTRINGS];

void BuildColInfo  (FILE *fpIn, FILE *fpOut1, FILE *fpOut2, FILE *fpOut3)
   {
   unsigned    char szCSV [STRSIZE];
   USHORT      usCommand, usNumCols, usState;
   HQRY        hqry;
   HHEAP       hheap;
   PSZ         pszQuery;
   PSZ         *ppszCols;
   static BOOL bDataBaseInitialized = FALSE;
   PSZ         pszTabHeader = "tableid.h";
   PSZ         pszColHeader = "colid.h";
   FILE        *fpColHeader, *fpTabHeader;
   USHORT      usColLen, usIndex, usTextPos, usCount, usOffset;

   if (!bDataBaseInitialized)
      {
      if (!DbInit ())
         ErrRes ("Could not initialize the database module!", "");
      bDataBaseInitialized = TRUE;
      }

   hheap = ItiMemCreateHeap (0);
   if (NULL == hheap)
      ErrRes ("Could not create heap!", "");

   if ((fpColHeader = fopen (pszColHeader, "wt")) == NULL)
      ErrRes ("Could not open the column header file ", pszColHeader);

   if ((fpTabHeader = fopen (pszTabHeader, "wt")) == NULL)
      ErrRes ("Could not open the table header file ", pszTabHeader);

   /* :Columns cols.bin tabs.bin coltab.bin */
   printf ("Building Columns to %s and %s...\n", szOutFile1, pszColHeader);

   /* gobble up lines */
   while (ReadLine (szCSV, sizeof szCSV, fpIn))
      {
      if (usCommand = CommandLine (szCSV, NULL, NULL, NULL))
         break;
      }

   PrintHeaderTop (fpColHeader, pszColHeader);
   PrintHeaderTop (fpTabHeader, pszTabHeader);

   /* create #defines for columns */
   pszQuery = "SELECT ColumnName, ColumnKey "
              "FROM Columns "
              "ORDER BY ColumnName ";

   hqry = DbExecQuery (pszQuery, hheap, &usNumCols, &usState);
   if (hqry == NULL)
      ErrRes ("Could not execute query:", pszQuery);

   usIndex = 0; 
   usTextPos = 0;
   while (DbGetQueryRow (hqry, &ppszCols, &usState))
      {
      fprintf (fpColHeader, "#define c%-32s %5s\n", 
               ppszCols [0], ppszCols [1]);

      if (usIndex >= MAXSTRINGS)
         ErrRes ("Too many columns!", "");

      usColLen = strlen (ppszCols [0]) + 1;
      aidx [usIndex].uId = atoi (ppszCols [1]);
      aidx [usIndex].uOffset = usTextPos;
      memcpy (txt + usTextPos, ppszCols [0], usColLen);
      usTextPos += usColLen;
      usIndex++;

      ItiFreeStrArray (hheap, ppszCols, usNumCols);
      }
   PrintColumnHeaderBottom (fpColHeader);

   /* sort index to force case insensitivity */
   usCount = usIndex;
   qsort (aidx, usCount, sizeof (IDX), IDBCompare);

   /* normalize text offsets to file offset */
   usOffset = sizeof (USHORT) + sizeof (IDX) * usCount;
   for (usIndex = 0; usIndex < usCount; usIndex++)
      aidx [usIndex].uOffset += usOffset;

   /* write columns out */
   fwrite (&usCount, sizeof (USHORT), 1,         fpOut1);
   fwrite (aidx,     sizeof (IDX),    usCount,   fpOut1);
   fwrite (txt,      sizeof (CHAR),   usTextPos, fpOut1);

   printf ("Building Tables to %s and %s...\n", szOutFile2, pszTabHeader);

   /* create #defines for tables */
   pszQuery = "SELECT TableName, TableKey "
              "FROM Tables "
              "ORDER BY TableName ";

   hqry = DbExecQuery (pszQuery, hheap, &usNumCols, &usState);
   if (hqry == NULL)
      ErrRes ("Could not execute query:", pszQuery);

   usIndex = 0; 
   usTextPos = 0;
   while (DbGetQueryRow (hqry, &ppszCols, &usState))
      {
      fprintf (fpTabHeader, "#define t%-32s %5s\n", 
               ppszCols [0], ppszCols [1]);

      if (usIndex >= MAXSTRINGS)
         ErrRes ("Too many tables!", "");

      usColLen = strlen (ppszCols [0]) + 1;
      aidx [usIndex].uId = atoi (ppszCols [1]);
      aidx [usIndex].uOffset = usTextPos;
      memcpy (txt + usTextPos, ppszCols [0], usColLen);
      usTextPos += usColLen;
      usIndex++;

      ItiFreeStrArray (hheap, ppszCols, usNumCols);
      }

   /* sort index to force case insensitivity */
   usCount = usIndex;
   qsort (aidx, usCount, sizeof (IDX), IDBCompare);

   /* normalize text offsets to file offset */
   usOffset = sizeof (USHORT) + sizeof (IDX) * usCount;
   for (usIndex = 0; usIndex < usCount; usIndex++)
      aidx [usIndex].uOffset += usOffset;

   /* write tables out */
   fwrite (&usCount, sizeof (USHORT), 1,         fpOut2);
   fwrite (aidx,     sizeof (IDX),    usCount,   fpOut2);
   fwrite (txt,      sizeof (CHAR),   usTextPos, fpOut2);

   printf ("Building Columns and Tables to %s...\n", szOutFile3);
   pszQuery = "SELECT TableKey, ColumnKey, DefaultValue "
              "FROM TablesColumns "
              "ORDER BY TableKey, ColumnKey";

   hqry = DbExecQuery (pszQuery, hheap, &usNumCols, &usState);
   if (hqry == NULL)
      ErrRes ("Could not execute query:", pszQuery);

   usIndex = 0; 
   usTextPos = 0;
   while (DbGetQueryRow (hqry, &ppszCols, &usState))
      {
      if (usIndex >= MAXSTRINGS)
         ErrRes ("Too many Tables * Columns!", "");

      usColLen = strlen (ppszCols [2]) + 1;
      atabcol [usIndex].uTableId = atoi (ppszCols [0]);
      atabcol [usIndex].uColumnId = atoi (ppszCols [1]);
      atabcol [usIndex].uOffset = usTextPos;
      memcpy (txt + usTextPos, ppszCols [2], usColLen);
      usTextPos += usColLen;
      usIndex++;

      ItiFreeStrArray (hheap, ppszCols, usNumCols);
      }

   /* normalize text offsets to file offset */
   usCount = usIndex;
   usOffset = sizeof (USHORT) + sizeof (TABCOL) * usCount;
   for (usIndex = 0; usIndex < usCount; usIndex++)
      atabcol [usIndex].uOffset += usOffset;

   /* write tables out */
   fwrite (&usCount, sizeof (USHORT), 1,         fpOut3);
   fwrite (atabcol,  sizeof (TABCOL), usCount,   fpOut3);
   fwrite (txt,      sizeof (CHAR),   usTextPos, fpOut3);


   if (usCommand)
      UnReadLine (fpIn);

   ItiMemDestroyHeap (hheap);
   fclose (fpOut1);
   fclose (fpOut2);
   fclose (fpOut3);
   fclose (fpColHeader);
   fclose (fpTabHeader);
   }






/*************************************************************************/

/* determines if text line is a command line.
 * if it is not, it returns a zero.
 *
 * if it is, and the command is not end, the appropriate out files are ret
 * and the command value is returned
 *
 * command lines must have a colon ':' as the first character on the line
 */

USHORT CommandLine (PSZ pszLine, PSZ pszF1, PSZ pszF2, PSZ pszF3)
   {
   USHORT   uVal = 0;
   char     szCmd[STRSIZE];

   if (*(pszLine++) != ':')
      return 0;

   StripWord (&pszLine, szCmd, FALSE);
   if (!strnicmp (szCmd, "CHILD",  5))
      uVal = CHILD ;
   else if (!strnicmp (szCmd, "COLUMNS",7))
      uVal = COLUMNS;
   else if (!strnicmp (szCmd, "DATA",   4))
      uVal = DATA  ;
   else if (!strnicmp (szCmd, "DIALOG", 6))
      uVal = DIALOG  ;
   else if (!strnicmp (szCmd, "END",    3))
      uVal = END   ;
   else if (!strnicmp (szCmd, "FORMAT", 6))
      uVal = FORMAT;
   else if (!strnicmp (szCmd, "LABEL",  5))
      uVal = LABEL ;
   else if (!strnicmp (szCmd, "LIST",   4))
      uVal = LIST  ;
   else if (!strnicmp (szCmd, "QUERY",  5))
      uVal = QUERY ;
   else if (!strnicmp (szCmd, "STATIC", 6))
      uVal = STATIC;
   else if (!strnicmp (szCmd, "TITLE",  5))
      uVal = TITLE ;
   else if (!strnicmp (szCmd, "TEXT",  5))
      uVal = TEXT  ;

   if (!uVal || uVal == END || pszF1 == NULL)
      return uVal;
   *pszF1 = '\0';
   if ( !StripWord (&pszLine, pszF1, FALSE))
      ErrRes ("Output File Expected for command:", szCmd);

   if (pszF2 == NULL)
      return uVal;
   if (!StripWord (&pszLine, pszF2, FALSE))
      *pszF2 = '\0';

   if (pszF3 == NULL)
      return uVal;
   if (!StripWord (&pszLine, pszF3, FALSE))
      *pszF3 = '\0';
   return uVal;
   }


void OpenFiles (PSZ psz1,   PSZ psz2,   PSZ psz3, 
                FILE **fp1, FILE **fp2, FILE **fp3)
   {
   if ((*fp1 = fopen (psz1, "wb")) == NULL)
      ErrRes ("Unable to Open Output File :", psz1);
   strcpy (szOutFile1, psz1);

   if (fp2 != NULL)
      {
      if (*psz2 == '\0')
         ErrRes ("2nd Output File expected after:", psz1);
      else if ((*fp2 = fopen (psz2,  "wb")) == NULL)
         ErrRes ("Unable to Open Output File :", psz2);
      strcpy (szOutFile2, psz2);
      }
   else
      szOutFile2 [0] = '\0';

   if (fp3 != NULL)
      {
      if (*psz3 == '\0')
         ErrRes ("3nd Output File expected after:", psz2);
      else if ((*fp3 = fopen (psz3,  "wb")) == NULL)
         ErrRes ("Unable to Open Output File :", psz3);
      strcpy (szOutFile3, psz3);
      }
   else
      szOutFile3 [0] = '\0';
   }


PSZ BuildTmpFile (PSZ pszDest, PSZ pszSrc)
   {
   char  sz[80], szFail[80];
   RESULTCODES results;

   printf ("PreProcessing Input File...\n");
   sprintf (sz, "CL%c/P /nologo %s%c%c", '\0', pszSrc, '\0', '\0');
   if (DosExecPgm (szFail, sizeof szFail, EXEC_SYNC, sz, NULL, &results, "cl.exe"))
      ErrRes ("Problem running CL on ", pszSrc);

   for (; *pszSrc != '\0' && *pszSrc != '.'; *(pszDest++) = *(pszSrc++))
      ;
   strcat (pszDest, ".I");
   return pszDest;
   }


void MainLoop (PSZ pszFName)
   {
   FILE  *fpIn, *fpOut1, *fpOut2, *fpOut3;
   char  szLine[STRSIZE];
   char  szF1[STRSIZE];
   char  szF2[STRSIZE];
   char  szF3[STRSIZE];

   /*--- Open The Input file ---*/
   BuildTmpFile (szTmpFile, pszFName);
   if ((fpIn  = fopen (szTmpFile, "rt")) == NULL)
         ErrRes ("Unable to Open Temp File :", szTmpFile);

   uLineNo = 0;
   strcpy (szInFile, pszFName);

   while (ReadLine (szLine, sizeof szLine, fpIn))
      {   
      switch (CommandLine (szLine, szF1, szF2, szF3))
         {
         case 0:
         case END:
            break;
         case CHILD:
            OpenFiles (szF1, szF2, szF3, &fpOut1, NULL, NULL);
            BuildChildInfo     (fpIn, fpOut1);
            break;
         case COLUMNS:
            OpenFiles (szF1, szF2, szF3, &fpOut1, &fpOut2, &fpOut3);
            BuildColInfo       (fpIn, fpOut1, fpOut2, fpOut3);
            break;
         case DATA:
            OpenFiles (szF1, szF2, szF3, &fpOut1, &fpOut2, NULL);
            BuildDataInfo      (fpIn, fpOut1, fpOut2);
            break;
         case DIALOG:
            OpenFiles (szF1, szF2, szF3, &fpOut1, &fpOut2, NULL);
            BuildDialogInfo    (fpIn, fpOut1, fpOut2);
            break;
         case FORMAT:
            OpenFiles (szF1, szF2, szF3, &fpOut1, NULL, NULL);
            BuildFormat        (fpIn, fpOut1);
            break;
         case LABEL:
            OpenFiles (szF1, szF2, szF3, &fpOut1, &fpOut2, NULL);
            BuildLabelInfo     (fpIn, fpOut1, fpOut2);
            break;
         case LIST:
            OpenFiles (szF1, szF2, szF3, &fpOut1, NULL, NULL);
            BuildListMetrics   (fpIn, fpOut1);
            break;
         case TITLE:
         case QUERY:
         case TEXT:
            OpenFiles (szF1, szF2, szF3, &fpOut1, NULL, NULL);
            BuildText (fpIn, fpOut1);
            break;
         case STATIC:
            OpenFiles (szF1, szF2, szF3, &fpOut1, NULL, NULL);
            BuildStaticMetrics (fpIn, fpOut1);
            break;
         default:
            ErrRes ("Unknown command", szLine);
            break;
         }
      }
   fclose (fpIn);
   if (DosDelete (szTmpFile, 0))
     ErrRes ("Unable to Delete Temp File :", szTmpFile);
   }


void Msg (void)
   {
   printf (" BuildRes  Window Resource Builder     v2.2  %s %s\n\n", DATE, TIME);
   printf (" USAGE:  BUILDRES ResFile\n\n");
   printf (" Commands Recognized in the resource file:\n");
   printf ("   :CHILD   ChildResourceFile\n");
   printf ("   :COLUMNS ColumnResource       ColHeader\n");
   printf ("   :DATA    DataResourceFile     FormatResourceFile\n");
   printf ("   :DIALOG  DialogResourceFile   DialogFormatResFile\n");
   printf ("   :FORMAT  FormatResourceFile\n");
   printf ("   :LABEL   LabelResourceFile    TextResourceFile\n");
   printf ("   :LIST    ListResourceFile\n");
   printf ("   :QUERY   QueryResourceFile (same as :TEXT)\n");
   printf ("   :STATIC  StaticResourceFile\n");
   printf ("   :TEXT    TextResourceFile\n");
   printf ("   :TITLE   TitleResourceFile (same as :TEXT)\n\n");
   printf ("Colon commands must start at the beginning of the line.\n");
   printf ("Lines which have \\\\ as the first non-blank chars are comment lines.\n");
   printf ("\n");
   exit (0);
   }


int cdecl main (int argc, char *argv[])
   {
#ifdef PRERELEASE
   printf ("BuildRes Release "VERSION_STRING" "PRERELEASE"\nCopyright (c) "COPYRIGHT" Info Tech, Inc.\n");
#else
   printf ("BuildRes Release "VERSION_STRING"\nCopyright (c) "COPYRIGHT" Info Tech, Inc.\n");
#endif

   pszAppName = argv [0];
   if (argc != 2)
      Msg ();
   MainLoop (argv[1]);
   return 0;
   }



