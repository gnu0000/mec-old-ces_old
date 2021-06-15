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


/************************************************************************
 *                                                                      *
 * defwndpr.c                                                           *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 * Mark Hampton                                                         *
 *                                                                      *
 * This module provides the default window processing for all           *
 * application workspace windows.                                       *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define INCL_WIN
#include <stdlib.h>
#include "..\include\iti.h"
#include "..\include\help.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\message.h"
#include "..\include\itiperm.h"
#include "..\include\itimenu.h"
#include "winmain.h"
#include "winutil.h"
#include "defwndpr.h"
#include "initdll.h"

#if defined (DEBUG)
#include <stdio.h>
#endif


#if defined (DEBUG)
static void DisplayStackInfo (HWND hwnd);
static void SaveStack (void);
static void ClearStack (void);
#endif



MRESULT EXPORT ItiWndDocDefWindowProc (HWND   hwnd,
                                       USHORT usMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)

   {
   switch (usMessage)
      {
      case WM_HELP:
         {
         USHORT usCommand;

         usCommand = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);
         if (pglobals->hwndHelp != NULL)
            {
            WinPostMsg (pglobals->hwndHelp, HM_DISPLAY_HELP, 
                        MPFROMSHORT (usCommand), 0);
            return 0;
            }
         }
         break;
      }
   return WinDefWindowProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT ItiWndAppDefWindowProc (HWND hwnd,
                                       USHORT usMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)

   {
   switch (usMessage)
      {
      case WM_CREATE:
         WinSetWindowPtr (hwnd, QWP_USER, ItiMemCreateHeap (0));
         break;

      case WM_ITIWNDQUERY:
         if (SHORT1FROMMP (mp1) == ITIWND_HEAP)
            return WinQueryWindowPtr (hwnd, QWP_USER);
         else
            return 0;

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_OPEN_CATALOG_MENU:
               ItiMenuInitOpenCatalogMenuPopup (HWNDFROMMP (mp2));
               break;

            case IDM_EDIT_MENU:
               {
               HWND hwndActiveDoc;

               hwndActiveDoc = WinSendMsg (pglobals->hwndAppFrame,
                                           WM_QUERYACTIVEDOC, 0, 0);

               if (hwndActiveDoc != NULL && !ItiPermQueryEdit (hwndActiveDoc))
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
                  }
               }
               break;

            case IDM_PRINT_MENU:
               ItiMenuInitPrintMenuPopup (HWNDFROMMP (mp2));
               break;

            case IDM_WINDOW_MENU:
               ItiMenuInitWindowMenu (hwnd, HWNDFROMMP (mp2));
               return 0;
               break;
            }
         break;

      case WM_ACTIVATE:
         if (SHORT1FROMMP (mp1) != FALSE && pglobals != NULL)
            {
            HWND hwndActiveDoc;

            hwndActiveDoc = WinSendMsg (pglobals->hwndAppFrame,
                                        WM_QUERYACTIVEDOC, 0, 0);
            if (hwndActiveDoc != NULL)
               WinSetFocus (pglobals->hwndDesktop, hwndActiveDoc);
            }
         break;

      case WM_SYSVALUECHANGED:
         if (pglobals != NULL)
            {
            ItiGlobReinitSysValues (SHORT1FROMMP (mp1), SHORT1FROMMP (mp2));
            return 0;
            }
         break;

      case WM_SIZE:
         {
         SHORT    x, y, cx, cy;
         PSWP     pswp;
         USHORT   usNumWindows, i, usMaxChanged;

         /* resize all maximized windows */
         if (pglobals == NULL)
            break;

         pswp = ItiMenuGetChildSwp (hwnd, &usNumWindows, FALSE);
         if (pswp == NULL)
            break;

         x  = - (SHORT) pglobals->alSysValues [SV_CXSIZEBORDER];
         y  = - (SHORT) pglobals->alSysValues [SV_CYSIZEBORDER];
         cx = SHORT1FROMMP (mp2) + 2 * -x;
         cy = SHORT2FROMMP (mp2) + 2 * -y;

         usMaxChanged = 0;
         for (i=0; i < usNumWindows; i++)
            {
            if (pswp [i].fs & SWP_MAXIMIZE)
               {
               pswp [i].x   = x;
               pswp [i].y   = y;
               pswp [i].cx  = cx;
               pswp [i].cy  = cy;
               pswp [i].fs  |= SWP_MOVE | SWP_SIZE;
               usMaxChanged = i + 1;
               }
            }
         if (usMaxChanged > 0)
            WinSetMultWindowPos (pglobals->habMainThread, pswp, usMaxChanged);
         ItiMemFreeSeg (pswp);
         }
         break;

      case WM_DESTROY:
         {
         HENUM  henum;
         HHEAP  hheap;
         HWND   hwndFrame, hwndStatic, hwndOwner;

         if (pglobals->hwndHelp != NULL)
            WinDestroyHelpInstance (pglobals->hwndHelp);

         /*--- Kill all top level static windows ---*/
         /*--- They in turn kill all there kids  ---*/
         /*--- And so on, and so on, ...         ---*/
         henum = WinBeginEnumWindows (hwnd);

         while ((hwndFrame = WinGetNextWindow (henum)) != NULL)
            {
            hwndStatic = WinWindowFromID (hwndFrame, FID_CLIENT);
            hwndOwner  = (HWND) QW (hwndStatic, ITIWND_OWNERWND, 0, 0, 0);
            if (!hwndOwner ||
                hwndOwner == hwnd ||
                !WinIsWindow (pglobals->habMainThread, hwndOwner))
               WinDestroyWindow (hwndFrame);
            }
         WinEndEnumWindows (henum);

         hheap = WinQueryWindowPtr (hwnd, QWP_USER);
         if (hheap != NULL)
            {
            WinSetWindowPtr (hwnd, QWP_USER, NULL);
            ItiMemDestroyHeap (hheap);
            }
         }
         break;


      case WM_HELP:
         {
         USHORT usCommand;
         USHORT usSource;
         BOOL   bPointer;

         usCommand = SHORT1FROMMP (mp1);
         usSource  = SHORT1FROMMP (mp2);
         bPointer  = SHORT2FROMMP (mp2);
         }
         break;

      case WM_COMMAND:
         if ((SHORT1FROMMP (mp1) >= IDM_WINDOW_LIST_FIRST) &&
             (SHORT1FROMMP (mp1) <= IDM_WINDOW_LIST_LAST + 1))
            {
            ItiMenuActivateWindowFromMenu (hwnd, SHORT1FROMMP (mp1));
            break;
            }

         switch (SHORT1FROMMP (mp1))
            {
#ifdef DEBUG
            case IDM_DEBUG_BREAK:
               DEBUG_BREAK;
               break;

            case IDM_DIE_SCUM:
               {
               PSZ   p;

               p = NULL;
               *p = '\0';
               }
               break;

            case IDM_STACK_INFO:
               DisplayStackInfo (hwnd);
               break;

            case IDM_STACK_CLEAR:
               ClearStack ();
               break;

            case IDM_HIDE_DEBUG_STUFF:
               {
               HWND hwndMenu;
               char szTemp [80];
               USHORT i;

               hwndMenu = WinWindowFromID (pglobals->hwndAppFrame, FID_MENU);
               if (hwndMenu == NULL)
                  break;
               WinSendMsg (hwndMenu, MM_DELETEITEM, 
                           MPFROM2SHORT (IDM_DEBUG_MENU, TRUE), 0);

               WinQueryWindowText (pglobals->hwndAppFrame, sizeof szTemp, szTemp);
               for (i=0; szTemp [i]; i++)
                  if (szTemp [i] == '[')
                     {
                     szTemp [i] = '\0';
                     break;
                     }
               WinSetWindowText (pglobals->hwndAppFrame, szTemp);
               }
               break;

#endif
            case IDM_EXIT:
               WinSendMsg (hwnd, WM_CLOSE, 0L, 0L);
               return 0L;
               break;

            case IDM_ARRANGE_TILED:
               ItiMenuArrangeWindows (hwnd, AW_TILED);
               return 0L;
               break;

            case IDM_ARRANGE_CASCADED:
               ItiMenuArrangeWindows (hwnd, AW_CASCADED);
               return 0L;
               break;

            /* process help commands */
            case IDM_INDEX:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_HELP_INDEX, 0, 0);
               return 0L;
               break;

            case IDM_KEYBOARD:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_KEYBOARD_HELP, 0), 0);
               return 0L;
               break;

            case IDM_MOUSE:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_MOUSE_HELP, 0), 0);
               return 0L;
               break;

            case IDM_COMMANDS:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_COMMANDS_HELP, 0), 0);
               return 0L;
               break;

            case IDM_PROCEDURES:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_PROCEDURES_HELP, 0), 0);
               return 0L;
               break;

            case IDM_GLOSSARY:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_GLOSSARY_HELP, 0), 0);
               return 0L;
               break;

            case IDM_GENERALHELP:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_USING_HELP_HELP, 0), 0);
               return 0L;
               break;

            case IDM_WINDOWS:
               if (pglobals->hwndHelp != NULL)
                  WinSendMsg (pglobals->hwndHelp, HM_DISPLAY_HELP,
                              MPFROM2SHORT (HP_WINDOWS_HELP, 0), 0);
               return 0L;
               break;

            } /* switch (SHORT1FROMMP (mp1)) */

      }
   return WinDefWindowProc (hwnd, usMessage, mp1, mp2);
   }



