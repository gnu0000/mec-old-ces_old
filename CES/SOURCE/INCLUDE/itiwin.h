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
 * window.h
 *
 */

#if !defined (WINDOW_INCLUDED)
#define WINDOW_INCLUDED


/*
 * General String Sizes
 */
#define SHORTSTRLEN        40
#define MEDSTRLEN          80
#define STRLEN             80
#define LONGSTRLEN         255
#define BIGSTRLEN          2048

/*
 * Actual class strings
 */
extern char ITIWND_STATICCLASS[];
extern char ITIWND_LISTCLASS[];
//extern char ITIWND_TEXTBOXCLASS[];

/*
 * WC type for list windows
 * All children of static windows have a WC type. Standard Child
 * windows have standard WC types (EG. WC_BUTTON) ITIWC_LIST is
 * the WC type for the list window
 */
#define ITIWC_LIST         100UL

/* This constant is used with add and change ID's for list windows.
 * It means to use the parents add or change dialog box.
 */
#define ITIWND_PARENT      0xFFFF


/* --- MESSAGES TO LIST AND STATIC WINDOWS ---
 */
#define WM_ITIWNDQUERY          (WNDMSG_START + 0) /* ask for result     */
#define WM_ITIWNDSET            (WNDMSG_START + 1) /* set/perform action */
   // described later


#define WM_INITQUERY            (WNDMSG_START + 2) /* start its query    */
   //
   //mp1  = parents hbuf    ||  mp1  = 0xFFFFFFFF  (this only works for lw's)
   //mp2l = parents row #   OR  mp2  = Query String To Initiate
   //mp2h = bNoInitKids     ||
   //
   //                        
   //                        
#define WM_CHANGEQUERY           (WNDMSG_START + 3) /* start/change query */
   //mp1  = parents hbuf  
   //mp2l = parents row#  
   //mp2h = TRUE = init childrens querys | FALSE = dont start kids querys
   //-----------------or---------------------
   //mp1  = 0xFFFFFFFF  (this only works for lw's)
   //mp2  = Query String To Initiate
   //-----------------or---------------------
   //mp1 = 0xFFFFFFFF  requery with the same query you already have
   //mp2 = 0xFFFFFFFF

/*--- MESSAGES FROM LIST WINDOWS ---
 */
#define WM_ITILWQBUTTON         (WNDMSG_START + 20)
#define WM_ITILWSCROLL          (WNDMSG_START + 21)
#define WM_ITILWMOUSE           (WNDMSG_START + 22)

#define WM_ACTIVEROWCHANGED     (WNDMSG_START + 26)

/*--- MESAGES TO/FROM DIALOG WINDOWS ---*/
#define WM_QUERYDONE            (WNDMSG_START + 23)
#define WM_GETDLGDAT            (WNDMSG_START + 24)
#define WM_ITIWNDRESETFOCUS     (WNDMSG_START + 25)
#define WM_NEWKEY               (WNDMSG_START + 27)
#define WM_CHILDQUERYDONE       (WNDMSG_START + 28)


#define WM_ITICUT               (WNDMSG_START + 29)
#define WM_ITICOPY              (WNDMSG_START + 30)
#define WM_ITIPASTE             (WNDMSG_START + 31)

#define WM_ITICALC              (WNDMSG_START + 32)



/*
 * WM_NEWKEY is sent when an insert is done to a table, and a unique
 * system generated key is retrieved.  mp1 = pszTableName, mp2 = pszKey.
 * This message is sent to the dialog box window.  If the dialog box
 * procedure processes this message, it should return 0.
 *
 * NOTE: mp1 and mp2 are freed immediately after the message is received.
 */




/*
 * These command constants are used with the WM_ITIWNDQUERY and
 * WM_ITIWNDSET messages. Note that not all commands work with 
 * all messages and window types
 */
#define ITIWND_NUMROWS            1
#define ITIWND_ACTIVE             2
#define ITIWND_SCROLLTYPE         3
#define ITIWND_COLOR              4
#define ITIWND_LABELEDT           5
#define ITIWND_DATAEDT            6
#define ITIWND_LABEL              7
#define ITIWND_BUFFER             8
#define ITIWND_BUTTON             9
#define ITIWND_TXTPROC            10
#define ITIWND_INITDRAWPROC       11
#define ITIWND_DRAWPROC           12
#define ITIWND_NUMSELECTIONS      13
#define ITIWND_SELECTION          14
#define ITIWND_NUMCOLS            15
#define ITIWND_LABELCOLS          16
#define ITIWND_HEAP               17
#define ITIWND_REDRAW             18
#define ITIWND_MAXSIZE            19
#define ITIWND_VIEWROW            20
#define ITIWND_SCROLL             21
#define ITIWND_ROWCHANGED         22
#define ITIWND_ROWINSERTED        23
#define ITIWND_ROWDELETED         24
#define ITIWND_NUMKIDS            25
#define ITIWND_KIDTYPE            26
#define ITIWND_ID                 27
#define ITIWND_ADDID              28
#define ITIWND_CHANGEID           29
#define ITIWND_DATA               30
#define ITIWND_OWNERWND           31

