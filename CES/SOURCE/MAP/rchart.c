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


/*  CES Report DLL Source Code
 *
 *  Timothy Blake (tlb)
 *
 *  NOTICE: This module expects ALL co-ordinates to be in SECONDS.
 *          ie. 4 Degrees 0 Minutes 0 Seconds is to be specified 
 *              as "14400"
 *
 *          also 78 Degrees 0 Minutes 1 Second is to be specified 
 *              as "280801"
 */


#define     INCL_BASE
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI
#define     INCL_WIN

#include    "..\include\iti.h"     
#include    <stdlib.h>
#include    <stdio.h>
#include    <stddef.h>
#include    <float.h>
#include    <limits.h>



#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"
#include    "..\include\itimbase.h"
#include    "..\include\itidbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\colid.h"
#include    "..\include\winid.h"
#include    "..\include\itiwin.h"
#include    "..\include\dialog.h"
#include    "..\include\itimenu.h"
#include    "dialog.h"

#include    "..\include\itirptdg.h"
#include    "..\include\itirpt.h"


/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif





#include "rChartIt.h"                                       
#include "..\include\rChart.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Chart Test"

#define  QUERY_ARRAY_SIZE       1023


#define SHOWCOND( t )   ItiRptUtErrorMsgBox(NULL,  t  ); 


#define SETQERRMSG(QY) ItiStrCpy (szErrorMsg,                                \
                              "ERROR: Query failed for ",                    \
                              sizeof szErrorMsg);                            \
                                                                             \
                   ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);         \
                   ItiStrCat (szErrorMsg,", Error Code: ",sizeof szErrorMsg);\
                                                                             \
                   ItiStrUSHORTToString                                      \
                      (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],            \
                       sizeof szErrorMsg);                                   \
                                                                             \
                   ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);   \
                   ItiStrCat (szErrorMsg, QY, sizeof szErrorMsg);            \
                                                                             \
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg)




typedef struct _LnSeg
         { POINTL pt1;
           POINTL pt2;
         } LNSEG; /* lnseg */


typedef struct _Outline
         { LONG lNumOfPts;
           POINTL ptl[3200]; //fix size later.
         }OUTLINE;

typedef OUTLINE *POUTLINE;



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rchart.dll";

static  HHEAP hheap;
static  CHAR szKey[SMARRAY] = "";

static BOOL bPointIsOnBoarder = FALSE;

static CHAR szMsg      [3 * SMARRAY] = "";
static CHAR szMsg2     [3 * SMARRAY] = "";
static CHAR szErrorMsg [LGARRAY] = "";

static  OUTLINE shape;

static  CHAR szX [SMARRAY] = "";
static  CHAR szY [SMARRAY] = "";

static  BOOL bX = FALSE;
static  BOOL bY = FALSE;

static  LONG lX = 0L;
static  LONG lY = 0L;


static CHAR szQuery   [LGARRAY] = "";
static CHAR szQryMap  [LGARRAY] = "";
static CHAR szCnjMap  [3 * SMARRAY] = "";



static  INT iUse = 0; // remove, used for testing only.
static  POINTL ptlLoc; // test code



           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprChart  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;



double Slope (POINTL pt1, POINTL pt2)
   {
   double dx, dy;
   double lRes = (double)0;


   if (pt1.y == pt2.y) /* horizontal line, or same point, no slope. */
      return (lRes); 

   if (pt1.x == pt2.x)   /* vertical line     */
      {
      lRes = FLT_MAX;
      return (lRes); /* slope is infinite */
      }

   dx = (double)(pt2.x - pt1.x);
   dy = (double)(pt2.y - pt1.y);

   lRes = (dy / dx); /*  dy/dx = slope */

   return (lRes);
   }/* end of function */


double Dy (POINTL ptlV, POINTL ptlF1, POINTL ptlF2)
   {
   double lResS;
   double dy;

   lResS = Slope(ptlF2, ptlV);
   dy = lResS * ((double)(ptlF1.x - ptlF2.x));
   return (dy);
   }/* end of function */



