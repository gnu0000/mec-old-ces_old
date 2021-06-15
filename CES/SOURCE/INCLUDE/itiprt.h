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


/*  ItiPrt                     (Online QuickHelp is available for this module)
 *  
 *    This module provides report page formatting functions.  It contains
 *    all of the interface calls to the OS/2 GPI.
 *
 *    Before any of the formatting functions can be used the
 *    ItiPrtDLLLoadInfo function must be called first.
 *
 *       ItiDllQueryVersion
 *       ItiDllQueryCompatibility
 *       ItiPrtLoadDLLInfo 
 *       ItiPrtSetError
 *       ItiPrtSkipLns        
 *       ItiPrtKeepNLn        
 *       ItiPrtMoveTo
 *      ~ItiPrtIncCurrentLn 
 *      ~ItiPrtQueryCurrentColNum 
 *      ~ItiPrtQueryCurrentLnNum  
 *      ~ItiPrtQueryCurrentPgNum  
 *      ~ItiPrtQueryTotalNumDisplayLns 
 *      ~ItiPrtQueryRemainNumDisplayLns 
 *       ItiPrtConvertMmToCol 
 *       ItiPrtConvertColToMm 
 *       ItiPrtChangeHeaderLn   
 *       ItiPrtStartOfDisplayLn 
 *       ItiPrtPutTextAt
 *       ItiPrtDrawSeparatorLn
 *       ItiPrtNewPg 
 *       ItiPrtSetActivePageTo
 *       ItiPrtBeginReport 
 *       ItiPrtEndReport 
 *       ItiPrtWrappedLnCnt
 *
 *    The functions marked with a "~" do not return an error
 *    value, but rather the function result.
 *    
 */
#if !defined (INCL_ITIPRT)
#define  INCL_ITIPRT

#include  <limits.h>

#define  ITIPRT_DLL_VERSION      1

#define  ALIGNMENT  enum ALIGNMENT_FORMAT
typedef  ALIGNMENT        {LNINC =  1,
                           WRAP  =  2,
                           CLIP  =  4,
                           CENTER=  8,
                           RIGHT = 16,
                           LEFT  = 32,
                           NONE  = 64};

//#define  CURRENT_LN                 0
#define  CURRENT_LN       (USHRT_MAX - 400)

#define  MAX_PAPER_COL   500

#define  CURRENT_COL      (USHRT_MAX - MAX_PAPER_COL)

/* -- Right margin bitwise OR values for use in PutTextAt function. */
#define  FROM_RT_EDGE           16384
#define  REL_LEFT                8192
#define  USE_RT_EDGE             4096

/* -- NamedLn values for headers, used by ItiPrtChangeHeaderLn.     */
#define  ITIPRT_TITLE               0
#define  ITIPRT_SUBTITLE            1
#define  ITIPRT_SUB_SUBTITLE        2
#define  ITIPRT_NEWPG               3

/* -- Returned by ItiPrtKeepNLns if a new page was started.         */
#define  PG_WAS_INC               100


#define  ITIPRT_USE_GRAPHICS   "ITIUSEGRAPHICS"
#define  ITIPRT_PIDWAIT        'Z'


#define REPORTSTATUS   enum REPORT_STATUS

typedef REPORTSTATUS   {PAUSED = 1, PRINTING, QUERYING, FORMATTING,
                        ENDED,  STARTED,  NOTSTARTED, CONTINUING,
                        NODEV};



/* ----------------------------------------------------------------------- *\
 *                                                            Error Codes  *
\* ----------------------------------------------------------------------- */
#define  ITIPRT_NOT_INITIALIZED            (ITIPRT_ERROR_BASE + 11)
#define  ITIPRT_CALL_VERSION_CHK           (ITIPRT_ERROR_BASE + 13)
#define  ITIPRT_PRINTING_ERR               (ITIPRT_ERROR_BASE + 17)
#define  ITIPRT_COORDINATE_ERR             (ITIPRT_ERROR_BASE + 19)
#define  ITIPRT_INIT_ERR                   (ITIPRT_ERROR_BASE + 23)
#define  ITIPRT_UNIT_ERR                   (ITIPRT_ERROR_BASE + 29)




