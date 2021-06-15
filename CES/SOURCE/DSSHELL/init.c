/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * Init.c
 * Mark Hampton
 */


#define INCL_WIN
#define INCL_DOS
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\dialog.h"
#include "DSSHELL.h"
#include "init.h"
#include "dialog.h"
#include "..\include\message.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "help.h"
#include "menu.h"
#include "..\include\winid.h"
#include "..\include\itiperm.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
// #include "..\include\itiest.h"

/* define global variables */
PGLOBALS pglobals = NULL;
HBUF     hbufSIC  = NULL;
CHAR szCurSpecYear[8];



/* define module specific globals */
//#ifdef DEBUG
//char szAppName [] = "DS/Shell  [Development Release]";
//#else
char szAppName [] = "DS/Shell Browser";
//#endif

#define NUM_DATA_BYTES  (1 * sizeof (void far *))


MRESULT EXPENTRY LoadingProc (HWND     hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpApplication;

#ifdef PRERELEASE
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, PRERELEASE);
#else
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, "");
#endif
         ItiStrPrintfDlgItem (hwnd, DID_COPY1, COPYRIGHT);
         ItiStrPrintfDlgItem (hwnd, DID_COPY2, COPYRIGHT);

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);

         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpApplication.y + swpApplication.cy -
                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYTITLEBAR] +
                    (SHORT) pglobals->alSysValues [SV_CYMENU] * 2);

         if (swpMe.x < 0)
            swpMe.x = 0;

         if (swpMe.y < 0)
            swpMe.y = 0;
         
         if (swpMe.x + swpMe.cx > (SHORT) pglobals->alSysValues [SV_CXSCREEN])
            swpMe.x -= (swpMe.x + swpMe.cx) -
                       (SHORT) pglobals->alSysValues [SV_CXSCREEN];

         if (swpMe.y + swpMe.cy > (SHORT) pglobals->alSysValues [SV_CYSCREEN])
            swpMe.y -= (swpMe.y + swpMe.cy) -
                       (SHORT) pglobals->alSysValues [SV_CYSCREEN];

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         }
         return 0;

      case WM_CLOSE:
         WinDismissDlg (hwnd, TRUE);
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


//MRESULT EXPENTRY LoadingProc (HWND     hwnd,
//                              USHORT   usMessage,
//                              MPARAM   mp1,
//                              MPARAM   mp2)
//   {
//   switch (usMessage)
//      {
//      case WM_INITDLG:
//         {
//         SWP   swpMe;
//         SWP   swpApplication;
//
//#ifdef PRERELEASE
//         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, PRERELEASE);
//#else
//         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, "");
//#endif
//         ItiStrPrintfDlgItem (hwnd, DID_COPY1, COPYRIGHT);
//         ItiStrPrintfDlgItem (hwnd, DID_COPY2, COPYRIGHT);
//
//         /* position the dialog box */
//         WinQueryWindowPos (hwnd, &swpMe);
//         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);
//
//         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
//         swpMe.y = swpApplication.y + swpApplication.cy -
//                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYTITLEBAR] +
//                    (SHORT) pglobals->alSysValues [SV_CYMENU] * 2);
//
//         if (swpMe.x < 0)
//            swpMe.x = 0;
//
//         if (swpMe.y < 0)
//            swpMe.y = 0;
//         
//         if (swpMe.x + swpMe.cx > (SHORT) pglobals->alSysValues [SV_CXSCREEN])
//            swpMe.x -= (swpMe.x + swpMe.cx) -
//                       (SHORT) pglobals->alSysValues [SV_CXSCREEN];
//
//         if (swpMe.y + swpMe.cy > (SHORT) pglobals->alSysValues [SV_CYSCREEN])
//            swpMe.y -= (swpMe.y + swpMe.cy) -
//                       (SHORT) pglobals->alSysValues [SV_CYSCREEN];
//
//         swpMe.fs = SWP_MOVE;
//         WinSetMultWindowPos (hwnd, &swpMe, 1);
//         }
//         return 0;
//
//      case WM_CLOSE:
//         WinDismissDlg (hwnd, TRUE);
//         break;
//      }
//   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
//   }







HWND PreloadBuffer (USHORT uWinId)
   {
   USHORT   uJunk;
   HMODULE  hmod;
   CHAR     szQuery [1024];

   ItiMbQueryHMOD (uWinId, &hmod);
   ItiMbQueryQueryText (hmod, ITIRID_WND, uWinId, szQuery, sizeof szQuery);
   return ItiDbCreateBuffer (NULL, uWinId, ITI_LIST, szQuery, &uJunk);
   }




void MBExit (PSZ pszStr)
   {
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, 
                              pszStr, "DS/Shell Init", 0, MB_OK);
   DosExit (EXIT_PROCESS, 0);
   }



BOOL RegisterWindows (void);

