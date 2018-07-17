/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DxeVtdLib.h>
#include <Private/Guid/ScPolicyHobGuid.h>
#include <Library/HobLib.h>
#include <Library/SteppingLib.h>
#include <Library/ConfigBlockLib.h>

/**
  For device that specified by Device Num and Function Num,
  mDevEnMap is used to check device presence.
  0x80 means use Device ID to determine presence

  The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {{0x0200, 0x80}, {0x1500, 0x80}, {0x1501, 0x80}};

/**
  Get the corresponding device Enable/Disable bit according DevNum and FunNum

  @param[in] DevNum             Device Number
  @param[in] FunNum             Function Number

  @retval    Bit                If the device is found, return Disable/Enable bit in FD/DevEn register
  @retval    0xFF               When device not found

**/
UINT16
GetFunDisableBit (
  IN UINT8    DevNum,
  IN UINT8    FunNum
  )
{
  UINTN  Index;

  for (Index = 0; Index < sizeof (mDevEnMap) / 4; Index++) {
    if (mDevEnMap[Index][0] == ((DevNum << 0x08) | FunNum)) {
      return mDevEnMap[Index][1];
    }
  }

  return 0xFF;
}


/**
  Update the DRHD structure

  @param[in, out] DrhdEnginePtr         A pointer to DRHD structure

**/
VOID
UpdateDrhd (
  IN OUT VOID    *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINT16                        DisableBit;
  UINTN                         DeviceScopeNum;
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  *DrhdEngine;

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE1_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE1_STRUCT *) DrhdEnginePtr;
  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);

  DisableBit = GetFunDisableBit (
                 DrhdEngine->DeviceScope[0].PciPath[0],
                 DrhdEngine->DeviceScope[0].PciPath[1]
                 );

  NeedRemove = FALSE;

  if ((DisableBit == 0xFF)                   ||
      (DrhdEngine->RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80)                  &&
      (MmioRead32 (MmPciBase (0, DrhdEngine->DeviceScope[0].PciPath[0], DrhdEngine->DeviceScope[0].PciPath[1]) + 0x00) == 0xFFFFFFFF))) {
    NeedRemove = TRUE;
  }

  if (NeedRemove) {
    Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  }

  //
  // If no device scope is left, we set the structure length as 0x00
  //
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }

  return;
}


/**
  Get IOAPIC ID from LPC.

  @retval UINT8                  IO APIC ID

**/
UINT8
GetIoApicId (
  VOID
  )
{
  UINT32  IoApicId;
  UINTN   P2sbMmbase;

  P2sbMmbase = MmPciAddress(
                 0,
                 DEFAULT_PCI_BUS_NUMBER_SC,
                 PCI_DEVICE_NUMBER_P2SB,
                 PCI_FUNCTION_NUMBER_P2SB,
                 0
                 );


  MmioOr16 (P2sbMmbase + R_P2SB_IOAC, (UINT16) (B_P2SB_IOAC_AE));
  //
  // Reads back for posted write to take effect and make sure it is set properly.
  //
  if ((MmioRead16 (P2sbMmbase + R_P2SB_IOAC) & (UINT16) B_P2SB_IOAC_AE) == (UINT16) 0x00) {
    DEBUG ((EFI_D_INFO, " IoApicId B_P2SB_IOAC_AE:0 Skip \n"));
    return 0;
  }

  //
  // Get current IO APIC ID
  //
  MmioWrite8 ((UINTN) IO_APIC_INDEX_REGISTER, R_IO_APIC_ID);
  IoApicId = MmioRead32 ((UINTN) IO_APIC_DATA_REGISTER) >> 24;

  return (UINT8) IoApicId;
}