/* ----------------------------------------------------------------------- *\
 *                                                          API Functions  *
\* ----------------------------------------------------------------------- */
USHORT EXPORT ItiDllQueryVersion (VOID);
BOOL   EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);



/* -------------------------------------------------------------------------
 * ItiPrtLoadDLLInfo
 *
 *    This function initializes the report's internal information structure
 * with the text strings that are passed to it via the prept pointer.  The
 * argument pointers from the invoking application's command line are used
 * to check for any run time user information flags and values that are
 * relevent to this DLL, otherwise those arguments are ignored.
 *
 *    One future flag might be to toggle between "draft" and "hi-res" print
 * quality modes.
 *
 *
 * Parameters:    hab        Anchor block handle from application.
 *                hmq        Message queue handle from application.
 *                prept      Pointer to structure of report title stings.
 *                argcnt     Argument count from application's cmd line.
 *                argvars[]  Arguments  from application's cmd line.
 *
 * Return value:  0 if successful or
 *                ITIPRT_INIT_ERR
 */

extern USHORT EXPORT ItiPrtLoadDLLInfo
                (HAB hab, HMQ hmq, PREPT prept, INT argcnt, CHAR *argvars[]);















/* -------------------------------------------------------------------------
 * ItiPrtSetError
 *
 *    This function screens returned error codes from PM functions and
 *    for now sends pszErrorText to the debug window.  This is done to
 *    facilitate the integration of our own error processing routine if we
 *    ever write one.
 *
 * Parameters: pszErrorText   A pointer to the error message text string.
 *
 * Return value: 0 if severity level of the errid is just a warning.
 *
 */
 
extern USHORT EXPORT ItiPrtSetError (PSZ pszErrorText);






/* -------------------------------------------------------------------------
 * ItiPrtSkipLns 
 *
 *    Skips over the specified number of lines.  This function is used
 *    to insert blank lines on the current page; if the requested number
 *    of lines causes a new page to be started the skipping stops at the
 *    first display line of the new page.  The page number is returned to
 *    facilitate the use of this function in control loops.
 *
 * Parameter: usNumLnsToSkip  
 *
 * Return value: The page number of the current page.
 */

extern USHORT EXPORT ItiPrtSkipLns  (USHORT usNumLnsToSkip);






/* -------------------------------------------------------------------------
 * ItiPrtKeepNLns
 *
 *     Keeps the next N text lines together.  If there are not N lines
 *     remaining on the current page the current location line is 
 *     incremented down to the first display line of the next page.
 *
 * Parameter:     usNumLnsToKeepTogether
 *
 * Return value:  0          if the N lines will fit on the current page or
 *                PG_WAS_INC if lines were skipped and a new page started.
 */

extern USHORT EXPORT ItiPrtKeepNLns (USHORT usNumLnsToKeepTogether);








/* -------------------------------------------------------------------------
 * ItiPrtMoveTo
 *
 *    Resets the current writing location on the current page iff
 *    the requested line and column are within the display space.
 *
 * Parameters:  usLn   The requested page's display line.
 *              usCol  The column location on the requested line.
 */

extern USHORT EXPORT ItiPrtMoveTo  (USHORT usLn, USHORT usCol);





/* -------------------------------------------------------------------------
 * ItiPrtIncCurrentLn
 *
 *    This function increments the value of the current line number.
 *
 * Parameters:    none
 *
 * Return value:  The new current line value.
 *
 */

extern USHORT EXPORT ItiPrtIncCurrentLn  (VOID);






/* -------------------------------------------------------------------------
 * ItiPrtQueryCurrentColNum 
 *
 *    This function returns the value of the current column number.
 *
 * Parameter:     none
 *
 * Return value:  The current column number.
 *
 */

extern USHORT EXPORT ItiPrtQueryCurrentColNum (VOID);






/* -------------------------------------------------------------------------
 * ItiPrtQueryCurrentLnNum
 *
 *     This function returns the value of the current line number.
 *
 * Parameters:    none
 *
 * Return value:  The current line number.
 *
 */

extern USHORT EXPORT ItiPrtQueryCurrentLnNum  (VOID);



/* -------------------------------------------------------------------------
 * ItiPrtQueryCurrentPgNum
 *
 *    This function returns the current page number.
 *
 */
 
