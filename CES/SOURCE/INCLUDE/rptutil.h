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



#if !defined (INCL_RPTUTIL)
#define  INCL_RPTUTIL


#define  RPTUTIL_DLL_VERSION  1


/* ----------------------------------------------------------------------- *\
 *                                                            Error Codes  *
\* ----------------------------------------------------------------------- */
#define  RPTUTIL_ERR                RPTUTIL_ERR_BASE
#define  RPTUTIL_NO_MORE_KEYS      (RPTUTIL_ERR_BASE + 1)
#define  RPTUTIL_USE_DEFAULT       (RPTUTIL_ERR_BASE + 3)



/* ----------------------------------------------------------------------- *\
 *                                                         Function Names  *
\* ----------------------------------------------------------------------- */
#define  DLL_NAME_RPTUTIL                 "RPTUTIL"
#define  RPTUTIL_ITOA_FUNC                "RPTUTILITOA"
#define  RPTUTIL_SETERROR_FUNC            "RPTUTILSETERROR"
#define  RPTUTIL_INITKEYLIST_FUNC         "RPTUTILINITKEYLIST"
#define  RPTUTIL_GETNEXTKEY_FUNC          "RPTUTILGETNEXTKEY" 
#define  RPTUTIL_PREPQUERY_FUNC           "RPTUTILPREPQUERY"
#define  RPTUTIL_GETOPTION_FUNC           "RPTUTILGETOPTION"

USHORT EXPORT ItiDllQueryVersion (VOID);
BOOL   EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);
PSZ    EXPORT RptUtilItoa        (INT iNum, CHAR str[], INT iBase);
USHORT EXPORT RptUtilSetError    (HAB hab, PSZ pszErrorText);
USHORT EXPORT RptUtilInitKeyList (INT iArgCnt, CHAR * pArgVar[]);
USHORT EXPORT RptUtilGetNextKey  (PSZ pszKeyStr, USHORT usKeyLen);
USHORT EXPORT RptUtilPrepQuery
  (PSZ pszQ, USHORT usQLen, PSZ pszQueryBase, PSZ pszConj, PSZ pszKeyStr);
USHORT EXPORT RptUtilGetOption
               (CHAR cOpt, PSZ pszOptionStr, INT argcnt, CHAR * argvars[]);
 

#endif    /* if !defined (INCL_RPTUTIL) */

