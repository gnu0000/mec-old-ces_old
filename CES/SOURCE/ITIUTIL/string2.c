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
 * string2.c
 *
 */

#define INCL_WIN
#define INCL_DOSMISC
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


/*
 * Eats a word from the front of a string and returns
 * that word and the rest of the string.
 *
 * 1> skip spaces and tabs
 * 2> read to delimeter character (exclusive)
 * 3> word ends at char before delim, string starts at char after delim
 * 4> returns word read and remaining string as parameters
 * 5> delimeter char is returns as fn val.
 *
 * returns 0xFFFF when there is nothing to return
 */
USHORT EXPORT ItiGetWord (PSZ *ppsz, PSZ pszWord, PSZ pszDelimeters, BOOL bEatDelim)
   {
   *pszWord = '\0';
   ItiStrip (*ppsz, " \t");

   if (*ppsz == NULL || **ppsz == '\0')
      return 0xFFFF;

   while (**ppsz != '\0' && strchr (pszDelimeters, **ppsz) == NULL)
      *(pszWord++) = *((*ppsz)++);
   *pszWord = '\0';

   if (bEatDelim)
      return (USHORT) (**ppsz ? *((*ppsz)++): 0);
   else
      return (USHORT) (**ppsz ? *(*ppsz): 0);
   }




/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
USHORT EXPORT ItiEat (PSZ *ppszStr, PSZ pszTarget)
   {
   char szWord [512];

   if ((ItiGetWord (ppszStr, szWord, " ,\t\n", TRUE)) == 0xFFFF)
      return 2;
   return !!stricmp (pszTarget, szWord);
   }





/*
 * Removes trailing chars from str
 */
PSZ EXPORT ItiClip (PSZ pszStr, PSZ pszClipList)
   {
   SHORT i;

   i = ItiStrLen (pszStr);
   while (i >= 0 && strchr (pszClipList, pszStr[i]) != NULL)
      pszStr[i--] = '\0';
   return pszStr;
   }



/*
 * Removes leading chars from str
 */
PSZ EXPORT ItiStrip (PSZ pszStr, PSZ pszStripList)
   {
   SHORT sLen, i = 0;

   if (!(sLen = ItiStrLen (pszStr)))
      return pszStr;
   while (i < sLen && strchr (pszStripList, pszStr[i]) != NULL)
      i++;
   if (i)
      memmove (pszStr, pszStr + i, sLen - i + 1);
   return pszStr;
   }


/*
 * Removes chars from anywhere in string
 *
 */
PSZ EXPORT ItiExtract (PSZ pszStr, PSZ pszExtractList)
   {
   PSZ pszPtrSrc, pszPtrDest;

   if (pszStr == NULL)
      return NULL;

   pszPtrSrc = pszPtrDest = pszStr;
   while (*pszPtrSrc != '\0')
      {
      if (strchr (pszExtractList, *pszPtrSrc) == NULL)
         *pszPtrDest++ = *pszPtrSrc;
      pszPtrSrc++;
      }
   *pszPtrDest = '\0';
   return pszStr;
   }




/* if 1st 2 chars are double quotes, this fn eats the first 2 chars
 * and returns true.
 * else the fn returns false
 */
BOOL EXPORT ItiTwoQuotes (PSZ *ppsz)
   {
   if (*ppsz == NULL || **ppsz == '\0')
      return FALSE;
   if ((*ppsz)[0] == '"' && (*ppsz)[1] == '"')
      return !!(*ppsz += 2);
   return FALSE;
   }



/* if 1st char is double quote, this fn eats the first char
 * and returns true.
 * else the fn returns false
 */
BOOL EXPORT ItiSingleQuotes (PSZ *ppsz)
   {
   return (**ppsz == '"' ? !!((*ppsz)++) : FALSE);
   }



/* Standard String Dup
 *
 */
