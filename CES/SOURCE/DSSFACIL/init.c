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
#include <stdlib.h>
#include "..\include\iti.h"
#include "dssfacil.h"
#include "init.h"
#include "dialog.h"
#include "..\include\message.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\winid.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"

/* define global variables */
PGLOBALS pglobals = NULL;



/* define module specific globals */
#ifdef DEBUG
char szAppName [] = "BAMS/DSS Facility Browser  [Development Version]";
#else
char szAppName [] = "BAMS/DSS Facility Browser";
#endif

#define NUM_DATA_BYTES  (1 * sizeof (void far *))

#define LOGGED_IN       1
#define CANCEL          2
#define NOT_AUTHORIZED  3



MRESULT EXPENTRY LoginUserProc (HWND    hwnd,
                                USHORT   uMsg,
                                MPARAM   mp1,
                                MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpApplication;
         SWP   swpLoading;
         HWND  hwndLoading;

         hwndLoading = HWNDFROMMP (mp2);

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);
         WinQueryWindowPos (hwndLoading, &swpLoading);

         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpLoading.y -
                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYMENU]);

         swpMe.x = max (0, swpMe.x);
         swpMe.y = max (0, swpMe.y);

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

      case WM_COMMAND:
         switch (COMMANDMSG (&uMsg)->cmd)
            {
            case DID_OK:
               WinDismissDlg (hwnd, LOGGED_IN);
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, CANCEL);
               return 0;
            }
         break;
      }
   return WinDefDlgProc (hwnd, uMsg, mp1, mp2);
   }







void MBExit (PSZ pszStr)
   {
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, 
                              pszStr, "BAMS/CES Init", 0, MB_OK);
   DosExit (EXIT_PROCESS, 0);
   }



BOOL RegisterWindows (void);

void Initialize (void)
   {
   USHORT   usLoggedIn;
   HAB      habMainThread;
   HMQ      hmqMainThread;
   ULONG    flStyle;
   HPOINTER hptr;

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
      ItiErrDisplayWindowError (habMainThread, HWND_DESKTOP, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   hmqMainThread = WinCreateMsgQueue (habMainThread, 1000);

   if (hmqMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, HWND_DESKTOP, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Initialize the Globals module. DO NOT MOVE THIS CODE TO BE
      ANY LATER THAN THIS LINE!!! OR ALL HELL WILL BREAK LOOSE!!! */
   pglobals = ItiGlobInitGlobals (habMainThread, hmqMainThread, 
                                  HWND_DESKTOP, HWND_DESKTOP, szAppName, "DSSFacil",
                                  VERSION_STRING);


   if (pglobals == NULL)
      DosExit (EXIT_PROCESS, 0);

   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;
   pglobals->ucVersionChar = VERSION_CHAR;

   /* set the multilingual character set */
   if (!WinSetCp (hmqMainThread, 850))
      {
      ItiErrWriteDebugMessage ("Could not set code page 850.");
      WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, 
                     "Could not set code page 850.  See your OS/2 "
                     "documentation regarding the CODEPAGE and DEVINFO "
                     "lines in the CONFIG.SYS file.  "
                     "This error will only affect the appearance of "
                     "certain characters on the screen.", 
                     szAppName, 0, 
                     MB_OK | MB_ICONHAND | MB_MOVEABLE);
      }

   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE);
   WinSetPointer (HWND_DESKTOP, hptr);
   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE);

   /*
    * Do time consuming initiailzation here
    */

   if (!ItiFmtInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The format module could not be initialized.");
      }

   if (!ItiDbInit (1, 1))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The data base module could not be initialized.");
      }

   if (ItiWndRegister (habMainThread))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The list window module could not be initialized.");
      }

   if (!RegisterWindows ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("Could not register windows!");
      }

   /*--- Log user in ---*/
   usLoggedIn = NOT_AUTHORIZED;

//   for (i = 0; i < 3; i++)
//      {
//      usLoggedIn = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppFrame,
//                              LoginUserProc, 0, IDD_LOGIN, hwndLoading);
//      if (usLoggedIn == CANCEL ||
//          usLoggedIn == LOGGED_IN)
//         break;
//      }
//
//   if (usLoggedIn == NOT_AUTHORIZED ||
//       usLoggedIn == CANCEL)
//      {
//      /* user failed to logon */
//      if (usLoggedIn == NOT_AUTHORIZED)
//             MbExit ("You are not authorizes to run BAMS/CES");
//      }


   WinSetPointer (HWND_DESKTOP, hptr);
   }


void Deinitialize (void)
   {
   ItiWndTerminate ();
   WinDestroyMsgQueue (pglobals->hmqMainMessageQueue); 
   WinTerminate (pglobals->habMainThread);
   }


BOOL RegisterWindows (void)
   {
   if (ItiMbRegisterStaticWnd (FacilityBrowserResultsS, FacilityBrowserResultsSProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityBrowserCountyL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityBrowserMaterialL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityBrowserVendorL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityBrowserResultsL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (FacilityBrowserD, FacilityBrowserD, FacilityBrowserDProc, 0))
      return FALSE;
   return TRUE;
   }
