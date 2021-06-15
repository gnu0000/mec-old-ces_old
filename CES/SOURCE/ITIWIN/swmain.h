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
 * swmain.h                                                             *
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

#if !defined (SWMAIN_INCLUDED)
#define SWMAIN_INCLUDED

#define WNDPTR_SWDAT 0

#define SW_INITIALHEAPSIZE        1024

/* this structure is maintained by static windows
 * it is pointed to by an extra byte pointer
 */
/* SWDAT - Static Win Data */

typedef struct
   { USHORT  uLabels;
     USHORT  uDatas;
     USHORT  uChildren;
     RECTL   rclPos;
     USHORT  uCoordType;
     LONG    lBColor;
     PEDT    pedtLabel;
     PEDT    pedtData;
     PSZ     *ppszLabels;
     PSZ     pszTitle;
     PCHD    pchdChildren;
     USHORT  uId;
     HHEAP   hheap;
     PVOID   hbuf;
     HMODULE hmod;
     USHORT  uAddId;         // edit-add    winid 
     USHORT  uChangeId;      // edit-change winid
     PVOID   pExtra;
     HWND    hwndOwner;
   } SWDAT;
typedef SWDAT FAR *PSWDAT;




typedef struct
   {
   HBUF     hbuf;
   HBUF     hbufParent;
   USHORT   uBuffId;
   } BUFDAT;
typedef BUFDAT FAR *PBUFDAT;



/*
 * Dialog Window Data
 *
 */
typedef struct
   { 
     USHORT  uResId; 
     HHEAP   hheap;
     HWND    hwndOwner;
     USHORT  uCtls;
     PDGI    pdgi;
     USHORT  uBuffers;
     PBUFDAT pbufdat;
     HMODULE hmod;

     /*---These are new---*/
     USHORT  uDlgMode;
     USHORT  uDlgId;
     USHORT  uDoneQueryCount;
     PVOID   pExtra;
     PSZ     *ppszLockRow;

   } DWDAT;
typedef DWDAT FAR *PDWDAT;


/*
 * This procedure registers the classes for
 * the static and list windows. It returns
 * OK or NOT_OK
 *
 */


USHORT EXPORT ItiWndRegister (HAB hab);


HWND EXPORT ItiWndKickListWindow (HWND   hwndList,
                                  USHORT uId);


#endif (if ! SWMAIN_INCLUDED)

