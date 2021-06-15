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
 * support.c
 * Copyright (C) 1990 Info Tech, Inc.
 *
 */

#define INCL_WIN
#define INCL_DOS
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <share.h>
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "window.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "..\include\dialog.h"
#include "..\include\itirpt.h"
#include "dialog.h"
#include "init.h"
#include "itimenu.h"
#include "dumpdll.h"


#define BUFFER_SIZE  2048

#define DLL_VERSION   1

/* do NOT use Z or H for an option switch */

#define OPT_PID      'P' 
#define OPT_PID_CHR  " /P " 

#define OPT_CMD      'C' 
#define OPT_CMD_CHR  " /C " 

#define OPT_SD      'S' 
#define OPT_SD_CHR  " /S " 

#define OPT_LD      'L' 
#define OPT_LD_CHR  " /L " 

#define OPT_REQID        'I' 
#define OPT_REQID_CHR    " /I " 

#define OPT_REQTYPE      'T' 
#define OPT_REQTYPE_CHR  " /T " 

#define OPT_REQPRI       'R' 
#define OPT_REQPRI_CHR   " /R " 

#define OPT_REQUSER      'U' 
#define OPT_REQUSER_CHR  " /U " 

#define OPT_REQREAL      'Y' 
#define OPT_REQREAL_CHR  " /Y " 


#define OPT_REQVOICE     'V' 
#define OPT_REQVOICE_CHR " /V " 

#define OPT_REQFAX       'X' 
#define OPT_REQFAX_CHR   " /X " 

#define OPT_LOG          'G' 
#define OPT_LOG_CHR      " /G " 

#define REQ_ID    0
#define REQ_TYPE  1
#define REQ_PRI   2
#define REQ_SD    3
#define REQ_LD    4
#define REQ_USER  5
#define REQ_REAL  6
#define REQ_VOICE 7
#define REQ_FAX   8
#define REQ_LOG   9




static CHAR szDllVersion[] = "1.1a0 Support.dll";



char szNum[] = ".000";
char szPID[256] = "";
char szCmd[640]= "";




BOOL QueryDriveFreeSpace (int    iDrive, 
                          PULONG pulTotalSpace, 
                          PULONG pulFreeSpace)
   {
   FSALLOCATE fsalloc;

   if (DosQFSInfo (iDrive, FSIL_ALLOC, (PBYTE) &fsalloc, sizeof fsalloc))
      {
      if (pulTotalSpace != NULL)
         *pulTotalSpace = 0;
      if (pulFreeSpace != NULL)
         *pulFreeSpace = 0;
      return FALSE;
      }

   if (pulTotalSpace != NULL)
      {
#ifdef cUnit
#undef cUnit
#endif
      *pulTotalSpace = (ULONG) fsalloc.cbSector *
                       (ULONG) fsalloc.cSectorUnit *
                       (ULONG) fsalloc.cUnit;
      }

   if (pulFreeSpace != NULL)
      {
      *pulFreeSpace = (ULONG) fsalloc.cbSector *
                      (ULONG) fsalloc.cSectorUnit *
                      (ULONG) fsalloc.cUnitAvail;
      }
   return TRUE;
   }


void FormatULong (ULONG ulNum, 
                  char  *pszNumber)
   {
   char  szNum [15];
   int   i;
   int   iCount;

   szNum [sizeof szNum - 1] = '\0';
   iCount = 0;
   for (i=sizeof szNum - 2; i >= 0 && ulNum > 9; i--)
      {
      szNum [i] = (char) (signed) ((short) (ulNum % 10) + (short) '0');
      ulNum /= 10;
      iCount++;
      if (iCount >= 3)
         {
         iCount = 0;
         szNum [--i] = ',';
         }
      }

   szNum [i] = (char) ((short) ((signed) (ulNum) + (short) '0'));

   while (i < sizeof szNum)
      {
      *pszNumber = szNum [i];
      pszNumber++;
      i++;
      }
   }


