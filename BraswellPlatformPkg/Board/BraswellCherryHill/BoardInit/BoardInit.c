/** @file
  Board Init driver.

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/PlatformInfo.h>
#include <Ppi/BoardDetection.h>
#include <Ppi/ChvPlatformPolicyPpi.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Library/GpioLib.h>
#include <Library/PchPlatformLib.h>
#include "BoardInit.h"
#include "BoardGpios.h"

EFI_STATUS
GetBoardFabIdsKsc (
  OUT UINT16                    *BoardFabIds
  );

EFI_STATUS
GetBoradFabIdsGpio (
  OUT UINT8 *BoardId,
  OUT UINT8 *FabId
  );

EFI_STATUS
EFIAPI
BraswellCherryHillBoardDetectionCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR mBraswellCherryHillDetectionStartNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectionStartPpiGuid,
  BraswellCherryHillBoardDetectionCallback
};

static EFI_PEI_PPI_DESCRIPTOR mBraswellCherryHillDetectedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectedPpiGuid,
  NULL
};

EFI_STATUS
EFIAPI
BraswellCherryHillBoardDetectionCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_PLATFORM_INFO_HOB            PlatformInfoHob;
  UINTN                            DataSize;
  UINT16                           BoardFabIds;
  UINT8                            BoardId;
  UINT8                            FabId;
  EFI_STATUS                       Status;
  VOID                             *Instance;

  Status = PeiServicesLocatePpi (
             &gBoardDetectedPpiGuid,
             0,
             NULL,
             &Instance
             );
  if (!EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "BraswellCherryHillBoardDetectionCallback\n"));

  ZeroMem (&PlatformInfoHob, sizeof (PlatformInfoHob));

  Status = GetBoardFabIdsKsc (&BoardFabIds);
  if (Status != EFI_SUCCESS) {
    GetBoradFabIdsGpio (&BoardId, &FabId);
    if (BoardId == 1) {

    //
    // Collect board specific configuration for CherryHill CRB.
    //

    //
    // ID
    //
      PlatformInfoHob.BoardId    = BOARD_ID_BSW_CH;
    PlatformInfoHob.FABID = FabId;
      PlatformInfoHob.PlatformFlavor = FlavorMobile;

    //
    // Power
    //
      PlatformInfoHob.BoardSvidConfig = BSW_SVID_CONFIG0;
      if (SocStepping() >= SocC0) {
        PlatformInfoHob.BoardSvidConfig = BSW_SVID_CONFIG1;
      }

      //
      // EC, Fan, Battery
      //
      PlatformInfoHob.ECSupport = FALSE;
      PlatformInfoHob.FanSupport = FALSE;
      PlatformInfoHob.BatterySupport = FALSE;

    //
    // Memory
    //
    PlatformInfoHob.MemCfgID   = 0;
    PcdSet8 (PcdOemMemeoryDimmType,DimmInstalled);


      //
      // Grpahic, Video, Display
      //
      DataSize = sizeof (EFI_GUID);
      PcdSetPtr (PcdBmpImageGuid, &DataSize, &gHdmiDpVbtGuid);

    //
    // Communication
    //
      PcdSet8 (PcdNfcConnection, 0);

      //
      // GPIO
      //
      CherryHillBoardGpioConfigure();
      PcdSet64 (PcdGpioInitFunc, (UINT64)(UINTN)BraswellCherryHillAdditionalGpioInit);

    //
    // Expose EFI_PLATFORM_INFO_HOB
    //
    DataSize = sizeof (EFI_PLATFORM_INFO_HOB);
      PcdSetPtr (PcdPlatformInfo, &DataSize, &PlatformInfoHob);

      Status = PeiServicesInstallPpi (&mBraswellCherryHillDetectedPpi);

    DEBUG ((EFI_D_INFO,  "PlatformInfoHob->FABID  = 0x%x\n", FabId ));
    DEBUG ((EFI_D_INFO,  "I'm Braswell Cherry Hill \n\n"));
      DEBUG ((EFI_D_INFO,  "SoC Stepping = 0x%x \n", ((UINT32)SocStepping())));
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->MemCfgID= 0x%x\n", PlatformInfoHob.MemCfgID));
    DEBUG ((EFI_D_INFO,  "PlatformInfoHob->PlatformFlavor  = 0x%x\n", PlatformInfoHob.PlatformFlavor ));
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->BoardSvidConfig  = 0x%x\n", PlatformInfoHob.BoardSvidConfig ));
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->BoardId  = 0x%x\n", PlatformInfoHob.BoardId ));

    }
  }

  return EFI_SUCCESS;
}

/**
  This function performs Board initialization in Pre-Memory.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).
**/
EFI_STATUS
EFIAPI
BraswellCherryHillInitConstructor (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;

  Status = PeiServicesNotifyPpi (&mBraswellCherryHillDetectionStartNotifyList);

  return Status;
}

