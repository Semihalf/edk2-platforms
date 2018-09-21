/** @file
  Locate and install Firmware Volume Hob's Once there is main memory.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/VariableFormat.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiPlatformConfigUpdateLib.h>
#include <Library/PeiVariableCacheLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HeciMsgLib.h>
#include <Guid/FspHeaderFile.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/BpdtLib.h>
#include "FvCallback.h"

#define MAX_DIGEST_SIZE    64
#define TEMP_TOLUD         0x80000000

extern EFI_GUID gObbyFirmwareFileSystemFvGuid;
extern EFI_GUID gFspSFirmwareFileSystemFvGuid;
extern EFI_GUID gIbbrFirmwareFileSystemFvGuid;


VOID
PrintSetupVariableData (
  IN UINT8   *Data8,
  IN UINTN   DataSize
  )
{
  UINTN      Index;

  for (Index = 0; Index < DataSize; Index++) {
    if (Index % 0x10 == 0) {
      DEBUG ((DEBUG_INFO, "\n%08X:", Index));
    }
    DEBUG ((DEBUG_INFO, " %02X", *Data8++));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}


/**
  Produces the default variable HOB to be consumed by the variable driver.

  @param[in]  VariableStoreHeader       Pointer to the NV Storage firmware volume header.
  @param[in]  DefaultVariableDataSize   The DataSize of variable.

  @retval     EFI_SUCCESS               If the function completed successfully.

**/
EFI_STATUS
EFIAPI
CreateDefaultVariableHob (
  IN CONST VARIABLE_STORE_HEADER  *VariableStoreHeader,
  IN       UINT32                 DefaultVariableDataSize
  )
{
  UINT32                  VariableDataOffset    = 0;
  UINT32                  VariableHobDataOffset = 0;

  UINT8                   *VariablePtr             = NULL;
  UINT8                   *VariableHobPtr          = NULL;
  VARIABLE_STORE_HEADER   *VariableStoreHeaderHob  = NULL;

  if (VariableStoreHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "Total size requested for HOB = %d.\n", (sizeof (VARIABLE_STORE_HEADER) + DefaultVariableDataSize + HEADER_ALIGNMENT - 1)));

  VariableStoreHeaderHob = (VARIABLE_STORE_HEADER *) BuildGuidHob (&VariableStoreHeader->Signature,
                                                                   sizeof (VARIABLE_STORE_HEADER) + DefaultVariableDataSize + HEADER_ALIGNMENT - 1);

  if (VariableStoreHeaderHob == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "Default HOB allocated at 0x%x\n", VariableStoreHeaderHob));

  //
  // Copy the variable store header to the beginning of the HOB
  //
  CopyMem (VariableStoreHeaderHob, VariableStoreHeader, sizeof (VARIABLE_STORE_HEADER));

  VariablePtr           = (UINT8 *) HEADER_ALIGN ((UINTN) (VariableStoreHeader + 1));
  VariableDataOffset    = (UINT32) ((UINTN) VariablePtr - (UINTN) VariableStoreHeader);
  VariableHobPtr        = (UINT8 *) HEADER_ALIGN ((UINTN) (VariableStoreHeaderHob + 1));
  VariableHobDataOffset = (UINT32) ((UINTN) VariableHobPtr - (UINTN) VariableStoreHeaderHob);

  //
  // Copy the Setup default variable
  //
  CopyMem (VariableHobPtr, VariablePtr, DefaultVariableDataSize);

  //
  // Update the variable store size in the HOB
  //
  VariableStoreHeaderHob->Size = sizeof (VARIABLE_STORE_HEADER) + DefaultVariableDataSize;

  return EFI_SUCCESS;
}


