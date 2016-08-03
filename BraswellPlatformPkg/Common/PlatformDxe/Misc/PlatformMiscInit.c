/** @file
  Platform Initialization Driver.

  Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include "Platform.h"
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/ExitPmAuth.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Guid/EventGroup.h>
#include <Guid/Chv2Variable.h>
#include <Library/PcdLib.h>
#include <Include/Library/BiosIdLib.h>
#include <Protocol/IgdOpRegion.h>
#include <KscLib.h>
#include <Library/GpioLib.h>
#include <SaveMemoryConfig.h>

#define EFI_MEMORY_PORT_IO                0x4000000000000000ULL

#define WC_PMIC_READ_INSTRUCTION_DELAY 10
#define WC_PMIC_WRITE_INSTRUCTION_DELAY 100

SYSTEM_CONFIGURATION            mSystemConfiguration;
EFI_PLATFORM_INFO_HOB           mPlatformInfo;
EFI_EVENT                       mReadyToBootEvent;
UINTN                           SpiBase;

typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
} MEMORY_MAP;



VOID
InitExI ();

VOID
PchInitBeforeBoot (
  );



VOID
InitRC6Policy (
  VOID
  );

UINT8
GetUsbIdDetectionStatus (
  );

VOID
InitPlatformResolution (
  VOID
  );

VOID
PlatformGcdInfoReport (
  VOID
  );


#define PchLpcPciCfg32(Register)  MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, Register))

/**
  This function read content of B:D:F 0:31:0, offset 44h (for PmcBase)

  @param[in] None

  @return  32 bit PmcBase

**/
UINT32
GetPmcBase (
  VOID
  )
{
  return (PchLpcPciCfg32 (R_PCH_LPC_PMC_BASE) & B_PCH_LPC_PMC_BASE_BAR);
}

/**
  Configure ExI.

  @param    None

  @retval   EFI_SUCCESS        The driver initializes correctly.

**/
VOID
InitExI (
  )
{
  SYSTEM_CONFIGURATION          SystemConfiguration;

  CopyMem (&SystemConfiguration, PcdGetPtr (PcdSystemConfiguration), sizeof(SYSTEM_CONFIGURATION));
  if (SystemConfiguration.ExISupport == 1) {
    MmioOr32 ((UINTN) (GetPmcBase() + R_PCH_PMC_MTPMC1), (UINT32) BIT0+BIT1+BIT2);
  } else if (SystemConfiguration.ExISupport == 0) {
    MmioAnd32 ((UINTN) (GetPmcBase() + R_PCH_PMC_MTPMC1), ~((UINT32) BIT0+BIT1+BIT2)); //clear bit 0,1,2
  }
}

VOID
SpiBiosProtectionFunction (
  EFI_EVENT Event,
  VOID      *Context
  )
{
  UINTN                 mPciD31F0RegBase;
  UINTN                 BiosFlaLower = 0;
  UINTN                 BiosFlaLimit = 0x7fffff;

  gBS->CloseEvent (Event);

  BiosFlaLower = PcdGet32 (PcdFlashMicroCodeAddress)- PcdGet32 (PcdFlashAreaBaseAddress);

  mPciD31F0RegBase = MmPciAddress (0,
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_LPC,
                       0
                       );
  SpiBase          = MmioRead32(mPciD31F0RegBase + R_PCH_LPC_SPI_BASE) & B_PCH_LPC_SPI_BASE_BAR;

  //
  //Set SMM_BWP, WPD and LE bit
  //
  MmioOr32 ((UINTN) (SpiBase + R_PCH_SPI_BCR), (UINT8) B_PCH_SPI_BCR_SMM_BWP);
  MmioAnd32 ((UINTN) (SpiBase + R_PCH_SPI_BCR), (UINT8)(~B_PCH_SPI_BCR_BIOSWE));
  MmioOr32 ((UINTN) (SpiBase + R_PCH_SPI_BCR), (UINT8) B_PCH_SPI_BCR_BLE);

  //
  //First check if FLOCKDN or PR0FLOCKDN is set. No action if either of them set already.
  //
  if ((MmioRead16(SpiBase + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) != 0 ||
      (MmioRead32(SpiBase + R_PCH_SPI_LOCK)& B_PCH_SPI_LOCK_PR0LOCKDN) != 0) {
    //
    // Already locked. we could take no action here
    //
    DEBUG((EFI_D_INFO, "PR0 already locked down. Stop configuring PR0.\n"));
    return;
  }

  //
  //Set PR0
  //
  MmioOr32((UINTN)(SpiBase + R_PCH_SPI_PR0),
    B_PCH_SPI_PR0_RPE|B_PCH_SPI_PR0_WPE|\
    (B_PCH_SPI_PR0_PRB_MASK&(BiosFlaLower>>12))|(B_PCH_SPI_PR0_PRL_MASK&(BiosFlaLimit>>12)<<16));
  //
  //Lock down PR0
  //
  MmioOr16 ((UINTN) (SpiBase + R_PCH_SPI_HSFS), (UINT16) (B_PCH_SPI_HSFS_FLOCKDN));

  //
  // Verify if it's really locked.
  //
  if ((MmioRead16 (SpiBase + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) == 0) {
    DEBUG((EFI_D_ERROR, "Failed to lock down PR0.\n"));
  }

  return;
}

VOID
InitPciDevPME (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  gBS->CloseEvent (Event);

  CopyMem (&mSystemConfiguration, PcdGetPtr (PcdSystemConfiguration), sizeof(SYSTEM_CONFIGURATION));

  //
  // Program HDA PME_EN
  //
  if (MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_AZALIA, PCI_FUNCTION_NUMBER_PCH_AZALIA, R_PCH_HDA_DEVVENID)) != 0xFFFFFFFF) {
    PchAzaliaPciCfg32Or (R_PCH_HDA_PCS, B_PCH_HDA_PCS_PMEE);
  }

#ifdef SATA_SUPPORT
  //
  // Program SATA PME_EN
  //
  if (MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA, R_PCH_SATA_ID)) != 0xFFFFFFFF) {
    PchSataPciCfg32Or (R_PCH_SATA_PMCS, B_PCH_SATA_PMCS_PMEE);
  }
