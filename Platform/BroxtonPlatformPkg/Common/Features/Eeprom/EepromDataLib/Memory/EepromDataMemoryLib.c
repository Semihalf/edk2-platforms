/** @file
  Memory EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataMemoryLib.h"

//
// Desc:        Reads from the EEPROM copy in memory and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_BUFFER_TOO_SMALL    Passed in buffer is too small to hold the data
//                                      requested
//
EFI_STATUS
EFIAPI
ReadEepromMemory (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  UINT8         *Address;
  UINT8         *EepromMemoryPtr;
  UINT32         EepromMemorySize;
  EFI_STATUS     Status;

  //
  // Try to load data from HOB
  //
  GetEepromDataHobData ();

  //
  // Initialize variables
  //
  Address          = NULL;
  EepromMemoryPtr  = (UINT8 *) (UINTN) PcdGet64 (PcdEepromMemoryPointer);
  EepromMemorySize = PcdGet32 (PcdEepromMemorySize);
  Status           = EFI_SUCCESS;

  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //

  if ((EepromMemoryPtr == NULL) || (EepromMemorySize == 0)) {
    //
    // Memory pointer not loaded yet.
    //
    Status = EFI_SUCCESS;
    *Size  = 0;
    goto Exit;
  }

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EepromMemoryPtr  = 0x%08x\n", __FUNCTION__, __LINE__, PcdGet64 (PcdEepromMemoryPointer)));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EepromMemorySize = 0x%08x\n", __FUNCTION__, __LINE__, PcdGet32 (PcdEepromMemorySize)));
  Address = EepromMemoryPtr;
  if (*Size == 0) {
    //
    // Nothing to copy, return EEPROM size.
    //
    *Size = EepromMemorySize;
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM size = 0x%08x\n", __FUNCTION__, __LINE__, *Size));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Exit;
  } else {
    //
    // Figure out the correct buffer size.
    //
    if ((*Size + Offset) > EepromMemorySize) {
      //
      // Buffer is larger than what is left in the FV. Update the passed in size.
      //
      if (EepromMemorySize < Offset) {
        //
        // Offset is larger than the FV size. Return 0.
        //
        *Size = 0;
      } else {
        if ((EepromMemorySize - Offset) > *Size) {
          //
          // Buffer isn't large enough. Bail.
          //
          *Size = EepromMemorySize - Offset;
          Status = EFI_BUFFER_TOO_SMALL;
          goto Exit;
        } else {
          *Size = EepromMemorySize - Offset;
        }
      }
    }
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Copying 0x%08x bytes of data...\n", __FUNCTION__, __LINE__, *Size));
    if (*Size > 0) {
      //
      // Asking to copy something
      //
      Address = Address + Offset;
      CopyMem ((VOID *) Buffer, (VOID *) Address, *Size);
    }
  }

Exit:
  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
// Desc:        Writes to the EEPROM memory copy and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Data to be copied to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_BUFFER_TOO_SMALL    Passed in buffer is too small to hold the data
//                                      requested
//              EFI_OUT_OF_RESOURCES    Unable to allocate memory
//
EFI_STATUS
EFIAPI
WriteEepromMemory (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  UINT8         *EepromMemoryPtr;
  UINT32         EepromMemorySize;
  EFI_STATUS     Status;
  UINT8         *TempBuffer;
  UINT32         TempSize;

  //
  // Initialize variables
  //
  EepromMemoryPtr  = (UINT8 *) (UINTN) PcdGet64 (PcdEepromMemoryPointer);
  EepromMemorySize = PcdGet32 (PcdEepromMemorySize);
  Status           = EFI_SUCCESS;
  TempBuffer       = NULL;
  TempSize         = 0;
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EepromMemoryPtr  = 0x%08x\n", __FUNCTION__, __LINE__, EepromMemoryPtr));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EepromMemorySize = 0x%08x\n", __FUNCTION__, __LINE__, EepromMemorySize));

  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //

  if (*Size == 0) {
    //
    // Return current EEPROM size.
    //
    *Size = EepromMemorySize;
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM size = 0x%08x\n", __FUNCTION__, __LINE__, *Size));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Exit;
  } else {
    //
    // Asking to write data into memory EEPROM buffer.
    //
    if ((Offset + *Size) > EepromMemorySize) {
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Incoming buffer is larger than existing buffer.\n", __FUNCTION__, __LINE__));
      //
      // Current memory buffer isn't large enough. Save current buffer/size off.
      //
      TempBuffer = EepromMemoryPtr;
      TempSize   = EepromMemorySize;
      //
      // Clear memory buffer/size.
      //
      EepromMemoryPtr  = NULL;
      EepromMemorySize = 0;
    }
    if (EepromMemorySize == 0) {
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Memory buffer is NULL, allocating pool.\n", __FUNCTION__, __LINE__));
      //
      // Need to allocate buffer.
      //
      EepromMemoryPtr = EepromAllocatePool (*Size + Offset);
      if (EepromMemoryPtr == NULL) {
        //
        // Failed to allocate space.
        //
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      EepromMemorySize = *Size + Offset;
    }
    //
    // If there is a temp buffer, copy that content over first.
    //
    if ((TempBuffer != NULL) && (TempSize > 0)) {
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Copying temp buffer to memory buffer.\n", __FUNCTION__, __LINE__));
      CopyMem ((VOID *) EepromMemoryPtr, (VOID *) TempBuffer, TempSize);
      TempBuffer = EepromFreePool (TempBuffer);
    }
    //
    // Copy input buffer to memory buffer.
    //
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Found EEPROM image @ %08x.\n", __FUNCTION__, __LINE__, EepromMemoryPtr));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Copying 0x%08x bytes of data, starting at offset 0x%08x...\n", __FUNCTION__, __LINE__, *Size, Offset));
    CopyMem ((VOID *) (EepromMemoryPtr + Offset), (VOID *) Buffer, *Size);
  }

Exit:
  if (Status == EFI_SUCCESS) {
    //
    // Save off buffer info
    //
    PcdSet64 (PcdEepromMemoryPointer, (UINTN) EepromMemoryPtr);
    PcdSet32 (PcdEepromMemorySize,    EepromMemorySize);

    //
    // Update HOB
    //
    SetEepromDataHobData ();
  }

  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