#define ITIWND_PSTRUCT             32
#define ITIWND_OWNERLISTBUFFER    33
#define ITIWND_OWNERSTATICBUFFER  34

#define ITIWND_TYPE               35
#define ITIWND_LABELHIDDEN        36
#define ITIWND_DATAHIDDEN         37
#define ITIWND_LOCKROW            38

#define ITIWND_FLAGS              39
#define ITIWND_KIDID              40
#define ITIWND_KID                41

/*
 * These constants are used as return values by the 
 *  WM_ITILWMOUSE message. 
 */
#define LW_DATAAREA               1
#define LW_LABELAREA              2
#define LW_DATA                   3
#define LW_LABEL                  4
#define LW_OTHER                  5



/*
 * These constants are for setting custom attributes of labels
 *  and data items in static and list windows
 */
#define ITIWND_HIDE               0x80000000UL
#define ITIWND_WORDBREAK          0x40000000UL


/*
 * --- LISTWIN STYLES ---
 * 
 *  These are the styles a listwin may have
 *  They are bitwise or'ed in the ufOptions parameter
 *  of the LWINIT struct. see LWINIT for description.
 */
#define LWS_HSCROLL        0x0001
#define LWS_VSPLIT         0x0003
#define LWS_VSCROLL        0x0004
#define LWS_HSPLIT         0x000C
#define LWS_QBUTTON        0x0010
#define LWS_LABEL          0x0020
#define LWS_SELECT         0x0040
#define LWS_MULTISELECT    0x00C0
#define LWS_SPLITATTOP     0x0100
//#define LWS_CLIENTBORDER   0x0200
//#define LWS_FULLBORDER     0x0400
#define LWS_BORDER         0x0400
#define LWS_UNDERLINELABEL 0x0800


/*
 * --- LISTWIN DEFAULT COLORS ---
 */
#define LWCLR_DEFCLIENT       CLR_WHITE
#define LWCLR_DEFLABEL        CLR_PALEGRAY
#define LWCLR_DEFSELECT       CLR_CYAN
#define LWCLR_DEFACTIVE       CLR_BLACK
#define LWCLR_DEFSPLITBORDER  SYSCLR_WINDOWFRAME
#define LWCLR_DEFSPLITMIN     SYSCLR_BACKGROUND
#define LWCLR_DEFSPLITMAX     SYSCLR_BACKGROUND


/*
 * --- VALID ColorTypes FOR ITIWND_COLOR ---
 */ 
#define LWC_RESET         0
#define LWC_CLIENT        1
#define LWC_LABEL         2
#define LWC_SELECT        3
#define LWC_ACTIVE        4
#define LWC_SPLITBORDER   5
#define LWC_SPLITMIN      6
#define LWC_SPLITMAX      7

#define ITI_DEFAULTWND    0

void EXPORT ItiWndPrintBetaMessage (HWND hwnd);


/*
 * This procedure registers the classes for
 * the static and list windows. It returns
 * OK or NOT_OK
 */
extern USHORT EXPORT ItiWndRegister (HAB hab);


#if !defined PFNTXT_DEFINED
#define PFNTXT_DEFINED
typedef int (EXPORT *PFNTXT) (HWND hwnd, USHORT uRow, USHORT uCol, PSZ *ppszText);
#endif













/* This proc will create a window af any registered type
 * STATIC, LIST, and DIALOG.
 * Stataic windows automatically create thier kids
 * list windows are not normally created with this proc
 */
extern HWND EXPORT ItiWndBuildWindow (HWND   hwnd, 
                                      USHORT uId);


extern HWND EXPORT ItiWndBuildStaticWindow (HWND   hwnd,
                                            USHORT uId);

extern ULONG EXPORT ItiWndBuildDialogWindow (HWND   hwndOwner, 
                                              USHORT uId);

extern HWND EXPORT ItiWndBuildListWindow (HWND   hwnd,
                                          USHORT uId,
                                          RECTL  rcl,
                                          USHORT uCoordType);


extern void EXPORT ItiWndSetDlgMode (USHORT uMode);

extern USHORT EXPORT ItiWndGetDlgMode (void);
                                          

extern void EXPORT ItiWndDoDialog (HWND   hwnd, 
                                   USHORT uWinId, 
                                   BOOL   bAdd);





/*
 * this function is called by a dialog box to create a new dialog box
 * or anyplace where a dlg should be created where its ID
 * is to be specified rather than looked up in the metadata
 */
extern void EXPORT ItiWndDoNextDialog (HWND   hwnd,      // current window
                                USHORT uParentId, // 0 - hwnd is parent
                                                  // # - this kid id is parent
                                USHORT uNewId,    // dialog id
                                USHORT uMode);       // new dlg mode



