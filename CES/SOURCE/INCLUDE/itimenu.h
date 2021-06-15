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
 * ItiMenu.h
 * Mark Hampton
 * This module is part of DS/Shell (CS/Base)
 *
 * This module provides an interface to the menu system of OS/2.
 *
 * This module also takes care of updating the window menu, and 
 * performing the activation of windows on that menu.  This module 
 * also provides commands to arrange the windows in a tiled or 
 * cascaded fashon.
 *
 * This module references the following DLLs:
 *    ITIGLOB.DLL
 *    ITIERROR.DLL
 *    ITICRT.DLL
 *    ITIBASE.DLL
 *    ITIWIN.DLL
 *
 * Functions:
 *
 * Menu manipulation:
 *    ItiMenuDeleteMenuItem
 *    ItiMenuInsertMenuSeparator
 *    ItiMenuInsertMenuItem
 *    ItiMenuCheckMenuItem
 *    ItiMenuGrayMenuItem
 *    ItiMenuQuerySubmenu
 *
 * Menu commands:
 *    ItiMenuArrangeWindows           - For arrange tiled and cascade
 *    ItiMenuInitWindowMenu           - Initialize window titles on menu
 *    ItiMenuActivateWindowFromMenu   - Activates windoe from menu
 *    ItiMenuOpenCatalog              - Opens a catalog window
 *
 * General:
 *    ItiMenuGetChildSwp
 *    ItiMenuQueryChildWindowCount
 *    ItiMenuGetWindowTitles
 *    ItiMenuSortWindowTitles
 *    ItiMenuFreeWindowTitles
 */


#if !defined (ITIMENU_INCLUDED)
#define ITIMENU_INCLUDED

/* addendum to sys menu */
#define IDM_NORMALSIZE                        999
#define IDM_EXIT                             1028

#define IDM_FILE_PRINT_FIRST                 1900
#define IDM_FILE_PRINT_LAST                  1949
#define IDM_FILE_OPEN_CATALOG_FIRST          1950
#define IDM_FILE_OPEN_CATALOG_LAST           1999


/* define top level menu and sub menu IDs */



/* define the IDs for the File Menu */ 
#define IDM_FILE_MENU                        1000

#define IDM_FILE_MENU_FIRST                  1001
#define IDM_FILE_MENU_LAST                   1099

#define IDM_OPEN_CATALOG_MENU                1001

#define IDM_CLOSE                            1014
#define IDM_PRINT_MENU                       1015
#define IDM_PRINTER_SETUP                    1018
#define IDM_IMPORT                           1019
#define IDM_EXPORT                           1020
#define IDM_RUN_MENU                         1021
#define IDM_RUN_ITEM_BROWSER                 1022
#define IDM_RUN_PROPOSAL_BROWSER             1023
#define IDM_RUN_FACILITY_BROWSER             1024
#define IDM_RUN_SQL_BROWSER                  1025
#define IDM_RUN_OTHER                        1026
#define IDM_PROCESS_STATUS                   1027


/* define the IDs for the Edit Menu */ 
#define IDM_EDIT_MENU                        1100

#define IDM_EDIT_MENU_FIRST                  1101
#define IDM_EDIT_MENU_LAST                   1199

#define IDM_ADD                              1101
#define IDM_CHANGE                           1102
#define IDM_DELETE                           1103
#define IDM_CUT                              1104
#define IDM_COPY                             1105
#define IDM_PASTE                            1106

/* id of the first user supplied menu item on the edit menu */
#define IDM_EDIT_USER_FIRST                  1150

/* id of the last user supplied menu item on the edit menu */
#define IDM_EDIT_USER_LAST                   1170


/* define the IDs for the View Menu */ 

#define IDM_VIEW_MENU                        1200

#define IDM_VIEW_MENU_FIRST                  1201
#define IDM_VIEW_MENU_LAST                   1299

#define IDM_PARENT                           1201
/* count of static items on the view menu */
#define VIEW_MENU_COUNT                      1

/* id of the first user supplied menu item on the view menu */
#define IDM_VIEW_USER_FIRST                  1250

/* id of the last user supplied menu item on the view menu */
#define IDM_VIEW_USER_LAST                   1270

/* define the IDs for the Utilities Menu */

#define IDM_UTILITIES_MENU                   1300

