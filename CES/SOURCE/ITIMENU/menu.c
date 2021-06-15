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
 * Menu.c
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 * This module provide menu interface routines
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "window.h"
#include <string.h>
#include "..\include\itimenu.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "init.h"

/*
 * ItiMenuQueryChildWindowCount returns the number of child windows for
 * hwndParent.
 *
 * Parameters: hwndParent:       The parent window who's children
 *                               should be counted.
 *
 *             usRecurse:        This is a USHORT representing the
 *                               number of levels to recurse.  If this
 *                               zero, then only the immediate children
 *                               of hwndParent are counted.
 *
 * Return Value:  The number of children of hwndParent.
 *
 * Comments:
 */

USHORT EXPORT ItiMenuQueryChildWindowCount (HWND     hwndParent,
                                          USHORT   usRecurse)
   {
   HENUM    henum;
   USHORT   usCount;
   HWND     hwndChild;

   henum = WinBeginEnumWindows (hwndParent);
   if (henum == NULL)
      return 0;

   usCount = 0;

   while ((hwndChild = WinGetNextWindow (henum)) != NULL)
      {
      usCount++;
      if (usRecurse > 0)
         usCount += ItiMenuQueryChildWindowCount (hwndChild, usRecurse - 1);
      }

   WinEndEnumWindows (henum);

   return usCount;
   }




/*
 * ItiMenuDeleteMenuItem deletes the specified menu item from the menu
 * hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             idCommand:        The command ID of the item to delete.
 *
 * Return value: The count of remaining items for the menu.
 *
 * Comments:
 */


USHORT EXPORT ItiMenuDeleteMenuItem (HWND   hwndMenu,
                                   USHORT idCommand)
   {
   return SHORT1FROMMR (WinSendMsg (hwndMenu, MM_DELETEITEM,
                               MPFROM2SHORT (idCommand, TRUE), 0));
   }




/*
 * ItiMenuQuerySubmenu retrieves the specified submenu.
 *
 * Parameters: hwndMenu:         The top level menu.
 *
 *             usCommand:        The command ID of the submenu.
 *
 * Return value: A handle to the submenu, if successful, or NULL 
 * if not.
 *
 * Comments:
 *    The search for the given menu will include searching of child menus.
 */


HWND EXPORT ItiMenuQuerySubmenu (HWND    hwndMenu,
                                USHORT  usCommand)
   {
   MENUITEM mi;

   if (WinSendMsg (hwndMenu, MM_QUERYITEM, MPFROM2SHORT (usCommand, TRUE),
                   (MPARAM) &mi))
      {
      /* we got the menu item */
      return mi.hwndSubMenu;
      }
   else
      return NULL;
   }





/*
 * ItiMenuInsertMenuSeparator inserts a menu separator at the end of menu
 * hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             idCommand:        The command ID of the menu separator to
 *                               insert.
 *
 * Return value: The position of the inserted menu separator, or either
 * MIT_MEMERROR if memory cannot be allocated for the separator, or
 * MIT_ERROR if any other error occured.
 *
 * Comments:
 */


USHORT EXPORT ItiMenuInsertMenuSeparator (HWND   hwndMenu,
                                        USHORT idCommand)
   {
   MENUITEM mi;

   mi.iPosition   = MIT_END;
   mi.afStyle     = MIS_SEPARATOR | MIS_STATIC;
   mi.afAttribute = 0;
   mi.id          = idCommand;
   mi.hwndSubMenu = NULL;
   mi.hItem       = 0;

   return SHORT1FROMMR (WinSendMsg (hwndMenu, MM_INSERTITEM, MPFROMP (&mi),
                                    MPFROMP (NULL)));
   }




/*
 * ItiMenuInsertMenuItem inserts a menu item at the end of menu hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             idCommand:           The command ID of the menu item to
 *                               insert.
 *
 * Return value: The position of the inserted menu item, or either
 * MIT_MEMERROR if memory cannot be allocated for the item, or
 * MIT_ERROR if any other error occured.
 *
 * Comments:
 */


