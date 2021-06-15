/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.         |
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
#include    <string.h>
#include    <ctype.h>
#include    "rtf.h"

#define BUF_SIZE  255

#if !defined (EXPORT)
/* NOTE: The EXPORT macro goes between the function return type and the
 * function name, eg. PVOID EXPORT FunctionName (...).
 */
#define EXPORT _far _pascal _loadds
#endif   /* if !defined (EXPORT) */


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
#define FIELD_SIZE    255

#define TOKEN_NULL        0
#define FULL_LINE_BUFFER 90
#define NEED_NEXT_LINE   91
#define END_OF_FILE      99

#define NOT  ! 
#define PAD_SIZE  127

/* === BNF specific definitions */
#define CMDLN_CONTINUATION  ','

             
/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR     szAppName[] = " RTF BNF Parser Utility Prototype ";
static CHAR     szStar[10] = "-\\|/+   ";
static char     szPad[PAD_SIZE+1];
static char     szCurrToken[PAD_SIZE+1] = "";

/* -- Error message IDs */
#define ERROR_NONE                    0
#define ERROR_UNRECOGNIZED_COMMAND    1
#define ERROR_FULL_LINE_BUFFER        2

#define MAX_LEN_ERRMSG   250
#define MAX_IDX_ERRMSG     5

#define ERROR_INVALID_MSG_ID  (MAX_IDX_ERRMSG+1)
#define DISPLAY_ERR_LIMIT     ERROR_INVALID_MSG_ID

struct ErrorMsg {
   int  iErrorNumber;  /* should be the same as the array index! */
   char cErrorClass;
   char szMessage[MAX_LEN_ERRMSG];
   } asErrorMsg[MAX_IDX_ERRMSG+2];


static char     aszErrorCodes [MAX_IDX_ERRMSG+1] [MAX_LEN_ERRMSG+1];


#define MAX_LEN_CMDWORD 10
#define MAX_LEN_KEYWORD 15
#define MAX_IDX_CMDWORD 23
#define MAX_IDX_KEYWORD 38
static char     aszCmdWord [MAX_IDX_CMDWORD+1] [MAX_LEN_CMDWORD+1];
static char     aszKeyWord [MAX_IDX_KEYWORD+1] [MAX_LEN_KEYWORD+1];

static INT      iLevel = 0;
static HAB      hab;
static HMQ      hmq;
static HMODULE  hmod;

static HFILE    hfIn;
static HFILE    hfOut;
static HFILE    hfERROROut;
static HFILE    hfTree;

static USHORT   usWrote;
static USHORT   usRead;
static USHORT   usLen;
static USHORT   usLineCount;
static char * pszCurrentLocation = NULL;

static USHORT   usFieldNumber;

static CHAR     szLine[512] = "";
static CHAR     szInFile           [64] = "";
static CHAR     szOutFile          [64] = "";
static CHAR     szErrorFile        [255] = "RTFerror.txt";

static CHAR     szTreeFile         [] = "RTF_tree.txt";
static BOOL     bGenerateTree = FALSE;
static USHORT   usIndentLevel = 0;

// Command line parameter order:
#define  INFILE            1
#define  OUTFILE           2

// enum TOKEN { UNKNOWN , CMDSYM, COMMNT } TOKN;
#define UNKNOWN   0
#define CMD_SYM   1
#define COMMNT    2
#define TOKEN    USHORT  
               

/* ======================================================================= *\
 *                                              MODULE UTILITY FUNCTIONS   *
\* ======================================================================= */

//USHORT EXPORT ItiIniStatus (PSZ pszReportName, PSZ pszBuffer, INT iBufferSize)
//   {
//   /* not yet implemented */
//   return 0;
//   }
//

VOID EXPORT InitErrorMessages (VOID)
   {
   /* -- The error messages have 3 replacable sprintf parameters  */
   /* -- the first is the error ID number, the second is the line */
   /* -- number and the third is a text string.                   */
   // Later we will init these error messages from a database table
   // but for now init them here.
   strcpy (asErrorMsg[ERROR_NONE].szMessage,             "\n NO ERROR, %d: At current line %d. %s \n");
   strcpy (asErrorMsg[ERROR_UNRECOGNIZED_COMMAND].szMessage, "\n *** Error %d, Line %d: UNRECOGNIZED COMMAND NAME. THE NAME \"%s\" IS NOT A VALID RTF COMMAND. \x007 \n");
   strcpy (asErrorMsg[ERROR_INVALID_MSG_ID].szMessage,   "\n *** ERROR %d: Invalid error message ID used on line %d. %s \x007 \n");
//( 1).class = 'E'; $( 1).message = "\n *** Error %d, Line %d: UNRECOGNIZED COMMAND NAME. THE NAME %s IS NOT A VALID RTF COMMAND.";

   return;
   }