BOOL Intersect (LNSEG lnseg, POINTL ptl)
   {
   extern BOOL bPointIsOnBoarder;
//   ldiv_t lResLnP1toP2;
//   ldiv_t lResLnP1toT;


   /* -- Cond1: segment is entirely to the left of ptl. */
   if ((lnseg.pt1.x < ptl.x) && (lnseg.pt2.x < ptl.x))
      {
      SHOWCOND("1F");
      return (FALSE);
      }

   /* -- Cond2: segment is entirely above ptl. */
   if ((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y > ptl.y))
      {
      SHOWCOND("2F");
      return (FALSE);
      }

   /* -- Cond3: segment is entirely below ptl. */
   if ((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y < ptl.y))
      {
      SHOWCOND("3F");
      return (FALSE);
      }


   /* -- Cond4: segment is entirely to the right of ptl and each
    *    segment's end point is on opposite sides of the impiled ray;
    *    in which case there is an intersection.
    */
   if ((lnseg.pt1.x > ptl.x) && (lnseg.pt2.x > ptl.x))
      {
      if ( ((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y))
         ||((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y)) )
            {
            SHOWCOND("4T");
            return (TRUE);
            }
      }

   /* -- Cond5: the first endpoint is the test point; only check for 1st
    *    point because it was the last point's 2nd point and don't want
    *    to count it twice.
    */
   if ((lnseg.pt1.x == ptl.x) && (lnseg.pt1.y == ptl.y))
      {
      bPointIsOnBoarder = TRUE;
      SHOWCOND("5T");
      return (TRUE);
      }

   /* -- Cond6: segment is on the impiled ray. */
   if ((lnseg.pt1.y == ptl.y) && (lnseg.pt2.y == ptl.y))
      {
       if ((lnseg.pt1.x > ptl.x) && (lnseg.pt2.x > ptl.x))
         {
         SHOWCOND("6F(entirely on)");
         return (FALSE);  /* entirely on the impiled ray. */
         }
       else
          {
          if ((lnseg.pt2.x == ptl.x))
            {
            SHOWCOND("6F(endPt)");
            return (FALSE); /* is an end point that is counted in Cond5 */
            }
          else
             {
             bPointIsOnBoarder = TRUE;
             SHOWCOND("6T(partial on)");
             return (TRUE);  /* partially on the impiled ray. */
             }
          }
      }


   /* -- Cond7   ____
    *            \   \
    *            /   /
    *            
    */
//   if ((lnseg.pt1.y == ptl.y) && (lnseg.pt1.x < ptl.x))
//      return (FALSE);



   /* -- Cond8.A */
   if ( (lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y) )
      {
      if ( (lnseg.pt1.x >= ptl.x) && (lnseg.pt2.x <= ptl.x) )    
         {
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) > Dy(ptl, ptl, lnseg.pt1))
            {
            SHOWCOND("8aT");
            return (TRUE);
            }
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
            {
            if (Dy(ptl, ptl, lnseg.pt1) != (double)0)
               {
               bPointIsOnBoarder = TRUE;
               SHOWCOND("8aT(Boarder)");
               return (TRUE);
               }
            else
               SHOWCOND("8a.F->");
            }
         }
      else   /* -- Cond8.B */
         if ( (lnseg.pt1.x <= ptl.x) && (lnseg.pt2.x >= ptl.x) )  
            {
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) < Dy(ptl, ptl, lnseg.pt1))
               {
               SHOWCOND("8bT");
               return (TRUE);
               }
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
               {
               if (Dy(ptl, ptl, lnseg.pt1) != (double)0)
                  {
                  bPointIsOnBoarder = TRUE;
                  SHOWCOND("8bT(Boarder)");
                  }
               else
                  SHOWCOND("8b.T");
               return (TRUE);
               }
            }
      }

   /* -- Cond8.C */
   if ( (lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y) )
      {
      if ( (lnseg.pt1.x >= ptl.x) && (lnseg.pt2.x <= ptl.x) ) 
         {
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) < Dy(ptl, ptl, lnseg.pt1))
            {
            SHOWCOND("8cT");
            return (TRUE);
            }
         if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
            {
            if (Dy(ptl, ptl, lnseg.pt1) != (double)0)
               {
               bPointIsOnBoarder = TRUE;
               SHOWCOND("8cT(Boarder)");
               return (TRUE);
               }
            else
               SHOWCOND("8c.F->");
            }
         }
       else   /* -- Cond8.D */
         if ( (lnseg.pt1.x <= ptl.x) && (lnseg.pt2.x >= ptl.x) ) 
            {
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) > Dy(ptl, ptl, lnseg.pt1))
               {
               SHOWCOND("8dT");
               return (TRUE);
               }
            if (Dy(lnseg.pt2, ptl, lnseg.pt1) == Dy(ptl, ptl, lnseg.pt1))
               {
               if (Dy(ptl, ptl, lnseg.pt1) != (double)0)
                  {
                  bPointIsOnBoarder = TRUE;
                  SHOWCOND("8dT(Boarder)");
                  }
               else
                  SHOWCOND("8d.T");
               return (TRUE);
               }
            }
      }

    
 
   /* -- Cond9 vertical segment on the X-coordinate. */
   if ((lnseg.pt1.x == ptl.x) && (lnseg.pt2.x == ptl.x))
      {
      if ( ((lnseg.pt1.y > ptl.y) && (lnseg.pt2.y < ptl.y))
         ||((lnseg.pt1.y < ptl.y) && (lnseg.pt2.y > ptl.y)) )
         {
         bPointIsOnBoarder = TRUE;
         SHOWCOND("9T");
         return (TRUE);
         }
      }

   SHOWCOND("(F) ");
   return (FALSE);
   }/* end of function */


