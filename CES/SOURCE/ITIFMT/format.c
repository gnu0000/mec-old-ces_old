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
 * Format.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"

/* include the default format modules */
#include <string.h>
#include "string.h"
#include "depend.h"
#include "binary.h"
#include "flag.h"
#include "date.h"
#include "qlevel.h"
#include "season.h"
#include "noformat.h"
#include "number.h"
#include "position.h"
#include "priv.h"
#include "prec.h"
#include "unittime.h"
#include "usertype.h"
#include "variable.h"
#include "year.h"
#include "password.h"

#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif


typedef struct
   {
   PSZ         pszFormatName;
   PFNFORMAT   pfnOutputFunction;
   PFNFORMAT   pfnInputFunction;
   } FORMATFUN;

typedef FORMATFUN *PFORMATFUN;



static FORMATFUN fmtfunList [] = 
   {
      { "Binary",          FmtBinary,         CheckBinary,         },
      { "DateTime",        FmtDateTime,       CheckDateTime,       },
      { "Dependency",      FmtDependency,     CheckDependency,     },
      { "Flag",            FmtFlag,           CheckFlag,           },
      { "Month",           FmtMonth,          CheckMonth,          },
      { "NoFormat",        FmtNoFormat,       CheckNoFormat,       },
      { "Number",          FmtNumber,         CheckNumber,         },
      { "Password",        FmtPassword,       CheckPassword,       },
      { "Position",        FmtPosition,       CheckPosition,       },
      { "Precision",       FmtPrecision,      CheckPrecision,      },
      { "Privilege",       FmtPrivilege,      CheckPrivilege,      },
      { "QuantityLevel",   FmtQuantityLevel,  CheckQuantityLevel,  },
      { "Season",          FmtSeason,         CheckSeason,         },
      { "String",          FmtString,         CheckString,         },
      { "UnitOfTime",      FmtUnitOfTime,     CheckUnitOfTime,     },
      { "UserType",        FmtUserType,       CheckUserType,       },
      { "Variable",        FmtVariable,       CheckNoFormat,       },
      { "Year",            FmtYear,           CheckYear,           },
   };




/* used when the format method cannot be found */
static PFORMATFUN pfmtfunNoFormat = NULL;

/* used when a null pointer is passed as the format method */
static PFORMATFUN pfmtfunNullFormat = NULL;



static SHORT FmtCmp (PFORMATFUN pfmt1, PFORMATFUN pfmt2)
   {
   return ItiStrCmp (pfmt1->pszFormatName, pfmt2->pszFormatName);
   }




/*
 * ItiFmtInitialize initializes the format module.  This function
 * should only be called once per process.
 *
 * Return Value: TRUE if the format module was initialized, FALSE if
 * not. 
 */

BOOL EXPORT ItiFmtInitialize (void)
   {
   FORMATFUN   fmtfun;

   fmtfun.pfnOutputFunction = NULL;
   fmtfun.pfnInputFunction = NULL;
   fmtfun.pszFormatName = "NoFormat";
   pfmtfunNoFormat = ItiMemBinSearch (&fmtfun, fmtfunList, 
                                      sizeof fmtfunList / sizeof (FORMATFUN),
                                      sizeof (FORMATFUN), FmtCmp);
   if (pfmtfunNoFormat == NULL)
      return FALSE;

   fmtfun.pszFormatName = "String";
   pfmtfunNullFormat = ItiMemBinSearch (&fmtfun, fmtfunList, 
                                        sizeof fmtfunList / sizeof (FORMATFUN),
                                        sizeof (FORMATFUN), FmtCmp);
   if (pfmtfunNullFormat == NULL)
      return FALSE;

   return TRUE;
   }



static PFORMATFUN SearchForFunction (PSZ pszFormat, PSZ *ppszFormatInfo)
   {
   PFORMATFUN  pfmtfun;
   FORMATFUN   fmtfun;
   char        szFormat [MAX_FORMAT_NAME_SIZE];
   PSZ         pszFormatInfo;

   pszFormatInfo = pszFormat;
   if (pszFormat != NULL)
      {
      while (*pszFormatInfo && *pszFormatInfo != ',')
         pszFormatInfo++;

      if (*pszFormatInfo == ',')
         {
         pszFormatInfo++;
         ItiStrCpy (szFormat, pszFormat, 
                    min (sizeof szFormat, pszFormatInfo - pszFormat));
         }
      else
         {
         ItiStrCpy (szFormat, pszFormat, 
                    min (sizeof szFormat, pszFormatInfo - pszFormat + 1));
         }
   
      fmtfun.pfnOutputFunction = NULL;
      fmtfun.pfnInputFunction = NULL;
      fmtfun.pszFormatName = szFormat;
   
      pfmtfun = ItiMemBinSearch (&fmtfun, fmtfunList, 
                                 sizeof fmtfunList / sizeof (FORMATFUN),
                                 sizeof (FORMATFUN), FmtCmp);
      }
   else
      pfmtfun = pfmtfunNullFormat;

   if (pfmtfun == NULL)
      pfmtfun = pfmtfunNoFormat;

   *ppszFormatInfo = pszFormatInfo;
   return pfmtfun;
   }



