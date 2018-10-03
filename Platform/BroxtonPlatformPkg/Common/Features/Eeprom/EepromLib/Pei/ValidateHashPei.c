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

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

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
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - GetVariable(%s) --> %r\n", __FUNCTION__, __LINE__, VariableName, Status));
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
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Returning --> %r\n", __FUNCTION__, __LINE__, Status));
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
  INTN                    CmpMemReturn;
  UINT8                  *Hash;
  UINT32                  HashSize;
  SIGNATURE_DATA         *Signature;
  UINT32                  Size;
  EFI_STATUS              Status;
  UINT8                  *Variable;
  UINT32                  VariableSize;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting [%a]...\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));

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
  Signature = NULL;
  Size      = 0;
  Status    = GetEepromStructureData (&LibraryIndex, EEPROM_SIGNATURE_SIGNATURE, NULL, sizeof (SIGNATURE_DATA), (UINT8**) &Signature, &Hash, &HashSize);
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - GetEepromStructureData(EEPROM_SIGNATURE_SIGNATURE) [%a] --> %r\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex], Status));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }

  //
  // Get stored hash
  //
  Variable     = NULL;
  VariableSize = 0;
  Status       = GetEepromVariable (LibraryIndex, &Variable, &VariableSize);
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - GetEepromVariable(%a) 0x%08x --> %r\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex], VariableSize, Status));
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
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Stored size returned 0! [%r]\n", __FUNCTION__, __LINE__, Status));
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
  CmpMemReturn = CompareMem (Variable, Hash, HashSize);
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CompareMem(Variable, Hash) --> %d\n", __FUNCTION__, __LINE__, CmpMemReturn));
  if (CmpMemReturn != 0) {
    //
    // Failed security match
    //
    Status = EFI_SECURITY_VIOLATION;
    if (mEepromLibDebugFlag) {
      DEBUG ((DEBUG_INFO, "\n\n================================================================================\n"));
      DEBUG ((DEBUG_INFO, "%a (#%4d) - Hash from variable...\n", __FUNCTION__, __LINE__));
      DEBUG ((DEBUG_INFO, "--------------------------------------------------------------------------------\n"));
      EepromDumpParagraph (DEBUG_INFO, Variable, VariableSize);
      DEBUG ((DEBUG_INFO, "================================================================================\n"));
      DEBUG ((DEBUG_INFO, "\n\n================================================================================\n"));
      DEBUG ((DEBUG_INFO, "%a (#%4d) - Hash from %a...\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex]));
      DEBUG ((DEBUG_INFO, "--------------------------------------------------------------------------------\n"));
      EepromDumpParagraph (DEBUG_INFO, Hash, HashSize);
      DEBUG ((DEBUG_INFO, "================================================================================\n\n"));
    }
  } else {
    Status = EFI_SUCCESS;
  }

Exit:
  //
  // Clear resources
  //
  Signature = EepromFreePool (Signature);
  Variable  = EepromFreePool (Variable);
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Returning [%a] --> %r\n", __FUNCTION__, __LINE__, mEepromLibraryString[LibraryIndex], Status));
  return Status;
}