#if defined (DEBUG)

static void DisplayStackInfo (HWND hwnd)
   {
   PCHAR    pcStack;
   SEL      selStack;
   USHORT   usStackMax;
   BOOL     bDone;
   USHORT   usUsed;
   char     szMessage [256];
   PVOID    pvBottom;

   pvBottom = &pvBottom;
   pcStack = (PCHAR) &pcStack;
   selStack = SELECTOROF (pcStack);
   pcStack = MAKEP (selStack, 0);
   usStackMax = ItiMemQuerySeg (&selStack);

   /* search for bottom of stack */
   bDone = FALSE;
   while (OFFSETOF (pcStack) < usStackMax && !bDone)
      {
      bDone = *pcStack != '\0';
      pcStack++;
      }
   pcStack--;

   usUsed = usStackMax - OFFSETOF (pcStack);
   sprintf (szMessage, "Stack size = %u (0x%x)\n"
                       "Stack Bottom = %u (0x%x)\n"
                       "Current SP = %u:%u (0x%x:0x%x)\n"
                       "Bytes Used = %u (0x%x)",
            usStackMax, usStackMax, 
            OFFSETOF (pcStack), OFFSETOF (pcStack),
            selStack, OFFSETOF (pvBottom), selStack, OFFSETOF (pvBottom), 
            usUsed, usUsed);
   WinMessageBox (HWND_DESKTOP, hwnd, szMessage, "Debug", 0, MB_OK);
   }


static void ClearStack (void)
   {
   PUSHORT  pusBottom;

   pusBottom = (PUSHORT) &pusBottom - 1;
   do
      {
      *pusBottom = 0;
      pusBottom--;
      } while (OFFSETOF (pusBottom) != 0);
   }


#endif

