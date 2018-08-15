/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_LIB_
#define _EEPROM_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <EepromStruct.h>
#include <Library/EepromDataLib.h>


////
//// defines
////
#define EEPROM_SIGNATURE_SIZE   9


////
//// Functions
////
EFI_STATUS
EFIAPI
EraseEeprom (
  IN       UINT8     LibraryIndex
  );

EFI_STATUS
EFIAPI
GetEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  );

UINT32
EFIAPI
GetImageSize (
  IN       UINT8      LibraryIndex
  );

EFI_STATUS
EFIAPI
GetNextEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   UINT32    *Index,
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  );

UINT8
EFIAPI
GetValidEepromLibrary (
  IN       BOOLEAN   CopyToMemory
  );

BOOLEAN
EFIAPI
InPeiPhase (VOID);

EFI_STATUS
EFIAPI
ValidateEeprom (
  IN       UINT8     LibraryIndex
  );

//
// Desc:        Registers the raw data libraries
// Variables:   None
// Return:      EFI_SUCCESS
//
EFI_STATUS
EFIAPI
EepromInitConstructor (VOID);

#endif // _EEPROM_LIB_

