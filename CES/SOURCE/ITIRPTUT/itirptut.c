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


#define     INCL_BASE
#define     INCL_WIN
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI

#include    "..\include\iti.h"     

#include    <ctype.h>
#include    <stdio.h>
#include    <process.h>
//include    <stdlib.h>
#include    <limits.h>

#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"     
#include    "..\include\itidbase.h"
#include    "..\include\itimbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\itiglob.h"
#include    "..\include\itiwin.h"
#include    "..\include\colid.h"


#include    "initdll.h"
#include    "..\include\itirpt.h"
#include    "..\include\itirptut.h"
#include    "..\include\itirptdg.h"




/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  MSG_BOX_TITLE                  "Report Module Message"
#define  MAX_KEY_IDX                    (USHRT_MAX - 2)
#define  BUFFER_LEN                     LGARRAY
#define  MAX_KEYFILE_SIZE               400000L
#define  MAX_KEYFILES                   32
#define  NON_FH                         USHRT_MAX
#define  NOT_LISTED                     NON_FH
#define  E_O_F                          (USHRT_MAX - 3)

#define  MODE_WRITE               "wt"
#define  MODE_READ                "rt"
#define  MODE_APPEND              "at"


  /* -- length of string to hold numerical text value. */
#define  NUMLEN                        SMARRAY      



/* -- Holds names and handles of open Key files. */
typedef struct _Files
           {
           CHAR  szName [BUFFER_LEN+1];
           HFILE hfile;
           } FILES; /* sf */

typedef FILES *PFILES;  /* psf */



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR    szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR    szDllVersion[] = "1.1a0 itirptut.dll";


static USHORT  usKeyIdx    = 0;
static USHORT  usMaxKeyIdx = 0;
static CHAR    **pKeyList;
static CHAR    szSuppInfo [LGARRAY + 1] = "";
               // holds data for the current key of the key list only.
               // also is only available via a key file for now.

static USHORT     usTotalFileKeyCnt = 0;
static USHORT     usCurFileKeyIdx   = 0;

static CHAR   szMsg [LGARRAY + 1] = "";

static CHAR   szFileName [LGARRAY + 1] = "";
static BOOL   bGetKeysFromFile = FALSE;
static BOOL   bGetKeysFromQuery= FALSE;

static BOOL   bInitQueryDone   = FALSE; // Used to indicate use of hourglass.


static PID  pidRpt = 0; // Set by ItiRptUtExecRpt
//static SID  sidRpt = 0; // Set by ItiRptUtExecRpt


static USHORT usNextAvailableAsfIdx = 0;
static USHORT usCurrentAsfIdx = 0;
static FILES  asf[MAX_KEYFILES+1] = {
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH},
                {"",NON_FH},{"",NON_FH},{"",NON_FH}  };




static FILE *pImportFile = NULL;

static HFILE hfExportFile = 0;

static ULONG ulNum = ULONG_MAX;


 /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = 0;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;



 /* Used by DbExecQuer & DbGetQueryRow functions to get keys: */
static  HQRY    hqryKey   = NULL;
static  USHORT  usKeyQryID   = ID_KEY;
static  USHORT  usNumColsKey = 0;
static  USHORT  usErrKey     = 0;
static  PSZ   * ppszKey   = NULL;


static  BOOL    bSQLConnectionUsed = FALSE;





/* MAX_DATE_STRING is the maxium size of a date/time string */
#define MAX_DATE_STRING    128


#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif



static PSZ apszMonths [13] =
   {
   "NA", "January", "February", "March", "April", "May", "June", 
   "July", "August", "September", "October", "November", "December"
   };


static PSZ apszDays [8] =
   {
   "NA", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
   "Friday", "Saturday"
   };





/* ======================================================================= *\
 *                                                       LOCAL FUNCTIONS   *
\* ======================================================================= */


BOOL ReadFileLine (HFILE hfile, PSZ pszLn, USHORT usLnLen)
   {
   USHORT us = 0;
   USHORT usRead = 0;

   if (pszLn == NULL)
      return FALSE;
    
   while ((0 == DosRead(hfile, pszLn, 1, &usRead)) &&
          (usRead == 1) && (*pszLn != '\n'))
      pszLn++;

   return (usRead == 1);
   }/* End of function ReadFileLine */



USHORT GetFileLine (USHORT usSfIdx, PSZ pszBuffer, USHORT usBufferLen)
   {
//   USHORT usRetCode;
   USHORT us = 0;
   USHORT usBytesRead = 0;
   BOOL   bEOL = FALSE;
   CHAR   ch = '\0';


   if ((pszBuffer == NULL) || (usBufferLen < 2)) 
      return (ITIRPTUT_PARM_ERROR);          /* 2 because we need a minimum */
                                             /* of 1 digit character + '\0' */

   /* -- Erase buffer contents: */
   while ( (us < usBufferLen) && (pszBuffer[us] != '\0') )
      pszBuffer[us++] = '\0';


   if (asf[usSfIdx].hfile == NON_FH)
      return (ITIRPTUT_FILE_IO_ERROR);

   while ((0 == DosRead(asf[usSfIdx].hfile, pszBuffer, 1, &usBytesRead)) &&
          (usBytesRead == 1) && (*pszBuffer != '\n'))
      pszBuffer++;

   if (usBytesRead == 1)
      return 0;
   else
      {
      DosClose(asf[usSfIdx].hfile);
      asf[usSfIdx].hfile = NON_FH;
      return (E_O_F);
      }


//   us = 0;
//   do{ /* get a file line */
//      usRetCode = DosRead(asf[usSfIdx].hfile, &ch, 1, &usBytesRead);
//      if ((ch != 0x0a) && (ch != 0x0d))
//         pszBuffer[us++] = ch;
//      else
//         {
//         usRetCode = DosRead(asf[usSfIdx].hfile, &ch, 1, &usBytesRead);
//         if ((ch != 0x0a) && (ch != 0x0d))
//            pszBuffer[us++] = ch;
//         else
//            bEOL = TRUE;
//         }
//
//     }while ((usRetCode == 0) && (us < usBufferLen)
//             && (usBytesRead == 1) && (!bEOL));
//                             
//    if (usBytesRead == 0)
//       {
//       /* ...then is eof; do whatever here for EOF condition. */
//       return (E_O_F);
//       }
//
//   return (usRetCode);

   } /* End of Function GetFileLine */









USHORT GetNonCommentFileLine (USHORT usSfIdx, PSZ pszBuffer, USHORT usBufferLen)
   {
   USHORT usRetCode;
   USHORT us = 0;
   USHORT usBytesRead = 0;
   CHAR   ch = '\0';


   if ((pszBuffer == NULL) || (usBufferLen < 2)) 
      return (ITIRPTUT_PARM_ERROR);          /* 2 because we need a minimum */
                                             /* of 1 digit character + '\0' */

   /* -- Erase buffer contents: */
   while ( (us < usBufferLen) && (pszBuffer[us] != '\0') )
      pszBuffer[us++] = '\0';


   if (asf[usSfIdx].hfile == NON_FH)
      return (ITIRPTUT_FILE_IO_ERROR);


   do {/* Skip comment lines, ie. those that start with a ";" or "//" or ""     */

      usRetCode = GetFileLine (usSfIdx, pszBuffer, usBufferLen);

      }while ((usRetCode == 0) &&
               ((pszBuffer[0] == ';')
               ||(pszBuffer[0] == '\0')
               ||((pszBuffer[0]=='/')&&(pszBuffer[1]=='/')) ));


   return(usRetCode);

   } /* End of Function GetNonCommentFileLine */







/* -- This function is intended to to get any DLL customization info from
 * -- the begining of a key file; currently this function is not used by
 * -- any of the beta version reports.
 */ 
USHORT LoadDllStruct (void)
   {
//   USHORT us;
//   USHORT usSfIdx;
//   CHAR   szBuffer[LGARRAY];

   /* -- get header line and check. */
//   us = GetFileLine(usSfIdx, szBuffer, sizeof szBuffer);
//   if (us == 0)
//      {
//      if (ItiStrCmp(szBuffer, szFileHeader) != 0)
//         return (ITIRPTUT_FILE_READ_ERROR);
//      else
         return (0);
//      }
//
//   return (ITIRPTUT_FILE_READ_ERROR);

   }/* End of function LoadDllStruct */






BOOL IsKeyFileHandle (HFILE hfile)
   {
   USHORT us = 0;

   while (us < MAX_KEYFILES)
      {
      if (asf[us].hfile == hfile)
         return TRUE;
      us++;
      }
   return FALSE;

   }/* End of Function IsKeyFileHandle */








USHORT QKeyFileName (PSZ pszName)
   {
   USHORT us = 0;

   if (pszName == NULL)
      return NOT_LISTED;

   while (us < MAX_KEYFILES)
      {
      if (ItiStrCmp(asf[us].szName, pszName) == 0)
         return us;
      us++;
      }
   return NOT_LISTED;

   }/* End of Function QKeyFileName */






