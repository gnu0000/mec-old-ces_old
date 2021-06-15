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


/*  ItiRptUt                   (Online QuickHelp is available for this module)
 *  
 *    This module provides report support functions for the various report 
 *    DLLs.
 *
 *
 *       ItiDllQueryVersion
 *       ItiDllQueryCompatibility
 *       ItiRptUtSetError    
 *       ItiRptUtInitKeyList      Establishes the Key values list to be used.
 *       ItiRptUtGetNextKey       Retrieves the next key from the list.
 *       ItiRptUtPrepQuery
 *       ItiRptUtGetOption        Retrieves an option switch's value if it was
 *                                given on the command line.
 *
 */

#if !defined (INCL_ITIRPTUT)
#define  INCL_ITIRPTUT


#define  ITIRPTUT_DLL_VERSION  1


/* NOTICE: The switch characters below must match those used in
 *         itirpt.h's manifest constants ITIRPT_OPTSW_...
 */
#define  ITIRPTUT_FILE_SWITCH      'K'
#define  ITIRPTUT_FOOTER_SWITCH    'F'
#define  ITIRPTUT_ORDERING_SWITCH  'O'
#define  ITIRPTUT_DATE_SWITCH      'D'

#define  ITIRPTUT_KEY_QUERY_SW_VAL '@'
#define  ITIRPTUT_KEY_QUERY_SW_STR "@"



/* -- This switch means to use only active cost sheets for jobs. */
#define ACTIVEONLY_SWITCH   'A'



#define  CLOSE_TXT   " Closing Window "

// -- The following `formats' are used by ItiRptUtMath Function via the
// -- ItiRMath.exe module.
// -- (See the ItiRptUt.met file.)
// --
// *** These 'format' defines are used in the ItiRptUt.MET file; so if you
// *** add something here be sure to put it in the .MET file.

#define  ID_EXP            1300
#define  STR_EXP           "select "

#define  ID_EXP_2D         1301
#define  STR_EXP_2D        "Number,$.."

#define  ID_EXP_2DC        1302
#define  STR_EXP_2DC       "Number,$,.."

#define  ID_EXP_3DC        1303
#define  STR_EXP_3DC       "Number,$,..."

#define  ID_EXP_4DC        1304
#define  STR_EXP_4DC       "Number,$,...."

#define  ID_KEY            1305
#define  STR_KEY           "Number"

#define  ID_EXP_2          1306
#define  STR_EXP_2         "Number,.."

#define  ID_EXP_4          1307
#define  STR_EXP_4         "Number,...."

#define  ID_EXP_10         1308
#define  STR_EXP_10        "Number,.........."

#define  ID_EXP_4C         1309
#define  STR_EXP_4C        "Number,,...."




#define  ERROR_KEY               "0"
#define  ERROR_KEY_CHAR          '0'
//                      The zero key character.

#define  DEV_MSG              "~"
#define  DEV_MSG_CHAR         '~'
//                      When this is the first character of a string
//                      that is passed to ItiRptUtErrorMsgBox then
//                      the string is ALWAYS displayed in a message
//                      box;  If the setting of the environment 
//                      string "REPORTMSGS" is "Y" or "y" the MsgBox
//                      will be shown.


#define  RESULT_SIZE                    512
//                  ...of the sharable memory.

#define  REPORT_USE_MATH      "REPORTMATH"
#define  REPORT_USE_MSGS      "REPORTMSGS"

/* ----------------------------------------------------------------------- *\
 *                                                            Error Codes  *
\* ----------------------------------------------------------------------- */
#define  ITIRPTUT_NO_MORE_KEYS                 (ITIRPTUT_ERROR_BASE +  1)
#define  ITIRPTUT_USE_DEFAULT                  (ITIRPTUT_ERROR_BASE +  3)
#define  ITIRPTUT_OPTION_NOT_FOUND             (ITIRPTUT_ERROR_BASE +  5)
#define  ITIRPTUT_CSV_BUFFER_TOO_SMALL         (ITIRPTUT_ERROR_BASE +  7)

#define  ITIRPTUT_FILE_IO_ERROR                (ITIRPTUT_ERROR_BASE + 11) 
#define  ITIRPTUT_FILE_READ_ERROR              (ITIRPTUT_ERROR_BASE + 17) 
#define  ITIRPTUT_OPEN_FAILED                  (ITIRPTUT_ERROR_BASE + 23) 
#define  ITIRPTUT_PROCESSING                   (ITIRPTUT_ERROR_BASE + 29) 

