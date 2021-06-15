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
 * Heap.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides heap based memory allocation.
 */

#define INCL_WINHEAP
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "heap.h"
#include <stdio.h>
#include "..\include\itierror.h"

#if defined (DEBUG)

#define FUDGE_FACTOR 62


static USHORT usDebugHeap = 0;
static BOOL bNoHeap = FALSE;

#else

#define FUDGE_FACTOR 0

#endif



/*
 * hsemHeapAccess is used to control access to the OS/2 heap management
 * functions.  hsemHeapAccess is initialized in the InitHeapModule function.
 */

static HSEM hsemHeapAccess = NULL;

#if defined (DEBUG) 

static char szBuffer [1024];

void EXPORT ItiMemDumpHeap (HHEAP hheap)
   {
   USHORT   usHeapSize;
   USHORT   usLinePos;
   unsigned char *pcData;
   USHORT   i, iMax;

   if (hheap == NULL || bNoHeap)
      return;

   pcData = (PCHAR) hheap;
   usHeapSize = ItiMemQuerySeg (hheap);

   while (usHeapSize > 0)
      {
      usLinePos = 0;
      sprintf (szBuffer + usLinePos, "%p: ", pcData);
      usLinePos += 11;

      iMax = (usHeapSize < 16) ? usHeapSize : 16;
      for (i=0; i < iMax; i++)
         {
         sprintf (szBuffer + usLinePos, "%.2x ", pcData [i] & 0xff);
         usLinePos += 3;
         }

      sprintf (szBuffer + usLinePos, "| ");
      usLinePos += 2;

      for (i=0; i < iMax; i++)
         {
         sprintf (szBuffer + usLinePos, "%c ", (pcData [i] < ' ' ? 'ù' : pcData [i]) );
         usLinePos += 2;
         }

      ItiErrWriteDebugMessage (szBuffer);

      usHeapSize -= iMax;
      pcData += iMax;
      }
   }


#define HEAP_CHECK_CHAR    '\xDE'


void InitCheckPointer (PCHAR pcData, USHORT usSize)
   {
//   if (usSize < FUDGE_FACTOR + sizeof (USHORT))
//      {
//      ItiErrWriteDebugMessage ("InitCheckPointer: internal error");
//      return;
//      }
//
//   pcData += usSize;
//   usSize = FUDGE_FACTOR;
//
//   while (usSize)
//      {
//      pcData--;
//      *pcData = HEAP_CHECK_CHAR;
//      usSize--;
//      }
   }

#else

void EXPORT ItiMemDumpHeap (HHEAP hheap)
   {
   }

#endif



/*
 * ItiMemCreateHeap returns a handle to a heap that may be used with 
 * the ItiMem functions.
 *
 * Parameters: usHeapSize        The size in bytes for the heap.  This
 *                               is used for the initial heap size.
 *
 * Return Value: A handle to a heap, or NULL on error.
 *
 * Comments: The usHeapSize is used for the initial size of the heap.
 * Later on, the heap may grow in size.  
 *
 * The maximum heap size is MAX_HEAP_SIZE bytes, defined at the top of
 * this header file.
 *
 * Each heap is allocated as a non-shared memory segment.  You should
 * strive to minimze the number of heaps that you create, as each heap
 * takes away from the number of non-shared memory segments available to
 * a given process.  From experimentation, about 5600 empty (0 byte) 
 * heaps can be created.  
 */

HHEAP EXPORT ItiMemCreateHeap (USHORT usHeapSize)
   {
   HHEAP hheap;

#if defined (DEBUG)
   char szTemp [100];

   if (bNoHeap)
      {
      static usCount = 1;

      return MAKEP (1, usCount++);
      }
#endif

   if (usHeapSize <= MAX_HEAP_SIZE + sizeof (USHORT))
      usHeapSize += sizeof (USHORT);
   else
      {
#if defined (DEBUG)
      char szTemp [100];

      sprintf (szTemp, "ItiMemCreateHeap: Could not allocate a %5u byte heap.", usHeapSize);
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }

   hheap = WinCreateHeap (0, usHeapSize, 0, 0, 0, 0);

#if defined (DEBUG)
   if (usDebugHeap > 0)
      {
      sprintf (szTemp, "ItiMemCreateHeap: Allocated %5u byte heap: %p.", usHeapSize, hheap);
      ItiErrWriteDebugMessage (szTemp);
      }
#endif

   return hheap;
   }  





/*
 * ItiMemDestroyHeap destroys a previously created heap.
 *
 * Parameters: hheap       The handle to the heap to destroy.
 *
 * Return Value: None.
 *
 * Comments: All allocated memory objects in the heap are destroyed.
 */

