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
 * TestMem.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides functions that tell whether or not a process has
 * read/write access to a given pointer.
 */



#include "..\include\iti.h"
#include "..\include\itibase.h"


/*
 * ItiMemQueryRead returns TRUE if the caller has read access for 
 * the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has read permission for the memory,
 * or FALSE if not.
 */

BOOL EXPORT ItiMemQueryRead (PVOID pvPointer)
   {
   SEL   sel;
   BOOL  bReturnValue;
   
   sel = SELECTOROF (pvPointer);
   bReturnValue = FALSE;
   _asm 
      {
      verr  sel               ; test the selector
      jnz   done              ; jump if access not allowed
      mov   bReturnValue, 1   ; set flag to TRUE
      done:
      }

   if (bReturnValue)
      {
      bReturnValue = OFFSETOF (pvPointer) < ItiMemQuerySeg (pvPointer);
      }

   return bReturnValue;
   }



/*
 * ItiMemQueryWrite returns TRUE if the caller has write access for 
 * the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has write permission for the memory,
 * or FALSE if not.
 */

BOOL EXPORT ItiMemQueryWrite (PVOID pvPointer)
   {
   SEL   sel;
   BOOL  bReturnValue;
   
   sel = SELECTOROF (pvPointer);
   bReturnValue = FALSE;
   _asm 
      {
      verw  sel               ; test the selector
      jnz   done              ; jump if access not allowed
      mov   bReturnValue, 1   ; set flag to TRUE
      done:
      }

   if (bReturnValue)
      {
      bReturnValue = OFFSETOF (pvPointer) < ItiMemQuerySeg (pvPointer);
      }

   return bReturnValue;
   }



/*
 * ItiMemQueryReadWrite returns TRUE if the caller has read and write
 * access for the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has read and write permission 
 * for the memory, or FALSE if not.
 */

BOOL EXPORT ItiMemQueryReadWrite (PVOID pvPointer)
   {
   SEL   sel;
   BOOL  bReturnValue;
   
   sel = SELECTOROF (pvPointer);
   bReturnValue = FALSE;
   _asm 
      {
      verw  sel               ; test the selector for write access
      jnz   done              ; jump if access not allowed
      verr  sel               ; test the selector for read access
      jnz   done              ; jump if access not allowed
      mov   bReturnValue, 1   ; set flag to TRUE
      done:
      }

   if (bReturnValue)
      {
      bReturnValue = OFFSETOF (pvPointer) < ItiMemQuerySeg (pvPointer);
      }

   return bReturnValue;
   }

