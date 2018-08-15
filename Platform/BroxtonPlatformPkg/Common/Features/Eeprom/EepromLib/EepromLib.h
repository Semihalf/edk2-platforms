/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_LIB_COMMON_
#define _EEPROM_LIB_COMMON_
////
//// Header files
////
#include <Uefi.h>

#include <Guid/EepromVariable.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>

#include <EepromStruct.h>


////
//// Defines
////
#define   HASH_TYPE_MASK               0x00FF
#define   MAX_HASH_TYPE                5
#define   MAX_DIGEST_SIZE              SHA512_DIGEST_SIZE
#define   EFI_CERT_TYPE_RSA2048_SIZE   256

////
//// TypeDefs
////

////
//// Structures
////

////
//// Functions
////
EFI_STATUS
EFIAPI
EepromCalculateCrc32 (
  IN     UINT8    *Data,
  IN     UINTN     DataSize,
  IN OUT UINT32   *CrcOut
  );

EFI_STATUS
EFIAPI
EepromLibNemToMemory (VOID);

EFI_STATUS
EFIAPI
SignedHashCheck (
  IN       UINT8             LibraryIndex
  );

#endif // _EEPROM_LIB_COMMON_