void EXPORT ItiMemDestroyHeap (HHEAP hheap)
   {
#if defined (DEBUG)

   if (bNoHeap)
      {
      return;
      }
#endif

   if (OFFSETOF (hheap) != 0 || hheap == NULL)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemDestroyHeap: Caller passed a bad heap: %p", hheap);
      ItiErrDisplayHardError (szTemp);
      return;
      }

#if defined (DEBUG)
   if (usDebugHeap > 0)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemDestroyHeap: Destroying heap: ........ %p", hheap);
      ItiErrWriteDebugMessage (szTemp);
      }
#endif

   WinDestroyHeap (hheap);
   }  





/*
 * ItiMemAlloc allocates usNumBytes from the specified heap.
 *
 * Parameters: hheap             A handle to the heap to allocate
 *                               from.
 *
 *             usNumBytes        The number of bytes to allocate.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A pointer to the allocated space, or NULL if no memory
 * could be allocated.
 *
 * Comments:
 * The largest chunk of memory that can be allocated is MAX_HEAP_SIZE bytes.
 */

PVOID EXPORT ItiMemAlloc (HHEAP  hheap, 
                          USHORT usNumBytes,
                          USHORT usFlags)
   {
   PUSHORT  pusData;
   PVOID    pvHeap;
   NPBYTE   npb;
   PCHAR    pcData;
   USHORT   i;

#if defined (DEBUG)
   if (bNoHeap)
      {
      return ItiMemAllocSeg (usNumBytes, SEG_NONSHARED, usFlags);
      }
#endif

   if (OFFSETOF (hheap) != 0 || hheap == NULL)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemAlloc: Caller passed a bad heap: %p", hheap);
      ItiErrDisplayHardError (szTemp);
      return NULL;
      }

   if (usNumBytes > (MAX_HEAP_SIZE - FUDGE_FACTOR) + sizeof (USHORT))
      {
#if defined (DEBUG)
      char  szTemp [200];

      sprintf (szTemp, "ItiMemAlloc: Caller requested too large of a pointer (%5u bytes) from heap %p.", usNumBytes, hheap);
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }

   usNumBytes += sizeof (USHORT) + FUDGE_FACTOR;

   if (0 == ItiSemRequestMutex (hsemHeapAccess, SEM_INDEFINITE_WAIT))
      {
      npb = WinAllocMem (hheap, usNumBytes);
      ItiSemReleaseMutex (hsemHeapAccess);
      }
   else
      {
#if defined (DEBUG)
      char szTemp [100];

      sprintf (szTemp, "ItiMemAlloc: Could not get semaphore.");
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }

   if (npb != NULL)
      {
      /* get a pointer to the data */
      pvHeap = WinLockHeap (hheap);
      pusData = MAKEP (SELECTOROF (pvHeap), (USHORT) npb);

#if defined (DEBUG)
      if (usDebugHeap > 4)
         {
         char szTemp [200];

         sprintf (szTemp, "ItiMemAlloc: Allocated %5u bytes from heap %p. Pointer = %p, SizeSeg = %5u, HeapAvail = %5u.", 
                  usNumBytes, hheap, pusData, 
                  ItiMemQuerySeg (hheap), ItiMemQueryAvail (hheap));
         ItiErrWriteDebugMessage (szTemp);
         }
#endif
      /* store the size of the memory block */
      *pusData = usNumBytes;
      pusData++;

      /* initialize the memory, if so requested */
      if (usFlags & MEM_ZERO_INIT)
         {
         usNumBytes -= sizeof (USHORT);
         pcData = (PCHAR) pusData;
         for (i=0; i < usNumBytes; i++, pcData++)
            {
            *pcData = '\0';
            }
         usNumBytes += sizeof (USHORT);
         }
#if defined (DEBUG)
      InitCheckPointer ((PCHAR) (pusData - 1), usNumBytes);
#endif
      }
   else
      {
#if defined (DEBUG)
      char szTemp [200];

      sprintf (szTemp, "ItiMemAlloc: Could not allocate a %5u bytes from heap %p. SizeSeg = %u, HeapAvail = %u", usNumBytes, hheap, ItiMemQuerySeg (hheap), ItiMemQueryAvail (hheap));
      ItiErrWriteDebugMessage (szTemp);
#endif
      pusData = NULL;
      }

   return (PVOID) pusData;
   }





/*
 * ItiMemRealloc allocats usNumBytes from the specified heap.
 *
 * Parameters: hheap          A handle to the heap to allocate
 *                            from.
 *
 *             pvData         A pointer in the heap to resize.
 *
 *             usNewSize      The new size in bytes of the memory block.
 *
 *             usFlags        Memory allocation flags.  See the 
 *                            Memory allocation flags section at
 *                            top of this header file for the
 *                            allowable values.
 *
 * Return Value: A pointer to the reallocated space, or NULL if no memory
 * could be reallocated.  If no memory could be reallocated, the old
 * pointer is still valid.
 *
 * Comments:
 * The largest chunk of memory that can be allocated is MAX_HEAP_SIZE bytes.
 */