/*
 * This procedure will do an add, change or delete for a specified 
 * window. pszQuery is assumed to be the query with the replaceable
 * parameters not replaced yet. If the uMode = ITI_CHANGE, the lock
 * query will be obtained from the metadata for hwnd. If the query
 * contains '@' characters, make sure hwnd is a dialog window.  If
 * you want the querys to be obtained from the metadata automatically,
 * use ItiWndAutoModifyDb.
 *
 * if uMode = ITI_DELETE, the verify msg box should be done before
 * calling this fn.
 *
 */
USHORT EXPORT ItiWndModifyDb (HWND hwnd, PSZ pszQuery, USHORT uMode);





/*
 * This procedure will do an add, change or delete for a specified 
 * window. The querys are obtained from the metadata of the correct
 * type. The query ID's are the same as the ID of hwnd
 * Use ItiWndModifyDb if you already have the query string
 *
 */
extern USHORT EXPORT ItiWndAutoModifyDb (HWND hwnd, USHORT uMode);


/*                  ---- ItiWndAddHook ----
 *
 * This function provides a way to hook ENTRYFIELDS and COMBOBOXES
 * in dialog boxes.  This function must be called to do this because
 * of the nonstandard way these controls are sometimes used.
 *
 * return values:
 *    0 - OK
 *    1 - incorrect ctl type (EDIT and COMBO only)
 *    2 - unable to hook
 *    3 - Incorrect mood setting of the METAPHYSICS environment variable
 *
 *    pfnNextProc - next window procedure to call in message chain
 *        This value should be stored in a static and called in the
 *        default case for the window proc you provide.
 *
 *    If the hwnd is a COMBOBOX control:
 *       1> ALL COMBOBOXES ARE HOOKED.
 *          (Check the hwnd in pfnHookProc to make sure its the one you want)
 *       2> All hooking is done after my validation hooks.
 *       3> subsequent calls are added to the top of the chain (after mine).
 *
 *    If the hwnd is a ENTRYFIELD control:
 *       1> only the specified entryfield is hooked.
 *       2> subsequent calls are added to the top of the chain (after mine).
 *    
 *    If you want to hook a combobox before my validation hook:
 *       1> enumerate to find the ENTRYFIELD associated with the combobox.
 *       2> call ItiWndAddHook with that ENTRYFIELD hwnd.
 *          This will only hook that one COMBOBOX.
 *
 */
USHORT EXPORT ItiWndAddHook (HWND hwndCtl,
                             PFNWP pfnHookProc,
                             PFNWP *pfnNextProc);






extern USHORT EXPORT ItiWndDoDelete (HWND   hwndStatic,
                                     USHORT usChildId);

extern BOOL EXPORT ItiWndQueryActive (HWND   hwndStatic,
                                      USHORT uListId,
                                      USHORT *uActive);

extern USHORT EXPORT ItiWndQuerySelection (HWND   hwndFrame, 
                                    USHORT uStart);

extern USHORT EXPORT ItiWndQueryBackSelection (HWND hwndFrame, USHORT uStart);

/*
 * This procedure creates control windows.
 * such as buttons.
 */
extern HWND EXPORT ItiWndBuildOtherWindow (HWND   hwnd,
                                           USHORT uId,
                                           ULONG  ulKind,
                                           RECTL  rcl,
                                           USHORT uCoordType);


/*
 * This function will activate a row in a dialogs listwindow
 * or combobox
 *
 * for rigth now
 * if the hwnd is another dialog window, 
 * the query to be used will be the main query for that dialog window only
 *
 * hwndDlg  - current dialog window
 * uCtl     - ctl id to select row of
 * uCtlCol  - ctl buffer col to match              0=col in metadata
 * hwnd     - window with hbuf to match on         Null=parents window
 * uBuffCol - col in hbuf to match on              0=same as uctlcol
 *
 * returns:  0: ok
 *           1: No match
 *           2: bad dlg mode         only works in change mode
 *           3: bad control          puts up error
 *           4: bad matching hwnd    puts up error
 *
 */

extern USHORT EXPORT ItiWndActivateRow (HWND   hwndDlg,
                                        USHORT uCtl,     
                                        USHORT uCtlCol,  
                                        HWND   hwnd,     
                                        USHORT uBuffCol);


/* This fn determines if the active row has changed since the call to
 * ItiWndActivateRow was called with the same parameter values
 *
 * return: FALSE : no change
 *         TRUE  : change made
 */
extern BOOL EXPORT ItiWndActiveRowChanged (HWND   hwndDlg,
                                           USHORT uCtl,
                                           USHORT uCtlCol,
                                           HWND   hwnd,
                                           USHORT uBuffCol);



/*
 * With this fn you can tell if the user has changed the text in an entry
 * field.  This fn compares the text in the field with the buffer text
 * that was initially put in the field and returns:
 *   FALSE - no change or an error
 *   TRUE  - change
 */
BOOL EXPORT ItiWndEntryChanged (HWND hwndDlg, USHORT uCtl);



