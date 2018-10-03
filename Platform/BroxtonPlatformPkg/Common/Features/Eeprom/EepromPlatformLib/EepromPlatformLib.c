/** @file
  Common EEPROM library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromPlatformLib.h"

BOOLEAN      mEepromPlatformLibDebugFlag = FALSE;

/**
  Returns the $BrdInfo structure

  @param[out]  StructureIndex  Index of where to start looking for the next structure
  @param[out]  BoardInfo       Buffer containing the BOARD_INFO_TABLE structure
                               - Up to the caller to free the buffer

  @retval      EFI_SUCCESS     $BrdInfo structure found
  @retval      EFI_NOT_FOUND   $BrdInfo structure not found
  @retval      EFI_NOT_READY   $BrdInfo structure not ready yet
**/
EFI_STATUS
EFIAPI
EepromGetBoardInfo (
  IN OUT   UINT32              *StructureIndex,
  OUT      BOARD_INFO_TABLE   **BoardInfo
  )
{
  CHAR8               AsciiData[32];
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if (BoardInfo == NULL) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    //
    // Return the $BrdInfo structure
    //
    Status = GetEepromStructureData (NULL, EEPROM_BOARD_INFO_SIGNATURE, StructureIndex, sizeof (BOARD_INFO_TABLE), (UINT8**) BoardInfo, NULL, NULL);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $BrdInfo structure in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      if (mEepromPlatformLibDebugFlag) {
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->manuname, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Manufacturer = %a\n",     __FUNCTION__, __LINE__, AsciiData));
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->brdname, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board name   = %a\n",     __FUNCTION__, __LINE__, AsciiData));
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->brdserial, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Serial #     = %a\n",     __FUNCTION__, __LINE__, AsciiData));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board ID     = 0x%08x\n", __FUNCTION__, __LINE__, (*BoardInfo)->boardid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Fab ID       = 0x%08x\n", __FUNCTION__, __LINE__, (*BoardInfo)->fabid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - EC ID        = 0x%08x\n", __FUNCTION__, __LINE__, (*BoardInfo)->ecid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board type   = %a\n",     __FUNCTION__, __LINE__, (*BoardInfo)->boardtype ? "Main board (RedBox)" : "Plug-in board"));
      }
    }
  }
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $HdCodec data

  @param[out]  StructureIndex  Index of where to start looking for the next structure
  @param[out]  Buffer          Buffer containing the $HdCodec data
                               - Up to the caller to free the buffer
  @param[out]  BufferSize      Size of the HdaData buffer

  @retval      EFI_SUCCESS     $HdCodec data found
  @retval      EFI_NOT_FOUND   $HdCodec data not found
  @retval      EFI_NOT_READY   $HdCodec data not ready yet
**/
EFI_STATUS
EFIAPI
EepromGetHdaCodec (
  IN OUT   UINT32   *StructureIndex,
  OUT      UINT8   **Buffer,
  OUT      UINT32   *BufferSize
  )
{
  HDA_CODEC          *HdaStructure;
  UINT8              *HdaData;
  UINT32              HdaSize;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = GetEepromStructureData (NULL, EEPROM_HDA_CODEC_SIGNATURE, StructureIndex, sizeof (HDA_CODEC), (UINT8**) &HdaStructure, &HdaData, &HdaSize);
    if (!EFI_ERROR (Status)) {
      if (HdaSize != 0) {
        *Buffer = AllocatePool (HdaSize);
        if (*Buffer == NULL) {
          HdaSize = 0;
        } else {
          CopyMem (*Buffer, HdaData, HdaSize);
          EepromFreePool (HdaStructure);
        }
      }
      *BufferSize = HdaSize;
    }
  }

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $Logo$ data

  @param[out]  StructureIndex  Index of where to start looking for the next structure
  @param[out]  Buffer          Buffer containing the $Logo$ data
                               - Up to the caller to free the buffer
  @param[out]  BufferSize      Size of the LogoData buffer

  @retval      EFI_SUCCESS     $Logo$ data found
  @retval      EFI_NOT_FOUND   $Logo$ data not found
  @retval      EFI_NOT_READY   $Logo$ data not ready yet