/**
  Caches the Setup defaults so Setup data is returned more quickly.

  @param[in]  NvStorageFvHeader   Pointer to the NV Storage firmware volume header.

  @retval     EFI_SUCCESS         If the function completed successfully.

**/
EFI_STATUS
EFIAPI
CreateVariableHobs (
  IN CONST EFI_FIRMWARE_VOLUME_HEADER *NvStorageFvHeader
  )
{
  EFI_STATUS                        Status;
  EFI_PHYSICAL_ADDRESS              VariableStoreBase;
  EFI_GUID                          VariableGuid;
  UINT8                             VariableState;
  BOOLEAN                           AuthenticatedVariableStore = FALSE;
  BOOLEAN                           FoundVariableDefaults      = FALSE;
  UINT32                            VariableHeaderSize = 0;
  UINT32                            VariableNameSize   = 0;
  UINT32                            VariableDataSize   = 0;
  UINT32                            SetupVariableSize  = 0;
  CHAR16                            *VariableName                     = NULL;
  UINT8                             *VariableSearchCandidateDataPtr   = NULL;
  VARIABLE_STORE_HEADER             *VariableStoreHeader              = NULL;
  VARIABLE_HEADER                   *StartVariableStoreVariableHeader = NULL;
  VARIABLE_HEADER                   *VariableSearchHeader             = NULL;
  VARIABLE_HEADER                   *LastVariableStoreVariableHeader  = NULL;
  SYSTEM_CONFIGURATION              *SystemConfiguration              = NULL;

  DEBUG ((DEBUG_INFO, "Loading variable defaults from NvStorage...\n"));
  DEBUG ((DEBUG_INFO, "  NvStorageHeader at 0x%x\n", NvStorageFvHeader));

  if (NvStorageFvHeader == NULL \
    || NvStorageFvHeader->Signature != EFI_FVH_SIGNATURE \
    || !CompareGuid (&gEfiSystemNvDataFvGuid, &NvStorageFvHeader->FileSystemGuid)) {
    DEBUG ((DEBUG_ERROR, "  NvStorage FV passed to gather setup defaults is invalid!\n"));
    return EFI_INVALID_PARAMETER;
  }

  VariableStoreBase   = (EFI_PHYSICAL_ADDRESS) ((UINTN) NvStorageFvHeader + (UINTN)(NvStorageFvHeader->HeaderLength));
  VariableStoreHeader = (VARIABLE_STORE_HEADER *) (UINTN) VariableStoreBase;

  DEBUG ((DEBUG_INFO, "  VariableStoreHeader at 0x%x. VariableStoreSize = %d\n", VariableStoreHeader, (UINTN) VariableStoreHeader->Size));

  AuthenticatedVariableStore = (BOOLEAN) CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid);

  StartVariableStoreVariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN (VariableStoreHeader + 1);
  LastVariableStoreVariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN ((CHAR8 *) VariableStoreHeader + VariableStoreHeader->Size);

  DEBUG ((DEBUG_INFO, "  StartVariableStoreVariableHeader at 0x%x. LastVariableSearchHeader at 0x%x\n", StartVariableStoreVariableHeader, LastVariableStoreVariableHeader));

  ASSERT (StartVariableStoreVariableHeader < LastVariableStoreVariableHeader);

  VariableHeaderSize = (AuthenticatedVariableStore) ? sizeof (AUTHENTICATED_VARIABLE_HEADER) : sizeof (VARIABLE_HEADER);

  //
  // Attempt to find the SetupDefault variable
  //
  VariableSearchHeader = StartVariableStoreVariableHeader;

  while ((VariableSearchHeader != NULL)
    && (VariableSearchHeader <= LastVariableStoreVariableHeader)
    && (VariableSearchHeader->StartId == VARIABLE_DATA)) {
    VariableName = (CHAR16 *) ((CHAR8 *) ((CHAR8 *) VariableSearchHeader + VariableHeaderSize));

    if (!AuthenticatedVariableStore) {
      VariableNameSize = VariableSearchHeader->NameSize;
      VariableDataSize = VariableSearchHeader->DataSize;
      VariableGuid     = VariableSearchHeader->VendorGuid;
      VariableState    = VariableSearchHeader->State;
    } else {
      VariableNameSize = ((AUTHENTICATED_VARIABLE_HEADER *) VariableSearchHeader)->NameSize;
      VariableDataSize = ((AUTHENTICATED_VARIABLE_HEADER *) VariableSearchHeader)->DataSize;
      VariableGuid     = ((AUTHENTICATED_VARIABLE_HEADER *) VariableSearchHeader)->VendorGuid;
      VariableState    = ((AUTHENTICATED_VARIABLE_HEADER *) VariableSearchHeader)->State;
    }

    DEBUG ((DEBUG_INFO, "  VariableName at 0x%x.\n", VariableName));

    VariableSearchCandidateDataPtr = (UINT8 *) (((CHAR8 *) VariableSearchHeader + VariableHeaderSize) \
                                       + VariableNameSize \
                                       + GET_PAD_SIZE (VariableNameSize));

    DEBUG ((DEBUG_INFO, "  VariableSearchCandidatePtr at 0x%x.\n", (UINTN) VariableSearchCandidateDataPtr));
    DEBUG ((DEBUG_INFO, "    Variable name is %s.\n", VariableName));
    DEBUG ((DEBUG_INFO, "    Variable data size is %d bytes.\n", VariableDataSize));

    if (CompareGuid (&VariableGuid, &gEfiSetupVariableGuid) \
      && !StrCmp (L"Setup", VariableName) \
      && (VariableState == VAR_ADDED)) {
      DEBUG ((DEBUG_INFO, "  Found the Setup defaults via BPDT to cache.\n"));

      SystemConfiguration   = (SYSTEM_CONFIGURATION *) VariableSearchCandidateDataPtr;
      SetupVariableSize     = VariableDataSize;
      FoundVariableDefaults = TRUE;
    }

    //
    // Get next variable header
    //
    VariableSearchHeader = (VARIABLE_HEADER *) HEADER_ALIGN ((CHAR8 *) VariableSearchCandidateDataPtr \
                                  + VariableDataSize \
                                  + GET_PAD_SIZE (VariableDataSize));
  }

  if (!FoundVariableDefaults) {
    DEBUG ((DEBUG_ERROR, "Could not find the setup defaults in the NvStorage FV!\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "  Setup data before update at address 0x%x\n", SystemConfiguration));
  DEBUG ((DEBUG_INFO, "  Size of data in Setup header = %d bytes. Size of SYSTEM_CONFIGURATION = %d bytes.\n", SetupVariableSize, sizeof (SYSTEM_CONFIGURATION)));
  DEBUG ((DEBUG_INFO, "  Printing system configuration variable data read from the FV:\n"));
  PrintSetupVariableData ((UINT8 *) SystemConfiguration, VariableDataSize);

  //
  // Update values in Setup before it is written to the default HOB
  //
  Status = UpdateSetupDataValues (SystemConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "  Couldn't override memory and platform information system configuration values.\n"));
  }

  DEBUG ((DEBUG_INFO, "  Total size of data to copy for default HOB = %d bytes.\n", (UINT32) ((VariableSearchCandidateDataPtr + VariableDataSize) - (UINTN) StartVariableStoreVariableHeader)));

  Status = CreateDefaultVariableHob (VariableStoreHeader, (UINT32) ((VariableSearchCandidateDataPtr + VariableDataSize) - (UINTN) StartVariableStoreVariableHeader));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "  Error occurred creating the default variable HOB. Variable data is invalid.\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Successfully read Setup defaults.\n"));

  return Status;
}


