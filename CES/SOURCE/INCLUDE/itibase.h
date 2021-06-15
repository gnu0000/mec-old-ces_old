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
 * ItiBase.h
 * Mark Hampton
 * This module is part of DS/Shell (CS/Base)
 *
 * The following DLLs are refereced by this DLL:
 *    ItiError
 *
 * 0) General memory functions:
 *    ItiMemQueryRead
 *    ItiMemQueryWrite
 *    ItiMemQueryReadWrite
 *
 * 1) Heap based memory management.
 *    The heap based memory functions are for small to medium (< 64K)
 *    memory management.  The following functions are provided for the
 *    heap based memory management:
 *
 *    ItiMemCreateHeap
 *    ItiMemDestroyHeap
 *    ItiMemAlloc
 *    ItiMemRealloc
 *    ItiMemQuerySize
 *    ItiMemQueryAvail
 *    ItiMemFree
 *
 * 2) Segment based memory management.
 *    The segment based memory management gives you a segment up to
 *    64K in size.  Use these functions if you want a section of flat
 *    memory.
 *
 *    ItiMemAllocSeg
 *    ItiMemReallocSeg
 *    ItiMemQuerySegSize
 *    ItiMemFreeSeg
 *
// * 3) Huge memory management.  (currently commented out!)
// *    The huge memory management functions are for > 64K memory management.
// *    The number of huge memory allocs is relatively small.  
// *    The huge memory management functions are:
// *
// *    ItiMemAllocHuge
// *    ItiMemRellocHuge
// *    ItiMemQueryHugeSize
// *    ItiMemFreeHuge
 *
 *
 * Semaphores
 *
 * Event Semaphore API:
 *    ItiSemCreateEvent
 *    ItiSemOpenEvent
 *    ItiSemCloseEvent
 *    ItiSemWaitEvent
 *    ItiSemWaitMultiEvent (NOT IMPLEMENTED YET!)
 *    ItiSemSetWaitEvent
 *    ItiSemSetEvent
 *    ItiSemClearEvent
 *
 * Mutual exclusion (Mutex) semaphore API:
 *    ItiSemCreateMutex
 *    ItiSemOpenMutex
 *    ItiSemCloseMutex
 *    ItiSemRequestMutex
 *    ItiSemReleaseMutex   
 *
 * Comments:
 *
 * The only way to create a semaphore is to call ItiSemCreateXxxx, where
 * Xxxx is either Event or Mutex.  
 *
 * You must not call a event function with a mutex semaphore, and
 * vice versa.
 *
 * You should call ItiSemCloseXxxx when you are done with a semaphore,
 * to release its memory.  
 *
 *
 * DLL Management API
 *    ItiDllLoadDll
 *    ItiDllFreeDll
 *    ItiDllQueryProcAddress
 *    ItiDllQueryDllVersion
 *
 *
 * Processes & threads
 *
 * Thread management API:
 *    ItiProcCreateThread
 *    ItiProcSuspendThread
 *    ItiProcResumeThread
 *    ItiProcExitThread
 *    ItiProcEnterCritSect
 *    ItiProcExitCritSect
 *
 * Session managemant API:  (This API soon will migrate to the IPC API)
 *    ItiProcCreateSession  
 *    ItiProcDestroySession
 *    ItiProcSelectSession
 */




/***************************************************************************
 * General memory functions
 **************************************************************************/

/*
 * ItiMemQueryRead returns TRUE if the caller has read access for 
 * the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has read permission for the memory,
 * or FALSE if not.
 */

extern BOOL EXPORT ItiMemQueryRead (PVOID pvPointer);



/*
 * ItiMemQueryWrite returns TRUE if the caller has write access for 
 * the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has write permission for the memory,
 * or FALSE if not.
 */

extern BOOL EXPORT ItiMemQueryWrite (PVOID pvPointer);



/*
 * ItiMemQueryReadWrite returns TRUE if the caller has read and write
 * access for the specified pointer.
 *
 * Parameters: pvPointer      A pointer to the memory in question.
 *
 * Return value: TRUE if the caller has read and write permission 
 * for the memory, or FALSE if not.
 */

