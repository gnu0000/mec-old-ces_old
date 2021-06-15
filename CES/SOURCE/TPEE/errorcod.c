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


/* TLB 21Aug90
 *
 * Error text source module was: BSEERR.H
 *
 * This file includes most, not all, of the error codes from BSEERR.H
 *
 * Given an error code returned from a Dosxxxxx function, this function
 * returns the text of its constant indentifier declaration.
 *
 * Example usage:
 *   printf( "%s Error Reported: %s \n\n",procName, ErrorCodeText(integer) );
 */

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "errorcod.h"

char *ErrorCodeText (USHORT usErrorCode)
   {
   switch (usErrorCode)
      {
      case 0   : return  "NO ERROR";
               break;
      case 1   : return  "INVALID FUNCTION";
               break;
      case 2   : return  "FILE NOT FOUND"; 
               break;
      case 3   : return  "PATH NOT FOUND";
               break;
      case 4   : return  "TOO MANY OPEN FILES";
               break;
      case 5   : return  "ACCESS DENIED";
               break;
      case 6   : return  "INVALID HANDLE";
               break;
      case 7   : return  "ARENA TRASHED";
               break;
      case 8   : return  "NOT ENOUGH MEMORY";
               break;
      case 9   : return  "INVALID BLOCK";
               break;
      case 10  : return  "BAD ENVIRONMENT";
               break;
      case 11  : return  "BAD FORMAT";
               break;
      case 12  : return  "INVALID ACCESS";
               break;
      case 13  : return  "INVALID DATA";
               break;
      case 15  : return  "INVALID DRIVE";
               break;
      case 16  : return  "CURRENT DIRECTORY";
               break;
      case 17  : return  "NOT SAME DEVICE";
               break;
      case 18  : return  "NO MORE FILES";
               break;
      case 19  : return  "WRITE PROTECT";
               break;
      case 20  : return  "BAD UNIT";
               break;
      case 21  : return  "NOT READY";
               break;
      case 22  : return  "BAD COMMAND";
               break;
      case 23  : return  "CRC";
               break;
      case 24  : return  "BAD LENGTH";
               break;
      case 25  : return  "SEEK";
               break;
      case 26  : return  "NOT DOS DISK";
               break;
      case 27  : return  "SECTOR NOT FOUND";
               break;
      case 28  : return  "OUT OF PAPER";
               break;
      case 29  : return  "WRITE FAULT";
               break;
      case 30  : return  "READ FAULT";
               break;
      case 31  : return  "GEN FAILURE";
               break;
      case 32  : return  "SHARING VIOLATION";
               break;
      case 33  : return  "LOCK VIOLATION";
               break;
      case 34  : return  "WRONG DISK";
               break;
      case 35  : return  "FCB UNAVAILABLE";
               break;
      case 36  : return  "SHARING BUFFER EXCEEDED";
               break;
      case 50  : return  "NOT SUPPORTED";
               break;
      case 80  : return  "FILE EXISTS";
               break;
      case 81  : return  "DUP FCB";
               break;
      case 82  : return  "CANNOT MAKE";
               break;
      case 83  : return  "FAIL I24";
               break;
      case 84  : return  "OUT OF STRUCTURES";
               break;
      case 85  : return  "ALREADY ASSIGNED";
               break;
      case 86  : return  "INVALID PASSWORD";
               break;
      case 87  : return  "INVALID PARAMETER";
               break;
      case 88  : return  "NET WRITE FAULT";
               break;
      case 89  : return  "NO PROC SLOTS";
               break;
      case 90  : return  "NOT FROZEN";
               break;
      case 91  : return  "ERR TSTOVFL";
               break;
      case 92  : return  "ERR TSTDUP";
               break;
      case 93  : return  "NO ITEMS";
               break;
      case 95  : return  "INTERRUPT";
               break;
      case 100 : return  "TOO MANY SEMAPHORES";
               break;
      case 101 : return  "EXCL SEM ALREADY OWNED";
               break;
      case 102 : return  "SEM IS SET";
               break;
      case 103 : return  "TOO MANY SEM REQUESTS";
               break;
      case 104 : return  "INVALID AT INTERRUPT TIME";
               break;
      case 105 : return  "SEM OWNER DIED";
               break;
      case 106 : return  "SEM USER LIMIT";
               break;
      case 107 : return  "DISK CHANGE";
               break;
      case 108 : return  "DRIVE LOCKED";
               break;
      case 109 : return  "BROKEN PIPE";
               break;
      case 110 : return  "OPEN FAILED";
               break;
      case 111 : return  "BUFFER OVERFLOW";
               break;
      case 112 : return  "DISK FULL";
               break;
      case 113 : return  "NO MORE SEARCH HANDLES";
               break;
      case 114 : return  "INVALID TARGET HANDLE";
               break;
      case 115 : return  "PROTECTION VIOLATION";
               break;
      case 116 : return  "VIOKBD REQUEST";
               break;
      case 117 : return  "INVALID CATEGORY";
               break;
      case 118 : return  "INVALID VERIFY SWITCH";
               break;
      case 119 : return  "BAD DRIVER LEVEL";
               break;
      case 120 : return  "CALL NOT IMPLEMENTED";
               break;
      case 121 : return  "SEM TIMEOUT";
               break;
      case 122 : return  "INSUFFICIENT BUFFER";
               break;
      case 123 : return  "INVALID NAME";
               break;
      case 124 : return  "INVALID LEVEL";
               break;
      case 125 : return  "NO VOLUME LABEL";
               break;
      case 126 : return  "MOD NOT FOUND";
               break;
      case 127 : return  "PROC NOT FOUND";
               break;
      case 128 : return  "WAIT NO CHILDREN";
               break;
      case 129 : return  "CHILD NOT COMPLETE";
               break;
      case 130 : return  "DIRECT ACCESS HANDLE";
               break;
      case 131 : return  "NEGATIVE SEEK";
               break;
      case 132 : return  "SEEK ON DEVICE";
               break;
      case 133 : return  "IS JOIN TARGET";
               break;
      case 134 : return  "IS JOINED";
               break;
      case 135 : return  "IS SUBSTED";
               break;
      case 136 : return  "NOT JOINED";
               break;
      case 137 : return  "NOT SUBSTED";
               break;
      case 138 : return  "JOIN TO JOIN";
               break;
      case 139 : return  "SUBST TO SUBST";
               break;
      case 140 : return  "JOIN TO SUBST";
               break;
      case 141 : return  "SUBST TO JOIN";
               break;
      case 142 : return  "BUSY DRIVE";
               break;
      case 143 : return  "SAME DRIVE";
               break;
      case 144 : return  "DIR NOT ROOT";
               break;
      case 145 : return  "DIR NOT EMPTY";
               break;
      case 146 : return  "IS SUBST PATH";
               break;
      case 147 : return  "IS JOIN PATH";
               break;
      case 148 : return  "PATH BUSY";
               break;
      case 149 : return  "IS SUBST TARGET";
               break;
      case 150 : return  "SYSTEM TRACE";
               break;
      case 151 : return  "INVALID EVENT COUNT";
               break;
      case 152 : return  "TOO MANY MUXWAITERS";
               break;
      case 153 : return  "INVALID LIST FORMAT";
               break;
      case 154 : return  "LABEL TOO LONG";
               break;
      case 155 : return  "TOO MANY TCBS";
               break;
      case 156 : return  "SIGNAL REFUSED";
               break;
      case 157 : return  "DISCARDED";
               break;
      case 158 : return  "NOT LOCKED";
               break;
      case 159 : return  "BAD THREADID ADDR";
               break;
      case 160 : return  "BAD ARGUMENTS";
               break;
      case 161 : return  "BAD PATHNAME";
               break;
      case 162 : return  "SIGNAL PENDING";
               break;
      case 163 : return  "UNCERTAIN MEDIA";
               break;
      case 164 : return  "MAX THRDS REACHED";
               break;
      case 165 : return  "MONITORS NOT SUPPORTED";
               break;
      case 166 : return  "UNC DRIVER NOT INSTALLED";
               break;
      case 167 : return  "LOCK FAILED";
               break;
      case 168 : return  "SWAPIO FAILED";
               break;
      case 169 : return  "SWAPIN FAILED";
               break;
      case 170 : return  "BUSY";
               break;
      case 180 : return  "INVALID SEGMENT NUMBER";
               break;
      case 181 : return  "INVALID CALLGATE";
               break;
      case 182 : return  "INVALID ORDINAL";
               break;
      case 183 : return  "ALREADY EXISTS";
               break;
      case 184 : return  "NO CHILD PROCESS";
               break;
      case 185 : return  "CHILD ALIVE NOWAIT";
               break;
      case 186 : return  "INVALID FLAG NUMBER";
               break;
      case 187 : return  "SEM NOT FOUND";
               break;
      case 188 : return  "INVALID STARTING CODESEG";
               break;
      case 189 : return  "INVALID STACKSEG";
               break;
      case 190 : return  "INVALID MODULETYPE";
               break;
      case 191 : return  "INVALID EXE SIGNATURE";
               break;
      case 192 : return  "EXE MARKED INVALID";
               break;
      case 193 : return  "BAD EXE FORMAT";
               break;
      case 194 : return  "ITERATED DATA EXCEEDS 64K";
               break;
      case 195 : return  "INVALID MINALLOCSIZE";
               break;
      case 196 : return  "DYNLINK FROM INVALID RING";
               break;
      case 197 : return  "IOPL NOT ENABLED";
               break;
      case 198 : return  "INVALID SEGDPL";
               break;
      case 199 : return  "AUTODATASEG EXCEEDS 64k";
               break;
      case 200 : return  "RING2SEG MUST BE MOVABLE";
               break;
      case 201 : return  "RELOC CHAIN XEEDS SEGLIM";
               break;
      case 202 : return  "INFLOOP IN RELOC CHAIN";
               break;
      case 203 : return  "ENVVAR NOT FOUND";
               break;
      case 204 : return  "NOT CURRENT CTRY";
               break;
      case 205 : return  "NO SIGNAL SENT";
               break;
      case 206 : return  "FILENAME EXCED RANGE";
               break;
      case 207 : return  "RING2 STACK IN USE";
               break;
      case 208 : return  "META EXPANSION TOO LONG";
               break;
      case 209 : return  "INVALID SIGNAL NUMBER";
               break;
      case 210 : return  "THREAD 1 INACTIVE";
               break;
      case 211 : return  "INFO NOT AVAIL";
               break;
      case 212 : return  "LOCKED";
               break;
      case 213 : return  "BAD DYNALINK";
               break;
      case 214 : return  "TOO MANY MODULES";
               break;
      case 215 : return  "NESTING NOT ALLOWED";
               break;
      case 230 : return  "BAD PIPE";
               break;
      case 231 : return  "PIPE BUSY";
               break;
      case 232 : return  "NO DATA";
               break;
      case 233 : return  "PIPE NOT CONNECTED";
               break;
      case 234 : return  "MORE DATA";
               break;
      case 240 : return  "VC DISCONNECTED";
               break;
      case 303 : return  "INVALID PROCID";
               break;
      case 304 : return  "INVALID PDELTA";
               break;
      case 305 : return  "NOT DESCENDANT";
               break;
      case 306 : return  "NOT SESSION MANAGER";
               break;
      case 307 : return  "INVALID PCLASS";
               break;
      case 308 : return  "INVALID SCOPE";
               break;
      case 309 : return  "INVALID THREADID";
               break;
      case 310 : return  "DOSSUB SHRINK";
               break;
      case 311 : return  "DOSSUB NOMEM";
               break;
      case 312 : return  "DOSSUB OVERLAP";
               break;
      case 313 : return  "DOSSUB BADSIZE";
               break;
      case 314 : return  "DOSSUB BADFLAG";
               break;
      case 315 : return  "DOSSUB BADSELECTOR";
               break;
      case 316 : return  "MR MSG TOO LONG";
               break;
      case 317 : return  "MR MID NOT FOUND";
               break;
      case 318 : return  "MR UN ACC MSGF";
               break;
      case 319 : return  "MR INV MSGF FORMAT";
               break;
      case 320 : return  "MR INV IVCOUNT";
               break;
      case 321 : return  "MR UN PERFORM";
               break;
      case 322 : return  "TS WAKEUP";
               break;
      case 323 : return  "TS SEMHANDLE";
               break;
      case 324 : return  "TS NOTIMER";
               break;
      case 326 : return  "TS HANDLE";
               break;
      case 327 : return  "TS DATETIME";
               break;
      case 328 : return  "SYS INTERNAL";
               break;
      case 329 : return  "QUE CURRENT NAME";
               break;
      case 330 : return  "QUE PROC NOT OWNED";
               break;
      case 331 : return  "QUE PROC OWNED";
               break;
      case 332 : return  "QUE DUPLICATE";
               break;
      case 333 : return  "QUE ELEMENT NOT EXIST";
               break;
      case 334 : return  "QUE NO MEMORY";
               break;
      case 335 : return  "QUE INVALID NAME";
               break;
      case 336 : return  "QUE INVALID PRIORITY";
               break;
      case 337 : return  "QUE INVALID HANDLE";
               break;
      case 338 : return  "QUE LINK NOT FOUND";
               break;
      case 339 : return  "QUE MEMORY ERROR";
               break;
      case 340 : return  "QUE PREV AT END";
               break;
      case 341 : return  "QUE PROC NO ACCESS";
               break;
      case 342 : return  "QUE EMPTY";
               break;
      case 343 : return  "QUE NAME NOT EXIST";
               break;
      case 344 : return  "QUE NOT INITIALIZED";
               break;
      case 345 : return  "QUE UNABLE TO ACCESS";
               break;
      case 346 : return  "QUE UNABLE TO ADD";
               break;
      case 347 : return  "QUE UNABLE TO INIT";
               break;
      case 349 : return  "VIO INVALID MASK";
               break;
      case 350 : return  "VIO PTR";
               break;
      case 351 : return  "VIO APTR";
               break;
      case 352 : return  "VIO RPTR";
               break;
      case 353 : return  "VIO CPTR";
               break;
      case 354 : return  "VIO LPTR";
               break;
      case 355 : return  "VIO MODE";
               break;
      case 356 : return  "VIO WIDTH";
               break;
      case 357 : return  "VIO ATTR";
               break;
      case 358 : return  "VIO ROW";
               break;
      case 359 : return  "VIO COL";
               break;
      case 360 : return  "VIO TOPROW";
               break;
      case 361 : return  "VIO BOTROW";
               break;
      case 362 : return  "VIO RIGHTCOL";
               break;
      case 363 : return  "VIO LEFTCOL";
               break;
      case 364 : return  "SCS CALL";
               break;
      case 365 : return  "SCS VALUE";
               break;
      case 366 : return  "VIO WAIT FLAG";
               break;
      case 367 : return  "VIO UNLOCK";
               break;
      case 368 : return  "SGS NOT SESSION MGR";
               break;
      /* case 369 : return  "SMG INVALID SGID";
               break; */
      case 369 : return  "SMG INVALID SESSION ID";
               break;
      /* case 370 : return  "SMG NOSG";
               break; */
      case 370 : return  "SMG NO SESSIONS";
               break;
      /* case 371 : return  "SMG GRP NOT FOUND";
               break; */
      case 371 : return  "SMG SESSION NOT FOUND";
               break;
      case 372 : return  "SMG SET TITLE";
               break;
      case 373 : return  "KBD PARAMETER";
               break;
      case 374 : return  "KBD NO DEVICE";
               break;
      case 375 : return  "KBD INVALID IOWAIT";
               break;
      case 376 : return  "KBD INVALID LENGTH";
               break;
      case 377 : return  "KBD INVALID ECHO MASK";
               break;
      case 378 : return  "KBD INVALID INPUT MASK";
               break;
      case 379 : return  "MON INVALID PARMS";
               break;
      case 380 : return  "MON INVALID DEVNAME";
               break;
      case 381 : return  "MON INVALID HANDLE";
               break;
      case 382 : return  "MON BUFFER TOO SMALL";
               break;
      case 383 : return  "MON BUFFER EMPTY";
               break;
      case 384 : return  "MON DATA TOO LARGE";
               break;
      case 385 : return  "MOUSE NO DEVICE";
               break;
      case 386 : return  "MOUSE INV HANDLE";
               break;
      case 387 : return  "MOUSE INV PARMS";
               break;
      case 388 : return  "MOUSE CANT RESET";
               break;
      case 389 : return  "MOUSE DISPLAY PARMS";
               break;
      case 390 : return  "MOUSE INV MODULE";
               break;
      case 391 : return  "MOUSE INV ENTRY PT";
               break;
      case 392 : return  "MOUSE INV MASK";
               break;
      case 393 : return  "NO ERROR MOUSE NO DATA";
               break;
      case 394 : return  "NO ERROR MOUSE PTR DRAWN";
               break;
      case 395 : return  "INVALID FREQUENCY";
               break;
      case 396 : return  "NLS NO COUNTRY FILE";
               break;
      case 397 : return  "NLS OPEN FAILED";
               break;
      /* case 398 : return  "NLS NO CTRY CODE";
               break; */
      case 398 : return  "NO COUNTRY OR CODEPAGE";
               break;
      case 399 : return  "NLS TABLE TRUNCATED";
               break;
      case 400 : return  "NLS BAD TYPE";
               break;
      case 401 : return  "NLS TYPE NOT FOUND";
               break;
      case 402 : return  "VIO SMG ONLY";
               break;
      case 403 : return  "VIO INVALID ASCIIZ";
               break;
      case 404 : return  "VIO DEREGISTER";
               break;
      case 405 : return  "VIO NO POPUP";
               break;
      case 406 : return  "VIO EXISTING POPUP";
               break;
      case 407 : return  "KBD SMG ONLY";
               break;
      case 408 : return  "KBD INVALID ASCIIZ";
               break;
      case 409 : return  "KBD INVALID MASK";
               break;
      case 410 : return  "KBD REGISTER";
               break;
      case 411 : return  "KBD DEREGISTER";
               break;
      case 412 : return  "MOUSE SMG ONLY";
               break;
      case 413 : return  "MOUSE INVALID ASCIIZ";
               break;
      case 414 : return  "MOUSE INVALID MASK";
               break;
      case 415 : return  "MOUSE REGISTER";
               break;
      case 416 : return  "MOUSE DEREGISTER";
               break;
      case 417 : return  "SMG BAD ACTION";
               break;
      case 418 : return  "SMG INVALID CALL";
               break;
      case 419 : return  "SCS SG NOTFOUND";
               break;
      case 420 : return  "SCS NOT SHELL";
               break;
      case 421 : return  "VIO INVALID PARMS";
               break;
      case 422 : return  "VIO FUNCTION OWNED";
               break;
      case 423 : return  "VIO RETURN";
               break;
      case 424 : return  "SCS INVALID FUNCTION";
               break;
      case 425 : return  "SCS NOT SESSION MGR";
               break;
      case 426 : return  "VIO REGISTER";
               break;
      case 427 : return  "VIO NO MODE THREAD";
               break;
      case 428 : return  "VIO NO SAVE RESTORE THD";
               break;
      case 429 : return  "VIO IN BG";
               break;
      case 430 : return  "VIO ILLEGAL DURING POPUP";
               break;
      case 431 : return  "SMG NOT BASESHELL";
               break;
      case 432 : return  "SMG BAD STATUSREQ";
               break;
      case 433 : return  "QUE INVALID WAIT";
               break;
      case 434 : return  "VIO LOCK";
               break;
      case 435 : return  "MOUSE INVALID IOWAIT";
               break;
      case 436 : return  "VIO INVALID HANDLE";
               break;
      case 437 : return  "VIO ILLEGAL DURING LOCK";
               break;
      case 438 : return  "VIO INVALID LENGTH";
               break;
      case 439 : return  "KBD INVALID HANDLE";
               break;
      case 440 : return  "KBD NO MORE HANDLE";
               break;
      case 441 : return  "KBD CANNOT CREATE KCB";
               break;
      case 442 : return  "KBD CODEPAGE LOAD INCOMPL";
               break;
      case 443 : return  "KBD INVALID CODEPAGE ID";
               break;
      case 444 : return  "KBD NO CODEPAGE SUPPORT";
               break;
      case 445 : return  "KBD FOCUS REQUIRED";
               break;
      case 446 : return  "KBD FOCUS ALREADY ACTIVE";
               break;
      case 447 : return  "KBD KEYBOARD BUSY";
               break;
      case 448 : return  "KBD INVALID CODEPAGE";
               break;
      case 449 : return  "KBD UNABLE TO FOCUS";
               break;
      case 450 : return  "SMG SESSION NON SELECT";
               break;
      case 451 : return  "SMG SESSION NOT FOREGRND";
               break;
      case 452 : return  "SMG SESSION NOT PARENT";
               break;
      case 453 : return  "SMG INVALID START MODE";
               break;
      case 454 : return  "SMG INVALID RELATED OPT";
               break;
      case 455 : return  "SMG INVALID BOND OPTION";
               break;
      case 456 : return  "SMG INVALID SELECT OPT";
               break;
      case 457 : return  "SMG START IN BACKGROUND";
               break;
      case 458 : return  "SMG INVALID STOP OPTION";
               break;
      case 459 : return  "SMG BAD RESERVE";
               break;
      case 460 : return  "SMG PROCESS NOT PARENT";
               break;
      case 461 : return  "SMG INVALID DATA LENGTH";
               break;
      case 462 : return  "SMG NOT BOUND";
               break;
      case 463 : return  "SMG RETRY SUB ALLOC";
               break;
      case 464 : return  "KBD DETACHED";
               break;
      case 465 : return  "VIO DETACHED";
               break;
      case 466 : return  "MOU DETACHED";
               break;
      case 467 : return  "VIO FONT";
               break;
      case 468 : return  "VIO USER FONT";
               break;
      case 469 : return  "VIO BAD CP";
               break;
      case 470 : return  "VIO NO CP";
               break;
      case 471 : return  "VIO NA CP";
               break;
      case 472 : return  "INVALID CODE PAGE";
               break;
      case 473 : return  "CPLIST TOO SMALL";
               break;
      case 474 : return  "CP NOT MOVED";
               break;
      case 475 : return  "MODE SWITCH INIT";
               break;
      case 476 : return  "CODE PAGE NOT FOUND";
               break;
      case 477 : return  "UNEXPECTED SLOT RETURNED";
               break;
      case 478 : return  "SMG INVALID TRACE OPTION";
               break;
      case 479 : return  "VIO INTERNAL RESOURCE";
               break;
      case 480 : return  "VIO SHELL INIT";
               break;
      case 481 : return  "SMG NO HARD ERRORS";
               break;
      case 482 : return  "CP SWITCH INCOMPLETE";
               break;
      case 483 : return  "VIO TRANSPARENT POPUP";
               break;
      case 484 : return  "CRITSEC OVERFLOW";
               break;
      case 485 : return  "CRITSEC UNDERFLOW";
               break;
      case 486 : return  "VIO BAD RESERVE";
               break;
      case 487 : return  "INVALID ADDRESS";
               break;
      case 488 : return  "ZERO SELECTORS REQUESTED";
               break;
      case 489 : return  "NOT ENOUGH SELECTORS AVA";
               break;
      case 490 : return  "INVALID SELECTOR";
               break;
      case 491 : return  "SMG INVALID PROGRAM TYPE";
               break;
      case 492 : return  "SMG INVALID PGM CONTROL";
               break;
      case 493 : return  "SMG INVALID INHERIT OPT";
               break;
      case 494 : return  "VIO EXTENDED SG";
               break;
      case 495 : return  "VIO NOT PRES MGR SG";
               break;
      case 496 : return  "VIO SHIELD OWNED";
               break;
      case 497 : return  "VIO NO MORE HANDLES";
               break;
      case 498 : return  "VIO SEE ERROR LOG";
               break;
      case 499 : return  "VIO ASSOCIATED DC";
               break;
      case 500 : return  "KBD NO CONSOLE";
               break;
      case 501 : return  "MOUSE NO CONSOLE";
               break;
      case 502 : return  "MOUSE INVALID HANDLE";
               break;
      case 503 : return  "SMG INVALID DEBUG PARMS";
               break;
      case 504 : return  "KBD EXTENDED SG";
               break;
      case 505 : return  "MOU EXTENDED SG";
               break;
      case 506 : return  "SMG INVALID ICON FILE";
               break;
      default: return  "Unrecognized ErrorCode Value";
               break;
      }
   }

///*   testing code:  */
//  void main()
//    {
//      int result, integer;
//  
//      /* Loop until input value is 0. */
//      do
//      {
//          printf( "Enter number: " );
//          result = scanf( "%i", &integer );
//          printf( "  error text: %s \n\n\n", ErrorCodeText(integer) );
//      } while( integer );
//    }

