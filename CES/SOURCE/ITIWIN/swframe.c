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
 * swframe.c                                                            *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This module contains the window subclass function for frame windows. *
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
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itimenu.h"
#include "..\include\message.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itidbase.h"
#include "swframe.h"
#include "swmain.h"
#include "winutil.h"
#include "initdll.h"



typedef struct
   {
   HWND  hwndSysMenu;      /* handle to system menu */
   HWND  hwndMaxButton;    /* handle to min/max button */
   PFNWP pfnwpOldDocFrameProc;
   PFNWP pfnwpOldDocTitleProc;
   HHEAP hheap;
   } DOCFRAMEINFO;

typedef DOCFRAMEINFO FAR *PDOCFRAMEINFO;



#define QWP_FRAME_INFO     0



USHORT FrameErr (PSZ psz)
   {
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, psz,
                  "SW-Frame", 666, MB_OK | MB_APPLMODAL);
   return 1;
   }





MRESULT EXPENTRY DocFrameProc (HWND    hwnd,
                               USHORT   umsg,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   PDOCFRAMEINFO  pdfi;
   pdfi = WinQueryWindowPtr (hwnd, QWP_FRAME_INFO);

   switch (umsg)
      {
      case WM_INITSUBCLASS:
         {
         HWND           hwndClient;
         HHEAP          hheap;

         /* allocate our data */
         hwndClient = WinWindowFromID (hwnd, FID_CLIENT);
         hheap = (HHEAP) QW (hwndClient, ITIWND_HEAP, 0, 0, 0);

#ifdef DEBUG
         if (hheap == NULL)
            return MPFROMSHORT (FrameErr ("Unable to obtain heap DocFrameProc"));
#endif
         pdfi = ItiMemAlloc (hheap, sizeof (DOCFRAMEINFO), MEM_ZERO_INIT);

#ifdef DEBUG
         if (pdfi == NULL)
            return MPFROMSHORT (FrameErr ("Unable to obtain enough mem in WM_CREATE, DocFrameProc"));
#endif
         pdfi->hwndSysMenu = NULL;
         pdfi->hwndMaxButton = NULL;
         pdfi->pfnwpOldDocFrameProc = (PFNWP) mp1;
         pdfi->hheap = hheap;
         WinSetWindowPtr (hwnd, QWP_FRAME_INFO, pdfi);
         break;
         }

      case WM_ACTIVATE:
         {
         HWND           hwndParentFrame;

         hwndParentFrame = WinQueryWindow (
                WinQueryWindow (hwnd, QW_PARENT, FALSE), QW_PARENT, FALSE);

         if (SHORT1FROMMP (mp1))
            {
            /* window is being activated */

            /* tell our parent that we're active */
            WinSendMsg (hwndParentFrame, WM_SETACTIVEDOC,
                        MPFROMHWND (hwnd), 0L);

            if (pdfi == NULL)
               break;

            /* When we get the first WM_ACTIVATE command, hwndSysMenu
               and hwndMaxButton can be NULL. */

            if (pdfi->hwndSysMenu != NULL)
               {
               WinSetParent (pdfi->hwndSysMenu, hwnd, FALSE);
               pdfi->hwndSysMenu = NULL;
               }

            if (pdfi->hwndMaxButton != NULL)
               {
               WinSetParent (pdfi->hwndMaxButton, hwnd, FALSE);
               pdfi->hwndMaxButton = NULL;
               }
            WinSendMsg (hwnd, WM_UPDATEFRAME, 0L, 0L);
            }
         else
            {
            /* window is being deactivated */
            /* tell our parent that nobody is active */
            WinSendMsg (hwndParentFrame, WM_SETACTIVEDOC, NULL, 0L);

            if (pdfi == NULL)
               break;

            pdfi->hwndSysMenu = WinWindowFromID (hwnd, FID_SYSMENU);
            pdfi->hwndMaxButton = WinWindowFromID (hwnd, FID_MINMAX);
            
            /* you must hide the windows before changing
               their parents, or they won't get painted correctly -- mdh */
            WinShowWindow (pdfi->hwndSysMenu, FALSE);
            WinShowWindow (pdfi->hwndMaxButton, FALSE);
            WinSetParent (pdfi->hwndSysMenu, pglobals->hwndObject, FALSE);
            WinSetParent (pdfi->hwndMaxButton, pglobals->hwndObject, FALSE);
            WinSendMsg (hwnd, WM_UPDATEFRAME, 0L, 0L);
            }
         }
         break;

      case WM_QUERYTRACKINFO:
         if (WinQueryWindowULong (hwnd, QWL_STYLE) & WS_MAXIMIZED)
            {
            WinSetActiveWindow (pglobals->hwndDesktop, hwnd);
            return FALSE;
            }
         break;

      case WM_DESTROY:
         if (pdfi != NULL)
            {
            if (pdfi->hwndMaxButton != NULL)
               WinDestroyWindow (pdfi->hwndMaxButton);

            if (pdfi->hwndSysMenu != NULL)
               WinDestroyWindow (pdfi->hwndSysMenu);

            WinSetWindowPtr (hwnd, QWP_FRAME_INFO, NULL);
            ItiMemFree (pdfi->hheap, pdfi);
            pdfi = NULL;
            }
//       GpiDeleteBitmap ();
         break;

      case WM_NEXTMENU:
         if (WinQueryWindowUShort (HWNDFROMMP (mp1), QWS_ID) == FID_SYSMENU)
            {
            if (SHORT1FROMMP (mp2))
               return (MRESULT) WinWindowFromID (pglobals->hwndAppFrame, FID_SYSMENU);
            else
               return (MRESULT) WinWindowFromID (pglobals->hwndAppFrame, FID_MENU);
            }
         return 0;

      case WM_SYSCOMMAND:

         if (SHORT1FROMMP (mp2) != (SHORT) CMDSRC_ACCELERATOR)
            break;

         /* if the command was sent because of an accelerator,
            we need to see if it goes to the document window or
            the main frame window */

         /* the control key is down -- we'll send it to the document
            frame, since that means it's either ctrl-esc or one of
            the document accelerators */

         if (WinGetKeyState (pglobals->hwndDesktop, VK_CTRL) & 0x8000)
            break;

         else if (SHORT1FROMMP (mp1) == IDM_SYSMENU)
            {
            /* if the window is maximized, we need to send the message
               to the system menu located on the main menu. */
            if (WinQueryWindowUShort (hwnd, QWL_STYLE) & WS_MAXIMIZED)
               {
               WinPostMsg (WinWindowFromID (pglobals->hwndAppFrame, FID_MENU),
                  MM_STARTMENUMODE, MPFROM2SHORT (TRUE, FALSE), 0L);
               return 0;
               }
            else
               {
               WinPostMsg (WinWindowFromID (hwnd, FID_SYSMENU),
                  MM_STARTMENUMODE, MPFROM2SHORT (TRUE, FALSE), 0L);
               }
            }
         else
            return WinSendMsg (pglobals->hwndAppFrame, umsg, mp1, mp2);
         break;
      }
   if (pdfi == NULL)
      return  WinDefWindowProc(hwnd, umsg, mp1, mp2);
   else
      return pdfi->pfnwpOldDocFrameProc (hwnd, umsg, mp1, mp2);
   }








