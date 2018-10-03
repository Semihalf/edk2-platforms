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
#define   EEPROM_DEADLOOP              0x00

///
/// FSP Reset Status code
/// These are defined in FSP EAS v2.0 section 11.2.2 - OEM Status Code
/// @{ came from FspApi.h
#define FSP_STATUS_RESET_REQUIRED_COLD         0x40000001
#define FSP_STATUS_RESET_REQUIRED_WARM         0x40000002
#define FSP_STATUS_RESET_REQUIRED_3            0x40000003
#define FSP_STATUS_RESET_REQUIRED_4            0x40000004
#define FSP_STATUS_RESET_REQUIRED_5            0x40000005
#define FSP_STATUS_RESET_REQUIRED_6            0x40000006
#define FSP_STATUS_RESET_REQUIRED_7            0x40000007
#define FSP_STATUS_RESET_REQUIRED_8            0x40000008
/// @}

////
//// TypeDefs
////

////
//// Structures
////

////
//// Functions
////
/**
  Perform platform related reset in FSP wrapper.

  This function will reset the system with requested ResetType.

  @param[in] FspStatusResetType  The type of reset the platform has to perform.
**/
VOID
EFIAPI
CallFspWrapperResetSystem (
  IN UINT32    FspStatusResetType
  );

EFI_STATUS
EFIAPI
EepromCalculateCrc32 (
  IN     UINT8    *Data,
  IN     UINTN     DataSize,
  IN OUT UINT32   *CrcOut
  );

EFI_STATUS
EFIAPI
SignedHashCheck (
  IN       UINT8             LibraryIndex
  );

#endif // _EEPROM_LIB_COMMON_

