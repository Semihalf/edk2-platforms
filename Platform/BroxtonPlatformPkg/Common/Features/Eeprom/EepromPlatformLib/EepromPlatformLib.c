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

BOOLEAN      mEepromPlatformLibDebugFlag = TRUE;

/**
  Returns the $BrdInfo structure

  @param[out]  BoardInfo       Buffer containing the BOARD_INFO_TABLE structure
                               - Up to the caller to free the buffer

  @retval      EFI_SUCCESS     $BrdInfo structure found
  @retval      EFI_NOT_FOUND   $BrdInfo structure not found
  @retval      EFI_NOT_READY   $BrdInfo structure not ready yet
**/
EFI_STATUS
EepromGetBoardInfo (
  OUT   BOARD_INFO_TABLE   **BoardInfo
  )
{
  CHAR8               AsciiData[32];
  UINT8               EepromLibrary;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Return the $BrdInfo structure
  // 1. Get valid EEPROM library index
  // 2. Find $BrdInfo structure
  //
  EepromLibrary = GetValidEepromLibrary (TRUE);
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, EEPROM_BOARD_INFO_SIGNATURE, (UINT8 **) BoardInfo, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $BrdInfo structure in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      if (mEepromPlatformLibDebugFlag) {
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->signature, 8);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Signature    = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Version      = %04x:%04x\n", __FUNCTION__, __LINE__, (*BoardInfo)->vermajor, (*BoardInfo)->verminor));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Length       = 0x%08x\n",    __FUNCTION__, __LINE__, (*BoardInfo)->length));
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->manuname, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Manufacturer = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->brdname, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board name   = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, (*BoardInfo)->brdserial, 16);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Serial #     = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board ID     = 0x%08x\n",    __FUNCTION__, __LINE__, (*BoardInfo)->boardid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Fab ID       = 0x%08x\n",    __FUNCTION__, __LINE__, (*BoardInfo)->fabid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - EC ID        = 0x%08x\n",    __FUNCTION__, __LINE__, (*BoardInfo)->ecid));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Board type   = %a\n",        __FUNCTION__, __LINE__, (*BoardInfo)->boardtype ? "Main board (RedBox)" : "Plug-in board"));
      }
      Status = EFI_SUCCESS;
    }
  }
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $Logo$ data

  @param[out]  LogoData        Buffer containing the $Logo$ data
  @param[out]  LogoSize        Size of the LogoData buffer

  @retval      EFI_SUCCESS     $Logo$ data found
  @retval      EFI_NOT_FOUND   $Logo$ data not found
  @retval      EFI_NOT_READY   $Logo$ data not ready yet
**/
EFI_STATUS
EepromGetLogo (
  OUT   UINT8   **LogoData,
  OUT   UINT32   *LogoSize
  )
{
  CHAR8               AsciiData[32];
  UINT8               EepromLibrary;
  LOGO_DATA          *LogoStructure;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Return the $Logo$ data
  // 1. Get valid EEPROM library index
  // 2. Find $Logo$ structure
  // 3. Return $Logo$ data
  //
  EepromLibrary = GetValidEepromLibrary (TRUE);
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, EEPROM_LOGO_DATA_SIGNATURE, (UINT8 **) &LogoStructure, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $Logo$ structure in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      *LogoSize = LogoStructure->length - sizeof (LOGO_DATA);
      if (*LogoSize == 0) {
        *LogoData = EepromFreePool (LogoStructure);
        DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $Logo$ structure data in the EERPOM binary!\n", __FUNCTION__, __LINE__));
        Status = EFI_NOT_FOUND;
      } else {
        *LogoData = (UINT8 *) LogoStructure + sizeof (LOGO_DATA);
        if (mEepromPlatformLibDebugFlag) {
          ZeroMem (AsciiData, 32);
          CopyMem (AsciiData, LogoStructure->signature, 8);
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Signature    = %a\n",        __FUNCTION__, __LINE__, AsciiData));
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Version      = %04x:%04x\n", __FUNCTION__, __LINE__, LogoStructure->vermajor, LogoStructure->verminor));
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Length       = 0x%08x\n",    __FUNCTION__, __LINE__, LogoStructure->length));
        }
        Status = EFI_SUCCESS;
      }
    }
  }
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the $Video$ data

  @param[out]  VideoData       Buffer containing the $Video$ data
  @param[out]  VideoSize       Size of the VideoData buffer

  @retval      EFI_SUCCESS     $Video$ data found
  @retval      EFI_NOT_FOUND   $Video$ data not found
  @retval      EFI_NOT_READY   $Video$ data not ready yet
**/

EFI_STATUS
EepromGetVbt (
  OUT   UINT8   **VideoData,
  OUT   UINT32   *VideoSize
  )
{
  CHAR8               AsciiData[32];
  UINT8               EepromLibrary;
  VIDEO_DATA         *VideoStructure;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Return the $Video$ data
  // 1. Get valid EEPROM library index
  // 2. Find $Video$ structure
  // 3. Return $Video$ data
  //
  EepromLibrary = GetValidEepromLibrary (TRUE);
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, EEPROM_VIDEO_DATA_SIGNATURE, (UINT8 **) &VideoStructure, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $Video$ structure in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      *VideoSize = VideoStructure->length - sizeof (VIDEO_DATA);
      if (*VideoSize == 0) {
        *VideoData = EepromFreePool (VideoStructure);
        DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $Video$ structure data in the EERPOM binary!\n", __FUNCTION__, __LINE__));
        Status = EFI_NOT_FOUND;
      } else {
        *VideoData = (UINT8 *) VideoStructure + sizeof (VIDEO_DATA);
        if (mEepromPlatformLibDebugFlag) {
          ZeroMem (AsciiData, 32);
          CopyMem (AsciiData, VideoStructure->signature, 8);
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Signature    = %a\n",        __FUNCTION__, __LINE__, AsciiData));
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Version      = %04x:%04x\n", __FUNCTION__, __LINE__, VideoStructure->vermajor, VideoStructure->verminor));
          DEBUG ((DEBUG_INFO, "%a(#%4d) - Length       = 0x%08x\n",    __FUNCTION__, __LINE__, VideoStructure->length));
        }
        Status = EFI_SUCCESS;
      }
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
  CHAR8               AsciiData[32];
  GPIO_DATA_RECORD   *EepromGpioData;
  GPIO_DATA_HEADER   *EepromGpioHeader;
  UINT8               EepromLibrary;
  UINT32              GpioPadData;
  UINT32              OldGpioPadData;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Program any EEPROM defined GPIOs
  // 1. Get valid EEPROM library index
  // 2. Find GPIO data in EEPROM binary
  // 3. Loop thru GPIO data and program PADs if PAD is in whitelist
  //
  EepromLibrary = GetValidEepromLibrary (TRUE);
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, EEPROM_GPIO_SIGNATURE, (UINT8 **) &EepromGpioHeader, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find any GPIO data in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      EepromGpioData = (GPIO_DATA_RECORD *) ((UINT8 *) EepromGpioHeader + sizeof (GPIO_DATA_HEADER));
      if (mEepromPlatformLibDebugFlag) {
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, EepromGpioHeader->signature, 8);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Signature    = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Version      = %04x:%04x\n", __FUNCTION__, __LINE__, EepromGpioHeader->vermajor, EepromGpioHeader->verminor));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Length       = 0x%08x\n",    __FUNCTION__, __LINE__, EepromGpioHeader->length));
      }
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
        }
        EepromGpioData++;
      }
    }
    EepromFreePool (EepromGpioHeader);
    Status = EFI_SUCCESS;
  }
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