#endif
}


// JYAO1 - move to S3
VOID
PchInitBeforeBoot()
{
  //
  // Saved SPI Opcode menu to fix EFI variable unable to write after S3 resume.
  //
  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (SPI_BASE_ADDRESS + (R_PCH_SPI_OPMENU0)),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + (R_PCH_SPI_OPMENU0))
    );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (SPI_BASE_ADDRESS + (R_PCH_SPI_OPMENU1)),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + (R_PCH_SPI_OPMENU1))
    );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint16,
    (UINTN) (SPI_BASE_ADDRESS + R_PCH_SPI_OPTYPE),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + R_PCH_SPI_OPTYPE)
    );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint16,
    (UINTN) (SPI_BASE_ADDRESS + R_PCH_SPI_PREOP),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + R_PCH_SPI_PREOP)
    );

  //
  // Saved MTPMC_1 for S3 resume.
  //
  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN)(PMC_BASE_ADDRESS + R_PCH_PMC_MTPMC1),
    1,
    (VOID *)(UINTN)(PMC_BASE_ADDRESS + R_PCH_PMC_MTPMC1));

  return;
}


VOID
InitRC6Policy (
  VOID
  )
{
  UINTN  Size;

  if ((mSystemConfiguration.S0ix==1) && (mSystemConfiguration.EnableRenderStandby==0)) {
    mSystemConfiguration.EnableRenderStandby=1;
    Size = sizeof(SYSTEM_CONFIGURATION);
    PcdSetPtr (PcdSystemConfiguration, &Size, &mSystemConfiguration);
  }
}

VOID
InitPlatformResolution (
  VOID
  )
{
  PANEL_RESOLUTION PanelResolution[] = {
    0, 0,     0,     //Natvie Mode, Find the highest resolution which GOP supports.
    1, 640,   480,
    2, 800,   600,
    3, 1024,  768,
    4, 1280,  1024,
    5, 1366,  768,
    6, 1680,  1050,
    7, 1920,  1200,
    8, 1280,  800,
  };

  //
  // Set the Panel Resolution.
  //
  PcdSet32 (PcdVideoHorizontalResolution, PanelResolution[mSystemConfiguration.IgdFlatPanel].HorizontalResolution);
  PcdSet32 (PcdVideoVerticalResolution, PanelResolution[mSystemConfiguration.IgdFlatPanel].VerticalResolution);

}

