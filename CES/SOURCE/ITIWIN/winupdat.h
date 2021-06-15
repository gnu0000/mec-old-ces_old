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
 * winupdat.h                                                           *
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

/* Although exported, these should remain out of itiwin.h to 
 * keep them hidden.
 *
 * The modify functions can be accessed via messages in itiwin
 */

extern USHORT EXPORT ItiWndDeleteDb (HWND hwnd, PSZ pszQuery);
extern USHORT EXPORT ItiWndChangeDb (HWND hwnd, PSZ pszQuery);
extern USHORT EXPORT ItiWndAddDb    (HWND hwnd, PSZ pszQuery);

// in itiwin.h
//
//
///*
// * This function will replace any repleable parameters for query's
// * The characters considered special are '%', '@', and '{'-'}'.
// * The '@' character can only be used when hwnd is a dialog window.
// *
// * When replacing parameters, the query will look at its parent window
// * for data to replace keys with. If the parent is a list window, the
// * parents static window will also be looked at if the parent does not
// * have the required data column.
// *
// * If the current mode is ITI_ADD the query will not be allowed to use
// * its parent windows buffer for data if it is a list window buffer. only
// * static and dialog window buffers may be used for data in this special
// * case. Think about it.
// *
// */
//extern USHORT EXPORT ItiWndPrepQuery (PSZ     pszDest,
//                                      PSZ     pszSrc,
//                                      HWND    hwnd,
//                                      USHORT  uMode);


//     in itiwin.h
///*
// * This procedure will do an add, change or delete for a specified 
// * window. The querys are obtained from the metadata of the correct
// * type. The query ID's are the same as the ID of hwnd
// * Use ItiWndModifyDb if you already have the query string
// *
// */
//extern USHORT EXPORT ItiWndAutoModifyDb (HWND hwnd, USHORT uMode);
//
//
//
///*
// * This procedure will do an add, change or delete for a specified 
// * window. pszQuery is assumed to be the query with the replaceable
// * parameters not replaced yet. If the uMode = ITI_CHANGE, the lock
// * query will be obtained from the metadata for hwnd. If the query
// * contains '@' characters, make sure hwnd is a dialog window.  If
// * you want the querys to be obtained from the metadata automatically,
// * use ItiWndAutoModifyDb.
// *
// * if uMode = ITI_DELETE, the verify msg box should be done before
// * calling this fn.
// *
// */
//extern USHORT EXPORT ItiWndModifyDb (HWND hwnd, PSZ pszQuery, USHORT uMode);
//

// in itiwin.h
//
//
//extern USHORT EXPORT ItiWndReplaceAllParams(PSZ    pszDest,
//                                             PSZ    pszSrc,
//                                             HWND   hwndSel,
//                                             HWND   hwndDlg,
//                                             HBUF   hbufOwner,
//                                             HBUF   hbufOwner2,
//                                             USHORT uRowOwner,
//                                             BOOL   bAllowMultiSel,
//                                             USHORT uMax);
//




/*
 * This fn loads/preps/executes the lock query for hwnd
 * and returns the locked row
 * 0 = no error
 * !0= error code
 */
USHORT ItiWndDoQueryLock (HWND hwnd, PSZ **pppszRow, USHORT *puCols);

