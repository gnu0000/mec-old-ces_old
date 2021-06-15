/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.         |
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
 * Import.h
 * Copyright (C) 1997 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module imports files into the BAMS/PC database.
 */

#if defined GLOBALS_INCLUDED
extern PGLOBALS pglobals;
#endif

/* pointer to a big buffer */
extern PSZ pszBuffer;

/* amount of memory pointed to by pszBuffer */
extern USHORT usBufferSize;


/* pointer to string that has the name of the production database */
extern PSZ pszProdDatabase;

/* number of characters pointed to by pszProdDatabase */
extern USHORT usProdDatabaseLen;


/* pointer to string that has the name of the import database */
extern PSZ pszImportDatabase;

/* number of characters pointed to by pszImportDatabase */
extern USHORT usImportDatabaseLen;