/*--- obselete---*/
extern USHORT EXPORT ItiWndSelectControlRow (HWND   hwndDlg,   
                                      USHORT uCtl,      
                                      USHORT uCtlCol,   
                                      USHORT uQueryId,  
                                      USHORT uQueryCol);




/*
 * This is the default procedure used by list windows for getting 
 * data elements.  List windows do not need a window procedure.
 * as you can see, this proc does not need to do much.
 *
 */
extern int EXPORT ItiWndDefListWndTxtProc (HWND hwnd, USHORT uRow, USHORT uCol, PSZ *ppszTxt);


/*
 * This procedure is the default window proc for static windows.
 * It in turn calls WNDefWndProc.
 * If you further subclass a static window, this should be the
 * procedure you call in the default case.
 */
extern WNDPROC EXPORT ItiWndDefStaticWndProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);



/*
 * This procedure is the default window proc for dialog windows.
 * this should be the procedure you call in the default case
 * of your dialog window procdure
 */
extern WNDPROC EXPORT ItiWndDefDlgProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);


/*
 * ItiWinAppDefWindowProc is to be called by the application's
 * main window as the default message processing handler.
 */
extern MRESULT EXPORT ItiWndAppDefWindowProc (HWND   hwnd, 
                                              USHORT usMessage,
                                              MPARAM mp1,
                                              MPARAM mp2);

extern WNDPROC EXPORT ItiWndDefWndProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);

extern USHORT EXPORT ItiWndQueryWndType (USHORT uId);


#define QW(hwnd,mp1L,mp1H,mp2L,mp2H)  WinSendMsg(hwnd,WM_ITIWNDQUERY,MPFROM2SHORT((mp1L),(mp1H)),MPFROM2SHORT ((mp2L),(mp2H)))
#define SW(hwnd,mp1L,mp1H,mp2L,mp2H)  WinSendMsg(hwnd,WM_ITIWNDSET,MPFROM2SHORT((mp1L),(mp1H)),MPFROM2SHORT ((mp2L),(mp2H)))
#define SWL(hwnd,mp1L,mp1H,mp2)       WinSendMsg(hwnd,WM_ITIWNDSET,MPFROM2SHORT((mp1L),(mp1H)),(MPARAM)(mp2))


/**********************************************************************/
/*****************  static window stuff  ******************************/
/**********************************************************************/

/*   ------- WM_ITIWNDQUERY MESSAGE FOR STATIC WINDOWS --------
-----------------------------------------------------------------------------
|            --MP1--              | --RETURN--  |       --COMMENT--         |
| L                    H          | L          H|                           |
|---------------------------------+-------------+---------------------------|
| ITIWND_ACTIVE        0          | 0           | active row # (always 0)   |
| ITIWND_ID            0          | uId         | window ID                 |
| ITIWND_NUMCOLS       0          | uNumDatCol  | # data elements           |
| ITIWND_LABELCOLS     0          | uNumLabCol  | # labels                  |
| ITIWND_COLOR         uColorType |    lColor   | specified color           |
| ITIWND_LABELEDT      uCol       |    PEDT     | EDT for a label           |
| ITIWND_DATAEDT       uCol       |    PEDT     | EDT for a data element    |
| ITIWND_LABEL         uCol       |    PSZ      | label string              |
| ITIWND_BUFFER        0          |    HBUF     | windows buffer            |
| ITIWND_HEAP          0          |    HHEAP    | windows heap              |
| ITIWND_NUMKIDS       0          |    uKids    | # kids                    |
| ITIWND_KID           uKid       |    HWND     | hwnd of kid               |
| ITIWND_KIDTYPE       uKid       |    ulKind   | kind of kid               |
| ITIWND_KIDID         uKid       | uId         | ID for specified kid      |
| ITIWND_ADDID         0 or uKidId| uId         | ID for add dlg            |
| ITIWND_CHANGEID      0 or uKidId| uId         | ID for change dlg         |
| ITIWND_TYPE          0          | ITI_STATIC  | identifies win type       |
| ITIWND_OWNERWND      N          |     HWND    | N # extra levels          |
| ITIWND_LABELHIDDEN   uCol       | bHidden     | Tels if label is hidden   |
| ITIWND_DATAHIDDEN    uCol       | bHidden     | Tels if data is hidden    |
|                        -L-MP2-H-|             |                           |
| ITIWND_DATA          0      uCol| pszDat      | BufferData                |
| ITIWND_FLAGS         0          | uFlags      | Creation Flags            |
-----------------------------------------------------------------------------
*/


