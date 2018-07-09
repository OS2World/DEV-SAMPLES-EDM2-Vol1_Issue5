/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R3STUBS.C - Ring 3 stubs
**
** History:
** 6/29/93 - created
**
*******************************************************************************
******************************************************************************/

#include "r3inc.h"


/******************************************************************************
** Name of IFS
******************************************************************************/

const char FS_NAME[] = "EXR0R3";


/******************************************************************************
*******************************************************************************
**
** Primary entry points
**
*******************************************************************************
******************************************************************************/





/*-----------------------------------------------------------------------------
--
-- Volume management
--
-----------------------------------------------------------------------------*/


/******************************************************************************
**
** FS_ATTACH - Attach or detach a drive or device
**
** Parameters
** ----------
** unsigned short flag                  indicates attaching/detaching
**   values:
**     FSA_ATTACH               attach drive/device
**     FSA_DETACH               detach drive/device
**     FSA_ATTACH_INFO          return info on attached drive/device
** char far *pDev                       drive or device that is being attached/detached
** struct vpfsd far *pvpfsd             pointer to FSD dependant volume parameters
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** void far *pParm                      UNVERIFIED pointer to FSD dependant attachment info
** unsigned short far *pLen             length of area pointed to by pParm
**
******************************************************************************/

#pragma argsused
short int far pascal FSD_ATTACH(unsigned short flag, char *pDev,
                               struct vpfsd *pvpfsd, struct cdfsd *pcdfsd,
                               void *pParm, unsigned short *pLen) {
   return NO_ERROR;
}


/******************************************************************************
**
** FS_MOUNT - Examine a volume to determine if the IFS knows its format
**
** Parameters
** ----------
** unsigned short flag                  indicates function to perform
**   values:
**     MOUNT_MOUNT              mount or accept the volume
**     MOUNT_VOL_REMOVED        volume has been removed
**     MOUNT_RELEASE            release all resources associated with the volume
**     MOUNT_ACCEPT             accept the volume in preparation for formatting
** struct vpfsi far *pvpfsi             pointer to FSD independant volume parameters
** struct vpfsd far *pvpfsd             pointer to FSD dependant volume parameters
** unsigned short hVPB                  volume handle
** char far *pBoot                      pointer to sector 0 data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_MOUNT(unsigned short flag, struct vpfsi far *pvpfsi,
                              struct vpfsd far *pvpfsd, unsigned short hVPB, char far *pBoot)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FSINFO - Get/Set file system information
**
** Parameters
** ----------
** unsigned short flag                  indicates function to perform
**   values:
**     INFO_RETREIVE                    retrieve information
**     INFO_SET                         set information
** unsigned short hVPB                  volume handle
** char far *pData                      UNVERIFIED pointer to data buffer
** unsigned short cbData                length of data buffer
** unsigned short level                 type of information to return
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FSINFO(unsigned short flag, unsigned short hVPB,
                               char far *pData, unsigned short cbData,
                               unsigned short level)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FLUSHBUF - Flush buffers for a specified volume
**
** Parameters
** ----------
** unsigned short hVPB                  handle to volume to flush
** unsigned short flag                  indicates whether to discard or retain cache
**   values:
**     FLUSH_RETAIN     retain cached information
**     FLUSH_DISCARD    discard cached information
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FLUSHBUF(unsigned short hVPB, unsigned short flag)
{
   return ERROR_NOT_SUPPORTED;
}



/*-----------------------------------------------------------------------------
--
-- Directory management
--
-----------------------------------------------------------------------------*/



/******************************************************************************
**
** FS_CHDIR - Change current directory
**
** Parameters
** ----------
** unsigned short flag                  indicates flavor of call
**   values:
**     CD_EXPLICIT      creating a new current directory
**     CD_VERIFY        verifying a current directory
**     CD_FREE          freeing an instance of a current directory
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pDir                       pointer to directory to change to
** unsigned short iCurDirEnd            offset to the end of the current directory in pDir
**
******************************************************************************/