#define  ITIRPTUT_DONE                         (ITIRPTUT_ERROR_BASE + 31) 
#define  ITIRPTUT_EOF                          (ITIRPTUT_ERROR_BASE + 37) 
#define  ITIRPTUT_NO_MORE_ROWS                 (ITIRPTUT_ERROR_BASE + 51) 
#define  ITIRPTUT_ERROR                        (ITIRPTUT_ERROR_BASE + 57) 

#define  ITIRPTUT_PARM_ERROR                   (ITIRPTUT_ERROR_BASE + 61) 
#define  ITIRPTUT_NO_SELECTED_KEYS             (ITIRPTUT_ERROR_BASE + 63)


// The following are used as message parameters to BM_SETCHECK's mp1  
// for checkbox controls.
#define  UNCHECKED             MPFROMSHORT((USHORT) 0)
#define  CHECKED               MPFROMSHORT((USHORT) 1)
#define  INDETERM              MPFROMSHORT((USHORT) 2)



#if !defined (INCL_ITIRPTDG)
#include "..\include\itirptdg.h"
#endif


#if !defined (COL_TITLES)

#define COL_TITLES

typedef struct _ColTtls                     
           {                                  
           CHAR    szTitle [RPT_TTL_LEN+1];   
           USHORT  usTtlCol;                  
           USHORT  usTtlColWth;               
           USHORT  usDtaCol;                  
           USHORT  usDtaColWth;               
           } COLTTLS; /* cttl */            

typedef COLTTLS    * PCOLTTLS;  /* pcttl */
     
#endif /* !defined (COL_TITLES) */




#if !defined (KEY_AND_ID)

#define KEY_AND_ID

#define SZKEY_LEN          32
typedef struct _Key_ID
   {
   PSZ    pszID;
   CHAR   szKey[SZKEY_LEN];
   double dVal;
   }KEY_ID; /* keyid */

typedef KEY_ID      *P_KEYID;     /* pkeyid */
typedef P_KEYID     *PP_KEYSIDS;  /* ppkis  */
typedef PP_KEYSIDS  *PPP_KEYSIDS; /* pppkis */

#endif /* !defined (KEY_AND_ID) */



#if !defined (ITIRPTUT_STR_LST)

#define ITIRPTUT_STR_LST

typedef struct _Str_Lst
   {
   PSZ *ppszStrLst;
   USHORT usStrCnt;
   }STR_LST;

#endif /* !defined (ITIRPTUT_STR_LST) */























/* ======================================================================= *\
 *                                                         Function Names  *
\* ======================================================================= */
extern USHORT EXPORT ItiDllQueryVersion (VOID);

extern BOOL   EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);

extern PSZ    EXPORT ItiRptUtItoa (INT iNum, CHAR str[], INT iBase);



/* ------------------------------------------------------------------------ *\
 * ItiRptUtUniqueFile                                                       *
 *                                                                          *
 *    This function generates a unique file name.                           *
 *                                                                          *
 * Parameters: pszFileName    The buffer space to receive the file name.    *
 *             usLen          The size of the buffer space.                 *
 *             bInclTmpPath   A value of 'true' will preface the file name  *
 *                            with environment string TMP or TEMP iff such  *
 *                            a string exists.  A value of 'false' returns  *
 *                            only a file name without a path.              *
 *                                                                          *
 * Return value: 0 if successful.                                           *
 *                                                                          *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtUniqueFile (PSZ pszFileName, USHORT usLen,
                                         BOOL bInclTmpPath );

/* -- Displays pszErrorText in a message box on screen. */
extern USHORT EXPORT ItiRptUtErrorMsgBox (HWND hwnd, PSZ pszErrorText);


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
extern USHORT EXPORT ItiRptUtSetError    (HAB hab, PSZ pszErrorText);


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
extern USHORT EXPORT ItiRptUtInitKeyList (INT iArgCnt, CHAR * pArgVar[]);



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
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtGetNextKey  (PSZ pszKeyStr, USHORT usKeyLen);



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
 * Comments:  If pszKeyStr is NULL or starts with a '\0' then               *
 *            neither it nor the pszConj is appended to pszQ.               *
\* ------------------------------------------------------------------------ */
extern USHORT EXPORT ItiRptUtPrepQuery
   (PSZ pszQ, USHORT usQLen, PSZ pszQueryBase, PSZ pszConj, PSZ pszKeyStr);



extern USHORT EXPORT ItiRptUtGetOption
   (CHAR cOpt, PSZ pszOptionStr, USHORT usLenOptionStr,
    INT argcnt, CHAR *argvars[]);



