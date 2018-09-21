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
#include <PiPei.h>
#include <Library/HobLib.h>

EFI_STATUS
EFIAPI
GetEepromDataHobData (VOID)
{
  //
  // Not supported in PEI.
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
SetEepromDataHobData (VOID)
{
  UINT8                        *BlockData;
  UINT16                        BlockNumber;
  UINT16                        BlockSize;
  UINT8                        *ImageData;
  UINT32                        ImageSize;
  MEMORY_HOB_HEADER            *MemoryHobHeader;
  UINT32                        Offset;
  EFI_STATUS                    Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  BlockData = NULL;
  ImageData = (UINT8 *) (UINTN) PcdGet64 (PcdEepromMemoryPointer);
  ImageSize = PcdGet32 (PcdEepromMemorySize);
  Status    = EFI_SUCCESS;

  if (!PcdGetBool (PcdEepromMemoryHobPresent) && (ImageData != NULL) && (ImageSize != 0)) {
    //
    // No HOB yet. Create one.
    //
    BlockNumber = 0;
    Offset      = 0;
    while (ImageSize > 0) {
      //
      // Adjust block size
      //
      if (ImageSize > MEMORY_HOB_MAX_DATA_SIZE) {
        BlockSize = MEMORY_HOB_MAX_DATA_SIZE;
      } else {
        BlockSize = (UINT16) ImageSize;
      }

      //
      // Create Block data buffer
      //
      BlockData = EepromFreePool (BlockData);
      BlockData = EepromAllocatePool (BlockSize + sizeof (MEMORY_HOB_HEADER));
      if (BlockData == NULL) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Unable to allocate buffer!\n", __FUNCTION__, __LINE__));
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }

      //
      // Setup header
      //
      MemoryHobHeader = (MEMORY_HOB_HEADER *) BlockData;
      MemoryHobHeader->Signature   = MEMORY_HOB_HEADER_SIGNATURE;
      MemoryHobHeader->Version     = MEMORY_HOB_HEADER_VERSION;
      MemoryHobHeader->BlockNumber = BlockNumber;
      MemoryHobHeader->BlockSize   = BlockSize;
      MemoryHobHeader->Reserved    = 0x00000000;

      //
      // Copy data into buffer
      //
      CopyMem (
        (BlockData + sizeof (MEMORY_HOB_HEADER)),
        (ImageData + Offset),
        (BlockSize + sizeof (MEMORY_HOB_HEADER))
        );

      //
      // Create GUID HOB
      //
      BuildGuidDataHob (&gEepromVariableGuid, BlockData, BlockSize);

      //
      // Adjust pointers and such
      //
      Offset      += BlockSize;
      BlockNumber += 1;
      ImageSize   -= BlockSize;
    }

    //
    // Toggle flag.
    //
    PcdSetBool (PcdEepromMemoryHobPresent, TRUE);
  }

Exit:
  BlockData = EepromFreePool (BlockData);
  return Status;
}

