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


#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include <stdio.h>

static void TestEnum (HHEAP hheap, PSZ pszEnumName);
static void TestCheckEnum (USHORT usEnum, PSZ pszValue);
static void TestQueryEnum (USHORT usEnum, USHORT usValue);
static void TestCheck (PSZ pszFormat, PSZ pszData);
static void TestFormat (PSZ pszFormat, PSZ pszData);

int _cdecl main (int argc, char *argv [])
   {
   HHEAP    hheap;

   if (!ItiFmtInitialize ())
      {
      printf ("Could not initialize format module.\n");
      return 1;
      }
   hheap = ItiMemCreateHeap (0);

   TestFormat (NULL, "1235 ");

   TestCheckEnum (ENUM_PRECISION, "tens");
   TestCheckEnum (ENUM_PRECISION, "gargb");

   TestQueryEnum (ENUM_PRECISION, 10);
   TestQueryEnum (ENUM_PRECISION, 100);

   TestEnum (hheap, "UserType");
   TestEnum (hheap, "Dependency");
   TestEnum (hheap, "Month");
   TestEnum (hheap, "Precision");
   TestEnum (hheap, "Privilege");
   TestEnum (hheap, "QuantityLevel");
   TestEnum (hheap, "Season");
   TestEnum (hheap, "UnitTime");

   TestCheck ("Position", "1 2 3 4 5 6");

   TestCheck ("*Binary,#", "90");
   TestCheck ("*Binary,#", "");
   TestCheck ("*Binary", "90");
   TestCheck ("*Binary", "");
   TestCheck ("*Year,#", "70");
   TestCheck ("*Year,#", "");
   TestCheck ("*Year", "70");
   TestCheck ("*Year", "");
   TestCheck ("*String,#", "This is a string with a quote \".");
   TestCheck ("*String,#", "");
   TestCheck ("*String", "This is a string with a quote \".");
   TestCheck ("*String", "");
   TestCheck ("*DateTime,#", "January 1, 1991 11:22:33p");
   TestCheck ("*DateTime,#", "");
   TestCheck ("*DateTime", "January 1, 1991 11:22:33p");
   TestCheck ("*DateTime", "");
   return 0;
   }


static void TestEnum (HHEAP hheap, PSZ pszEnumName)
   {
   PSZ *ppszShort;
   PSZ *ppszLong;
   USHORT usCount;
   PUSHORT pusValues;
   BOOL bOK;
   USHORT i;

   bOK = ItiFmtGetEnumData (hheap, pszEnumName, &ppszShort, &ppszLong, 
                            &pusValues, &usCount);
   if (!bOK)
      {
      printf ("Enum '%s' does not exist.\n", pszEnumName);
      return;
      }
   printf ("Enum '%s' has %u entries:\n", pszEnumName, usCount);
   for (i=0; i < usCount; i++)
      printf ("%10s, %20s, %u\n", ppszShort [i], ppszLong [i], pusValues [i]);

   ItiFmtFreeEnumData (hheap, ppszShort, ppszLong, pusValues, usCount);
   }



static void TestCheckEnum (USHORT usEnum, PSZ pszValue)
   {
   USHORT usValue, usError;

   usError = ItiFmtCheckEnumValue (usEnum, pszValue, &usValue);
   printf ("Enum %u, \"%s\" = %u (%s)\n", usEnum, pszValue, usValue,
           usError ? "OK" : "Failed");
   }


static void TestQueryEnum (USHORT usEnum, USHORT usValue)
   {
   char szTemp [100];
   USHORT usError;

   usError = ItiFmtQueryEnumString (usEnum, usValue, FALSE, szTemp, sizeof szTemp);
   printf ("Enum %u, %u = \"%s\" (short) (%s)\n", usEnum, usValue, szTemp, 
           usError ? "OK" : "Failed");

   usError = ItiFmtQueryEnumString (usEnum, usValue, TRUE, szTemp, sizeof szTemp);
   printf ("Enum %u, %u = \"%s\" (long)  (%s)\n", usEnum, usValue, szTemp, 
           usError ? "OK" : "Failed");
   }


static void TestCheck (PSZ pszFormat, PSZ pszData)
   {
   char szDest [256];
   USHORT usLength, usError;

   usError = ItiFmtCheckString (pszData, szDest, sizeof szDest, 
                                pszFormat, &usLength);
   printf ("\"%s\" Checked with \"%s\" = \"%s\" (%u chars) usError = %u\n", 
           pszData, pszFormat, szDest, usLength, usError); 
   }


static void TestFormat (PSZ pszFormat, PSZ pszData)
   {
   char szDest [256];
   USHORT usLength, usError;

   usError = ItiFmtFormatString (pszData, szDest, sizeof szDest, 
                                 pszFormat, &usLength);
   printf ("\"%s\" Formatted with \"%s\" = \"%s\" (%u chars) usError = %u\n", 
           pszData, pszFormat, szDest, usLength, usError); 
   }