#define IDM_UTILITIES_MENU_FIRST             1301
#define IDM_UTILITIES_MENU_LAST              1399

#define IDM_FIND                             1301
#define IDM_SORT                             1302
#define IDM_SET_PREFERENCES                  1303
#define IDM_CHANGE_PASSWORD                  1304
#define IDM_SHOW_USERS                       1305
#define IDM_SEND_MESSAGE                     1306
#define IDM_INSTALL                          1307
#define IDM_DENY_LOGON                       1308
#define IDM_GENERATE_SUPPORT_REQUEST         1309

#define IDM_OPEN_SPECYEAR_MENU               1310

#define IDM_SPECYR_FIRST          1320
#define IDM_SPECYR_LAST           1350

/* define the IDs for the Window Menu */ 
#define IDM_WINDOW_MENU                      1400

#define IDM_WINDOW_MENU_FIRST                1401
#define IDM_WINDOW_MENU_LAST                 1499

#define IDM_ARRANGE_CASCADED                 1401
#define IDM_ARRANGE_TILED                    1402
#define IDM_WINDOW_SEPARATOR                 1403
#define IDM_WINDOW_LIST_FIRST                1404
#define IDM_WINDOW_LIST_LAST                 1412


/* define menu IDs for the standard help commands */
//#define IDM_HELP_MENU                        1800

//#define IDM_HELP_MENU_FIRST                  1801
//#define IDM_HELP_MENU_LAST                   1899
//
//#define IDM_INDEX                            1801
//#define IDM_KEYBOARD                         1802     
//#define IDM_MOUSE                            1803
//#define IDM_COMMANDS                         1804
//#define IDM_PROCEDURES                       1805
//#define IDM_GLOSSARY                         1806
//#define IDM_USING_HELP                       1807
//
//#define IDM_ABOUT                            1808

#define IDM_HELPMENU                         1800

#define IDM_HELP_MENU_FIRST                  1801
#define IDM_HELP_MENU_LAST                   1899

#define IDM_GENERALHELP                      1801
#define IDM_KEYBOARD                         1802
#define IDM_MOUSE                            1803
#define IDM_COMMANDS                         1804
#define IDM_PROCEDURES                       1805
#define IDM_WINDOWS                          1806
#define IDM_GLOSSARY                         1807
#define IDM_INDEX                            1808
#define IDM_ABOUT                            1809


#ifdef DEBUG
/* define the IDs for the Debug Menu */

#define IDM_DEBUG_MENU                       1500

#define IDM_DEBUG_MENU_FIRST                 1501
#define IDM_DEBUG_MENU_LAST                  1599

#define IDM_DIE_SCUM                         1501
#define IDM_DEBUG_BREAK                      1502
#define IDM_STACK_INFO                       1503
#define IDM_STACK_CLEAR                      1504
#define IDM_HIDE_DEBUG_STUFF                 1505


#define MAX_YEARS  50
#define ENTRYLENGTH 6

USHORT usNumberOfSpecYrs;
CHAR   aszSpecYr [MAX_YEARS][ENTRYLENGTH + 1];
USHORT usSpecYrIndex;

#define SPYRQUERY "SELECT DISTINCT SpecYear FROM StandardItem ORDER BY SpecYear DESC "

#endif



#if !defined (RC_INVOKED)

/* WINDOW MENU COUNT is the count of static menu items on the window menu */
#define WINDOW_MENU_COUNT                       2
#define NUMBER_OF_WINDOW_LISTS  (IDM_WINDOW_LIST_LAST - IDM_WINDOW_LIST_FIRST + 1)

/*
 * ItiMenuInitialize initializes the ItiMenu module.
 *
 * Parameters: pszAppName     The application's name.  This value is used
 *                            to search the Application table, therefore 
 *                            the name passed to this function must be the
 *                            same as what is in the Application table, 
 *                            column ApplicationID.
 *
 *             hwndMainMenu   The handle to the main menu.
 *
 */

extern BOOL EXPORT ItiMenuInitialize (HWND hwndMainMenu);




/*
 * ItiMenuArrangeWindows arranges the immediate child windows of
 * hwndApplication, according to the value of usStyle.
 *
 * Parameters: hwndApplication:  The parent of the windows to be arranged.
 *
 *             usStyle:          Tells how to arrange the windows. Valid
 *                               values are:
 *
 *                               AW_TILED       Arrange the windows in a
 *                                              tiled manner.
 *
 *                               AW_CASCADED    Arrange the windows in a
 *                                              cascaded manner.
 *
 * Return Value:
 *   TRUE if successful
 *   FALSE otherwise
 *
 * Comments:
 */