void DoDriveSpace (void)
   {
   USHORT   i, us;
   char     cDrive;
   ULONG    ulFreeSpace;
   ULONG    ulDriveTotal;
   USHORT   usCurrentDisk;
   ULONG    ulDrives;
   char     szTemp [50];
   PSZ      pszBuffer, psz;
   PUSHORT  pus;

   pszBuffer = ItiMemAllocSeg (BUFFER_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   DosError (HARDERROR_DISABLE);
   DosQCurDisk (&usCurrentDisk, &ulDrives);
   cDrive = 'A';
   for (i = 0; i < 26; i++, cDrive++)
      {
      if (!(i == 0) && !(i == 1))
         {
         if (ulDrives & 1)
            {
            if (!QueryDriveFreeSpace (i + 1, &ulDriveTotal, &ulFreeSpace))
               continue;

            sprintf (szTemp, "Drive %c: ", cDrive);

            ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, szTemp);
            FormatULong (ulFreeSpace, szTemp);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, RIGHT, REL_LEFT | 14, szTemp);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT,  REL_LEFT | 12, " bytes free, ");
            FormatULong (ulDriveTotal, szTemp);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, RIGHT, REL_LEFT | 14, szTemp);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT,  USE_RT_EDGE,   " bytes total.");

            us = BUFFER_SIZE;
            sprintf (szTemp, "%c:", cDrive);
            if (pszBuffer &&
                !DosQFSAttach (szTemp, 0, FSAIL_QUERYNAME, pszBuffer, &us, 0))
               {
               pus = (PUSHORT) (pszBuffer) + 1;
               /* get a pointer to the szFSDName field */
               psz = pszBuffer + 3 * sizeof (USHORT) + *pus + 1;

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT,  USE_RT_EDGE, "  (" );
               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT,  USE_RT_EDGE, psz );
               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT,  USE_RT_EDGE, ")" );
               }

            ItiPrtIncCurrentLn ();
            }
         }
      ulDrives >>= 1;
      }
   if (pszBuffer)
      {
      ItiMemFreeSeg (pszBuffer);
      }
   DosError (HARDERROR_ENABLE);
   }


static ULONG QueryFileSize (PSZ pszFileName)
   {
   FILEFINDBUF findbuf;
   USHORT      usNumFiles;
   HDIR        hdir;
   USHORT      usError;

   hdir = HDIR_CREATE;
   usError = DosFindFirst2 (pszFileName, &hdir, FILE_NORMAL, 
                            &findbuf, sizeof findbuf, 
                            &usNumFiles, FIL_STANDARD, 0);
   if (usError)
      return 0;

   DosFindClose (hdir);
   return findbuf.cbFileAlloc;
   }


void DoConfigDump (void)
   {     
   char     szFile [300];
   char     szLine [300];
   PSZ      psz;
   FILE     *pf;
   USHORT   usLen, i;
   ULONG    ulSize;

   sprintf (szFile, "%c:\\config.sys",
            (UCHAR) (pglobals->pgis->bootdrive + 'A' - 1));
   pf = _fsopen (szFile, "rt", SH_DENYNO);
   if (pf == NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Could not open file  ");
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, WRAP | LEFT, USE_RT_EDGE, szFile);
      ItiPrtIncCurrentLn ();
      return;
      }

   while (fgets (szLine, sizeof szLine, pf))
      {
      usLen = strlen (szLine);
      if (usLen != 0)
         usLen--;
      szLine [usLen] = '\0';
      if (strnicmp (szLine, "swap", 4)    == 0 ||
          strnicmp (szLine, "libpath", 7) == 0 ||
          strnicmp (szLine, "memman", 6)  == 0)
         {
         ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, szLine);
         ItiPrtIncCurrentLn ();
         }
      if (strnicmp (szLine, "swappath", 8) == 0)
         {
         /* search for first non-space after '"' */
         for (i=8; szLine [i] != '\0' && szLine [i] != '='; i++)
            ;
         if (szLine [i] == '=')
            i++;
         for (; szLine [i] != '\0' && szLine [i] == ' '; i++)
            ;

         /* find the end of the path, and chop the rest of the line */
         for (psz = szLine + i; *psz != '\0' && 
                                *psz != ' '  && 
                                *psz != '\n' &&
                                *psz != '\t' &&
                                *psz != '\r'; psz++)
            ;
         *psz = '\0';

         /* copy path to destination */
         strncpy (szFile, szLine + i, sizeof szFile);
         strncat (szFile, "\\SWAPPER.DAT", sizeof szFile - strlen (szFile) - 1);
         
         ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Swap file size = ");
         ulSize = QueryFileSize (szFile);
         FormatULong (ulSize, szLine);
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT, USE_RT_EDGE, szLine);
         
         ItiPrtIncCurrentLn ();
         }
      }
   fclose (pf);
   }



