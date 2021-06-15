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
 *
 * BAMS/DSS initialization code
 */


#define INCL_WIN
#define INCL_DOS
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "bamsdss.h"
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
#include "..\include\dialog.h"
#include "open.h"

#include "..\include\itiperm.h"

/* include references to proposal window procedures */

#include "proposal.h"
#include "bidder.h"
#include "project.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"

#include "specyear.h"

/* define global variables */
PGLOBALS pglobals = NULL;
HBUF     hbufSIC  = NULL;
CHAR szCurSpecYear[8];



char szAppName [] = "Data Browser (BAMS/DSS)";

#define NUM_DATA_BYTES  (1 * sizeof (void far *))

#define LOGGED_IN       1
#define CANCEL          2
#define NOT_AUTHORIZED  3

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
                              pszStr, "BAMS/DSS Init", 0, MB_OK);
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
   CHAR     szQ[250] = "";
   CHAR     szQry[250] = "";
   CHAR     szIQry[250] = "";
   PSZ   pszMetricYear = NULL;
   PSZ   pszEnglishYear = NULL;

   extern CHAR  szMetricYear[];
   extern CHAR  szEnglishYear[];
   extern BOOL bEnglishYearGiven, bMetricYearGiven;


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
                               hwndAppClient, szAppName, "BAMS/DSS",
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

   /* create and display the initial loading dialog box */
   hwndLoading = WinLoadDlg (pglobals->hwndDesktop, hwndAppFrame,
                             LoadingProc, 0, IDD_INITIAL, NULL);

   if (hwndLoading == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
//      DosExit (EXIT_PROCESS, 0);
      }


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

// -------------------------------------------------------------------


   if (!ItiMenuInitialize (hwndMainMenu))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The ITIMENU module could not be initialized! "
               "Make sure that the tables Application, "
               "ApplicationCatalog, ApplicationReport, and "
               "Report have not been corrupted.");
      }

   if (!ItiEstInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The ITIEST module could not be initialized! ");
      }

   if (!RegisterWindows ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("Could not register windows!");
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
   hinit.idAccelTable             = BAMSDSS_ACCELERATORS;
   hinit.idActionBar              = BAMSDSS_MAIN_MENU;
   hinit.pszHelpWindowTitle       = "Data Browser Help",
   hinit.usShowPanelId            = CMIC_HIDE_PANEL_ID;
   hinit.pszHelpLibraryName       = "BAMSCES.hlp";

   pglobals->hwndHelp = WinCreateHelpInstance (habMainThread, &hinit);

   if (pglobals->hwndHelp == NULL || hinit.ulReturnCode != 0)
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      }
   else
      {
      WinAssociateHelpInstance (pglobals->hwndHelp, hwndAppClient);
      }

   /*--- Log user in ---*/
   if (!ItiPermLogin (hwndLoading))
      DosExit (EXIT_PROCESS, 0);

   /* -- SpecYear settings. */
   DosScanEnv("ENGLISHSPECYR", &pszEnglishYear);
   if (pszEnglishYear != NULL)
      {
      ItiStrCpy(&szEnglishYear[0], pszEnglishYear, YEARLEN);
      if ( ItiCharIsDigit(szEnglishYear[0]) 
          && ItiCharIsDigit(szEnglishYear[1])
          && ItiCharIsDigit(szEnglishYear[2])
          && ItiCharIsDigit(szEnglishYear[3])
         )
         {
         /* -- Do further validation here. */
         bEnglishYearGiven = TRUE;
         ItiErrWriteDebugMessage (szEnglishYear);
         }
      else
         {
         bEnglishYearGiven = FALSE;
         ItiErrWriteDebugMessage (" NO EnglishSpecYr environment variable specified.");
         }
      } /* end if (pszEnglishYear... */
   else
      bEnglishYearGiven = FALSE;

   DosScanEnv("METRICSPECYR", &pszMetricYear);
   if (pszMetricYear != NULL)
      {
      ItiStrCpy(&szMetricYear[0], pszMetricYear, YEARLEN);
      if ( ItiCharIsDigit(szMetricYear[0]) 
          && ItiCharIsDigit(szMetricYear[1])
          && ItiCharIsDigit(szMetricYear[2])
          && ItiCharIsDigit(szMetricYear[3])
         )
         {
         /* -- Do further validation here. */
         bMetricYearGiven = TRUE;
         ItiErrWriteDebugMessage (szMetricYear);
         }
      else
         {
         bMetricYearGiven = FALSE;
         ItiErrWriteDebugMessage (" NO MetricSpecYr environment variable specified.");
         }
      } /* end if (pszMetricYear... */
   else
      bMetricYearGiven = FALSE;



// --------------------------------------------- Metric System Changes
                    /* Note this section comes AFTER the ItiDbInit! */
   hhpInit = ItiMemCreateHeap (1024);

   sprintf (szQry,
       " /* bamsdss.init.c.Initialize */ "
       "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
           " SELECT 'User already initialized.' "
       " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (0, %s) ",
       pglobals->pszUserKey,
       pglobals->pszUserKey);
   ItiDbExecSQLStatement (hhpInit, szQry);

   /* -- Set the metric flag. */
   sprintf (szQ,
           " /* bamsdss.init.c.Initialize */ "
           "SELECT UnitType FROM UnitTypeSystem WHERE UserKey = %s "
           ,pglobals->pszUserKey); 
   pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
   if ( (pszUS != NULL) && ((*pszUS) == '1') )
      pglobals->bUseMetric = TRUE;
   else
      pglobals->bUseMetric = FALSE;
 
   /* -- Set spec year stuff. */
   sprintf (szIQry,
       " /* bamsdss.init.c.Initialize */ "
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

   if (hhpInit != NULL)
      ItiMemDestroyHeap (hhpInit);

    /* -- Set the correct window title. */
    SetSpecYrTitle();
// -------------------------------------------------------------------



   WinSetPointer (HWND_DESKTOP, hptr);
   WinDestroyWindow (hwndLoading);
   }


void Deinitialize (void)
   {
   ItiWndTerminate ();
   WinDestroyWindow (pglobals->hwndAppFrame); 
   WinDestroyMsgQueue (pglobals->hmqMainMessageQueue); 
   WinTerminate (pglobals->habMainThread);


   /*--- here we destroy windows we auto initialized at startup ---*/
   ItiDbDeleteBuffer (hbufSIC, NULL);
   }


BOOL RegisterWindows (void)
   {
   /* register static windows */

   if (ItiMbRegisterStaticWnd (ProposalS, ProposalSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (BidderS, BidderSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProjectS, ProjectSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProposalMajItemS, ProposalMajItemSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CategoryS, CategorySProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProjectItemS, ProjectItemSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProposalItemS, ProposalItemSProc, 0))
      return FALSE;






   /* register list windows */

   if (ItiMbRegisterListWnd (BidderL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProjectL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProposalMajItemL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProjectItemL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProposalItemL, ItiWndDefListWndTxtProc, 0))
      return FALSE;





   /* register dialog boxes */

   if (ItiMbRegisterDialogWnd (OpenProposalD, OpenProposalD, OpenProposalDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (OpenProjectD, OpenProjectD, OpenProjectDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd (ProposalD, ProposalD, ProposalDProc, 0))
      return FALSE;


   return TRUE;
   }