#define AW_TILED     1
#define AW_CASCADED  2

extern BOOL EXPORT ItiMenuArrangeWindows (HWND    hwndApplication, 
                                         USHORT  usStyle);





/*
 * ItiMenuGetChildSwp gets the SWPs for the children of hwndParent.
 *
 * Parameters: hwndParent:    The parent window.
 *
 *             pusNumWindows: A pointer to a USHORT that will contain
 *                            the number of elements in the returned
 *                            SWP.  This parameter may not be NULL.
 *
 *             bForArrange:   If TRUE, then the only the hwnd field
 *                            of the SWP array is filled in, and all
 *                            maximized windows are restored.  If FALSE
 *                            then the full SWP structure is filled in.
 *
 * Return Value: A pointer to an array of SWPs.  The number of SWPs
 * is stored in *pusNumWindows
 *
 * Comments: The pointer to the array of SWPs was allocated with
 * ItiMemAllocSeg.  The caller must call ItiMemFreeSeg when finished 
 * with the SWPs in order to free up the memory.
 */


extern PSWP EXPORT ItiMenuGetChildSwp (HWND    hwndParent, 
                                      PUSHORT pusNumWindows,
                                      BOOL    bForArrange);






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

extern USHORT EXPORT ItiMenuQueryChildWindowCount (HWND     hwndParent,
                                                  USHORT   usRecurse);





/*
 * ItiMenuDeleteMenuItem deletes the specified menu item from the menu
 * hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             usCommand:        The command ID of the item to delete.
 *
 * Return value: The count of remaining items for the menu.
 *
 * Comments:
 */


extern USHORT EXPORT ItiMenuDeleteMenuItem (HWND   hwndMenu,
                                           USHORT usCommand);








/*
 * ItiMenuInsertMenuSeparator inserts a menu separator at the end of menu
 * hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             usCommand:        The command ID of the menu separator to
 *                               insert.
 *
 * Return value: The position of the inserted menu separator, or either
 * MIT_MEMERROR if memory cannot be allocated for the separator, or
 * MIT_ERROR if any other error occured.
 *
 * Comments:
 */


extern USHORT EXPORT ItiMenuInsertMenuSeparator (HWND   hwndMenu,
                                                USHORT usCommand);




/*
 * ItiMenuInsertMenuItem inserts a menu item at the end of menu hwndMenu.
 *
 * Parameters: hwndMenu:         The menu to modify.
 *
 *             pszMenuText:      The text for the menu item.         
 *
 *             usCommand:        The command ID of the menu item to
 *                               insert.
 *
 * Return value: The position of the inserted menu item, or either
 * MIT_MEMERROR if memory cannot be allocated for the item, or
 * MIT_ERROR if any other error occured.
 *
 * Comments:
 */


extern USHORT EXPORT ItiMenuInsertMenuItem (HWND   hwndMenu,
                                           PSZ    pszMenuText,
                                           USHORT usCommand);





/*
 * ItiMenuCheckMenuItem checks the specified menu item on the menu hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             usCommand:        The command ID of the item to check.
 *
 *             bCheck:           If TRUE, the item is checked.  If FALSE,
 *                               any checks are removed.
 *
 * Return value: TRUE if the menu item was succesfully change, FALSE
 * if not.
 *
 * Comments:
 */


extern BOOL EXPORT ItiMenuCheckMenuItem (HWND   hwndMenu,
                                        USHORT usCommand,
                                        BOOL   bCheck);





/*
 * ItiMenuGrayMenuItem grays the specified menu item on the menu hwndMenu.
 *
 * Parameters: hwndMenu:         The window to modify.
 *
 *             usCommand:        The command ID of the item to check.
 *
 *             bGray:            If TRUE, the item is grayed.  If FALSE,
 *                               the item is enabled.
 *
 * Return value: TRUE if the menu item was succesfully change, FALSE
 * if not.
 *
 * Comments:
 */

extern VOID EXPORT ItiMenuGrayMenuItem (HWND    hwndMenu,
                                       USHORT  usCommand,
                                       BOOL    bGray);



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
 */


