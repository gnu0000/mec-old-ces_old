/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1994 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program uses some of the CSV utility functions from            |
|       the CES module ItiUtil.                                             |
+--------------------------------------------------------------------------*/


#define     INCL_ERRORS 
#define     INCL_BASE
#define     INCL_DOS
#define     INCL_WIN

#include    "..\include\iti.h"     

#include    "..\include\itiutil.h"

#include    <stdio.h>
#include    "csvrep.h"





#define BUF_SIZE  256

/* Flags for OpenFile */
#define OF_REOPEN 0x8000
#define OF_EXIST  0x4000
#define OF_PROMPT 0x2000
#define OF_CREATE 0x1000
#define OF_CANCEL 0x0800
#define OF_VERIFY 0x0400
#define OF_DELETE 0x0200
#define OF_PARSE  0x0100

#define OF_READ      0x0080
#define OF_WRITE     0x0040
#define OF_READWRITE 0x0020

#define MAX_FNAME_LEN 80
#define FIELD_SIZE    256

#define END_OF_FILE   99
             
/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR     szAppName[] = " CSV Field Replacement Utility ";
static INT      iLevel = 0;
static HAB      hab;
static HMQ      hmq;
static HMODULE  hmod;

static HFILE    hfIn;
static HFILE    hfOut;

static USHORT   usWrote;
static USHORT   usRead;
static USHORT   usLen;
static USHORT   usFieldNumber;

static CHAR     szInFile           [64] = "";
static CHAR     szOutFile          [64] = "";
static CHAR     szTableName        [64] = "";
static CHAR     szFieldNumber      [64] = "";
static CHAR     szReplacementValue [64] = "";
static CHAR     szMatchValue       [64] = "";
static BOOL     bMatchTest = FALSE;

// Command line parameter order:
#define  INFILE            1
#define  OUTFILE           2
#define  TABLENAME         3
#define  FIELDNUMBER       4
#define  REPLACEMENTVALUE  5
#define  MATCHVALUE        6


/* ======================================================================= *\
 *                                              MODULE UTILITY FUNCTIONS   *
\* ======================================================================= */




USHORT ItiIniStatus (PSZ pszReportName, PSZ pszBuffer, INT iBufferSize)
   {
   /* not yet implemented */
   return 0;
   }





VOID DisplayUsage (VOID)
   {
   printf("\n  To REPLACE a CSV field value in a CES import file section: \n\r");

   printf("\n     CSVRep InFile OutFile TableName Field# ReplacementValue [MatchValue]\n\n");

   printf("\r  Note if the MatchValue is not present all the #th values will be changed\n");
   printf("\r  otherwise only those lines with matching values will change. \n\r ");
   }


/*
 *  OptionSwitch
 *     Processes a command line option switch.
 */
USHORT OptionSwitch (PSZ pszOptSwitch)
   {

   switch(pszOptSwitch[1])
      {
      case 'h':
      case 'H':
      case '?':
         /* Usage inquiry. */
         DisplayUsage();
         return 0;
         break;

      default:
         break;
      }
   return 0;
   }



/****************************************************************************\
* This function returns the OS/2 file handle if operation is successful,
* 0 otherwise.
*
* Effects:  Depend on wMode:
*     OF_READ:      open file for reading only
*     OF_WRITE:     open file for writing only
*     OF_READWRITE: open file for reading and writing
*     OF_CREATE:    create the file if it does not exist
*     OF_REOPEN:    open file using info in the reopen buffer
*     OF_EXIST:     test file existence
*     OF_PARSE:     fill reopen buffer, with no other action
\****************************************************************************/


BOOL EXPENTRY OpenFile(PSZ pszFile, PHFILE pHandle, USHORT wMode)
    {
    HFILE hFile;
    USHORT wAction = 0;
    USHORT wAttrs = 0;
    USHORT wOpenFlag = 0;
    USHORT wOpenMode = 0;
    CHAR   sz[MAX_FNAME_LEN] = "";


    if (wMode & OF_READ)
        {
        wOpenFlag = 0x01;               /* fail if it doesn't exist */
        wOpenMode = 0x20;               /* read only */
        }
    else if (wMode & OF_WRITE)
        {
        wOpenFlag = 0x11;               /* create if necessary */
        wOpenMode = 0x11;               /* write only */
        }
    else if (wMode & OF_READWRITE)
        {
        wOpenFlag = 0x11;               /* create if necessary */
        wOpenMode = 0x12;               /* read-write */
        }
    else if (wMode & OF_CREATE)
        {
        /* create and close file */
        wOpenFlag = 0x10;               /* fail if exists */
        wOpenMode = 0x10;               /* read only */
        }
    else
        {
        return FALSE;
        }

    if (DosOpen(pszFile, (PHFILE)&hFile, &wAction,
                (ULONG) 0, 0, wOpenFlag, wOpenMode, (ULONG) 0))
        return FALSE;

//    if (wMode & OF_CREATE)
//        {
//        if (DosClose(hFile))
//            return FALSE;
//        }
//    else
        *pHandle = hFile;

    return TRUE;
    }



