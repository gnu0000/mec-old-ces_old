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
 * DBqry.h
 * Judy A. Siggelkow
 * Craig Fitzgerald
 * This module is part of DS/Shell (CS/Base)
 *
 */

#if !defined (DATABASE_INCLUDED)
#define DATABASE_INCLUDED

#if defined (DATABASE_INTERNAL)

typedef LOGINREC  *HLOGIN;
typedef DBPROCESS *HDB;

#else

typedef PVOID HLOGIN;

typedef PVOID HDB;

#endif



typedef struct
   {
   HDB         hdb;
   HHEAP       hheap;
   USHORT      uNumCols;
   } QRY;
typedef QRY *HQRY;




extern BOOL DbInit (void);


/*
 * If an error occurs, this fn returns NULL
 * and you can check uState for the error code. 
 * If tou get NULL but error code = SUCCEED, there was insuficient memory
 * currently hmod,uResId, and uId are not used. They are for the 
 * formatting which has yet to be implemented
 */
extern HQRY DbExecQuery (PSZ     pszQuery,
                         HHEAP   hheap,  
                         USHORT  *puNumCols,
                         USHORT  *puState);


extern BOOL DbGetQueryRow (HQRY   hqry,
                           PSZ    **pppsz,
                           USHORT *puState);




extern void DbTerminateQuery (HQRY hqry);

#endif