PVOID EXPORT ItiMemRealloc (HHEAP   hheap, 
                            PVOID   pvData, 
                            USHORT  usNewSize,
                            USHORT  usFlags)
   {
   PUSHORT  pusData;
   PUSHORT  pusOldData;
   USHORT   usOldSize;
   NPBYTE   npb;
   PVOID    pvHeap;
   USHORT   i;
   PCHAR    pcData;
   USHORT   usMax;

#if defined (DEBUG)
   if (bNoHeap)
      {
      return ItiMemReallocSeg (pvData, usNewSize, usFlags);
      }
#endif 

   if (pvData == NULL)
      return ItiMemAlloc (hheap, usNewSize, usFlags);

   if (usNewSize > (MAX_HEAP_SIZE - FUDGE_FACTOR) + sizeof (USHORT))
      {
#if defined (DEBUG)
      char szTemp [200];

      sprintf (szTemp, "ItiMemRealloc: Caller requested too large of a pointer (%5u bytes) from heap %p.", usNewSize, hheap);
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }

   if (SELECTOROF (hheap) != SELECTOROF (pvData))
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemRealloc: Caller tried to realloc pointer %p from heap %p:\nWrong heap!", pvData, hheap);
      ItiErrDisplayHardError (szTemp);
      return NULL;
      }

   if (OFFSETOF (hheap) != 0 || hheap == NULL)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemRealloc: Caller passed a bad heap: %p", hheap);
      ItiErrDisplayHardError (szTemp);
      return NULL;
      }

   usNewSize += sizeof (USHORT) + FUDGE_FACTOR;
   pusData = (PUSHORT) pvData;
   pusData--;
   pusOldData = pusData;
   usOldSize = *pusData;

   npb = (NPBYTE) OFFSETOF (pusData);
   if (0 == ItiSemRequestMutex (hsemHeapAccess, SEM_INDEFINITE_WAIT))
      {
      npb = WinReallocMem (hheap, npb, usOldSize, usNewSize);
      ItiSemReleaseMutex (hsemHeapAccess);
      }
   else
      {
#if defined (DEBUG)
      char szTemp [100];

      sprintf (szTemp, "ItiMemRealloc: Could not get semaphore.");
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }


   if (npb != NULL)
      {
#if defined (DEBUG)
#endif

      /* get a pointer to the data */
      pvHeap = WinLockHeap (hheap);
      pusData = MAKEP (SELECTOROF (pvHeap), (USHORT) npb);

#if defined (DEBUG)
      if (usDebugHeap > 4)
         {
         char szTemp [200];

         sprintf (szTemp, "ItiMemRealloc: Realloc %5u bytes in heap   %p. Pointer = %p, SizeSeg = %5u, HeapAvail = %5u.  Old = %p (%5u bytes)", 
                  usNewSize, hheap, pusData, ItiMemQuerySeg (hheap), 
                  ItiMemQueryAvail (hheap), pusOldData, usOldSize);
         ItiErrWriteDebugMessage (szTemp);
         }
#endif

      /* store the size of the memory block */
      *pusData = usNewSize;
      pusData++;

      /* initialize the memory, if so requested */
#if defined (DEBUG)
      if (usOldSize < usNewSize)
#else
      if ((usFlags & MEM_ZERO_INIT) && usOldSize < usNewSize)
#endif
         {
         pcData = (PCHAR) pusData - sizeof (USHORT) + usOldSize;
         usMax = usNewSize - usOldSize;
         for (i=0; i < usMax; i++, pcData++)
            {
            *pcData = '\0';
            }
         }
#if defined (DEBUG)
      InitCheckPointer ((PCHAR) (pusData - 1), usNewSize);
#endif
      }
   else
      {
#if defined (DEBUG)
      char szTemp [200];

      sprintf (szTemp, "ItiMemRealloc: Could not reallocate pointer %p (%5u bytes) to %5u bytes from heap %p.  SizeSeg = %u, HeapAvail = %u", pusOldData, usOldSize, usNewSize, hheap, ItiMemQuerySeg (hheap), ItiMemQueryAvail (hheap));
      ItiErrWriteDebugMessage (szTemp);
#endif
      pusData = NULL;
      }

   return (PVOID) pusData;
   }