void ItiIncId (PSZ psz)
   {
   if (psz[3] < '9')
      psz[3] += 1;

   else if (psz[2] < '9')
      {
      psz[2] += 1;
      psz[3] = '0';
      }
   else
      {
      psz[1] += 1;
      psz[2] = '0';
      psz[3] = '0';
      }
   }






USHORT SetReportTitles (PREPT preptitles)
   {
   USHORT us = 0;

   if (preptitles == NULL)
      return ITIRPT_INVALID_PARM;

   /* -- Set the title text. */
   ItiStrCpy(preptitles->szTitle, "OS/2 BAMS Support Request",
             sizeof(preptitles->szTitle) );


   /* -- Set the subtitle text. */
   ItiStrCpy(preptitles->szSubTitle, "",
             sizeof(preptitles->szSubTitle));


   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle[0] = '\0';


   /* -- Set the default footer text. */
   ItiStrCpy(preptitles->szFooter, "",
             sizeof(preptitles->szFooter) );

   return 0;
   }/* End of Function SetReportTitles */





USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2, PSZ *ppsz)
   {
   USHORT us;
   CHAR   szCommandLn[3024] = "";


   ItiStrCpy (szCommandLn, " ItiMenu ", sizeof szCommandLn);

   ItiStrCat (szCommandLn, OPT_PID_CHR, sizeof szCommandLn); 
   ItiStrCat (szCommandLn, szPID, sizeof szCommandLn);

   ItiStrCat (szCommandLn, OPT_CMD_CHR, sizeof szCommandLn); 
   ItiStrCat (szCommandLn, szCmd, sizeof szCommandLn);

   if (ppsz != NULL)
      {
      ItiStrCat (szCommandLn, OPT_LD_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_LD], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQID_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_ID], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQTYPE_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_TYPE], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQPRI_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_PRI], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQUSER_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_USER], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQREAL_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_REAL], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQVOICE_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_VOICE], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_REQFAX_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_FAX], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_LOG_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_LOG], sizeof szCommandLn);

      ItiStrCat (szCommandLn, OPT_SD_CHR, sizeof szCommandLn); 
      ItiStrCat (szCommandLn, ppsz[REQ_SD], sizeof szCommandLn);
      }


   us = ItiRptUtExecRptCmdLn (hwnd, szCommandLn, "OS/2 BAMS Support Request");

   return (us);

   }/* End of PrintProc Function */