/*
 * ItiFmtFormatString formats a string.
 *
 * Parameters: pszSource      A pointer to the string to format.
 *
 *             pszDest        A pointer to where to store the 
 *                            formated string.
 *
 *             usDestMax      The maximum number of characters to store
 *                            in pszDest.
 *
 *             pszFormat      A pointer to the string that contains
 *                            formatting information.
 *
 *             pusDestLength  A pointer to a USHORT that recieves the number
 *                            of characters stored in pszDest, excluding
 *                            the null terminator.  This parameter may be
 *                            NULL.
 *
 * Return value: 0 if sucsessful,
 *               ITIFMT_TRUNCATED if pszDest was truncated because of space
 *               ITIFMT_FORMAT_ERROR if pszFormat was in error.
 *               ITIFMT_BAD_DATA if the data passed to the format function 
 *               is invalid or out of range.
 */

USHORT EXPORT ItiFmtFormatString (PSZ     pszSource, 
                                  PSZ     pszDest,
                                  USHORT  usDestMax,
                                  PSZ     pszFormat,
                                  PUSHORT pusDestLength)
   {
   PFORMATFUN  pfmtfun;
   USHORT      usReturn;
   PSZ         pszFormatInfo;

   if (pszDest == NULL || pszSource == NULL)
      {
      if (pszDest != NULL)
         ItiStrCpy (pszDest, "-Bad param-", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      return ITIFMT_FORMAT_ERROR;
      }

   pfmtfun = SearchForFunction (pszFormat, &pszFormatInfo);
   if (pfmtfun != NULL)
      {
      usReturn = pfmtfun->pfnOutputFunction (pszSource, pszDest, usDestMax,
                                             pszFormatInfo, pusDestLength);
      }
   else
      usReturn = ITIFMT_FORMAT_ERROR;

   if (usReturn == ITIFMT_FORMAT_ERROR)
      {
#if defined (DEBUG)
      ItiStrCpy (pszDest, "*Bad format \"", usDestMax);
      ItiStrCat (pszDest, pszFormat, usDestMax);
      ItiStrCat (pszDest, "\"*", usDestMax);
#else
      ItiStrCpy (pszDest, "*Bad format*", usDestMax);
#endif 
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      }
   else if (usReturn == ITIFMT_BAD_DATA)
      {
#if defined (DEBUG)
      ItiStrCpy (pszDest, "!Bad data \"", usDestMax);
      ItiStrCat (pszDest, pszSource, usDestMax);
      ItiStrCat (pszDest, "\"!", usDestMax);
#else
      ItiStrCpy (pszDest, "!Bad data!", usDestMax);
#endif 
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      }

   return usReturn;
   }





/*
 * ItiFmtCheckString checks a string for valid data, and prepares it
 * for sending to the database.
 *
 * Parameters: pszSource      A pointer to the string to check.
 *
 *             pszDest        A pointer to where to store the 
 *                            formated string.
 *
 *             usDestMax      The maximum number of characters to store
 *                            in pszDest.
 *
 *             pszFormat      A pointer to the string that contains
 *                            formatting information.
 *
 *             pusDestLength  A pointer to a USHORT that recieves the number
 *                            of characters stored in pszDest, excluding
 *                            the null terminator.  This parameter may be
 *                            NULL.
 *
 * Return value: 0 if sucsessful,
 *               ITIFMT_TRUNCATED if pszDest was truncated because of space
 *               ITIFMT_FORMAT_ERROR if pszFormat was in error.
 *               ITIFMT_BAD_DATA if the data passed to the format function 
 *               is invalid or out of range.
 */

USHORT EXPORT ItiFmtCheckString (PSZ     pszSource, 
                                 PSZ     pszDest,
                                 USHORT  usDestMax,
                                 PSZ     pszFormat,
                                 PUSHORT pusDestLength)
   {
   PFORMATFUN  pfmtfun;
   USHORT      usReturn;
   PSZ         pszFormatInfo;
   USHORT      i;

   if (pszDest == NULL || pszSource == NULL)
      {
      if (pszDest != NULL)
         ItiStrCpy (pszDest, "-Bad param-", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      return ITIFMT_FORMAT_ERROR;
      }

   /* skip past space characters for null check */
   for (i=0; ItiCharIsSpace (pszSource [i]); i++)
      ;
   if (*pszFormat == '*')
      {
      /* check for null */
      if (pszSource [i] == '\0')
         {
         pszFormat++;

         if (strstr (pszFormat, ",#"))
            *pszDest = '\0';
         else if (!strnicmp (pszFormat, "Number", 6))
            ItiStrCpy (pszDest, "0", usDestMax);
         else
            ItiStrCpy (pszDest, "''", usDestMax);

         if (pusDestLength)
            *pusDestLength = ItiStrLen (pszDest);
         return 0;
         }
      pszFormat++;
      }
//      {
//      /* check for null */
//      if (pszSource [i] == '\0')
//         {
//         pszFormatInfo = ItiStrChr (pszFormat, ',');
//         if (pszFormatInfo == NULL || *(pszFormatInfo + 1) != '#')
//            ItiStrCpy (pszDest, "''", usDestMax);
//         else
//            *pszDest = '\0';
//
//         if (pusDestLength)
//            *pusDestLength = ItiStrLen (pszDest);
//         return 0;
//         }
//      pszFormat++;
//      }
   else if (pszSource [i] == '\0')
      {
      *pszDest = '\0';
      if (pusDestLength)
         *pusDestLength = 0;
      return ITIFMT_NULL;
      }

   pfmtfun = SearchForFunction (pszFormat, &pszFormatInfo);
   if (pfmtfun != NULL)
      {
      usReturn = pfmtfun->pfnInputFunction (pszSource, pszDest, usDestMax,
                                            pszFormatInfo, pusDestLength);
      }
   else
      usReturn = ITIFMT_FORMAT_ERROR;

   if (usReturn == ITIFMT_FORMAT_ERROR)
      {
#if defined (DEBUG)
      ItiStrCpy (pszDest, "*Bad format \"", usDestMax);
      ItiStrCat (pszDest, pszFormat, usDestMax);
      ItiStrCat (pszDest, "\"*", usDestMax);
#else
      ItiStrCpy (pszDest, "*Bad format*", usDestMax);
#endif 
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      }
   else if (usReturn == ITIFMT_BAD_DATA)
      {
#if defined (DEBUG)
      ItiStrCpy (pszDest, "!Bad data \"", usDestMax);
      ItiStrCat (pszDest, pszSource, usDestMax);
      ItiStrCat (pszDest, "\"!", usDestMax);
#else
      ItiStrCpy (pszDest, "!Bad data!", usDestMax);
#endif 
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      }

   return usReturn;
   }




/*
 * ItiFmtRegisterFormat registers a formatting function.
 * 
 * Parameters: pszFormatName     A pointer to a string that contains
 *                               the name of the format method.  The name 
 *                               must not contain any commas.  The name 
 *                               of the format method is case insensitive.
 *
 *             pfnOutput         A pointer to a function that formats
 *                               strings for output to the user.
 *
 *             pfnInput          A pointer to a function that formats
 *                               strings for input to the database.
 *
 *             bOverride         Override the current format method.
 *
 *             ppfnOldFormat     A pointer to a pointer to the old 
 *                               format function.  This parameter may
 *                               be NULL if you don't want a pointer
 *                               to the old format function.
 *
 * Return Value: 0 if succesful,
 *               ITIFMT_NAME_ALREADY_REGISTRERED if a format with the 
 *               same name exists and bOverride is FALSE, 
 *               ITIFMT_INVALID_NAME if pszFormatName is invalid, or
 *               ERROR_NOT_ENOUGH_MEMORY
 *
 * Comments:
 * The function pfnFormat must be defined as follows:
 * USHORT EXPORT FunctionName (PSZ     pszSource,
 *                             PSZ     pszDest,
 *                             USHORT  usDestMax,
 *                             PSZ     pszFormatInfo,
 *                             PUSHORT pusDestLength) 
 *
 * The parameter pszFormatInfo points to the format string, just after the
 * comma that separates the format method name from the format method data.
 * For example, if ItiFmtFormatString was called with pszFormat = "Date,mmm"
 * then pszFormatInfo will point to "mmm".
 *
 * The format function should return 0 if the string was formated, or
 * ITIFMT_TRUNCATED if pszDest was truncated because of space, 
 * ITIFMT_FORMAT_ERROR if pszFormat was in error, or ITIFMT_BAD_DATA if the
 * data passed to the format function is invalid or out of range.
 * The format function must ensure that pszDest is always null ('\0') 
 * terminated.
 * The format function store the number of characters placed in the
 * pszDest string (excluding the null terminator) in the pusDestLength 
 * parameter.  The pusDestLength parameter may be a NULL pointer.
 *
 * If your function returns ITIFMT_BAD_DATA, ItiFmtFormatString fills pszDest
 * with the string "!Bad data!".  If your function return ITIFMT_FORMAT_ERROR,
 * ItiFmtFormatString fills pszDest with the string "*Bad format*".  
 */

USHORT EXPORT ItiFmtRegisterFormat (PSZ        pszFormatName,
                                    PFNFORMAT  pfnOutput,
                                    PFNFORMAT  pfnInput,
                                    BOOL       bOverride,
                                    PFNFORMAT  *ppfnFormat)
   {
   return ERROR_NOT_ENOUGH_MEMORY;
   }