USHORT OpenImportFileNamed (PSZ pszFileName)
   {
   USHORT usErr;
   USHORT usIdx;
   USHORT usAction;

   /* -- Open the file. */
//   if( (pImportFile = fopen(pszFileName, MODE_READ )) == NULL )
//      return (ITIRPTUT_OPEN_FAILED);
   
   usIdx = QKeyFileName(pszFileName);
   if (usIdx == NOT_LISTED)
      {
      /* -- then add it to the list of key files, because   *
       * -- some other process may have built the key file. */
      ItiStrCpy (asf[usNextAvailableAsfIdx].szName, pszFileName, BUFFER_LEN);
      usCurrentAsfIdx = usNextAvailableAsfIdx;
      usIdx = usCurrentAsfIdx;
      usNextAvailableAsfIdx++;
      }

   usErr = DosOpen2(asf[usIdx].szName,
                    &(asf[usIdx].hfile), &usAction,
                    0,
                    FILE_NORMAL,      
                    FILE_OPEN,
                    OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                    NULL,       /* Don't use Extended Attribs */
                    0L);        /* reserved                   */
   if (usErr != 0)
      {
       // add error message here. 
      return (ITIRPTUT_OPEN_FAILED);
      }



   if (LoadDllStruct() != 0)
      {
      /* Could not load data structure, so some kind of file data error. */
      pImportFile = NULL;
      return (ITIRPTUT_FILE_READ_ERROR);
      }

   return (0);

   }/* -- End of OpenImportFileNamed */





/*
 * Currently assumes one key's digit string per file line
 * and that no file line's length can be greater than SMARRAY.
 */
USHORT AccessKeyDataFile(USHORT usSfIdx, PSZ pszKeyString, USHORT usKeyLength)
   {
   USHORT us;
   USHORT usBytesRead = 0;
   CHAR   ch = '\0';
   CHAR   szBuffer[SMARRAY + 1] = "";
   PSZ    pszBuffer = szBuffer;


   /* -- Test if we have a place to put a key value. */
   if ((pszKeyString == NULL) || (usKeyLength < 2)) 
      return (ITIRPTUT_PARM_ERROR);          /* 2 because we need a minimum */
                                             /* of 1 digit character + '\0' */
   if (asf[usSfIdx].hfile == NON_FH) 
      return (ITIRPTUT_FILE_IO_ERROR);


   us = GetNonCommentFileLine (usSfIdx, szBuffer, sizeof szBuffer );
   if (   ((us == E_O_F) && (szBuffer[0] == '\0'))
       || (us == ITIRPTUT_FILE_IO_ERROR))
      {
      /* ...then we are at the EndOfFile so discard file. */
//      DosDelete(asf[usSfIdx].szName, 0L);
      }
   else
      {
      ItiCsvGetField (1, szBuffer, pszKeyString, usKeyLength);

      /* -- Remove the trailing CR; SQL does NOT treat it as white space. */
      us = ItiStrLen(pszKeyString) - 1;
      while ((us > 0) && ItiCharIsSpace(pszKeyString[us]) )
         {
         pszKeyString[us] = '\0';
         us--;
         }

      /* -- Append a blank space to the key for the cases when
       * -- the key is not going to be at the end of a query.
       */
      ItiStrCat(pszKeyString, " ", usKeyLength);

           /* -- Note that currently no report uses the SuppInfo field. */
      //ItiCsvGetField (2, szBuffer, szSuppInfo,   sizeof(szSuppInfo));
      //if (szSuppInfo[0] != '\0')
      //   ;

      if ((pszKeyString[0] != '\0') && (pszKeyString[0] != '\n'))
         {
         usTotalFileKeyCnt++;
         return (0);
         }
      }/* end of if (us... else clause */


   usKeyIdx = ITIRPTUT_NO_MORE_KEYS;
   return (ITIRPTUT_NO_MORE_KEYS);


   }/* -- End of AccessKeyDataFile */










static BOOL ConvertToNumber (PSZ pszString, PUSHORT pusResult)
   {
   *pusResult = 0;

   if ('\0' == *pszString)
      return FALSE;

   return ItiStrToUSHORT (pszString, pusResult);
   }









static USHORT DaysInMonth (USHORT usMonth, USHORT usYear)
   {
   switch (usMonth)
      {
      /* months with 31 days */
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
         return 31;
         break;

      /* months with 30 days */
      case 4:
      case 6:
      case 9:
      case 11:
         return 30;
         break;

      /* february -- the month from hell */
      case 2:
         if ((usYear % 4) == 0)
            {
            if ((usYear % 400) != 0)
               {
               return 29;
               }
            else
               {
               return 28;
               }
            }
         else 
            {
            return 28;
            }
         break;

      default: /* months with zero days */
         return 0;
      }
   }




static BOOL IsValidDay (USHORT usMonth, USHORT usDay, USHORT usYear)
   {
   if ((usDay == 0) || (usYear == 0))
      return (FALSE);

   return (usDay <= DaysInMonth (usMonth, usYear));
   }




static BOOL ConvertToMonth (PSZ pszString, USHORT *pusMonth)
   {
   USHORT i;
   USHORT j;
   PSZ    pszTemp;

   *pusMonth = 0;

   if (ItiStrLen (pszString) < 3)
      return FALSE;

   for (i=1; i <= 12; i++)
      {
      pszTemp = apszMonths [i];

      for (j=0; pszTemp [j] != '\0' && pszString[j] != '\0'; j++)
         if (ItiCharLower (pszTemp [j]) != ItiCharLower (pszString [j]))
            break;

      if ('\0' == pszString [j])
         {
         *pusMonth = i;
         return TRUE;
         }
      }

   return FALSE;
   }




/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
extern USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (ITIRPTUT_DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

extern BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {

   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);
   return (FALSE);
   }







/* ------------------------------------------------------------------------
 * ItiRptUtItoa
 * Reference: K&R second edition, pg 64
 */
extern PSZ EXPORT ItiRptUtItoa (INT iNum, CHAR str[], INT iBase)
   {
   INT  a,b,i,k;
   CHAR c;

   if ((k = iNum) < 0)
      iNum = -iNum; /* make it positive */

   i = 0;
   do{
     str[i++] = (CHAR)(iNum % iBase + '0');
     }while ((iNum /= iBase) > 0);

   if (k < 0)
      str[i++] = '-';
   str[i] = '\0';

   /* put back in order */
   for (a=0, b=i-1; a<b; a++, b--)
      {
      c = str[a];
      str[a] = str[b];
      str[b] = c;
      }
   return &str[0];
   }/* END OF FUNCTION */





extern USHORT EXPORT ItiRptUtUniqueFile (PSZ pszFileName, USHORT usLen,
                                         BOOL bInclTmpPath)
   {
   DATETIME dt;
   CHAR   szTmp[SMARRAY] = "";
   CHAR   szulNum[SMARRAY] = "";
   HFILE  hfTmp;
   USHORT usErr, us;
   PSZ    pszEnv = NULL;

   if ((usLen < 13) || (pszFileName == NULL))
      return 13;



   DosSleep(1001L);      /* assure a different datetime to the second. */
   DosGetDateTime(&dt);        /* use date and time for temp file name */
   us = 0;
   ItiStrUSHORTToString(dt.month, szTmp, sizeof szTmp);
   szulNum[us++] = szTmp[0];
   if (dt.month > 9)
      szulNum[us++] = szTmp[1];

   ItiStrUSHORTToString(dt.day, szTmp, sizeof szTmp);
   szulNum[us++] = szTmp[0];
   if (dt.day > 9)
      szulNum[us++] = szTmp[1];

   ItiStrUSHORTToString(dt.hours, szTmp, sizeof szTmp);
   szulNum[us++] = szTmp[0];
   if (dt.hours > 9)
      szulNum[us++] = szTmp[1];

   ItiStrUSHORTToString(dt.minutes, szTmp, sizeof szTmp);
   szulNum[us++] = szTmp[0];
   szulNum[us++] = '.';
   if (dt.minutes > 9)
      szulNum[us++] = szTmp[1];

   ItiStrUSHORTToString(dt.seconds, szTmp, sizeof szTmp);
   szulNum[us++] = szTmp[0];
   if (dt.seconds > 9)
      szulNum[us++] = szTmp[1];
   szulNum[us] = '\0';



   /* -- Check for tmp subdir. */
   if (bInclTmpPath &&
      ((!DosScanEnv("TMP", &pszEnv)) || (!DosScanEnv("TEMP", &pszEnv))) )
      {
      ItiStrCpy(pszFileName, pszEnv, usLen);
      ItiStrCat(pszFileName, "\\r", usLen);
      }
   else
      ItiStrCpy(pszFileName, "r", usLen);

   ItiStrCat(pszFileName, szulNum, usLen);

   /* -- Check if such a named file already exists */
   usErr = DosOpen2(pszFileName,
                    &hfTmp, &us,
                    0,
                    FILE_NORMAL,      
                    FILE_OPEN,
                    OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                    NULL,       /* Don't use Extended Attribs */
                    0L);        /* reserved                   */

   if (usErr == 0) /* the file all ready exists. */
      {
      DosClose(hfTmp);

      // add error message here; try again. 
      ItiRptUtUniqueFile (pszFileName, usLen, bInclTmpPath);
      }

   return 0;
   }/* End of Function ...UniqueFile */



