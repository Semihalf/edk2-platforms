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
EepromDataInitConstructor (VOID)
{
  //
  // 00 - NULL raw library
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM NULL raw data library into slot #%02x\n", __FUNCTION__, __LINE__, EEPROM_NULL));
  mEepromDataLibIndex[EEPROM_NULL].Active        = TRUE;
  mEepromDataLibIndex[EEPROM_NULL].ReadFunction  = ReadEepromNull;
  mEepromDataLibIndex[EEPROM_NULL].WriteFunction = WriteEepromNull;

  //
  // 01 - EEPROM raw library
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM raw data library into slot #%02x\n", __FUNCTION__, __LINE__, EEPROM_EEPROM));
  mEepromDataLibIndex[EEPROM_EEPROM].Active        = TRUE;
  mEepromDataLibIndex[EEPROM_EEPROM].ReadFunction  = ReadEepromEeprom;
  mEepromDataLibIndex[EEPROM_EEPROM].WriteFunction = WriteEepromEeprom;

  //
  // 02 - FV raw library
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM FV raw data library into slot #%02x\n", __FUNCTION__, __LINE__, EEPROM_FV));
  mEepromDataLibIndex[EEPROM_FV].Active        = TRUE;
  mEepromDataLibIndex[EEPROM_FV].ReadFunction  = ReadEepromFv;
  mEepromDataLibIndex[EEPROM_FV].WriteFunction = WriteEepromFv;

  //
  // 03 - Memory raw library
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM memory raw data library into slot #%02x\n", __FUNCTION__, __LINE__, EEPROM_MEMORY));
  mEepromDataLibIndex[EEPROM_MEMORY].Active        = TRUE;
  mEepromDataLibIndex[EEPROM_MEMORY].ReadFunction  = ReadEepromMemory;
  mEepromDataLibIndex[EEPROM_MEMORY].WriteFunction = WriteEepromMemory;

  return EFI_SUCCESS;
}

