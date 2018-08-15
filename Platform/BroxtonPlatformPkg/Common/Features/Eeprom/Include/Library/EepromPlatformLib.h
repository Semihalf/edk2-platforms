/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_PLATFORM_LIB_
#define _EEPROM_PLATFORM_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <EepromStruct.h>
#include <Library/EepromDataLib.h>
#include <Library/EepromLib.h>


////
//// defines
////
#define END_OF_GPIO_ARRAY   0xFFFFFFFF


////
//// Functions
////
/**
  Returns the $BrdInfo structure

  @param[out]  BoardInfo       Buffer containing the BOARD_INFO_TABLE structure
                               - Up to the caller to free the buffer

  @retval      EFI_SUCCESS     $BrdInfo structure found
  @retval      EFI_NOT_FOUND   $BrdInfo structure not found
**/
EFI_STATUS
EepromGetBoardInfo (
  OUT   BOARD_INFO_TABLE   **BoardInfo
  );

/**
  Returns the $Logo$ data

  @param[out]  LogoData        Buffer containing the $Logo$ data
  @param[out]  LogoSize        Size of the LogoData buffer

  @retval      EFI_SUCCESS     $Logo$ data found
  @retval      EFI_NOT_FOUND   $Logo$ data not found
**/
EFI_STATUS
EepromGetLogo (
  OUT   UINT8   **LogoData,
  OUT   UINT32   *LogoSize
  );

/**
  Returns the $Video$ data

  @param[out]  VideoData       Buffer containing the $Video$ data
  @param[out]  VideoSize       Size of the VideoData buffer

  @retval      EFI_SUCCESS     $Logo$ data found
  @retval      EFI_NOT_FOUND   $Logo$ data not found
**/

EFI_STATUS
EepromGetVbt (
  OUT   UINT8   **VideoData,
  OUT   UINT32   *VideoSize
  );

/**
  Checks whether the PadOffset is in the platform GPIO whitelist.

  @param[in]  PadOffset    The PAD offset to check against the platform whitelist

  @retval     TRUE         PAD offset is in the whitelist
  @retval     FALSE        PAD offset is not in the whitelist
**/
BOOLEAN
EepromPadCheck (
  IN  UINT32    PadOffset
  );

/**
  Programs the ACPI SSDT data in $AcpiTbl

  @param[in]  VOID

  @retval     EFI_SUCCESS     $AcpiTbl data found
  @retval     EFI_NOT_FOUND   $AcpiTbl data not found
  @retval     EFI_NOT_READY   $AcpiTbl data not ready to be programmed
**/
EFI_STATUS
EepromProgramAcpi (VOID);

/**
  Program GPIOs per binary and whitelist.

  @param[in]  VOID

  @retval     EFI_SUCCESS     GPIOs programmed successfully.
  @retval     EFI_NOT_FOUND   GPIO data not found.
**/
EFI_STATUS
EepromProgramGpioPads (VOID);

#endif // _EEPROM_PLATFORM_LIB_

