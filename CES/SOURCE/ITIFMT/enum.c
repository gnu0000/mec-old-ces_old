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
 * Enum.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"





typedef struct
   {
   USHORT      usEnumID;
   USHORT      usValue;
   PSZ         pszShortStr;
   PSZ         pszLongStr;
   } ENUMDATA;



typedef struct
   {
   PSZ      pszEnum;
   USHORT   usEnumID;
   } ENUMNAME;

typedef ENUMNAME *PENUMNAME;



static ENUMNAME enumnameEnumName [] =
   {
   { "Dependency",      ENUM_DEPENDENCY,     },
   { "Month",           ENUM_MONTH,          },
   { "Precision",       ENUM_PRECISION,      },
   { "Privilege",       ENUM_PRIVILEGE,      },
   { "QuantityLevel",   ENUM_QUANTITYLEVEL,  },
   { "Season",          ENUM_SEASON,         },
   { "UnitOfTime",      ENUM_UNITTIME,       },
   { "UserType",        ENUM_USERTYPE,       },
   { "UnitSystem",      ENUM_UNITSYSTEM,     },
//   { "UnitTypeSystem",  ENUM_UNITTYPESYSTEM, },
   };


#define ENUMNAME_SIZE  (sizeof enumnameEnumName / sizeof (ENUMNAME))

static int iENsize = ENUMNAME_SIZE ;

static ENUMDATA enumdata [] =
   {
   { ENUM_PRECISION, ROUND_NONE  , "None",         "None",               }, 
   { ENUM_PRECISION, ROUND_0_001 , "1/10 Cent",    "Tenths of a Cent",   }, 
   { ENUM_PRECISION, ROUND_0_01  , "Cent",         "Cent",               }, 
   { ENUM_PRECISION, ROUND_0_1   , "Dime",         "Dime",               }, 
   { ENUM_PRECISION, ROUND_1     , "Dollar",       "Dollar",             }, 
   { ENUM_PRECISION, ROUND_10    , "$10",          "Tens of Dollars",    }, 
   { ENUM_PRECISION, ROUND_100   , "$100",         "Hundreds of Dollars",}, 
   { ENUM_PRECISION, ROUND_1000  , "$1,000",       "Thousands of Dollars", }, 
   { ENUM_PRECISION, ROUND_10000 , "$10,000",      "Ten Thousands",      }, 
   { ENUM_PRECISION, ROUND_100000, "$100,000",     "Hundred Thousands",  }, 
   { ENUM_DEPENDENCY,    0, "NA",           "Not Available",      },
   { ENUM_DEPENDENCY,    1, "SS",           "Start-to-Start",     },
   { ENUM_DEPENDENCY,    2, "SF",           "Start-to-Finish",    },
   { ENUM_DEPENDENCY,    3, "FS",           "Finish-to-Start",    },
   { ENUM_DEPENDENCY,    4, "FF",           "Finish-to-Finish",   },
   { ENUM_PRIVILEGE,     0, "N/A",          "Not Available",      },
   { ENUM_PRIVILEGE,    10, "Viewer",       "Viewer",             },
   { ENUM_PRIVILEGE,    20, "User",         "User",               },
   { ENUM_PRIVILEGE,    30, "Owner",        "Owner",              },
   { ENUM_QUANTITYLEVEL, 0, "NA",           "Not Available",      },
   { ENUM_QUANTITYLEVEL, 1, "*",            "All",                },
   { ENUM_QUANTITYLEVEL, 2, "0",            "Too Low",            },
   { ENUM_QUANTITYLEVEL, 3, "1",            "Very Low",           },
   { ENUM_QUANTITYLEVEL, 4, "2",            "Low",                },
   { ENUM_QUANTITYLEVEL, 5, "3",            "High",               },
   { ENUM_QUANTITYLEVEL, 6, "4",            "Very High",          },
   { ENUM_QUANTITYLEVEL, 7, "5",            "Too High",           },
   { ENUM_SEASON,        0, "NA",           "Not Available",      },
   { ENUM_SEASON,        1, "Winter",       "Winter",             },
   { ENUM_SEASON,        2, "Spring",       "Spring",             },
   { ENUM_SEASON,        3, "Summer",       "Summer",             },
   { ENUM_SEASON,        4, "Fall",         "Fall",               },
   { ENUM_UNITTIME,      0, "NA",           "Not Available",      },
   { ENUM_UNITTIME,     10, "Sec",          "Second",             },
   { ENUM_UNITTIME,     20, "Min",          "Minute",             },
   { ENUM_UNITTIME,     30, "Hour",         "Hour",               },
   { ENUM_UNITTIME,     35, "HfDy",         "HalfDay",            },
   { ENUM_UNITTIME,     40, "Day",          "Day",                },
   { ENUM_UNITTIME,     50, "Week",         "Week",               },
   { ENUM_UNITTIME,     60, "Mth",          "Month",              },
   { ENUM_UNITTIME,     70, "Qtr",          "Quarter",            },
   { ENUM_UNITTIME,     80, "Year",         "Year",               },
   { ENUM_MONTH,         0, "NA",           "Not Available",      },
   { ENUM_MONTH,         1, "Jan",          "January",            },
   { ENUM_MONTH,         2, "Feb",          "February",           },
   { ENUM_MONTH,         3, "Mar",          "March",              },
   { ENUM_MONTH,         4, "Apr",          "April",              },
   { ENUM_MONTH,         5, "May",          "May",                },
   { ENUM_MONTH,         6, "Jun",          "June",               },
   { ENUM_MONTH,         7, "Jul",          "July",               },
   { ENUM_MONTH,         8, "Aug",          "August",             },
   { ENUM_MONTH,         9, "Sep",          "September",          },
   { ENUM_MONTH,        10, "Oct",          "October",            },
   { ENUM_MONTH,        11, "Nov",          "November",           },
   { ENUM_MONTH,        12, "Dec",          "December",           },
   { ENUM_USERTYPE,      0, "N\A",          "Not Available",      },
   { ENUM_USERTYPE,     10, "User",         "User",               },
   { ENUM_USERTYPE,     20, "Sys Mgr",      "System Manager",     },
   { ENUM_USERTYPE,     30, "Devlpr",       "Developer",          },
   { ENUM_UNITSYSTEM,    0, "English",      "English System",     },
   { ENUM_UNITSYSTEM,    1, "Metric",       "Metric System",      },
   { ENUM_UNITSYSTEM,    2, "Both\Niether", "Both or Niether",    },
//   { ENUM_UNITTYPESYSTEM,  0, "Eng",      " English",   },
//   { ENUM_UNITTYPESYSTEM,  1, "Met",       " Metric",    },
//   { ENUM_UNITTYPESYSTEM,  2, "Niether\Both", "Niether or Both",  },
   };

