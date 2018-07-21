/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_DATA_MEMORY_LIB_
#define _EEPROM_DATA_MEMORY_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <Guid/EepromVariable.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/PcdLib.h>

////
//// Defines
////
#define   MEMORY_HOB_HEADER_SIGNATURE   SIGNATURE_32 ('e', 'H', 'B', 'm')
#define   MEMORY_HOB_HEADER_VERSION     0x00000001
#define   MEMORY_HOB_MAX_DATA_SIZE      (30 * 1024)

////
//// Structures
////
typedef struct {
   UINT32    Signature;
   UINT32    Version;
   UINT16    BlockNumber;
   UINT16    BlockSize;
   UINT32    Reserved;
} MEMORY_HOB_HEADER;

////
//// Functions
////
EFI_STATUS
EFIAPI
GetEepromDataHobData (VOID);

EFI_STATUS
EFIAPI
SetEepromDataHobData (VOID);

#endif // _EEPROM_DATA_MEMORY_LIB_

