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
 * gauss.c
 * Timothy Blake
 * Mark Hampton
 */


#include "..\include\iti.h"
#include "..\include\itiBase.h"
#include "gauss.h"
#include <math.h>
#include <stdio.h>

void EXPORT PrintMatrix (PMATRIX pmat);

BOOL EXPORT ForwardElimination (PMATRIX pmat)
   {
   USHORT usRow, usCol, j, x;
   DOUBLE dTmp;

   /* -- For each row...
    */
   for(usRow=0; usRow < pmat->usRows; usRow++)
      {
      x = usRow;

      /* -- ...find the element with the largest magnitude
       * -- in the column under the current usRow.
       */
      for(j = usRow+1; j < pmat->usRows; j++)
         {
         if (fabs (pmat->pdMat [j][usRow]) > fabs (pmat->pdMat [x][usRow]))
            x = j;
         }

      /* -- Swap the current usRow with the row x that
       * -- has the larger element.
       */
      if (x != usRow)
         {
         for (usCol=0; usCol < pmat->usCols; usCol++)
            {
            dTmp = pmat->pdMat [x][usCol];
            pmat->pdMat [x][usCol] = pmat->pdMat [usRow][usCol];
            pmat->pdMat [usRow][usCol] = dTmp;
            }/* end for usCol */
         }

      /* -- Now eliminate the current usRow's variable
       * -- from all of the rows below usRow in the matrix.
       */
      for(j=usRow+1; j < pmat->usRows; j++)
         {
         dTmp = pmat->pdMat [j][usRow];
         for (usCol=usRow; usCol < pmat->usCols; usCol++)
            {
            if (pmat->pdMat [usRow][usRow] != 0.0)
               {
               pmat->pdMat [j][usCol] = pmat->pdMat [j][usCol] -
                                pmat->pdMat [usRow][usCol] * dTmp / pmat->pdMat [usRow][usRow];
               }
            else
               {
               /* division by zero error */
               return FALSE;
               }
            }
         pmat->pdMat [j][usRow] = 0.0;
         }

      }/* end for usRow */

   return TRUE;
   }



/*
 */

BOOL EXPORT BackSubstitution (PMATRIX pmat,
                              PMATRIX pmatResults)
   {
   int      r;
   USHORT   c;

   for (r = pmat->usRows - 1; r >= 0; r--)
      {
      if (pmat->pdMat [r][r] == 0)
         return FALSE;

      /* substitute previous values in */
      pmatResults->pdMat [r][0] = pmat->pdMat [r][pmat->usCols - 1];
      for (c=r+1; c < pmat->usCols - 1; c++)
         pmatResults->pdMat [r][0] -= pmat->pdMat [r][c] *
                                      pmatResults->pdMat [c][0];

      pmatResults->pdMat [r][0] /= pmat->pdMat [r][r];
      }
   return TRUE;
   }




BOOL EXPORT SolveMatrix (PMATRIX pmat,
                         PMATRIX pmatResults)
   {
   BOOL bAns = FALSE;

printf ("SolveMatrix, pmat: \n");
PrintMatrix (pmat);

   if (ForwardElimination (pmat))
      bAns = BackSubstitution (pmat, pmatResults);

printf ("pmat after ForwardElimination and BackSubstitution: \n");
PrintMatrix (pmat);

printf ("SolveMatrix, pmatResults: \n");
PrintMatrix (pmatResults);
printf ("\n \n");

   return bAns;
   }


