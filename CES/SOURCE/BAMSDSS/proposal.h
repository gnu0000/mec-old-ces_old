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
 * Proposal.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Proposal window.
 */



/*
 * Window procedure for Proposal window.
 */

extern MRESULT EXPORT ProposalSProc (HWND   hwnd,
                                    USHORT usMessage,
                                    MPARAM mp1,
                                    MPARAM mp2);


extern MRESULT EXPORT ProposalMajItemSProc (HWND   hwnd,
                                           USHORT usMessage,
                                           MPARAM mp1,
                                           MPARAM mp2);




extern MRESULT EXPORT ProposalItemSProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2);




extern void DoProposalDialogBox (void);


extern MRESULT EXPORT ProposalDProc (HWND   hwnd, 
                                     USHORT usMessage, 
                                     MPARAM mp1, 
                                     MPARAM mp2);