/*   ------- WM_ITIWNDSET MESSAGE FOR STATIC WINDOWS --------
-----------------------------------------------------------------------------
|            --MP1--              |   --MP2--   |       --COMMENT--         |
| L                    H          | L          H|                           |
|---------------------------------+-------------+---------------------------|
| ITIWND_COLOR         uColorType |    lColor   | new color value           |
| ITIWND_LABELEDT      uCol       |    PEDT     | new EDT for label         |
| ITIWND_DATAEDT       uCol       |    PEDT     | new EDT for data          |
| ITIWND_LABEL         uCol       |    PSZ      | new label string          |
| ITIWND_BUFFER        0          |    HBUF     | new buffer handle         |
| ITIWND_MAXSIZE       0          | 0          0| max the window to parent  |
| ITIWND_ADDID         uNewId     | 0          0| new ID for add dlg        |
| ITIWND_CHANGEID      uNewId     | 0          0| new ID for change dlg     |
| ITIWND_LABELHIDDEN   uCol       | bHide       | sets hide state for label |
| ITIWND_DATAHIDDEN    uCol       | bHide       | sets hide state for data  |
-----------------------------------------------------------------------------
*/


/**********************************************************************/
/*****************  list   window stuff  ******************************/
/**********************************************************************/


/*   ------- WM_ITIWNDQUERY MESSAGE FOR LIST WINDOWS --------
-----------------------------------------------------------------------------
|            --MP1--              | --RETURN--  |       --COMMENT--         |
| L                    H          | L          H|                           |
|---------------------------------+-------------+---------------------------|
| ITIWND_NUMROWS       0          | uNumRows    | # rows in client          |
| ITIWND_ID            0          | uId         | window ID                 |
| ITIWND_ACTIVE        0          | uActive     | active row # (-1 if none) |
| ITIWND_SCROLLTYPE    0          | uScrollType | SB_SLIDERPOSITION or      |
|                                 |             | SB_SLIDERTRACK            |
| ITIWND_COLOR         uColorType |     lColor  | specified color val       |
| ITIWND_LABELEDT      uCol       |     PEDT    | NULL if bad col #         |
| ITIWND_DATAEDT       uCol       |     PEDT    | NULL if bad col #         |
| ITIWND_LABEL         uCol       |     psz     | NULL if bad col #         |
| ITIWND_BUFFER        0          |     HBUF    | Buff handle               |
| ITIWND_BUTTON        0          | uButtonId   | scrollbar button VALUE    |
| ITIWND_TXTPROC       0          |     pfn     | ptr to txt proc           |
| ITIWND_INITDRAWPROC  0          |     pfn     | ptr to draw init proc     |
| ITIWND_DRAWPROC      0          |     pfn     | ptr to draw proc          |
| ITIWND_SELECTION     uStartlook | uSel        | first sel starting at uSta|
| ITIWND_NUMCOLS       0          | uNumCol     | # data columns            |
| ITIWND_LABELCOLS     0          | uNumCol     | # label columns           |
| ITIWND_HEAP          0          |     HHEAP   | handle to heap            |
| ITIWND_ADDID         0          | uId         | ID for add dlg            |
| ITIWND_CHANGEID      0          | uId         | ID for change dlg         |
| ITIWND_OWNERWND:     0          |     HWND    |                           |
| ITIWND_TYPE          0          | ITI_LIST    | identifies win type       |
| ITIWND_LABELHIDDEN   uCol       | bHidden     | Tels if label is hidden   |
| ITIWND_DATAHIDDEN    uCol       | bHidden     | Tels if data is hidden    |
|                        -L-MP2-H-|             |                           |
| ITIWND_DATA          0 uRow,uCol| pszDat      | BufferData                |
-----------------------------------------------------------------------------
*/      



/*   ------- WM_ITIWNDSET MESSAGE FOR LIST WINDOWS --------
-----------------------------------------------------------------------------
|            --MP1--              |   --MP2--   |       --COMMENT--         |
| L                    H          | L          H|                           |
|---------------------------------+-------------+---------------------------|
| ITIWND_NUMROWS       0          | uNumRows    | # rows in client          |
| ITIWND_ACTIVE        0          | uActive     | active row # (-1 if none) |
| ITIWND_SCROLLTYPE    0          | uScrollType | SB_SLIDERPOSITION or      |
|                                 |             | SB_SLIDERTRACK            |
| ITIWND_COLOR         uColorType |     lColor  | specified color val       |
| ITIWND_LABELEDT      uCol       |     PEDT    | NULL if bad col #         |
| ITIWND_DATAEDT       uCol       |     PEDT    | NULL if bad col #         |
| ITIWND_LABEL         uCol       |     psz     | NULL if bad col #         |
| ITIWND_BUFFER        uBuff      |     HBUF    | uBuff = 0 or 1            |
| ITIWND_BUTTON        0          | uButtonId   | scrollbar button VALUE    |
| ITIWND_TXTPROC       0          |     pfn     | ptr to txt proc           |
| ITIWND_INITDRAWPROC  0          |     pfn     | ptr to draw init proc     |
| ITIWND_DRAWPROC      0          |     pfn     | ptr to draw proc          |
| ITIWND_SELECTION     0=unsel 1=s| uStart  uEnd| select/unselect range     |
| ITIWND_REDRAW        0          | 0         0 |                           |
| ITIWND_MAXSIZE       0          |     RECTL   |                           |
| ITIWND_VIEWROW       0          | uRow        |                           |
| ITIWND_SCROLL        uType      | sVal        | uType = 0(ABS) or 1 (REL) |
| ITIWND_ROWCHANGED    0          | uRow        |                           |
| ITIWND_ROWINSERTED   0          | uRow        |                           |
| ITIWND_ROWDELETED    0          | uRow        |                           |
| ITIWND_ADDID         uNewId     | 0          0| new ID for add dlg        |
| ITIWND_CHANGEID      uNewId     | 0          0| new ID for change dlg     |
| ITIWND_LABELHIDDEN   uCol       | bHide       | sets hide state for label |
| ITIWND_DATAHIDDEN    uCol       | bHide       | sets hide state for data  |
-----------------------------------------------------------------------------
*/