void ItiGenerateSupportFile (HWND hwnd, HHEAP hheap, PSZ **pppsz)
   {
   char      szFile [128];
   char      szRptFile [512];
   FILE      *fp;
   FILE      *fpR;
   USHORT    uSel, uOffset;
   PLINFOSEG pli;
   PSZ       pszEnv, pszTmp;
   char      szType     [40];
   char      szPriority [40];
   char      szTopic    [128];
   char      szLDesc    [2048];
   char      szUserName [128];
   char      szRealName [128];
   char      szVoice    [128];
   char      szFax      [128];
   char      szID       [128];
//   char      szTmp      [255];
   char      szTmpUserName [128];
   char      szTmpRealName [128];
   char      szTmpVoice    [128];
   char      szTmpFax      [128];
   char      szTmpType     [50];
   char      szTmpPriority [50];
   char      szTmpTopic    [128];
   char      szTmpLDesc    [sizeof szLDesc];
   char      szLogFile     [256];


   strcpy (szLogFile, ItiErrQueryLogFileName ());

   strcpy (szFile, ItiErrQueryLogFileName ());
   strcat (szFile, szNum);

   if (!(fp = fopen (szFile, "w")))
      return;


   /*--- SuperKludge(tm) for now ---*/
   
   strcpy (szUserName, pglobals->pszUserName);
   strcpy (szRealName, pglobals->pszUserName);
   strcpy (szVoice,    pglobals->pszPhoneNumber);
   strcpy (szFax,      "0-000-000-0000");
   strcpy (szID,       szNum+1);

   ItiIncId (szNum);

   WinQueryWindowText (WinWindowFromID (hwnd, DID_TYPE),
                         sizeof szType, szType);
   WinQueryWindowText (WinWindowFromID (hwnd, DID_PRIORITY),
                         sizeof szPriority, szPriority);
   WinQueryWindowText (WinWindowFromID (hwnd, DID_TOPIC),
                         sizeof szTopic, szTopic);
   WinQueryWindowText (WinWindowFromID (hwnd, DID_LDESC),
                         sizeof szLDesc, szLDesc);

   /* -- remove '\r' from szLDesc. */
   pszTmp = szLDesc;
   while ((pszTmp != NULL) && (*pszTmp != '\0'))
      {
      if (*pszTmp == '\r')
         *pszTmp = ' ';
      pszTmp++;
      }

   ItiRptUtUniqueFile (szRptFile, sizeof szRptFile, TRUE);

   if (!(fpR = fopen (szRptFile, "w")))
      {
      ItiStrCpy(szTmpLDesc, " \" Failed to create description file. \" ", sizeof szTmpLDesc);
//      return;
      }
   else
      {
      fputs (szLDesc, fpR);
      fclose (fpR);
      /* -- The filename is what will be a synm. for the long description. */
      ItiStrCpy(szTmpLDesc, szRptFile, sizeof szTmpLDesc);
      }

   fprintf (fp, "[USER INFO]\n");
   fprintf (fp, "User Name    : %s\n", szUserName);
   fprintf (fp, "Real Name    : %s\n", szRealName);
   fprintf (fp, "Voice #      : %s\n", szVoice);
   fprintf (fp, "Fax #        : %s\n", szFax);
   fprintf (fp, "\n");

   fprintf (fp, "[REQUEST INFO]\n");
   fprintf (fp, "Request ID   : %s\n", szID);
   fprintf (fp, "Request Type : %s\n", szType);
   fprintf (fp, "Priority     : %s\n", szPriority);
   fprintf (fp, "Topic        : %s\n", szTopic);
   fprintf (fp, "Log File     : %s\n", szLogFile);
   fprintf (fp, "\n");

   fprintf (fp, "[DESCRIPTION]\n");
   fprintf (fp, "Long Description\n----------------\n");
   fprintf (fp, "%s\n", szLDesc);
   fprintf (fp, "\n");



   sprintf (szTmpUserName, "\" %s \" ", szUserName);
   sprintf (szTmpRealName, "\" %s \" ", szRealName);
   sprintf (szTmpVoice,    "\" %s \" ", szVoice);
   sprintf (szTmpFax,      "\" %s \" ", szFax);
   sprintf (szTmpTopic,    "\" %s \" ", szTopic);
   sprintf (szTmpType,     "\" %s \" ", szType);
   sprintf (szTmpPriority, "\" %s \" ", szPriority);

   /* -- Save the info to pass to the printed report. */
   (*pppsz) = ItiStrMakePPSZ(hheap, 10,
                 szID,            /*   REQ_ID    */
                 szTmpType,       /*   REQ_TYPE  */
                 szTmpPriority,   /*   REQ_PRI   */
                 szTmpTopic,      /*   REQ_TOPIC */
                 szTmpLDesc,      /*   REQ_LD    */
                 szTmpUserName,   /*   REQ_USER  */
                 szTmpRealName,   /*   REQ_REAL  */
                 szTmpVoice,      /*   REQ_VOICE */
                 szTmpFax,        /*   REQ_FAX   */
                 szLogFile);      /*   REQ_LOG   */



   fprintf (fp, "[SYSTEM INFO]\n");
   fprintf (fp, "Environment Data\n----------------\n");

   DosGetInfoSeg (&uOffset, &uSel);
   pli = MAKEP (uSel, 0);
   fprintf (fp, "PID          : %u \n", pli->pidCurrent);
   sprintf (szPID, "\"PID: %u \" ", pli->pidCurrent);


   DosGetEnv (&uSel, &uOffset);
   fprintf (fp, "Command Line : %s\n", MAKEP (uSel, uOffset));
   sprintf (szCmd, "\"Command Line: %s \" ", MAKEP (uSel, uOffset));

   pszEnv = MAKEP (uSel, 0);
   while (*pszEnv)
      {
      fprintf (fp, "%s\n", pszEnv);
      pszEnv = strchr (pszEnv, '\0');
      pszEnv++;
      }
   fprintf (fp, "\n");

   fprintf (fp, "\n");
   fclose (fp);
//
//   sprintf (szTmp, "Support Request sent to %s.  Please record this path", szFile);
//   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame,
//                  szTmp, "Support Request", 0, MB_OK | MB_APPLMODAL);
   }