MRESULT EXPENTRY DocTitleProc (HWND    hwnd,
                               USHORT   umsg,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   PDOCFRAMEINFO  pdfi;
   pdfi = WinQueryWindowPtr (WinQueryWindow (hwnd, QW_PARENT, 0), QWP_FRAME_INFO);

   switch (umsg)
      {
      case WM_INITSUBCLASS:
         {
         pdfi->pfnwpOldDocTitleProc = (PFNWP) mp1;
         break;
         }

      case WM_BUTTON2DBLCLK:
         {
         HWND  hwndClient;

         hwndClient = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT, 0),
                                       FID_CLIENT);
         WinPostMsg (hwndClient, WM_COMMAND, MPFROMSHORT (IDM_NORMALSIZE),
                                             MPFROMSHORT (CMDSRC_MENU));
         }
      }
   if (pdfi == NULL)
      return  WinDefWindowProc(hwnd, umsg, mp1, mp2);
   else
      return pdfi->pfnwpOldDocTitleProc (hwnd, umsg, mp1, mp2);
   }









typedef struct
   {
   HWND  hwndActiveDoc;
   PFNWP pfnwpOldAppFrameProc;
   HHEAP hheap;
   } FRAMEINFO;

typedef FRAMEINFO FAR *PFRAMEINFO;




