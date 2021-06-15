/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*  
 *  itimap
 *  Timothy Blake 
 *  Mark Hampton
 *
 *  NOTICE: This module expects ALL co-ordinates to be in SECONDS.
 *          ie. 4 Degrees 0 Minutes 0 Seconds is to be specified
 *              as "14400"
 *
 *          also 78 Degrees 0 Minutes 1 Second is to be specified
 *              as "280801"
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "..\include\itimap.h"
#include "init.h"
#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <float.h>
#include <limits.h>






typedef struct _LnSeg
   {
   POINTL pt1;
   POINTL pt2;
   } LNSEG; /* lnseg */


#define MAX_VERTICIES      3200

typedef struct _Outline
   {
   LONG     lNumOfPts;
   POINTL   ptl [MAX_VERTICIES];
   } OUTLINE;

typedef OUTLINE *POUTLINE;


/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 itimap.dll";

static POUTLINE pshape = NULL;






static DOUBLE Slope (POINTL pt1, POINTL pt2)
   {
   DOUBLE dx, dy;
   DOUBLE dRes = 0;

   if (pt1.y == pt2.y) /* horizontal line, or same point, no slope. */
      return dRes;

   if (pt1.x == pt2.x)   /* vertical line     */
      {
      dRes = FLT_MAX;
      return dRes; /* slope is infinite */
      }

   dx = (DOUBLE) (pt2.x - pt1.x);
   dy = (DOUBLE) (pt2.y - pt1.y);

   dRes = (dy / dx); /*  dy/dx = slope */

   return dRes;
   }




static DOUBLE Dy (POINTL ptlV, POINTL ptlF1, POINTL ptlF2)
   {
   DOUBLE dResS;
   DOUBLE dy;

   dResS = Slope (ptlF2, ptlV);
   dy = dResS * ((DOUBLE) (ptlF1.x - ptlF2.x));
   return dy;
   }





static BOOL Intersect (LNSEG lnseg, POINTL ptl, PBOOL pbPointOnBorder)
   {
//   ldiv_t lResLnP1toP2;
//   ldiv_t lResLnP1toT;

   *pbPointOnBorder = FALSE;

   /* -- Cond1: segment is entirely to the left of ptl. */
   if ((lnseg.pt1.x < ptl.x) && (lnseg.pt2.x < ptl.x))
      return FALSE;

   /* -- Cond2: segment is entirely above ptl. */
   if ((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y > ptl.y))
      return FALSE;

   /* -- Cond3: segment is entirely below ptl. */
   if ((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y < ptl.y))
      return FALSE;

   /* -- Cond4: segment is entirely to the right of ptl and each
    *    segment's end point is on opposite sides of the impiled ray;
    *    in which case there is an intersection.
    */
   if ((lnseg.pt1.x > ptl.x) && (lnseg.pt2.x > ptl.x))
      {
      if ( ((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y))
         ||((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y)) )
            {
            return TRUE;
            }
      }

   /* -- Cond5: the first endpoint is the test point; only check for 1st
    *    point because it was the last point's 2nd point and don't want
    *    to count it twice.
    */
   if ((lnseg.pt1.x == ptl.x) && (lnseg.pt1.y == ptl.y))
      {
      *pbPointOnBorder = TRUE;
      return TRUE;
      }

   /* -- Cond6: segment is on the impiled ray. */
   if ((lnseg.pt1.y == ptl.y) && (lnseg.pt2.y == ptl.y))
      {
       if ((lnseg.pt1.x > ptl.x) && (lnseg.pt2.x > ptl.x))
         return FALSE;  /* entirely on the impiled ray. */
       else
          {
          if ((lnseg.pt2.x == ptl.x))
            return FALSE; /* is an end point that is counted in Cond5 */
          else
             {
             *pbPointOnBorder = TRUE;
             return TRUE;  /* partially on the impiled ray. */
             }
          }
      }


   /* -- Cond7   ____
    *            \   \
    *            /   /
    *
    */
//   if ((lnseg.pt1.y == ptl.y) && (lnseg.pt1.x < ptl.x))
//      return FALSE;



   /* -- Cond8.A */
   if ( (lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y) )
      {
      if ( (lnseg.pt1.x >= ptl.x) && (lnseg.pt2.x <= ptl.x) )
         {
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) > Dy(ptl, ptl, lnseg.pt1))
            return TRUE;
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
            {
            if (Dy(ptl, ptl, lnseg.pt1) != (DOUBLE)0)
               {
               *pbPointOnBorder = TRUE;
               return TRUE;
               }
            }
         }
      else   /* -- Cond8.B */
         if ( (lnseg.pt1.x <= ptl.x) && (lnseg.pt2.x >= ptl.x) )
            {
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) < Dy(ptl, ptl, lnseg.pt1))
               return TRUE;
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
               {
               if (Dy(ptl, ptl, lnseg.pt1) != (DOUBLE)0)
                  *pbPointOnBorder = TRUE;
               return TRUE;
               }
            }
      }

   /* -- Cond8.C */
   if ( (lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y) )
      {
      if ( (lnseg.pt1.x >= ptl.x) && (lnseg.pt2.x <= ptl.x) )
         {
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) < Dy(ptl, ptl, lnseg.pt1))
            return TRUE;
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
            {
            if (Dy(ptl, ptl, lnseg.pt1) != (DOUBLE)0)
               {
               *pbPointOnBorder = TRUE;
               return TRUE;
               }
            }
         }
       else   /* -- Cond8.D */
         if ( (lnseg.pt1.x <= ptl.x) && (lnseg.pt2.x >= ptl.x) )
            {
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) > Dy(ptl, ptl, lnseg.pt1))
               return TRUE;
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
               {
               if (Dy(ptl, ptl, lnseg.pt1) != (DOUBLE)0)
                  *pbPointOnBorder = TRUE;
               return TRUE;
               }
            }
      }



   /* -- Cond9 vertical segment on the X-coordinate. */
   if ((lnseg.pt1.x == ptl.x) && (lnseg.pt2.x == ptl.x))
      {
      if ( ((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y))
         ||((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y)) )
         {
         *pbPointOnBorder = TRUE;
         return TRUE;
         }
      }

   return FALSE;
   }/* end of function */