void Initialize (void)
   {
   HWND     hwndLoading;
   HAB      habMainThread;
   HMQ      hmqMainThread;
   HWND     hwndAppFrame;
   HWND     hwndAppClient;
   HWND     hwndMainMenu;
   HELPINIT hinit;
   ULONG    flStyle;
   USHORT   y, cx, cy;
   HPOINTER hptr;
   HHEAP    hhpInit = NULL;
   PSZ      pszUS = NULL;
   CHAR     szQ[256] = "";
   CHAR     szQry[256] = "";
   CHAR     szIQry[256] = "";


   flStyle = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST |
             FCF_MENU | FCF_ACCELTABLE | FCF_ICON;

   /*
    * Initialize the thread for making Presentation Manager calls and
    * create the message queue.
    */

   habMainThread = WinInitialize (0);
   if (habMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   hmqMainThread = WinCreateMsgQueue (habMainThread, 1000);

   if (hmqMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Register the main window class, terminate on failure. */
   if (!WinRegisterClass (habMainThread, szAppName,
                          MainWindowProc, 0, NUM_DATA_BYTES))
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Create the main window, terminate on failure. */
   hwndAppFrame = WinCreateStdWindow (HWND_DESKTOP, 
                                      FS_NOBYTEALIGN,
                                      &flStyle, szAppName, NULL, 
                                      0L, 0, ID_RESOURCE, &hwndAppClient);

   if (hwndAppFrame == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }
   y  = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYICON) * 2;
   cx = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXFULLSCREEN);
   cy = (USHORT) (WinQuerySysValue (HWND_DESKTOP, SV_CYFULLSCREEN) - y +
                  WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR));
   WinSetWindowPos (hwndAppFrame, HWND_TOP, 0, y, cx, cy, 
                    SWP_MOVE | SWP_SIZE | SWP_SHOW);

   /* Initialize the Globals module. DO NOT MOVE THIS CODE TO BE
      ANY LATER THAN THIS LINE!!! OR ALL HELL WILL BREAK LOOSE!!! */
   pglobals = ItiGlobInitGlobals (habMainThread, hmqMainThread, hwndAppFrame,
                                  hwndAppClient, szAppName, "DS/Shell",
                                  VERSION_STRING);

   if (pglobals == NULL)
      {
      DosExit (EXIT_PROCESS, 0);
      }
   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;
   pglobals->ucVersionChar = VERSION_CHAR;

   if (!ItiWndSetMDIApp (hwndAppFrame))
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* set the correct window title */
   WinSetWindowText (hwndAppFrame, szAppName);

   /* set the multilingual character set */
   if (!WinSetCp (hmqMainThread, 850))
      {
      ItiErrWriteDebugMessage ("Could not set code page 850.");
      WinMessageBox (HWND_DESKTOP, hwndAppFrame, 
                     "Could not set code page 850.  See your OS/2 "
                     "documentation regarding the CODEPAGE and DEVINFO "
                     "lines in the CONFIG.SYS file.  "
                     "This error will only affect the appearance of "
                     "certain characters on the screen.", 
                     szAppName, 0, 
                     MB_OK | MB_ICONHAND | MB_MOVEABLE);
      }

//   /* create and display the initial loading dialog box */
//   hwndLoading = WinLoadDlg (pglobals->hwndDesktop, hwndAppFrame,
//                             LoadingProc, 0, IDD_INITIAL, NULL);
//
//   if (hwndLoading == NULL)
//      {
//      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
////      DosExit (EXIT_PROCESS, 0);
//      }


   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE);
   WinSetPointer (HWND_DESKTOP, hptr);
   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE);

   WinUpdateWindow (hwndAppClient);
   
   /*
    * Do time consuming initiailzation here
    */

   if (!ItiFmtInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The format module could not be initialized.");
      }

   if (!ItiDbInit (2, 3))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The data base module could not be initialized.");
      }

   if (ItiWndRegister (habMainThread))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The list window module could not be initialized.");
      }
      
   hwndMainMenu = WinWindowFromID (hwndAppFrame, FID_MENU);

#ifdef DEBUG
   if (hwndMainMenu == NULL)
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The Main Menu could not be found!");
      }
#endif 

// --------------------------------------------- Metric System Changes
                    /* Note this section comes AFTER the ItiDbInit! */
   hhpInit = ItiMemCreateHeap (1024);

   /* -- Set the metric flag. */
   sprintf (szQ,
           "SELECT UnitType FROM UnitTypeSystem WHERE UserKey = %s "
           ,pglobals->pszUserKey); 
   pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
   if ( (pszUS != NULL) && ((*pszUS) == '1') )
      pglobals->bUseMetric = TRUE;
   else
      pglobals->bUseMetric = FALSE;

   /* -- Set spec year stuff. */
   ItiDbExecSQLStatement (hhpInit, szIQry);

   sprintf (szIQry,
       " /* bamsces.init.c.Initialize */ "
       " IF NOT EXISTS (SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s ) "
       " INSERT INTO CurrentSpecYear (SpecYear, UserKey) "
       " SELECT MAX(SpecYear), %s FROM StandardItem "
       , pglobals->pszUserKey,
       pglobals->pszUserKey);
   ItiDbExecSQLStatement (hhpInit, szIQry);

   sprintf (szQ,
           "SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s "
           ,pglobals->pszUserKey); 
   pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
   if ( (pszUS != NULL) && ((*pszUS) != '\0') && ((*pszUS) != '0') )
      {
      ItiStrCpy(&szCurSpecYear[0], pszUS, sizeof szCurSpecYear);
      }
   else
      {
      ItiStrCpy(&szCurSpecYear[0], "1900", sizeof szCurSpecYear);
      }

   pglobals->pszCurrentSpecYear = &szCurSpecYear[0];
 //  if (hhpInit != NULL)
 //     ItiMemDestroyHeap (hhpInit);

   // /* -- Set the correct window title. */
   // SetSpecYrTitle();
