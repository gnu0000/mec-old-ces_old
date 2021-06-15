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
#define   INCL_WINCOUNTRY

#include    "..\include\iti.h"     
// #include    "..\include\itiutil.h"

#include    <stdio.h>
#include    "splitcsv.h"


#define BUF_SIZE    256
#define OUT_LN_LEN  1225

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
static CHAR     szAppName[] = " CSV File Utility ";
static INT      iLevel = 0;
static HAB      hab;
static HMQ      hmq;
static HMODULE  hmod;

static HFILE    hfIn;
static HFILE    hfOut;

static FILE         *fpIn;
static FILE         *fpOut;

static USHORT   usWrote;
static USHORT   usRead;
static USHORT   usLen;
static USHORT   usFieldNumber;

#define ID_LEN    16
#define ID_NUMBER 64
static CHAR     aszIDs [ID_NUMBER] [ID_LEN];
static USHORT   usMaxID;

static CHAR     szInFile           [64] = "";
static CHAR     szOutFile          [64] = "";
static CHAR     szTableName        [64] = "";

static CHAR     szFieldNumber      [64] = "";

static CHAR   szStar0[10] = "-\\|/+  ";
static CHAR   szStar1[10] =  " °÷ú ";
static CHAR   szStar2[10] = "   ";
static CHAR   szStar3[10] = "ßÞÜÝ   ";
static PSZ    pszStar;

typedef BOOL (*PFNCSV) (PSZ, PSZ, USHORT);

// Command line parameter order:
#define  INFILE            1
#define  OUTFILE           2


/* ======================================================================= *\
 *                                              MODULE UTILITY FUNCTIONS   *
\* ======================================================================= */

USHORT IniStatus (PSZ pszReportName, PSZ pszBuffer, INT iBufferSize)
   {
   /* not yet implemented */
   return 0;
   }





VOID DisplayUsage (VOID)
   {
   fprintf(stderr,"\n Usage:  \n\r");
   fprintf(stderr,"\n    SPLITCSV InFile OutFile  ID_String  { ID_String } \n\n\r");
   fprintf(stderr," where InFile    is the name of the CSV data file (NOT an ECSV file). \n\r");
   fprintf(stderr,"       OutFile   is the result file to be generated. \n\r");
   fprintf(stderr,"       ID_String the unique ID string key(s). \n\r");
   fprintf(stderr,"\n  This utility will pull all of the specified ID_Strings from \n\r");
   fprintf(stderr,  "  the InFile, typically the CESPROP generated file.  \n\n\r");

   } /* End of Function DisplayUsage */


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
         fprintf(stderr, "\n Utility Program, (C) 1994 Info Tech, Inc.\n");
         DisplayUsage();
         return 0;
         break;

      default:
         break;
      }
   return 0;
   }



// #define toupper(c) (WinUpperChar (NULL, 0, 0, (c)))
#define toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c)+'A'-'a' : (c))
#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) ^ (char)0x20) : (c))

/*
 * LocStrICmp performs a case insignificant string compare.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

SHORT EXPORT LocStrICmp (PSZ pszStr1, PSZ pszStr2)
   {
   while ('\0' != *pszStr1 && 
          '\0' != *pszStr2 && 
          toupper (*pszStr1) == toupper (*pszStr2))
      {
      pszStr1++;
      pszStr2++;
      }

   return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
          (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);
   } /* End of Function LocStrICmp */



/*
 * LocStrLen returns the length of string pszString.
 *
 * Parameters: pszString      A pointer to a string.
 *
 * Return Value: The length of the string, not including the null 
 * terminator.
 */

USHORT EXPORT LocStrLen (PSZ pszString)
   {
   USHORT      i;

   if (pszString == NULL)
      return 0;

   for (i=0; '\0' != *pszString; i++)
      pszString++;

   return i;
   }