/* ------------------------------------------------------------------------ *\
 * ItiRptUtErrorMsgBox                                                         *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtErrorMsgBox (HWND hwnd, PSZ pszErrorText)
   {
   USHORT us;
   PSZ    pszEnv = NULL;

   DBGMSG("In ItiRptUtErrorMsgBox");


   /* -- Development version of error message set up. */
   if (pszErrorText != NULL)
      ItiErrWriteDebugMessage (pszErrorText);

   if (hwnd == NULL)
      {
      return(ITIRPTUT_PARM_ERROR);
      }


   if (*pszErrorText == DEV_MSG_CHAR)
      {          /* Show the user the message, always. */
      pszErrorText++; /* skip over the flag character. */

      us = WinMessageBox (HWND_DESKTOP, hwnd, pszErrorText,
                          MSG_BOX_TITLE,
                          0, MB_MOVEABLE | MB_OK | MB_APPLMODAL);
      if (us == MBID_ERROR)
         ItiErrWriteDebugMessage ("ItiRptUtErrorMsgBox ~ failed.");
      }
   else
      {
      DosScanEnv(REPORT_USE_MSGS, &pszEnv);

      if ((pszEnv != NULL) && ((pszEnv[0] == 'Y') || (pszEnv[0] == 'y')) )
         {
         us = WinMessageBox (HWND_DESKTOP, hwnd, pszErrorText,
                             MSG_BOX_TITLE " Beta Development",
                             0, MB_MOVEABLE | MB_OK | MB_APPLMODAL);
         if (us == MBID_ERROR)
            ItiErrWriteDebugMessage ("ItiRptUtErrorMsgBox failed.");
         }
      }
    

   return (us);

   }/* END OF FUNCTION ItiRptUtErrorMsgBox */








/* ------------------------------------------------------------------------ *\
 * ItiRptUtSetError                                                         *
 *                                                                          *
 *    This function screens the returned error codes and sends pszErrorText *
 *    to the debug window.                                                  *
 *                                                                          *
 * Parameters: hab            The application's anchor block handle.        *
 *             pszErrorText   A pointer to the error message text string.   *
 *                                                                          *
 * Return value: 0 if severity level of the errid is just a warning.        *
 *                                                                          *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtSetError(HAB hab, PSZ pszErrorText)
   {
   USHORT  usErrSeverity = 1;
   USHORT  usErrValue = ITIRPTUT_ERROR;
   ERRORID errid = MAKEERRORID(SEVERITY_WARNING, 0);
   PSZ  pszEnv = NULL;

   DBGMSG("In ItiRptUtSetError");

   if (hab != NULL)
      {
      errid = WinGetLastError(hab);
      ItiErrFreeErrorString (ItiErrGetErrorString(hab));

      usErrValue = (USHORT) ERRORIDERROR(errid);
      usErrSeverity = (USHORT) ERRORIDSEV(errid);
      }
   else
      {
      /* -- Set somekind of message here. */
      }

   /* -- Development version of error message set up. */
   ItiErrWriteDebugMessage (pszErrorText);

   /* ...insert other error message handling here.    */



   DBGMSG("Exit Leaving ItiRptUtSetError");
   if (ERRORIDSEV(errid) == SEVERITY_WARNING)
      return (0);
   else
      return (usErrValue);

   }/* END OF FUNCTION ItiRptUtSetError */




/* ------------------------------------------------------------------------ *\
 * ItiRptUtInitKeyList                                                      *
 *                                                                          *
 *    This function initializes the module's pointer to the command         *
 *    line's key list values.                                               *
 *                                                                          *
 * Parameters:    The arg pointers from the application's command line.     *
 *                                                                          *
 * Return value:  0                    if successful.                       *
 *                ITIRPTUT_OPEN_FAILED if key file specified and            *
 *                                     failed to open it.                   *
 *                                                                          *
 * Comments: Assumes option flags ("/X ") are followed by a separate        *
 *           parameter string that is the option string.                    *
 *           ie.   /X "some kind of text"                                   *
 *           or    /X  TextWithoutSpaces                                    *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtInitKeyList (INT iArgCnt, CHAR * pArgVar[])
   {
   INT    i = 0;
   USHORT us;
   CHAR   szOptionStr[LGARRAY + 1] = "";
   INT   *paTmp = NULL;
   PSZ    pszQuery = NULL;


   if (hhpRptUt == NULL) // Added 17 MAR 92; sometimes initdll order problem.
      {
      hhpRptUt = ItiMemCreateHeap (MIN_HEAP_SIZE);
      }


   usKeyIdx = ITIRPTUT_USE_DEFAULT;

   if (pArgVar == NULL)
      return (ITIRPT_INVALID_PARM);
   else
      {
      if (hhpKey == NULL)
         {
         hhpKey = ItiMemCreateHeap (MIN_HEAP_SIZE);
         }


      /* -- Skip over possible program and DLL names. */
      while ((i < iArgCnt) && (isalpha(pArgVar[i][0])))
         i++;

      if (iArgCnt <= i)
         /* For first time thru there are no keys in the list, we  *
          * want to indicate that we will use the default query    *
          * by having usKeyIdx set to the ITIRPTUT_USE_DEFAULT      */
         return (0);
      else
         {
         us = ItiRptUtGetOption(ITIRPTUT_FILE_SWITCH,
                                &szOptionStr[0], sizeof szOptionStr,
                                iArgCnt, pArgVar);
         if (us == 0)
            {
            if (szOptionStr[0] == ITIRPTUT_KEY_QUERY_SW_VAL)
               {
               /* -------------------------------------------------------- *\
                * Getting the Key List from a Query.                       *
               \* -------------------------------------------------------- */
               bGetKeysFromQuery = TRUE;
               pszQuery = &szOptionStr[1]; /* skip over the marker char. */

               /* -- Do the database query. */
               hqryKey = ItiDbExecQuery (pszQuery, hhpKey, hmodModule,
                                         ITIRID_RPT, usKeyQryID,
                                         &usNumColsKey, &usErrKey);
               if (hqryKey != NULL)
                  bSQLConnectionUsed = TRUE;
               else
                  bSQLConnectionUsed = FALSE;
               }
            else
               {
               /* -------------------------------------------------------- *\
                * Getting the Key List from a FILE.                        *
               \* -------------------------------------------------------- */
               i = OpenImportFileNamed (szOptionStr);
               if (i == ITIRPTUT_OPEN_FAILED)
                  return (ITIRPTUT_OPEN_FAILED);

               bGetKeysFromFile = TRUE;
               pKeyList = NULL;
               }
            }
         else
            {
            /* ----------------------------------------------------------- *\
             * Getting the Key List from the command line.                 *
            \* ----------------------------------------------------------- */
            /* -- Skip over possible option switches. */
            while ((i < iArgCnt)
                   && ((pArgVar[i][0] == '/')||(pArgVar[i][0] == '-')) )
               {
               i++;                     /* Skip over the '/X' option flag. */
               if ((i < iArgCnt) && (pArgVar[i] != NULL))
                  i++;                  /* Skip over the option's string.  */
               else
                  {                     /* Flag present but no string.     */
                  return (ITIRPTUT_ERROR);
                  }
               }/* end while */
   
            if (i < iArgCnt)
               {
               pKeyList = &pArgVar[i];
               usKeyIdx = 0;
               usMaxKeyIdx = iArgCnt - i;
               }
            else
               { /* there is NO key list available. */
               usKeyIdx = ITIRPTUT_USE_DEFAULT;
               pKeyList = NULL;
               }

            }/* end else (us == 0 */

         }/* end else (iArgCnt... */

      }/* end else ((pArgVar... */


   return (0);

   }/* END OF FUNCTION ItiRptUtInitKeyList */