EFI_STATUS
EFIAPI
ParseObbPayload (
  IN   UINT8                       *PayloadPtr,
  IN   UINTN                       PayloadSize,
  IN   EFI_BOOT_MODE               BootMode
  )
{
  UINT32                           PayloadTail;
  UINTN                            VariableSize;
  EFI_STATUS                       Status;
  EFI_FIRMWARE_VOLUME_HEADER       *FvHeader;
  EFI_PLATFORM_INFO_HOB            *PlatformInfo;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SYSTEM_CONFIGURATION             SystemConfiguration;
  EFI_HOB_GUID_TYPE                *GuidHobPtr;
  EFI_HOB_GUID_TYPE                *FdoEnabledGuidHob = NULL;
  EFI_GUID                         *FvName;
  FSP_INFO_HEADER                  *FspHeader;
  UINT32                           FspSImageBase;
  VOID                             *Memory;

  DEBUG ((DEBUG_INFO, "Parsing and checking OBB Payload\n"));

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) PayloadPtr;
  PayloadTail = (UINTN) PayloadPtr + PayloadSize;
  DEBUG ((DEBUG_INFO, "FvHeader = 0x%x; PayloadTail = 0x%x\n", (UINT32) FvHeader, PayloadTail));

  FspHeader = (FSP_INFO_HEADER *) FspFindFspHeader ((UINT32) FvHeader);
  if (FspHeader == NULL) {
    DEBUG ((DEBUG_INFO, "Cannot find FSP-S header!\n"));
    return EFI_DEVICE_ERROR;
  }
  FspSImageBase = FspHeader->ImageBase;
  DEBUG ((DEBUG_INFO, "FSP-S FV Base = 0x%X\n", (UINT32) FvHeader));
  DEBUG ((DEBUG_INFO, "FSP-S FV Length = 0x%X\n", (UINT32) FvHeader->FvLength));
  DEBUG ((DEBUG_INFO, "FSP-S Fsp Base = 0x%X\n", (UINT32) FspSImageBase));
  DEBUG ((DEBUG_INFO, "FSP-S Fsp Size = 0x%X\n", FspHeader->ImageSize));

  //
  // Copy to FSP-S to preferred base. The preferred base is defined in FSP Integration Guide.
  // This region must be reserved for BIOS S3 resume.
  //
  BuildMemoryAllocationHob (
    (EFI_PHYSICAL_ADDRESS)FspSImageBase,
    (UINT64)FspHeader->ImageSize,
    EfiReservedMemoryType
  );
  CopyMemSse4 ((VOID*) FspSImageBase, FvHeader, (UINT32) FvHeader->FvLength);

  PcdSet32S (PcdFspsBaseAddress, (UINT32) FspSImageBase);

  while ((UINT32) FvHeader < PayloadTail) {
    if (FvHeader->Signature != EFI_FVH_SIGNATURE) {
      DEBUG ((DEBUG_ERROR, "Found Fv with invalid signature -- Stop parsing OBB.\n"));
      break;
    }

    if (FvHeader->ExtHeaderOffset == 0) {
      FvName = (EFI_GUID *) ((UINTN) FvHeader + 0x48);  // 0x48 == FvHeader->BlockMap
    } else {
      FvName = (EFI_GUID *) ((UINTN) FvHeader + FvHeader->ExtHeaderOffset);
    }
    DEBUG ((DEBUG_INFO, "Found Fv with GUID: %g\n", FvName));

    if (CompareGuid (&FvHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid)) {
      DEBUG ((DEBUG_INFO, "NVStorage FV at 0x%x.\n", (UINT32) FvHeader));
      Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Could not locate EFI_PEI_READ_ONLY_VARIABLE2_PPI.\n"));
        ASSERT_EFI_ERROR (Status);
        return EFI_NOT_FOUND;
      }
      FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

      VariableSize = sizeof (SYSTEM_CONFIGURATION);
      Status = VariableServices->GetVariable (
                                   VariableServices,
                                   PLATFORM_SETUP_VARIABLE_NAME,
                                   &gEfiSetupVariableGuid,
                                   NULL,
                                   &VariableSize,
                                   &SystemConfiguration
                                   );

      if (Status == EFI_NOT_FOUND || FdoEnabledGuidHob != NULL) {
        DEBUG ((DEBUG_INFO, "Initializing variable defaults from BPDT...\n"));

        //
        // Create the variable default HOB
        //
        Status = CreateVariableHobs (FvHeader);
      } else if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Unable to read Setup variable to determine if defauls should be loaded.\n"));
        ASSERT_EFI_ERROR (Status);
      }
    } else if (CompareGuid (FvName, &gObbyFirmwareFileSystemFvGuid)) {
      DEBUG ((DEBUG_INFO, "Found OBBY: Saving info and skipping Install FV.\n"));
      GuidHobPtr = GetFirstGuidHob (&gEfiPlatformInfoGuid);
      ASSERT (GuidHobPtr != NULL);
      PlatformInfo = GET_GUID_HOB_DATA (GuidHobPtr);
      PlatformInfo->FvMain3Base = (UINTN) FvHeader;
      PlatformInfo->FvMain3Length = (UINT32) (FvHeader->FvLength);
    } else if (!CompareGuid(FvName, &gFspSFirmwareFileSystemFvGuid)) {
      Memory = AllocatePages (EFI_SIZE_TO_PAGES ((UINT32) (FvHeader->FvLength)));
      if(Memory !=NULL) {
        CopyMem (Memory, FvHeader, (UINT32) (FvHeader->FvLength));
        PeiServicesInstallFvInfoPpi (
          NULL,
          (VOID *) Memory,
          (UINT32) (FvHeader->FvLength),
          NULL,
          NULL
          );
      } else  {
        ASSERT (FALSE);
      }

    }//if/else S3

    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) FvHeader + (UINTN) FvHeader->FvLength);
  } //while (< PayloadTail)

  DEBUG ((DEBUG_INFO, "PcdFspsBaseAddress:  0x%x\n", PcdGet32 (PcdFspsBaseAddress) ));

  return EFI_SUCCESS;
}


