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


#define DBMSOS2
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include <sqlfront.h>
#include <sqldb.h>
#include <stdio.h>

USHORT EXPORT ItiStrMoneyToString (PSZ pszDest, DBMONEY *pmoney, USHORT usSize)
   {
   USHORT usFraction;
   USHORT i;

   dbconvert (NULL, SQLMONEY, (PBYTE) pmoney, 8, SQLCHAR, pszDest, usSize);
   i = ItiStrLen (pszDest) + 2;
   usFraction = (USHORT) (pmoney->mnylow & 0x7fffU);
   if (i < usSize)
      {
      pszDest [i] = '\0';
      pszDest [i-1] = (char) (usFraction % 10) + (char) '0';
      usFraction /= 10;
      pszDest [i-2] = (char) (usFraction % 10) + (char) '0';
      }
   return 0;
   }


int _cdecl main (int argc, char *argv [])
   {
   DBMONEY money;
   USHORT   usError;
   char szTemp [100];

   money.mnyhigh = 0;
   money.mnylow = 0x499602da; /* 123456.7898 */
   usError = ItiStrMoneyToString  (szTemp, &money, sizeof szTemp);
   printf ("number = %s, usError = %u\n", szTemp, usError);

   return 0;
   }