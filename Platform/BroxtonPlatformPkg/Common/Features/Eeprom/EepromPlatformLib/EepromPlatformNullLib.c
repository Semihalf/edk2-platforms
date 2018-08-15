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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
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
