/** @file
  Common EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataLib.h"

//
// Desc:        Registers the raw data libraries
// Variables:   None
// Return:      EFI_SUCCESS, anything else will cause an ASSERT
//
EFI_STATUS
EFIAPI
EepromDataNullInitConstructor (VOID)
{
  //
  // 00 - NULL raw library
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM NULL raw data library into slot #%02x\n", __FUNCTION__, __LINE__, EEPROM_NULL));
  mEepromDataLibIndex[EEPROM_NULL].Active        = TRUE;
  mEepromDataLibIndex[EEPROM_NULL].ReadFunction  = ReadEepromNull;
  mEepromDataLibIndex[EEPROM_NULL].WriteFunction = WriteEepromNull;

  return EFI_SUCCESS;
}