extern USHORT EXPORT ItiPrtQueryCurrentPgNum  (VOID);







/* -------------------------------------------------------------------------
 * ItiPrtQueryTotalNumDisplayLns
 *
 *    This function returns the number of lines in the display space of
 *    the page; ie. the number of lines on a page minus the number of lines
 *    currently used for the header and footer.
 * 
 */

extern USHORT EXPORT ItiPrtQueryTotalNumDisplayLns  (VOID);     









/* -------------------------------------------------------------------------
 * ItiPrtQueryRemainNumDisplayLns
 *
 *    This function returns the remaining number of lines in the display
 *    space on the page.
 *
 */

extern USHORT EXPORT ItiPrtQueryRemainNumDisplayLns (VOID); 









/* -------------------------------------------------------------------------
 * ItiPrtConvertMmToCol
 *
 *    Converts millimeters to column widths.
 *
 */ 

extern USHORT EXPORT ItiPrtConvertMmToCol (USHORT usMm,  PUSHORT pusCol);




/* -------------------------------------------------------------------------
 * ItiPrtConvertColToMm
 *
 *    Converts column widths to millimeters.
 *
 */ 

extern USHORT EXPORT ItiPrtConvertColToMm (USHORT usCol, PUSHORT pusMm);







/* -------------------------------------------------------------------------
 * ItiPrtChangeHeaderLn   
 *
 *    This function changes the text of the page header.
 *
 * Parameters:    usNamedLn   The value can be any of the following:
 *                                     ITIPRT_TITLE
 *                                     ITIPRT_SUBTITLE
 *
 *                pszNewText  The replacement text.
 *
 *
 * Return value:  0 if successful.
 *
 */

extern USHORT EXPORT ItiPrtChangeHeaderLn (USHORT usNamedLn, PSZ pszNewText);







/* -------------------------------------------------------------------------
 * ItiPrtStartOfDisplayLn 
 *
 *     This function resets the current position to the requested line number.
 *     If the requested value is greater than the number of lines on a page 
 *     than the current line is used.
 *
 * Parameters:    usRequestedLn  The requested line number; can be CURRENT_LN.
 *                pusLn          The actual line moved to.
 *
 * Return value:  0 if successful.
 *
 */

extern USHORT EXPORT ItiPrtStartOfDisplayLn
                        (USHORT usRequestedLn, PUSHORT pusLn);






/* -------------------------------------------------------------------------
 * ItiPrtPutTextAt 
 *
 *    This function will write a string to the page space at the specified
 *    location. 
 *
 *
 * Parameter    Description
 * ----------------------------------------------------------------------
 * usLn         Display line for text.
 *
 * usCol        Column for text.
 *
 * algnMode     Specifies the alignment of the string on the line. 
 *              These values may be bitwise ORed together.  Note that
 *              the default values are NONE and CLIP.  The precedence
 *              order of the values within the subgroups are:
 *
 *                        CLIP > WRAP 
 *
 *                        NONE > LEFT > RIGHT > CENTER
 *                         
 *                        LNINC
 *
 *              i.e. if given (CENTER | LEFT | CLIP) the text will be
 *              left aligned at <ptl> and clipped at the <lRtMargin>.
 *
 *              Note that the value LNINC can be bitwise ORed with the
 *              algnMode value to increment the current line after text
 *              is written.
 *
 *              <algnMode> can be any of the following values:
 *
 *                 Value      Meaning
 *                 ------------------------------------------------------
 *                 CLIP       (DEFAULT)  Any text that goes beyond the 
 *                            <lRtMargin> is clipped from the page view.
 *
 *                 WRAP       Any text that goes beyond the <lRtMargin>
 *                            wrapped around to the next line with its
 *                            left margin being the same as the starting
 *                            point. 
 *                       _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
 *
 *                 NONE       (DEFAULT)  Start the text at the current 
 *                            X,Y position;  The value <ptl> should be
 *                            set by a call to ItiPrtQueryCurrentPt()
 *                            which usually is the end of the text
 *                            written by the last GpiCharString or
 *                            GpiMove.  
 *                            Note that this <ptl> point also becomes
 *                            the new left margin for wrapping.
 *
 *                 LEFT       The starting point is moved to the leftmost
 *                            point of the current display line.
 *              
 *                 RIGHT      Calculates the needed starting point such 
 *                            that the string will END at the rightmost 
 *                            edge of the page space.  Will use the value
 *                            specified by <lRtMargin> if given, otherwise
 *                            defaults to the rightmost edge of the page.
 *
 *                 CENTER     Calculates the needed starting point to
 *                            center the given text on the current line.
 *
 *
 * usRtMCol     The number of columns FROM the LEFT side at which to
 *              stop the text.  If this value locates a col greater than the
 *              rightmost edge of the page, or if the given <lRtMCol> is less
 *              than the starting point plus one maximum character width, then
 *              the right margin defaults to the rightmost side of the page
 *              space.  
 *
 *                 USE_RT_EDGE  Use the right page edge as the margin.
 *
 *              The number of columns may be bitwise ORed the following:
 *
 *                 FROM_RT_EDGE Specifies <usRtMCol> as the number of 
 *                              columns from the RIGHT page space edge.
 *
 *                 REL_LEFT     Specifies <usRtMCol> as the number of 
 *                              columns from the LEFT MARGIN given by <usCol>.
 *
 *
 * psz          The null terminated string that is to be printed.
 *
 *
 *
 * Return Value:
 *              0    if successful.
 *
 * Errors:
 *    Could be one of the following values:
 *              ITIRPT_INVALID_PARM
 *              ITIPRT_PRINTING_ERR
 *
 * Comments:
 *    Currently WRAP works only for LEFT justified text; also the current
 *    line is incremented after the last one, as if LNINC were used.
 * 
 */