/* ------------------------------------------------------------------------ *\
 * ItiRptUtGetNextKey                                                       *
 *                                                                          *
 *    Gets the next available string of text from the KeyList that          *
 *    was prepared by a call to ItiRptUtInitKeyList.                        *
 *                                                                          *
 *                                                                          *
 * Parameters:    pszKeyStr   Pointer to hold returned key from             *
 *                            ItiRptUtInitKeyList.                          *
 *                                                                          *
 *                usKeyLen    The length of the string buffer pointed       *
 *                            to by pszKeyStr.                              *
 *                                                                          *
 *                                                                          *
 * Return value:  0           if text was put into pszKeyStr.               *
 *                                                                          *
 *                ITIRPTUT_USE_DEFAULT  the FIRST time this function is     *
 *                                     called and the KeyList is empty;     *
 *                                                                          *
 *                ITIRPTUT_NO_MORE_KEYS if at the end of the KeyList or     *
 *                                     any call AFTER the first call        *
 *                                     with an empty KeyList.               *
 *                                                                          *
 * Comments:   The KeyList is maintained internally and there is no         *
 *             direct access to it.                                         *
 *                                                                          *
 * See Also:                                                                *
 *     ItiRptUtInitKeyList                                                  *
 *                                  --                                     *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtGetNextKey (PSZ pszKeyStr, USHORT usKeyLen)
   {
   INT   *paTmp = NULL;
   USHORT us = 0;


   if (pszKeyStr != NULL)
      *pszKeyStr = '\0';  /* nil init key */
   else
      return (ITIRPT_INVALID_PARM);  /* Can't assign a key to NULL */  

   /* -- If we are called after the default of NO keys... */
   if (usKeyIdx == ITIRPTUT_NO_MORE_KEYS)
      return (ITIRPTUT_NO_MORE_KEYS);

   if ((pKeyList != NULL) && (usKeyIdx < usMaxKeyIdx)
       && (pKeyList[usKeyIdx] != NULL))
      {
      if (ItiStrCpy(pszKeyStr, pKeyList[usKeyIdx], usKeyLen) != NULL)
         {
         usKeyIdx++;
         return (0);
         }
      }
   else
      {
      /* If first time thru there are no keys in the list, we
       * want to indicate that we will use a default query;
       * also reset the key index value to indicate no keys.
       */
      if ((usKeyIdx == ITIRPTUT_USE_DEFAULT)
          && (bGetKeysFromFile  == FALSE)
          && (bGetKeysFromQuery == FALSE))
         {
         usKeyIdx = ITIRPTUT_NO_MORE_KEYS;
         return (ITIRPTUT_USE_DEFAULT);
         }

      /* This is the stopping case. */
      if ((usKeyIdx >= usMaxKeyIdx)
          && (bGetKeysFromFile  == FALSE)
          && (bGetKeysFromQuery == FALSE))
         return (ITIRPTUT_NO_MORE_KEYS);


      if ((bGetKeysFromFile == TRUE) && (bGetKeysFromQuery == FALSE))
         {
                  // for now use...
         us = AccessKeyDataFile (usCurrentAsfIdx, pszKeyStr, usKeyLen);
         return (us);
         }/* end of if (bGetKeysFromFile... */


      if (bGetKeysFromQuery == TRUE)
         {
         if (hqryKey == NULL)
            {
            bSQLConnectionUsed = FALSE;
            return ITIRPTUT_NO_MORE_KEYS;
            }

         if (ItiDbGetQueryRow(hqryKey,&ppszKey,&usErrKey) )
            {
            us = 0;
            ItiStrCpy(pszKeyStr, ppszKey[0], usKeyLen);
            ItiFreeStrArray (hhpKey, ppszKey, usNumColsKey);
            }
         else
            {
            bSQLConnectionUsed = FALSE;
            us = ITIRPTUT_NO_MORE_KEYS;
            }
         
         return (us);
         }

      }/* end of else clause */

   return (ITIRPTUT_ERROR);
   }/* END OF FUNCTION ItiRptUtGetNextKey */




/* ------------------------------------------------------------------------ *\
 * ItiRptUtPrepQuery                                                        *
 *                                                                          *
 *    This function concatenates query clauses together into a query string.*
 *                                                                          *
 * Parameters:    pszQ          The destination query string.               *
 *                usQLen        Length of the destination query string.     *
 *                pszQueryBase  The base query text.                        *
 *                pszConj       Conjuction clause base string;              *
 *                              ie. "SomeKeyName = "                        *
 *                pszKeyStr     The key value text string.                  *
 *                                                                          *
 * Return value:  0 if successful.                                          *
 *                                                                          *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtPrepQuery
  (PSZ pszQ, USHORT usQLen, PSZ pszQueryBase, PSZ pszConj, PSZ pszKeyStr)
   {
//   PSZ pszPtr;

   *pszQ = '\0'; /* erase old query. */
//   pszPtr = pszQ;

   if ((pszQ == NULL) || (pszQueryBase == NULL) || (usQLen == 0))
      return (ITIRPT_INVALID_PARM);  
   else
      ItiStrCpy(pszQ, pszQueryBase, usQLen);

   if ((pszConj != NULL) && (*pszConj != '\0'))
      {
      ItiStrCat(pszQ, pszConj, usQLen);
      }

   if ((pszKeyStr != NULL) && (*pszKeyStr != '\0'))
      {
      ItiStrCat(pszQ, pszKeyStr, usQLen);
      }


   return (0);
   }




/* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtGetOption
               (CHAR cOpt, PSZ pszOptionStr, USHORT usLenOptionStr,
                INT argcnt, CHAR * argvars[])
   {
   INT     i = 0;
   CHAR cOtherCase = '~';


   /* -- Desensitize letter case for now. */
   if (islower(cOpt))
      cOtherCase = (CHAR)toupper(cOpt); /* other case is UPPER */
   else
      cOtherCase = (CHAR)tolower(cOpt); /* other case is LOWER */


   /* -- Clear the option string. */
   pszOptionStr[0] = '\0';

   while (i < argcnt) 
      {
      if( ((argvars[i][0] == '/') || (argvars[i][0] == '-'))
         && ((argvars[i][1] == cOpt) || (argvars[i][1] == cOtherCase))
        )
        {
        if (argcnt >= i+1)
           {
           ItiStrCpy (pszOptionStr, argvars[i+1], usLenOptionStr);
           return (0);
           }
        }
      i++;
      }

   return(ITIRPTUT_OPTION_NOT_FOUND);

   }/* END OF FUNCTION */





/* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtKeySuppInfo (PSZ pszBuffer, USHORT usBufferSize)
   {
   ItiStrCpy (pszBuffer, szSuppInfo, usBufferSize);
   return (0);
   }/* END OF FUNCTION */
 




/* ------------------------------------------------------------------------ *\
 * ItiRptUtMonthName                                                        *
 *                                                                          *
 *   Given a usMonthNum value, ie 1 or 2 etc., the first usLen characters   *
 *   of the month's name are copied into pszMonth; "***" if usMonthNum is   *
 *   zero or greater than twelve.                                           *
 *                                                                          *
 *   Return: 0 if successful; or ITIRPTUT_ERROR if pszMonth is null or the  *
 *           usMonthNum is out of range.                                    *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtMonthName
        (USHORT usMonthNum, PSZ pszMonth, USHORT usLen)
   {
   if (pszMonth == NULL)
      return (ITIRPTUT_ERROR);


   switch (usMonthNum)
      {
      case 1:
         ItiStrCpy(pszMonth, "January", usLen);
         break;

      case 2:
         ItiStrCpy(pszMonth, "February", usLen);
         break;


      case 3:
         ItiStrCpy(pszMonth, "March", usLen);
         break;


      case 4:
         ItiStrCpy(pszMonth, "April", usLen);
         break;


      case 5:
         ItiStrCpy(pszMonth, "May", usLen);
         break;


      case 6:
         ItiStrCpy(pszMonth, "June", usLen);
         break;


      case 7:
         ItiStrCpy(pszMonth, "July", usLen);
         break;


      case 8:
         ItiStrCpy(pszMonth, "August", usLen);
         break;


      case 9:
         ItiStrCpy(pszMonth, "September", usLen);
         break;


      case 10:
         ItiStrCpy(pszMonth, "October", usLen);
         break;


      case 11:
         ItiStrCpy(pszMonth, "November", usLen);
         break;


      case 12:
         ItiStrCpy(pszMonth, "December", usLen);
         break;


      default:
         ItiStrCpy(pszMonth, "***", usLen);
         return (ITIRPTUT_ERROR);
         break;
      }

   return (0);
   }/* END OF FUNCTION */
 



/* -- Copied from ItiFmt's date module. */
extern BOOL EXPORT ItiRptUtParseDate (PSZ       *ppszDate, 
                                      PUSHORT   pusYear, 
                                      PUSHORT   pusMonth, 
                                      PUSHORT   pusDay)
   {
   char szField [MAX_DATE_STRING], szCopy [MAX_DATE_STRING];
   PSZ pszToken;

   *pusMonth = *pusDay = *pusYear = 0;

   ItiStrCpy (szCopy, *ppszDate, sizeof szCopy);

   pszToken = ItiStrTok (szCopy, " -/,.");
   if (pszToken == NULL) 
      return FALSE;

   ItiStrCpy (szField, pszToken, sizeof szField);

   if (ItiCharIsAlpha (*szField))
      {
      if (!ConvertToMonth (szField, pusMonth))
         return FALSE;
      pszToken = ItiStrTok (NULL, " -/,.");
      if (pszToken == NULL)
         return FALSE;
      ItiStrCpy (szField, pszToken, sizeof szField);
      if (!ConvertToNumber (szField, pusDay))
         return FALSE;
      }
   else
      {
      if (!ConvertToNumber (szField, pusMonth))
         return FALSE;
      pszToken = ItiStrTok (NULL, " -/,.");
      if (pszToken == NULL)
         return FALSE;
      ItiStrCpy (szField, pszToken, sizeof szField);
      if (ItiCharIsAlpha (*szField))
         {
         *pusDay = *pusMonth;
         if (!ConvertToMonth (szField, pusMonth))
            return FALSE;
         }
      else
         {
         if (!ConvertToNumber (szField, pusDay))
            return FALSE;
         }
      }

   pszToken = ItiStrTok (NULL, " -/,.");
   if (pszToken == NULL)
      return FALSE;
   ItiStrCpy (szField, pszToken, sizeof szField);
   if (!ConvertToNumber (szField, pusYear))
      return FALSE;

   if (*pusYear > 9999)
      return FALSE;

   if (*pusYear < 100)
      {
      if (*pusYear < 70)
         {
         *pusYear += 2000;
         }
      else
         {
         *pusYear += 1900;
         }
      }

   return IsValidDay (*pusMonth, *pusDay, *pusYear);

   }/* END OF FUNCTION */