**/
EFI_STATUS
EFIAPI
EepromGetLogo (
  IN OUT   UINT32   *StructureIndex,
  OUT      UINT8   **Buffer,
  OUT      UINT32   *BufferSize
  )
{
  UINT8              *LogoData;
  UINT32              LogoSize;
  LOGO_DATA          *LogoStructure;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = GetEepromStructureData (NULL, EEPROM_LOGO_DATA_SIGNATURE, StructureIndex, sizeof (LOGO_DATA), (UINT8**) &LogoStructure, &LogoData, &LogoSize);
    if (!EFI_ERROR (Status)) {
      if (LogoSize != 0) {
        *Buffer = AllocatePool (LogoSize);
        if (*Buffer == NULL) {
          LogoSize = 0;
        } else {
          CopyMem (*Buffer, LogoData, LogoSize);
          EepromFreePool (LogoStructure);
        }
      }
      *BufferSize = LogoSize;
    }
  }

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $MemCnfg data

  @param[out]  StructureIndex  Index of where to start looking for the next structure
  @param[out]  SpdSlotFlag     Flags of which SPD this data is valid for
  @param[out]  Buffer          Buffer containing the $MemCnfg data
                               - Up to the caller to free the buffer
  @param[out]  BufferSize      Size of the MemoryData buffer

  @retval      EFI_SUCCESS     $MemCnfg data found
  @retval      EFI_NOT_FOUND   $MemCnfg data not found
  @retval      EFI_NOT_READY   $MemCnfg data not ready yet