BOOL PtInsideOf (POUTLINE pCurve, POINTL pnt)
   {
   extern BOOL bPointIsOnBoarder;
   BOOL bResult;
   LONG i = 0L;
   LONG cnt = 0L;
   LONG lEndPtPair = 0L;
   ldiv_t lResult = {1L,1L};
   LNSEG lnC;

   bPointIsOnBoarder = FALSE;

   for (i=1L; i<=pCurve->lNumOfPts; i++)
      {
      lnC.pt1 = pCurve->ptl[i];
      lnC.pt2 = pCurve->ptl[i+1];

      if ( Intersect(lnC, pnt) )
            cnt++;

      /* -- now test for missed boundry case. */
      if ((pCurve->ptl[i].y == pnt.y) && (pCurve->ptl[i].x > pnt.x))
         {
         if( ((pCurve->ptl[i-1].y > pnt.y) && (pCurve->ptl[i+1].y < pnt.y))
            ||((pCurve->ptl[i-1].y < pnt.y) && (pCurve->ptl[i+1].y > pnt.y)) )
            lEndPtPair++;
         }

      }/* end for */

   sprintf(szMsg, "  < %Ld EndPtPairs >", lEndPtPair); // msg.
   SHOWCOND(szMsg);

   cnt = cnt + lEndPtPair;
   sprintf(szMsg, "  Total # %Ld,", cnt);

   lResult = ldiv(cnt, 2L);
   sprintf(szMsg2, "  # mod 2 = %Ld ", lResult.rem);
   ItiStrCat(szMsg, szMsg2, sizeof szMsg);
   SHOWCOND( szMsg );

   if ( ((lResult.rem) == 1L) || bPointIsOnBoarder )
     bResult = TRUE;
   else
     bResult = FALSE;

   return (bResult);
   }/* end of function */





