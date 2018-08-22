/** @file
  Common EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataLib.h"

BOOLEAN                      mEepromDataLibDebugFlag = FALSE;
EEPROM_DATA_LIBRARY_INDEX    mEepromDataLibIndex[EEPROM_DATA_LIBRARY_INDEX_MAX];
CHAR8                       *mEepromLibraryString[EEPROM_DATA_LIBRARY_INDEX_MAX] = {
                              "EEPROM_NULL",
                              "EEPROM_EEPROM",
                              "EEPROM_FV",
                              "EEPROM_MEMORY"
                             };

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
  )
{
  EFI_STATUS   Status;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - LibraryIndex out of range\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Size == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Size == NULL\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Buffer == NULL\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (mEepromDataLibIndex[LibraryIndex].Active == FALSE) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Library %a is not active\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));
    Status = EFI_NOT_READY;
    goto Exit;
  }

  //
  // Call function
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Calling EEPROM raw library %a read function\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));
  Status = mEepromDataLibIndex[LibraryIndex].ReadFunction (Offset, Size, Buffer, FunctionInfo);

Exit:
  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   LibraryIndex   Determines which raw data library to use
//              Offset         Start copying from the offset
//                             - If set to 0, then return size of EEPROM binary
//              Size           Size of the buffer and the number of bytes to copy
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
  )
{
  EFI_STATUS   Status;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - LibraryIndex out of range\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Size == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Size == NULL\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Buffer == NULL\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (mEepromDataLibIndex[LibraryIndex].Active == FALSE) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Library %a is not active\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));
    Status = EFI_NOT_READY;
    goto Exit;
  }

  //
  // Call function
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Calling EEPROM raw library %a write function\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));
  Status = mEepromDataLibIndex[LibraryIndex].WriteFunction (Offset, Size, Buffer, FunctionInfo);

Exit:
  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