#pragma argsused
short int far pascal FS_CHDIR(unsigned short flag, struct cdfsi far *pcdfsi,
                              struct cdfsd far *pcdfsd, char far *pDir,
                              unsigned short iCurDirEnd)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_MKDIR - Make a new directory
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to directory name to create
** unsigned short iCurDirEnd            offset to the end of the current directory 
**                                      in pName
** char *pEABuf                         UNVERIFIED pointer to EAs to attach 
**                                      to new directory
** unsigned short flags                 0x40 = directory is non 8.3 filename
**
******************************************************************************/

#pragma argsused
short int far pascal FS_MKDIR(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                              char far *pName, unsigned short iCurDirEnd,
                              char far *pEABuf, unsigned short flags)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_RMDIR - Delete directory
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to directory name to delete
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
**
******************************************************************************/

#pragma argsused
short int far pascal FS_RMDIR(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                              char far *pName, unsigned short iCurDirEnd)
{
   return ERROR_NOT_SUPPORTED;
}



/*-----------------------------------------------------------------------------
--
-- File management
--
-----------------------------------------------------------------------------*/



/******************************************************************************
**
** FS_CHGFILEPTR - Change current location in file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** long offset                          signed offset
** unsigned short type                  indicates seek type
**   values:
**     CFP_RELBEGIN     move pointer relative to begining of file
**     CFP_RELCUR       move pointer relative to current position in file
**     CFP_RELEND       move pointer relative to end of file
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_CHGFILEPTR(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                   long offset, unsigned short type, unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_CLOSE - Close an open file
**
** Parameters
** ----------
** unsigned short type                  indicates close type
**   values:
**     FS_CL_ORDINARY   this is not the final close of the file
**     FS_CL_FORPROC    this is the final close of the file for the process
**     FS_CL_FORSYS     this is the final close of the file for the whole system
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
**
******************************************************************************/

#pragma argsused
short int far pascal FS_CLOSE(unsigned short type, unsigned short IOflag,
                              struct sffsi far *psffsi, struct sffsd far *psffsd)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_COMMIT - Commit a file to disk
**
** Parameters
** ----------
** unsigned short type                  indicates commit type
**   values:
**     FS_COMMIT_ONE    commit this one file
**     FS_COMMIT_ALL    commit all files
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
**
******************************************************************************/

#pragma argsused
short int far pascal FS_COMMIT(unsigned short type, unsigned short IOflag,
                               struct sffsi far *psffsi, struct sffsd far *psffsd)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_COPY - Copy a file
**
** Parameters
** ----------
** unsigned short flag                  indicates flavor of call
**   values:
**     DCPY_EXISTING    if destination file exists, replace it
**     DCPY_APPEND      source file should be appended to destination file
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pSrc                       pointer to source filename
** unsigned short iSrcCurDirEnd         offset to the end of the current directory in pSrc
** char far *pDst                       pointer to destination filename
** unsigned short iDstCurDirEnd         offset to the end of the current directory in pDst
** unsigned short nameType              0x40 = destination is non 8.3 filename
**
******************************************************************************/

#pragma argsused
short int far pascal FS_COPY(unsigned short flag, struct cdfsi far *pcdfsi,
                             struct cdfsd far *pcdfsd, char far *pSrc,
                             unsigned short iSrcCurDirEnd, char far *pDst,
                             unsigned short iDstCurDirEnd, unsigned short nameType)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_DELETE - Delete a file
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pFile                      pointer to filename to delete
** unsigned short iCurDirEnd            offset to the end of the current directory in pFile
**
******************************************************************************/

#pragma argsused
short int far pascal FS_DELETE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                               char far *pFile, unsigned short iCurDirEnd)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FILEATTRIBUTE - Get/Set DOS file attributes
**
** Parameters
** ----------
** unsigned short flag                  indicates flavor of call
**   values:
**     FA_RETRIEVE      retrieve attribute
**     FA_SET           set attribute
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to filename
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
** unsigned short far *pAttr            pointer to the attribute
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FILEATTRIBUTE(unsigned short flag, struct cdfsi far *pcdfsi,
                                      struct cdfsd far *pcdfsd, char far *pName,
                                      unsigned short iCurDirEnd, unsigned short far *pAttr)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FILEINFO - Get/Set file information