/**
  Locate and install Firmware Volume Hob's once there is main memory

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.

**/
EFI_STATUS
EFIAPI
GetFvNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_BOOT_MODE                  BootMode;
  BPDT_PAYLOAD_DATA             *ObbPayloadPtr;
  EFI_HOB_GUID_TYPE             *GuidHobPtr;
  BPDT_HEADER                   *Bp1HdrPtr;
  BPDT_HEADER                   *Bp2HdrPtr;
  SUBPART_DIR_HEADER            *DirectoryHeader;
  SUBPART_DIR_ENTRY             *DirectoryEntry;
  VOID                          *ObbDataPtr;
  UINT32                         ObbDataSize;

  PeiServicesGetBootMode (&BootMode);

  //
  // If the Hob exists, then GetBpdtPayloadAddress() has already been called
  // one or more times already, So we do not need to re-enter this flow.
  //
  GuidHobPtr = GetFirstGuidHob (&gEfiBpdtLibBp2DataGuid);
  if (GuidHobPtr != NULL) {
    DEBUG ((DEBUG_INFO, "GetFvNotifyCallback already called. Skipping.\n"));
    return Status;
  }

  //
  // Locate headers of both Boot partion 1 and 2
  //
  GetBootPartitionPointer (BootPart1, (VOID **)&Bp1HdrPtr);
  GetBootPartitionPointer (BootPart2, (VOID **)&Bp2HdrPtr);
  DEBUG ((DEBUG_INFO, "Signature BP1 = 0x%x BP2 = 0x%x\n",Bp1HdrPtr->Signature,Bp2HdrPtr->Signature));
  if (Bp1HdrPtr->Signature != BPDT_SIGN_GREEN || Bp2HdrPtr->Signature != BPDT_SIGN_GREEN) {
    DEBUG ((DEBUG_INFO, "FW Recovery needed. \n"));
  }

  //
  //  Get the OBB payload, shadow it, and check the hash before processing it.
  //
  Status = GetBpdtPayloadData (BootPart2, BpdtObb, &ObbPayloadPtr);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "ERROR: Failed to find OBB, Status = %r\n", Status));
    CpuDeadLoop();
  }
  DirectoryHeader = (SUBPART_DIR_HEADER *) ObbPayloadPtr->DataPtr;
  DirectoryEntry  = (SUBPART_DIR_ENTRY *) ((UINTN) DirectoryHeader + sizeof (SUBPART_DIR_HEADER));
  ObbDataPtr      = (VOID *) ((UINTN) DirectoryHeader + DirectoryEntry->EntryOffset);
  ObbDataSize     = ObbPayloadPtr->Size - 0x1000;

#if (BOOT_GUARD_ENABLE == 1)
  //
  // For Normal boot, just verify OBB, since CSE does hash verify of both IBBL and IBB.
  // IBBL check is done before bringing cores out of reset,
  // IBB check is done during RBP and indicated by IBB_VERIFICATION_DONE in IBBL
  //

  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = LocateAndVerifyHashBpm (HashObb);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Verify OBB failed, Status = %r\n", Status));
      CpuDeadLoop();
    }
  }
#endif

  DEBUG ((DEBUG_INFO, "GetFvNotifyCallback: Processing OBB Payload.\n"));

  ParseObbPayload ((UINT8*) ObbDataPtr, ObbDataSize, BootMode);

  //
  // Set Zephr ID
  //
  PcdSet32S (PcdIfwiZid, 0);
  DEBUG ((DEBUG_INFO, "ZID value = 0x%X\n", PcdGet32 (PcdIfwiZid)));

  return Status;
}

