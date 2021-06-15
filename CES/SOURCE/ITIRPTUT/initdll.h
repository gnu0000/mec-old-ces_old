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
 * InitDll.h
 *
 * This file declares the module wide global variables.
 */


#if !defined (INITDLL_INCLUDED)

#define INITDLL_INCLUDED

/*
 * hmodModule is the handle to this DLL.  This variable is initialized
 * at module load time.
 */

extern HMODULE  hmodModule;

extern HHEAP    hhpRptUt;
extern HHEAP    hhpKey;

extern PGLOBALS pglobRptUt;

#endif 