PSZ EXPORT ItiStrDup(HHEAP hheap, PSZ pszSourceStr)
   {
   USHORT i;
   PSZ pszDestStr;

   if (NULL == pszSourceStr)
      {
      ItiErrWriteDebugMessage (" *** Null pszSourceStr passed to ItiStrDup.");
      return NULL;
      }

   if ((pszDestStr = ItiMemAlloc(hheap, ItiStrLen(pszSourceStr)+1, 0))== NULL)
      return NULL;
   for (i = 0; pszSourceStr[i] != '\0'; i++)
      pszDestStr[i] = pszSourceStr[i];
   pszDestStr[i] = '\0';
   return(pszDestStr);
   }



/*
 * Standard Mem Dup
 */
PVOID EXPORT ItiMemDup (HHEAP hheap, PVOID pSrc, USHORT usSize)
   {
   PVOID pDest;

   if ((pDest = ItiMemAlloc (hheap, usSize, 0)) == NULL)
      return NULL;
   ItiMemCpy (pDest, pSrc, usSize);
   return pDest;
   }




/*
 * frees a ptr to array of str
 */
void EXPORT ItiFreeStrArray (HHEAP hheap, PSZ *ppsz, USHORT usCount)
   {
   if (ppsz == NULL)
      return;

   while ((usCount != 0) && (usCount--))
      if (ppsz[usCount] != NULL)
         ItiMemFree (hheap, ppsz[usCount]);

   if ((ppsz != NULL) && ((*ppsz) != NULL))
      ItiMemFree (hheap, ppsz);
   }


void EXPORT ItiFreeStrArray2 (HHEAP hheap, PSZ *ppsz)
   {
   USHORT usCount;

   if (ppsz == NULL)
      return;
   for (usCount = 0; ppsz [usCount]; usCount++)
      ItiMemFree (hheap, ppsz [usCount]);
   ItiMemFree (hheap, ppsz);
   }




USHORT EXPORT ItiOpenProfile (HAB hab, PSZ pszIniFileName, PHINI phiniPrf)
   {
   *phiniPrf = PrfOpenProfile(hab, pszIniFileName);

   if (*phiniPrf != NULL)
      return 0;
   else
      return 13;
   }


USHORT EXPORT ItiCloseProfile (PHINI phiniPrf)
   {
   if (PrfCloseProfile(*phiniPrf))
      return 0;
   else
      return 13;
   }



USHORT EXPORT ItiGetProfileString (HINI   hiniPrf,
                                   PSZ    pszAppName,
                                   PSZ    pszStrName,
                                   PSZ    pszStrBuffer,
                                   USHORT usStrBufferLength,
                                   PSZ    pszDefaultBuffer)
   {
   PSZ    pszEnv = NULL;
   ULONG  ul;
   USHORT us;

   /* -- The environment string overrides the ini file string. */
   us = DosScanEnv(pszStrName, &pszEnv);
   if ((us != ERROR_ENVVAR_NOT_FOUND) && (pszEnv != NULL))
      {       
      ItiStrCpy(pszStrBuffer, pszEnv, usStrBufferLength);
      return 0;
      }
   else
      {
      if (hiniPrf != NULL)
         {
         ul = PrfQueryProfileString(hiniPrf,
                                    pszAppName,
                                    pszStrName,
                                    pszDefaultBuffer,
                                    pszStrBuffer,
                                    usStrBufferLength);
         return 0L;
         }
      else
         {
         ItiStrCpy(pszStrBuffer, pszDefaultBuffer, usStrBufferLength);
         return (13);
         }/* end if (hiniPrf... else clause */
      }/* end if (pszEnv... else clause */


   return (0);
   }/* End of function */






USHORT EXPORT ItiWriteProfileString (HINI hiniPrf,
                                     PSZ  pszAppName,
                                     PSZ  pszStrName,
                                     PSZ  pszStrValue)
   {
   if (PrfWriteProfileString (hiniPrf, pszAppName, pszStrName, pszStrValue))
      return 0;
   else
      return 13;
   }