/* ----------------------------------------------------------------------- *\
 * ItiRptUtConvertToDayOfYr                                                *
 *                                                                         *
 * Given a date string this function returns which day of the year         *
 * that date falls upon; ie 1991, 2, 14 is the 46th day of the year.       *
 *                                                                         *
 * If an invalid date is given the return value is 0.                      *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtConvertToDayOfYr (PSZ *ppszDate)
   {
   USHORT usCnt = 0;
   USHORT us;
   USHORT usYear;
   USHORT usMonth; 
   USHORT usDay;


   ItiFmtParseDate (ppszDate, &usYear, &usMonth, &usDay);


   /* -- Check if we were given a valid date. */
   if ( !IsValidDay(usMonth, usDay, usYear) )
      return (0);

   /* -- Add up all of the days of the months prior to the given month. */
   for (us = 1; us < usMonth; us++)
      usCnt = usCnt + DaysInMonth (us, usYear);

   /* -- Now add the day of the given month to the total of days in the *
    *    previous months.                                               */
   usCnt = usCnt + usDay;

   return (usCnt);
   }/* End of function ConvertToDayOfYr */
   










/* ----------------------------------------------------------------------- *\
 * ItiRptUtDateDiff                                                        *
 *                                                                         *
 * Given two date strings,this function returns the number of days         *
 * difference between the two dates.                                       *
 *                                                                         *
 * Comment: The two dates can not be any more 179 years apart.  This       *
 *          is because the returned value has to fit into a USHORT.        *
 *                                                                         *
 * If an invalid date is given the return value is USHRT_MAX.              *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtDateDiff (PSZ pszDate1, PSZ pszDate2)
   {
   USHORT usCnt = 0;
   USHORT us = 0;

   USHORT usYear1;
   USHORT usMonth1; 
   USHORT usDay1;

   USHORT usYear2;
   USHORT usMonth2; 
   USHORT usDay2;

   BOOL   bSwapDates = FALSE;


   /* -- Parse the date strings into numerical values. */
   if (ItiFmtParseDate (&pszDate1, &usYear1, &usMonth1, &usDay1)
      || ItiFmtParseDate (&pszDate2, &usYear2, &usMonth2, &usDay2))
     return (USHRT_MAX);

   /* -- Make sure date2 is AFTER date1. */
   if (usYear1 > usYear2)
      {
      bSwapDates = TRUE;
      }
   else if (usYear1 == usYear2)
      {
      if (usMonth1 > usMonth2)
         {
         bSwapDates = TRUE;
         }
      else if (usMonth1 == usMonth2)
              {
              /* -- short circuit algorithm since same year & month. */
              if (usDay1 > usDay2)
                 {
                 return (usDay1 - usDay2);
                 }
              else
                 return (usDay2 - usDay1);
              }
      }


   if (bSwapDates)
      {
      us = usYear1;
      usYear1 = usYear2;
      usYear2 = us;

      us = usMonth1;
      usMonth1 = usMonth2;
      usMonth2 = us;

      us = usDay1;
      usDay1 = usDay2;
      usDay2 = us;
      }


   /* -- Range checking. */
   if ((usYear2 - usYear1) > 179)
      return (USHRT_MAX);


   /* -- Number of days REMAINING in the first month. */
   usCnt = DaysInMonth(usMonth1, usYear1) - usDay1;

   /* -- Set to the next month. */
   usMonth1++;
   if (usMonth1 > 12)
      {
      usMonth1 = 1;
      usYear1++;
      }


   while ( ((usMonth1 < usMonth2) && (usYear1  == usYear2))
         || (usYear1 < usYear2) )
      {
      usCnt = usCnt + DaysInMonth (usMonth1, usYear1);
      usMonth1++;
      if (usMonth1 > 12)
         {
         usMonth1 = 1;
         usYear1++;
         }
      }

   /* -- Now add the number of days of the remaining month. */
   usCnt = usCnt + usDay2;


   return (usCnt);
   }/* End of function DateDiff */
   









extern USHORT EXPORT ItiRptUtMath (USHORT usFmt, USHORT usResLen, PSZ pszRes, 
                     PSZ pszExp1, PSZ pszOp, PSZ pszExp2)
   {
   USHORT us = 0;
   USHORT usNumOfCols = 0;
   PID    pidMath;
   SID    sidMath;
   CHAR   szNum[32] = "";
   CHAR   szShrSegName [SMARRAY]    = "\\SHAREMEM\\";
   CHAR   szQueryExp [3*SMARRAY]    = "\"select ";
   CHAR   szCmdLine  [4*SMARRAY+33] = "";
   CHAR   szFileName [SMARRAY] = "";
   SEL    selResult;
   PSZ    pszEnv    = NULL;
   PSZ    pszResult = NULL;
   PSZ    *ppszResult;


   /* -- Validate the parameters. */
   if ((pszRes==NULL) || (pszExp1==NULL) || (pszOp==NULL) || (pszExp2==NULL))
      {
      return (ITIRPTUT_PARM_ERROR);
      }

   if ((pszExp1[0] == '\0') || (pszOp[0] == '\0') || (pszExp2[0] == '\0'))
      {
      if (usResLen >= 2)
         {
         pszRes[0] = '0';
         pszRes[1] = '\0';
         }
      return (ITIRPTUT_PARM_ERROR);
      }

   /* -- Build the expression query. */
   ItiStrCat (szQueryExp, pszExp1, sizeof szQueryExp );
   ItiStrCat (szQueryExp, pszOp,   sizeof szQueryExp );
   ItiStrCat (szQueryExp, pszExp2, sizeof szQueryExp );


   DosScanEnv(REPORT_USE_MATH, &pszEnv);
   if ((bSQLConnectionUsed == FALSE) && ((pszEnv == NULL)
       ||((pszEnv != NULL) && ((pszEnv[0] != 'Y') || (pszEnv[0] != 'y'))) ))
      {/* ... then do NOT use a second process for math;                 */
       /*     this also means that we have to remove the 1st quote mark. */
      szQueryExp[0] = ' ';

      if (hhpRptUt == NULL)
         {
         hhpRptUt = ItiMemCreateHeap (MIN_HEAP_SIZE);
         }

      ppszResult = ItiDbGetRow1 (szQueryExp, hhpRptUt, hmodModule, ITIRID_RPT,
                                 usFmt, &usNumCols);

      if ((ppszResult != NULL) && (*ppszResult != NULL))
         /* -- Get the result from the data area. */
         ItiStrCpy(pszRes, *ppszResult, usResLen);
      else
         {                     
         ItiRptUtSetError(NULL, szQueryExp);
         ItiStrCpy(pszRes, " 0 ", usResLen);
         }


      return (us);
      }
   else /* ...add the close quote to the string. */
      ItiStrCat (szQueryExp," \" ",sizeof szQueryExp ); 


////////////

   /* -- Name the result's sharable memory space. */
   ItiRptUtUniqueFile (szFileName, sizeof szFileName, FALSE);
   ItiStrCat(szShrSegName, szFileName, sizeof szShrSegName);

   /* -- Setup the result memory space. */
   us = DosAllocShrSeg((RESULT_SIZE+1), szShrSegName, &selResult);
   if (us != 0)
      {
      ItiRptUtErrorMsgBox (NULL, "*ERR: ItiRptUtMath function did NOT get ShrSeg.");
      return (us);
      }
   
   pszResult = MAKEP(selResult, 0);

   /* -- Init result memory. */
   for(us=0; us<RESULT_SIZE; us++)
      pszResult[us] = '\0';


   /* -- Build the cmd line. */
   ItiStrCat (szCmdLine, szQueryExp, sizeof szCmdLine);  // param #1
   ItiStrCat (szCmdLine, szFileName, sizeof szCmdLine);  // param #2
   ItiStrCat (szCmdLine, " ", sizeof szCmdLine);
   ItiStrUSHORTToString (usFmt, szNum, sizeof szNum);
   ItiStrCat (szCmdLine, szNum, sizeof szCmdLine);       // param #3
   ItiStrCat (szCmdLine, " ", sizeof szCmdLine);
   ItiStrCat (szCmdLine, szShrSegName, sizeof szCmdLine);  // param #4



   /* -- Exec the math process. */
   us = ItiProcCreateSession ("RptMathSession", "itirmath.exe", szCmdLine,
                              (SESSION_BACKGROUND | SESSION_RELATED),
                              &pidMath, &sidMath);
   if (us == 0)
      {
      /* -- Wait until the itirmath process finishes. */
      while (pszResult[0] == '\0')
         DosSleep(23L);
   
      /* -- Get the result from the data area. */
      ItiStrCpy(pszRes, &pszResult[1], usResLen);

      /* -- Recover the shared memory. */ 
      DosFreeSeg(selResult);
      }


//////

   return (us);

   }/* End of function ItiRptUtMath */