**
** Parameters
** ----------
** unsigned short flag                  indicates flavor of call
**   values:
**     FI_RETRIEVE      retrieve information
**     FI_SET           set information
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned short level                 level of information to get/set
** char far *pData                      UNVERIFIED? pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FILEINFO(unsigned short flag, struct sffsi far *psffsi,
                                 struct sffsd far *psffsd, unsigned short level,
                                 char far *pData, unsigned short cbData,
                                 unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FILEIO - Atomic I/O operations
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** char far *pCmdList                   UNVERIFIED pointer to information area
** unsigned short cbCmdList             size of area pointed to by pCmdList
** unsigned short far *poError          UNVERIFIED pointer to offset within
**                                      pCmdList of command that caused an error
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FILEIO(struct sffsi far *psffsi, struct sffsd far *psffsd,
                               char far *pCmdList, unsigned short cbCmdList,
                               unsigned short far *poError, unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_MOVE - Move/rename a file
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pSrc                       pointer to source filename
** unsigned short iSrcCurDirEnd         offset to the end of the current directory in pSrc
** char far *pDst                       pointer to destination filename
** unsigned short iDstCurDirEnd         offset to the end of the current directory in pDst
** unsigned short flags                 0x40 = destination is non 8.3 filename
**
******************************************************************************/

#pragma argsused
short int far pascal FS_MOVE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                             char far *pSrc, unsigned short iSrcCurDirEnd,
                             char far *pDst, unsigned short iDstCurDirEnd,
                             unsigned short flags)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_NEWSIZE - Change size of file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned long len                    new length of file
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_NEWSIZE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                unsigned long len, unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_OPENCREATE - Open or create a new file
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to filename
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned long openmode               sharing and access mode
** unsigned short openflag              action to take when file exists/doesn't exist
** unsigned short far *pAction          returns the action that the IFS took
** unsigned short attr                  OS/2 file attributes
** char *pEABuf                         UNVERIFIED pointer to EAs to attach to new file
** unsigned short fat *pfgenFlag        flags returned by the IFS
**   values:
**     FOC_NEEDEAS      indicates there are critical EAs associated with the file
**
******************************************************************************/

#pragma argsused
short int far pascal FS_OPENCREATE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                                   char far *pName, unsigned short iCurDirEnd,
                                   struct sffsi far *psffsi, struct sffsd far *psffsd,
                                   unsigned long openmode, unsigned short openflag,
                                   unsigned short far *pAction, unsigned short attr,
                                   char far *pEABuf, unsigned short far *pfgenFlag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_PATHINFO - get/set file information by filename
**
** Parameters
** ----------
** unsigned short flag                  indicates flavor of call
**   values:
**     PI_RETRIEVE      retrieve information
**     PI_SET           set information
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to filename
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
** unsigned short level                 level of information to get/set
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
**
******************************************************************************/

#pragma argsused
short int far pascal FS_PATHINFO(unsigned short flag, struct cdfsi far *pcdfsi,
                                 struct cdfsd far *pcdfsd, char far *pName,
                                 unsigned short iCurDirEnd, unsigned short level,
                                 char far *pData, unsigned short cbData)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_READ - read data from a file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** char far *pData                      UNVERIFIED pointer to buffer
** unsigned short far *pLen             length of buffer
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_READ(struct sffsi far *psffsi, struct sffsd far *psffsd,
                             char far *pData, unsigned short far *pLen,
                             unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_WRITE - write data to a file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** char far *pData                      UNVERIFIED pointer to buffer
** unsigned short far *pLen             length of buffer
** unsigned short IOflag                bitfield of I/O suggestions
**   values:
**     IOFL_WRITETHRU   write all updated data before returning
**     IOFL_NOCACHE     don't cache any new data
**
******************************************************************************/

#pragma argsused
short int far pascal FS_WRITE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                              char far *pData, unsigned short far *pLen,
                              unsigned short IOflag)
{
   return ERROR_NOT_SUPPORTED;
}



/*-----------------------------------------------------------------------------
--
-- Directory management
--
-----------------------------------------------------------------------------*/



/******************************************************************************
**
** FS_FINDCLOSE - End a directory search
**
** Parameters
** ----------
** struct fsfsi far *pfsfsi             pointer to FSD independant search record
** struct fsfsd far *pfsfsd             pointer to FSD dependant search record
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDCLOSE(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDFIRST - Begin a new directory search
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to filename mask
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
** unsigned short attr                  attribute mask
** struct fsfsi far *pfsfsi             pointer to FSD independant search record
** struct fsfsd far *pfsfsd             pointer to FSD dependant search record
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short far *pcMatch          maximum number of entries to return*
**                                      number of entries actually returned
** unsigned short level                 level of information to return
** unsigned short flags                 indicates whether to return position information
**   values:
**     FF_NOPOS         don't return any position information
**     FF_GETPOS        return position information in buffer
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDFIRST(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                                  char far *pName, unsigned short iCurDirEnd,
                                  unsigned short attr, struct fsfsi far *pfsfsi,
                                  struct fsfsd far *pfsfsd, char far *pData,
                                  unsigned short cbData, unsigned short far *pcMatch,
                                  unsigned short level, unsigned short flags)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDFROMNAME - Restart directory search
**
** Parameters
** ----------
** struct fsfsi far *pfsfsi             pointer to FSD independant search record
** struct fsfsd far *pfsfsd             pointer to FSD dependant search record
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short far *pcMatch          maximum number of entries to return*
**                                      number of entries actually returned
** unsigned short level                 level of information to return
** unsigned long position               position in directory to restart search from
** char far *pName                      pointer to filename to restart search from
** unsigned short flags                 indicates whether to return position information
**   values:
**     FF_NOPOS         don't return any position information
**     FF_GETPOS        return position information in buffer
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDFROMNAME(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd,
                                     char far *pData, unsigned short cbData,
                                     unsigned short far *pcMatch, unsigned short level,
                                     unsigned long position, char far *pName,
                                     unsigned short flags)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDNEXT - Continue directory search
**
** Parameters
** ----------
** struct fsfsi far *pfsfsi             pointer to FSD independant search record
** struct fsfsd far *pfsfsd             pointer to FSD dependant search record
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short far *pcMatch          maximum number of entries to return*
**                                      number of entries actually returned
** unsigned short level                 level of information to return
** unsigned short flag                  indicates whether to return position information
**   values:
**     FF_NOPOS         don't return any position information
**     FF_GETPOS        return position information in buffer
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDNEXT(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd,
                                 char far *pData, unsigned short cbData, 
                                 unsigned short far *pcMatch, unsigned short level,
                                 unsigned short flag)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDNOTIFYCLOSE - End a directory update request
**
** Parameters
** ----------
** unsigned short handle                handle of update request to close
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDNOTIFYCLOSE(unsigned short handle)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDNOTIFYFIRST - Begin a new directory update request
**
** Parameters
** ----------
** struct cdfsi far *pcdfsi             pointer to FSD independant current directory
** struct cdfsd far *pcdfsd             pointer to FSD dependant current directory
** char far *pName                      pointer to filename mask
** unsigned short iCurDirEnd            offset to the end of the current directory in pName
** unsigned short attr                  attribute mask
** unsigned short far *pHandle          pointer to place where FSD stores its handle
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short far *pcMatch          maximum number of entries to return*
**                                      number of entries actually returned
** unsigned short level                 level of information to return
** unsigned long timeout                timeout in milliseconds
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDNOTIFYFIRST(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                                        char far *pName, unsigned short iCurDirEnd,
                                        unsigned short attr, unsigned short far *pHandle,
                                        char far *pData, unsigned short cbData,
                                        unsigned short far *pcMatch, unsigned short level,
                                        unsigned long timeout)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_FINDNOTIFYNEXT - Continue directory update request
**
** Parameters
** ----------
** unsigned short handle                directory update handle
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short cbData                size of area pointed to by pData
** unsigned short far *pcMatch          maximum number of entries to return*
**                                      number of entries actually returned
** unsigned short level                 level of information to return
** unsigned long timeout                timeout in milliseconds
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FINDNOTIFYNEXT(unsigned short handle, char far *pData,
                                       unsigned short cbData, unsigned short far *pcMatch,
                                       unsigned short level, unsigned long timeout)
{
   return ERROR_NOT_SUPPORTED;
}



/*-----------------------------------------------------------------------------
--
-- FSD Extended Interface
--
-----------------------------------------------------------------------------*/



/******************************************************************************
**
** FS_FSCTL - Extended IFS control
**
** Parameters
** ----------
** union argdat far *pArgdat
** unsigned short iArgType              argument type
**   values:
**     FSCTL_ARG_FILEINSTANCE   filehandle directed
**     FSCTL_ARG_CURDIR         pathname directed
**     FSCTL_ARG_NULL           FSD directed
** unsigned short func                  function code
**   values:
**     FSCTL_FUNC_NONE          NULL function
**     FSCTL_FUNC_NEW_INFO      return error code information
**     FSCTL_FUNC_EASIZE        return max EA size and max EA list size
** char far *pParm                      UNVERIFIED pointer to parameter area
** unsigned short lenParm               size of area pointed to by pParm
** unsigned short far *plenParmOut      length of parameters passed in pParm
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short lenData               size of area pointed to by pData
** unsigned short far *plenDataOut      length of parameters passed in pData
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FSCTL(union argdat far *pArgdat, unsigned short iArgType,
                              unsigned short func, char far *pParm, unsigned short lenParm,
                              unsigned short far *plenParmOut, char far *pData,
                              unsigned short lenData, unsigned short far *plenDataOut)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_IOCTL - Perform an IOCTL on the IFS
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned short cat                   function category
** unsigned short func                  function to perform
** char far *pParm                      UNVERIFIED pointer to parameter area
** unsigned short lenParm               size of area pointed to by pParm
** unsigned short far *pParmLenInOut    length of parameters passed in pParm
** char far *pData                      UNVERIFIED pointer to information area
** unsigned short lenData               size of area pointed to by pData
** unsigned short far *pDataLenInOut    length of parameters passed in pData
**
******************************************************************************/

#pragma argsused
short int far pascal FS_IOCTL(struct sffsi far *psffsi, struct sffsd far *psffsd,
                              unsigned short cat, unsigned short func,
                              char far *pParm, unsigned short lenParm,
                              unsigned far *pParmLenInOut, char far *pData,
                              unsigned short lenData, unsigned far *pDataLenInOut)
{
   return ERROR_NOT_SUPPORTED;
}



/*-----------------------------------------------------------------------------
--
-- Miscellaneous Functions
--
-----------------------------------------------------------------------------*/



/******************************************************************************
**
** FS_EXIT - Notify FSD that a process is ending
**
** Parameters
** ----------
** unsigned short uid                   user ID of process
** unsigned short pid                   process ID of process
** unsigned short pdb                   DOS mode process ID of process
**
******************************************************************************/

#pragma argsused
void far pascal FS_EXIT(unsigned short uid, unsigned short pid, unsigned short pdb)
{
}


/******************************************************************************
**
** FS_INIT - Initialize the IFS
**
** Parameters
** ----------
** char far *szParm                     pointer to command line parameters
** unsigned long pDevHlp                pointer to DevHlp entry point
** unsigned long far *pMiniFSD          pointer to data passed between the 
**                                      mini-FSD and the IFS
**
******************************************************************************/

#pragma argsused
short int far pascal FS_INIT(char far *szParm, unsigned long pDevHlp,
                             unsigned long far *pMiniFSD)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_NMPIPE - Perform a named pipe operation
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned short OpType                operation to perform
**   values:
**     NMP_GetPHandState
**     NMP_SetPHandState
**     NMP_PipeQInfo
**     NMP_PeekPipe
**     NMP_ConnectPipe
**     NMP_DisconnectPipe
**     NMP_TransactPipe
**     NMP_READRAW
**     NMP_WRITERAW
**     NMP_WAITPIPE
**     NMP_CALLPIPE
**     NMP_QNmPipeSemState
** union npoper far *pOpRec             data for operation
** char far *pData                      pointer to user data
** char far *pName                      pointer to remote named pipe
**
******************************************************************************/

#pragma argsused
short int far pascal FS_NMPIPE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                               unsigned short OpType, union npoper far *pOpRec,
                               char far *pData, char far *pName)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_PROCESSNAME - Canonicalize a filename
