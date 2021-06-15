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



LONG ConvertColToColCoord (USHORT usCol)
   /*
    * Note that minimal range checking is done.
    */
   {
   LONG   lVal;
   USHORT us;
   USHORT usColumnsPerPage;

   DBGMSG( ItiPrtIndentedStr("In ConvertColToColCoord") );


   usColumnsPerPage = pgfmt.sizlPgDim.cx / fm.lAveCharWidth; 


   if (usCol > CURRENT_COL)
      {
      /* -- Get any column spaces requested that are to be added *
       * -- to the current column.                               */
      us = usCol - CURRENT_COL;

      /* -- Get the value of the current column. */
      usCol = ItiPrtQueryCurrentColNum();

      /* -- Add the additonal column spaces. */
      usCol = usCol + us;
      }

   else if (usCol == CURRENT_COL)
      {
      /* -- Get the value of the current column. */
      usCol = ItiPrtQueryCurrentColNum();
      }

   else if (usCol > (CURRENT_COL - MAX_PAPER_COL))
      {
      /* -- Get the value of the current column. */
      us = ItiPrtQueryCurrentColNum();

      /* -- Remove any column spaces requested that are to be *
       * -- subtracted from the current column.               *
       * -- and set the column number.                        */
      usCol = us - (CURRENT_COL - usCol);
      }

   else if (usCol >= FROM_RT_EDGE)                  /*   |        <--|    */
      {
      usCol -= FROM_RT_EDGE;
      usCol = usColumnsPerPage - usCol;
      }

   else if (usCol >= REL_LEFT)                      /*   |    .-->   |    */
      {
      /* -- Get the value of the current column. */
      us = ItiPrtQueryCurrentColNum();

      /* -- Remove the Position Value Mask *
       * -- and set the column number.     */
      usCol -= REL_LEFT;
      usCol += us;
      }

   else if (usCol == USE_RT_EDGE)                   /*   |         ->|<-  */
      {
      usCol = usColumnsPerPage;
      }

   else
      {
      /* -- Do nothing, the given usCol value is an absolute. */
      }


   lVal = (fm.lAveCharWidth * ((LONG)usCol + pgfmt.lLeftMargin)); // +LM
                                          /* ^ADDED 7 JUL 92 */
   if (lVal > pgfmt.sizlPgDim.cx)
      lVal = pgfmt.sizlPgDim.cx; 


   DBGMSG( ItiPrtIndentedStr("Exit ConvertColToColCoord") );
   return (lVal);
   }/* End of Function ConvertColToColCoord */





//   if ((usCol >= FROM_RT_EDGE)                     /*   |        <--|    */
//       && (usCol < (CURRENT_COL - MAX_PAPER_COL)))
//      {
//      usCol = usCol - FROM_RT_EDGE;
//      ptl.x = ConvertColToColCoord (usCol);
//      ptl.x = pgfmt.sizlPgDim.cx - ptl.x;
//      }
//   else
//      {
//      ptl.x = ConvertColToColCoord (usCol);
//      }
//
//
//
//   lLtM = ptl.x;
//
//   ptlTmp = ptl;
//   ptlTmp.x = ptl.x + fm.lMaxCharInc;
//   if ( !PtInDisplaySpace(ptlTmp) )
//      {
//      DBGMSG("ERROR: Invalid text location point attempted.");
//      return (ITIRPT_INVALID_PARM);
//      }
//
//   /* -- Check the right margin. */
//   if (usRtMarginCol >= FROM_RT_EDGE)               /*   |        <--|    */
//      {
//      usRtMarginCol = usRtMarginCol - FROM_RT_EDGE;
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      lRtM = pgfmt.sizlPgDim.cx - lRtM;
//      }
//   else if (usRtMarginCol >= REL_LEFT)              /*   |    .-->   |    */
//      {
//      usRtMarginCol = usRtMarginCol - REL_LEFT;
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      lRtM -= (pgfmt.lLeftMargin * fm.lAveCharWidth); //7 JUL 92
//      lRtM = lRtM + ptl.x;
//      }
//   else if (usRtMarginCol == USE_RT_EDGE)           /*   |         ->|<-  */
//      {
//      lRtM = pgfmt.sizlPgDim.cx;
//      }
//   else
//      {
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      }
//
//   /* -- Check for a valid right margin value. */
//   if (lRtM > pgfmt.sizlPgDim.cx)
//      lRtM = pgfmt.sizlPgDim.cx;



