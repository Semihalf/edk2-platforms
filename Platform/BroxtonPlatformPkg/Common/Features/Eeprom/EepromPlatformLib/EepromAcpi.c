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

#include <Guid/AcpiTableStorage.h>
#include <Guid/PlatformInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/FirmwareVolume2.h>

/*++

Routine Description:

Locate the first instance of a protocol.  If the protocol requested is an
FV protocol, then it will return the first FV that contains the ACPI table
storage file.

Arguments:

Protocol  -  The protocol to find.
Instance  -  Return pointer to the first instance of the protocol.
Type      -  The type of protocol to locate.

Returns:

EFI_SUCCESS           -  The function completed successfully.
EFI_NOT_FOUND         -  The protocol could not be located.
EFI_OUT_OF_RESOURCES  -  There are not enough resources to find the protocol.

--*/
EFI_STATUS
LocateSupportProtocol(
IN   EFI_GUID       *Protocol,
OUT  VOID           **Instance,
IN   UINT32         Type
)
{
    EFI_STATUS              Status;
    EFI_HANDLE              *HandleBuffer;
    UINTN                   NumberOfHandles;
    EFI_FV_FILETYPE         FileType;
    UINT32                  FvStatus;
    EFI_FV_FILE_ATTRIBUTES  Attributes;
    UINTN                   Size;
    UINTN                   Index;

    FvStatus = 0;
    //
    // Locate protocol.
    //
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        Protocol,
        NULL,
        &NumberOfHandles,
        &HandleBuffer
        );
    if (EFI_ERROR(Status)) {
        //
        // Defined errors at this time are not found and out of resources.
        //
        return Status;
    }

    //
    // Looking for FV with ACPI storage file
    //
    for (Index = 0; Index < NumberOfHandles; Index++) {
        //
        // Get the protocol on this handle
        // This should not fail because of LocateHandleBuffer
        //
        Status = gBS->HandleProtocol(
            HandleBuffer[Index],
            Protocol,
            Instance
            );
        ASSERT(!EFI_ERROR(Status));

        if (!Type) {
            //
            // Not looking for the FV protocol, so find the first instance of the
            // protocol.  There should not be any errors because our handle buffer
            // should always contain at least one or LocateHandleBuffer would have
            // returned not found.
            //
            break;
        }
        //
        // See if it has the ACPI storage file
        //
        Status = ((EFI_FIRMWARE_VOLUME2_PROTOCOL*)(*Instance))->ReadFile(
            *Instance,
            &gEfiAcpiTableStorageGuid,
            NULL,
            &Size,
            &FileType,
            &Attributes,
            &FvStatus
            );
        //
        // If we found it, then we are done
        //
        if (!EFI_ERROR(Status)) {
            break;
        }
    }
    //
    // Our exit status is determined by the success of the previous operations
    // If the protocol was found, Instance already points to it.
    //
    //
    // Free any allocated buffers
    //
    gBS->FreePool(HandleBuffer);

    return Status;
}

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
  UINT8                        EepromLibrary;
  UINT8                       *AcpiData;
  UINT32                       AcpiSize;
  ACPI_TABLE                  *AcpiStructure;
  EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupport;
  UINT32                       Size;
  EFI_STATUS                   Status;
  UINTN                        TableHandle;

  if (mEepromPlatformLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (&gEfiAcpiSupportProtocolGuid, (VOID **) &AcpiSupport, 0);
  if (EFI_ERROR (Status)) {
    Status = EFI_NOT_READY;
    goto Exit;
  }

  //
  // Program the $AcpiTbl data
  // 1. Get valid EEPROM library index
  // 2. Find $AcpiTbl structure
  // 3. Program the $AcpiTbl data
  //
  EepromLibrary = GetValidEepromLibrary (TRUE);
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, EEPROM_ACPI_TABLE_SIGNATURE, (UINT8 **) &AcpiStructure, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $AcpiTbl structure in the EERPOM binary!\n", __FUNCTION__, __LINE__));
      Status = EFI_NOT_FOUND;
    } else {
      AcpiSize = AcpiStructure->length - sizeof (ACPI_TABLE);
      if (AcpiSize == 0) {
        EepromFreePool (AcpiStructure);
        DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the $AcpiTbl structure data in the EERPOM binary!\n", __FUNCTION__, __LINE__));
        Status = EFI_NOT_FOUND;
      } else {
        AcpiData = (UINT8 *) AcpiStructure + sizeof (ACPI_TABLE);
        //
        // publish the AcpiTable
        //
        TableHandle = 0;
        Status = AcpiSupport->SetAcpiTable (
                                AcpiSupport,
                                AcpiData,
                                TRUE,
                                EFI_ACPI_TABLE_VERSION_2_0,
                                &TableHandle
                                );
        //
        // re-init variables
        //
        AcpiStructure = EepromFreePool (AcpiStructure);
      }
    }
  }
Exit:
  return Status;
}