int EXPORT ErrorMsg (int iErrorID, char * pszMsgText)
   {
   static char szBuff[MAX_LEN_ERRMSG + 31] = "";
   int usRet, usLength, usWrote;

   /* -- Determine the given error code's validity. */
   if (iErrorID > MAX_IDX_ERRMSG)
      iErrorID = ERROR_INVALID_MSG_ID;

   if (pszMsgText == NULL)
      pszMsgText = "  ";

   /* -- If the valid code is to be displayed on the screen... */
   if ((iErrorID > 0) && (iErrorID <= DISPLAY_ERR_LIMIT))
      fprintf (stderr, asErrorMsg[iErrorID].szMessage, iErrorID, usLineCount, pszMsgText);

   /* -- Write the error to the log file. */
   sprintf (szBuff, asErrorMsg[iErrorID].szMessage, iErrorID, usLineCount, pszMsgText);   
   usLength = ItiStrLen(szBuff);
   usRet = DosWrite(hfERROROut, (PVOID) szBuff, usLength, &usWrote);

   /* -- If the error is severe enough to stop cold turkey... */

   /* -- Return the error code. */
   return iErrorID;
   }/* End of Function */



VOID EXPORT InitReservedWords (VOID)
   {
   /*- init list of valid commands -*/
   strcpy( aszCmdWord[0], "%");           /* command symbol */
   strcpy( aszCmdWord[1], "TITLE");
   strcpy( aszCmdWord[2], "ENDTITLE");
   strcpy( aszCmdWord[3], "FORMAT");
   strcpy( aszCmdWord[4], "NICKNAME");
   strcpy( aszCmdWord[5], "PAGE");
   strcpy( aszCmdWord[6], "SKIP");
   strcpy( aszCmdWord[7], "IF");
   strcpy( aszCmdWord[8], "ENDIF");
   strcpy( aszCmdWord[9], "REPEAT");
   strcpy( aszCmdWord[10],"ENDREPEAT");
   strcpy( aszCmdWord[11],"CENTER");
   strcpy( aszCmdWord[12],"ENDCENTER");
   strcpy( aszCmdWord[13],"INCLUDE");
   strcpy( aszCmdWord[14],"EXIT");
   strcpy( aszCmdWord[15],"GET");
   strcpy( aszCmdWord[16],"WHEN");
   strcpy( aszCmdWord[17],"SET");
   strcpy( aszCmdWord[18],"ELSE");
   strcpy( aszCmdWord[19],"ENDELSE");
   strcpy( aszCmdWord[20],"LEAVE");
   strcpy( aszCmdWord[21],"LET");
   strcpy( aszCmdWord[22],"*");  /* comment command symbol */
   aszCmdWord[23][0] = '\0';

  /*- list of reserved keywords -*/
   strcpy( aszKeyWord[0], "ITIDEBUG");
   strcpy( aszKeyWord[1], "THEN");
   strcpy( aszKeyWord[2], "OVER");
   strcpy( aszKeyWord[3], "WHERE");
   strcpy( aszKeyWord[4], "TO");
   strcpy( aszKeyWord[5], "TITLE");
   strcpy( aszKeyWord[6], "NEW");
   strcpy( aszKeyWord[7], "IS");
   strcpy( aszKeyWord[8], "OVERFLOW");
   strcpy( aszKeyWord[9], "TEMPLATE");
   strcpy( aszKeyWord[10],"LISTING");
   strcpy( aszKeyWord[11],"MESSAGE");
   strcpy( aszKeyWord[12],"LEVEL");
   strcpy( aszKeyWord[13],"COMMAND");
   strcpy( aszKeyWord[14],"LINE");
   strcpy( aszKeyWord[15],"AFTER");
   strcpy( aszKeyWord[16],"WHERE_OR_AFTER");
   strcpy( aszKeyWord[17],"CHARACTER");
   strcpy( aszKeyWord[18],"PAGESIZE");
   strcpy( aszKeyWord[19],"CONTINUE");
   strcpy( aszKeyWord[20],"ABORT");
   strcpy( aszKeyWord[21],"ON");
   strcpy( aszKeyWord[22],"OFF");
   strcpy( aszKeyWord[23],"ALL");
   strcpy( aszKeyWord[24],"WARNINGS");
   strcpy( aszKeyWord[25],"ERRORS");
   strcpy( aszKeyWord[26],"EXIT");
   strcpy( aszKeyWord[27],"NONE");
   strcpy( aszKeyWord[28],"USING_KEY");
   strcpy( aszKeyWord[29],"FOR");
   strcpy( aszKeyWord[30],"GETTING");
   strcpy( aszKeyWord[31],"GET");
   strcpy( aszKeyWord[32],"NO");
   strcpy( aszKeyWord[33],"LONGER");
   strcpy( aszKeyWord[34],"LOCK");       
   strcpy( aszKeyWord[35],"BEHAVIOR");   
   strcpy( aszKeyWord[36],"SHIFTING");   
   strcpy( aszKeyWord[37],"FIELDS_ONLY");
   aszKeyWord[38][0] = '\0';

   }/* End of Function InitReservedWords */