extern BOOL EXPORT ItiMemQueryReadWrite (PVOID pvPointer);





/***************************************************************************
 * Memory allocation flags
 **************************************************************************/

/*
 * Each API in this module that allocates or reallocates memory has a 
 * paramter usFlags.  These flags effect the memory that you allocate.
 * Permisible values are:
 *
 * Zero:          Use zero if you don't want anything special to happen
 *                to your allocated memory.
 *
 * MEM_ZERO_INIT: Use this if you want your memory to be initialized to
 *                all zeros.  Currently, the Huge memory API does not
 *                zero init the memory.
 *
 */

#define MEM_ZERO_INIT   0x0001






/***************************************************************************
 * Heap memory management functions
 **************************************************************************/


/* The MIN_HEAP_SIZE manifest constant defines the size of the smallest heap */
#define MIN_HEAP_SIZE   0

/* The MAX_HEAP_SIZE manifest constant defines the size of the largest heap */
#define MAX_HEAP_SIZE   65510U


/*
 * ItiMemCreateHeap returns a handle to a heap that may be used with 
 * the ItiMem functions.
 *
 * Parameters: usHeapSize        The size in bytes for the heap.  This
 *                               is used for the initial heap size.
 *
 * Return Value: A handle to a heap, or NULL on error.
 *
 * Comments: The usHeapSize is used for the initial size of the heap.
 * Later on, the heap may grow in size.  
 *
 * The maximum heap size is MAX_HEAP_SIZE bytes, defined at the top of
 * this header file.
 *
 * Each heap is allocated as a non-shared memory segment.  You should
 * strive to minimze the number of heaps that you create, as each heap
 * takes away from the number of non-shared memory segments available to
 * a given process.  From experimentation, about 5600 empty (0 byte) 
 * heaps can be created.  
 */

extern HHEAP EXPORT ItiMemCreateHeap (USHORT usHeapSize);




/*
 * ItiMemDestroyHeap destroys a previously created heap.
 *
 * Parameters: hheap       The handle to the heap to destroy.
 *
 * Return Value: None.
 *
 * Comments: All allocated memory objects in the heap are destroyed.
 */

extern void EXPORT ItiMemDestroyHeap (HHEAP hheap);





/*
 * ItiMemAlloc allocates usNumBytes from the specified heap.
 *
 * Parameters: hheap             A handle to the heap to allocate
 *                               from.
 *
 *             usNumBytes        The number of bytes to allocate.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A pointer to the allocated space, or NULL if no memory
 * could be allocated.
 *
 * Comments:
 * The largest chunk of memory that can be allocated is MAX_HEAP_SIZE bytes.
 */

extern PVOID EXPORT ItiMemAlloc (HHEAP  hheap, 
                                 USHORT usNumBytes,
                                 USHORT usFlags);





/*
 * ItiMemRealloc allocates usNumBytes from the specified heap.
 *
 * Parameters: hheap          A handle to the heap to allocate
 *                            from.
 *
 *             pvData         A pointer in the heap to resize.  May
 *                            be NULL.
 *
 *             usNewSize      The new size in bytes of the memory block.
 *
 *             usFlags        Memory allocation flags.  See the 
 *                            Memory allocation flags section at
 *                            top of this header file for the
 *                            allowable values.
 *
 * Return Value: A pointer to the reallocated space, or NULL if no memory
 * could be reallocated.  If no memory could be reallocated, the old
 * pointer is still valid.
 *
 * Comments:
 * If pvData is NULL, then this function behaves like ItiMemAlloc.
 */

extern PVOID EXPORT ItiMemRealloc (HHEAP   hheap, 
                                   PVOID   pvData, 
                                   USHORT  usNewSize,
                                   USHORT  usFlags);




/*
 * ItiMemQuerySize returns the size of a pointer that was 
 * allocated with ItiMemAlloc or ItiMemRealloc.
 *
 * Parameters: pvData         A pointer that was previously returned
 *                            from the ItiMemAlloc or ItiMemRealloc
 *                            function.
 *
 * Return value: The size in bytes of the pointer.  May be zero on 
 * error.
 *
 * Notes: If you call this function with a pointer that was not returned
 * from the ItiMemAlloc or ItiMemRealloc function, all hell will break 
 * loose.
 */