// 
// Original version
// USHORT EXPORT ItiStrReplaceParams (PSZ    pszDest,
//                                    PSZ    pszSource,
//                                    USHORT usDestMax,
//                                    PSZ    apszTokens [], 
//                                    PSZ    apszValues [], 
//                                    USHORT usNumValues)
//    {
//    PSZ      pszEnd;
//    USHORT   usNotFound = 1;
//    USHORT   usLen, i;
//    char     szToken [MAX_TOKEN_SIZE + 1];
//    BOOL     bFound;
// 
//    /* 7/9/92 mdh: added this to allow NULL for value or token parameter. 
//       this is used by the DSSIMP module. */
//    if (apszTokens == NULL || apszValues == NULL)
//       {
//       while (*pszSource && usDestMax > 1)
//          {
//          usNotFound += *pszSource == '%';
//          *pszDest++ = *pszSource++;
// 
// //       930105-4101 mdh 1/19/93: added decrement of usDestMax to
// //       prevent overwriting of memory
//          usDestMax--;
//          }
//       *pszDest = 0;
//       return usNotFound;
//       }
// 
//    usNotFound = 0;
// 
// // 930105-4101 mdh 1/19/93: changed check of usDestMax -- off by one error
//    while (*pszSource && usDestMax > 1)
//       {
//       if (*pszSource == '%')
//          {
//          pszSource++;
//          if (*pszSource == '%')
//             {
//             *pszDest++ = *pszSource++;
//             usDestMax--;
//             }
//          else
//             {
//             for (pszEnd = pszSource; 
//                  *pszEnd && ((*pszEnd >= 'a' && *pszEnd <= 'z') || 
//                              (*pszEnd >= 'A' && *pszEnd <= 'Z') ||
//                              (*pszEnd >= '0' && *pszEnd <= '9') ||
//                              *pszEnd == '_');
//                  pszEnd++)
//                ;
//             ItiStrCpy (szToken, pszSource, sizeof szToken < pszEnd - pszSource + 1? 
//                                            sizeof szToken : pszEnd - pszSource + 1);
//             for (i=0, bFound = FALSE; apszTokens [i] && i < usNumValues; i++)
//                if ((bFound = ItiStrICmp (apszTokens [i], szToken) == 0))
//                   break;
//             if (bFound)
//                {
//                if (apszValues [i] == NULL)
//                   {
//                   *pszDest = '\0';
//                   usNotFound++; /* Returned to indicate that a null value was */ 
//                                 /* given for a token value, ie. a probable    */
//                                 /* missing key value.    (tlb, 14 Sep 92)     */
//                   }
//                else
//                   ItiStrCpy (pszDest, apszValues [i], usDestMax);
//                usLen = ItiStrLen (pszDest);
//                pszDest += usLen;
//                usDestMax -= usLen;
//                pszSource = pszEnd;
//                }
//             else
//                {
//                /* token not found, so just put the %token in the string */
//                usNotFound++;
//                *pszDest++ = '%';
//                usDestMax--;
//                }
//             }
//          }
//       else
//          {
//          *pszDest++ = *pszSource++;
//          usDestMax--;
//          }
//       }
//    *pszDest = '\0';
//    return usNotFound;
//    }
// 

