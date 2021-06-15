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
 * Date.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * 
 * This file provides the DateTime and Month format method.
 */



/* -- The following two manifest constants are used
 * -- to compute which day of the week a particular year
 * -- starts on.
 * --
 * -- For example, the first of January 1991 is a Tuesday, the THIRD
 * -- day of the week, so the offset for 1991 is TWO; ie the first
 * -- two days of the week (Sunday and Monday) are NOT in the year.
 */
#define  BASE_YEAR          1970
#define  BASE_YEAR_OFFSET      4


/* MAX_DATE_STRING is the maxium size of a date/time string */
#define MAX_DATE_STRING    128


extern USHORT EXPORT FmtMonth (PSZ     pszSource,
                               PSZ     pszDest,
                               USHORT  usDestMax,
                               PSZ     pszFormatInfo,
                               PUSHORT pusDestLength);


extern USHORT EXPORT FmtDateTime (PSZ     pszSource,
                                  PSZ     pszDest,
                                  USHORT  usDestMax,
                                  PSZ     pszFormatInfo,
                                  PUSHORT pusDestLength);


extern USHORT EXPORT CheckMonth (PSZ     pszSource,
                                 PSZ     pszDest,
                                 USHORT  usDestMax,
                                 PSZ     pszFormatInfo,
                                 PUSHORT pusDestLength);


extern USHORT EXPORT CheckDateTime (PSZ     pszSource,
                                    PSZ     pszDest,
                                    USHORT  usDestMax,
                                    PSZ     pszFormatInfo,
                                    PUSHORT pusDestLength);




       