// Added 30 AUG 91
USHORT EXPORT ItiRptUtStartKeyFile
                 (PHFILE phfile, PSZ pszFileName, USHORT usLen)
   {
   CHAR   szFileName[2 * SMARRAY] = "";
   USHORT usAction, usErr;


   /* -- Make a unique file name? */
   if ((pszFileName != NULL) && (pszFileName[0] != '\0'))
      {
      /* -- Use the given name. */
      ItiStrCpy(szFileName, pszFileName, sizeof szFileName);
      }
   else
      {
      ItiRptUtUniqueFile (szFileName, sizeof szFileName, TRUE);
      if (pszFileName != NULL)
         ItiStrCpy(pszFileName, szFileName, usLen);
      }


   /* -- Create open file. */                        
    usErr = DosOpen2(szFileName, phfile, &usAction,
                     0,
                     FILE_NORMAL,      
                     FILE_OPEN | FILE_CREATE,
                     OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE,
                     NULL,       /* Don't use Extended Attribs */
                     0L);        /* reserved                   */

   if (usErr != 0)
      fprintf (stderr, "ItiRptUt failed to create a key file.\n");
   else
      {
      ItiStrCpy(asf[usNextAvailableAsfIdx].szName, szFileName, BUFFER_LEN);
      asf[usNextAvailableAsfIdx].hfile = *phfile;
      usCurrentAsfIdx = usNextAvailableAsfIdx;
      usNextAvailableAsfIdx++;
      }
      

   return 0;
   }/* End of Function ItiRptUtStartKeyFile */





// Added 30 AUG 91
USHORT EXPORT ItiRptUtWriteToKeyFile (HFILE hfile, PSZ pszKey, PSZ pszSuppInfo)
   {
   USHORT usErr;
   USHORT usBytesWritten = 0;
   CHAR   szEOL[] = { ' ', 0x0d, 0x0a, '\0' };
   CHAR   szBuffer[LGARRAY+1] = "";


   if (pszKey != NULL)
      ItiStrCpy(szBuffer, pszKey, sizeof szBuffer);
   else
      return ITIRPTUT_NO_MORE_KEYS;

   if ((pszSuppInfo != NULL) && (ItiStrLen(pszSuppInfo) > 0) )
      {
      ItiStrCat(szBuffer, " , ", sizeof szBuffer);       /* CSV format */
      ItiStrCat(szBuffer, pszSuppInfo, sizeof szBuffer);
      }

   /* -- Append an EOL. */
   ItiStrCat(szBuffer, szEOL, sizeof szBuffer);

   if (IsKeyFileHandle(hfile))
      usErr =  DosWrite(hfile, szBuffer, ItiStrLen(szBuffer),&usBytesWritten);

   /* -- add error handling here. */

   return usErr;
   }/* End of Function ItiRptUtWriteToKeyFile */




// Added 30 AUG 91
USHORT EXPORT ItiRptUtEndKeyFile (HFILE hfile)
   {

   return DosClose(hfile);

   }/* End of Function ItiRptUtEndKeyFile */




USHORT EXPORT ItiRptUtErrorLogMsg (PSZ pszErrorText)
   {
   return 0;
   }








USHORT EXPORT ItiRptUtBuildSelectedKeysFile
                     (HWND   hwnd,
                      USHORT usListWndID,
                      USHORT usColumnID,
                      PSZ    pszKeyFileName,
                      USHORT usLenKeyFileName)
   {
   HFILE  hfile;
   USHORT usRow;
   HBUF   hbuf;
   CHAR   szCol[SMARRAY] = "";
   HWND   hwndStatic, hwndList;


   if ((pszKeyFileName == NULL) || (usLenKeyFileName < 8))
      return (ITIRPTUT_PARM_ERROR);

   ItiRptUtStartKeyFile (&hfile, pszKeyFileName, usLenKeyFileName);

   hwndStatic = (HWND) QW (hwnd, ITIWND_OWNERWND, 0, 0, 0);

             /* -- Get the list window that belongs to the static window. */
   hwndList = WinWindowFromID (hwndStatic, usListWndID);

   hbuf = (HBUF) QW (hwndList, ITIWND_BUFFER, 0, 0, 0);
   if (hbuf != NULL)
      {
      usRow = (UM) QW (hwndList, ITIWND_SELECTION, 0, 0, 0);
   
      szCol[0] = '\0';
      ItiDbGetBufferRowCol(hbuf, usRow, usColumnID, szCol);
      if (szCol[0] == '\0') /* ie we don't have any rows... */
         {
         WinMessageBox (HWND_DESKTOP, 
                        hwnd,
                        "No rows were selected (or available) from the list.",
                        MSG_BOX_TITLE,
                        0, MB_MOVEABLE | MB_OK | MB_APPLMODAL);

         ItiRptUtWriteToKeyFile (hfile, ERROR_KEY, NULL);//dummy key to file
         ItiRptUtEndKeyFile (hfile);
         return (ITIRPTUT_ERROR);
         }
      ItiStrCat(szCol, " ", sizeof szCol);
      ItiRptUtWriteToKeyFile (hfile, szCol, NULL);
   
      while((usRow = (UM) QW (hwndList, ITIWND_SELECTION, usRow+1, 0, 0)) != 0xffff)
         {
         szCol[0] = '\0';
         ItiDbGetBufferRowCol(hbuf, usRow, usColumnID, szCol);
         ItiStrCat(szCol, " ", sizeof szCol);
         ItiRptUtWriteToKeyFile (hfile, szCol, NULL);
         }
   
      ItiRptUtEndKeyFile (hfile);
      }/* end of if (hbuf... then clause  */
   else
      {
      ItiRptUtWriteToKeyFile (hfile, ERROR_KEY, NULL); //dummy key to file
      ItiRptUtEndKeyFile (hfile);

      WinMessageBox (HWND_DESKTOP, 
                     hwnd,
                     "No rows were selected (or available) from the list.",
                     "Report Module Message",
                     0, MB_MOVEABLE | MB_OK | MB_APPLMODAL);

      ItiStrCpy(szMsg, "ERROR: Selected rows failed to fill key file ", sizeof szMsg);
      ItiStrCat(szMsg, pszKeyFileName, sizeof szMsg);
      ItiRptUtErrorMsgBox (hwnd, szMsg);

      return (ITIRPTUT_ERROR);
      }/* end of if (hbuf... else clause  */

   return (0);
   }/* End of Function ItiRptUtBuildSelectedKeysFile */






USHORT EXPORT ItiRptUtExecRptCmdLn (HWND hwnd, PSZ pszCmdLn, PSZ pszSessionName)
   {
   int   i;
   CHAR  szPID [16] = "";
   CHAR  szErrorMsg [LGARRAY] = "";

   if (pszCmdLn == NULL)
      return ITIRPTUT_PARM_ERROR;

//   930106-4104: 1/15/93 mdh: I changed this to a call to spawnl, since
//   it allows a child process to keep running when the parent dies.
//
//   us = ItiProcCreateSession (pszSessionName, ITIRPT_EXE, pszCmdLn,
//                              (SESSION_BACKGROUND | SESSION_RELATED),
//                              &pidRpt, &sidRpt);
   i = spawnl (P_NOWAIT, ITIRPT_EXE, pszCmdLn, pszCmdLn, NULL);
   if (-1 == i)
      {/* send somekind of error message. */
      ItiStrCpy (szErrorMsg, "ERROR: Failed to exec process for ", sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, pszSessionName, sizeof szErrorMsg);

      ItiErrWriteDebugMessage (szErrorMsg);
      if (hwnd)
         ItiRptUtErrorMsgBox (hwnd, szErrorMsg);

      return (ITIRPTUT_ERROR);
      }
   else
      {
      ItiStrCpy (szErrorMsg, pszSessionName, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, "\n\r   Command: ", sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, pszCmdLn, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, "\n\r   ProcessID: ", sizeof szErrorMsg);
      ItiStrUSHORTToString (i, szPID, sizeof szPID);
      ItiStrCat (szErrorMsg, szPID, sizeof szErrorMsg);

      ItiRptUtSetError (NULL, szErrorMsg);
      pidRpt = i;
      return 0;
      }

   }/* End of Function ItiRptUtExecRptCmdLn  */





