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
 * Segment.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides segment based memory allocation.
 */

#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"



/*
 * ItiMemAllocSeg is used to allocate memory segments.
 *
 * Parameters: usBytesToAlloc    The number of bytes to allocate.
 *
 *             usAttribute       This contains flags for the kind
 *                               of memory object you want to create.
 *                               Usually, you will use SEG_NONSHARED.
 *                               See the description of the fsAttr
 *                               parameter of the DosAllocSeg API for
 *                               a full list.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A selector to the memory segment, or zero on error.
 *
 * Comments:
 * If you use SEG_NONSHARED as the memory attribute, you may in fact
 * get a selector to shared memory.  This occurs when the operating system
 * runs out of private selectors.  (There are about 2000 private selectors
 * per process.)
 */

PVOID EXPORT ItiMemAllocSeg (USHORT usBytesToAlloc,
                             USHORT usAttribute,
                             USHORT usFlags)

   {
   SEL      selMem;
   USHORT   usError;

   usError = DosAllocSeg (usBytesToAlloc, &selMem, usAttribute);
   if ((usError == ERROR_NOT_ENOUGH_SELECTORS_AVA ||
        usError == ERROR_NOT_ENOUGH_MEMORY) && 
       usAttribute == SEG_NONSHARED)
      {
      usError = DosAllocSeg (usBytesToAlloc, &selMem, SEG_GETTABLE |
                                                      SEG_SIZEABLE);
      if (usError)
         return NULL;
      }

   if (usFlags & MEM_ZERO_INIT)
      {
      PCHAR    pcData;
      register USHORT   i;

      pcData = MAKEP (selMem, 0);
      for (i=0; i < usBytesToAlloc; i++, pcData++)
         {
         *pcData = '\0';
         }
      }
   return MAKEP (selMem, 0);
   }



/*
 * ItiMemReallocSeg is used to reallocate a memory selector.
 *
 * Parameters: pvData            The pointer to resize.  
 *
 *             usNewSize         The number of bytes to allocate.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A pointer to the memory segment, or NULL on error.
 */

PVOID EXPORT ItiMemReallocSeg (PVOID   pvData,
                               USHORT  usNewSize,
                               USHORT  usFlags)
   {
   USHORT usError;
   USHORT usOldSize;

   if (pvData == NULL)
      return ItiMemAllocSeg (usNewSize, SEG_NONSHARED, usFlags);

   usOldSize = ItiMemQuerySeg (pvData);

   usError = DosReallocSeg (usNewSize, SELECTOROF (pvData));
   if (usError)
      return NULL;

   if ((usFlags & MEM_ZERO_INIT) && usOldSize < usNewSize)
      {
      PCHAR    pcData;
      register USHORT   i;
      register USHORT   usMax;

      pcData = ((PCHAR) pvData) + usOldSize;
      usMax = usNewSize - usOldSize;
      for (i=0; i < usMax; i++, pcData++)
         {
         *pcData = '\0';
         }
      }

   return pvData;
   }






/*
 * ItiMemQuerySeg is used to query the size of a memory segment.
 *
 * Parameters: pvData               The pointer to check.
 *
 * Return Value: The size of the pointer, in bytes.  Zero
 *               can be retured if an error occurred.
 */

USHORT EXPORT ItiMemQuerySeg (PVOID pvData)
   {
   ULONG    ulSegmentSize;
   USHORT   usError;

   usError = DosSizeSeg (SELECTOROF (pvData), &ulSegmentSize);
   if (usError || ulSegmentSize > MAX_SEG_SIZE)
      return 0;

   return (USHORT) ulSegmentSize;
   }






/*
 * ItiMemFreeSeg is used to free memory segments.
 *
 * Parameters: selMem            The selector to free.
 *
 * Return Value: None.
 */

void EXPORT ItiMemFreeSeg (PVOID pvData)
   {
   DosFreeSeg (SELECTOROF (pvData));
   }




