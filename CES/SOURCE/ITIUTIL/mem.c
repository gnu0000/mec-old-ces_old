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
 * mem.c
 * Charles Engelke
 * Mark Hampton
 * Craig Fitzgerald
 *
 * memory handling functions for large data models.
 */


/*-----------------------------------------------------------------------+
|                                                                        |
|    Copyright (c) 1989 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                        |
|    This program module is part of DS/Shell, a proprietary product of   |
|    Info Tech, Inc., no part of which may be reproduced or              |
|    transmitted in any form or by any means, electronic,                |
|    mechanical, or otherwise, including photocopying and recording      |
|    or in connection with any information storage or retrieval          |
|    system, without permission in writing from Info Tech, Inc.          |
|                                                                        |
+-----------------------------------------------------------------------*/


#include "..\include\iti.h"
#include "..\include\itiutil.h"





/*
 * ItiMemBinSearch performs a binary search on the array pvBase, looking for
 * a match of lpKey, using the compare function pCmpFunc.  nWidth is the
 * width in bytes of each element of the array, and nMax is the number of 
 * elements in the array.
 *
 *
 * Return Value: returns a pointer to the element that matches lpKey, or
 * NULL if no match found.
 */

PVOID EXPORT ItiMemBinSearch (PVOID   pvKey, 
                              PVOID   pvBase, 
                              USHORT  usMax, 
                              USHORT  usWidth,
                              SHORT   (*pfnCmpFunc) (PVOID pv1, PVOID pv2))
   {
   USHORT   usFirst, usLast, usMid;
   SHORT    sCmpVal;
   PCHAR    pcBase;

#ifdef DEBUG
   if ((ULONG) usMax * (ULONG) usWidth > (ULONG) MAX_USHORT)
      *((PCHAR)0) = 0;
#endif

   usLast = usMax;
   usFirst = 0;
   pcBase = pvBase;
   while (usFirst < usLast)
      {
      usMid = (usFirst + usLast) / 2;
      sCmpVal = pfnCmpFunc (pvKey, pcBase + usMid * usWidth);
      if (sCmpVal > 0)
         {
         usFirst = usMid + 1;
         }
      else if (sCmpVal < 0)
         {
         usLast = usMid;
         }
      else 
         {
         return pcBase + usMid * usWidth;
         }
      }
   return NULL;
   }




static VOID qsort (PSZ      lpArray,
                   USHORT   usLeft,
                   USHORT   usRight,
                   USHORT   usElementSize,
                   SHORT    (*pfnCmpFunc) (PVOID pv1, PVOID pv2))
   {
   USHORT i, usLast;

   /* do nothing if less that two elements to sort */
   if (usLeft >= usRight)
      return;

   ItiMemSwap (lpArray, usLeft, (usLeft + usRight) / 2, usElementSize);
   usLast = usLeft;

   for (i = usLeft + 1; i <= usRight; i++)
      {
      if (pfnCmpFunc (lpArray + i * usElementSize, 
                    lpArray + usLeft * usElementSize) < 0)
         {
         ItiMemSwap (lpArray, ++usLast, i, usElementSize);
         }
      }

   ItiMemSwap (lpArray, usLeft, usLast, usElementSize);
   qsort (lpArray, usLeft, usLast - 1, usElementSize, pfnCmpFunc);
   qsort (lpArray, usLast + 1, usRight, usElementSize, pfnCmpFunc);
   }




/*
 * ItiMemQSort performs a quick sort on the array pvArray. 
 * usElementSize is the size, in bytes, of each element of the array. 
 * usNumElements is the number of elements the array has. 
 * pfnCmpFunc is a compare fucntion.
 */

VOID EXPORT ItiMemQSort (PVOID    pvArray,
                         USHORT   usElementSize,
                         USHORT   usNumElements,
                         SHORT    (*pfnCmpFunc) (PVOID pv1, PVOID pv2))
   {
   qsort (pvArray, 0, usNumElements - 1, usElementSize, pfnCmpFunc);
   }






/*
 * ItiMemCpy copies usSize bytes from pvSource to pvDest.  
 * The copy is performed in a forward copy, ie pvSource and pvDest
 * are both INCREMENTED for each byte copied. 
 *
 * Return Value: ItiMemCpy always returns pvDest.
 */

PVOID EXPORT ItiMemCpy (PVOID  pvDest, PVOID  pvSource, USHORT usSize)
   {
   PUSHORT  pusDest, pusSource;
   PCHAR    pcDest, pcSource;

   pusDest = (PUSHORT) pvDest;
   pusSource = (PUSHORT) pvSource;
   while (usSize > sizeof (CHAR))
      {
      *pusDest++ = *pusSource++;
      usSize -= sizeof (USHORT);
      }

   if (usSize > 0)
      {
      pcDest = ((PCHAR) pusDest);
      pcSource = ((PCHAR) pusSource);
      *pcDest = *pcSource;
      }

   return pvDest;
   }