USHORT EXPORT ItiRptUtInitDlgBox (HWND hwnd, PSZ pszTitleText, USHORT us, PSZ *ppsz)
   {
   SWP   swpMe;
   SWP   swpParent;
   HWND  hwndTitleBar;
   CHAR  szTitle[80] = "";

   /* position the dialog box */
   WinQueryWindowPos (hwnd, &swpMe);

   WinQueryWindowPos (WinQueryWindow(hwnd,QW_PARENT,0),
                      &swpParent);

   swpMe.x = swpParent.x + swpParent.cx / 2 - swpMe.cx / 2;
   swpMe.y = swpParent.y + swpParent.cy / 2 - swpMe.cy / 2;

   swpMe.fs = SWP_MOVE;
   WinSetMultWindowPos (hwnd, &swpMe, 1);


   ItiStrCpy(szTitle, "REPORT: ", sizeof szTitle);
   ItiStrCat(szTitle, pszTitleText, sizeof szTitle);
   hwndTitleBar = WinWindowFromID(hwnd, FID_TITLEBAR);
   WinSetWindowText(hwndTitleBar, szTitle);

   /* -- Show the user that we are really doing something 
    * -- while the initial query of the StandardItem is done. */
   bInitQueryDone = FALSE;

   WinSetPointer (HWND_DESKTOP,
                   WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));
   ItiWndSetHourGlass (TRUE);

   /* -- Disable the "PRINT" button until the WM_INITQUERY message. */
   WinEnableWindow(WinWindowFromID(hwnd, DID_PRINT), FALSE);

   return 0;

   }/* End of Function ItiRptUtInitDlgBox */









USHORT EXPORT ItiRptUtCloseDlgBox (HWND hwnd, PSZ pszCloseText)
   {
   HWND  hwndTitleBar;
   BOOL  bRes;

   hwndTitleBar = WinWindowFromID(hwnd, FID_TITLEBAR);

   if (pszCloseText == NULL)
      bRes = WinSetWindowText(hwndTitleBar, "Closing");
   else
      bRes = WinSetWindowText(hwndTitleBar, pszCloseText);


   WinInvalidateRect(hwnd, NULL, TRUE);               

   return ((USHORT) (bRes != TRUE));
   } /* End of Function ItiRptUtCloseDlgBox */









USHORT EXPORT ItiRptUtChkDlgMsg (HWND   hwnd, USHORT usMessage,
                                 MPARAM mp1,  MPARAM mp2)
   {

   switch (usMessage)
      {
//      case WM_MOUSEMOVE:
//         if (bInitQueryDone)
//            WinSetPointer (HWND_DESKTOP,
//                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));
//         else
//            WinSetPointer (HWND_DESKTOP,
//                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));
//         break;


      case WM_QUERYDONE:
         bInitQueryDone = TRUE;
         WinEnableWindow(WinWindowFromID(hwnd, DID_PRINT), TRUE);
         break;
      }

   return 0;

   }/* End of Function ItiRptUtChkDlgMsg */










USHORT EXPORT ItiRptUtGetDlgDate
                (PSZ pszCmdStr, USHORT usCmdStrLen,
                 PSZ pszDate,   USHORT usDateLen,
                 USHORT usCtrlID,
                 HWND hwnd, USHORT usMessage, MPARAM mp1, MPARAM mp2)
   {
   USHORT usYear  = 0;
   USHORT usMonth = 0;
   USHORT usDay   = 0;
   USHORT us      = 0;
   PSZ    pszTmp  = NULL;
   PSZ    pszTmp2 = NULL;
   PSZ    pszEnv  = NULL; /* MUST be null initialized! */
   CHAR   szMonth [SMARRAY]   = "";
   CHAR   szYear  [SMARRAY]   = "";
   CHAR   szTempStr[2*SMARRAY]= "";
   CHAR   szCpyCmd [2*SMARRAY]= "";

#define NO_DATE_USED_STR    " No Date Used"
#define INVALID_DATE_STR    " Invalid Date"
#define DATA_ENTRY_ERR_STR  " Date Entry Error"

   if ((pszCmdStr == NULL) || (pszDate == NULL))
      return (ITIRPTUT_PARM_ERROR);


   /* -- Get the date to inflate to, if any. */
   us =  WinQueryDlgItemText(hwnd, usCtrlID, usDateLen, pszDate);

   if (ItiStrCmp(pszDate, NO_DATE_USED_STR) == 0)
      {
      pszDate[0] = '\0';
      pszCmdStr[0] = '\0';
      return (ITIRPTUT_ERROR);
      }


   if ((ItiStrCmp(pszDate, INVALID_DATE_STR) == 0)
       || (ItiStrCmp(pszDate, DATA_ENTRY_ERR_STR) == 0))
      {
      WinSetDlgItemText(hwnd, usCtrlID, NO_DATE_USED_STR);
      pszDate[0] = '\0';
      pszCmdStr[0] = '\0';
      return (ITIRPTUT_ERROR);
      }



   if (us < 3) /* -- Do not use any inflate to date, */
      {        /* -- so do not alter the szCmdLn     */
      WinSetDlgItemText(hwnd, usCtrlID, NO_DATE_USED_STR);
      pszDate[0] = '\0';
      pszCmdStr[0] = '\0';
      return (ITIRPTUT_ERROR);
      }
   else if (us < 6)
      {
      WinSetDlgItemText(hwnd, usCtrlID, DATA_ENTRY_ERR_STR);
      pszDate[0] = '\0';
      pszCmdStr[0] = '\0';
      return (ITIRPTUT_ERROR);
      }
   else
      {  /* this section could be spead up */
      us = ItiRptUtConvertToDayOfYr (&pszDate);
      if (us == 0)
         {
         WinSetDlgItemText(hwnd, usCtrlID, INVALID_DATE_STR);
         pszDate[0] = '\0';
         pszCmdStr[0] = '\0';
         return (ITIRPTUT_ERROR);
         }
      else
         {/* build and append the date option to the cmd. */
         ItiStrCpy(szTempStr, ITIRPT_OPTSW_DATE " \"", sizeof szTempStr);

//         ItiRptUtParseDate (&pszDate,&usYear,&usMonth, &usDay);
         if (ItiFmtParseDate(&pszDate,&usYear,&usMonth, &usDay) != 0)
            ItiRptUtErrorMsgBox (hwnd, pszDate);


         ItiRptUtMonthName (usMonth, szMonth, sizeof szMonth);
         szMonth[3] = ' ';
         szMonth[4] = '\0';
         ItiStrCat(szTempStr, szMonth, sizeof szTempStr);

         /* do Day (using szMonth as a buffer). */
         ItiStrUSHORTToString (usDay, szMonth, sizeof szMonth);
         ItiStrCat(szTempStr, szMonth, sizeof szTempStr);
         ItiStrCat(szTempStr, " ", sizeof szTempStr);

         /* -- Do the Year and close quote. */
         ItiStrUSHORTToString (usYear, szYear, sizeof szYear);
         
         if (szYear[4] != '\0')
            {
            szYear[4] = '\0';
            ItiStrToUSHORT(szYear, &usYear);
            ItiStrUSHORTToString (usYear, szYear, sizeof szYear);
            }

         ItiStrCat(szTempStr, szYear, sizeof szTempStr);
         ItiStrCat(szTempStr, "\" ", sizeof szTempStr);

         pszDate[0] = '+'; /* set to non-null */

         /* -- Test if we want to see development messages. */
         DosScanEnv(REPORT_USE_MSGS, &pszEnv);
         if ((pszEnv != NULL) && ((pszEnv[0] == 'Y') || (pszEnv[0] == 'y')) )
            {
            szCpyCmd[0] = DEV_MSG_CHAR;
            szCpyCmd[1] = '\0';
            }
         else
            {
            szCpyCmd[0] = ' ';
            szCpyCmd[1] = '\0';
            }

         ItiStrCat(szCpyCmd, szTempStr, sizeof szCpyCmd);
         ItiStrCpy(pszCmdStr, szTempStr, usCmdStrLen);

         /* -- Show user what date string we are using. */
         pszTmp = szTempStr;
         while (*pszTmp != '"')
            pszTmp++;
         *pszTmp = ' ';

         pszTmp2 = pszTmp;
         while (*pszTmp2 != '"')
            pszTmp2++;
         *pszTmp2 = ' ';

         WinSetDlgItemText(hwnd, usCtrlID, pszTmp);

         }
      }/* end of else clause, if (us <... */


   ItiRptUtErrorMsgBox (hwnd, szCpyCmd);

#undef NO_DATE_USED_STR  
#undef INVALID_DATE_STR  
#undef DATA_ENTRY_ERR_STR

   return (0);
   }/* END OF FUNCTION ItiRptUtGetDlgDate */