/**
  Update the second DRHD structure

  @param[in, out] DrhdEnginePtr         A pointer to DRHD structure

**/
VOID
UpdateDrhd2 (
  IN OUT VOID *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINTN                         DeviceScopeNum;
  UINTN                         ValidDeviceScopeNum;
  UINT16                        Data16;
  UINT16                        Index;
  UINT8                         Bus;
  UINT8                         Path[2] = { 0, 0 };
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  *DrhdEngine;
  UINTN                         P2sbMmbase;

  P2sbMmbase = MmPciAddress(
                 0,
                 DEFAULT_PCI_BUS_NUMBER_SC,
                 PCI_DEVICE_NUMBER_P2SB,
                 PCI_FUNCTION_NUMBER_P2SB,
                 0
                 );

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE2_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE2_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  Data16          = 0;
  Bus             = 0;
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    //
    //  For HPET and APIC, update device scope if Interrupt remapping is supported. remove device scope
    //  if interrupt remapping is not supported.
    //  - Index = 0 - IOAPIC
    //  - Index = 1 - HPET
    //
    if ((MmioRead32 (PcdGet32 (PcdVtdGfxBaseAddress) + VTD_ECAP_REG) & IR)
         && (MmioRead32 (DEF_VTD_BASE_ADDRESS + VTD_ECAP_REG) & IR)
         && ((GetBxtSeries() == BxtP && BxtStepping() > BxtPA0) || GetBxtSeries() == Bxt1)) {
      if (Index == 0) {
        //
        // Update source id for IoApic's device scope entry
        //
        Data16 = MmioRead16 (P2sbMmbase + R_PCH_P2SB_IBDF);
        Bus     = (UINT8) (Data16 >> 8);

        if (Bus != 0x00) {
          Path[0] = (UINT8) ((Data16 & 0xff) >> 3);
          Path[1] = (UINT8) (Data16 & 0x7);
        } else {
          Bus     = 0xF0;
          Path[0] = 0x1F;
          Path[1] = 0x0;
        }

        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
        DrhdEngine->DeviceScope[Index].PciPath[0] = Path[0];
        DrhdEngine->DeviceScope[Index].PciPath[1] = Path[1];

        //
        // Update APIC ID
        //
        DrhdEngine->DeviceScope[Index].EnumId = GetIoApicId();
        DEBUG ((EFI_D_INFO, " VTd check IoApicId : 0x%x\n", GetIoApicId()));
      }

      if (Index == 1) {
        //
        // Update source id for HPET's device scope entry
        //
        Data16 = MmioRead16 (P2sbMmbase + R_PCH_P2SB_HBDF);
        Bus     = (UINT8) (Data16 >> 8);
        Path[0] = (UINT8) ((Data16 & 0xFF) >> 3);
        Path[1] = (UINT8) (Data16 & 0x7);

        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
        DrhdEngine->DeviceScope[Index].PciPath[0] = Path[0];
        DrhdEngine->DeviceScope[Index].PciPath[1] = Path[1];
      }
    } else {
      if ((Index == 0) || (Index == 1)) {
        NeedRemove = TRUE;
      }
    }

    CopyMem (
      &DrhdEngine->DeviceScope[ValidDeviceScopeNum],
      &DrhdEngine->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  //
  // If no device scope is left, we set the structure length as 0x00
  //
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }
}