**
** Parameters
** ----------
** char far *pNameBuf                   filename to canonicalize
**
******************************************************************************/

#pragma argsused
short int far pascal FS_PROCESSNAME(char far *pNameBuf)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_SHUTDOWN - Prepare for a system shutdown
**
** Parameters
** ----------
** unsigned short usType                flavor of call
**   values:
**     SD_BEGIN                 begining the shutdown process
**     SD_COMPLETE              finishing the shutdown process
** unsigned long ulReserved             reserved
**
******************************************************************************/

#pragma argsused
short int far pascal FS_SHUTDOWN(unsigned short usType, unsigned long ulReserved)
{
   return ERROR_NOT_SUPPORTED;
}



/******************************************************************************
*******************************************************************************
**
** Swapper support entry points
**
*******************************************************************************
******************************************************************************/



/******************************************************************************
**
** FS_ALLOCATEPAGESPACE - Adjust size of the page file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned long ISize                  new size of page file
** unsigned long IWantContig            minimum continuity requirement
**
******************************************************************************/

#pragma argsused
short int far pascal FS_ALLOCATEPAGESPACE(struct sffsi far *psffsi,
                                          struct sffsd far *psffsd, 
                                          unsigned long lSize, 
                                          unsigned long lWantContig)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_DOPAGEIO - Perform I/O to the page file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** struct PageCmdHeader far *pPageCmdList operations to perform