USHORT EXPORT ItiMenuInsertMenuItem (HWND   hwndMenu,
                                   PSZ    pszMenuText,
                                   USHORT idCommand)
   {
   MENUITEM mi;

   mi.iPosition   = MIT_END;
   mi.afStyle     = MIS_TEXT;
   mi.afAttribute = 0;
   mi.id          = idCommand;
   mi.hwndSubMenu = NULL;
   mi.hItem       = 0;

   return SHORT1FROMMR (WinSendMsg (hwndMenu, MM_INSERTITEM, MPFROMP (&mi),
                                    MPFROMP (pszMenuText)));
   }





/*
 * ItiMenuCheckMenuItem checks the specified menu item on the menu hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             idCommand:        The command ID of the item to check.
 *
 *             bCheck:           If TRUE, the item is checked.  If FALSE,
 *                               any checks are removed.
 *
 * Return value: TRUE if the menu item was succesfully change, FALSE
 * if not.
 *
 * Comments:
 */


BOOL EXPORT ItiMenuCheckMenuItem (HWND   hwndMenu,
                                USHORT idCommand,
                                BOOL   bCheck)
   {
   return SHORT1FROMMR (WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (idCommand, FALSE),
               MPFROM2SHORT (MIA_CHECKED, bCheck ? MIA_CHECKED : FALSE)));
   }





/*
 * ItiMenuGrayMenuItem grays the specified menu item on the menu hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             idCommand:        The command ID of the item to check.
 *
 *             bGray:            If TRUE, the item is grayed.  If FALSE,
 *                               the item is enabled.
 *
 * Return value: TRUE if the menu item was succesfully change, FALSE
 * if not.
 *
 * Comments:
 */


VOID EXPORT ItiMenuGrayMenuItem (HWND    hwndMenu,
                               USHORT  idCommand,
                               BOOL    bGray)
   {
   WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT (idCommand, FALSE),
               MPFROM2SHORT (MIA_DISABLED, bGray ? MIA_DISABLED : FALSE));
   }










/*
 * ItiMenuGetChildSwp gets the SWPs for the children of hwndParent.
 *
 * Parameters: hwndParent:    The parent window.
 *
 *             pusNumWindows: A pointer to a USHORT that will contain
 *                            the number of elements in the returned
 *                            SWP.  This may not be NULL.
 *
 *             bForArrange:   If this parameter is TRUE, then only
 *                            the hwnd field of the swp structure is
 *                            filled in.  Also, any maximized windows
 *                            are restored.  If this field is FALSE, then
 *                            the full swp structure is filled in.
 *
 * Return Value: A pointer to an array of SWPs.  The number of SWPs
 * is stored in *pusNumWindows.  The array of swps is dynamically allocated
 * by calling ItiMemAllocSeg.  The caller must free the memory by calling
 * ItiMemFreeSeg.
 */


PSWP EXPORT ItiMenuGetChildSwp (HWND     hwndParent,
                              PUSHORT  pusNumWindows,
                              BOOL     bForArrange)
   {
   PSWP     pswp;
   HENUM    henum;
   HWND     hwnd;
   USHORT   i;
   ULONG    ulStyle;
   
   *pusNumWindows = ItiMenuQueryChildWindowCount (hwndParent, 0);

   if (*pusNumWindows == 0)
      return NULL;

   pswp = ItiMemAllocSeg (*pusNumWindows * sizeof (SWP), SEG_NONSHARED, 
                          MEM_ZERO_INIT);
   if (pswp == NULL)
      return NULL;

   henum = WinBeginEnumWindows (hwndParent);
   if (henum == NULL)
      {
      ItiErrDisplayWindowError (pglobals->habMainThread, hwndParent, 0);
      ItiMemFreeSeg (pswp);
      return NULL;
      }

   i = 0;
   while (hwnd = WinGetNextWindow (henum))
      {
      ulStyle = WinQueryWindowULong (hwnd, QWL_STYLE);
      pswp [i].hwnd = hwnd;

      if (bForArrange)
         {
         if (ulStyle & WS_MAXIMIZED)
            {
            /* restore maximized windows to their previous state */
            WinSetWindowPos (hwnd, NULL, 0, 0, 0, 0, SWP_RESTORE);
            }
         }
      else
         {
         WinQueryWindowPos (hwnd, &pswp [i]);
         }
      i++;
      }

   WinEndEnumWindows (henum);

   return pswp;
   }