extern USHORT EXPORT ItiMemQuerySize (PVOID pvData);




/*
 * ItiMemQueryAvail returns the size of the largest available
 * memory block in the specified heap.
 *
 * Parameters: hheap          The heap to examine.
 *
 * Return value: The size in bytes of the largest memory block.
 * May be zero on error.
 */

extern USHORT EXPORT ItiMemQueryAvail (HHEAP hheap);





/*
 * ItiMemFree frees the specified memory from the specified heap.
 *
 * Parameters: hheap             A handle to the heap that the memory
 *                               was allocated from.
 *
 *             pvData            A pointer that was previously returned
 *                               by the ItiMemAlloc or ItiMemRealloc
 *                               function.
 *
 * Return value: None.
 */

extern void EXPORT ItiMemFree (HHEAP hheap, 
                               PVOID pvData);




/***************************************************************************
 * End of heap memory management functions.
 **************************************************************************/



/***************************************************************************
 * Segment memory management functions.
 **************************************************************************/


/* the MAX_SEG_SIZE manifest constant defines the size of the largest 
   segment. Note that it is an unsigned long. */

#define MAX_SEG_SIZE    65536UL

#if !defined (SEG_NONSHARED)

#define SEG_NONSHARED      0
#define SEG_GIVEABLE       1
#define SEG_GETTABLE       2
#define SEG_DISCARDABLE    4
#define SEG_SIZEABLE       8

#endif /* if !defined (SEG_NONSHARED) */

/*
 * ItiMemAllocSeg is used to allocate memory segments.
 *
 * Parameters: usBytesToAlloc    The number of bytes to allocate.
 *
 *             usAttribute       This contains flags for the kind
 *                               of memory object you want to create.
 *                               Usually, you will use SEG_NONSHARED.
 *                               See the description of the fsAttr
 *                               parameter of the DosAllocSeg API for
 *                               a full list.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A pointer to the memory segment, or zero on error.
 *
 * Comments:
 * If you use SEG_NONSHARED as the memory attribute, you may in fact
 * get a selector to shared memory.  This occurs when the operating system
 * runs out of private selectors.  (There are about 2000 private selectors
 * per process.)
 */

extern PVOID EXPORT ItiMemAllocSeg (USHORT usBytesToAlloc,
                                    USHORT usAttribute,
                                    USHORT usFlags);





/*
 * ItiMemReallocSeg is used to reallocate a memory segment.
 *
 * Parameters: pvData            The segment to resize.  May be NULL.
 *
 *             usNewSize         The number of bytes to allocate.
 *
 *             usFlags           Memory allocation flags.  See the 
 *                               Memory allocation flags section at
 *                               top of this header file for the
 *                               allowable values.
 *
 * Return Value: A pointer to the memory segment, or zero on error.
 *
 * Comments:
 * The pointer returned will not be different than the pointer 
 * passed in, unless NULL is passed in.
 *
 * If pvData is NULL, then this function behaves like ItiMemAllocSeg, 
 * with SEG_NONSHARED used as the usAttribute parameter.
 */

extern PVOID EXPORT ItiMemReallocSeg (PVOID    pvData,
                                      USHORT   usNewSize,
                                      USHORT   usFlags);






/*
 * ItiMemQuerySeg is used to query the size of memory segments.
 *
 * Parameters: pvData         The segment to size.
 *
 * Return Value: The size of the segment, in bytes.  Zero
 *               can be retured if an error occurred.
 */

extern USHORT EXPORT ItiMemQuerySeg (PVOID pvData);







/*
 * ItiMemFreeSeg is used to free memory segments.
 *
 * Parameters: pvData         A pointer to the segment to free.
 *
 * Return Value: None.
 */

extern void EXPORT ItiMemFreeSeg (PVOID pvData);





/***************************************************************************
 * End of segment memory management functions.
 **************************************************************************/