extern HWND EXPORT ItiMenuQuerySubmenu (HWND    hwndMenu,
                                       USHORT  usCommand);













typedef struct
   {
   HWND  hwnd;
   PSZ   pszTitle;
   } WINDOWTITLELIST;

typedef WINDOWTITLELIST FAR *PWINDOWTITLELIST;


typedef struct
   {
   USHORT            usNumWindows;
   PWINDOWTITLELIST  pwtl;
   } WINDOWTITLES;


typedef WINDOWTITLES FAR *PWINDOWTITLES;






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

PWINDOWTITLES EXPORT ItiMenuGetWindowTitles (HWND hwndParent);



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

VOID EXPORT ItiMenuSortWindowTitles (PWINDOWTITLES pwt);







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


VOID EXPORT ItiMenuFreeWindowTitles (PWINDOWTITLES pwt);







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

VOID EXPORT ItiMenuInitWindowMenu (HWND hwndAppClient, 
                                  HWND hwndMenu);



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

VOID EXPORT ItiMenuActivateWindowFromMenu (HWND   hwndAppClient, 
                                          USHORT idWindow);




/*
 * call this function whenever the application receives the WM_INITMENUPOPUP
 * message for the Open Catalog menu.
 */

extern BOOL EXPORT ItiMenuInitOpenCatalogMenuPopup (HWND hwndOpenCatalogMenu);

extern BOOL EXPORT ItiMenuInitOpenSpecYrMenuPopup (HWND hwndOpenSpecYrMenu);


/*
 * call this function whenever the application receives the WM_INITMENUPOPUP
 * message for the Print menu.
 */

extern BOOL EXPORT ItiMenuInitPrintMenuPopup (HWND hwndPrintMenu);





/*
 * ItiMenuOpenCatalog opens the window specified by usCommand.  usCommand
 * is SHORT1FROMMP (mp1) of the WM_COMMAND message.
 *
 * Parameters: hwndAppClient  A handle to the application's client window.
 *
 *             usCommand      SHORT1FROMMP (mp1) from the WM_COMMAND 
 *                            message. 
 *
 * Return Value: A handle to the client area of opened catalog window, 
 * or NULL on error.
 *
 * Comments: Possible errors include:
 *          usCommand out of range (IDM_FILE_OPEN_CATALOG_FIST through
 *          IDM_FILE_OPEN_CATALOG_LAST)
 *
 *          Not enough memory to create window.
 */

extern HWND EXPORT ItiMenuOpenCatalog (HWND    hwndAppClient,
                                       USHORT  usCommand);

extern BOOL EXPORT ItiMenuSetSpecYr (USHORT usCommand);

/*
 * ItiMenuPrint opens the window specified by usCommand.  usCommand
 * is SHORT1FROMMP (mp1) of the WM_COMMAND message.
 *
 * Parameters: hwndAppClient  A handle to the application's client window.
 *
 *             usCommand      SHORT1FROMMP (mp1) from the WM_COMMAND 
 *                            message. 
 *
 * Return Value: nothing.
 */

extern void EXPORT ItiMenuPrint (HWND    hwndAppClient,
                                 USHORT  usCommand);



extern USHORT EXPORT ItiMenuDoFind (HWND hwndList);

extern USHORT EXPORT ItiMenuDoSort (HWND hwndList);

extern USHORT EXPORT ItiMenuDoSupport (HWND hwndList);

extern USHORT EXPORT ItiMenuDoPrinterSetup (HWND hwndList);

extern void EXPORT ItiMenuChangePassword (HWND hwnd);
extern void EXPORT ItiMenuShowUsers (HWND hwnd);
extern void EXPORT ItiMenuDenyLogon (HWND hwnd);
extern void EXPORT ItiMenuInstall (HWND hwnd);
extern void EXPORT ItiMenuSetPreferences (HWND hwnd);
extern void EXPORT ItiMenuSendMessage (HWND hwnd);
extern USHORT EXPORT ItiMenuDoImport (HWND hwnd);
extern USHORT EXPORT ItiMenuDoExport (HWND hwnd);
extern USHORT EXPORT ItiMenuDaDoRunRunRun (HWND hwnd);


extern void EXPORT ItiMenuShowUsers (HWND hwnd);

#endif /* #if !defined (RC_INVOKED) */

#endif /* #if !defined (ITIMENU_INCLUDED) */

