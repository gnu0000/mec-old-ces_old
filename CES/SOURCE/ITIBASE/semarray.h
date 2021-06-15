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
 * Semarray.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This moudule handles dynamic allocation of the RAM semaphores.
 * These functions are used by the Event and Mutex semaphore modules.
 */ 




/*
 * GetNextFreeRamSemaphore returns a handle to the next available
 * RAM semaphore.
 *
 * Parameters: None.
 *
 * Return value:
 * A handle to a RAM semaphore, or NULL if no more RAM semaphores can
 * be allocated.
 */

extern HSEM GetNextFreeRamSemaphore (void);





/*
 * FreeRamSemaphore frees the specified ram semaphore from the semaphore
 * array.
 *
 * Parameters: hsem           A handle to a semaphore to free.
 *
 * Return value: 
 * Zero if no error occured, or ERROR_TOO_MANY_SEM_REQUESTS if an internal
 * semaphore is not received.
 */ 
   
extern USHORT FreeRamSemaphore (HSEM hsem);




/*
 * IsRamSemaphore returns TRUE is the given semaphore is a RAM semapbore,
 * or FALSE if not.
 */

extern BOOL IsRamSemaphore (HSEM hsem);





/*
 * InitSemArrayModule initializes this module.  It should only be called
 * once, at module (DLL) initialization time.
 */

extern BOOL InitSemArrayModule (HMODULE hmodDll);