VOID EXPORT DisplayUsage (VOID)
   {
   fprintf(stderr,"  Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.\n");
   printf(        "  This program will parse an RTF file for BNF compliance. \n\r");
   printf(        "\n      Usage:  RTF InFile OutFile [-options]\n\n");
   printf(          "       options:  -?   This help screen.\n");
   printf(          "                 -e   Test write the error messages. \n");
   printf(          "                 -t   Write the parsed tree to the RTF_tree.txt file. \n");
   printf(          "                 -\x092   Fix All Errors and solve all problems.  \x007 \n");
   }


/*
 *  OptionSwitch
 *     Processes any command line option switchs.
 */
USHORT EXPORT OptionSwitch (char * argvs[], int argcnt)
   {
   int k;

   if (argcnt < 1)
      {
      DisplayUsage();
      return 13;
      }

   while ( --argcnt > 0)
      {
      if ((argvs[argcnt][0] == '-') || (argvs[argcnt][0] == '/'))
         switch(argvs[argcnt][1])
            {
            case 'e':     /* Test the error messages */
            case 'E' :
               fprintf (stderr, "\n Displaying, and writing to the error log, of the current RTF error messages. \n \n");
               for (k = 0; k <= MAX_IDX_ERRMSG; k++)
                  ErrorMsg ( k, " Error Test" );

               return 13;
               break;

            case 't': /* -- Write the parsed tree to a file. */
            case 'T':
               bGenerateTree = TRUE;
               fprintf (stderr, "\x007");
               break;

            case 'h':
            case 'H':
            case '?':
               /* Usage inquiry. */
               DisplayUsage();
               return 13;
               break;

            default:
               fprintf (stderr, "\n  Unknown option switch: %s \n\n", argvs[argcnt]);
               DisplayUsage();
               return 13;
               break;
            }/* end of switch */
     }/* end of while */

   if (argcnt < 3)
      {
      DisplayUsage();
      return 13;
      }

   return 0;
   }