/*
 * ItiMemRCpy copies usSize bytes from pvSource to pvDest.  
 * The copy is performed in a reverse copy, ie pvSource and pvDest
 * are both DECREMENTED for each byte copied. 
 *
 * Return Value: ItiMemCpy always returns pvDest.
 */

PVOID EXPORT ItiMemRCpy (PVOID  pvDest, PVOID  pvSource, USHORT usSize)
   {
   PUSHORT  pusDest, pusSource;
   PCHAR    pcDest, pcSource;

   pusDest = ((PUSHORT) pvDest) + (usSize / sizeof (USHORT));
   pusSource = ((PUSHORT) pvSource) + (usSize / sizeof (USHORT));

   /* check to see if we aren't pointing to the end of the buffer 
      NON PORTABLE CODE! */
   if (usSize & 1)
      {
      pcDest = ((PCHAR) pusDest) + sizeof (CHAR);
      pcSource = ((PCHAR) pusSource) + sizeof (CHAR);
      *pcDest = *pcSource;
      usSize -= sizeof (CHAR);
      }

   while (usSize > sizeof (CHAR))
      {
      *pusDest++ = *pusSource++;
      usSize -= sizeof (USHORT);
      }

   if (usSize > 0)
      {
      pcDest = ((PCHAR) pusDest);
      pcSource = ((PCHAR) pusSource);
      *pcDest = *pcSource;
      }

   return pvDest;
   }






/*
 * ItiMemSwap swaps usSize bytes between pvBase + (usFirst * usSize) and
 * pvBase + (usSecond * usSize).  The swap is done in a forward direction.
 */

VOID EXPORT ItiMemSwap (PSZ     pvBase,
                        USHORT  usFirst,
                        USHORT  usSecond,
                        USHORT  usSize)
   {
   PSZ    psz1, psz2;
   UCHAR  c;

   psz1 = ((PSZ) pvBase) + usFirst  * usSize;
   psz2 = ((PSZ) pvBase) + usSecond * usSize;

   while (usSize--)
      {
      c = *psz1;
      *psz1++ = *psz2;
      *psz2++ = c;
      }
   }






/*
 * ItiMemSet 
 */ 

PVOID EXPORT ItiMemSet (PVOID pvData, UCHAR ucByte, USHORT usNumBytes)
   {
   PUSHORT  pusDest;
   PCHAR    pcDest;
   register USHORT usWord;

   pusDest = (PUSHORT) pvData;
   usWord = ((USHORT) ucByte) | ((USHORT) ucByte) << 8;
   while (usNumBytes > sizeof (CHAR))
      {
      *pusDest++ = ucByte;
      usNumBytes -= sizeof (USHORT);
      }

   if (usNumBytes > 0)
      {
      pcDest = ((PCHAR) pusDest);
      *pcDest = ucByte;
      }

   return pvData;
   }



///*
// * This proc is like strDup except it 
// * is length terminated
// */
//
//PVOID ItiMemDup (PVOID pSrc, HHEAP hheap, USHORT uSize)
//   {
//   PVOID pDest;
//
//   if ((pDest = ItiMemAlloc (hheap, uSize, 0)) == NULL)
//      return NULL;
//
//   ItiMemCpy (pDest, pSrc, uSize);
//   return pDest;
//   }
//
//
//


///*
// * lpmemcmp compares uiBufferSize bytes in memory areas lpBuffer1 and 
// * lpBuffer2.  The compare is done in a forward direction.
// * Return Values: 0 if memory compares OK.
// *                - if lpBuffer1 [n] < lpBuffer2 [n]
// *                + if lpBuffer1 [n] > lpBuffer2 [n]
// */
//
//int lpmemcmp (PSZ        lpBuffer1,
//              PSZ        lpBuffer2,
//              USHORT uiBufferSize)
//   {
//
//   while (uiBufferSize--)
//      {
//      if (*lpBuffer1 != *lpBuffer2)
//         return *lpBuffer1 - *lpBuffer2;
//
//      lpBuffer1++;
//      lpBuffer2++;
//      }
//
//   return 0;
//   }
//
//
///*
// * hpmemcmp compares ulBufferSize bytes in memory areas hpBuffer1 and 
// * hpBuffer2.  The compare is done in a forward direction.
// * Return Values: 0 if memory compares OK.
// *                - if hpBuffer1 [n] < hpBuffer2 [n]
// *                + if hpBuffer1 [n] > hpBuffer2 [n]
// */
//
//int hpmemcmp (char HUGE       *hpBuffer1,
//              char HUGE       *hpBuffer2,
//              unsigned long   ulBufferSize)
//   {
//
//   while (ulBufferSize--)
//      {
//      if (*hpBuffer1 != *hpBuffer2)
//         return *hpBuffer1 - *hpBuffer2;
//
//      hpBuffer1++;
//      hpBuffer2++;
//      }
//
//   return 0;
//   }
//
///*
// * hpmemset fills n bytes, starting at location hpMem, with the character
// * c.  The fill is done in a forward direction.
// */
//
//VOID hpmemset (HPSTR          hpMem,
//               char           c,
//               USHORT   n)
//   {
//   while (n--)
//      *hpMem++ = c;
//   }
//
//
//
//