USHORT GetFileLine (HFILE hf, PSZ pszBuffer, USHORT usBufferLen)
   {
   USHORT us = 0;
   USHORT usBytesRead = 0;
   BOOL   bEOL = FALSE;
   CHAR   ch = '\0';


   if ((pszBuffer == NULL) || (usBufferLen < 2)) 
      return (13);                           /* 2 because we need a minimum */
                                             /* of 1 digit character + '\0' */

   /* -- Erase buffer contents: */
   while ( (us < usBufferLen) && (pszBuffer[us] != '\0') )
      pszBuffer[us++] = '\0';

   while ((0 == DosRead(hf, pszBuffer, 1, &usBytesRead)) &&
          (usBytesRead == 1) && (*pszBuffer != '\n'))
      pszBuffer++;

   if (usBytesRead == 1)
      return 0;
   else
      return (END_OF_FILE);

   } /* End of Function GetFileLine */




/* ======================================================================= *\
\* ======================================================================= */
USHORT _cdecl main (int argc, char * argv[])
   {
   BOOL bEOL, bInTableSection;
   USHORT  Nth, rc, us = 0;
   USHORT  usLineCount;
   USHORT  usTableSectionLineCount, usChangedLineCount;
   CHAR szField [FIELD_SIZE] = "";
   CHAR szNewField [FIELD_SIZE] = "";
   PSZ  psz, pszNew;
   USHORT usFieldSize = FIELD_SIZE;
   CHAR szLine[512] = "";
   CHAR szOutLine[512] = "";

   hab = WinInitialize(0);
   if (hab == NULL)           
      {
      return (13);
      }

   if (argc < 5)
      {
      DisplayUsage();
      return 13;
      }
   else // Load parameters
      {
       ItiStrCpy (szInFile, argv[INFILE], sizeof szInFile);
       ItiStrCpy (szOutFile, argv[OUTFILE], sizeof szOutFile);
       ItiStrCpy (szTableName, argv[TABLENAME], sizeof szTableName);

       ItiStrCpy (szFieldNumber, argv[FIELDNUMBER], sizeof szFieldNumber);
       ItiStrToUSHORT (szFieldNumber, &usFieldNumber);

       ItiStrCpy (szReplacementValue, argv[REPLACEMENTVALUE], sizeof szReplacementValue);
       if (NULL != argv[MATCHVALUE])
          {  
          ItiStrCpy (szMatchValue, argv[MATCHVALUE], sizeof szMatchValue);
          bMatchTest = TRUE;
          }
        else
          bMatchTest = FALSE;
      }

   /* Setup files. */
   OpenFile (szInFile, (PHFILE)&hfIn, OF_READ);
   OpenFile (szOutFile, (PHFILE)&hfOut, OF_WRITE);

   fprintf(stderr, "\n\n CSV Field Replacement Utility, (C) 1994 Info Tech, Inc. \n");

// ===================================================
usTableSectionLineCount = 0;
usChangedLineCount = 0;
usLineCount = 0;
rc = 0;
while (END_OF_FILE != GetFileLine(hfIn, szLine, sizeof szLine) )
   {                                       
   // process a csv line
   bEOL = FALSE;
   Nth = 1;
   do
      {
      psz = ItiCsvGetField (Nth, szLine, szField, usFieldSize);

      if ( (Nth == 1) && (0 == ItiStrCmp(szField, szTableName)) )
         {
         bInTableSection = TRUE;
         usTableSectionLineCount++;
         }
      else if (Nth == 1)
         { 
         bInTableSection = FALSE;
         ItiStrCpy(szOutLine, szLine, sizeof szOutLine);
         break;
         }

      if ( ItiStrIsCharInString('\n', psz) ) 
         {
         bEOL = TRUE;
         }

      if ((Nth == usFieldNumber) && (bInTableSection) && (usTableSectionLineCount != 1))
         {
         if ((bMatchTest == FALSE) ||
             ((bMatchTest) && (0 == ItiStrCmp(szField, szMatchValue)) ) )
            {
            ItiStrCpy(szField, szReplacementValue, sizeof szField);
            usChangedLineCount++;
            }
         }

      pszNew = ItiCsvMakeField (szField, szNewField, sizeof szNewField);

      if (Nth == 1)
         { 
         ItiStrCpy(szOutLine, szNewField, sizeof szOutLine);
         }
      else
         {
         ItiStrCat(szOutLine, "," , sizeof szOutLine);
         ItiStrCat(szOutLine, szNewField, sizeof szOutLine);
         }

      /* -- Erase buffer contents: */
      us = 0;
      while ( (us < sizeof szNewField) && (szNewField[us] != '\0') )
         szNewField[us++] = '\0';

      Nth++;
      }
   while (bEOL == FALSE);


   usLen = ItiStrLen(szOutLine);
   us = DosWrite(hfOut, (PVOID) szOutLine, usLen, &usWrote);

   usLineCount++;
   if ( 0 == (usLineCount % 100))
      fprintf(stderr, ".");

   /* -- Erase buffer contents: */
   us = 0;
   while ( (us < usLen) && (szOutLine[us] != '\0') )
      szOutLine[us++] = '\0';
   }

 usTableSectionLineCount--;
 fprintf(stderr, "\n %d line(s) in the %s table section.\n", usTableSectionLineCount, szTableName);
 fprintf(stderr,   " %d line(s) changed.\n", usChangedLineCount );

// ===================================================

   /* Close out files. */
   DosClose (hfIn);
   DosClose (hfOut);

   /* -- Finished using the DLL. */
   WinTerminate(hab);

   return (us);
   }/* END OF main */

