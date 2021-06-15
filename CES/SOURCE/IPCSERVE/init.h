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
 * init.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module initializes the local IPC server
 */

/* maximum number of miliseconds for the Init function to wait for a 
   semaphore to be cleared */

#define MAX_INIT_WAIT   10000UL

extern BOOL Init (void);
