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
 * ItiGlob.c
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 * This module maintains the process-wide global variables for DS/Shell
 */

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itiutil.h"
#include <stdio.h>

static char szModuleName [] = "ItiGlob.dll";
static CHAR szDllVersion[] = "1.1b4 ItiGlob.DLL";

/*
 * Define the global varaibles
 */

GLOBALS globals =
   {
   /* initialize the system values */
      { 
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
      },

   /* zero out the character info */
   0, 0,

   /* zero out the application data */
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

   /* null out the user info */
   NULL, NULL, NULL, NULL, 0,

   /* version */
   NULL, 0, 0, '\0',

   /* Use Metric flag */
   FALSE,

   /* Current spec year */
   NULL,

   /* Test Import, run but do NOT alter the production DB. */
   FALSE
   };

BOOL bInitGlobCalled = FALSE;


PGLOBALS EXPORT ItiGlobInitGlobals (HAB   hab,
                                    HMQ   hmqMainMessageQueue,
                                    HWND  hwndAppFrame,
                                    HWND  hwndAppClient,
                                    PSZ   pszAppName,
                                    PSZ   pszApplicationID,
                                    PSZ   pszVersion)
   {
   FONTMETRICS fm;
   HPS         hps;
   SEL         selGlobal;
   SEL         selLocal;
   POINTL      aptl [TXTBOX_COUNT];
   CHAR        szTemp [1024] = "";

   LONG        lnum, lTemp = 0L;
   FATTRS      fat;
   FONTMETRICS fm2;
   PSZ  pszTestEnv = NULL;
   CHAR szReset[100] = "    DLL Version loaded: ";


   if (bInitGlobCalled)
      return &globals;

   /* --------------------------------------------------------- */
   ItiStrCat (szReset, szDllVersion, sizeof szReset);
   ItiErrWriteDebugMessage (szReset);



   /* initialize the application constants */
   globals.habMainThread = hab;
   globals.hwndAppFrame = hwndAppFrame;
   globals.hwndAppClient = hwndAppClient;
   globals.hmqMainMessageQueue = hmqMainMessageQueue;
   globals.pszAppName = pszAppName;
   globals.pszApplicationID = pszApplicationID;

   globals.hwndDesktop = WinQueryDesktopWindow (hab, NULL);
   globals.hwndObject = WinQueryObjectWindow (globals.hwndDesktop);

   if (DosGetInfoSeg (&selGlobal, &selLocal))
      {
      WinMessageBox (globals.hwndDesktop,
                     hwndAppFrame ? hwndAppFrame : globals.hwndDesktop,
                     "Could not get global and local info segments!",
                     szModuleName, 0,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
      return NULL;
      }

   globals.pgis = MAKEP (selGlobal, 0);
   globals.plis = MAKEP (selLocal, 0);

   /* write app name/version to log */
   ItiStrCpy (szTemp, pszAppName, sizeof szTemp);
   ItiStrCat (szTemp, " ", sizeof szTemp);
   ItiStrCat (szTemp, pszVersion, sizeof szTemp);
   ItiErrWriteDebugMessage (szTemp);

   /* write debug junk to output file */
   sprintf (szTemp, "%.2d/%.2d/%.4d %2d:%.2d:%.2d%cm OS/2 version %d.%.2d%c", 
            globals.pgis->month, globals.pgis->day, globals.pgis->year,
            globals.pgis->hour <= 12 ? (USHORT) globals.pgis->hour : (USHORT) globals.pgis->hour - 12 ,
            (USHORT) globals.pgis->minutes, (USHORT) globals.pgis->seconds,
            globals.pgis->hour < 12 ? 'a' : 'p', 
            (int) (globals.pgis->uchMajorVersion) / 10, 
            (int) (globals.pgis->uchMinorVersion), 
            globals.pgis->chRevisionLetter);
   ItiErrWriteDebugMessage (szTemp);

#if defined (DEBUG)
   sprintf (szTemp, "%s, %s Scheduling, Max Wait = %d seconds",
            globals.pgis->fProtectModeOnly ? "Protected Only" : "With DOS Box",
            globals.pgis->fDynamicSched ? "Dynamic" : "Static",
            globals.pgis->csecMaxWait);
   ItiErrWriteDebugMessage (szTemp);

   sprintf (szTemp, "Min Slice = %dms, Max Slice = %dms, Boot Drive = %c:",
            globals.pgis->cmsecMinSlice, globals.pgis->cmsecMaxSlice,
            (char) (globals.pgis->bootdrive - 1) + 'a');
   ItiErrWriteDebugMessage (szTemp);

   sprintf (szTemp, "Max Screen Groups = %d, Max VIO Sess. = %d, Max PM Sess. = %d",
            globals.pgis->sgMax,
            globals.pgis->csgWindowableVioMax, globals.pgis->csgPMMax);
   ItiErrWriteDebugMessage (szTemp);
#endif

   ItiGlobReinitSysValues (0, SV_RESERVEDFIRST1 - 1);
   ItiGlobReinitSysValues (SV_RESERVEDLAST1 + 1, SV_RESERVEDFIRST - 1);
   ItiGlobReinitSysValues (SV_RESERVEDLAST + 1, SV_CSYSVALUES - 1);

   /* initialize the character info */
   if (hwndAppClient != NULL)
      {
      hps = WinGetPS (hwndAppClient);

      if (hps == NULL)
         {
         ItiErrDisplayWindowError (hab, hwndAppFrame, 0);
         return NULL;
         }

      /* =============== Set Font to "System", 1995 April \/ ========== */
   //   lnum = GpiQueryFonts
   //      (hps, QF_PUBLIC, "System", &lTemp, (LONG) sizeof (fm2), &fm2);
   //   ItiStrCpy (fat.szFacename, "System", sizeof (fat.szFacename) );
   //   fat.usRecordLength = sizeof(FATTRS);
   //   fat.fsSelection = fm2.fsSelection ;
   //   fat.lMatch = fm2.lMatch ;
   //   fat.idRegistry = fm2.idRegistry ;
   //   fat.usCodePage = fm2.usCodePage ;
   //   fat.lMaxBaselineExt = fm2.lMaxBaselineExt;
   //   fat.lAveCharWidth = fm2.lAveCharWidth;
   //   fat.fsType = fm2.fsType;
   //   fat.fsFontUse = 0;
   //
   //   GpiCreateLogFont(hps, NULL, lnum, &fat);
   //   GpiSetCharSet(hps, lnum);
   //   fm = fm2;
 //     ItiErrWriteDebugMessage ("Face names:");
 //
 //     ItiErrWriteDebugMessage (fat.szFacename);
 //     ItiErrWriteDebugMessage (fm2.szFacename);
 //     ItiErrWriteDebugMessage (fm.szFacename);
 //     ItiErrWriteDebugMessage (fm2.szFamilyname);
 //     ItiErrWriteDebugMessage (" === ");

      /* =============== Set Font, 1995 April /\ ========== */
   
      if (GPI_ERROR == GpiQueryFontMetrics (hps, sizeof fm, &fm))
         {
         WinReleasePS (hps);
         ItiErrDisplayWindowError (hab, hwndAppFrame, 0);
         return NULL;
         }
   
      /* base xChar and yChar on the letter 'X' */
      GpiQueryTextBox (hps, 1, "X", TXTBOX_COUNT, aptl);
      globals.xChar = (USHORT) (aptl [TXTBOX_TOPRIGHT].x - 
                                aptl [TXTBOX_TOPLEFT].x);
      globals.yChar = (USHORT) (fm.lMaxBaselineExt);
   
      WinReleasePS (hps);
      }


   DosScanEnv ("TESTIMPORT", &pszTestEnv);
   if (pszTestEnv != NULL)
      {
      globals.bTestImportFlag = TRUE;
      }

   if (globals.bTestImportFlag)
      ItiErrWriteDebugMessage (" === ImportTest flag is initially set as TRUE. ");
   else
      ItiErrWriteDebugMessage (" === ImportTest flag is initially set as FALSE. ");
    
     

   bInitGlobCalled = TRUE;

   return &globals;
   }



PGLOBALS EXPORT ItiGlobQueryGlobalsPointer (void)
   {
   return &globals;
   }


void EXPORT ItiGlobResetImportTestFlag (BOOL bFlag)
   {
   if (bFlag)
      {
      globals.bTestImportFlag = TRUE;
      ItiErrWriteDebugMessage (" === ImportTest flag is now set as TRUE. ");
      }
   else
      {
      globals.bTestImportFlag = FALSE;
      ItiErrWriteDebugMessage (" === ImportTest flag is now set as FALSE. ");
      }

   return;
   }

BOOL EXPORT ItiGlobGetImportTestFlag (void)
   {
   return globals.bTestImportFlag;
   }


void EXPORT ItiGlobReinitSysValues (int iFirst, int iLast)
   {
   if (iFirst > SV_CSYSVALUES || iLast  >= SV_CSYSVALUES)
      {
      WinMessageBox (globals.hwndDesktop, globals.hwndAppFrame,
                     "ItiGlobReinitSysValues called with an invalid number",
                     szModuleName, 0,
                     MB_OK | MB_ICONEXCLAMATION | MB_HELP);
      }
   else
      {
      while (iFirst <= iLast)
         {
         globals.alSysValues [iFirst] = WinQuerySysValue (globals.hwndDesktop,
                                                          iFirst);
         iFirst++;
         }
      }
   }