/****************************************************************************\
* This function sets the file handle if operation is successful
* and returns TRUE, or FALSE otherwise.
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
BOOL EXPENTRY OpenFile (PSZ pszFile, PHFILE pHandle, USHORT wMode)
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
static   PSZ  pszBufferPointer = NULL;
static   PSZ  pszCmdlnCont = NULL;
   CHAR   szMark[3] = "";
   USHORT usCnt, us = 0;
   USHORT usBytesRead = 0;
   BOOL   bKeepReading;
   BOOL   bEOL = FALSE;
   BOOL   bCmdlnContinuation = FALSE;
   BOOL   bInsideQuote = FALSE;
   CHAR   ch = '\0';

   if ((pszBuffer == NULL) || (usBufferLen < 2)) 
      return (13);                           /* 2 because we need a minimum */
                                             /* of 1 digit character + '\0' */
   pszBufferPointer = pszBuffer;
   pszCmdlnCont = NULL;

   /* -- Erase buffer contents: */
   while ( (us < usBufferLen) && (pszBuffer[us] != '\0') )
      pszBuffer[us++] = '\0';
      
   usCnt = 0;
   bKeepReading = TRUE;
   while ((bKeepReading) 
          && (0 == DosRead(hf, pszBufferPointer, 1, &usBytesRead))
          && (usBytesRead == 1) )
      {
      usCnt++;
      /* -- Zeroth, test for the end of the buffer. */
      if (usCnt == (usBufferLen - 1))
         {
         bKeepReading = FALSE;
         /* -- Issue a "Full buffer" error message. */
         return ErrorMsg (ERROR_FULL_LINE_BUFFER, " -- ");
         }

      /* -- First, test for a qoute mark so the quote flag can be toggled. */
      if (*pszBufferPointer == '\'')
         {
         if (!bInsideQuote)
            /* ...then we are starting a quote... */
            bInsideQuote = TRUE;
         else
            /* ...we are already in a quotation and are now at its end. */
            bInsideQuote = FALSE;
         }

      /* -- Second, if we are not inside a quoted string... */
      if (!bInsideQuote)
         {
         /* ...and encounter the continuation marker... */
         if (*pszBufferPointer == CMDLN_CONTINUATION )
            {
            if (!bCmdlnContinuation)
               /* ...then set continuation marker true for now. */
               bCmdlnContinuation = TRUE;
            else
               ; /* do nothing for now. */

            /* Point to the latest continuation marker for the line. */
            pszCmdlnCont = pszBufferPointer;
            }
         else 
            { /*  we are not at a continuation marker, so...              */
            /*    if the continuation flag is set and we encounter        */
            /*    non-white space before the EOL then we must unset the   */
            /*    flag; the mark is part of some syntactical construct.   */ 
            if (bCmdlnContinuation)
               {
               if ((*pszBufferPointer != '\n') && (!isspace(*pszBufferPointer)) )
                  {
                  bCmdlnContinuation = FALSE; /* Reset the marker to false */
                  pszCmdlnCont = NULL;    /* and remove the pointer to it. */
                  }

               if (*pszBufferPointer == '\n')
                  {
                  if (NULL != pszCmdlnCont)
                     {
                     /* - remove continuation marker */
                     (*pszCmdlnCont) = ' ';

                     /* - remove the newline char to append next physical line */
                     (*pszBufferPointer) = ' ';

                     /* - If CR preceeds the newline char, then over write it. */
                     if ((*(pszBufferPointer - 1)) == '\x0d')
                        {
                        pszBufferPointer--;
                        (*pszBufferPointer) = ' ';
                        pszBufferPointer--;
                        }
                     }

                  if (!bCmdlnContinuation) 
                     bKeepReading = FALSE;
                  }
               }/* end if (bCmdlnContinuation... */

            if (*pszBufferPointer == '\n')
               { /* then we are at end of the line */
               bKeepReading = FALSE;
               }
            } /* end else clause */
         } /* end of IF (!bInsideQuote... then clause  */
      else /*  we are inside of a quote... */
         { 
         if (*pszBufferPointer == '\n')
            { /* ...and reached the end of the line. */
            bKeepReading = FALSE;
            //ERROR call here : unmatched quote mark on the current line
            fprintf (stderr, "Unmatched quote mark error on line %d \n", usLineCount);
            }
         }

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (usCnt % 6) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s   Processing line: %d \r", szMark, usLineCount);

      /* -- Lastly, increment buffer pointer. */
      pszBufferPointer++;
      }

   if (usBytesRead == 1)
      return 0;
   else
      return (END_OF_FILE);

   } /* End of Function GetFileLine */

