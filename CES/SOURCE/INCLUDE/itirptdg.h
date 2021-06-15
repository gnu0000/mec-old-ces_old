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


#ifndef  INCL_ITIRPTDG
#define  INCL_ITIRPTDG


#define DID_PRINT           DID_OK

#include "..\include\winid.h"


// Manifest constants used in the report .met files for title construction.

#define  ID_RPT_TTL               99
#define  RPT_TTL_NUM              70
#define  RPT_TTL_LEN              80

#define  RPT_TTL_1                 1
#define  RPT_TTL_2                 2
#define  RPT_TTL_3                 3
#define  RPT_TTL_4                 4
#define  RPT_TTL_5                 5
#define  RPT_TTL_6                 6
#define  RPT_TTL_7                 7
#define  RPT_TTL_8                 8
#define  RPT_TTL_9                 9
#define  RPT_TTL_10               10
#define  RPT_TTL_11               11
#define  RPT_TTL_12               12
#define  RPT_TTL_13               13
#define  RPT_TTL_14               14
#define  RPT_TTL_15               15
#define  RPT_TTL_16               16
#define  RPT_TTL_17               17
#define  RPT_TTL_18               18
#define  RPT_TTL_19               19
#define  RPT_TTL_20               20
#define  RPT_TTL_21               21
#define  RPT_TTL_22               22
#define  RPT_TTL_23               23
#define  RPT_TTL_24               24
#define  RPT_TTL_25               25
#define  RPT_TTL_26               26
#define  RPT_TTL_27               27
#define  RPT_TTL_28               28
#define  RPT_TTL_29               29
#define  RPT_TTL_30               30
#define  RPT_TTL_31               31
#define  RPT_TTL_32               32
#define  RPT_TTL_33               33
#define  RPT_TTL_34               34
#define  RPT_TTL_35               35
#define  RPT_TTL_36               36
#define  RPT_TTL_37               37
#define  RPT_TTL_38               38
#define  RPT_TTL_39               39
#define  RPT_TTL_40               40
#define  RPT_TTL_41               41
#define  RPT_TTL_42               42
#define  RPT_TTL_43               43
#define  RPT_TTL_44               44
#define  RPT_TTL_45               45
#define  RPT_TTL_46               46
#define  RPT_TTL_47               47
#define  RPT_TTL_48               48
#define  RPT_TTL_49               49
#define  RPT_TTL_50               50
#define  RPT_TTL_51               51
#define  RPT_TTL_52               52
#define  RPT_TTL_53               53
#define  RPT_TTL_54               54
#define  RPT_TTL_55               55
#define  RPT_TTL_56               56
#define  RPT_TTL_57               57
#define  RPT_TTL_58               58
#define  RPT_TTL_59               59
#define  RPT_TTL_60               60
#define  RPT_TTL_61               61
#define  RPT_TTL_62               62
#define  RPT_TTL_63               63
#define  RPT_TTL_64               64
#define  RPT_TTL_65               65
#define  RPT_TTL_66               66
#define  RPT_TTL_67               67
#define  RPT_TTL_68               68
#define  RPT_TTL_69               69
#define  RPT_TTL_70               70



// The following are used as message parameters to BM_SETCHECK's mp1  
// for checkbox controls. (In ItiRptUt.h)
//
//define  UNCHECKED             MPFROMSHORT((USHORT) 0)
//define  CHECKED               MPFROMSHORT((USHORT) 1)
//define  INDETERM              MPFROMSHORT((USHORT) 2)


                       

#define CTRL_ACCEPT_DATE CONTROL "Inflate to date: ", DID_DATEMSG,           \
                          14, 38, 124, 10,                                   \
                          WC_STATIC, SS_TEXT | DT_LEFT | DT_TOP              \
                          | WS_GROUP | WS_VISIBLE                            \
                                                                             \
                  CONTROL " ", DID_DATEEDIT, 14, 25, 84, 10,                 \
                          WC_ENTRYFIELD, ES_LEFT | ES_AUTOSCROLL             \
                          | ES_AUTOTAB | ES_MARGIN | WS_TABSTOP | WS_VISIBLE \
                                                                             \
                  CONTROL "<Ä Use", DID_ACCEPT_EDIT, 100, 24, 38, 13,       \
                          WC_BUTTON, BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE \
                                                                       



#define CTRL_PCH  CONTROL "~Print", DID_PRINT, 7, 5, 38, 13,        \
                          WC_BUTTON, BS_PUSHBUTTON |                \
                          WS_TABSTOP | WS_VISIBLE  |                \
                          BS_DEFAULT                                \
                                                                    \
                  CONTROL "~Cancel" DID_CANCEL, 54, 5, 38, 13,      \
                          WC_BUTTON, BS_PUSHBUTTON |                \
                          WS_TABSTOP | WS_VISIBLE                   \
                                                                    \
                  CONTROL "~Help",  DID_HELP, 101, 5, 38, 13,       \
                          WC_BUTTON, BS_PUSHBUTTON |                \
                          WS_TABSTOP | WS_VISIBLE                   \
                          | BS_HELP  | BS_NOPOINTERFOCUS            \
                           

#endif