// -------------------------------------------------------------------


   if (!ItiMenuInitialize (hwndMainMenu))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The ITIMENU module could not be initialized! "
               "Make sure that the tables Application, "
               "ApplicationCatalog, ApplicationReport, and "
               "Report have not been corrupted.");
      }



   /* --- PRELOAD  SOME BUFFERS --- */
   hbufSIC = PreloadBuffer (StandardItemCatL);

   /* initialize help system */
   hinit.cb = sizeof hinit;
   hinit.ulReturnCode             = 0L;
   hinit.pszTutorialName          = NULL;
   hinit.phtHelpTable             = MAKEP (0xFFFF, HT_HELPTABLE);
   hinit.hmodHelpTableModule      = 0;
   hinit.hmodAccelActionBarModule = 0;
   hinit.idAccelTable             = DSSHELL_ACCELERATORS;
   hinit.idActionBar              = DSSHELL_MAIN_MENU;
   hinit.pszHelpWindowTitle       = "DS/Shell Help",
   hinit.usShowPanelId            = CMIC_HIDE_PANEL_ID;
   hinit.pszHelpLibraryName       = "dsshell.hlp";

   pglobals->hwndHelp = WinCreateHelpInstance (habMainThread, &hinit);

   if (pglobals->hwndHelp == NULL || hinit.ulReturnCode != 0)
      {
      WinSetPointer (HWND_DESKTOP, hptr);
//      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);  BSR# TEMP_FIX
      }
   else
      {
      WinAssociateHelpInstance (pglobals->hwndHelp, hwndAppClient);
      }

   /*--- Log user in ---*/
   if (!ItiPermLogin (hwndLoading))
      DosExit (EXIT_PROCESS, 0);

// --------------------------------------------- Metric System Changes
                    /* Note this section comes AFTER the ItiDbInit! */
   // hhpInit = ItiMemCreateHeap (1024);

   sprintf (szQry,
       " /* bamsces.init.c.Initialize */ "
       "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
           " SELECT 'User already initialized.' "
       " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (0, %s) ",
       pglobals->pszUserKey,
       pglobals->pszUserKey);
   ItiDbExecSQLStatement (hhpInit, szQry);

 //    /* -- Set the metric flag. */
 //    sprintf (szQ,
 //            "SELECT UnitType FROM UnitTypeSystem WHERE UserKey = %s "
 //            ,pglobals->pszUserKey); 
 //    pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
 //    if ( (pszUS != NULL) && ((*pszUS) == '1') )
 //       pglobals->bUseMetric = TRUE;
 //    else
 //       pglobals->bUseMetric = FALSE;
 //  
 //    /* -- Set spec year stuff. */
 //    ItiDbExecSQLStatement (hhpInit, szIQry);
 //  
 //    sprintf (szIQry,
 //        " /* bamsces.init.c.Initialize */ "
 //        " IF NOT EXISTS (SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s ) "
 //        " INSERT INTO CurrentSpecYear (SpecYear, UserKey) "
 //        " SELECT MAX(SpecYear), %s FROM StandardItem "
 //        , pglobals->pszUserKey,
 //        pglobals->pszUserKey);
 //    ItiDbExecSQLStatement (hhpInit, szIQry);
 //  
 //    sprintf (szQ,
 //            "SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s "
 //            ,pglobals->pszUserKey); 
 //    pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
 //    if ( (pszUS != NULL) && ((*pszUS) != '\0') && ((*pszUS) != '0') )
 //       {
 //       ItiStrCpy(&szCurSpecYear[0], pszUS, sizeof szCurSpecYear);
 //       }
 //    else
 //       {
 //       ItiStrCpy(&szCurSpecYear[0], "1900", sizeof szCurSpecYear);
 //       }
 //  
 //    pglobals->pszCurrentSpecYear = &szCurSpecYear[0];
   if (hhpInit != NULL)
      ItiMemDestroyHeap (hhpInit);

   // /* -- Set the correct window title. */
   // SetSpecYrTitle();
// -------------------------------------------------------------------



   WinSetPointer (HWND_DESKTOP, hptr);
   WinDestroyWindow (hwndLoading);
   }


void Deinitialize (void)
   {
   WinDestroyWindow (pglobals->hwndAppFrame); 
   WinDestroyMsgQueue (pglobals->hmqMainMessageQueue); 
   ItiDbDeleteBuffer (hbufSIC, NULL);
   ItiWndTerminate ();
   ItiDbTerminate ();
   WinTerminate (pglobals->habMainThread);
   }