// Modified function
USHORT EXPORT ItiStrReplaceParams (PSZ    pszDest,
                                   PSZ    pszSource,
                                   USHORT usDestMax,
                                   PSZ    apszTokens [], 
                                   PSZ    apszValues [], 
                                   USHORT usNumValues)
   {
 static  PSZ    pszDestOrg;
 static  PSZ    pszSourceOrg;
 static  USHORT usDestMaxOrg;
   PSZ      pszEnd;
   USHORT   usNotFound = 1;
   USHORT   usLen, i, x;
   char     szToken [MAX_TOKEN_SIZE + 1];
   BOOL     bFound;

   /* 7/9/92 mdh: added this to allow NULL for value or token parameter. 
      this is used by the DSSIMP module. */
   if (apszTokens == NULL || apszValues == NULL)
      {
      while (*pszSource && usDestMax > 1)
         {
         usNotFound += *pszSource == '%';
         *pszDest++ = *pszSource++;

//       930105-4101 mdh 1/19/93: added decrement of usDestMax to
//       prevent overwriting of memory
         usDestMax--;
         }
      *pszDest = 0;
      return usNotFound;
      }

   usNotFound = 0;

usDestMaxOrg = usDestMax;
pszDestOrg   = pszDest;
pszSourceOrg = pszSource;
for (x=0; x<usNumValues; x++)  //this is to find duplicate parameters.
   {
   pszDest  = pszDestOrg;
   pszSource = pszSourceOrg;
   usDestMax = usDestMaxOrg;

// 930105-4101 mdh 1/19/93: changed check of usDestMax -- off by one error
   while (*pszSource && usDestMax > 1)
      {
      if (*pszSource == '%')
         {
         pszSource++;
         if (*pszSource == '%')
            {
            *pszDest++ = *pszSource++;
            usDestMax--;
            }
         else
            {
            for (pszEnd = pszSource; 
                 *pszEnd && ((*pszEnd >= 'a' && *pszEnd <= 'z') || 
                             (*pszEnd >= 'A' && *pszEnd <= 'Z') ||
                             (*pszEnd >= '0' && *pszEnd <= '9') ||
                             *pszEnd == '_');
                 pszEnd++)
               ;
            ItiStrCpy (szToken, pszSource, sizeof szToken < pszEnd - pszSource + 1? 
                                           sizeof szToken : pszEnd - pszSource + 1);
            for (i=0, bFound = FALSE; apszTokens [i] && i < usNumValues; i++)
               if ((bFound = ItiStrICmp (apszTokens [i], szToken) == 0))
                  break;
            if (bFound)
               {
               if (apszValues [i] == NULL)
                  {
                  *pszDest = '\0';
                  usNotFound++; /* Returned to indicate that a null value was */ 
                                /* given for a token value, ie. a probable    */
                                /* missing key value.    (tlb, 14 Sep 92)     */
                  }
               else
                  ItiStrCpy (pszDest, apszValues [i], usDestMax);
               //usLen = ItiStrLen (pszDest);
               usLen = ItiStrLen (apszValues [i]);
               pszDest += usLen;
               usDestMax -= usLen;
               pszSource = pszEnd;
               }
            else
               {
               /* token not found, so just put the %token in the string */
               usNotFound++;
               *pszDest++ = '%';
               usDestMax--;
               }
            }
         }
      else
         {
         *pszDest++ = *pszSource++;
         usDestMax--;
         }
      }/* end of while */


   *pszDest = '\0';

   }/* end of for (x... */

// ItiErrWriteDebugMessage (pszDestOrg);

   return usNotFound;
   }/* End of Function */






PSZ *_far _cdecl _loadds ItiStrMakePPSZ (HHEAP hheap, USHORT usCount, ...)
   {
   va_list  va;
   PSZ      *ppsz, psz;
   USHORT   i;

   if (usCount == 0)
      return NULL;

   va_start (va, usCount);      /* Initialize variable arguments  */

   ppsz = ItiMemAlloc (hheap, usCount * sizeof (PSZ), 0);
   if (ppsz == NULL)
      return NULL;

   for (i=0; i < usCount; i++)
      {
      psz = va_arg (va, PSZ);

      ppsz [i] = (psz ? ItiStrDup (hheap, psz) : NULL);
      }
   return ppsz;
   }



PSZ *_far _cdecl _loadds ItiStrMakePPSZ2 (HHEAP hheap, ...)
   {
   va_list  va;
   PSZ      *ppsz, psz;
   USHORT   i, usCount;

   va_start (va, hheap);      /* Initialize variable arguments  */

   psz = va_arg (va, PSZ);
   for (usCount = 1; psz; usCount++)
      psz = va_arg (va, PSZ);

   va_start (va, hheap);      /* Initialize variable arguments  */
   ppsz = ItiMemAlloc (hheap, usCount * sizeof (PSZ), 0);
   if (ppsz == NULL)
      return NULL;

   for (i=0; i < usCount; i++)
      {
      psz = va_arg (va, PSZ);

      ppsz [i] = (psz ? ItiStrDup (hheap, psz) : NULL);
      }
   return ppsz;
   }



BOOL EXPORT ItiStrIsBlank (PSZ psz)
   {
   if (psz == NULL)
      return TRUE;
   while (*psz != '\0' && ItiCharIsSpace (*psz))
      psz++;
   return *psz == '\0';
   }


/* this fills a ppsz with usCount bits of data
 *
 */

