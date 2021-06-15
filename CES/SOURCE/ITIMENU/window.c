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
 * Window.c
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 * This module takes care of updating the window menu.
 */

#define INCL_WIN
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "window.h"
#include "..\include\itimenu.h"
#include "..\include\Message.h"
#include "..\include\itibase.h"
#include "dialog.h"
#include "init.h"



/*
 * ItiMenuGetWindowTitles returns a pointer to a PWINDOWTITLES structure
 * that contains a list of all window child window handles and their
 * titles.
 *
 * Parameters: hwndParent:       The parent window.
 *
 * Return Value: A pointer to a PWINDOWTITLES structure, or NULL on
 * error.
 *
 * Comments: You must call ItiMenuFreeWindowTitles to release the memory
 * allocated by this function.
 */

PWINDOWTITLES EXPORT ItiMenuGetWindowTitles (HWND hwndParent)
   {
   HENUM          henum;
   USHORT         i, j;
   HWND           hwndChild;
   PWINDOWTITLES  pwt;
   USHORT         usCount;

   usCount = ItiMenuQueryChildWindowCount (hwndParent, 0);
   if (usCount == 0)
      return NULL;

   pwt = (PWINDOWTITLES) ItiMemAllocSeg (sizeof (WINDOWTITLES) + 
                                         usCount * sizeof (WINDOWTITLELIST),
                                         SEG_NONSHARED, 
                                         MEM_ZERO_INIT);
   if (pwt == NULL)
      return NULL;

   pwt->usNumWindows = usCount;

   pwt->pwtl = (PWINDOWTITLELIST) (pwt + 1);

   henum = WinBeginEnumWindows (hwndParent);
   if (henum == NULL)
      {
      ItiMemFreeSeg (pwt);
      return NULL;
      }

   i = 0;
   while ((hwndChild = WinGetNextWindow (henum)) != NULL)
      {
      pwt->pwtl [i].hwnd = hwndChild;

      j = WinQueryWindowTextLength (hwndChild) + 1;

      pwt->pwtl [i].pszTitle = calloc (j, sizeof (char));

      if (pwt->pwtl [i].pszTitle == NULL)
         break;

      WinQueryWindowText (hwndChild, j, pwt->pwtl [i].pszTitle);

      i++;
      }

   WinEndEnumWindows (henum);

   return pwt;
   }







/*
 * This function is used by the ItiMenuSortWindowTitles function.  Note
 * that the function must be a _cdecl, since that is the calling
 * method of the c run-time library.
 */

static int _cdecl WindowTitleCmp (PWINDOWTITLELIST pwtl1,
                                  PWINDOWTITLELIST pwtl2)
   {
   return stricmp (pwtl1->pszTitle, pwtl2->pszTitle);
   }




/*
 * ItiMenuSortWindowTitles sorts the given window titles.  This function
 * uses a case insensitive sort.
 *
 * Parameters: pwt         A pointer to a window titles structure.
 *
 * Return Value: None.  The window titles pointed to by pwt are
 * sorted.
 *
 * Comments:
 */

VOID EXPORT ItiMenuSortWindowTitles (PWINDOWTITLES pwt)
   {
   qsort (pwt->pwtl, pwt->usNumWindows, sizeof (WINDOWTITLELIST), 
          WindowTitleCmp);
   }








/*
 * ItiMenuFreeWindowTitles frees the memory taken up by the given window
 * titles.
 *
 * Parameters: pwt         A pointer to a window titles structure.
 *
 * Return Value: None.  The window titles pointed to by pwt are
 * freed.
 *
 * Comments:  The caller must not reference the pwt variable after
 * this function is called, or a segmentation fault may occur.
 */


VOID EXPORT ItiMenuFreeWindowTitles (PWINDOWTITLES pwt)
   {
   if (pwt == NULL)
      return;

   ItiMemFreeSeg (pwt);
   }







