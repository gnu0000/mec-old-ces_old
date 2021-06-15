/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * Import.h
 * Mark Hampton
 */




#define PESEXP_GETPROJECTS    1
#define PESEXP_PROJECT        2
#define PESEXP_CATEGORY       3
#define PESEXP_PROJECT_ITEM   4

#if !defined (RC_INVOKED)

/* pointer to a big buffer */
extern PSZ pszBuffer;

/* amount of memory pointed to by pszBuffer */
extern USHORT usBufferSize;

#if defined GLOBALS_INCLUDED
extern PGLOBALS pglobals;
#endif

#endif