/*
 * LocStrCmp compares string pszStr1 to string pszStr2.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

SHORT EXPORT LocStrCmp (PSZ pszStr1, PSZ pszStr2)
   {
   while ('\0' != *pszStr1 && '\0' != *pszStr2 && *pszStr1 == *pszStr2)
      {
      pszStr1++;
      pszStr2++;
      }

   return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
          (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);
   } /* End of Function LocStrCmp */




PSZ EXPORT LocStrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax)
   {
   USHORT   i;

   if ((pszSource == NULL) || (pszDest == NULL))
      {
      pszDest = NULL;
      return pszDest;
      }

   for (i=0; i < usDestMax && pszSource [i] != '\0'; i++)
      {
      pszDest [i] = pszSource [i];
      }
   i = (i > usDestMax - 1) ? usDestMax - 1 : i;
   pszDest [i] = '\0';
   return pszDest;
   }/* End of Function LocStrCpy */


   

/*
 * LocStrChr returns a pointer to the first occurance of character c
 * in string pszStr.
 *
 * Parameters: pszString      A pointer to the string to search.
 *
 *             c              The character to search for.
 *
 * Return value: A pointer to the character c in the string if it is 
 * found, or NULL if it isn't.
 */

PSZ EXPORT LocStrChr (PSZ pszString, UCHAR c)
   {
   while (*pszString && *pszString != c)
      pszString++;
   return *pszString == c ? pszString : NULL;
   }




// === included csv.c file

static PSZ AdvanceAField (PSZ pszString);
static PSZ AdvanceQuotedField (PSZ pszString);
static void ConvertAField (PSZ pszString, PSZ pszResult, USHORT usResultSize);


PSZ EXPORT LocCSVGetField (USHORT usField, 
                           PSZ    pszCSVString, 
                           PSZ    pszResult,
                           USHORT usResultSize)
   {
   USHORT usFieldNumber;

   /* Grammar for pszCSVString is:
      pszCSVString     = Field {',' Field} .
      Field          = Empty | NonQuotedField | QuotedField .
      NonQuotedField = { NonQuoteComma } .
      QuotedField    = '"' { NonQuoteComma | ',' | '""' } '"' .
      Empty          = .
      NonQuoteComma  = anything other than '"' or ',' .
   */

   for (usFieldNumber = 1; usFieldNumber < usField; usFieldNumber++)
      {
      pszCSVString = AdvanceAField (pszCSVString);
      if (*pszCSVString != '\0')
         pszCSVString++;
      }

   ConvertAField (pszCSVString, pszResult, usResultSize);

   return pszResult;
   } /* End of Function */





static PSZ AdvanceAField (PSZ pszString)
   {
   switch (*pszString)
      {
      case ',':
      case '\0':
         return pszString;
         break;

      case '"':
         return AdvanceQuotedField (pszString);
         break;

      default:
         while (*pszString != ',' && *pszString != '\0')
            pszString++;
         return pszString;
         break;
      }

   return pszString;
   }






static PSZ AdvanceQuotedField (PSZ pszString)
   {
   pszString++;

   while (TRUE)
      {
      switch (*pszString)
         {
         case '\0':
            return pszString;
            break;

         case '"':
            if (*(pszString+1) == '"')
               pszString += 2;
            else
               {
               pszString++;
               return pszString;
               }
            break;

         default:
            pszString++;
            break;
         }
      }
   return pszString;
   } /* End of Function */



static void ConvertAField (PSZ pszString, PSZ pszResult, USHORT usResultSize)
   {
   switch (*pszString)
      {
      case ',':
      case '\0':
         *pszResult = '\0';
         return;
         break;

      case '"':
         pszString++;

         while (TRUE)
            {
            switch (*pszString)
               {
               case '\0':
                  *pszResult = '\0';
                  return;
                  break;

               case '"':
                  if (*(pszString+1) == '"')
                     {
                     pszString += 2;
                     *pszResult = '"';
                     pszResult++;
                     }
                  else
                     {
                     pszString++;
                     *pszResult = '\0';
                     return;
                     }
                  break;

               default:
                  *pszResult = *pszString;
                  pszString++;
                  pszResult++;
                  break;
               }
            }
         break;

      default:
         while (*pszString != ',' && *pszString != '\0')
            {
            *pszResult = *pszString;
            pszString++;
            pszResult++;
            }
         *pszResult = '\0';
         return;
         break;
      }
   } /* End of Function */



