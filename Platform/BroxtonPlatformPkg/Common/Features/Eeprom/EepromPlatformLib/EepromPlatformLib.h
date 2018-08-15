/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_PLATFORM_LIB_COMMON_
#define _EEPROM_PLATFORM_LIB_COMMON_
////
//// Header files
////
#include <Uefi.h>

#include <Guid/EepromVariable.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/EepromLib.h>
#include <Library/EepromPlatformLib.h>
#include <Library/GpioLib.h>
#include <Library/MemoryAllocationLib.h>

#include <EepromStruct.h>

extern  BOOLEAN      mEepromPlatformLibDebugFlag;

#endif // _EEPROM_PLATFORM_LIB_COMMON_