MRESULT EXPENTRY AppFrameProc (HWND     hwnd,
                               USHORT   umsg,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   PFRAMEINFO  pfi;

   pfi = WinQueryWindowPtr (hwnd, QWP_FRAME_INFO);

   switch (umsg)
      {
      case WM_INITSUBCLASS:
         {
         HHEAP hheap;
         HWND  hwndClient;

         /* allocate our data */
         hwndClient = WinWindowFromID (hwnd, FID_CLIENT);
         hheap = (HHEAP) QW (hwndClient, ITIWND_HEAP, 0, 0, 0);

#ifdef DEBUG
         if (hheap == NULL)
            return MPFROMSHORT (FrameErr ("Unable to obtain heap, AppFrameProc"));
#endif
         pfi = ItiMemAlloc (hheap, sizeof (FRAMEINFO), MEM_ZERO_INIT);

#ifdef DEBUG
         if (pfi == NULL)
            return MPFROMSHORT (FrameErr ("Unable to obtain enough mem in WM_CREATE, AppFrameProc"));
#endif

         pfi->hwndActiveDoc = NULL;
         pfi->hheap = hheap;
         pfi->pfnwpOldAppFrameProc = (PFNWP) mp1;
         WinSetWindowPtr (hwnd, QWP_FRAME_INFO, pfi);
         break;
         }

      case WM_SETACTIVEDOC:
         /* set the active child flag */
         if (pfi != NULL)
            pfi->hwndActiveDoc = HWNDFROMMP (mp1);
         break;

      case WM_QUERYACTIVEDOC:
         if (pfi != NULL)
            return pfi->hwndActiveDoc;
         return NULL;

      case WM_NEXTMENU:
         {
         BOOL bPrevious;

         if (pfi == NULL)
            break;

         bPrevious = SHORT1FROMMP (mp2);
         if ((mp1 == WinWindowFromID (hwnd, FID_SYSMENU) && !bPrevious) ||
             (mp1 == WinWindowFromID (hwnd, FID_MENU)    &&  bPrevious))
            {
            if (pfi->hwndActiveDoc != NULL)
               return WinWindowFromID (pfi->hwndActiveDoc, FID_SYSMENU);
            }
         break;
         }

      case WM_DESTROY:
         if (pfi != NULL)
            {
            WinSetWindowPtr (hwnd, QWP_FRAME_INFO, NULL);
            ItiMemFree (pfi->hheap, pfi);
            pfi = NULL;
            }
         break;

      case WM_SYSCOMMAND:
         if (pfi == NULL)
            break;

         if (SHORT1FROMMP (mp2) == (SHORT) CMDSRC_ACCELERATOR &&
             pfi->hwndActiveDoc != NULL)
            {
            /* if the command was sent because of an accelerator,
               we need to see if it goes to the document window or
               the main frame window */
            if (WinGetKeyState (pglobals->hwndDesktop, VK_CTRL) & 0x8000)
               {
               /* the control key is down -- we'll send it to the document
                  frame, since that means it's either ctrl-esc or one of
                  the document accelerators */
               return WinSendMsg (pfi->hwndActiveDoc, umsg, mp1, mp2);
               }
            else if (SHORT1FROMMP (mp1) == IDM_SYSMENU)
               {
               WinPostMsg (WinWindowFromID (pfi->hwndActiveDoc, FID_SYSMENU),
                  MM_STARTMENUMODE, MPFROM2SHORT (TRUE, FALSE), 0L);
               }
            }
         break;


      case WM_COMMAND:
         {
         if (pfi == NULL)
            break;
         if (SHORT1FROMMP (mp2) != (SHORT) CMDSRC_ACCELERATOR)
            break;
         if (SHORT1FROMMP (mp1) != IDM_NORMALSIZE)
            break;

         WinPostMsg (WinWindowFromID (pfi->hwndActiveDoc, FID_CLIENT),
                           WM_COMMAND, mp1, MPFROMSHORT (CMDSRC_MENU));

         break;
         }


      case WM_BUTTON2DBLCLK:
         {
         DosBeep (1775, 135);
         return (MRESULT) TRUE;
         }

      }

   if (pfi == NULL)
      return  WinDefWindowProc(hwnd, umsg, mp1, mp2);
   else
      return pfi->pfnwpOldAppFrameProc (hwnd, umsg, mp1, mp2);
   }





