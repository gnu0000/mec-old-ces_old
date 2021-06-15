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
#include <stdio.h>
#include <memory.h>

char szSource [30000];
char szDest   [30000];


void _near * _near _pascal npbMemCpy (char _near   *npcDest, 
                                      char _near   *npcSource,
                                      USHORT       usSize)
   {
   char _near *npcReturnValue = npcDest;

   while (usSize)
      {
      *npcDest++ = *npcSource++;
      usSize--;
      }
   return npcReturnValue;
   }


void _far * _far _pascal fpbMemCpy (char _far   *fpcDest, 
                                      char _far   *fpcSource,
                                      USHORT       usSize)
   {
   char _far *fpcReturnValue = fpcDest;
   
   while (usSize)
      {
      *fpcDest++ = *fpcSource++;
      usSize--;
      }
   return fpcReturnValue;
   }


void _near * _near _pascal npwMemCpy (char _near   *npcDest, 
                                      char _near   *npcSource,
                                      USHORT       usSize)
   {
   char _near *npcReturnValue = npcDest;
   USHORT _near *npwDest;
   USHORT _near *npwSource;

   npwDest = (USHORT _near *) npcDest;
   npwSource = (USHORT _near *) npcSource;
   while (usSize > 1)
      {
      *npwDest++ = *npwSource++;
      usSize -= 2;
      }

   if (usSize > 0)
      {
      npcDest = ((char _near *) npwDest) - 1;
      npcSource = ((char _near *) npwSource) - 1;
      *npcDest = *npcSource;
      }

   return npcReturnValue;
   }

void _far * _far _pascal fpwMemCpy (char _far   *fpcDest, 
                                      char _far   *fpcSource,
                                      USHORT       usSize)
   {
   char _far *fpcReturnValue = fpcDest;
   USHORT _far *fpwDest;
   USHORT _far *fpwSource;

   fpwDest = (USHORT _far *) fpcDest;
   fpwSource = (USHORT _far *) fpcSource;
   while (usSize > 1)
      {
      *fpwDest++ = *fpwSource++;
      usSize -= 2;
      }

   if (usSize > 0)
      {
      fpcDest = ((char _far *) fpwDest) - 1;
      fpcSource = ((char _far *) fpwSource) - 1;
      *fpcDest = *fpcSource;
      }

   return fpcReturnValue;
   }


#define MAX_ITERATIONS  100

int main (int argc, char *argv [])
   {
   SEL         selLocalInfoSeg;
   SEL         selGlobalInfoSeg;
   PGINFOSEG   pgis;
   PLINFOSEG   plis;
   SEL         selSource, selDest;
   PVOID       pvSource, pvDest;
   PCHAR       pcSource;
   ULONG       ulStart, ulEnd;
   USHORT      i;

   DosGetInfoSeg (&selGlobalInfoSeg, &selLocalInfoSeg);
   pgis = MAKEP (selGlobalInfoSeg, 0);
   plis = MAKEP (selLocalInfoSeg, 0);


   /************************************************************************/
   /* FAR TESTING                                                          */
   /************************************************************************/
   if (DosAllocSeg (65535U, &selSource, SEG_NONSHARED))
      return 1;

   if (DosAllocSeg (65535U, &selDest, SEG_NONSHARED))
      return 2;

   pvSource = MAKEP (selSource, 0);
   pvDest = MAKEP (selDest, 0);

   pcSource = (PCHAR) pvSource;
   for (i=0; i < 65535U; i++)
      *pcSource++ = 'b';

   printf ("Testing far pointers\n\n");

   printf ("Testing far byte copy pass 1...  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      fpbMemCpy (pvDest, pvSource, 65535U);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);

   printf ("Testing far word copy pass 1...  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      fpwMemCpy (pvDest, pvSource, 65535U);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);
   
   
   printf ("Testing far word, odd boundary.  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      fpwMemCpy (pvDest, ((PCHAR)pvSource)+1, 65534U);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);
   
   printf ("Testing _fmemcpy ..............  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      _fmemcpy (pvDest, pvSource, 65535U);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);

   /************************************************************************/
   /* NEAR TESTING                                                         */
   /************************************************************************/

   printf ("\nTesting near pointers\n\n");

   printf ("Testing near byte copy pass 1..  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      npbMemCpy (szDest, szSource, sizeof szSource);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);

   printf ("Testing near word copy pass 1..  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      npwMemCpy (szDest, szSource, sizeof szSource);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);
   
   printf ("Testing memcpy ................  ");

   ulStart = pgis->msecs;
   for (i=0; i < MAX_ITERATIONS; i++)
      memcpy (szDest, szSource, sizeof szSource);
   ulEnd = pgis->msecs;
   printf ("Time = %lu (%lu secs)\n", ulEnd - ulStart, (ulEnd - ulStart) / 1000);

   }


