/** @file
  FV EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataFvLib.h"

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//                             - If set to 0, then return size of EEPROM binary
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromFv (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  VOID               *Address;
  FV_FUNCTION_INFO   *FvInfo;
  EFI_GUID            FvSectionGuid;
  UINTN               FvSize;
  VOID               *Ptr;
  EFI_STATUS          Status;

  //
  // Initialize variables
  //
  CopyMem (&FvSectionGuid, PcdGetPtr (PcdEepromFvImageFile), sizeof (EFI_GUID));
  Address = NULL;
  FvInfo  = (FV_FUNCTION_INFO *) FunctionInfo;
  FvSize  = 0;

  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //
  if (FvInfo != NULL) {
    if ((FvInfo->LibraryIndex == EEPROM_FV) & (FvInfo->FvFileGuid != NULL)) {
      //
      // Looking for a different FV file than the EEPROM image
      //
      CopyMem (&FvSectionGuid, FvInfo->FvFileGuid, sizeof (EFI_GUID));
    }
  }

  //
  // Grab file from FV
  //
  Status = GetImage (
             &FvSectionGuid,
             &Address,
             &FvSize
             );

  if (!EFI_ERROR (Status)) {
    //
    // EEPROM image is present in FV
    //
    if (Address != NULL) {
      Ptr = Address;
      DEBUG ((DEBUG_INFO, "%a (#%4d) - Found EEPROM image @ 0x%08x.\n", __FUNCTION__, __LINE__, Address));
      if (*Size == 0) {
        //
        // Nothing to copy, return FV EEPROM size.
        //
        *Size = (UINT32) FvSize;
        Status = EFI_BUFFER_TOO_SMALL;
        DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM size = 0x%08x\n", __FUNCTION__, __LINE__, *Size));
      } else {
        //
        // Figure out the correct buffer size.
        //
        if ((*Size + Offset) > FvSize) {
          //
          // Buffer is larger than what is left in the FV. Update the passed in size.
          //
          if (FvSize < Offset) {
            //
            // Offset is larger than the FV size. Return 0.
            //
            *Size = 0;
          } else {
            if (((UINT32) FvSize - Offset) > *Size) {
              //
              // Buffer isn't large enough. Bail.
              //
              *Size = (UINT32) FvSize - Offset;
              Status = EFI_BUFFER_TOO_SMALL;
              goto Exit;
            } else {
              *Size = (UINT32) FvSize - Offset;
            }
          }
        }
        DEBUG ((DEBUG_INFO, "%a (#%4d) - Copying 0x%08x bytes of data starting at offset 0x%08x...\n", __FUNCTION__, __LINE__, *Size, Offset));
        if (*Size > 0) {
          //
          // Asking to copy something
          //
          Address = (VOID *) ((UINT8 *) Address + Offset);
          CopyMem ((VOID *) Buffer, Address, *Size);
        }
      }
      //
      // GetImage () allocated buffer for Address, now clear it
      //
      FreePool (Ptr);
      Ptr = NULL;
    }
  }

Exit:
  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//                             - If set to 0, then return size of EEPROM binary
//              Buffer         Data to be copied to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromFv (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //

  return EFI_UNSUPPORTED;
}

