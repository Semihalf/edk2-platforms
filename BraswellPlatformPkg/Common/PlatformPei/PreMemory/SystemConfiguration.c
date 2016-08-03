/** @file
  This PEIM initialize platform for MRC, following action is performed,
    1. Initialize GMCH
    2. Detect boot mode
    3. Detect video adapter to determine whether we need pre-allocated memory
    4. Calls MRC to initialize memory and install a PPI notify to do post memory initialization.
  This file contains the main entry point of the PEIM.

  Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "PlatformPei.h"
#include "PlatformBaseAddresses.h"
#include "PchRegs.h"
#include <Guid/PlatformInfo.h>
#include "ChvAccess.h"
#include <Library/GpioLib.h>
#include <Guid/Chv2Variable.h>
#include <Ppi/ChvPlatformPolicyPpi.h>
#include <Ppi/BoardDetection.h>
#include <Guid/SetupVariable.h>
#include "SetupVariableDefault.h"

EFI_STATUS
GetSetupVariable (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN   SYSTEM_CONFIGURATION          *SystemConfiguration
  )
{
  UINTN                        VariableSize;
  EFI_STATUS                   Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  ZeroMem (SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));

  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gEfiPeiReadOnlyVariable2PpiGuid,
                                      0,
                                      NULL,
                                      &Variable
                                      );
  ASSERT_EFI_ERROR (Status);

  //
  // Use normal setup default from NVRAM variable,
  // the Platform Mode (manufacturing/safe/normal) is handle in PeiGetVariable.
  //
  VariableSize = sizeof(SYSTEM_CONFIGURATION);
  Status = Variable->GetVariable (Variable,
                                   L"Setup",
                                   &gEfiSetupVariableGuid,
                                   NULL,
                                   &VariableSize,
                                   SystemConfiguration);
  return Status;
}

EFI_STATUS
GetSystemConfiguration (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  UINTN                            Size;
  SYSTEM_CONFIGURATION             SystemConfiguration;

  //
  // Get setup variable. This can only be done after BootMode is updated
  //
  Status = GetSetupVariable (PeiServices, &SystemConfiguration);
  Size = sizeof(SYSTEM_CONFIGURATION);

  if(EFI_ERROR(Status)){
    //
    // For OC solution, there is no setup variable, so set the SystemConfiguration with default value.
    //
    CopyMem(&SystemConfiguration, &gDefaultSystemConfiguration,Size);
    //
    // Update the values according to PCD settings
    //

    //
    // Platform
    //
    SystemConfiguration.TurboModeEnable = PcdGet8(PcdTurboMode);

    //
    // South Complex
    //
    SystemConfiguration.SccSdcardEnabled = PcdGet8(PcdSdcardMode);
    SystemConfiguration.LpssHsuart0Enabled = PcdGet8(PcdEnableHsuart0);
    SystemConfiguration.LpssHsuart1Enabled = PcdGet8(PcdEnableHsuart1);
    SystemConfiguration.PchAzalia = PcdGet8(PcdEnableAzalia);
    SystemConfiguration.PchSata = PcdGet8(PcdEnableSata);
    SystemConfiguration.PchUsb30Mode = PcdGet8(PcdEnableXhci);
    SystemConfiguration.PchLpeEnabled = PcdGet8(PcdEnableLpe);
    SystemConfiguration.LpssDma0Enabled = PcdGet8(PcdEnableDma0);
    SystemConfiguration.LpssDma1Enabled = PcdGet8(PcdEnableDma1);
    SystemConfiguration.LpssI2C0Enabled = PcdGet8(PcdEnableI2C0);
    SystemConfiguration.LpssI2C1Enabled = PcdGet8(PcdEnableI2C1);
    SystemConfiguration.LpssI2C2Enabled = PcdGet8(PcdEnableI2C2);
    SystemConfiguration.LpssI2C3Enabled = PcdGet8(PcdEnableI2C3);
    SystemConfiguration.LpssI2C4Enabled = PcdGet8(PcdEnableI2C4);
    SystemConfiguration.LpssI2C5Enabled = PcdGet8(PcdEnableI2C5);
    SystemConfiguration.LpssI2C6Enabled = PcdGet8(PcdEnableI2C6);
    SystemConfiguration.ScceMMCEnabled = PcdGet8(PcdEmmcMode);
    SystemConfiguration.SataInterfaceSpeed = PcdGet8(PcdSataInterfaceSpeed);
    SystemConfiguration.ISPEn = PcdGet8(ISPEnable);
    SystemConfiguration.ISPDevSel = PcdGet8(ISPPciDevConfig);
    SystemConfiguration.PchSata = PcdGet8(PcdEnableSata);
    SystemConfiguration.MrcDvfsEnable = PcdGet8(PcdDvfsEnable);
    SystemConfiguration.PnpSettings = PcdGet8(PcdPnpSettings);

    //
    // North Complex
    //
    SystemConfiguration.GTTSize = PcdGet8(PcdGttSize);
    SystemConfiguration.IgdApertureSize = PcdGet8(PcdApertureSize);
    SystemConfiguration.IgdDvmt50PreAlloc = PcdGet8(PcdIgdDvmt50PreAlloc);
    SystemConfiguration.TSEGSizeSel = (UINT8)PcdGet16(PcdMrcInitTsegSize);
    DEBUG ((EFI_D_INFO, "PeiInitPlatform(): GetSetupVariable returns EFI_NOT_FOUND!! \n"));
  }

  PcdSetPtr (PcdSystemConfiguration, &Size, &SystemConfiguration);
  DEBUG ((EFI_D_INFO, "PcdSystemConfiguration size - 0x%x\n", LibPcdGetExSize(&gEfiEdkIIPlatformTokenSpaceGuid, PcdTokenEx(&gEfiEdkIIPlatformTokenSpaceGuid, PcdSystemConfiguration)) ));

  return EFI_SUCCESS;
}
