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




typedef struct
   {
   USHORT   usRows;
   USHORT   usCols;
   DOUBLE   **pdMat;
   } MATRIX;

typedef MATRIX *PMATRIX;

extern void EXPORT PrintMatrix (PMATRIX pmat);

extern BOOL EXPORT SolveMatrix (PMATRIX pmat, PMATRIX pmatResults);

extern BOOL EXPORT AllocMatrix (USHORT usNumRows, USHORT usNumCols, PMATRIX *pmat);

extern void EXPORT FreeMatrix (PMATRIX pmat);



