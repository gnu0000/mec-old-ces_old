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
 * BAMSCES.c
 * Mark Hampton
 *
 */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "BAMSCES.h"
#include "menu.h"
#include "init.h"
#include "command.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"


/* exported functions */

MRESULT EXPENTRY MainWindowProc (HWND   hwnd,
                                 USHORT usMessage,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_PAINT:
         {
         HPS   hps;        /* presentation-space handle */
         RECTL rclClient;

         hps = WinBeginPaint (hwnd, NULL, &rclClient); /* start painting  */

         WinFillRect (hps, &rclClient, SYSCLR_APPWORKSPACE);     /* fill background */

         WinEndPaint (hps);                      /* end painting  */
         }
         return 0L;
         break;

      case WM_INITMENU:
         if (GreyMenuItems (HWNDFROMMP (mp2), SHORT1FROMMP (mp1)))
            return 0L;
         else
            /* fall through to default message processing */
            break;
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            default:
               if (ProcessCommand (hwnd, SHORT1FROMMP (mp1), mp2))
                  return 0L;
               else
                  /* fall through to default message processing */
                  break;
               break;
            } /* switch (SHORT1FROMMP (mp1)) */
         break;

      }
   /* NOTE: make sure to use ItiWndAppDefWindowProc here */
   return ItiWndAppDefWindowProc (hwnd, usMessage, mp1, mp2);
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
static char  szBuffer [250] = "";
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
         mov selOldStack, ss     /* save old beef stock segment */
         mov ss,selNewStack      /* load new beef stock segment */
         mov usOldSP, sp         /* save old beef stock pointer */
         mov sp, usNewSP         /* load new beef stock pointer */
         }
      }
   else
      {
      DosExit (1, EXIT_PROCESS);
      return 0;
      }

   /* Initialize this module */
   Initialize ();

   /* Get and dispatch messages. */
   while (WinGetMsg (pglobals->habMainThread, &qmsg, NULL, 0, 0))
      {
      if (qmsg.hwnd != NULL)
         WinDispatchMsg (pglobals->habMainThread, &qmsg);
      else
         {
         /* process WM_SEMx messages */
#ifdef DEBUG
         sprintf (szBuffer, "Got a message with NULL for hwnd: "
            "usMessage: %d (0x%.4x), "
            "mp1:  0x%.8lx, "
            "mp2:  0x%.8lx, "
            "Time: 0x%.8lx, "
            "Mouse:(%ld, %ld)",
            qmsg.msg, qmsg.msg, qmsg.mp1, qmsg.mp2,
            qmsg.time, qmsg.ptl.x, qmsg.ptl.y);
         ItiErrWriteDebugMessage (szBuffer);
#endif
         }
      }

   Deinitialize ();

   if (selOldStack != 0)
      _asm
      {
      mov ss, selOldStack     /* restore beef stock segment */
      mov sp, usOldSP         /* restore beef stock offset */
      }

   DosExit (EXIT_PROCESS, 0);
   /* please get rid of anoying compiler warning removers! */

   /* please get rid of anoying compiler warning remover removers! */


    /****************************************************************/ 
   /******************************************************************/
   /******************************************************************/
   /******                                                      ******/
   /*****      In the beginning GOD created the heavens          *****/
   /****        and the earth...                                  ****/
   /*****                                                        *****/
   /******                                                      ******/
   /******************************************************************/
   /******************************************************************/
    /****************************************************************/  
   return 0;
   }
