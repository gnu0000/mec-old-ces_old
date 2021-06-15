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
 * arg.h 
 * Craig Fitzgerald
 * This module helps parse command line parameters
 */

#define  NO_ERROR       0
#define  NO_EXTENSION   1
#define  UNKNOWN_CMD    2

#ifndef BOOL 
#define  BOOL           int 
#define  FALSE          0
#define  TRUE           1
#endif

typedef  struct 
   {
   char *            pszArgument;
   char *            pszParam;
   unsigned int   uiCount;
   } ARGBLK;

extern int ProcessParams (char *          argv [], 
                          ARGBLK       argrec [], 
                          unsigned int uiCount, 
                          unsigned int *uiError);

/* Returns # of args processed.  Compound args (I.E. -F 5000 ) count   *
 * as two. If an error occurs, the error code is stored in the high    *
 * byte of iError and the relative index of the offending arg is       *
 * stored in the low byte.  In case of multiple errors the last one    *
 * encountered is returned. In case of compound argument redifinitions *
 * the last definition is used.                                        */
