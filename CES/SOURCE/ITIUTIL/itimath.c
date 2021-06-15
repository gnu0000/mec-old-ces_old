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
 * itimath.c
 *
 * This module provides double string conversion
 */


#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include <math.h>



DOUBLE EXPORT ItiStrToDouble (PSZ pszNumber)
   {
   DOUBLE d, dDivide, dExp;
   DOUBLE dSign, dExpSign;

   dSign = 1;
   while (ItiCharIsSpace (*pszNumber) || *pszNumber == '-' || *pszNumber == '+' ||
          *pszNumber == '$' || *pszNumber == '(')
      {
      if (*pszNumber == '-' || *pszNumber == '(')
         dSign = -1;
      pszNumber++;
      }

   d = 0;
   while (ItiCharIsDigit (*pszNumber) || *pszNumber == ',')
      {
      if (ItiCharIsDigit (*pszNumber))
         d = d * 10 + (DOUBLE) ((int) *pszNumber - (int) '0');
      pszNumber++;
      }
   if (*pszNumber == '.')
      {
      pszNumber++;
      dDivide = 10;
      while (ItiCharIsDigit (*pszNumber))
         {
         d += (DOUBLE) ((int) *pszNumber - (int) '0') / dDivide;
         dDivide *= 10.0;
         pszNumber++;
         }
      }
   if (*pszNumber == 'e' || *pszNumber == 'E')
      {
      pszNumber++;

      /* assume a positive exponent */
      dExpSign = 1;
      if (*pszNumber == '-')
         {
         dExpSign = -1;
         pszNumber++;
         }
      else
         {
         pszNumber++;
         }
      dExp = 0;
      while (ItiCharIsDigit (*pszNumber))
         {
         dExp = dExp * 10 + (DOUBLE) ((int) *pszNumber - (int) '0');
         pszNumber++;
         }
      d *= pow (10, dExp * dExpSign);
      }
   return d * dSign;
   }


BOOL EXPORT ItiStrDoubleToString (DOUBLE dNum,
                                  PSZ    pszDest, 
                                  USHORT usDestMax)

   {
   DOUBLE dInt, dRem;
   SHORT sInt, sRem;
   USHORT x = 0;
   CHAR szInt[15] = "";
   CHAR szRem[15] = "";

   if (pszDest == NULL)
      return FALSE;

   for (x = 0; x < usDestMax ;x++)
      pszDest[x] = '\0';

   pszDest[0] = ' ';
   if (dNum < 0.0)
      pszDest[1] = '-';

   dRem = modf (dNum, &dInt);
   sInt = (SHORT) dInt;
   sRem = (SHORT) (dRem * 1000.0); // moves the decimal point

   ItiStrSHORTToString (sInt, szInt, sizeof szInt);
   ItiStrSHORTToString (sRem, szRem, sizeof szRem);

   ItiStrCat (pszDest, szInt, usDestMax);
   ItiStrCat (pszDest, ".", usDestMax);
   x = ItiStrLen (pszDest);
   ItiStrCat (pszDest, szRem, x+4);

   return TRUE;
   }

