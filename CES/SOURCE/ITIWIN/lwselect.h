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
 * lwselect.h                                                           *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
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

#if !defined (LWSELECT_INCLUDED)
#define LWSELECT_INCLUDED


#define LWF_UP          0x0001
#define LWF_DOWN        0x0002
#define LWF_RIGHT       0x0003
#define LWF_LEFT        0x0004
#define LWF_DEFAULT     0x0005
#define LWF_CURRENT     0x0006
#define LWF_NEXT        0x0007
#define LWF_SPECIFIED   0x0008

#define LWMP_BLANK      0xFFFF
#define LWMP_BELOW      0xFFFE
#define LWMP_ABOVE      0xFFFD
#define LWMP_ERROR      0xFFF0

//#define ITI_SELECT      0x0000
//#define ITI_UNSELECT    0x0001
//#define ITI_INVERT      0x0002


/*****************************************************************/
/*                                                               */
/*                         Focus Procedures                      */
/*                                                               */
/*****************************************************************/

/* hwnd = client window if bClient = TRUE
 *        frame window if bClient = FALSE
 * uCmd:
 *    LWF_UP         - set focus to client above
 *    LWF_DOWN       - set focus to client below
 *    LWF_RIGHT      - set focus to client to right
 *    LWF_LEFT       - set focus to client to left
 *    LWF_DEFAULT    - set focus to top left (depends on initial split loc)
 *    LWF_CURRENT    - set focus to same window (but checks splits)
 *    LWF_NEXT       - set focus to next dir.
 *    LWF_SPECIFIED  - set focus to hwnd (check split pos)
 */
extern BOOL SetClientFocus (HWND hwnd, USHORT uCmd, BOOL bChild);



/*****************************************************************/
/*                                                               */
/*                    Validation Procedures                      */
/*                                                               */
/*****************************************************************/

extern int GetUpdateRange (HWND hwnd, PRECTL prclUpdate, USHORT uYScrPos,
                USHORT *puFirst, USHORT *puLast,  USHORT *puInc);

extern USHORT InvalidateRow (HWND hwndFrame, USHORT uRow);

/* this routine returns 0xFFFF if the item is visible
 * or the abs scroll value that will just make it visible
 */
extern USHORT RowVisible (HWND hwndClient, USHORT uRow);

extern USHORT UpdateWindowsIfNeeded (HWND hwndFrame, 
                                     USHORT uFirst, 
                                     USHORT uLast);


/*****************************************************************/
/*                                                               */
/*                    Active Row Procedures                      */
/*                                                               */
/*****************************************************************/

/*
 * This function deactivated uOld and activates uNew
 * invalidating the correct rows
 * This proc also ensures that the active row is at least half visible 
 */
extern USHORT SetNewActive (HWND hwndClient, USHORT uVSBID,
                            USHORT uOld,     USHORT uNew);


/*****************************************************************/
/*                                                               */
/*                 Selected Rows Procedures                      */
/*                                                               */
/*****************************************************************/

/*
 * This fn inverts the selection state of a range of rows
 * This fn assumes the multi select option is valid
 * uModes:
 *    ITI_SELECT
 *    ITI_UNSELECT
 *    ITI_INVERT
 */
USHORT LWSelectRange (HWND   hwndClient,
                      USHORT uStart,
                      USHORT uEnd,
                      USHORT uMode);


/*
 * Does NO Bounds or error checking 
 */
extern void LWSelectElement (HWND hwndClient, USHORT i, BOOL bSelect);


/*
 * returns true if selected
 */
extern BOOL LWIsSelected (HWND hwndClient, USHORT i);


/*
 * This fn un-selects all rows
 */
extern USHORT LWDeselectAll (HWND hwndClient, BOOL bChild);

//itiwin.h
///*
// * returns first found selection starting at row uStart
// * returns 0xFFFF if no selection found
// */
//extern USHORT WndQuerySelection (HWND hwndFrame, USHORT uStart);
//


/* assumes:
 *        uNew != uOld
 *        uNew, uOld, uAnchor are valid values
 *
 * uAnchor - base point of current selection
 * uOld    - start of range to add to selection
 * uNew    - end of range to add to selection
 *
 */

/* assumes:
 *        uNew != uOld
 *        uNew, uOld, uAnchor are valid values
 *
 * uAnchor - base point of current selection
 * uOld    - start of range to add to selection
 * uNew    - end of range to add to selection
 *
 */
void LWSelectBoundedRange (HWND   hwndClient,
                           USHORT uAnchor,
                           USHORT uNew,
                           USHORT uOld);

#endif


