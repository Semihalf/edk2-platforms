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

/**
  Programs the ACPI SSDT data in $AcpiTbl

  @param[in]  VOID

  @retval     EFI_SUCCESS     $AcpiTbl data found
  @retval     EFI_NOT_FOUND   $AcpiTbl data not found
  @retval     EFI_NOT_READY   $AcpiTbl data not ready to be programmed
**/
EFI_STATUS
EepromProgramAcpi (VOID)
{
  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  return EFI_NOT_READY;
}