static BOOL PtInsideOf (POUTLINE pCurve, POINTL pnt)
   {
   LONG     i = 0L;
   LONG     cnt = 0L;
   LONG     lEndPtPair = 0L;
   ldiv_t   lResult = {1L,1L};
   LNSEG    lnC;
   BOOL     bPointOnBorder = FALSE, bTemp;

   bPointOnBorder = FALSE;

   for (i=1L; i <= pCurve->lNumOfPts; i++)
      {
      lnC.pt1 = pCurve->ptl[i];
      lnC.pt2 = pCurve->ptl[i+1];

      if (Intersect (lnC, pnt, &bTemp))
         cnt++;
      bPointOnBorder |= bTemp;

      /* -- now test for missed boundary case. */
      if ((pCurve->ptl[i].y == pnt.y) && (pCurve->ptl[i].x > pnt.x))
         {
         if(((pCurve->ptl[i-1].y > pnt.y) && 
             (pCurve->ptl[i+1].y < pnt.y)) ||
            ((pCurve->ptl[i-1].y < pnt.y) && 
             (pCurve->ptl[i+1].y > pnt.y)))
            {
            lEndPtPair++;
            }
         }

      }/* end for */

   cnt = cnt + lEndPtPair;
   lResult = ldiv (cnt, 2L);

   return lResult.rem == 1L || bPointOnBorder;
   }





static BOOL InitArray (HHEAP     hheap, 
                       POUTLINE  poly, 
                       PSZ       pszMapKey)
   {
   LONG  l = 0L;
   LONG  lcnt = 0L;
   LONG  lx = 0L;
   LONG  ly = 0L;
   LONG  N;
   HQRY  hqry;
   char  szTemp [512], szQuery [512];
   PSZ   *ppsz, pszTemp;
   USHORT   usNumCols, usErr;

   if (pszMapKey == NULL)
      return FALSE;

   ItiMbQueryQueryText (hmod, ITIRID_CALC, GET_VERTICIES, 
                        szTemp, sizeof szTemp);
   pszTemp = "MapKey";
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        &pszTemp, &pszMapKey, 1);

   N = (LONG) ItiDbGetQueryCount (szQuery, &usNumCols, &usErr);

   if (N + 2 > MAX_VERTICIES)
      return FALSE;

   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_CALC,
                          GET_VERTICIES, &usNumCols, &usErr);
   if (hqry == NULL)
      return FALSE;

   poly->lNumOfPts = N; /* Set the number of vertices in the map. */

   lcnt = 1L; /* start with the first vertex (point). */

   while (ItiDbGetQueryRow (hqry, &ppsz, &usErr))
      {
      ItiStrToLONG (ppsz[0], &lx);
      ItiStrToLONG (ppsz[1], &ly);

      poly->ptl [lcnt].x = lx;
      poly->ptl [lcnt].y = ly;

      lcnt++;
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   poly->ptl [0].x = poly->ptl [N].x;     /* Nth pt */
   poly->ptl [0].y = poly->ptl [N].y;     /* Nth pt */

   poly->ptl [N + 1].x = poly->ptl [1].x; /* Nth + 1 pt */
   poly->ptl [N + 1].y = poly->ptl [1].y; /* Nth + 1 pt */

   return TRUE;
   }/* End of function InitArray */







BOOL EXPORT ItiMapIsPointInMap (HHEAP hheap, 
                                PSZ   pszMapKey, 
                                PSZ   pszPosition)
   {
   BOOL     bX, bY;
   POINTL   ptlTestPoint;
   char     szLong [32], szLat [32];

   if (pszMapKey == NULL || pszPosition == NULL)
      {
      ItiErrWriteDebugMessage ("PointInMap -- parameter is NULL");
      return FALSE;
      }

   if (pshape == NULL)
      {
      pshape = ItiMemAllocSeg (sizeof *pshape, SEG_NONSHARED, MEM_ZERO_INIT);
      if (pshape == NULL)
         return FALSE;
      }

   /* have the format module parse the position for us */
   ItiFmtFormatString (pszPosition, szLong, sizeof szLong,
                       "Position,<long><lSeconds>", NULL);
   ItiFmtFormatString (pszPosition, szLat, sizeof szLat,
                       "Position,<lat><lSeconds>", NULL);

   bX = ItiStrToLONG (szLong, &ptlTestPoint.x);
   bY = ItiStrToLONG (szLat, &ptlTestPoint.y);

   if (!bX || !bY || szLong [0] == '\0' || szLat [0] == '\0')
      {
      ItiErrWriteDebugMessage ("PointInMap, parameter conversion failed.");
      return FALSE;
      }

   InitArray (hheap, pshape, pszMapKey);

   return PtInsideOf (pshape, ptlTestPoint);
   }











/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return DLL_VERSION;
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;

   return FALSE;
   }