#define ENUMDATA_SIZE (sizeof enumdata / sizeof (ENUMDATA))

static int iEsize = ENUMDATA_SIZE ;

/*
 * ItiFmtGetEnumData
 *
 * Parameters: hheap             A heap to allocate from.
 *
 *             pszEnum           A pointer to a string form of the
 *                               enumeration.
 *
 *             pppszEnumShortStr A pointer to an array of PSZs that
 *                               will be filled in by this function.
 *                               This array will contain pointers to
 *                               the short form of the enumeration.
 *
 *             pppszEnumLongStr  A pointer to an array of PSZs that
 *                               will be filled in by this function.
 *                               This array will contain pointers to
 *                               the long form of the enumeration.
 *
 *             ppusEnumValues    A pointer to an array of USHORT that
 *                               will be filled in by this function.
 *                               This array will contain USHORT that
 *                               are the values for each enumeration
 *                               string.
 *
 *             pusNumValues      The number of entries for the previous
 *                               three parameters.
 *
 * Returns TRUE if the enumeration exists, FALSE if not.
 */

BOOL EXPORT ItiFmtGetEnumData (HHEAP   hheap,
                               PSZ     pszEnum,
                               PSZ     **pppszEnumShortStr,
                               PSZ     **pppszEnumLongStr,
                               PUSHORT *ppusEnumValues,
                               PUSHORT pusNumValues)
   {
   USHORT usEnumID;
   USHORT usCount, i, j;

   usEnumID = ItiFmtQueryEnumID (pszEnum);
   if (usEnumID == 0)
      return FALSE;

   usCount = ItiFmtQueryEnumCount (usEnumID);

   if (pppszEnumShortStr != NULL)
      *pppszEnumShortStr = ItiMemAlloc (hheap, usCount * sizeof (PSZ), 
                                       MEM_ZERO_INIT);

   if (pppszEnumLongStr != NULL)
      *pppszEnumLongStr = ItiMemAlloc (hheap, usCount * sizeof (PSZ), 
                                      MEM_ZERO_INIT);

   if (ppusEnumValues != NULL)
      *ppusEnumValues = ItiMemAlloc (hheap, usCount * sizeof (PUSHORT), 
                                     MEM_ZERO_INIT);

   for (j=0, i=0; i < ENUMDATA_SIZE && j < usCount; i++)
      {
      if (enumdata [i].usEnumID == usEnumID)
         {
         if (pppszEnumShortStr && *pppszEnumShortStr)
            (*pppszEnumShortStr) [j] = ItiStrDup (hheap, enumdata [i].pszShortStr);
         if (pppszEnumLongStr && *pppszEnumLongStr)
            (*pppszEnumLongStr) [j] = ItiStrDup (hheap, enumdata [i].pszLongStr);
         if (ppusEnumValues && *ppusEnumValues)
            (*ppusEnumValues) [j] = enumdata [i].usValue;
         j++;
         }
      }

   *pusNumValues = j;

   return TRUE;
   }



