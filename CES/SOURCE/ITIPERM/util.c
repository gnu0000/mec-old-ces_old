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
 * util.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "..\include\dialog.h"
#include "init.h"
#include <stdio.h>
#include "query.h"
#include "util.h"



BOOL VerifyPassword (PSZ pszUserID, PSZ pszPassword)
   {
   char     szPassword [256];
   PSZ      apszTokens [2], apszValues [2], *ppsz;
   USHORT   usNumCols;
   char     szTemp [256];
   char     szQuery [1024];
   BOOL     bOK;

   if (0 != ItiFmtCheckString (pszPassword, szPassword, sizeof szPassword, 
                               "Password", NULL))
      return FALSE;

   apszTokens [0] = "UserID";
   apszTokens [1] = "Password";
   apszValues [0] = pszUserID;
   apszValues [1] = szPassword;

   ItiMbQueryQueryText (hmod, ITIRID_CALC, GET_USER_INFO, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, 2);

   ppsz = ItiDbGetRow1 (szQuery, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL)
      return FALSE;

   bOK = ppsz [0] != NULL;

   ItiFreeStrArray (hheap, ppsz, usNumCols);

   return bOK;
   }


