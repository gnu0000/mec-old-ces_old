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





/* returns TRUE if it worked */
extern BOOL Joe (HHEAP hheap, PSZ pszJobKey);



/*
 * call this function whenever a new project category is created
 * returns TRUE if it worked 
 */

extern BOOL AddProjectItems (HHEAP hheap,
                             PSZ   pszJobKey,
                             PSZ   pszPCN,
                             PSZ   pszCategory);

