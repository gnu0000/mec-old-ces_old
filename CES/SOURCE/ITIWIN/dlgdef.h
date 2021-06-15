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
 * dlgdef.h                                                             *
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

extern ULONG CtlClassID (HWND hwndDlg, USHORT uCtlId);


extern USHORT BuffTypeFromCtlId (HWND hwndDlg, USHORT uCtlId);

extern void SetControlText (HWND hwndDlg, USHORT uCtlId,
                            PSZ pszStr, USHORT uIndex);


extern void EnableDlgItem (HWND hwndDlg, USHORT uCtlId, BOOL bEnable);

PDGI GetMatchingPDGI (PDWDAT pdwdat,
                      USHORT *puCtlId,
                      USHORT *puColId,
                      HBUF   hbuf);

extern USHORT GetHbufRow (HWND   hwndDlg,
                         HBUF   hbuf, 
                         PDWDAT pdwdat, 
                         USHORT uCtlId);

extern HBUF GetDlgHbuf (HWND   hwndDlg,
                        PDWDAT pdwdat, 
                        USHORT *puCtlId, 
                        USHORT uColId);


extern USHORT QueryId (PDWDAT pdwdat, USHORT i);

extern USHORT QueryId2 (PDWDAT pdwdat, PDGI pdgi);