CHAR *ppszType[] =     {"Enhancement",
                        "Documentation",
                        "Bug Report",
                        "Other"};

CHAR *ppszPriority[] = {"Very Low",
                        "Low",
                        "Medium",
                        "High",
                        "Very High",
                        "Urgent",
                        "N/A"};
   
CHAR *ppszTopic[] =     {"Calculation",
                         "Display",
                         "Editing",
                         "Export",
                         "Import",
                         "Printing",
                         "Other"};


MRESULT EXPORT LWSupportDlgProc (HWND    hwnd,
                                 USHORT  uMsg,
                                 MPARAM  mp1,
                                 MPARAM  mp2)
   {
   HHEAP hheap;
   PSZ   *ppsz = NULL;


   switch (uMsg)
      {
      case WM_INITDLG:
         {
         USHORT   i;

         /*--- Fill Comboboxes ---*/
         for (i=0; i < sizeof ppszType / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, DID_TYPE, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszType[i]));

         for (i=0; i < sizeof ppszPriority / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, DID_PRIORITY, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszPriority[i]));

         for (i=0; i < sizeof ppszTopic / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, DID_TOPIC, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszTopic[i]));

         /*--- Select Initial data ---*/
         WinSendDlgItemMsg (hwnd, DID_TYPE,     LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, DID_PRIORITY, LM_SELECTITEM, (MPARAM)2, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, DID_TOPIC,    LM_SELECTITEM, (MPARAM)6, MPFROMP (TRUE));
         break;
         }

      case WM_CLOSE:
         WinDismissDlg (hwnd, 0xffff);
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               hheap = ItiMemCreateHeap(MIN_HEAP_SIZE);
               ItiGenerateSupportFile (hwnd, hheap, &ppsz);
               PrintProc (hwnd, mp1, mp2, ppsz);
               ItiMemDestroyHeap(hheap);

            case DID_CANCEL:
               WinDismissDlg (hwnd, 0xffff);
               break;

            case DID_HELP:
               return (0L);
            }
      }
   return WinDefDlgProc (hwnd, uMsg, mp1, mp2);
   }




USHORT EXPORT ItiMenuDoSupport (HWND hwndList)
   {
   USHORT uRet;

   uRet = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppClient,
                     LWSupportDlgProc, hmodMe, IDD_SUPPORT, hwndList);
   return uRet;
   }




/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
//   if ( ItiVerCmp(szDllVersion) )  
      return (TRUE);
//   return (FALSE);
   }








VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,   
                                 USHORT  usMaxSize,   
                                 PUSHORT pusWindowID) 
   {                                                  
   ItiStrCpy (pszBuffer, "", usMaxSize);   
   *pusWindowID = 0;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   return FALSE;
   }/* END OF FUNCTION */



void DumpCRC (void)
   {
   USHORT   usCurrentPg;
   PSZ      psz;

   if ((psz = getenv ("JOE")) &&
       strcmp (psz, "098ecg mpj-047d-034z-2,23-48u3-cpu845ylkjfs,mv0974-2 ") == 0)
      return;

   ItiPrtNewPg();
   usCurrentPg = ItiPrtQueryCurrentPgNum();
   ItiPrtChangeHeaderLn (ITIPRT_NEWPG, "");

   DumpFiles ();
   }


