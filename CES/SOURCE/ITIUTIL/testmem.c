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
#include "..\include\itiutil.h"


char szBuffer [1024];

char szTemp [1024];

char szSmall [10];


int main (int argc, char *argv [])
   {
   ULONG    ulNumber;
   USHORT   usNumber;
   SHORT    sNumber;
   LONG     lNumber;
   BOOL     bOverflow;

   ItiStrCpy (szBuffer, "This is a test.", sizeof szBuffer);
   printf ("szBuffer = '%s'\n", szBuffer);

   ItiStrCpy (szSmall, "012345678901234567890", sizeof szSmall);
   printf ("szSmall = '%s'\n", szSmall);

   ItiStrCat (szSmall, "abcdefg", sizeof szSmall);
   printf ("szSmall = '%s'\n", szSmall);

   ItiStrCpy (szSmall, "01234", sizeof szSmall);
   ItiStrCat (szSmall, "abcdefg", sizeof szSmall);
   printf ("szSmall = '%s'\n", szSmall);

   printf ("searching for 'a' in '%s': %c\n", szSmall, 
            *ItiStrChr (szSmall, 'a'));

   printf ("searching for 'z' in '%s': %Fp\n", szSmall, 
            ItiStrChr (szSmall, 'z'));

   printf ("ItiStrCmp (szBuffer, szSmall) = %d\n", ItiStrCmp (szBuffer, szSmall));
   printf ("ItiStrCmp (szSmall, szBuffer) = %d\n", ItiStrCmp (szSmall, szBuffer));
   printf ("ItiStrCmp (szBuffer, szBuffer) = %d\n", ItiStrCmp (szBuffer, szBuffer));

   printf ("ItiStrICmp (\"abcdefg\", \"ABCDEFG\") = %d\n", ItiStrICmp ("abcdefg", "ABCDEFG"));

   printf ("ItiStrNCmp (\"012345xyz\", \"01234\", 5) = %d\n",  
            ItiStrNCmp ("012345xyz", "01234", 5));
   printf ("ItiStrNCmp (\"012345xyz\", \"01234\", 4) = %d\n",  
            ItiStrNCmp ("012345xyz", "01234", 4));
   printf ("ItiStrNCmp (\"012345xyz\", \"01234\", 6) = %d\n",  
            ItiStrNCmp ("012345xyz", "01234", 6));
   printf ("ItiStrNCmp (\"012345xyz\", \"01234z\", 5) = %d\n",  
            ItiStrNCmp ("012345xyz", "01234z", 5));
   printf ("ItiStrNCmp (\"012345xyz\", \"01234z\", 6) = %d\n",  
            ItiStrNCmp ("012345xyz", "01234z", 6));

   printf ("ItiStrIsCharInString ('x', \"0123435abcxyz\") = %d\n", 
            ItiStrIsCharInString ('x', "0123435abcxyz"));
   printf ("ItiStrIsCharInString ('0', \"0123435abcxyz\") = %d\n", 
            ItiStrIsCharInString ('0', "0123435abcxyz"));
   printf ("ItiStrIsCharInString ('9', \"0123435abcxyz\") = %d\n", 
            ItiStrIsCharInString ('9', "0123435abcxyz"));
   printf ("ItiStrIsCharInString ('z', \"0123435abcxyz\") = %d\n", 
            ItiStrIsCharInString ('z', "0123435abcxyz"));

   bOverflow = ItiStrToULONG ("429496725", &ulNumber);
   printf ("ItiStrToULong (\"4294967295\") = %lu Overflow = %d\n", ulNumber, bOverflow);
   bOverflow = ItiStrToULONG ("429496726", &ulNumber);
   printf ("ItiStrToULong (\"4294967296\") = %lu Overflow = %d\n", ulNumber, bOverflow);
   bOverflow = ItiStrToULONG ("   5", &ulNumber);
   printf ("ItiStrToULong (\"   5\") = %lu Overflow = %d\n", ulNumber, bOverflow);
   bOverflow = ItiStrToULONG ("   0", &ulNumber);
   printf ("ItiStrToULong (\"   0\") = %lu Overflow = %d\n", ulNumber, bOverflow);
            
   bOverflow = ItiStrToLONG ("429496725", &lNumber);
   printf ("ItiStrToLong (\"4294967295\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("2147483648", &lNumber);
   printf ("ItiStrToLong (\"2147483648\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("-2147483648", &lNumber);
   printf ("ItiStrToLong (\"-2147483648\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("2147483647", &lNumber);
   printf ("ItiStrToLong (\"2147483647\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("-2147483647", &lNumber);
   printf ("ItiStrToLong (\"-2147483647\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("   0", &lNumber);
   printf ("ItiStrToLong (\"   0\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("   5", &lNumber);
   printf ("ItiStrToLong (\"   5\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("234", &lNumber);
   printf ("ItiStrToLong (\"234\") = %ld Overflow = %d\n", lNumber, bOverflow);
   bOverflow = ItiStrToLONG ("-234", &lNumber);
   printf ("ItiStrToLong (\"-234\") = %ld Overflow = %d\n", lNumber, bOverflow);
            
            
            
   }