/*    -------- MESSAGES GENERATED BY LIST WINDOW FOR PARENT --------
---------------------------------------------------------------------
| LIST WNDDOW             |      --MP1--        |    --MP2--        |
|   MESSAGE               | L            H      | L          H      |
|-------------------------+---------------------+-------------------|
| WM_ITILWQBUTTON         | uButtonId    LW Id  |     hListWnd      |
|                         |                     |                   |
| WM_ITILWSCROLL          | uDeltaX      uDeltaY| uAbsX      uAbsY  |
|                         |                     |                   |
| WM_ITILWMOUSE           | uRow         uCol   | winID  (see below)|
|                         |                     |                   |
---------------------------------------------------------------------

      --- WM_ITILWMOUSE - MP2 - HighWord ---

         HIGH BYTE       LOW BYTE
      F E D C B A 9 8 7 6 5 4 3 2 1 0
      --------------------------------
      |              |   areatype    |
      |           ^ ^|               |
      ------------+-+-----------------
                  | |
                  | Single(0) or Double(1) click
                  |
                  Left(0) or Right(1) button

      use following 2 functions to check bits:
*/
extern BOOL EXPORT ItiWndIsSingleClick (MPARAM mp);
extern BOOL EXPORT ItiWndIsLeftClick  (MPARAM mp);


/*
 * The ITIWND_COLOR cmd is used to change the color of one or
 * more parts of a list window. The parameters are:
 *
 * hwnd           The window whose color(s) are to be changed
 * uColorType     The element to change. The choices are:
 *       
 *    LWC_RESET ...... Resets all color values to thier default
 *    LWC_CLIENT ..... Set Data Background Color
 *    LWC_LABEL ...... Set Label Background Color
 *    LWC_SELECT ..... Set Selected Element Background Color
 *    LWC_ACTIVE ..... Set Active Outline Color
 *    LWC_SPLITBORDER  Set Split Bar Border Color
 *    LWC_SPLITMIN ... Set Split Bar Interior color when minimized
 *    LWC_SPLITMAX ... Set Split Bar Interior color when maximized
 *
 * lColorVal      The new color. This may be any color value including
 *                one of:
 *    LWCLR_DEFCLIENT     
 *    LWCLR_DEFLABEL      
 *    LWCLR_DEFSELECT     
 *    LWCLR_DEFACTIVE     
 *    LWCLR_DEFSPLITBORDER
 *    LWCLR_DEFSPLITMIN   
 *    LWCLR_DEFSPLITMAX   
 *
 * LWRedraw should be called after a call to LWChangeColor.
 *
 */



/* The ItiWndTerminate procedure must be called before the process is
 * terminated so any allocated resources may be freed
 */
extern USHORT EXPORT ItiWndTerminate (void);


/*
 * ItiWinAppDefWindowProc is to be called by the application's
 * main window as the default message processing handler.
 */
extern MRESULT EXPORT ItiWndAppDefWindowProc (HWND   hwnd, 
                                              USHORT usMessage,
                                              MPARAM mp1,
                                              MPARAM mp2);



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

extern BOOL EXPORT ItiWndSetMDIApp (HWND hwndAppFrame);



#if !defined (EDT_DEFINED)
#define EDT_DEFINED
/*********************************************************************/
/* Information below defines the structures that are used by the     */
/* metabase module to pass required information to the windows       */
/*********************************************************************/
/*
 * EDT - Element Description Table
 * 2 arrays of these structures are used when initializing
 * list or static windows. 1 for the data elements and one for
 * the label elements
 */
typedef struct
   { USHORT uIndex;         // col # for data, str index for labels
     USHORT uNextId;        // window linked to this column (for dbl-clk)
     RECTL  rcl;            // bounding rectangle for data
     USHORT uCoordType;     // type of coordinates rcl is in
     LONG   lFColor;        // foreground color
//   USHORT uFlags;       // text drawing flags
     ULONG  lFlags;         // text drawing flags in lowword 
                            // custom flags in hiword  
   } EDT;
typedef EDT FAR *PEDT;