void DumpLog (PSZ pszLog)
   {
   char     szTemp [256];
   USHORT   usCurrentPg, usLen;
   FILE     *pf;

   ItiPrtNewPg();
   usCurrentPg = ItiPrtQueryCurrentPgNum();
   ItiPrtChangeHeaderLn (ITIPRT_NEWPG, "");

   ItiStrCpy (szTemp, "Log File ", sizeof szTemp);
   ItiStrCat (szTemp, pszLog, sizeof szTemp);
   ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE, szTemp);
   ItiPrtIncCurrentLn();

   /* open the file, and dump all of the lines */
   pf = _fsopen (pszLog, "rt", SH_DENYNO);
   if (pf == NULL)
      ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE, 
                       "Could not open log file!");
   else
      {
      while (fgets (szTemp, sizeof szTemp, pf))
         {
         usLen = strlen (szTemp);
         if (usLen != 0)
            szTemp [usLen-1] = '\0';

         if ((strstr (szTemp, "[01]:  RptDll: r") &&
              strstr (szTemp, "timestamp:")))
            continue;

         ItiPrtPutTextAt (CURRENT_LN, 0, (WRAP|LEFT), USE_RT_EDGE, szTemp);
         }
      fclose (pf);
      }   
   ItiPrtIncCurrentLn();
   }


static void DoFaxHeader (PSZ pszFrom)
   {
   ItiPrtIncCurrentLn();

   ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE, "FAX TRANSMITTAL");
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE,
                    "  TO: INFO TECH at (352) 373-9586, attn: BAMS/CES Support");
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "FROM: ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, pszFrom);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn (); //blank line 
   }



void DoUserInfo (PSZ pszReqUser, 
                 PSZ pszReqID, 
                 PSZ pszReqReal, 
                 PSZ pszReqType, 
                 PSZ pszReqVoice, 
                 PSZ pszReqPri, 
                 PSZ pszReqFax, 
                 PSZ pszLog)
   {
   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "User Name: ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, pszReqUser);
   ItiPrtPutTextAt (CURRENT_LN, 44, LEFT, USE_RT_EDGE,"Request ID  : ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, pszReqID);
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Real Name: ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, pszReqReal);
   ItiPrtPutTextAt (CURRENT_LN, 44, LEFT, USE_RT_EDGE,"Request Type: ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, pszReqType);
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Voice #  : ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, pszReqVoice);
   ItiPrtPutTextAt (CURRENT_LN, 44, LEFT, USE_RT_EDGE,"Priority    : ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, pszReqPri);
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Fax #    : ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, pszReqFax);
   ItiPrtPutTextAt (CURRENT_LN, 44, LEFT, USE_RT_EDGE,"Log File    : ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, pszLog);
   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();
   }

static void DoDatabaseRelease (HHEAP hheap)
   {
   HQRY     hqry;
   PPSZ     ppsz;
   USHORT   usNumCols, usState;

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Database Release History: ");
   ItiPrtIncCurrentLn ();

   hqry = ItiDbExecQuery (" SELECT Release, Date, Comment"
                          " FROM DatabaseRelease"
                          " ORDER BY Release DESC",
                          hheap, hmodMe, ITIRID_RPT, VERSION_FORMAT,
                          &usNumCols, &usState);
   if (hqry == NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "No database release history!");
      ItiPrtIncCurrentLn ();
      }
   else
      while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
         {
         ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, ppsz [0]);
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, ", ");
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, ppsz [1]);
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, ", ");
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, ppsz [2]);
         ItiPrtIncCurrentLn ();
         ItiMemFree (hheap, ppsz);
         }
   }