**/
EFI_STATUS
EFIAPI
EepromGetMemoryData (
  IN OUT   UINT32   *StructureIndex,
  OUT      UINT16   *SpdSlotFlag,
  OUT      UINT8   **Buffer,
  OUT      UINT32   *BufferSize
  )
{
  UINT8              *MemoryData;
  UINT32              MemorySize;
  MEMORY_DATA        *MemoryStructure;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if ((SpdSlotFlag == NULL) || (Buffer == NULL) || (BufferSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = GetEepromStructureData (NULL, EEPROM_MEMORY_DATA_SIGNATURE, StructureIndex, sizeof (MEMORY_DATA), (UINT8**) &MemoryStructure, &MemoryData, &MemorySize);
    if (!EFI_ERROR (Status)) {
      *SpdSlotFlag = MemoryStructure->spdslot;
      if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - SpdSlots     = %04x\n", __FUNCTION__, __LINE__, MemoryStructure->spdslot));
      if (MemorySize != 0) {
        *Buffer = AllocatePool (MemorySize);
        if (*Buffer == NULL) {
          MemorySize = 0;
        } else {
          CopyMem (*Buffer, MemoryData, MemorySize);
          EepromFreePool (MemoryStructure);
        }
      }
      *BufferSize = MemorySize;
    }
  }

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $Video$ data

  @param[out]  StructureIndex  Index of where to start looking for the next structure
  @param[out]  Buffer          Buffer containing the $Video$ data
                               - Up to the caller to free the buffer
  @param[out]  BufferSize      Size of the VideoData buffer

  @retval      EFI_SUCCESS     $Video$ data found
  @retval      EFI_NOT_FOUND   $Video$ data not found
  @retval      EFI_NOT_READY   $Video$ data not ready yet
**/
EFI_STATUS
EFIAPI
EepromGetVbt (
  IN OUT   UINT32   *StructureIndex,
  OUT      UINT8   **Buffer,
  OUT      UINT32   *BufferSize
  )
{
  EFI_STATUS          Status;
  UINT8              *VideoData;
  UINT32              VideoSize;
  VIDEO_DATA         *VideoStructure;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = GetEepromStructureData (NULL, EEPROM_VIDEO_DATA_SIGNATURE, StructureIndex, sizeof (VIDEO_DATA), (UINT8**) &VideoStructure, &VideoData, &VideoSize);
    if (!EFI_ERROR (Status)) {
      if (VideoSize != 0) {
        *Buffer = AllocatePool (VideoSize);
        if (*Buffer == NULL) {
          VideoSize = 0;
        } else {
          CopyMem (*Buffer, VideoData, VideoSize);
          EepromFreePool (VideoStructure);
        }
      }
      *BufferSize = VideoSize;
    }
  }

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Checks whether the PadOffset is in the platform GPIO whitelist.

  @param[in]  PadOffset    The PAD offset to check against the platform whitelist

  @retval     TRUE         PAD offset is in the whitelist
  @retval     FALSE        PAD offset is not in the whitelist
**/
BOOLEAN
EFIAPI
EepromPadCheck (
  IN  UINT32    PadOffset
  )
{
  UINT32     AlignedPadOffset;
  UINTN      GpioCount;
  UINT32    *GpioWhiteListPtr;
  UINTN      GpioWhiteListLength;
  BOOLEAN    ReturnValue;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  GpioCount           = 0;
  GpioWhiteListPtr    = (UINT32 *) PcdGetPtr (PcdGpioWhiteList);
  GpioWhiteListLength = (UINTN) PcdGetSize (PcdGpioWhiteList) / sizeof (UINT32);
  ReturnValue         = FALSE;

  //
  // Force to PAD offset so the whitelist only needs to account for the DW0 value and not both DW0 and DW1
  //
  AlignedPadOffset = PadOffset - (PadOffset % 0x08);

  //
  // Sanity checks
  //
  if (GpioWhiteListPtr == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() - ERROR: GpioWhiteListPtr == NULL\n", __FUNCTION__));
    goto Exit;
  }
  if (GpioWhiteListLength == 0) {
    DEBUG ((DEBUG_ERROR, "%a() - ERROR: GpioWhiteListLength == 0\n", __FUNCTION__));
    goto Exit;
  }
  if (AlignedPadOffset == 0) {
    DEBUG ((DEBUG_ERROR, "%a() - ERROR: AlignedPadOffset == 0\n", __FUNCTION__));
    goto Exit;
  }

  //
  // Verify PAD offset is in white list
  //
  while ((GpioWhiteListPtr[GpioCount] != END_OF_GPIO_ARRAY) && (GpioCount < GpioWhiteListLength)) {
    if (GpioWhiteListPtr[GpioCount] == AlignedPadOffset) {
      ReturnValue = TRUE;
      break;
    }
    GpioCount++;
  }

  if (ReturnValue == FALSE) {
    DEBUG ((DEBUG_INFO, "%a() - WARNING: 0x%08x [0x%08x] was not found in the whitelist.\n", __FUNCTION__, AlignedPadOffset, PadOffset));
  }

Exit:
  return ReturnValue;
}

/**
  Program GPIOs per binary and whitelist. The $GpioDat structure could be used to program things other than
  GPIOs, but that is all it is used for at this point.

  @param[in]  VOID

  @retval     EFI_SUCCESS     GPIOs programmed successfully
  @retval     EFI_NOT_FOUND   GPIO data not found
  @retval     EFI_NOT_READY   GPIO data not ready yet
**/
EFI_STATUS
EFIAPI
EepromProgramGpioPads (VOID)
{
  CHAR8               AsciiData[32];
  GPIO_DATA_RECORD   *EepromGpioData;
  GPIO_DATA_HEADER   *EepromGpioHeader;
  BOOLEAN             GpioDataFound;
  UINT32              GpioPadData;
  UINT32              Index;
  UINT32              OldGpioPadData;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Program any EEPROM defined GPIOs
  // 1. Find GPIO data in EEPROM binary
  // 2. Loop thru GPIO data and program PADs if PAD is in whitelist
  //
  GpioDataFound = FALSE;
  Index         = 0;
  Status        = EFI_SUCCESS;
  while (!EFI_ERROR (Status)) {
    Status = GetEepromStructureData (NULL, EEPROM_GPIO_SIGNATURE, &Index, sizeof (GPIO_DATA_HEADER), (UINT8**) &EepromGpioHeader, (UINT8**) &EepromGpioData, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      if (!GpioDataFound) DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find any GPIO data in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      //
      // Loop thru GPIO records. We currently only allow 32-bit PAD offset data records that are in the white-list.
      //
      while ((UINT8 *) EepromGpioData < ((UINT8 *) EepromGpioHeader + Size)) {
        OldGpioPadData = GpioPadRead (EepromGpioData->offset);
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, EepromGpioData->gpiolabel, 16);
        if (EepromPadCheck (EepromGpioData->offset)                   &&
            (EepromGpioData->datatype == EEPROM_GPIO_TYPE_PAD_OFFSET) &&
            (EepromGpioData->datasize == sizeof (UINT32))) {
          GpioPadData  = OldGpioPadData;
          GpioPadData &= EepromGpioData->anddata;
          GpioPadData |= EepromGpioData->ordata;
          GpioPadWrite (EepromGpioData->offset, GpioPadData);
        } else {
          DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: GPIO PAD offset not in the whitelist, wrong GPIO Type, or wrong data size!\n", __FUNCTION__, __LINE__));
        }
        if (OldGpioPadData != GpioPadRead (EepromGpioData->offset)) {
          DEBUG ((DEBUG_INFO, "%a(#%4d) - WARNING: PAD programming changing due to EEPROM binary!\n", __FUNCTION__, __LINE__));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -   Label      = %a\n",     __FUNCTION__, __LINE__, AsciiData));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     Offset   = 0x%08x\n", __FUNCTION__, __LINE__, EepromGpioData->offset));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     DataType = 0x%08x\n", __FUNCTION__, __LINE__, EepromGpioData->datatype));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     DataSize = 0x%08x\n", __FUNCTION__, __LINE__, EepromGpioData->datasize));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     AndData  = 0x%08x\n", __FUNCTION__, __LINE__, EepromGpioData->anddata));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     OrData   = 0x%08x\n", __FUNCTION__, __LINE__, EepromGpioData->ordata));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     OldData  = 0x%08x\n", __FUNCTION__, __LINE__, OldGpioPadData));
          DEBUG ((DEBUG_INFO, "%a(#%4d) -     NewData  = 0x%08x\n", __FUNCTION__, __LINE__, GpioPadRead (EepromGpioData->offset)));
          DEBUG ((DEBUG_INFO, "\n"));
        }
        EepromGpioData++;
      }
      GpioDataFound = TRUE;
    }
    EepromFreePool (EepromGpioHeader);
  }
  if (GpioDataFound) Status = EFI_SUCCESS;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