extern USHORT EXPORT ItiRptUtKeySuppInfo (PSZ pszBuffer, USHORT usBufferSize);




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
                            (USHORT usMonthNum, PSZ pszMonth, USHORT usLen);

  
extern BOOL EXPORT ItiRptUtParseDate (PSZ       *ppszDate, 
                                      PUSHORT   pusYear, 
        /* from ItiFmt date.c */      PUSHORT   pusMonth, 
                                      PUSHORT   pusDay);



/* ----------------------------------------------------------------------- *\
 * ItiRptUtConvertToDayOfYr                                                *
 *                                                                         *
 * Given a date string this function returns which day of the year         *
 * that date falls upon; ie 1991, 2, 14 is the 46th day of the year.       *
 *                                                                         *
 * If an invalid date is given the return value is 0.                      *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtConvertToDayOfYr (PSZ *ppszDate);




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
extern USHORT EXPORT ItiRptUtDateDiff (PSZ pszDate1, PSZ pszDate2);



/* ----------------------------------------------------------------------- *\
 * ItiRptUtMath                                                            *
 *         Uses SQL to do a math experesion.  SigFigs of SQL.              *
 * Parms:                                                                  *
 *    usFmt    Result format, refer to the ItiRptUt.MET file               *
 *    usResLen Result buffer length.                                       *
 *    pszRes   Result buffer pointer.                                      *
 *    pszExp1  Math experesion, example:  (4 * 3)                          *
 *    pszOp    Operator:  - + * /                                          *
 *    pszExp2  Math experesion, example:   5                               *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtMath (USHORT usFmt, USHORT usResLen, PSZ pszRes, 
                                   PSZ pszExp1, PSZ pszOp, PSZ pszExp2);




/* ----------------------------------------------------------------------- *\
 * ItiRptUtStartKeyFile                                                    *
 * ItiRptUtWriteKeyKeyFile                                                 *
 * ItiRptUtEndKeyFile                                                      *
 *                                                                         *
 * The following functions are for generating a report key file;           *
 * Note that there is currently only one key file per report so the        *
 * pszFileName can be NULL.                                                *
 *                                                                         *
 * ItiRptUtStartKeyFile                                                    *
 *    If pszFileName is not NULL and the zeroth character is not '\0'      *
 *    then a uniqfile name is generated and copied (as musch as will fit)  *
 *    into the pszFileName string; the phfile value is assigned for use by *
 *    the other two functions.  If pszFileName is NULL the phfile is to    *
 *    an annonamous file.                                                  *
 *                                                                         *
 * ItiRptUtWriteKeyKeyFile                                                 *
 *    Must be given the hfile set by ItiRptUtStartKeyFile.                 *
 *    For now none of the reports use the pszSuppInfo string so it is      *
 *    ignored (it can be NULL).                                            *
 *                                                                         *
 * ItiRptUtEndKeyFile                                                      *
 *    Must be called to close the file or the file will be empty!          *
 *                                                                         *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtStartKeyFile (PHFILE phfile,
                                           PSZ pszFileName, USHORT usLen);
extern USHORT EXPORT ItiRptUtWriteToKeyFile (HFILE hfile,
                                             PSZ pszKey, PSZ pszSuppInfo);
extern USHORT EXPORT ItiRptUtEndKeyFile (HFILE hfile);







extern USHORT EXPORT ItiRptUtErrorLogMsg (PSZ pszErrorText);
/* not yet imp */