USHORT EXPORT ItiRptDLLPrintReport (HAB   hab,
                                    HMQ   hmq,
                                    INT   argcnt,
                                    CHAR  *argvars[])
   {
   HHEAP       hheap;
   USHORT      us = 0;
   USHORT      usCurrentPg = 0;
   FILE        *fpR;
   CHAR        chr;
   CHAR        szRptFile [512];
   REPTITLES   rept;
   CHAR        szPid[128] = "";
   CHAR        szCmd[1023]= "";
   USHORT      uSel, uOffset;
   PSZ         pszEnv;
   PSZ         pszTmp;
   PSZ         pszTmpLn;

   CHAR  szReqID   [128] = "";
   CHAR  szReqType [128] = "";
   CHAR  szReqPri  [128] = "";
   CHAR  szReqUser [128] = "";
   CHAR  szReqReal [128] = "";
   CHAR  szReqVoice[128] = "";
   CHAR  szReqFax  [128] = "";
   CHAR  szLog  [256] = "";
                   
   CHAR  szSD [128] = "";
   CHAR  szLD [20480] = "";

   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);

   ItiRptUtGetOption (OPT_PID, szPid, sizeof szPid, argcnt, argvars);
   ItiRptUtGetOption (OPT_CMD, szCmd, sizeof szCmd, argcnt, argvars);
   ItiRptUtGetOption (OPT_REQID,  szReqID,  sizeof szReqID,  argcnt, argvars);
   ItiRptUtGetOption (OPT_REQTYPE,szReqType,sizeof szReqType,argcnt, argvars);
   ItiRptUtGetOption (OPT_REQPRI, szReqPri, sizeof szReqPri, argcnt, argvars);

   ItiRptUtGetOption (OPT_REQUSER, szReqUser, sizeof szReqUser, argcnt, argvars);
   ItiRptUtGetOption (OPT_REQREAL, szReqReal, sizeof szReqReal, argcnt, argvars);
   ItiRptUtGetOption (OPT_REQVOICE,szReqVoice,sizeof szReqVoice,argcnt, argvars);
   ItiRptUtGetOption (OPT_REQFAX,  szReqFax,  sizeof szReqFax,  argcnt, argvars);
   ItiRptUtGetOption (OPT_LOG,  szLog,  sizeof szLog,  argcnt, argvars);

   ItiRptUtGetOption (OPT_SD,  szSD,  sizeof szSD,  argcnt, argvars);

   /* -- Get name of the file holding the long description. */
   ItiRptUtGetOption (OPT_LD,  szRptFile,  sizeof szRptFile,  argcnt, argvars);

   if (!(fpR = fopen (szRptFile, "r")))
      return 13;
   else
      {
      us = 0;
      chr = (CHAR)getc(fpR);
      while ((chr != (CHAR)EOF) && (us < sizeof szLD))
         {
         szLD[us++] = chr;
         chr = (CHAR)getc(fpR);
         }

      fclose (fpR);
      }

   SetReportTitles (&rept);

   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */
   us =  ItiPrtLoadDLLInfo(hab, hmq, &rept, argcnt, argvars);
   if (us != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return (ITIPRT_NOT_INITIALIZED);
      }

   ItiPrtBeginReport("SupportRequest");


   /* -- Start first page. */
   usCurrentPg = ItiPrtQueryCurrentPgNum();
   ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

   DoFaxHeader (szReqReal);

   /* -- User and request IDs. */
   DoUserInfo (szReqUser, szReqID, szReqReal, szReqType, szReqVoice, 
               szReqPri, szReqFax, szLog);


   /* -- Description IDs. */
   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Topic:  ");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szSD);
   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn (); //blank line 


   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, "Request Description: ");
   ItiPrtIncCurrentLn ();

   pszTmp = szLD;
   pszTmpLn = szLD;
   while ((pszTmp != NULL) && (*pszTmp != '\0'))
      {
      if (*pszTmp == '\n')
         {
         *pszTmp = '\0';
         ItiPrtPutTextAt (CURRENT_LN, 3, (WRAP|LEFT), USE_RT_EDGE, pszTmpLn);
         ItiPrtMoveTo(CURRENT_LN, 0); //Reset to start of the line.
         pszTmp++;                    //Move past the newline character,
         pszTmpLn = pszTmp;           //and start a new string.
         }
      else
         {
         pszTmp++;
         }
      }

   ItiPrtIncCurrentLn ();

   /* -- SYSTEM INFO */
   ItiPrtNewPg();
   usCurrentPg = ItiPrtQueryCurrentPgNum();
   ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

   ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE,"System Information");
   ItiPrtIncCurrentLn();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, szPid);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, USE_RT_EDGE, szCmd);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn ();
   DoDriveSpace ();
   ItiPrtIncCurrentLn ();
   DoDatabaseRelease (hheap);
   ItiPrtIncCurrentLn ();
   DoConfigDump ();

   ItiPrtIncCurrentLn (); //blank line 

   /* -- Get the environment vars. */
   DosGetEnv (&uSel, &uOffset);

   pszEnv = MAKEP (uSel, 0);
   while (*pszEnv)
      {
      if (ItiPrtKeepNLns(3) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }
      ItiPrtPutTextAt (CURRENT_LN, 0, (LEFT|WRAP), USE_RT_EDGE, pszEnv);
//      ItiPrtIncCurrentLn();
      pszEnv = strchr (pszEnv, '\0');
      pszEnv++;
      }

   DumpCRC ();

   DumpLog (szLog);

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
