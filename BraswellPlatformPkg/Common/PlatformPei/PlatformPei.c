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


//
// The global indicator, the FvFileLoader callback will modify it to TRUE after loading PEIM into memory
//

EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN CONST EFI_PEI_STALL_PPI      *This,
  IN UINTN                        Microseconds
  );

static EFI_PEI_STALL_PPI  mStallPpi = {
  PEI_STALL_RESOLUTION,
  Stall
};

static EFI_PEI_PPI_DESCRIPTOR mInstallStallPpi[] = {
  {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiStallPpiGuid,
  &mStallPpi
  }
};

static EFI_PEI_PPI_DESCRIPTOR mBoardDetectionStartPpi[] = {
  {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gBoardDetectionStartPpiGuid,
  NULL
  }
};

EFI_PEI_NOTIFY_DESCRIPTOR mMemoryDiscoveredNotifyList[] = {
  {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  MemoryDiscoveredPpiNotifyCallback
  },
  {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  PostMemoryInitialization
  }
};

EFI_PEI_NOTIFY_DESCRIPTOR mFspInitDoneNotifyList[] = {
  {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gFspInitDonePpiGuid,
  PostSiliconInitialization
  }
};

EFI_STATUS
PeiInitPlatform (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS Status;

  //
  // Init PcdSystemConfiguration.
  //
  Status = GetSystemConfiguration(PeiServices);

  //
  // Initialize Stall PPIs.
  //
  Status = (*PeiServices)->InstallPpi (PeiServices, &mInstallStallPpi[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Start board detection.
  //
  Status = PeiServicesInstallPpi (mBoardDetectionStartPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Set the new boot mode for MRC.
  //
  Status = UpdateBootMode (PeiServices);
  ASSERT_EFI_ERROR (Status);

  //
  // Add code here for pre-memory initialization.
  //
  Status =PreMemoryInitialization (PeiServices);

  //
  // Register callback functions to gEfiPeiMemoryDiscoveredPpiGuid, which will be triggered when
  // after DRAM initialization but before chipset initialization.
  //
  Status = (*PeiServices)->NotifyPpi (PeiServices, &mMemoryDiscoveredNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Register callback functions to gFspInitDonePpiGuid, which will be triggered when
  // all silicon initialzations have been done by FSP.
  //
  Status = (*PeiServices)->NotifyPpi (PeiServices, &mFspInitDoneNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