/*
 * DGI - Dialog Info
 * Dialog Windows
 */
typedef struct         // Used by metabase in ItiMbGetDlgInfo
   {                   //
   USHORT   uAddQuery;    // Add Query id
   USHORT   uChangeQuery; // Change Query id
   USHORT   uColId;    // Colid in query
   USHORT   uCtlId;    // Ctl Id of dialog box
   USHORT   uOffset;   // Internal use (offset to formats)    /*--- internal use (offset to buffdat)---*/
   PSZ      pszInputFmt; // Input validation format
   } DGI;
typedef DGI FAR *PDGI;

#endif



/*
 * CHD - Child Window 
 * Static windows 
 */
typedef struct
   { USHORT uId;            // ID of child
     ULONG  ulKind;         // window kind (i.e. list window, button ...)
     RECTL  rcl;            // bounding rcl of child
     USHORT uCoordType;     // coord type for rcl
     USHORT uButtonId;      // ID of scroll bar button for list windws
     USHORT uAddId;         // edit-add    winid 
     USHORT uChangeId;      // edit-change winid
   } CHD;
typedef CHD FAR *PCHD;


/*********************************************************************/
/* Information below this line is not needed or used unless you are  */
/* doing something non-standard with a window                        */
/*********************************************************************/



/*
 *  --- DRAWINFO STRUCTURE ---
 *
 * This structure is only used if the owner is to draw the client
 * manually. If so, this structure is passed to the paint procedure.
 * The parameters are:
 *
 * hps            The handle to the current presentation space
 * uRow           The current row to paint
 * uCol           The current col to paint
 * prclPos        The bounding rectangle of the area to paint
 * lFColor        The foreground color given by the EDT Structure
 * lBColor        The background color given by the currently set color
 * ufFlags        The Text drawing flags given by the EDT Structure
 *
 */
typedef struct 
   { HPS    hps;
     USHORT uRow;
     USHORT uCol;
     PRECTL prcl;
     LONG   lFColor;
     LONG   lBColor;
     ULONG  lFlags;
   } DRAWINFO;
typedef DRAWINFO FAR *PDRAWINFO;







/*
 *  --- LWINITCLIENTDRAW STRUCTURE ---
 *
 * Calling this procedure signifies that the owner wishes to do
 * the painting of the client window. When this proc is called,
 * the GetText procedure is no longer used and the given paint
 * proc is used instead. If the paint proc is set to NULL, the
 * GetText proc is called and the area is painted automatically.
 * If this proc is used after the LWInit, LWRedraw should be used
 * to redraw the list window. The parameters are as follows:
 *
 * hwnd            The window to change.
 * InitClientDraw  The function to be called once at the
 *                 beginning of each paint. If this is NULL,
 *                 No init procedure is called. The form of this
 *                 function is as follows:
 *                    int InitClientDraw (HWND hwnd, HPS hps)
 *
 *
 * DrawClientText  The function to be called once for each
 *                 row,col. If this is NULL, the GetText proc is
 *                 called and the painting is done internally.
 *                 The form of this function is as follows:
 *                    int DrawRect (HWND hwnd, PDRAWINFO pdi)
 *
 */
typedef int (*PFNINITDRAW) (HWND hwnd, HPS hps);
typedef int (*PFNDRAW) (HWND hwnd, PDRAWINFO pdi);


/**********************************************************************/
/*****************  coordinate stuff  *********************************/
/**********************************************************************/





/* Coordinate Scales
 */
#define COORD_SCALETYPE    0x0003
#define PIX   0x0000
#define DLG   0x0001
#define CHR   0x0002
#define BLK   0x0003

/* Coordinate Origins
 * TL = Windows
 * BL = OS/2
 */
#define COORD_ORIGINTYPE    0x000C
#define TL    0x0000
#define TR    0x0004
#define BL    0x0008
#define BR    0x000C

/* These are the coordinate types for each of the 
 * 4 coordinates that make up a rectangle.
 *
 * ABS = normal coordinates
 * REL = # represents distance from parents adjacent wall
 * PCT = window position as a % size of parent
 * RNG = X1,Y1 = sizes rather than abs coordinates.
 *       xxxxxxxxxx RNG is already converted to ABS in res < not any more
 */

#define COORD_X0TYPE    0xC000
#define X0ABS           0x0000
#define X0REL           0x4000
#define X0PCT           0x8000

#define COORD_Y0TYPE    0x3000
#define Y0ABS           0x0000
#define Y0REL           0x1000
#define Y0PCT           0x2000

#define COORD_X1TYPE    0x0C00
#define X1ABS           0x0000
#define X1REL           0x0400
#define X1PCT           0x0800
#define X1RNG           0x0C00

#define COORD_Y1TYPE    0x0300
#define Y1ABS           0x0000
#define Y1REL           0x0100
#define Y1PCT           0x0200
#define Y1RNG           0x0300

#define COORD_DISPLAY       PIX | BL | X0ABS | Y0ABS | X1ABS | Y1ABS


