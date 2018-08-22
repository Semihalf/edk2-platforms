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

#include "EepromDataEepromLib.h"
#include <PiDxe.h>
#include <Library/HobLib.h>

EFI_STATUS
EFIAPI
GetEepromDataHobDataEeprom (VOID)
{
  UINT8                        *BlockData;
  BOOLEAN                       BlockFound;
  UINT16                        BlockNumber;
  UINT32                        BlockSize;
  UINT8                        *Buffer;
  EFI_HOB_GUID_TYPE            *GuidHobPtr;
  UINT16                        LastBlockNumber;
  EEPROM_MEMORY_HOB_HEADER     *MemoryHobHeader;
  EFI_STATUS                    Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  Buffer = NULL;
  Status = EFI_SUCCESS;

  if (PcdGetBool (PcdEepromMemoryHobPresent)) {
    //
    // Clear PCD values
    //
    PcdSet64 (PcdEepromMemoryPointer, 0);
    PcdSet32 (PcdEepromMemorySize,    0);

    //
    // Gather HOB data into coherent chunk by looping thru the HOBs looking for the blocks in order.
    //
    LastBlockNumber = 0;
    while (TRUE) {
      //
      // Reset to the beginning of the HOBs
      //
      BlockFound = FALSE;
      GuidHobPtr = GetFirstGuidHob (&gEepromVariableGuid);
      while (GuidHobPtr != NULL) {
        //
        // Get Memory HOB header pointer
        //
        MemoryHobHeader = GET_GUID_HOB_DATA (GuidHobPtr);
        BlockData       = (UINT8 *) MemoryHobHeader + sizeof (EEPROM_MEMORY_HOB_HEADER);
        BlockNumber     = MemoryHobHeader->BlockNumber;
        BlockSize       = MemoryHobHeader->BlockSize;

        //
        // Is this our block?
        //
        if ((LastBlockNumber == BlockNumber) && (MemoryHobHeader->Signature == EEPROM_HOB_HEADER_SIGNATURE)) {
          //
          // Yep. Copy existing buffer to larger buffer.
          //
          Buffer = EepromAllocateCopyPool (PcdGet32 (PcdEepromMemorySize) + BlockSize, (UINT8 *) (UINTN) PcdGet64 (PcdEepromMemoryPointer));
          if (Buffer == NULL) {
            DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Unable to allocate buffer!\n", __FUNCTION__, __LINE__));
            Status = EFI_OUT_OF_RESOURCES;
            goto Exit;
          }

          //
          // Append new data
          //
          CopyMem (Buffer + PcdGet32 (PcdEepromMemorySize), BlockData, BlockSize);

          //
          // Free old buffer and update PCDs
          //
          if (PcdGet64 (PcdEepromMemoryPointer) != 0) {
            //
            // Buffer exists. Free it.
            //
            EepromFreePool ((UINT8 *) (UINTN) PcdGet64 (PcdEepromMemoryPointer));
          }
          BlockSize += PcdGet32 (PcdEepromMemorySize);
          PcdSet64 (PcdEepromMemoryPointer, (UINTN) Buffer);
          PcdSet32 (PcdEepromMemorySize,    BlockSize);

          //
          // Increment counter and set flag
          //
          LastBlockNumber++;
          BlockFound = TRUE;
          break;
        }

        //
        // Skip past present HOB to get next HOB
        //
        GuidHobPtr = GET_NEXT_HOB (GuidHobPtr);
        GuidHobPtr = GetNextGuidHob (&gEepromVariableGuid, GuidHobPtr);
      }
      if (!BlockFound) {
        //
        // We're done. Bail.
        //
        break;
      }
    }

    //
    // Toggle flag.
    //
    PcdSetBool (PcdEepromMemoryHobPresent, FALSE);
  }

Exit:
  return Status;
}

EFI_STATUS
EFIAPI
SetEepromDataHobDataEeprom (VOID)
{
  //
  // Not supported in DXE.
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_UNSUPPORTED;
}