/*
 * ItiMenuInitWindowMenu initializes the window menu for the given application.
 *
 * Parameters: hwndAppClient:    A handle to the application's client
 *                               window.
 *
 *             hwndMenu:         A handle to the window menu.
 *
 * Return Value: None.
 *
 * Comments: This function enables or disables the Arrange Tiled and
 * Arrange Cascaded commands, and lists the most immediate children
 * of hwndAppClient.
 */

VOID EXPORT ItiMenuInitWindowMenu (HWND hwndAppClient, HWND hwndMenu)
   {
   USHORT         i, iMax;
   PWINDOWTITLES  pwt;
   char           szMenuLine [80];
   HWND           hwndActive;
   BOOL           bFoundActive;

   hwndActive = WinSendMsg (WinQueryWindow (pglobals->hwndAppClient,
                                            QW_PARENT, FALSE),
                            WM_QUERYACTIVEDOC, 0, 0);

   /* delete all of the old window menu stuff */
   if (WINDOW_MENU_COUNT < ItiMenuDeleteMenuItem (hwndMenu, IDM_WINDOW_SEPARATOR))
      {
      for (i=IDM_WINDOW_LIST_FIRST; i <= IDM_WINDOW_LIST_LAST + 1; i++)
         {
         if (WINDOW_MENU_COUNT == ItiMenuDeleteMenuItem (hwndMenu, i))
            {
            break;
            }
         }
      }

   pwt = ItiMenuGetWindowTitles (pglobals->hwndAppClient);
   if (pwt == NULL)
      {
      /* gray the arrange tiled and arrange cascaded */
      ItiMenuGrayMenuItem (hwndMenu, IDM_ARRANGE_CASCADED, TRUE);
      ItiMenuGrayMenuItem (hwndMenu, IDM_ARRANGE_TILED, TRUE);

      return;
      }

   /* ungray the arrange tiled/cascaded commands */
   ItiMenuGrayMenuItem (hwndMenu, IDM_ARRANGE_CASCADED, FALSE);
   ItiMenuGrayMenuItem (hwndMenu, IDM_ARRANGE_TILED, FALSE);

   ItiMenuSortWindowTitles (pwt);

   ItiMenuInsertMenuSeparator (hwndMenu, IDM_WINDOW_SEPARATOR);

   /* insert the first nine on the window menu */
   bFoundActive = FALSE;
   iMax = (pwt->usNumWindows > NUMBER_OF_WINDOW_LISTS) ?
          NUMBER_OF_WINDOW_LISTS : pwt->usNumWindows;
   for (i=0; i < iMax; i++)
      {
      sprintf (szMenuLine, "~%d %.70s", i+1, pwt->pwtl [i].pszTitle);
      ItiMenuInsertMenuItem (hwndMenu, szMenuLine, IDM_WINDOW_LIST_FIRST + i);
      if (pwt->pwtl [i].hwnd == hwndActive)
         {
         ItiMenuCheckMenuItem (hwndMenu, IDM_WINDOW_LIST_FIRST + i, TRUE);
         bFoundActive = TRUE;
         }
      }

   if (pwt->usNumWindows > NUMBER_OF_WINDOW_LISTS)
      {
      /* insert more windows command */
      ItiMenuInsertMenuItem (hwndMenu, "~More Windows...", IDM_WINDOW_LIST_LAST+1);
      if (!bFoundActive)
         {
         ItiMenuCheckMenuItem (hwndMenu, IDM_WINDOW_LIST_LAST + 1, TRUE);
         }
      }

   ItiMenuFreeWindowTitles (pwt);
   }




/*
 * This is the dialog box procedure for the More Windows menu
 * choice. This function should not be referenced by other modules.
 */