///***************************************************************************
// * Huge memory management functions.
// **************************************************************************/
//
//
///*
// * ItiMemAllocHuge is used to allocate > 64K memory objects.
// *
// * Parameters: ulBytesToAlloc    The number of bytes to allocate.
// *
// *             ulMaxRealloc      The maximum size to allow for resizing
// *                               the memory object.  If this value
// *                               is zero, then you are not allowed to 
// *                               grow the memory; however you are allowed
// *                               to shrink the memory.
// *
// *             usAttribute       This contains flags for the kind
// *                               of memory object you want to create.
// *                               Usually, you will use SEG_NONSHARED.
// *                               See the description of the fsAttr
// *                               parameter of the DosAllocSeg API for
// *                               a full list.
// *
// *             usFlags           Memory allocation flags.  See the 
// *                               Memory allocation flags section at
// *                               top of this header file for the
// *                               allowable values.
// *
// * Return Value: A huge pointer to the memory, or NULL on error.
// *
// * Comments: 
// * The ulMaxRealloc parameter reserves space in the calling process's 
// * LDT, thus limiting the total number of selectors available to that
// * process.
// */
//
//extern HPVOID EXPORT ItiMemAllocHuge (ULONG  ulBytesToAlloc,
//                                      ULONG  ulMaxRealloc,
//                                      USHORT usAttribute,
//                                      USHORT usFlags);
//
//                                      
//                                      
///*
// * ItiMemReallocHuge is used to reallocate > 64K memory objects.
// *
// * Parameters: hpvData       A pointer returned from the ItiMemAllocHuge or 
// *                           ItiMemReallocHuge function.
// *                           
// *             ulNewSize     The new size of the memory object.
// *
// *             usFlags       Memory allocation flags.  See the 
// *                           Memory allocation flags section at
// *                           top of this header file for the
// *                           allowable values.
// *
// * Return Value: A huge pointer to the memory, or NULL on error.
// *
// * If pvData is NULL, then this function behaves like ItiMemAllocSeg, 
// * with SEG_NONSHARED used as the usAttribute parameter.
// */
//
//extern HPVOID EXPORT ItiMemReallocHuge (HPVOID  hpvData,
//                                        ULONG   ulNewSize,
//                                        USHORT  usFlags);
//
//
//
///*
// * ItiMemQueryHugeSize returns the current size, in bytes, of 
// * a huge memory block.
// *
// * Parameters: hpvData       A huge pointer that was returned
// *                           from either ItiMemAllocHuge or
// *                           ItiMemReallocHuge.
// *
// * Return Value: Size in bytes of the pointer.  Zero may be
// * returned if there is an error.
// */
//
//extern ULONG EXPORT ItiMemQueryHugeSize (HPVOID hpvData);
//
//
//
///*
// * ItiMemFreeHuge is used to free a >64K object.
// *
// * Parameters: hpvData       A huge pointer that was returned
// *                           from either ItiMemAllocHuge or
// *                           ItiMemReallocHuge.
// *
// * Return Value: None.
// */
//
//extern void EXPORT ItiMemFreeHuge (HPVOID hpvData);
//
//
///***************************************************************************
// * End of huge memory management functions.
// **************************************************************************/



/***********************************************************************
 * Event semaphore API
 **********************************************************************/


#define SEM_PRIVATE     0
#define SEM_PUBLIC      1

#define ITIBASE_SEM_ALREADY_EXISTS    (ITIBASE_ERROR_BASE + 1)
#define ITIBASE_SEM_INVALID_NAME      (ITIBASE_ERROR_BASE + 2)
#define ITIBASE_SEM_NOT_FOUND         (ITIBASE_ERROR_BASE + 3)
#define ITIBASE_SEM_IS_SET            (ITIBASE_ERROR_BASE + 4)
#define ITIBASE_SEM_ALREADY_OWNED     (ITIBASE_ERROR_BASE + 5)
#define ITIBASE_SEM_TIMEOUT           (ITIBASE_ERROR_BASE + 6)

