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
 * SQLBrow.c
 * Mark Hampton
 * Copyright (C) 1991 AASHTO
 *
 */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <stdio.h>
#include "..\include\iti.h"
//#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "SQLBrow.h"
#include "init.h"
#include "..\include\dialog.h"
#include "dialog.h"
#include "..\include\winid.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"

#include "process.h"

/* exported functions */

static char szTemp [10240];




MRESULT EXPORT DoSearch (HWND   hwnd, 
                         USHORT usMessage,
                         MPARAM mp1,
                         MPARAM mp2)
   {
   static HWND hwndMLE;

   switch (usMessage)
      {
      case WM_INITDLG:
         hwndMLE = mp2;
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MLE_SEARCHDATA mlesrch; 

               WinQueryDlgItemText (hwnd, DID_SEARCH, sizeof szTemp, szTemp);
               mlesrch.cb = sizeof mlesrch;
               mlesrch.pchFind = szTemp;
               mlesrch.pchReplace = NULL;
               mlesrch.cchFind = 0;
               mlesrch.cchReplace = 0;
               mlesrch.iptStart = -1;
               mlesrch.iptStop = -1;
               WinSendMsg (hwndMLE, MLM_SEARCH, 
                           MPFROMLONG (MLFSEARCH_SELECTMATCH), 
                           MPFROMP (&mlesrch));
               WinDismissDlg (hwnd, 1);
               }
               return 0;
               
            case DID_CANCEL:
               WinDismissDlg (hwnd, 0);
               return 0;
            }
            

      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT SQLBrowserProc (HWND   hwnd, 
                               USHORT usMessage,
                               MPARAM mp1,
                               MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         WinSendMsg (hwnd, WM_SETICON, 
                     MPFROMP (WinLoadPointer (HWND_DESKTOP, 0, 
                                              SQLBrowserDialog)), 0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_SEARCH:
               WinDlgBox (HWND_DESKTOP, hwnd, DoSearch, 0, IDD_SEARCH,
                          WinWindowFromID (hwnd, DID_RESULTS));
               return 0;
               break;

            case DID_EXECUTE:
               {
               ULONG ulStart = 0, ulCopy = 0, ulLen;

               WinSendMsg (hwnd, WM_COMMAND, MPFROMSHORT (DID_CLEARRESULTS), 0);
               WinSendDlgItemMsg (hwnd, DID_QUERY, MLM_SETIMPORTEXPORT, 
                                  MPFROMP (szTemp), 
                                  MPFROMSHORT ((USHORT) sizeof (szTemp)));
               ulCopy = (ULONG) WinSendDlgItemMsg (hwnd, DID_QUERY,
                                MLM_QUERYTEXTLENGTH, 0, 0);
               ulLen = (ULONG) WinSendDlgItemMsg (hwnd, DID_QUERY, MLM_EXPORT, 
                                  MPFROMP (&ulStart),
                                  MPFROMP (&ulCopy));
               szTemp [ulLen] = '\0';
               ProcessQuery (WinWindowFromID (hwnd, DID_RESULTS), szTemp);
               }
               return 0;
               break;

            case DID_COPYQUERY:
            case DID_COPYRESULTS:
               {
               ULONG ulAnchor, ulCursor;
               USHORT usWindowId = DID_RESULTS;

               if (SHORT1FROMMP (mp1) == DID_COPYQUERY)
                  usWindowId = DID_QUERY;

               ulAnchor =  (ULONG) WinSendDlgItemMsg (hwnd, usWindowId,
                                   MLM_QUERYSEL, 
                                   MPFROMSHORT (MLFQS_ANCHORSEL), 0);
               ulCursor =  (ULONG) WinSendDlgItemMsg (hwnd, usWindowId,
                                   MLM_QUERYSEL, 
                                   MPFROMSHORT (MLFQS_CURSORSEL), 0);

               if (ulAnchor == ulCursor)
                  WinSendDlgItemMsg (hwnd, usWindowId, MLM_SETSEL,
                                     MPFROMLONG (0), 
                                     WinSendDlgItemMsg (hwnd, usWindowId,
                                     MLM_QUERYTEXTLENGTH, 0, 0));

               WinSendDlgItemMsg (hwnd, usWindowId, MLM_COPY, 0, 0);

               if (ulAnchor == ulCursor)
                  WinSendDlgItemMsg (hwnd, usWindowId, MLM_SETSEL,
                                     MPFROMLONG (ulAnchor), 
                                     MPFROMLONG (ulCursor));
               return 0;
               }
               break;

            case DID_PASTE:
               WinSendDlgItemMsg (hwnd, DID_QUERY, MLM_PASTE, 0, 0);
               return 0;
               break;

            case DID_CLEARRESULTS:
               /* clear the results window */
               WinSendDlgItemMsg (hwnd, DID_RESULTS, MLM_DELETE,
                                  MPFROMLONG (0), 
                                  WinSendDlgItemMsg (hwnd, DID_RESULTS,
                                  MLM_QUERYTEXTLENGTH, 0, 0));
               return 0;
               break;
            }
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






#define STACK_SIZE   65534U

static QMSG    qmsg;
static PVOID   pvNewStack;
static SEL     selNewStack;
static SEL     selOldStack;
static USHORT  usOldSP;
static USHORT  usNewSP;

static int     iArgCount;
static char    **ppszArgv;


#ifdef DEBUG
static char  szBuffer [256];
#endif


int _cdecl main (int argc, char *argv [])
   {
/***********************************************/
/* DO NOT USE ANY LOCAL VARIABLES!!! WE RESET  */
/* THE STACK POINTER.  USE ONLY STATIC         */
/* VARIALBES.                                  */
/***********************************************/

   /* store old argc, argv */
   iArgCount = argc;
   ppszArgv = argv;

/************************************************/
/* DO NOT USE ARGC OR ARGV AFTER THIS POINT!!!! */
/************************************************/
   pvNewStack = ItiMemAllocSeg (STACK_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pvNewStack != NULL)
      {
      selNewStack = SELECTOROF (pvNewStack);

      /* calc new stack pointer, and force it to be on an even boundary */
      usNewSP = STACK_SIZE - (sizeof (USHORT) + STACK_SIZE % 2);

      _asm 
         {
         mov selOldStack, ss     /* save old stack segment */
         mov ss,selNewStack      /* load new stack segment */
         mov usOldSP, sp         /* save old stack pointer */
         mov sp, usNewSP         /* load new stack pointer */
         }
      }
   else
      {
      DosExit (1, EXIT_PROCESS);
      return 0;
      }

   /* Initialize this module */
   Initialize ();

   /* do the dialog box */

//   ItiWndSetDlgMode (DLG_OTHERMODE);
//   ItiWndBuildWindow (HWND_DESKTOP, SQLBrowserDialog);

   WinDlgBox (HWND_DESKTOP, HWND_DESKTOP, SQLBrowserProc, 0,
              SQLBrowserDialog, NULL);

   Deinitialize ();

   if (selOldStack != 0)
      _asm
      {
      mov ss, selOldStack     /* restore beef stock segment */
      mov sp, usOldSP         /* restore beef stock offset */
      }

   DosExit (EXIT_PROCESS, 0);
   /* please get rid of anoying compiler warning removers! */

    /****************************************************************/ 
   /******************************************************************/
   /******************************************************************/
   /******                                                      ******/
   /*****                                                        *****/
   /****               Do unto others as you would                ****/
   /****                  have them do unto you.                  ****/
   /*****                                                        *****/
   /******                                                      ******/
   /******************************************************************/
   /******************************************************************/
    /****************************************************************/  
   return 0;
   }
