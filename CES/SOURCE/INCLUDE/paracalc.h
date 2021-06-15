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

#if !defined (PARACALC_INCLUDED)
#define PARACALC_INCLUDED

#define PARACALC_VOLUME   3
#define PARACALC_AREA     2 
#define PARACALC_LENGTH   1


/* variable IDs */
#define VAR_WORKTYPE             0
#define VAR_DISTRICT             1
#define VAR_ROADTYPE             2
#define VAR_STRUCTURETYPE        3
#define NUM_VARIABLES            4


#define VARIABLE_OFFSET          10


/* general query ids */

#define DELETE_JBMI        1
#define GET_PROFILE_KEY    2
#define MAKE_JBMI          3
#define GETQUANTITY        4
#define UPDATEQUANTITY     5
#define GETPRICE           6
#define UPDATEPRICE        7
#define CALC_JBMI_EXT      8
#define UPDATE_JB          9
#define UPDATE_JB1         10
#define UPDATE_JB2         11
#define UPDATE_JMIQTYREGS  12
#define UPDATE_J           13
#define QUERY_JMIQTYREGS   14

#define GETQTY_MIKEY       15

/* the variable queries must be in order! */

/* quantity adjustment queries */
#define GETQADJUST_FIRST         100
#define GETWORKTYPEQADJUST       100
#define GETDISTRICTQADJUST       101
#define GETROADTYPEQADJUST       102
#define GETSTRUCTURETYPEQADJUST  103

/* price adjustment queries */
#define GETPADJUST_FIRST         104
#define GETWORKTYPEPADJUST       104
#define GETDISTRICTPADJUST       105
#define GETROADTYPEPADJUST       106
#define GETSTRUCTURETYPEPADJUST  107



#if !defined (RC_INVOKED)

extern BOOL EXPORT MakeJobBreakdownMajorItem (HHEAP hheap, 
                                       PSZ   pszJobKey, 
                                       PSZ   pszJobBreakdownKey);

extern void EXPORT ResetMIValues (HWND hwnd);

extern PSZ EXPORT ParametMajorItemMedianQty
                     (HHEAP hhp, BOOL bFormatted, PSZ pszMajorItemKey, PSZ pszWhereClause);

extern PSZ EXPORT ParametMajorItemSlopeP
                     (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause);

extern PSZ EXPORT ParametMajorItemInterceptP
                     (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause);

extern USHORT EXPORT ParaCalcDetermineVorAorL (PSZ pszMajorItemKey);

extern PSZ EXPORT ParametMajorItemSlopeQ
   (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL);

extern PSZ EXPORT ParametMajorItemInterceptQ
   (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL);

extern PSZ EXPORT ParametMajorItemNewQtyPrice
         (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszJobKey, PSZ pszJobBrkdwnKey,
          PSZ pszQty, PSZ pszWhereClause, BOOL bResetDB);

extern PSZ EXPORT ResetJBMIQty
         (HHEAP hhpLoc, PSZ pszJobKey, PSZ pszJobBrkKey, PSZ pszJobBrkMIKey, USHORT szVAL, PSZ pszConv, BOOL bChgDB);

extern PSZ EXPORT ParametMajorItemRSQ
         (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL);

extern USHORT EXPORT ParametResetHistPropMajItms (BOOL bReset);

#endif 

#endif /* if defined (PARACALC_INCLUDED) */