/*
 * ItiSemCreateEvent creates a mutual exclusion semaphore.  The 
 * semaphore can be named or anonymous.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  If this parameter
 *                               is NULL, then an anonymous semaphore
 *                               is created.
 *
 *             usFlags           Semaphore creation flags.  For named
 *                               semaphores, specify SEM_PRIVATE for
 *                               semaphores provate to your process,
 *                               or SEM_PUBLIC if the semaphore is to
 *                               be available to all processes.
 *                               This parameter is ignored if pszSemName
 *                               is NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_ALREADY_EXISTS
 *    ITIBASE_SEM_INVALID_NAME
 */

extern USHORT EXPORT ItiSemCreateEvent (PSZ     pszSemName, 
                                        USHORT  usFlags,
                                        PHSEM   phsemEvent);


/*
 * ItiSemOpenEvent opens an existing named semaphore.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  This parameter cannot
 *                               be NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_NOT_FOUND
 *    ITIBASE_SEM_INVALID_NAME
 */

extern USHORT EXPORT ItiSemOpenEvent (PSZ    pszSemName,
                                      PHSEM  phsemEvent);


                                      
                                      
/*
 * ItiSemCloseEvent closes an existing named or anonymous semaphore.
 *
 * Parameters: hsemEvent         A handle to an event semaphore.
 *
 * Return value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_IS_SET
 *
 * Comments: 
 * The return value is ITIBASE_SEM_SET if you close a named semaphore, 
 * and the semaphore is still set.  This seems to be informational, 
 * rather that an error.
 */
                                                                           
extern USHORT EXPORT ItiSemCloseEvent (HSEM hsemEvent);






/*
 * ItiSemWaitEvent waits for the specified event semaphore to be cleared.
 *
 * Parameters: hsemEvent      The event to wait for.
 *
 *             lTimeOut       The number of milliseconds to wait before
 *                            timing out.  Use SEM_INDEFINITE_WAIT
 *                            if you want to wait forever.
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_ALREADY_OWNED
 *    ITIBASE_SEM_TIMEOUT
 */

extern USHORT EXPORT ItiSemWaitEvent (HSEM hsemEvent, LONG lTimeOut);






/*
 * ItiSemSetWaitEvent sets a event semaphore and then waits for it
 * to be cleared.
 *
 * Parameters: hsemEvent      The event semaphore to use.
 *
 *             lTimeOut       The number of milliseconds to wait before
 *                            timing out.  Use SEM_INDEFINITE_WAIT
 *                            if you want to wait forever.
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_ALREADY_OWNED
 *    ITIBASE_SEM_TIMEOUT
 */

extern USHORT EXPORT ItiSemSetWaitEvent (HSEM hsemEvent, LONG lTimeOut);






/*
 * ItiSemSetEvent sets a given event semaphore.
 *
 * Parameters: hsemEvent      The event semaphore to set.
 *
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_ALREADY_OWNED
 */

extern  USHORT EXPORT ItiSemSetEvent (HSEM hsemEvent);






/*
 * ItiSemClearEvent clears a given event semaphore.
 *
 * Parameters: hsemEvent      The event semaphore to clear.
 *
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_ALREADY_OWNED
 */

extern USHORT EXPORT ItiSemClearEvent (HSEM hsemEvent);




/***********************************************************************
 * End of event semaphore API
 **********************************************************************/




/***********************************************************************
 * Mutual exclusion semaphore API
 **********************************************************************/


/*
 * ItiSemCreateMutex creates a mutual exclusion semaphore.  The 
 * semaphore can be named or anonymous.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  If this parameter
 *                               is NULL, then an anonymous semaphore
 *                               is created.
 *
 *             usFlags           Semaphore creation flags.  For named
 *                               semaphores, specify SEM_PRIVATE for
 *                               semaphores provate to your process,
 *                               or SEM_PUBLIC if the semaphore is to
 *                               be available to all processes.
 *                               This parameter is ignored if pszSemName
 *                               is NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_ALREADY_EXISTS
 *    ITIBASE_SEM_INVALID_NAME
 */

extern USHORT EXPORT ItiSemCreateMutex (PSZ     pszSemName, 
                                        USHORT  usFlags,
                                        PHSEM   phsemMutex);


/*
 * ItiSemOpenMutex opens an existing named semaphore.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  This parameter cannot
 *                               be NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_NOT_FOUND
 *    ITIBASE_SEM_INVALID_NAME
 */