BOOL InitAry (POUTLINE poly, PSZ pszMap_Key)
   {
   extern INT iUse;
   LONG l = 0L;
   LONG lcnt = 0L;
   LONG lx = 0L;
   LONG ly = 0L;
   LONG N;


   hhprChart = ItiMemCreateHeap (MIN_HEAP_SIZE);


   if (pszMap_Key != NULL)
      {
      ItiMbQueryQueryText(hmodrChart, ITIRID_RPT, ID_QRY_MAP,
                          szQryMap, sizeof szQryMap );

      ItiMbQueryQueryText(hmodrChart, ITIRID_RPT, ID_CNJ_MAP,
                          szCnjMap, sizeof szCnjMap );

      ItiStrCpy(szQuery, szQryMap, sizeof szQuery);
      ItiStrCat(szQuery, pszMap_Key, sizeof szQuery);
      ItiStrCat(szQuery, szCnjMap, sizeof szQuery);


      N = (LONG)ItiDbGetQueryCount(szQuery, &usNumCols, &usErr);

      hqry = ItiDbExecQuery (szQuery, hhprChart, hmodrChart, ITIRID_RPT,
                             ID_QRY_MAP, &usNumCols, &usErr);
      if (hqry == NULL)
         {
         SETQERRMSG(szQuery);
         return (13);
         }
      else
         {
         poly->lNumOfPts = N; /* Set the number of vertices in the map. */

         lcnt = 1L; /* start with the first vertex (point). */

         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
            ItiStrToLONG(ppsz[0], &lx);
            ItiStrToLONG(ppsz[1], &ly);

            //test
            ItiStrCpy(szMsg, "Point (in seconds): ", sizeof szMsg);
            ItiStrCat(szMsg, ppsz[0], sizeof szMsg);
            ItiStrCat(szMsg, ",  ", sizeof szMsg);
            ItiStrCat(szMsg, ppsz[1], sizeof szMsg);
            ItiStrCat(szMsg, "  ", sizeof szMsg);
            ItiRptUtErrorMsgBox (NULL, szMsg);


            poly->ptl[lcnt].x = lx;
            poly->ptl[lcnt].y = ly;

            lcnt++;
            ItiFreeStrArray (hhprChart, ppsz, usNumCols);
            }/* end while */
          }
      }
   else
      {

      switch (iUse)
         {
         case 1:
            N = 6L;
            poly->lNumOfPts = N;
            poly->ptl[1].x = 1L;
            poly->ptl[1].y = 1L;
         
            poly->ptl[2].x = 3L;
            poly->ptl[2].y = 4L;
         
            poly->ptl[3].x = 1L;
            poly->ptl[3].y = 7L;
         
            poly->ptl[4].x = 5L;
            poly->ptl[4].y = 7L;
         
            poly->ptl[5].x = 7L;
            poly->ptl[5].y = 4L;
         
            poly->ptl[6].x = 5L;   /* lNumOfPts == 6 */
            poly->ptl[6].y = 1L;
            break;

         case 2:
            N = 9L;
            poly->lNumOfPts = N;
            poly->ptl[1].x = 1L;
            poly->ptl[1].y = 1L;
         
            poly->ptl[2].x = 3L;
            poly->ptl[2].y = 4L;
         
            poly->ptl[3].x = 1L;
            poly->ptl[3].y = 7L;
         
            poly->ptl[4].x = 2L;
            poly->ptl[4].y = 7L;
         
            poly->ptl[5].x = 4L;
            poly->ptl[5].y = 4L;
         
            poly->ptl[6].x = 5L; 
            poly->ptl[6].y = 4L;
         
            poly->ptl[7].x = 5L;
            poly->ptl[7].y = 7L;
         
            poly->ptl[8].x = 7L;
            poly->ptl[8].y = 4L;
         
            poly->ptl[9].x = 5L;   /* lNumOfPts == 9 */
            poly->ptl[9].y = 1L;
            break;

         case 3:  /* HAMILTON county Ohio */
            N = 112L;
            poly->lNumOfPts = N;

            poly->ptl[1].x = 109250475;
            poly->ptl[1].y =  50766927;

            poly->ptl[2].x = 109248247;
            poly->ptl[2].y =  50758759;

            poly->ptl[3].x = 109254930;
            poly->ptl[3].y =  50745393;

            poly->ptl[4].x = 109270524;
            poly->ptl[4].y =  50631040;

            poly->ptl[5].x = 109280919;
            poly->ptl[5].y =  50573863;

            poly->ptl[6].x = 109287602;
            poly->ptl[6].y =  50579061;

            poly->ptl[7].x = 109302453;
            poly->ptl[7].y =  50587229;

            poly->ptl[8].x = 109308394;
            poly->ptl[8].y =  50589457;

            poly->ptl[9].x = 109317304;
            poly->ptl[9].y =  50593169;

            poly->ptl[10].x = 109333641;
            poly->ptl[10].y =  50597625;

            poly->ptl[11].x = 109352947;
            poly->ptl[11].y =  50600595;

            poly->ptl[12].x = 109361115;
            poly->ptl[12].y =  50602080;

            poly->ptl[13].x = 109369283;
            poly->ptl[13].y =  50600595;

            poly->ptl[14].x = 109377451;
            poly->ptl[14].y =  50602823;

            poly->ptl[15].x = 109385619;
            poly->ptl[15].y =  50602823;

            poly->ptl[16].x = 109393787;
            poly->ptl[16].y =  50603565;

            poly->ptl[17].x = 109408639;
            poly->ptl[17].y =  50609506;

            poly->ptl[18].x = 109422005;
            poly->ptl[18].y =  50620644;

            poly->ptl[19].x = 109424232;
            poly->ptl[19].y =  50628069;

            poly->ptl[20].x = 109424975;
            poly->ptl[20].y =  50628812;

            poly->ptl[21].x = 109427202;
            poly->ptl[21].y =  50670395;

            poly->ptl[22].x = 109430915;
            poly->ptl[22].y =  50679306;

            poly->ptl[23].x = 109435370;
            poly->ptl[23].y =  50686731;

            poly->ptl[24].x = 109441311;
            poly->ptl[24].y =  50692672;

            poly->ptl[25].x = 109456162;
            poly->ptl[25].y =  50700097;

            poly->ptl[26].x = 109463588;
            poly->ptl[26].y =  50701582;

            poly->ptl[27].x = 109479924;
            poly->ptl[27].y =  50700097;

            poly->ptl[28].x = 109488092;
            poly->ptl[28].y =  50695642;

            poly->ptl[29].x = 109505171;
            poly->ptl[29].y =  50676335;

            poly->ptl[30].x = 109512596;
            poly->ptl[30].y =  50670395;

            poly->ptl[31].x = 109531160;
            poly->ptl[31].y =  50664455;

            poly->ptl[32].x = 109540071;
            poly->ptl[32].y =  50662970;

            poly->ptl[33].x = 109548981;
            poly->ptl[33].y =  50665197;

            poly->ptl[34].x = 109557149;
            poly->ptl[34].y =  50669653;

            poly->ptl[35].x = 109564575;
            poly->ptl[35].y =  50671880;

            poly->ptl[36].x = 109572000;
            poly->ptl[36].y =  50673365;

            poly->ptl[37].x = 109580911;
            poly->ptl[37].y =  50671138;

            poly->ptl[38].x = 109587594;
            poly->ptl[38].y =  50665197;

            poly->ptl[39].x = 109592792;
            poly->ptl[39].y =  50658514;

            poly->ptl[40].x = 109606900;
            poly->ptl[40].y =  50649604;

            poly->ptl[41].x = 109630662;
            poly->ptl[41].y =  50644406;

            poly->ptl[42].x = 109647741;
            poly->ptl[42].y =  50639208;

            poly->ptl[43].x = 109655909;
            poly->ptl[43].y =  50638465;

            poly->ptl[44].x = 109663335;
            poly->ptl[44].y =  50638465;

            poly->ptl[45].x = 109673730;
            poly->ptl[45].y =  50640693;

            poly->ptl[46].x = 109676701;
            poly->ptl[46].y =  50640693;

            poly->ptl[47].x = 109684869;
            poly->ptl[47].y =  50644406;

            poly->ptl[48].x = 109688581;
            poly->ptl[48].y =  50647376;

            poly->ptl[49].x = 109702690;
            poly->ptl[49].y =  50659257;

            poly->ptl[50].x = 109709373;
            poly->ptl[50].y =  50663712;

            poly->ptl[51].x = 109717541;
            poly->ptl[51].y =  50668167;

            poly->ptl[52].x = 109724967;
            poly->ptl[52].y =  50670395;

            poly->ptl[53].x = 109740560;
            poly->ptl[53].y =  50670395;

            poly->ptl[54].x = 109748728;
            poly->ptl[54].y =  50672623;

            poly->ptl[55].x = 109755411;
            poly->ptl[55].y =  50675593;

            poly->ptl[56].x = 109790311;
            poly->ptl[56].y =  50714948;

            poly->ptl[57].x = 109797737;
            poly->ptl[57].y =  50723116;

            poly->ptl[58].x = 109805162;
            poly->ptl[58].y =  50727572;

            poly->ptl[59].x = 109812588;
            poly->ptl[59].y =  50730542;

            poly->ptl[60].x = 109835607;
            poly->ptl[60].y =  50734997;

            poly->ptl[61].x = 109843033;
            poly->ptl[61].y =  50732770;

            poly->ptl[62].x = 109849716;
            poly->ptl[62].y =  50729057;

            poly->ptl[63].x = 109861596;
            poly->ptl[63].y =  50718661;

            poly->ptl[64].x = 109877190;
            poly->ptl[64].y =  50699355;

            poly->ptl[65].x = 109883131;
            poly->ptl[65].y =  50694157;

            poly->ptl[66].x = 109891299;
            poly->ptl[66].y =  50690444;

            poly->ptl[67].x = 109915060;
            poly->ptl[67].y =  50684504;

            poly->ptl[68].x = 109922486;
            poly->ptl[68].y =  50680791;

            poly->ptl[69].x = 109926199;
            poly->ptl[69].y =  50828559;

            poly->ptl[70].x = 109778430;
            poly->ptl[70].y =  50939200;

            poly->ptl[71].x = 109698235;
            poly->ptl[71].y =  50938457;

            poly->ptl[72].x = 109696007;
            poly->ptl[72].y =  50938457;

            poly->ptl[73].x = 109691552;
            poly->ptl[73].y =  50945140;

            poly->ptl[74].x = 109684126;
            poly->ptl[74].y =  50946625;

            poly->ptl[75].x = 109681898;
            poly->ptl[75].y =  50947368;

            poly->ptl[76].x = 109673730;
            poly->ptl[76].y =  50947368;

            poly->ptl[77].x = 109592792;
            poly->ptl[77].y =  50942170;

            poly->ptl[78].x = 109548239;
            poly->ptl[78].y =  50938457;

            poly->ptl[79].x = 109514081;
            poly->ptl[79].y =  50936229;

            poly->ptl[80].x = 109493290;
            poly->ptl[80].y =  50934744;

            poly->ptl[81].x = 109438341;
            poly->ptl[81].y =  50931031;

            poly->ptl[82].x = 109361115;
            poly->ptl[82].y =  50925091;

            poly->ptl[83].x = 109297255;
            poly->ptl[83].y =  50922863;

            poly->ptl[84].x = 109272751;
            poly->ptl[84].y =  50922863;

            poly->ptl[85].x = 109207406;
            poly->ptl[85].y =  50910240;

            poly->ptl[86].x = 109203694;
            poly->ptl[86].y =  50907270;

            poly->ptl[87].x = 109201466;
            poly->ptl[87].y =  50905785;

            poly->ptl[88].x = 109199238;
            poly->ptl[88].y =  50898359;

            poly->ptl[89].x = 109202209;
            poly->ptl[89].y =  50887221;

            poly->ptl[90].x = 109209634;
            poly->ptl[90].y =  50883508;

            poly->ptl[91].x = 109216317;
            poly->ptl[91].y =  50877568;

            poly->ptl[92].x = 109224485;
            poly->ptl[92].y =  50873855;

            poly->ptl[93].x = 109231168;
            poly->ptl[93].y =  50870142;

            poly->ptl[94].x = 109239336;
            poly->ptl[94].y =  50867914;

            poly->ptl[95].x = 109247504;
            poly->ptl[95].y =  50859746;

            poly->ptl[96].x = 109248989;
            poly->ptl[96].y =  50852321;

            poly->ptl[97].x = 109254930;
            poly->ptl[97].y =  50847123;

            poly->ptl[98].x = 109262355;
            poly->ptl[98].y =  50846380;

            poly->ptl[99].x = 109268296;
            poly->ptl[99].y =  50841182;

            poly->ptl[100].x = 109273494;
            poly->ptl[100].y =  50829302;

            poly->ptl[101].x = 109268296;
            poly->ptl[101].y =  50817421;

            poly->ptl[102].x = 109260870;
            poly->ptl[102].y =  50814451;

            poly->ptl[103].x = 109253445;
            poly->ptl[103].y =  50814451;

            poly->ptl[104].x = 109247504;
            poly->ptl[104].y =  50815936;

            poly->ptl[105].x = 109245277;
            poly->ptl[105].y =  50815936;

            poly->ptl[106].x = 109237109;
            poly->ptl[106].y =  50819648;

            poly->ptl[107].x = 109228941;
            poly->ptl[107].y =  50816678;

            poly->ptl[108].x = 109224485;
            poly->ptl[108].y =  50810738;

            poly->ptl[109].x = 109225970;
            poly->ptl[109].y =  50797372;

            poly->ptl[110].x = 109237109;
            poly->ptl[110].y =  50785491;

            poly->ptl[111].x = 109239336;
            poly->ptl[111].y =  50778065;

            poly->ptl[112].x = 109246762;
            poly->ptl[112].y =  50774353;
            break;


         default:  /* square */
            N = 4L;
            poly->lNumOfPts = N;
            poly->ptl[1].x = 1L;
            poly->ptl[1].y = 1L;
         
            poly->ptl[2].x = 1L;
            poly->ptl[2].y = 4L;
         
            poly->ptl[3].x = 4L;
            poly->ptl[3].y = 4L;
         
            poly->ptl[4].x = 4L;
            poly->ptl[4].y = 1L;
            break;

         } /* end of switch */

   } /* End of if else clause */



   
   poly->ptl[0].x = poly->ptl[N].x;     /* Nth pt */
   poly->ptl[0].y = poly->ptl[N].y;     /* Nth pt */

   poly->ptl[N + 1].x = poly->ptl[1].x; /* Nth + 1 pt */
   poly->ptl[N + 1].y = poly->ptl[1].y; /* Nth + 1 pt */

   return (TRUE);

   }/* End of function InitAry */







