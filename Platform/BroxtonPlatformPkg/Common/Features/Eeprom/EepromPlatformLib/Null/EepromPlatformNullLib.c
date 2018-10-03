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
EepromGetBoardInfo (
  IN OUT   UINT32              *StructureIndex,
  OUT      BOARD_INFO_TABLE   **BoardInfo
  )
{
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
}

/**
  Checks whether the PadOffset is in the platform GPIO whitelist.

  @param[in]  PadOffset    The PAD offset to check against the platform whitelist

  @retval     TRUE         PAD offset is in the whitelist
  @retval     FALSE        PAD offset is not in the whitelist
**/
BOOLEAN
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
EepromProgramGpioPads (VOID)
{
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
}

/**
  Reset the system using the platform desired reset method.

  @param[in]  ResetType    The reset type to perform

  @retval     None
**/
VOID
EFIAPI
EepromResetSystem (
  IN EFI_RESET_TYPE   ResetType
  )
{
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return;
}