extern USHORT EXPORT ItiSemOpenMutex (PSZ    pszSemName,
                                      PHSEM  phsemMutex);


                                      
                                      
/*
 * ItiSemCloseMutex closes an existing named or anonymous semaphore.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 * Return value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_IS_SET
 *
 * Comments: 
 * The return value is ITIBASE_SEM_SET if you close a named semaphore, 
 * and the semaphore is still set.  This seems to be informational, 
 * rather that an error.
 */
                                                                           
extern USHORT EXPORT ItiSemCloseMutex (HSEM hsemMutex);





/*
 * ItiSemRequestMutex requests an mutex semaphore.  If the semaphore
 * is already owned, the calling thread will block until either the
 * semaphore is released, or until the call times out.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 *             lTimeOut          The number of to miliseconds to wait
 *                               before timing out.
 *
 * Return value:
 * Zero if the function was successfull.  Otherwise, it is one of the
 * following error codes:
 *    ITIBASE_SEM_TIMEOUT
 */

extern BOOL EXPORT ItiSemRequestMutex (HSEM    hsemMutex, 
                                       LONG    lTimeOut);




/*
 * ItiSemReleaseMutex releases an mutex semaphore.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 * Return value:
 * Zero if the function was successfull.  Otherwise, it is one of the
 * following error codes:
 *    ITIBASE_SEM_ALREADY_OWNED
 */

extern BOOL EXPORT ItiSemReleaseMutex (HSEM hsemMutex);



/***********************************************************************
 * End of mutual exclusion semaphore API
 **********************************************************************/


/***********************************************************************
 * Start of DLL management API
 **********************************************************************/

typedef void (EXPORT *PFNMENUNAME) (PSZ pszMenuName, USHORT usStringSize, PUSHORT pusWindowID);

typedef BOOL (EXPORT *PFNCOMPAT) (PSZ pszVersion);

typedef BOOL (EXPORT *PFNINIT) (void);

typedef void (EXPORT *PFNDEINIT) (void);

typedef USHORT (EXPORT *PFNVERSION) (void);

typedef USHORT (EXPORT *PDLLFN)();

typedef USHORT (EXPORT *PDLLWNDFN)(HWND hwnd);

typedef BOOL (EXPORT *PAVAILFN)(USHORT, USHORT);

/*
 * ItiDllLoadDll loads the dynamic link library pszDllName, and verifies 
 * that it is compatible with usVersion.
 *
 * Parameters: pszDllName:    The name of the DLL to load.  Usually,
 *                            the caller should not include the ".DLL"
 *                            extension.
 *
 *             pszVersion:    The version string of the caller.
 *
 * Return Value:
 * A handle to the module, or 0 on error.   
 * 
 * Comments:
 * Errors could be caused by any of the following:  DLL not found,
 * DLL doesn't have version function, DLL is incompatible with your
 * version.
 */

extern HMODULE EXPORT ItiDllLoadDll (PSZ pszDllName, PSZ pszVersion);






/*
 * ItiDllQueryProcAddress returns a pointer to the address of a 
 * function in the specified DLL.
 *
 * Parameters: hmodDll           The DLL that contains the function.
 *
 *             pszFunctionName   The name of the function to be found.
 *
 * Return Value:
 * A pointer to the function, or NULL on error.
 *
 */

extern PDLLFN EXPORT ItiDllQueryProcAddress (HMODULE hmodDll,
                                             PSZ     pszFunctionName);





/*
 * ItiDllQueryDllVersion returns the version number of the specified DLL.
 *
 * Parameters: hmodDll           The DLL to check.
 *
 * Return Value:
 * The version number of the DLL, or 0 on error.
 *
 */

extern USHORT EXPORT ItiDllQueryDllVersion (HMODULE hmodDll);







/*
 * ItiDllFreeDll frees the given DLL.  The caller must no longer 
 * try to use the DLL, unless ItiDllLoadDll is called.
 */

extern void EXPORT ItiDllFreeDll (HMODULE hmodDll);



/***********************************************************************
 * End of DLL management API
 **********************************************************************/




#define THREAD_MIN_STACK_SIZE 8192
#define THREAD_STACK_SIZE     10240