**
******************************************************************************/

#pragma argsused
short int far pascal FS_DOPAGEIO(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                 struct PageCmdHeader far *pPageCmdList)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_OPENPAGEFILE - Open the paging file
**
** Parameters
** ----------
** unsigned long far *pFlag             flags
**     PGIO_FIRSTOPEN           first open of page file
**     PGIO_PADDR               physical addresses required
**     PGIO_VADDR               16:16 virtual address required
** unsigned long far *pcMaxReq          pointer to maximum request list size
** char far *pName                      pointer to filename
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** unsigned long OpenMode               sharing and access mode
** unsigned short OpenFlag              action to take when file exists/doesn't exist
** unsigned short Attr                  OS/2 file attributes
** unsigned long Reserved               reserved
**
******************************************************************************/

#pragma argsused
short int far pascal FS_OPENPAGEFILE(unsigned long far *pFlags,
                                     unsigned long far *pcMaxReq,
                                     char far *pName, struct sffsi far *psffsi,
                                     struct sffsd far *psffsd, unsigned short OpenMode,
                                     unsigned short OpenFlag, unsigned short Attr,
                                     unsigned long Reserved)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_SETSWAP - Inform IFS it will be used to manage the swap file
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
**
******************************************************************************/

#pragma argsused
short int far pascal FS_SETSWAP(struct sffsi far *psffsi, struct sffsd far *psffsd)
{
   return ERROR_NOT_SUPPORTED;
}



