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
 * load.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This module is part of DS/Shell.
 *
 * This module loads data from an import file into a database table.
 */




extern USHORT EXPORT LoadTable (PCSVFILEINFO   pcfi,
                         PIMPORTINFO    pii);

extern PCOLINFO EXPORT FindKeyColumn (PIMPORTINFO pii);

