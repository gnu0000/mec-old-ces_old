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
 * DSSFACIL.c
 * Copyright (C) 1990 AASHTO
 */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "dssprop.h"
#include "init.h"
#include "dialog.h"
#include "..\include\dialog.h"
#include "buildqry.h"
#include "..\include\winid.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"


/* exported functions */
MRESULT EXPORT PropBrowserResultsSProc (HWND   hwnd, 
                                            USHORT uMsg,
                                            MPARAM mp1,
                                            MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_CREATE:
         WinSendMsg (WinQueryWindow (hwnd, QW_PARENT, 0), WM_SETICON, 
                     MPFROMP (WinLoadPointer (HWND_DESKTOP, 0, 
                              PropBrowserResultsS)), 0);
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }



MRESULT EXPORT PropBrowserDProc (HWND   hwnd, 
                                     USHORT uMsg,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITDLG:
         WinSendMsg (hwnd, WM_SETICON, 
                     MPFROMP (WinLoadPointer (HWND_DESKTOP, 0, 
                                              DSSPROP_ICON)), 0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_SEARCH:
               {
               HWND hwndChild, hwndList;
               PSZ pszQuery;

               pszQuery = BuildQuery (hwnd);
               if (pszQuery == NULL)
                  break;

               hwndChild = ItiWndBuildWindow (hwnd, PropBrowserResultsS);
               if (hwndChild == NULL)
                  break;

               hwndList = WinWindowFromID (hwndChild, PropBrowserResultsL);

               if (hwndList == NULL)
                  break;

               WinSendMsg (hwndList, WM_CHANGEQUERY, MPFROMLONG (-1), 
                           MPFROMP (pszQuery));
               }
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
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

   ItiWndSetDlgMode (ITI_OTHER);
   ItiWndBuildWindow (HWND_DESKTOP, PropBrowserD);

   Deinitialize ();

   if (selOldStack != 0)
      _asm
      {
      mov ss, selOldStack     /* restore beef stock segment */
      mov sp, usOldSP         /* restore beef stock offset */
      }

   DosExit (EXIT_PROCESS, 0);

    /****************************************************************/ 
   /******************************************************************/
   /******************************************************************/
   /******                                                      ******/
   /*****                                                        *****/
   /****               Do unto others as you would                ****/
   /****                     have them do unto you.               ****/
   /*****                                                        *****/
   /******                                                      ******/
   /******************************************************************/
   /******************************************************************/
    /****************************************************************/  
   return 0;
   }