PSZ EXPORT LocCSVMakeField (PSZ pszInput, PSZ pszOutput, USHORT usOutputSize)
   {
   BOOL     bNeedsQuotes;
   USHORT   i;

   bNeedsQuotes = FALSE;

   i = 0;
   bNeedsQuotes = (LocStrChr (pszInput, ',') != NULL) ||
                  (LocStrChr (pszInput, '"') != NULL);

   if (!bNeedsQuotes)
      {
      LocStrCpy (pszOutput, pszInput, usOutputSize);
      return pszOutput;
      }

   /* quote the string, and double the internal quotes */
   usOutputSize -= 2;
   pszOutput [0] = '"';
   for (i=1; i < usOutputSize && '\0' != *pszInput;)
      {
      pszOutput [i++] = *pszInput;
      if ('"' == *pszInput)
         pszOutput [i++] = *pszInput;

      pszInput++;
      }

   pszOutput [i] = '"';
   pszOutput [i+1] = '\0';

   return pszOutput;
   } /* End of Function */

// === end of included csv.c file


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


BOOL HistoricalProposal (PSZ pszLineToCheck, PSZ szOutputLine, USHORT usOutLineLength);

/* ======================================================================= *\
\* ======================================================================= */
USHORT _cdecl main (int argc, char * argv[])
   {
   USHORT  Nth, rc, us = 0;
   USHORT  usLineCount;
   CHAR szMark[3] = "";
   PSZ  psz;
   PFNCSV  pfnProcessCSV;
   CHAR szLine[OUT_LN_LEN-1] = "";
   CHAR szOutLine[OUT_LN_LEN+1] = "";
   CHAR szField1 [FIELD_SIZE+1] = "";
   CHAR szField3 [FIELD_SIZE+1] = "";



   if (argc < 4)
      {
      DisplayUsage();
      return 13;
      }
   else // Load parameters
      {
       LocStrCpy (szInFile, argv[INFILE], sizeof szInFile);
       LocStrCpy (szOutFile, argv[OUTFILE], sizeof szOutFile);

       for (Nth = 0; Nth < ID_NUMBER; Nth++)
          aszIDs[Nth][0] = '\0';
       /* -- Load the proposal IDs from the command line. */
       us = 3;
       while (us < (USHORT)argc)
          {  
          LocStrCpy (aszIDs[us-3], argv[us], sizeof aszIDs[us-3]);
          us++;
          }
      usMaxID = argc - 3;
      }

   fprintf(stderr, "\n\n SPLITCSV Utility Program, (C) 1994 Info Tech, Inc. \n");

   switch (argc % 4)
        {
        case 3: pszStar = szStar3;
                break;
        case 2: pszStar = szStar2;
                break;
        case 1: pszStar = szStar1;
                break;
        case 0:
        default:
                pszStar = szStar0;
                break;
        }

   /* -- Select CSV processing function here. */
   pfnProcessCSV = HistoricalProposal;   /* Handles CESPROP files. */


// ===================================================
   /* -- Setup files. */
   OpenFile (szInFile, (PHFILE)&hfIn, OF_READ);
   if (END_OF_FILE != GetFileLine(hfIn, szLine, sizeof szLine) )
      {/* Check the header line. */
      psz = LocCSVGetField (1, szLine, szField1, FIELD_SIZE);
      psz = LocCSVGetField (3, szLine, szField3, FIELD_SIZE);

      while (psz != NULL && *psz && *psz != 'E')
         psz++;

      if ( (0 != LocStrCmp(szField1, "DS/Shell Import File"))
           || (*psz=='E' && *(psz+1)=='C' && *(psz+2)=='S' && *(psz+3)=='V'))
         { 
         fprintf(stderr, "\n Incorrect InFile header line: \n   ");
         fprintf(stderr, szLine);
         fprintf(stderr, "\n ");
         if (*psz=='E')
            fprintf(stderr, " ECSV files can not be processed by this utility program.\n ");
         return 13;
         }
      else /* Header line was okay. */
         {
         if (argc > (ID_NUMBER + 3))
            {
            fprintf(stderr, "\n WARNING: Too many command line ID_strings.\n\n");
            return 8;
            }


         OpenFile (szOutFile, (PHFILE)&hfOut, OF_WRITE);

         usLen = LocStrLen(szLine);
         us = DosWrite(hfOut, (PVOID) szLine, usLen, &usWrote);
         }
      } /* end if (END_OF_FILE... */
   else
      {
      DosClose (hfIn);
      // DosClose (hfOut);
      fprintf(stderr, " \n ERROR: Input file problem. \n ");
      return 13;
      }
      
//   fprintf(stderr, "\n");
   usLineCount = 0;
   rc = 0;
   while (END_OF_FILE != GetFileLine(hfIn, szLine, sizeof szLine) )
      {
      if ( pfnProcessCSV (szLine, szOutLine, OUT_LN_LEN) )
         {
         usLen = LocStrLen(szOutLine);
         us = DosWrite(hfOut, (PVOID) szOutLine, usLen, &usWrote);
   
         /* -- Erase output buffer line contents: */
         us = 0;
         while ( (us < usLen) && (szOutLine[us] != '\0') )
            szOutLine[us++] = '\0';
         }
   
      /* -- Show the user that something is happening. */
      szMark[0] = pszStar[ (rc % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, "     %s \r", szMark);
      rc++;
//      usLineCount++;
//      if ( 0 == (usLineCount % 100))
//         fprintf(stderr, ".");
   
      }/* End of while (END_OF_FILE... */
   
   // ===================================================

   /* Close out files. */
   DosClose (hfIn);
   DosClose (hfOut);
                       
   return (0);
   }/* END OF main */





/* -- Returns TRUE if pszOutputLine is to be written out. */
BOOL HistoricalProposal (PSZ pszInLine, PSZ pszOutputLine, USHORT usOutLineLength)
   {
   BOOL bEOL = FALSE;
   BOOL bUseIt = FALSE;
   USHORT Nth = 0;
   PSZ psz;
   CHAR szField [FIELD_SIZE+1] = "";
   CHAR szNewField [FIELD_SIZE+1] = "";
   USHORT usFieldSize = FIELD_SIZE;


   bEOL = FALSE;

   psz = LocCSVGetField (1, pszInLine, szNewField, FIELD_SIZE);
   if (0 != LocStrICmp(szNewField, szTableName))
     { /* We are in a new section, so write out the column headers. */
     LocStrCpy(pszOutputLine, pszInLine, usOutLineLength);
     LocStrCpy(szTableName, szNewField, FIELD_SIZE);
     return TRUE;
     }

   /* -- Check if we have a target ID. */
   psz = LocCSVGetField (2, pszInLine, szField, FIELD_SIZE);
   bUseIt = FALSE;
   Nth = 0;
   while (aszIDs[Nth][0] != '\0' && Nth < ID_NUMBER)
      {
      if (0 == LocStrCmp(szField, aszIDs[Nth]))
        {
        bUseIt = TRUE;
        break;
        }
      Nth++;
      }
   if (bUseIt == FALSE)
      return FALSE;

   /* -- Unique case for CESPROP csv files. */
   if (0 == LocStrICmp(szNewField, "HISTORICAL-PROJECT"))
      {
      psz = LocCSVGetField (3, pszInLine, szField, FIELD_SIZE);
      LocStrCpy (aszIDs[usMaxID], szField, sizeof aszIDs[usMaxID]);
      usMaxID++;
      }

   LocStrCpy(pszOutputLine, pszInLine, usOutLineLength);
   return TRUE;

   } /* End of Function HistoricalProposal */