BOOL EXPORT PointInMap (PSZ pszMapKey, PSZ pszLong, PSZ pszLat)
   {
   BOOL bAns, bX, bY;
   POINTL  ptlTestPoint;
   LONG lLongitude = 0L;
   LONG lLatitude  = 0L;


   iUse = 2; //testing map #2


   if (pszMapKey == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, "PointInMap, MapKey parameter is NULL");
      return FALSE;
      }

   if (pszLong == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, "PointInMap, pszLong parameter is NULL");
      return FALSE;
      }

   if (pszLat == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, "PointInMap, pszLat parameter is NULL");
      return FALSE;
      }


   bX = ItiStrToLONG(pszLong, &lLongitude);
   bY = ItiStrToLONG(pszLat, &lLatitude );

   if ((!bX) || (!bY))
      {
      ItiRptUtErrorMsgBox (NULL, "PointInMap, parameter conversion failed.");
      return FALSE;
      }



   ptlTestPoint.x = lLongitude;
   ptlTestPoint.y = lLatitude;

   InitAry(&shape, pszMapKey);

   bAns = PtInsideOf(&shape, ptlTestPoint);

   
   /* -- Write result to the trace file. */
   sprintf(szMsg, "\n The point  %Ld,", ptlTestPoint.x);

   if (bAns)
      {
      sprintf(szMsg2," %Ld is INSIDE of map.\n\n", ptlTestPoint.y);
      }
   else
      {
      sprintf(szMsg2," %Ld is OUTSIDE of map.\n\n", ptlTestPoint.y);
      }

   ItiStrCat(szMsg, szMsg2, sizeof szMsg);
   ItiRptUtErrorMsgBox (NULL, szMsg);


   return bAns;
   }