PPSZ _far _cdecl _loadds ItiFillPPSZ (PPSZ ppszList, USHORT usCount, ...)
   {
   va_list  va;
   USHORT   i;

   va_start (va, usCount);
   for (i=0; i<usCount; i++)
      ppszList[i] = va_arg (va, PSZ);
   return ppszList;
   }


/* -- Gets just the requested key value from the parameter list. */
/* -- Returns: 0 okay or non-zero for error.                     */
USHORT EXPORT ItiStrGet1Param (PSZ    pszDest,
                               PSZ    pszRequestedValue,
                               USHORT usDestMax,
                               PSZ    apszTokens [], 
                               PSZ    apszValues [])
   {
   USHORT   usNotFound = 13;
   USHORT   i;
   CHAR     szToken [MAX_TOKEN_SIZE + 1];
   BOOL     bFound;
   PSZ      psz;

   if (apszTokens == NULL || apszValues == NULL
       || pszDest == NULL || pszRequestedValue == NULL)
      {
      return usNotFound;
      }

   psz = pszRequestedValue;

   if ((*psz == '%') || (*psz == ' '))
      psz++;

   ItiStrCpy (szToken, psz, sizeof szToken);
   
   bFound = FALSE;
   i=0;
   while (!bFound && apszTokens[i])
      {
      if (ItiStrICmp (apszTokens[i], szToken) == 0)
         {
         if (apszValues[i] == NULL)
            return 13;

         ItiStrCpy (pszDest, apszValues[i], usDestMax);
         bFound = TRUE;
         usNotFound = 0;
         break;
         }
      i++;
      } /* end of while */

   return usNotFound;
   } /* End of Function ItiStrGet1Param */


INT EXPORT Base36Val (CHAR ch)
   {
   CHAR c;
   INT  i;

   if (ch == '\0')
      return 0;

   if ( FALSE == ((ch >= '0') && (ch <= '9')) )
      {
      c = (CHAR) tolower(ch);
      i = (int) ( c - 'a' ) + 10;
      }
   else
      i = (int) ch - '0';

   return i;
   } /* End of Function Base36Val */



INT EXPORT ConvertStrToVal36 (PSZ psz36)
   {
   INT iVal = 0;

   while ((psz36 != NULL) && (*psz36 != '.') && (*psz36 != '\0'))
      psz36++;

   if (*psz36 == '\0')
      return 0;

   iVal =        Base36Val(psz36[1]) * 1296;
   iVal = iVal + Base36Val(psz36[2]) * 36;
   iVal = iVal + Base36Val(psz36[3]);

   return iVal;
   } /* End ofFunction ConvertStrToVal36 */


BOOL EXPORT VerComp (PSZ pszDllVer)
   {
   PSZ pszDB;
   INT iX, iY;
   HHEAP  hhp;

   hhp = ItiMemCreateHeap (1000); 

   pszDB = ItiDbGetRow1Col1 (
              " select MAX(Release) from DatabaseRelease ", hhp, 0, 0, 0);

   iX = ConvertStrToVal36 (pszDB);
   iY = ConvertStrToVal36 (pszDllVer);

   ItiMemDestroyHeap (hhp);

   if (iY >= iX)
      return TRUE;
   else
      return FALSE;

   }/* End of Function VerComp */


/* ItiVerCmp
 *
 *   The version id is  1.1a0
 *     note: the digit after the leter is the PreRelease digit, an
 *           underscore character is used for the final release version. 
 *
 * Returns true, but if the dll's version is less than
 *    the version of the application program a message
 *    is written to the ITI0000 log file.
 */
BOOL EXPORT ItiVerCmp (PSZ pszDll)
   {
   PGLOBALS  pglob;
   PSZ psz;
   CHAR szMsg[100] = "";

   pglob = ItiGlobQueryGlobalsPointer();
   psz = pglob->pszVersionString;

  if ( VerComp(pszDll) )
     {
     sprintf (szMsg,  
        "\n *** WARNING: Loaded version %s, the program version is %s. \n ", 
        pszDll, psz); 
     ItiErrWriteDebugMessage(szMsg);
     //DosBeep (1580, 10); 
     return FALSE;
     }
  else
     return TRUE;

   }/* End of Function ItiVerCmp */