/**
  Update the RMRR structure

  @param[in, out] RmrrPtr               A pointer to RMRR structure

**/
VOID
UpdateRmrr (
  IN OUT VOID    *RmrrPtr
  )
{
  UINT16                  Length;
  UINT16                  DisableBit;
  UINTN                   DeviceScopeNum;
  UINTN                   ValidDeviceScopeNum;
  UINTN                   Index;
  BOOLEAN                 NeedRemove;
  EFI_ACPI_RMRR_USB_STRUC *Rmrr;

  //
  // To make sure all device scope can be checked,
  // we convert the RmrrPtr to EFI_ACPI_RMRR_USB_STRUC pointer
  //
  Rmrr                = (EFI_ACPI_RMRR_USB_STRUC *) RmrrPtr;
  Length              = Rmrr->Length;
  ValidDeviceScopeNum = 0;
  DeviceScopeNum      = (Rmrr->Length - EFI_ACPI_RMRR_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    DisableBit = GetFunDisableBit (
                   Rmrr->DeviceScope[Index].PciPath[0],
                   Rmrr->DeviceScope[Index].PciPath[1]
                   );
    NeedRemove = FALSE;

    if ((DisableBit == 0xFF) ||
        ((DisableBit == 0x80) &&
        (MmioRead32 (MmPciBase (0, Rmrr->DeviceScope[Index].PciPath[0], Rmrr->DeviceScope[Index].PciPath[1]) + 0x00) == 0xFFFFFFFF))) {
      NeedRemove = TRUE;
    }

    CopyMem (
      &Rmrr->DeviceScope[ValidDeviceScopeNum],
      &Rmrr->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  //
  // If No deviceScope is left, set length as 0x00
  //
  if (Length > EFI_ACPI_RMRR_HEADER_LENGTH) {
    Rmrr->Length = Length;
  } else {
    Rmrr->Length = 0;
  }
}


/**
  Update the DMAR table

  @param[in, out] TableHeader           The table to be set
  @param[in, out] Version               Version to publish

**/
VOID
DmarTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER       *TableHeader,
  IN OUT EFI_ACPI_TABLE_VERSION            *Version
  )
{
  EFI_ACPI_DMAR_TABLE         *DmarTable;
  EFI_ACPI_DMAR_TABLE         TempDmarTable;
  UINTN                       Offset;
  UINTN                       StructureLen;
  UINTN                       McD0BaseAddress;
  UINTN                       MchBar;
  UINT16                      IgdMode;
  UINT16                      GttMode;
  UINT32                      IgdMemSize;
  UINT32                      GttMemSize;
  SC_POLICY_HOB               *ScPolicy;
  EFI_PEI_HOB_POINTERS        HobPtr;
  SC_VTD_CONFIG               *VtdConfig;
  EFI_STATUS                  Status;

  //
  // Get SC VT-d config block
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  ScPolicy = (SC_POLICY_HOB *) GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "ScPolicy VtdEnable = %x\n", VtdConfig->VtdEnable));

  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) TableHeader;

  DEBUG ((DEBUG_INFO, "DmarTableUpdate() - start\n"));

  //
  // Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  //
  if ((MmioRead32 (PcdGet32 (PcdVtdGfxBaseAddress) + VTD_ECAP_REG) & IR)
         && (MmioRead32 (DEF_VTD_BASE_ADDRESS + VTD_ECAP_REG) & IR)
         && ((GetBxtSeries() == BxtP && BxtStepping() > BxtPA0) || GetBxtSeries() == Bxt1)) {
    DmarTable->Flags |= BIT0;
  }

  if (VtdConfig->x2ApicEnabled == 1) {
    DmarTable->Flags |= BIT1;
  } else {
    DmarTable->Flags &= 0xFD;
  }

  //
  // Calculate IGD mem size
  //
  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
  MchBar          = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR) & ~BIT0;
  IgdMode = (MmioRead16 (McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GMS_MASK) >> N_SA_GGC_GMS_OFFSET;

  if (IgdMode < V_SA_GGC_GMS_2016MB) {
    IgdMemSize = IgdMode * 32 * (1024) * (1024);
  } else {
    IgdMemSize = 0;
  }

  //
  // Calculate GTT mem size
  //
  GttMemSize = 0;
  GttMode = (MmioRead16 (McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GGMS_MASK) >> N_SA_GGC_GGMS_OFFSET;

  if (GttMode <= V_SA_GGC_GGMS_8MB) {
    GttMemSize = (1 << GttMode) * (1024) * (1024);
  }

  DmarTable->RmrrIgd.RmrBaseAddress   = (MmioRead32 (McD0BaseAddress + R_SA_TOLUD) & ~(0x01)) - IgdMemSize - GttMemSize;
  DmarTable->RmrrIgd.RmrLimitAddress  = DmarTable->RmrrIgd.RmrBaseAddress + IgdMemSize + GttMemSize - 1;

  DEBUG ((DEBUG_INFO, "RMRR Base  address IGD %016lX\n", DmarTable->RmrrIgd.RmrBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address IGD %016lX\n", DmarTable->RmrrIgd.RmrLimitAddress));

  DmarTable->RmrrUsb.RmrBaseAddress  = VtdConfig->RmrrUsbBaseAddr;
  DmarTable->RmrrUsb.RmrLimitAddress = VtdConfig->RmrrUsbLimit;

  //
  // Convert to 4KB alignment.
  //
  DmarTable->RmrrUsb.RmrBaseAddress  &= (EFI_PHYSICAL_ADDRESS) ~0xFFF;
  DmarTable->RmrrUsb.RmrLimitAddress &= (EFI_PHYSICAL_ADDRESS) ~0xFFF;
  DmarTable->RmrrUsb.RmrLimitAddress += 0x1000 - 1;

  DEBUG ((DEBUG_INFO, "RMRR Base  address USB %016lX\n", DmarTable->RmrrUsb.RmrBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address USB %016lX\n", DmarTable->RmrrUsb.RmrLimitAddress));


  //
  // Update DRHD structures of DmarTable
  //
  DmarTable->DrhdEngine1.RegisterBaseAddress = (MmioRead32 (MchBar + R_SA_MCHBAR_VTD1_OFFSET) &~3);
  DmarTable->DrhdEngine2.RegisterBaseAddress = (MmioRead32 (MchBar + R_SA_MCHBAR_VTD2_OFFSET) &~3);

  DEBUG ((DEBUG_INFO, "VTD base address1 %x\n", DmarTable->DrhdEngine1.RegisterBaseAddress));
  DEBUG ((DEBUG_INFO, "VTD base address2 %x\n", DmarTable->DrhdEngine2.RegisterBaseAddress));

  //
  // Copy DmarTable to TempDmarTable to be processed
  //
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  //
  // Update DRHD structures of temp DMAR table
  //
  UpdateDrhd (&TempDmarTable.DrhdEngine1);
  UpdateDrhd2 (&TempDmarTable.DrhdEngine2);

  //
  // Update RMRR structures of temp DMAR table
  //
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrUsb);
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrIgd);

  //
  // Remove unused device scope or entire DRHD structures
  //
  Offset = (UINTN) (&TempDmarTable.DrhdEngine1);
  if (TempDmarTable.DrhdEngine1.Length != 0) {
    Offset += TempDmarTable.DrhdEngine1.Length;
  }

  if (TempDmarTable.DrhdEngine2.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine2.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine2, TempDmarTable.DrhdEngine2.Length);
    Offset += StructureLen;
  }

  //
  // Remove unused device scope or entire RMRR structures
  //
  if (TempDmarTable.RmrrUsb.Length != 0) {
    StructureLen = TempDmarTable.RmrrUsb.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrUsb, TempDmarTable.RmrrUsb.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.RmrrIgd.Length != 0) {
    StructureLen = TempDmarTable.RmrrIgd.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrIgd, TempDmarTable.RmrrIgd.Length);
    Offset += StructureLen;
  }

  Offset = Offset - (UINTN) &TempDmarTable;

  //
  // Re-calculate DMAR table check sum
  //
  TempDmarTable.Header.Checksum = (UINT8) (TempDmarTable.Header.Checksum + TempDmarTable.Header.Length - Offset);

  //
  // Set DMAR table length
  //
  TempDmarTable.Header.Length = (UINT32) Offset;

  //
  // Replace DMAR table with rebuilt table TempDmarTable
  //
  CopyMem ((VOID *) DmarTable, (VOID *) &TempDmarTable, TempDmarTable.Header.Length);

  DEBUG ((DEBUG_INFO, "DmarTableUpdate()  - end\n"));
}


/**
  ReadyToBoot callback routine to update DMAR

**/
VOID
UpdateDmarOnReadyToBoot (
  BOOLEAN   VtEnable
  )
{
  EFI_ACPI_TABLE_PROTOCOL         *AcpiTableProtocol;
  EFI_ACPI_TABLE_VERSION          Version;
  EFI_STATUS                      Status;
  UINT16                          Index;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  EFI_FV_FILETYPE                 FileType;
  UINT32                          FvStatus;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINTN                           Size;
  INTN                            Instance;
  EFI_ACPI_COMMON_HEADER          *CurrentTable;
  UINTN                           AcpiTableHandle;
  EFI_FIRMWARE_VOLUME2_PROTOCOL   *FwVol;
  EFI_ACPI_DESCRIPTION_HEADER     *DmarAcpiTable;
  SC_POLICY_HOB                   *ScPolicy;
  EFI_PEI_HOB_POINTERS            HobPtr;
  SC_VTD_CONFIG                   *VtdConfig;

  AcpiTableProtocol = NULL;
  DmarAcpiTable = NULL;
  Index       = 0;

  //
  // Get SC VT-d config block
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  ScPolicy = (SC_POLICY_HOB*) GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "ScPolicy PrebootVTdEnable = %x\n", VtdConfig->PrebootVTdEnable));

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locate protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Looking for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gAcpiDmarTableFileGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }
  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gAcpiDmarTableFileGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the Signature ID to modify the table
      ///
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Signature) {

        case EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE:
          DmarAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          DmarTableUpdate (DmarAcpiTable, &Version);
          break;

        default:
          break;
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }
  ///
  /// Update the VTD table in the ACPI tables.
  ///
  AcpiTableHandle = 0;
  if (DmarAcpiTable != NULL) {
    DEBUG ((DEBUG_INFO, "Installing DMAR ACPI table.\n"));
    Status = AcpiTableProtocol->InstallAcpiTable (
                          AcpiTableProtocol,
                          DmarAcpiTable,
                          DmarAcpiTable->Length,
                          &AcpiTableHandle
                          );
    ASSERT_EFI_ERROR (Status);
  }


  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "DMAR ACPI table has been successfully installed and updated.\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "Error updating the DMAR ACPI table\n"));
  }

  //
  // Only notify IntelVtdDxe driver to setup pre-boot VT-d if pre-boot VT-d has been enabled by BIOS Setup option.
  //
  if ((BOOLEAN)(VtdConfig->PrebootVTdEnable) == TRUE) {
    DEBUG ((DEBUG_INFO, "VT-d signal gEfiAcpi10TableGuid.\n"));
    EfiEventGroupSignal (&gEfiAcpi10TableGuid);
  }
}