TOKEN GetToken ( void )
   {
   USHORT  us, usLen, usWrote;
   char * pszCounter;
   char * pszCurrToken;
   char * pszTokenText;
   char * pszTokenTextEnd;


   pszTokenText = pszCurrentLocation;
   if ((pszTokenText == NULL) || (*pszTokenText == '\0'))
      {
      szCurrToken[0] = '\0';
      return TOKEN_NULL;
      }

   while (isspace(*pszTokenText))
      {
      if (*pszTokenText == '\n')
         {
         szCurrToken[0] = '\0';
         return NEED_NEXT_LINE;
         }
      pszTokenText++;
      pszCurrentLocation++;
      }

   pszCounter = pszTokenText;
   pszCurrToken = szCurrToken;
   usLen = 0;
   while (!isspace(*pszCounter))
      {
      *pszCurrToken = *pszCounter;
      usLen++;
      pszCurrToken++;
      pszCounter++;
      pszCurrentLocation++;
      }

   if (usIndentLevel >=6)
      usIndentLevel = 0;

   if (bGenerateTree)
      {
      us = DosWrite(hfTree, (PVOID) szPad, usIndentLevel, &usWrote);
      us = DosWrite(hfTree, (PVOID) pszTokenText, usLen, &usWrote);
      us = DosWrite(hfTree, (PVOID) "\n ", 2, &usWrote);
      }

   return 0;
   }/* End of Function GetToken */


BOOL ParseFile ( HFILE hfInFile )
   {
   BOOL bRC = FALSE;
   USHORT rc, us, usLen, usWrote;
   TOKEN  eToken = 0;

   usLineCount = 1;
   rc = GetFileLine (hfInFile, szLine, sizeof szLine);

   while ((END_OF_FILE != rc) && (FULL_LINE_BUFFER != rc))
      {
      bRC = TRUE;
      pszCurrentLocation = szLine;
      do {
         eToken = GetToken();
         usIndentLevel++;

         if ((eToken == NEED_NEXT_LINE) || (eToken == TOKEN_NULL))
            bRC = FALSE;
         //   continue;

         //bRC = Parse (eToken);
         } while (bRC);


      // -- During development:
      /* -- Write the processed line to the output file. */
      usLen = ItiStrLen(szLine);
      us = DosWrite(hfOut, (PVOID) szLine, usLen, &usWrote);

      usLineCount++;
      rc = GetFileLine (hfInFile, szLine, sizeof szLine);
      } /* End while (END_OF_FILE != Get... */

   fprintf(stderr, "     Processed %d lines. \r", usLineCount);
   fprintf(stderr,  " \n" );

   return (END_OF_FILE == rc);
   }  /* End of Function ParseFile */


/* ======================================================================= *\
\* ======================================================================= */
USHORT _cdecl main (int argc, char * argv[])
   {
   BOOL  bParsed;
   int   i;
   USHORT  us = 0;
   char * pszSource = NULL;
   char * pszDest = NULL;

   hab = WinInitialize(0);
   if (hab == NULL)           
      {
      return (13);
      }

   /* Prepare error log file. */
   OpenFile (szErrorFile, (PHFILE)&hfERROROut, OF_WRITE);

   InitErrorMessages ();
   InitReservedWords ();

   us = OptionSwitch(argv, argc);
   if (0 == us )
      {
      /* -- setup the InFile. */
      pszSource = szInFile;
      pszDest = argv[INFILE];
      while (*pszSource++ = *pszDest++)
         ;

      /* -- setup the OutFile. */
      pszSource = szOutFile;
      pszDest = argv[OUTFILE];
      while (*pszSource++ = *pszDest++)
         ;
      }
   else
      {
      switch (us)
         {
         default:
            DosClose (hfERROROut);
            return 13;
            break;
         }/* end switch */
      }/* end else clause */

   /* Setup files. */
   OpenFile (szInFile, (PHFILE)&hfIn, OF_READ);
   OpenFile (szOutFile, (PHFILE)&hfOut, OF_WRITE);
   if (bGenerateTree)
      {
      OpenFile (szTreeFile, (PHFILE)&hfTree, OF_WRITE);
      /* - init printed tree padding character. */
      for (i=0; i<PAD_SIZE; i++)
        szPad[i] = ' ';
      }

   fprintf(stderr, "\n\n RTF BNF Parser Utility, (C) 1997 Info Tech, Inc. \n\n");

// ===================================================

   bParsed = ParseFile (hfIn);

// ===================================================

   /* Close out files. */
   DosClose (hfIn);
   DosClose (hfOut);
   DosClose (hfERROROut);
   if (bGenerateTree)
      DosClose (hfTree);


   /* -- Finished using the DLL. */
   WinTerminate(hab);

   return (us);
   }/* END OF main */