/* ----------------------------------------------------------------------- *\
 * ItiRptUtBuildSelectedKeysFile                                           *
 *                                                                         *
 *    This function 
 *                                                                         *
 * Parameters:    
 *                                                                         *
 * Return value:  0 if successful                                          *
 *                ITIRPTUT_PARM_ERROR or ITIRPTUT_ERROR if unsuccessful.   *
 *                                                                         *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtBuildSelectedKeysFile
                        (HWND   hwnd,
                         USHORT usListWndID,
                         USHORT usColumnID,
                         PSZ    pszKeyFileName,
                         USHORT usLenKeyFileName);








extern USHORT EXPORT ItiRptUtExecRptCmdLn
                        (HWND hwnd, PSZ pszCmdLn, PSZ pszSessionName);

extern USHORT EXPORT ItiRptUtInitDlgBox
                        (HWND hwnd, PSZ pszTitleText, USHORT us, PSZ *ppsz);

extern USHORT EXPORT ItiRptUtChkDlgMsg (HWND   hwnd, USHORT usMessage,
                                        MPARAM mp1,  MPARAM mp2);



/* ----------------------------------------------------------------------- *\
 * ItiRptUtGetDlgDate                                                      *
 *                                                                         *
 *    This function gets a valid date from a dialog box control that is    *
 *    of the type WC_ENTRYFIELD.                                           *
 *    Need more of an explination here.                                    *
 *                                                                         *
 * Parameters:    pszCmdLn    The CmdLn string to be appended.             *
 *                pszDate     Holds the valid date or error msg string.    *
 *                usCmdLnLen  SizeOf the CmdLn string.                     *
 *                usDateLen   SizeOf the Date string.                      *
 *                usCtrlID    The dialog box's control ID, DID_DATEEDIT.   *
 *                hwnd        Dialog box window handle.                    *
 *                usMessage   Dialog box window's message.                 *
 *                mp1         Dialog box window's 1st msg parameter.       *
 *                mp2         Dialog box window's 2nd msg parameter.       *
 *                                                                         *
 * Return value:  0 if successful.                                         *
 *                ITIRPTUT_PARM_ERROR if any of the PSZs are NULL.         *
 *                ITIRPTUT_ERROR if invalid date or otherwise unsuccessful.*
 *                                                                         *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtGetDlgDate
                       (PSZ pszCmdStr, USHORT usCmdStrLen,
                        PSZ pszDate,  USHORT usDateLen,
                        USHORT usCtrlID,
                        HWND hwnd, USHORT usMessage, MPARAM mp1, MPARAM mp2);



extern USHORT EXPORT ItiRptUtCloseDlgBox (HWND hwnd, PSZ pszCloseText);


extern USHORT EXPORT ItiRptUtLoadLabels (HMODULE  hmodCurrent,
                                         PUSHORT  pusNumOfTitles,
                                         PCOLTTLS pacttl);



extern USHORT EXPORT ItiRptUtEstablishKeyIDList (HHEAP hhpr,
                                          USHORT usNumArrayElements,
                                          USHORT usElementSize,
                                          PPP_KEYSIDS pppkis);


extern USHORT EXPORT ItiRptUtDestroyKeyIDList (HHEAP hhpr,
                                        PPP_KEYSIDS pppkis, USHORT usMaxIdx);


extern USHORT EXPORT ItiRptUtAddToKeyIDList (HHEAP hhpr,
                                             PSZ pszID,
                                             PSZ pszKey,  
                                             USHORT     usNth,
                                             PP_KEYSIDS ppkis,
                                             USHORT usArrayElementCount);


/* -- This function builds a Key&ID list from the given query;  *
 * -- that is function uses ItiRptUtEstablishKeyIDList and      *
 * -- ItiRptUtAddToKeyIDList to build a 'standard' key&ID list. *
* == Don't use this function yet; use the 3 functions above for now. <<<< *
 * -- Assumes query column 0 is the ID and column 1 is the Key! */
extern USHORT ItiRptUtMakeList(HMODULE hmodML,         /* Caller's */
                               HHEAP hhpML,            /* Caller's */
                               PSZ pszQry,             /* Key&ID SQL Query */
                               USHORT usQryID,         /* Met file QueryID */
                               PUSHORT pusKeyCnt,      /* # of keys found  */
                               PPP_KEYSIDS pppkisML);  /* pointer to list  */



/* -- The following functions return the PID and SID values set *
 * -- by the LAST call to the ItiRptUtExecRpt function.         */
extern USHORT EXPORT ItiRptUtCurrentRptPID (VOID);

extern USHORT EXPORT ItiRptUtCurrentRptSID (VOID);



/* ----------------------------------------------------------------------- *\
 * ItiRptUtDayOfWeek                                                       *
 *                                                                         *
 * Given a date, this function returns which day of the week               *
 * that date falls upon; i.e. 4 21 1992 is a Tuesday.                      *
 *                                                                         *
 * If an error occurs the return value is ITIRPTUT_ERROR,                  *
 * otherwise the returned value is ordinal value of the day;               *
 * Sun=0, Mon=1, Tue=2...                                                  *
 *                                                                         *
 * The pszDay is filled with the first pszLen letters of the day name; the *
 * first letter is capitalized.                                            *
\* ----------------------------------------------------------------------- */
extern USHORT EXPORT ItiRptUtDayOfWeek (INT month, INT day, INT year,
                                        PSZ pszDay, USHORT pszLen);


#endif    /* if !defined (INCL_ITIRPTUT) */

