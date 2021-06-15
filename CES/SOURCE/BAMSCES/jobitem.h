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
 * JobItem.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Item windows.
 */




/*
 * Window procedure procedure for Job Item List window.
 */

extern MRESULT EXPORT JobItemsSProc (HWND   hwnd,
                                       USHORT usMessage,
                                       MPARAM mp1,
                                       MPARAM mp2);

/*
 * Window procedure for the Job Item window.
 */

extern MRESULT EXPORT JobItemSProc (HWND   hwnd,
                                   USHORT usMessage,
                                   MPARAM mp1,
                                   MPARAM mp2);

extern MRESULT EXPORT JobItemDProc (HWND    hwnd,
                                       USHORT   uMsg,
                                       MPARAM   mp1,
                                       MPARAM   mp2);


extern MRESULT EXPORT JobCBESTAddProc (HWND     hwnd,
                                     USHORT   usMessage,
                                     MPARAM   mp1,
                                     MPARAM   mp2);

/*
 * This function creates all of the tables for JobCostBasedEstimation 
 * when a new job item is added.  
 * It also calls a function to recalculate the job item, if the user
 * chose CBEst as an estimation method.
 *
 */

int EXPORT  CopyCBEInfoForNewJobItem(HWND hwnd, PSZ pszNewKey);


/*
 * Determine what estimation method is being used for the job item.
 */

int static GetEstimationMethod(HHEAP hheap, 
                               PSZ   pszJobKey, 
                               PSZ   pszKey);

