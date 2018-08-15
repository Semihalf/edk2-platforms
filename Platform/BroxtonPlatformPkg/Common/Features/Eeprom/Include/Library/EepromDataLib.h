/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_DATA_LIB_
#define _EEPROM_DATA_LIB_
////
//// Header files
////
#include <Uefi.h>


////
//// Defines
////

////
//// Enums
////
typedef enum {
  EEPROM_NULL                   = 0,
  EEPROM_EEPROM                 = 1,
  EEPROM_FV                     = 2,
  EEPROM_MEMORY                 = 3,
  EEPROM_DATA_LIBRARY_INDEX_MAX = 4
} EEPROM_DATA_LIBRARY_TYPE;


////
//// Externs
////
extern BOOLEAN    mEepromDataLibDebugFlag;
extern CHAR8     *mEepromLibraryString[EEPROM_DATA_LIBRARY_INDEX_MAX];


////
//// Structures
////
typedef struct {
  UINT8                   LibraryIndex;
  UINT8                   Address;
  UINT8                   Bus;
  UINT32                  Size;         // NOTE: Set this to 0x00.
  UINT8                  *Buffer;       // NOTE: Set this to NULL.
  UINT8                  *ScanBuffer;   // NOTE: Should be NULL unless requesting a scan on that I2C bus
  UINT8                  *ScanSize;     // NOTE: Should be NULL unless requesting a scan on that I2C bus
} EEPROM_FUNCTION_INFO;

//
// Should only use this if you need to get a different file from the FV.
//
typedef struct {
  UINT8                   LibraryIndex;
  EFI_GUID               *FvFileGuid;
} FV_FUNCTION_INFO;


////
//// Functions
////
UINTN
EFIAPI
DisplayStackPointer (
  IN   CHAR8    *Function,
  IN   UINTN     LineNumber
  );

//
// Desc:        Copies the contents of an existing memory pool into a new memory pool of equal or greater size.
// Variables:   Size           Size of the pool to copy existing pool into
//              SourcePointer  Pointer to the source buffer to copy
// Return:      Pointer        Pointer to your copy of the pool
//
VOID*
EFIAPI
EepromAllocateCopyPool (
  IN  UINTN    Size,
  IN  VOID    *SourcePointer
  );

//
// Desc:        Creates a new memory pool.
// Variables:   Size           Size of the pool requested
// Return:      Pointer        Pointer the new pool
//
VOID*
EFIAPI
EepromAllocatePool (
  IN  UINTN   Size
  );

EFI_STATUS
EFIAPI
EepromDataLibNemToMemory (VOID);

//
// Desc:        Frees a memory pool.
// Variables:   Pointer        Pointer to the beginning of the pool to be freed
// Return:      Pointer        NULL
//
VOID*
EFIAPI
EepromFreePool (
  IN  VOID  *Pointer
  );

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   LibraryIndex   Determines which raw data library to use
//              Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//                             - If set to 0, then return size of EEPROM binary
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary library available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEeprom (
  IN       UINT8     LibraryIndex,
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   LibraryIndex   Determines which raw data library to use
//              Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//                             - If set to 0, then return size of EEPROM binary
//              Buffer         Data to be copied to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary library available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEeprom (
  IN       UINT8     LibraryIndex,
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

#endif // _EEPROM_DATA_LIB_