BOOL EXPORT AllocMatrix (USHORT usNumRows, USHORT usNumCols, PMATRIX *pmat)
   {
//   static PMATRIX  pmat;
   ULONG    ulSize, ulDataSize;
   USHORT   i;
   PDOUBLE  pd;

   ulSize = (ULONG) sizeof (MATRIX) + 
            (ULONG) (usNumRows+1) * (ULONG) sizeof (PDOUBLE);
   ulDataSize = (ULONG) usNumRows * (ULONG) (usNumCols+1) * (ULONG) sizeof (DOUBLE);

   if (ulSize > MAX_SEG_SIZE || ulDataSize > MAX_SEG_SIZE)
      return FALSE;

   (*pmat) = ItiMemAllocSeg ((USHORT) ulSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if ((*pmat) == NULL)
      return FALSE;

   pd = ItiMemAllocSeg ((USHORT) ulDataSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pd == NULL)
      {
      ItiMemFreeSeg (pmat);
      return FALSE;
      }

   (*pmat)->usCols = usNumCols;
   (*pmat)->usRows = usNumRows;
   (*pmat)->pdMat = (PDOUBLE *) ((*pmat) + 1);
   for (i=0; i < usNumRows; i++)
      {
      (*pmat)->pdMat [i] = pd + i * usNumCols;
      }

printf ("AllocMatrix: \n");
PrintMatrix ((*pmat));

   return TRUE;
   }




void EXPORT FreeMatrix (PMATRIX pmat)
   {
   if (pmat)
      {
      ItiMemFreeSeg (pmat->pdMat);
      ItiMemFreeSeg (pmat);
      }
   }

// use this stuff for testing
//
//
void EXPORT PrintMatrix (PMATRIX pmat)
   {
   USHORT i, j;

   for (i = 0; i < pmat->usRows; i++)
      {
      for (j = 0; j < pmat->usCols; j++)
         printf ("% 11.2f ", pmat->pdMat [i][j]);
      printf ("\n");
      }
   printf ("\n");
   }
//
//
//
//
//
//main ()
//   {
//   BOOL b;
//   USHORT i;
//   PMATRIX pmat, pmatResults;
//
//   pmat = AllocMatrix (5, 6);
//   pmatResults = AllocMatrix (5, 1);
//
//   pmat->pdMat [0][0] =   4;
//   pmat->pdMat [0][1] =  -4;
//   pmat->pdMat [0][2] =   5;
//   pmat->pdMat [0][3] =   2;
//   pmat->pdMat [0][4] =  -8;
//   pmat->pdMat [0][5] =  37;
//
//   pmat->pdMat [1][0] =   0;
//   pmat->pdMat [1][1] =   1;
//   pmat->pdMat [1][2] =   1;
//   pmat->pdMat [1][3] =   0;
//   pmat->pdMat [1][4] =  -9;
//   pmat->pdMat [1][5] =  14;
//
//   pmat->pdMat [2][0] =   0;
//   pmat->pdMat [2][1] =   0;
//   pmat->pdMat [2][2] =   1;
//   pmat->pdMat [2][3] =   1;
//   pmat->pdMat [2][4] = -10;
//   pmat->pdMat [2][5] =  29;
//
//   pmat->pdMat [3][0] =   1;
//   pmat->pdMat [3][1] =   0;
//   pmat->pdMat [3][2] =   2;
//   pmat->pdMat [3][3] =   1;
//   pmat->pdMat [3][4] =  -7;
//   pmat->pdMat [3][5] =  31;
//
//   pmat->pdMat [4][0] =  -1;
//   pmat->pdMat [4][1] =  -1;
//   pmat->pdMat [4][2] =   1;
//   pmat->pdMat [4][3] =   0;
//   pmat->pdMat [4][4] = -11;
//   pmat->pdMat [4][5] =  23;
//
//   printf ("Start:\n");
//   PrintMatrix (pmat);
//   b = ForwardElimination (pmat);
//   printf ("before back:\n");
//   PrintMatrix (pmat);
//   if (b)
//      printf ("ok\n");
//   else
//      printf ("not ok\n");
//
//   b = BackSubstitution (pmat, pmatResults);
//   printf ("End:\n");
//   PrintMatrix (pmat);
//   PrintMatrix (pmatResults);
//
//   if (b)
//      printf ("ok\n");
//   else
//      printf ("not ok\n");
//
//   FreeMatrix (pmat);
//   FreeMatrix (pmatResults);
//   return 0;
//   }
//
// Answer should be:
//-16.473684 
//  3.105263 
// 16.578947 
// 18.736842 
//  0.631579 
//