MRESULT EXPORT ChooseActiveWindow (HWND     hwnd,
                                     USHORT   usMessage,
                                     MPARAM   mp1,
                                     MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         PWINDOWTITLES  pwt;
         HWND           hwndList, hwndActive, hwndAppFrame;
         USHORT         i;
         SWP            swpMe;
         SWP            swpApplication;

         pwt = (PWINDOWTITLES) PVOIDFROMMP (mp2);
         hwndList = WinWindowFromID (hwnd, DID_WINDOW_LIST);
         hwndAppFrame = WinQueryWindow (hwnd, QW_OWNER, FALSE);
         hwndActive = WinSendMsg (hwndAppFrame, WM_QUERYACTIVEDOC, 0, 0);

         for (i = 0; i < pwt->usNumWindows; i++)
            {
            WinSendMsg (hwndList, LM_INSERTITEM,
               MPFROMSHORT (i), MPFROMP (pwt->pwtl [i].pszTitle));

            if (pwt->pwtl[i].hwnd == hwndActive)
               {
               WinSendMsg (hwndList, LM_SELECTITEM, MPFROMSHORT (i),
                  MPFROMSHORT (TRUE));
               }
            }

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
            {
            swpMe.x -= (swpMe.x + swpMe.cx) -
                       pglobals->alSysValues [SV_CXSCREEN];
            }
         if (swpMe.y + swpMe.cy > (SHORT) pglobals->alSysValues [SV_CYSCREEN])
            {
            swpMe.y -= (swpMe.y + swpMe.cy) -
                       pglobals->alSysValues [SV_CYSCREEN];
            }

         swpMe.fs |= SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         }
         break;

      case WM_BUTTON1DBLCLK:
         {
         POINTL ptl;
         HWND hwndList;
         
         ptl.x = (SHORT) SHORT1FROMMP (mp1);
         ptl.y = (SHORT) SHORT2FROMMP (mp1);
         hwndList = WinWindowFromID (hwnd, DID_WINDOW_LIST);

         if (hwndList == WinWindowFromPoint (hwnd, &ptl, FALSE, 0))
            {
            WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (DID_OK),
                                          MPFROM2SHORT (CMDSRC_OTHER, TRUE));
            }
         }
         break;

      case WM_CLOSE:
         WinDismissDlg (hwnd, 0xffff);
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HWND   hwndList;
               USHORT iSelected;

               hwndList = WinWindowFromID (hwnd, DID_WINDOW_LIST);

               iSelected = SHORT1FROMMR (WinSendMsg (hwndList,
                                                     LM_QUERYSELECTION,
                                                     0, 0));

               if (iSelected == LIT_NONE)
                  iSelected = 0xffff;

               WinDismissDlg (hwnd, iSelected);
               return 0L;
               }
               break;

            case DID_CANCEL:
               WinDismissDlg (hwnd, 0xffff);
               return 0L;
               break;
            }
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




/*
 * ItiMenuActivateWindowFromMenu Activates a window based on the user's
 * choice from the window menu.
 *
 * Parameters: hwndAppClient:    A handle to the application's client
 *                               window.
 *
 *             idWindow:         The window command received.
 *
 * Return Value: None.
 *
 * Comments: This function activates the given window.  This function
 * assumes that no windows have been created or destroyed between the
 * time that ItiMenuInitWindowMenu and ItiMenuActivateWindowFromMenu have been
 * called.
 */

VOID EXPORT ItiMenuActivateWindowFromMenu (HWND hwndAppClient, USHORT idWindow)
   {
   PWINDOWTITLES  pwt;
   USHORT         usWindowNumber;

   pwt = ItiMenuGetWindowTitles (hwndAppClient);
   if (pwt == NULL)
      return;

   ItiMenuSortWindowTitles (pwt);

   usWindowNumber = idWindow - IDM_WINDOW_LIST_FIRST;
   if (usWindowNumber >= NUMBER_OF_WINDOW_LISTS)
      {
      /* create the dialog box with the list of windows */
      usWindowNumber = WinDlgBox (pglobals->hwndDesktop, hwndAppClient,
                                  ChooseActiveWindow, hmodMe,
                                  IDD_CHOOSE_ACTIVE_WINDOW, pwt);
      }

   if (usWindowNumber < pwt->usNumWindows)
      WinSetActiveWindow (pglobals->hwndDesktop, pwt->pwtl [usWindowNumber].hwnd);

   ItiMenuFreeWindowTitles (pwt);
   }

