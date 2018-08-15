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

#include "EepromLib.h"

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>

EFI_STATUS
EFIAPI
GetEepromVariable (
  IN       UINT8             LibraryIndex,
  OUT      UINT8           **Buffer,
  OUT      UINT32           *BufferSize
  )
{
  UINT32                            Attributes;
  UINT8                            *Data;
  EFI_STATUS                        Status;
  CHAR16                            VariableName[32];
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINTN                             VariableSize;

  //
  // Initialize variables
  //
  Attributes   = 0;
  Data         = NULL;
  Status       = EFI_SUCCESS;
  UnicodeSPrint (VariableName, 32, L"PromSig-%d", LibraryIndex);

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if ((*Buffer != NULL) || (BufferSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Find variable services
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to locate Variable PPI! [%r]\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

  //
  // Get variable size
  //
  VariableSize = 0;
  Status = VariableServices->GetVariable (
                               VariableServices,       // *This
                               VariableName,           // Variable name in Unicode
                               &gEepromVariableGuid,   // Variable GUID
                               NULL,                   // Attributes
                               &VariableSize,          // Data size
                               NULL                    // Data
                               );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    if (Status == EFI_NOT_FOUND) {
      //
      // No variable to find
      //
      *Buffer     = NULL;
      *BufferSize = 0;
      Status = EFI_SUCCESS;
      goto Exit;
    } else {
      //
      // Oops!
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get size of variable! [%r]\n", __FUNCTION__, __LINE__, Status));
      goto Exit;
    }
  }

  //
  // Allocate buffer
  //
  Data = EepromAllocatePool (VariableSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to allocate buffer! [%r]\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

  //
  // Get variable
  //
  Status = VariableServices->GetVariable (
                               VariableServices,       // *This
                               VariableName,           // Variable name in Unicode
                               &gEepromVariableGuid,   // Variable GUID
                               NULL,                   // Attributes
                               &VariableSize,          // Data size
                               Data                    // Data
                               );
  if (EFI_ERROR (Status)) {
    //
    // Failed to get data
    //
    *Buffer     = NULL;
    *BufferSize = 0;
  } else {
    *Buffer     = Data;
    *BufferSize = VariableSize;
  }

Exit:
  if (EFI_ERROR (Status)) {
    Data = EepromFreePool (Data);
  }
  return Status;
}

BOOLEAN
EFIAPI
InPeiPhase (VOID)
{
  return TRUE;
}

EFI_STATUS
EFIAPI
SignedHashCheck (
  IN       UINT8             LibraryIndex
  )
{
  UINT8                  *Hash;
  UINT32                  HashSize;
  SIGNATURE_DATA         *Signature;
  UINT32                  Size;
  EFI_STATUS              Status;
  UINT8                  *Variable;
  UINT32                  VariableSize;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get $PromSig structure
  //
  Size   = 0;
  Status = GetEepromStructure (LibraryIndex, EEPROM_SIGNATURE_SIGNATURE, (UINT8 **) &Signature, &Size);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }
  Hash     = ((UINT8 *) Signature) + sizeof (SIGNATURE_DATA);
  HashSize = Signature->length - sizeof (SIGNATURE_DATA);

  //
  // Get stored hash
  //
  Variable     = NULL;
  VariableSize = 0;
  Status = GetEepromVariable (LibraryIndex, &Variable, &VariableSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get variable! [%r]\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

  //
  // Sanity checks
  //
  if (VariableSize == 0) {
    //
    // Nothing stored for this library. Bail.
    //
    Status = EFI_MEDIA_CHANGED;
    goto Exit;
  }
  if (VariableSize != HashSize) {
    Status = EFI_MEDIA_CHANGED;
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Stored size doesn't match real size! [%r]\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

  //
  // Compare stored to real
  //
  if (CompareMem (Variable, Hash, HashSize) != 0) {
    //
    // Failed security match
    //
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Exit:
  //
  // Clear resources
  //
  Signature = EepromFreePool (Signature);
  Variable  = EepromFreePool (Variable);
  return Status;
}

