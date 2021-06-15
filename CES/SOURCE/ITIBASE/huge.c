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
 * Huge.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides huge memory object allocation.
 */


#define INCL_DOSMEMMGR
#include "..\include\iti.h"
#include "..\include\itibase.h"



#define SEGMENT_SIZE    65536UL

//
//
///*
// * ItiMemAllocHuge is used to allocate > 64K memory objects.
// *
// * Parameters: ulBytesToAlloc    The number of bytes to allocate.
// *
// *             ulMaxRealloc      The maximum size to allow for resizing
// *                               the memory object.  If this value
// *                               is zero, then you are not allowed to 
// *                               grow the memory; however you are allowed
// *                               to shrink the memory.
// *
// *             usFlags           Memory allocation flags.  See the 
// *                               Memory allocation flags section at
// *                               top of this header file for the
// *                               allowable values.
// *
// * Return Value: A huge pointer to the memory, or NULL on error.
// *
// * Comments: 
// * The ulMaxRealloc parameter reserves space in the calling process's 
// * LDT, thus limiting the total number of selectors available to that
// * process.
// */
//
//HPVOID EXPORT ItiMemAllocHuge (ULONG   ulBytesToAlloc,
//                               ULONG   ulMaxRealloc,
//                               USHORT  usAttribute,
//                               USHORT  usFlags)
//   {
//   USHORT   usNumSelectors;
//   USHORT   usMaxNumSelectors;
//   USHORT   usRemainder;
//   USHORT   usError;
//   SEL      sel;   
//   
//   usNumSelectors = (USHORT) (ulBytesToAlloc / SEGMENT_SIZE);
//   usRemainder = (USHORT) (ulBytesToAlloc - 
//                           (ULONG) usNumSelectors * SEGMENT_SIZE);
//
//   usMaxNumSelectors = (USHORT) (ulMaxRealloc / SEGMENT_SIZE);
//   usError = DosAllocHuge (usNumSelectors, usRemainder, &sel, 
//                           usMaxNumSelectors, usAttribute);
//
//   if (usError)
//      {
//      return NULL;
//      }
//
//   return MAKEP (sel, 0);
//   }
//
//                                      
//                                      
///*
// * ItiMemReallocHuge is used to reallocate > 64K memory objects.
// *
// * Parameters: ulNewSize            The new size of the memory object.
// *
// * Return Value: A huge pointer to the memory, or NULL on error.
// *
// */
//
//HPVOID EXPORT ItiMemReallocHuge (HPVOID   hpvData,
//                                 ULONG    ulNewSize,
//                                 USHORT   usFlags)
//   {
//   USHORT   usNumSelectors;
//   USHORT   usRemainder;
//   USHORT   usError;
//   
//   if (hpvData == NULL)
//      return ItiMemAllocHuge (ulNewSize, 0, usFlags, SEG_NONSHARED);
//
//   usNumSelectors = (USHORT) (ulNewSize / SEGMENT_SIZE);
//   usRemainder = (USHORT) (ulNewSize - (ULONG) usNumSelectors * SEGMENT_SIZE);
//
//   usError = DosReallocHuge (usNumSelectors, usRemainder, 
//                             SELECTOROF (hpvData));
//
//   if (usError)
//      return NULL;
//   else
//      return hpvData;
//   }
//
//
//
//
//
///*
// * ItiMemQueryHugeSize returns the current size, in bytes, of 
// * a huge memory block.
// *
// * Parameters: hpvData              A huge pointer that was returned
// *                                  from either ItiMemAllocHuge or
// *                                  ItiMemReallocHuge.
// *
// * Return Value: Size in bytes of the pointer.  Zero may be
// * returned if there is an error.
// */
//
//ULONG EXPORT ItiMemQueryHugeSize (HPVOID hpvData)
//   {
//   ULONG    ulSize;
//   USHORT   usError;
//
//   usError = DosSizeSeg (SELECTOROF (hpvData), &ulSize);
//   if (usError)
//      {
//      return 0;
//      }
//   else
//      return ulSize;
//   }
//
//
//
//
//
///*
// * ItiMemFreeHuge is used to free a >64K object.
// *
// * Parameters: hpvData              A huge pointer that was returned
// *                                  from either ItiMemAllocHuge or
// *                                  ItiMemReallocHuge.
// *
// * Return Value: None.
// */
//
//void EXPORT ItiMemFreeHuge (HPVOID hpvData)
//   {
//   DosFreeSeg (SELECTOROF (hpvData));
//   }
//
//
//
//
//
//
