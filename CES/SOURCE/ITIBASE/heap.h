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
 * Heap.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */


/*
 * InitHeapModule is called by DLLInit.c during DLL initialization.
 * Note that InitHealpModule MUST BE CALLED AFTER the semaphore 
 * module is initialized.
 *
 * This function returns TRUE if initialization was successful, 
 * or FALSE if not.
 */

extern BOOL InitHeapModule (HMODULE hmodDll);