/*
 * ItiMemQuerySize returns the size of a pointer that was 
 * allocated with ItiMemAlloc or ItiMemRealloc.
 *
 * Parameters: pvData         A pointer that was previously returned
 *                            from the ItiMemAlloc or ItiMemRealloc
 *                            function.
 *
 * Notes: If you call this function with a pointer that was not returned
 * from the ItiMemAlloc or ItiMemRealloc function, all hell will break 
 * lose.
 */

USHORT EXPORT ItiMemQuerySize (PVOID pvData)
   {
   PUSHORT pusData;

#if defined (DEBUG)
   if (bNoHeap)
      {
      return ItiMemQuerySeg (pvData);
      }
#endif

   pusData = (PUSHORT) pvData;
   pusData--;
   return *pusData - sizeof (USHORT) - FUDGE_FACTOR;
   }





/*
 * ItiMemQueryAvail returns the size of the largest available
 * memory block in the specified heap.
 *
 * Parameters: hheap          The heap to examine.
 *
 * Return value: The size in bytes of the largest memory block.
 * May be zero on error.
 */

USHORT EXPORT ItiMemQueryAvail (HHEAP hheap)
   {
   USHORT usReturnValue;

#if defined (DEBUG)
   if (bNoHeap)
      {
      return 65535;
      }
#endif 

   if (OFFSETOF (hheap) != 0 || hheap == NULL)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemQueryAvail: Caller passed a bad heap: %p", hheap);
      ItiErrDisplayHardError (szTemp);
      return 0;
      }

   if (0 == ItiSemRequestMutex (hsemHeapAccess, SEM_INDEFINITE_WAIT))
      {
      usReturnValue = WinAvailMem (hheap, TRUE, 0);
      ItiSemReleaseMutex (hsemHeapAccess);
      }
   else
      return 0;

   if (usReturnValue == 0xffff)
      {
      /* error */
      return 0;
      }
   return usReturnValue;
   }




/*
 * ItiMemFree frees the specified memory from the specified heap.
 *
 * Parameters: hheap             A handle to the heap that the memory
 *                               was allocated from.
 *
 *             pvData            A pointer that was previously returned
 *                               by the ItiMemAlloc function.
 *
 * Return value: None.
 */

void EXPORT ItiMemFree (HHEAP hheap, 
                        PVOID pvData)
   {
   PUSHORT  pusSize;
   USHORT   usSize;

#if defined (DEBUG)
   if (bNoHeap)
      {
      ItiMemFreeSeg (pvData);
      return;
      }
#endif 

   if (pvData == NULL)
      {
#if defined (DEBUG)
      if (usDebugHeap > 1)
         {
         char szTemp [200];

         sprintf (szTemp, "ItiMemFree: CALLER tried to free a null pointer from heap %p.", hheap);
         ItiErrWriteDebugMessage (szTemp);
         return;
         }
#endif

      return;
      }

   if (SELECTOROF (hheap) != SELECTOROF (pvData))
      {
      char szTemp [100] = "";

      sprintf (szTemp, "ItiMemFree: Caller tried to free pointer %p from heap %p:\nWrong heap!", pvData, hheap);
      ItiErrDisplayHardError (szTemp);
      ;
      return;
      }

   if (OFFSETOF (hheap) != 0 || hheap == NULL)
      {
      char szTemp [100];

      sprintf (szTemp, "ItiMemFree: Caller passed a bad heap: %p", hheap);
      ItiErrDisplayHardError (szTemp);
      return;
      }

   pusSize = (PUSHORT) pvData;
   pusSize--;
   usSize = *pusSize;
#if defined (DEBUG)
   if (usDebugHeap > 4)
      {
      char szTemp [200];

      sprintf (szTemp, "ItiMemFree:  Freeing   %5u bytes from heap %p. Pointer = %p", usSize, hheap, pusSize);
      ItiErrWriteDebugMessage (szTemp);
      }
#endif
   WinFreeMem (hheap, (NPBYTE) OFFSETOF (pusSize), usSize);
   }




/*
 * InitHeapModule is called by DLLInit.c during DLL initialization.
 * Note that InitHealpModule MUST BE CALLED AFTER the semaphore 
 * module is initialized.
 *
 * This function returns TRUE if initialization was successful, 
 * or FALSE if not.
 */

BOOL InitHeapModule (HMODULE hmodDll)
   {
   USHORT usError;

#if defined (DEBUG)
   PSZ pszTemp;

   if (0 == DosScanEnv ("NOHEAP", &pszTemp) && pszTemp != NULL)
      bNoHeap = TRUE;

   if (0 == DosScanEnv ("DEBUGHEAP", &pszTemp) && pszTemp != NULL)
      usDebugHeap = *pszTemp - '0';

#endif

   usError = ItiSemCreateMutex (NULL, 0, &hsemHeapAccess);
   if (usError)
      return FALSE;
   else
      return TRUE;
   }