USHORT EXPORT ItiRptUtLoadLabels (HMODULE  hmodCurrent,
                                  PUSHORT  pusNumOfTitles,
                                  PCOLTTLS pacttl)
   {
   CHAR   szBuff [RPT_TTL_LEN+1] = "";
   USHORT us;
   USHORT usErr;


   /* -- Prep from meta files. */

   /* get the number of titles. */
   usErr = ItiMbQueryQueryText(hmodCurrent, ITIRID_RPT, ID_RPT_TTL,
                               szBuff, sizeof szBuff );
   ItiStrToUSHORT(szBuff, pusNumOfTitles);

   /* get the titles themselves. */
   for (us=1; us <= *pusNumOfTitles; us++)
      {
      usErr = ItiMbQueryQueryText(hmodCurrent, ITIRID_RPT, us,
                                  szBuff, sizeof szBuff );
      if (usErr != 0)
         break;

      /* Get the title column location. */
      ItiStrToUSHORT (ItiStrTok(szBuff, ", "), &pacttl[us].usTtlCol);

      /* Get the title column width. */
      ItiStrToUSHORT (ItiStrTok(NULL,   ", "), &pacttl[us].usTtlColWth);

      /* Get the data column location. */
      ItiStrToUSHORT (ItiStrTok(NULL, ", "), &pacttl[us].usDtaCol);

      /* Get the data column width. */
      ItiStrToUSHORT (ItiStrTok(NULL,   ", "), &pacttl[us].usDtaColWth);

      /* Get the column title. */
      ItiStrCpy(pacttl[us].szTitle, ItiStrTok(NULL,","),
                sizeof pacttl[us].szTitle);
      }

   return usErr;
   }/* END OF FUNCTION LoadLabels */







USHORT EXPORT ItiRptUtEstablishKeyIDList (HHEAP hhpr,
                                          USHORT usNumArrayElements,
                                          USHORT usElementSize,
                                          PPP_KEYSIDS pppkis)
   {
   USHORT us;

   if (pppkis == NULL)
      return (ITIRPT_ERR);

   us = (usNumArrayElements + 1) * usElementSize;

   /* -- Get space for pointer array. */
   *pppkis = ItiMemAlloc (hhpr, us, MEM_ZERO_INIT);

   if (*pppkis == NULL)
      return (ITIRPT_ERR);

   return(0);
   }/* End of function */




USHORT EXPORT ItiRptUtDestroyKeyIDList (HHEAP hhpr,
                                        PPP_KEYSIDS pppkis, USHORT usMaxIdx)
   {
   USHORT us;

   /* -- Recover memory from any previous allocation. */
   if ((usMaxIdx > 0) && (pppkis != NULL) && (*pppkis != NULL))
      {
      for (us=0; us < usMaxIdx; us++)
         {
         if ((*pppkis)[us] != NULL)
            {
            if ((*pppkis)[us]->pszID != NULL)
               ItiMemFree (hhpr, (*pppkis)[us]->pszID);

            ItiMemFree (hhpr, (*pppkis)[us]);
            }
         }

      ItiMemFree (hhpr, *pppkis);
      }

   *pppkis = NULL; //Added 30 Oct 91

   return(0);
   }/* End of function */





USHORT EXPORT ItiRptUtAddToKeyIDList (HHEAP hhpr,
                                      PSZ pszID,
                                      PSZ pszKey,  
                                      USHORT     usNth,
                                      PP_KEYSIDS ppkis,
                                      USHORT usArrayElementCount)
   {
   USHORT usSize;
   USHORT us = 0;

   if ((pszID == NULL) || (pszKey == NULL))
      return (ITIRPT_INVALID_PARM);

   if (usNth > usArrayElementCount)
      return (ITIRPT_INVALID_PARM + 1);

   /* -- Get space for the Key&Id structure. */
   usSize = sizeof(KEY_ID);
   ppkis[usNth] = ItiMemAlloc(hhpr, usSize, MEM_ZERO_INIT);
   if (ppkis[usNth] == NULL)
      return (ITIRPT_MEM_ERR);

   /* -- Get space for the ID string that will come from the row data.  */
   usSize = ItiStrLen(pszID) + 4; /* term char plus extra for adding "s */

   ppkis[usNth]->pszID = ItiMemAlloc(hhpr, usSize, MEM_ZERO_INIT);
   if (ppkis[usNth]->pszID == NULL)
      return (ITIRPT_MEM_ERR);

   ItiStrCpy(ppkis[usNth]->pszID, pszID, usSize);


   /* -- Get the breakdown key value from the row data. */
   ItiStrCpy(ppkis[usNth]->szKey, pszKey, SZKEY_LEN);


   ppkis[usNth]->dVal = 0.0;

   return 0;
   }/* END OF FUNCTION ItiRptUtAddToKeyIDList */







USHORT ItiRptUtMakeList(HMODULE hmodML,    HHEAP hhpML,
                        PSZ pszQry,        USHORT usQryID,
                        PUSHORT pusKeyCnt, PPP_KEYSIDS pppkisML)
/* -- This function builds a Key&ID list from the given query.  */
/* -- Assumes query column 0 is the ID and column 1 is the Key. */
   {
   USHORT  us, usLen;
   HQRY    hqryML   = NULL;
   USHORT  usColsML = 0;
   USHORT  usErrML  = 0;
   PSZ   * ppszML   = NULL;
   PSZ     pszIDCol, pszKeyCol;
   CHAR    szTmp[2 * SMARRAY] = "";


   if ((pszQry == NULL) || (pppkisML == NULL) || (pusKeyCnt == NULL))
      return ITIRPTUT_PARM_ERROR;

   /* -- Find out how many keys (rows) we have. */
   *pusKeyCnt = ItiDbGetQueryCount(pszQry, &usColsML, &usErrML);
   ItiRptUtEstablishKeyIDList (hhpML, (*pusKeyCnt + 1),
                               sizeof(P_KEYID), pppkisML);

   /* -- Do the database query. */
   hqryML = ItiDbExecQuery (pszQry, hhpML, hmodModule, ITIRID_RPT,
                            usQryID, &usColsML, &usErrML);
   if (hqryML == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, " ItiRptUtMakeList query failed.");
      return ITIRPTUT_ERROR;
      }
   else
      {/* -- Build a list of category keys. */
      us = 0;
      while( ItiDbGetQueryRow (hqryML, &ppszML, &usErrML) )
         {
         if ((ppszML == NULL) || (*ppszML == NULL))
            break;

         pszIDCol  = (ppszML[0] != NULL) ? ppszML[0] : " N/A ";
         pszKeyCol = (ppszML[1] != NULL) ? ppszML[1] : " 0 ";
         ItiRptUtAddToKeyIDList(hhpML, pszIDCol, pszKeyCol,
                                us, *pppkisML, *pusKeyCnt);

         ItiStrCpy(szTmp, " '", sizeof szTmp);
         ItiStrCat(szTmp, (*pppkisML)[us]->pszID, sizeof szTmp);
         ItiStrCat(szTmp, "' ", sizeof szTmp);
         usLen = ItiStrLen(szTmp) +1;
         ItiStrCpy((*pppkisML)[us]->pszID, szTmp, usLen);

         us++;   /* the variable us keeps count of the query row number */

         ItiFreeStrArray (hhpML, ppszML, usColsML);
         }
      }/* end if (hqry... else clause */


   return 0;

   }/* END OF FUNCTION ItiRptUtMakeList */







USHORT EXPORT ItiRptUtCurrentRptPID (VOID)
   {
   return pidRpt;
   }



USHORT EXPORT ItiRptUtCurrentRptSID (VOID)
   {
   return -1;
   }







/* ----------------------------------------------------------------------- *\
 * ItiRptUtDayOfWeek  (Not yet ready)                                      *
 *                                                                         *
 * Given a date, this function returns which day of the week               *
 * that date falls upon; i.e. 4 21 1992 is a Tuesday.                      *
 *                                                                         *
 * If an error occurs the return value is ITIRPTUT_ERROR,                  *
 * otherwise the returned value is ordinal value of the day;               *
 * Sun=0, Mon=1, Tue=2...                                                  *
 *                                                                         *
\* ----------------------------------------------------------------------- */

USHORT EXPORT ItiRptUtDayOfWeek (INT month, INT day, INT year,
                                 PSZ pszDay, USHORT pszLen)
   {
   signed char calendar[13] = { 0, 1, -1, 0, 0, 1, 1, 2, 3, 3, 4, 4 };

   INT index;
   USHORT us, usCnt;

   if (year < 50)
      year = year + 2000;
   else if (year < 99)
      year = year + 1900;


   /* -- Check if we were given a valid date and place to put the result. */
   if ((pszDay == NULL) || (!IsValidDay(month, day, year)) )
      return ITIRPTUT_ERROR;
   
   year = year - 1950;/* Jan 1,1950 is a sunday, the 1st day of the week. */


   /* -- Add up all of the days of the months prior to the given month. */
   for (us = 1; us < (USHORT)month; us++)
      usCnt = usCnt + DaysInMonth (us, (USHORT)year);

   /* -- Now add the day of the given month to the total of days in the *
    *    previous months.                                               */
   usCnt = usCnt + (USHORT)day;
   /* -- usCnt now equals the day of year value. */


//   switch (year) /* bogus kludge */
//      {
//      case 1992:
//         usCnt = usCnt - 1;
//         break;
//
//      case 1993:
//         usCnt = usCnt + 1;
//         break;
//
//      case 1994:
//         usCnt = usCnt + 4;
//         break;
//
//      default:
//         break;
//      }
   
   index = (int)usCnt % 7;

   if ((index > 6) || (index < 0))
      index = 0;
   else
      index = index + 1;

   ItiStrCpy(pszDay, apszDays[index], pszLen);
   return (USHORT)index;
   }