/*
 * ItiFmtFreeEnumData
 *
 * Parameters: hheap             A heap to free from.
 *
 *             ppszEnumShortStr  A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             ppszEnumLongStr   A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             pusEnumValues     A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             usNumValues       The number of entries for the previous
 *                               three parameters.
 */
VOID EXPORT ItiFmtFreeEnumData (HHEAP   hheap,
                                PSZ     *ppszEnumShortStr,
                                PSZ     *ppszEnumLongStr,
                                PUSHORT pusEnumValues,
                                USHORT  usNumValues)
   {
   if (ppszEnumShortStr)
      ItiFreeStrArray (hheap, ppszEnumShortStr, usNumValues);

   if (ppszEnumLongStr)
      ItiFreeStrArray (hheap, ppszEnumLongStr, usNumValues);

   if (pusEnumValues)
      ItiMemFree (hheap, pusEnumValues);
   }



/*
 * ItiFmtCheckEnumValue checks to see if a pszValue is a valid string 
 * representation of the enumeration usEnumID.  
 *
 * Parameters: usEnumID       The enumeration to check.
 *
 *             pszValue       The string value to check.
 *
 *             pusValue       A pointer to a USHORT that will contain
 *                            the enumeration value.  This parameter
 *                            may be NULL.
 *
 * Return Value: TRUE if the string exists in the enumeration, FALSE
 * otherwise.
 */

BOOL EXPORT ItiFmtCheckEnumValue (USHORT  usEnumID, 
                                  PSZ     pszValue, 
                                  PUSHORT pusValue)
   {
   USHORT i;
   CHAR   szEnumVal[32] = "";

   for (i=0; i < ENUMDATA_SIZE; i++)
      {
      ItiStrUSHORTToString
         (enumdata[i].usValue, &szEnumVal[0], sizeof szEnumVal);
      // The above was added because some of the export routines
      // write out only the enum value.

      if (enumdata [i].usEnumID == usEnumID &&
          (ItiStrICmp (enumdata [i].pszShortStr, pszValue) == 0 ||
           ItiStrICmp (enumdata [i].pszLongStr, pszValue)  == 0
         || ItiStrICmp (szEnumVal, pszValue) == 0
          ))
         {
         if (pusValue)
            *pusValue = enumdata [i].usValue;
         return TRUE;
         }
      }
   return FALSE;
   }





/*
 * ItiFmtQueryEnumString returns the enumeration string for a 
 * given enumeration value.
 *
 * Parameters: usEnumID       The enumeration.
 *
 *             usValue        The value.
 *
 *             bLong          TRUE if you want the long version of the
 *                            string, FALSE if you want the short version.
 *
 *             pszDest        The place to store the string.
 *
 *             pusDestMax     The maximum number of bytes to store in
 *                            pszDest.
 *
 * Return Value: TRUE if the value exists in the enumeration, FALSE
 * otherwise.
 */

BOOL EXPORT ItiFmtQueryEnumString (USHORT  usEnumID, 
                                   USHORT  usValue,
                                   BOOL    bLong,
                                   PSZ     pszDest, 
                                   USHORT  usDestMax)
   {
   USHORT i;

   for (i=0; i < ENUMDATA_SIZE; i++)
      {
      if (enumdata [i].usEnumID == usEnumID && 
          enumdata [i].usValue == usValue)
         {
         ItiStrCpy (pszDest, 
                    bLong ? enumdata [i].pszLongStr : enumdata [i].pszShortStr,
                    usDestMax);
         return TRUE;
         }
      }
   return FALSE;
   }





USHORT EXPORT ItiFmtQueryEnumCount (USHORT usEnumID)
   {
   USHORT i;
   USHORT usCount;

   for (usCount = 0, i = 0; i < ENUMDATA_SIZE; i++)
      if (enumdata [i].usEnumID == usEnumID)
         usCount++;
   return usCount;
   }


static SHORT EnumCmpFunc (PENUMNAME penumname1, PENUMNAME penumname2)
   {
   return ItiStrICmp (penumname1->pszEnum, penumname2->pszEnum);
   }

/*
 * ItiFmtQueryEnumID returns the enumeration ID for the string
 * name of the enumeration.
 *
 */

USHORT EXPORT ItiFmtQueryEnumID (PSZ pszEnum)
   {
   ENUMNAME enumname;
   PENUMNAME penumname;

   enumname.pszEnum = pszEnum;
   penumname = ItiMemBinSearch (&enumname, enumnameEnumName, 
                                ENUMNAME_SIZE, sizeof (ENUMNAME), EnumCmpFunc);

   if (penumname)
      return penumname->usEnumID;
   else
      return 0;
   }