VOID
PlatformGcdInfoReport (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PHYSICAL_ADDRESS            BaseAddress;
  UINT64                          Length;
  UINT8                           Index;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdMemorySpaceDescriptor;
  UINT64                          Attributes;

  //
  // Base address and length must exist and match
  // the entries in MemoryCallback.c
  //
  MEMORY_MAP                      MmioMap[] = {
    {PCIEX_BASE_ADDRESS,      0x4000000},
    {IO_APIC_ADDRESS,         0x1000},
    {SPI_BASE_ADDRESS,        0x1000},
    {PMC_BASE_ADDRESS,        0x1000},
    {IO_BASE_ADDRESS,         0x40000},
    {ILB_BASE_ADDRESS,        0x2000},
    {MPHY_BASE_ADDRESS,       0x100000},
    {PUNIT_BASE_ADDRESS,      0x1000},
    {RCBA_BASE_ADDRESS,       0x1000},
    {LOCAL_APIC_ADDRESS,      0x1000}
  };

  for (Index = 0; Index < sizeof (MmioMap) / (sizeof (MEMORY_MAP)); Index++) {
    BaseAddress = MmioMap[Index].BaseAddress;
    Length      = MmioMap[Index].Length;
    DEBUG ((EFI_D_ERROR, "BaseAddress = 0x%X\n", BaseAddress));
    Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &GcdMemorySpaceDescriptor);
    ASSERT_EFI_ERROR (Status);

    Attributes = GcdMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;
    if (MmioMap[Index].BaseAddress == PCIEX_BASE_ADDRESS) {
      Attributes |= EFI_MEMORY_PORT_IO;
    }

    if (MmioMap[Index].BaseAddress == PMC_BASE_ADDRESS) {
      Attributes |= EFI_MEMORY_UC;
    }

    Status = gDS->SetMemorySpaceAttributes (
                    BaseAddress,
                    Length,
                    Attributes
                    );
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  This is the standard EFI driver point for the Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param[in]  ImageHandle     Handle for the image of this driver.
  @param[in]  SystemTable     Pointer to the EFI System Table.

  @retval   EFI_SUCCESS       Policy decisions set.

**/
EFI_STATUS
EFIAPI
InitializePlatform (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          Handle;
  UINT8                               Buffer8;

  DEBUG ((EFI_D_INFO, "InitializePlatform\n"));

  Handle        = NULL;
  Buffer8       = 0;

  ReportStatusCodeEx (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_PLATFORM_DXE_INIT,
    0,
    &gEfiCallerIdGuid,
    NULL,
    NULL,
    0
    );

  CopyMem (&mPlatformInfo, PcdGetPtr (PcdPlatformInfo), sizeof(mPlatformInfo));
  //
  // PANEL0_BACKLIGHT_CTL (GPO) required to toggle to make HSIC Hub functional in Warm reset flow
  //
  if (mPlatformInfo.WarmResetOccured) {
    GPIOSetTXValue(CFIO_COMMUNITY_NORTH,65,LOW);
    DEBUG ((EFI_D_INFO, "HSIC: Programming Low \n"));
  }

  //
  // Check for valid date
  //
  CheckValidDate ();

  CopyMem (&mSystemConfiguration, PcdGetPtr (PcdSystemConfiguration), sizeof(SYSTEM_CONFIGURATION));

  //
  // Detect the Manufacture Mode
  //
  DEBUG ((EFI_D_ERROR, "SystemConfiguration.MfgMode= %d \n",mSystemConfiguration.MfgMode));

  //
  // RC6 should be enabled if S0ix is enabled.
  //
  InitRC6Policy ();
  //
  // Create a ReadyToBoot Event to run the PME init process
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             InitPciDevPME,
             NULL,
             &mReadyToBootEvent
             );
  //
  // Create a ReadyToBoot Event to run enable PR0 and lock down
  //
  if (mSystemConfiguration.PchBiosLock == 1) {
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               SpiBiosProtectionFunction,
               NULL,
               &mReadyToBootEvent
               );
  }

  InitPlatformResolution ();

  //
  // Init ExI
  //
  InitExI ();

  PlatformGcdInfoReport ();

  //
  // Process dynamic entries
  //

  PchInitBeforeBoot ();

  ReportStatusCodeEx (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_PLATFORM_DXE_INIT_DONE,
    0,
    &gEfiCallerIdGuid,
    NULL,
    NULL,
    0
    );

  //
  // PANEL0_BACKLIGHT_CTL (GPO) required to toggle to make HSIC Hub functional in Warm reset flow
  //
  if (mPlatformInfo.WarmResetOccured) {
    GPIOSetTXValue (CFIO_COMMUNITY_NORTH,65,HIGH);
    DEBUG ((EFI_D_INFO, "HSIC: Programming high \n"));
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
SiliconInitDoneCallback (
  IN EFI_EVENT                      Event,
  IN VOID                           *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Protocol;

  Status = gBS->LocateProtocol (
                  &gIgdOpRegionProtocolGuid,
                  NULL,
                  &Protocol
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  InitializePlatform (gImageHandle, gST);
}