#define ItiWndGetX0(TY)     ((TY) & (COORD_X0TYPE))
#define ItiWndGetY0(TY)     ((TY) & (COORD_Y0TYPE))
#define ItiWndGetX1(TY)     ((TY) & (COORD_X1TYPE))
#define ItiWndGetY1(TY)     ((TY) & (COORD_Y1TYPE))
#define ItiWndGetOrigin(TY) ((TY) & (COORD_ORIGINTYPE))
#define ItiWndGetScale(TY)  ((TY) & (COORD_SCALETYPE))

#define ItiWndBuildCoordType(TY,XB,YB,XO,YO,OR,SC)  ((TY)=(SC)|(OR)|(YO)|(XO)|(YB)|(XB))











/**********************************************************************/
/****************  dialog   window stuff  *****************************/
/**********************************************************************/

/*   ------- WM_ITIWNDQUERY MESSAGE FOR DIALOG WINDOWS --------
-----------------------------------------------------------------------------
|            --MP1--      --MP2-- | --RETURN--  |       --COMMENT--         |
| L                    H  L     H | L          H|                           |
|---------------------------------+-------------+---------------------------|
| ITIWND_NUMROWS       0          | uNumRows    | # rows in client          |
| ITIWND_ID            0          | uId         | window ID                 |
| ITIWND_ACTIVE        0          | uActive     | active row # (-1 if none) |
| ITIWND_BUFFER        0 buffid   |     HBUF    | Buff handle  (0=main buff)|
| ITIWND_HEAP          0          |     HHEAP   | handle to heap            |
| ITIWND_DATA     buffid uRow,uCol| pszDat      | BufferData                |
| ITIWND_OWNERWND:     0          |     HWND    |                           |
| ITIWND_PSTRUCT:       0          |     pdwdat  |                           |
| ITIWND_TYPE          0          | ITI_DIALOG  | identifies win type       |
| ITIWND_OWNERLISTBUFFER: 0       |     HBUF    |                           |
| ITIWND_OWNERSTATICBUFFER: 0     |     HBUF    |                           |
-----------------------------------------------------------------------------
*/      



/**********************************************************************/
/*****************  end window stuff  *********************************/
/**********************************************************************/


#define ITIWND_LIST     0x0000
#define ITIWND_ENUM     0xFFFE
#define ITIWND_PARENT   0xFFFF
extern void EXPORT ItiWndSetDlgMode (USHORT uMode);



/**********************************************************************/
/* Utility functions                                                  */
/**********************************************************************/

PVOID EXPORT ItiWndGetExtra (HWND hwnd, USHORT uType);
BOOL EXPORT ItiWndSetExtra (HWND hwnd, USHORT uType, PVOID pData);




/*
 * ItiWndSetHourGlass sets the hourglass icon.
 *
 * Parameters: bHour          TRUE for hourglass, FALSE for normal pointer.
 *
 * Return value: TRUE if it worked, FALSE if not.  
 */

extern BOOL EXPORT ItiWndSetHourGlass (BOOL bHour);

extern USHORT EXPORT ItiWndPrepRcl (HWND hwnd, USHORT uType, PRECTL prcl);

/*
 * This function will replace any repleable parameters for query's
 * The characters considered special are '%', '@', and '{'-'}'.
 * The '@' character can only be used when hwnd is a dialog window.
 *
 * When replacing parameters, the query will look at its parent window
 * for data to replace keys with. If the parent is a list window, the
 * parents static window will also be looked at if the parent does not
 * have the required data column.
 *
 * If the current mode is ITI_ADD the query will not be allowed to use
 * its parent windows buffer for data if it is a list window buffer. only
 * static and dialog window buffers may be used for data in this special
 * case. Think about it.
 *
 */
extern USHORT EXPORT ItiWndPrepQuery (PSZ     pszDest,
                                      PSZ     pszSrc,
                                      HWND    hwnd,
                                      USHORT  uMode);








extern USHORT EXPORT ItiWndReplaceAllParams(PSZ    pszDest,
                                            PSZ    pszSrc,
                                            HWND   hwndSel,
                                            HWND   hwndDlg,
                                            PVOID  hbufOwner,
                                            PVOID  hbufOwner2,
                                            USHORT uRowOwner,
                                            BOOL   bAllowMultiSel,
                                            USHORT uMax);


extern USHORT EXPORT ItiWndReplaceDlgParams (PSZ    pszDest,
                                             PSZ    pszSrc,
                                             HWND   hwndDlg,
                                             USHORT uDestSize);
       



USHORT ItiWndSetLBHandles (HWND hwnd, USHORT uCtlId, ULONG ulHandle);
USHORT ItiWndMoveSelectedItem (HWND hwnd, USHORT uCtlDest, USHORT uCtlSrc);

USHORT ItiWndFadeWindow (HWND hwnd, ULONG lTopColor, ULONG lBottomColor);


#endif

