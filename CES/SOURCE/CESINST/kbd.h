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
 * Kbd.h
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */


void ClearKeyBuff (void);
int GetKey (PSZ pszKeyList);
BOOL GetString (PSZ *ppsz, PSZ pszOld, USHORT y, USHORT x, USHORT uMax);
int EditCell (PSZ psz, USHORT x, USHORT y, USHORT uMax, USHORT uCmd,
               PSZ pszNormalReturns, PSZ pszExtendedReturns);