/******************************************************************************
*******************************************************************************
**
** Locking support entry points
**
*******************************************************************************
******************************************************************************/



/******************************************************************************
**
** FS_CANCELLOCKREQUEST - unlock a range
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** void far *pLockRange                 range to unlock
**
******************************************************************************/

#pragma argsused
short int far pascal FS_CANCELLOCKREQUEST(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                          void far *pLockRange)
{
   return ERROR_NOT_SUPPORTED;
}


/******************************************************************************
**
** FS_CANCELLOCKREQUEST - unlock a range
**
** Parameters
** ----------
** struct sffsi far *psffsi             pointer to FSD independant file instance
** struct sffsd far *psffsd             pointer to FSD dependant file instance
** void far *pUnLockRange               range to unlock
** void far *pLockRange                 range to lock
** unsigned long timeout                time in milliseconds to wait
** unsigned long flags                  flags
**   values:
**     0x01                     sharing of this file region is allowed
**     0x02                     atomic lock request
**
******************************************************************************/

#pragma argsused
short int far pascal FS_FILELOCKS(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                  void far *pUnLockRange, void far *pLockRange,
                                  unsigned long timeout, unsigned long flags)
{
   return ERROR_NOT_SUPPORTED;
}



/******************************************************************************
*******************************************************************************
**
** UNC entry point
**
*******************************************************************************
******************************************************************************/



/******************************************************************************
**
** FS_VERIFYUNCNAME - Check if the IFS controls the server in question
**
** Parameters
** ----------
** unsigned long far *pFlag             flags
**   values:
**     VUN_PASS1                pass 1 poll
**     VUN_PASS2                pass 2 poll
** char far *pName                      pointer to server in UNC format
**
******************************************************************************/

#pragma argsused
short int far pascal FS_VERIFYUNCNAME(unsigned short flag, char far *pName)
{
   return ERROR_NOT_SUPPORTED;
}
