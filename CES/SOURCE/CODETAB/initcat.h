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
 * InitCat.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * Use the ITIDLLI.OBJ module when your DLL does not use the C run time
 * library.  Use the ITIDLLC.OBJ module when you need to use the C run
 * time library, and you need to initialize your DLL.  Note that when
 * you do this, you will have to link with CRTDLL_I.OBJ, a standard 
 * Microsoft module.
 *
 * Note: Do not rename this file to DLLINIT, since this file name
 * is already used by Microsoft for C run time library initialization.
 */


extern HMODULE hmodMe;