/**
  Update RMRR Base and Limit Address for USB.

**/
VOID
UpdateRmrrUsbAddress (
  IN SC_VTD_CONFIG        *VtdConfig
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    Address;
  UINTN                   Size;

  if (VtdConfig->RmrrUsbBaseAddr != 0) {
    //
    // The buffer did assign.
    //
    return;
  }

  Size = EFI_SIZE_TO_PAGES(VTD_RMRR_USB_LENGTH);
  Address = 0xFFFFFFFF;

  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  Size,
                  &Address
                  );
  ASSERT_EFI_ERROR (Status);


  VtdConfig->RmrrUsbBaseAddr = Address;
  VtdConfig->RmrrUsbLimit = Address + VTD_RMRR_USB_LENGTH - 1;

  return;
}


/**
  Notification function of ACPI Table change.

  This is a notification function registered on ACPI Table change event.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
PciEnumerationCompleteCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS          Status;
  SC_POLICY_HOB         *ScPolicy;
  EFI_PEI_HOB_POINTERS  HobPtr;
  SC_VTD_CONFIG         *VtdConfig;

  gBS->CloseEvent (Event);

  //
  // Get SC VT-d config block
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  ScPolicy = (SC_POLICY_HOB*) GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gVtdConfigGuid, (VOID *) &VtdConfig);

  if ((BOOLEAN)(UINT8)(VtdConfig->VtdEnable) == TRUE) {
    UpdateDmarOnReadyToBoot ((BOOLEAN)(UINT8)(VtdConfig->VtdEnable));
  }
  
}

VOID
InstallAcpiDmarTable (
  BOOLEAN VtdEnable
)
{

  VOID        *CallbackNotifyReg;
  EFI_EVENT   CallbackEvent;
  EFI_STATUS  Status;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PciEnumerationCompleteCallback,
                  NULL,
                  &CallbackEvent
                  );

  ASSERT_EFI_ERROR (Status);


  Status = gBS->RegisterProtocolNotify (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  CallbackEvent,
                  &CallbackNotifyReg
                  );
  ASSERT_EFI_ERROR (Status);


  return;

}


/**
  Locate the VT-d ACPI tables data file and update it based on current configuration and capabilities.

  @retval  EFI_SUCCESS        VT-d initialization complete
  @retval  EFI_UNSUPPORTED    VT-d is disabled by policy or not supported

**/
EFI_STATUS
VtdInit (
  VOID
  )
{
  UINTN                 McD0BaseAddress;
  SC_POLICY_HOB         *ScPolicy;
  EFI_PEI_HOB_POINTERS  HobPtr;
  SC_VTD_CONFIG         *VtdConfig;
  EFI_STATUS            Status;

  DEBUG ((DEBUG_INFO, "VtdInit () - Start\n"));
  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);

  //
  // Get SC VT-d config block
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  ScPolicy = (SC_POLICY_HOB*) GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "ScPolicy VtdEnable = %x\n", VtdConfig->VtdEnable));

  if ((VtdConfig->VtdEnable == 0) || (MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT23)) {
    DEBUG ((DEBUG_INFO, "VT-d Disabled or not supported on this platform, skip DMAR Table update\n"));
    return EFI_UNSUPPORTED;
  }

  if (((AsmReadMsr64 (EFI_MSR_XAPIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10) {
    VtdConfig->x2ApicEnabled = 1;
  } else {
    VtdConfig->x2ApicEnabled = 0;
  }

  //
  // Update RMRR USB address
  //
  UpdateRmrrUsbAddress (VtdConfig);

  //
  // Register callback function for updating DMAR table.
  //
  InstallAcpiDmarTable ((BOOLEAN)(VtdConfig->VtdEnable));
  
  DEBUG ((DEBUG_INFO, "VtdInit () - End\n"));

  return EFI_SUCCESS;
}