/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   DBGMSG(" *** In DLL version check function.");

   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);

   return (FALSE);
   }







USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;
   BOOL    bCurrentMapKeyContainsThePoint = FALSE;

           /* General return code variable: */
   USHORT  us = 0;

           /* Page formatting location variables: */
   USHORT  usLn  = 1;
   USHORT  usCol = 0;
   LONG    lColWidthPgUnits = 0L;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;

           /* Title strings for this module. */
   REPTITLES rept;
   PREPT     prept = &rept;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


   ItiRptUtGetOption (OPT_SW_X, szX, sizeof szX,
                      argcnt, argvars);
   if (szX[0] == '\0') 
      bX = FALSE;
   else
      {
      bX = ItiStrToLONG(szX, &lX);
      }


   ItiRptUtGetOption (OPT_SW_Y, szY, sizeof szY,
                      argcnt, argvars);
   if (szY[0] == '\0')
      {
      bY = FALSE;
      }
   else
      {
      bY = ItiStrToLONG(szY, &lY);
      }





   us =  ItiPrtLoadDLLInfo(hab, hmq, prept, argcnt, argvars);
   if (us != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return (ITIPRT_NOT_INITIALIZED);
      }

//   ItiPrtBeginReport (PRT_Q_NAME);



   /* -------------------------------------------------------------------- *\
    * -- For each map key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while ((ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
          && (!bCurrentMapKeyContainsThePoint) )
      {

      bCurrentMapKeyContainsThePoint = PointInMap(szKey, szX, szY);

      }/* End of WHILE GetNextKey */



   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





