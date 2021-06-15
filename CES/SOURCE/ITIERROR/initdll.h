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
 * initdll.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This header file declares variables initialized at DLL initialization
 * time.
 */






extern HFILE hfDebugPipe;

extern char szPipeName [];
extern char szFileName [];

extern HSEM hsemPipeCreated;

extern HSEM hsemWritePipe;

/* plisLocalInfo is a global pointer to the local info seg. */
extern PLINFOSEG plisLocalInfo;