/*
 * ItiWndSetMDI sets a given frame for an application document window
 * to behave as an ITI MDI window of an MDI application in a MDI
 * world in a MDI universe.
 *
 * Parameters: hwndFrame      The frame of the window to become an MDI
 *                            window.  Note that this must be an immediate
 *                            child window of the application window that
 *                            has been subclassed by the ItiWndSetMDIApp
 *                            function.
 *
 * Return value:
 * TRUE if the window was subclassed, or FALSE if not.
 */

BOOL EXPORT ItiWndSetMDI (HWND hwndFrame)
   {
   HWND     hwndSysMenu, hwndTitle;
   ULONG    ulFlags;
   PFNWP    pfnwpOldDocFrameProc;
   PFNWP    pfnwpOldDocTitleProc;

   /* make our windows work correctly with DS/Shell  -- mdh */
   /* subclass the frame window so that the MDI stuff works properly */
   pfnwpOldDocFrameProc = WinSubclassWindow (hwndFrame, DocFrameProc);


   /* Just to totally confuse the issue, here we subclass the */
   /* title window to intercept dbl-clk-right messages        */
   hwndTitle = WinWindowFromID (hwndFrame, FID_TITLEBAR);
   pfnwpOldDocTitleProc = WinSubclassWindow (hwndTitle, DocTitleProc);


   if (pfnwpOldDocFrameProc == NULL || pfnwpOldDocTitleProc == NULL)
      {
      ItiErrDisplayWindowError (pglobals->habMainThread,
                             pglobals->hwndAppFrame, 0);
      WinDestroyWindow (hwndFrame);
      return FALSE;
      }

   WinSendMsg (hwndFrame, WM_INITSUBCLASS, MPFROMP (pfnwpOldDocFrameProc), 0);
   WinSendMsg (hwndTitle, WM_INITSUBCLASS, MPFROMP (pfnwpOldDocTitleProc), 0);

   /* load in the document window's system menu */
   hwndSysMenu = WinLoadMenu (hwndFrame, hmodModule, CHILD_SYSTEM_MENU);
   if (hwndSysMenu == NULL)
      {
      ItiErrDisplayWindowError (pglobals->habMainThread,
                             pglobals->hwndAppFrame, 0);
      }

   /* make our new system menu look like a normal system menu to the
      frame manager, so it get formatted correctly */
   ulFlags = WinQueryWindowULong (hwndSysMenu, QWL_STYLE);
   WinSetWindowULong (hwndSysMenu, QWL_STYLE, ulFlags | MS_TITLEBUTTON);
   WinSetWindowUShort (hwndSysMenu, QWS_ID, FID_SYSMENU);

   /* set the bitmap to the SBMP_CHILDSYSMENU bitmap */
   WinSendMsg (hwndSysMenu, MM_SETITEMHANDLE, (MPARAM) IDM_SYSMENU,
               (MPARAM) WinGetSysBitmap(pglobals->hwndDesktop, SBMP_CHILDSYSMENU));

   return TRUE;
   }





/*
 * ItiWndSetMDIApp sets a given frame for an application window
 * to behave as an ITI MDI application.
 *
 * Parameters: hwndAppFrame   The frame of the application to become an MDI
 *                            application.  
 *
 * Return value:
 * TRUE if the window was subclassed, or FALSE if not.
 */

BOOL EXPORT ItiWndSetMDIApp (HWND hwndAppFrame)
   {
   PFNWP    pfnwpOldAppFrameProc;

   /* make our windows work correctly with DS/Shell  -- mdh */
   /* subclass the application frame window so that the MDI stuff works 
      properly */
   pfnwpOldAppFrameProc = WinSubclassWindow (hwndAppFrame, AppFrameProc);

   if (pfnwpOldAppFrameProc == NULL)
      {
      ItiErrDisplayWindowError (pglobals->habMainThread,
                             pglobals->hwndAppFrame, 0);
      WinDestroyWindow (hwndAppFrame);
      return FALSE;
      }

   WinSendMsg (hwndAppFrame, WM_INITSUBCLASS, MPFROMP (pfnwpOldAppFrameProc), 0);

   return TRUE;
   }