extern USHORT EXPORT ItiPrtPutTextAt (USHORT usLn,        USHORT usCol,
                               ALIGNMENT algnMode, USHORT usRtMarginCol,
                               PSZ psz);







/* -------------------------------------------------------------------------
 * ItiPrtDrawSeparatorLn
 *
 *    This function draws a separator line FromCol ToCol on the
 *    given display page line number.  
 *
 * Parameters:    lusLnNum    Display space line number.
 *                lFromCol    Starting column.
 *                lToCol      Ending column.
 *
 * Return value: 0 if successful otherwise... 
 *
 * Error values: ITIPRT_COORDINATE_ERR
 *
 *
 * Comments:   If the given line number is 0 then the current line is used.
 *             Also note that the current line number is NOT changed, ie
 *             you have to call ItiPrtIncCurrentLine.  This done so that it
 *             is convenient for you to draw more than one separator line
 *             segment on the same line.
 *
 */

extern USHORT EXPORT ItiPrtDrawSeparatorLn
                           (USHORT usLnNum, USHORT usFromCol, USHORT usToCol);







/* -------------------------------------------------------------------------
 * ItiPrtNewPg       
 *
 *     Sends needed device escape codes, sets the ReportStatus and
 *     asscociated internal location variables.
 *
 */

extern USHORT EXPORT ItiPrtNewPg (VOID);





/* -- Disable use of printer by using FALSE
 *    reactivate by using TRUE
 */
USHORT EXPORT ItiPrtActive (BOOL bActivate);




/* -------------------------------------------------------------------------
 * ItiPrtBeginReport 
 *
 *    This function starts a report by sending needed device escape codes.
 *
 * Parameters:    pszDocumentName   Name that will be displayed in the
 *                                  printer's queue.
 *
 * Return Value:  0               if started OK,
 *                STARTED         if the report was already started,
 *                ITIPRT_INIT_ERR if the report failed to start.
 *
 */

extern USHORT EXPORT ItiPrtBeginReport (PSZ pszDocumentName);







/* -------------------------------------------------------------------------
 * ItiPrtEndReport   
 *
 *    This function sends needed device escape codes and
 *    sets the ReportStatus. 
 *
 */

extern USHORT EXPORT ItiPrtEndReport (VOID);



/* -------------------------------------------------------------------------
 * ItiPrtWrappedLnCnt 
 */
extern USHORT EXPORT ItiPrtWrappedLnCnt (PSZ pszString,
                                         USHORT usColWidth,
                                         BOOL bIgnorePgDim);



#endif /* !defined INCL_ITIPRT */