#define THREAD FAR _loadds


#define ITIBASE_INVALID_THREAD_ID  (ITIBASE_ERROR_BASE + 10)

/***********************************************************************
 * Thread management API
 **********************************************************************/

/*
 * ItiProcCreateThread creates a thread.  NOTE: THIS FUNCTION CANNOT BE
 * USED BY A PROCESS THAT USES THE C RUN TIME LIBRARY!
 *
 * Parameters: pfnThreadFunc     A pointer to the thread function.
 *
 *             ptidThread        A pointer to a TID (thread ID).
 *
 *             usStackSize       The number of bytes to allocate for
 *                               the thread's stack.  If this parameter
 *                               is less than THREAD_MIN_STACK_SIZE, then 
 *                               the default stack size of THREAD_STACK_SIZE 
 *                               is used.  
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ERROR_NOT_ENOUGH_MEMORY
 */

extern USHORT EXPORT ItiProcCreateThread (PFNTHREAD pfnThreadFunc, 
                                          PTID      ptidThread,
                                          USHORT    usStackSize);




/*
 * ItiProcSuspendThread suspends a given thread.
 *
 * Parameters: tidThread         The thread to suspend.
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ITIBASE_INVALID_THREAD_ID
 */

extern USHORT EXPORT ItiProcSuspendThread (TID  tidThread);





/*
 * ItiProcResumeThread resumes a given thread.
 *
 * Parameters: tidThread         The thread to resume.
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ITIBASE_INVALID_THREAD_ID
 */

extern USHORT EXPORT ItiProcResumeThread (TID  tidThread);





/*
 * ItiProcExitThread exits the current thread.  YOU MUST NOT USE THIS
 * FUNCTION WITH THREADS THAT USE THE C RUN-TIME LIBRARY.
 *
 * Parameters: None.
 *
 * Return Value: This function never returns.
 */

extern void EXPORT ItiProcExitThread (void);






/*
 * ItiProcEnterCritSect suspends all but the current thread for a
 * given process.
 *
 * Parameters: None.
 *
 * Return Value: Zero is returned on success, or non-zero on failure.
 *
 * Comments: You should never have to call this function.  You should 
 * use semaphores instead.  You will be haunted for the rest of your
 * life if you use this function.
 */

extern USHORT EXPORT ItiProcEnterCritSect (void);






/*
 * ItiProcExitCritSect resumes all of the threads previously suspended
 * by ItiProcEnterCritSect.
 *
 * Parameters: None.
 *
 * Return Value: Zero is returned on success, or non-zero on failure.
 *
 *    
 */

extern USHORT EXPORT ItiProcExitCritSect (void);



/***********************************************************************
 * End of thread management API
 **********************************************************************/


/***********************************************************************
 * Start of session management API
 **********************************************************************/

/* session flags */
#define SESSION_RELATED          0x80000000
#define SESSION_UNRELATED        0x00000000
#define SESSION_BACKGROUND       0x40000000
#define SESSION_FOREGROUND       0x00000000

#define SESSION_MAXIMIZED        0x00000002
#define SESSION_ICONIC           0x00000004
#define SESSION_NO_AUTO_CLOSE    0x00000008

#define SESSION_FULL_SCREEN      0x01000000
#define SESSION_WINDOWED         0x02000000
#define SESSION_PM               0x03000000


typedef USHORT SID;

typedef SID *PSID;

#define ITIBASE_SESSION_IN_BACKGROUND     (ITIBASE_ERROR_BASE + 20)
#define ITIBASE_SESSION_NOT_FOREGROUND    (ITIBASE_ERROR_BASE + 21)


extern USHORT EXPORT ItiProcCreateSession (PSZ   pszSessionTitle,
                                           PSZ   pszProgramName,
                                           PSZ   pszProgramParameters,
                                           ULONG ulFlags,
                                           PPID  ppidProcess,
                                           PSID  psidSession);


extern USHORT EXPORT ItiProcDestroySession (SID sidSession);


extern USHORT EXPORT ItiProcSelectSession (SID sidSession);

/***********************************************************************
 * End of session management API
 **********************************************************************/


