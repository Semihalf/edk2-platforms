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
#include "BoardInit.h"
#include "BoardGpios.h"
#include "Dram.h"

extern EFI_GUID  gBswCrImageGuid;


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
BraswellCRBoardDetectionCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR mBraswellCRDetectionStartNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectionStartPpiGuid,
  BraswellCRBoardDetectionCallback
};

static EFI_PEI_PPI_DESCRIPTOR mBraswellCRDetectedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectedPpiGuid,
  NULL
};

EFI_STATUS
EFIAPI
BraswellCRBoardDetectionCallback (
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

  DEBUG ((EFI_D_INFO, "BraswellCRDetectionCallback\n"));

  ZeroMem (&PlatformInfoHob, sizeof (PlatformInfoHob));

  Status = GetBoardFabIdsKsc (&BoardFabIds);
  if (Status == EFI_SUCCESS) {
    FabId = (UINT8) BoardFabIds & 0x07;
    BoardId   = (UINT8) ((BoardFabIds & 0x0F00) >> 8);

    if ((BoardId == 0x07)|| (BoardId == 0x06)) {

      //
    // Collect board specific configuration for Braswell CR.
    //

    //
    // ID
    //
      PlatformInfoHob.BoardId    = BOARD_ID_BSW_CR;
      PlatformInfoHob.FABID = FabId;
    PlatformInfoHob.PlatformFlavor = FlavorMobile;

    //
    // Memory
    //
      PlatformInfoHob.MemCfgID   = 0;
      PcdSet8 (PcdOemMemeoryDimmType,SolderDownMemory);
      PcdSet64 (PcdMemorySpdPtr, (UINT64)(UINTN)&mBSW_CR_SpdDataMemorySolderDown);

      //
      // EC, Fan, Battery
      //
      PlatformInfoHob.ECSupport = TRUE;
      PlatformInfoHob.FanSupport = TRUE;
      PlatformInfoHob.BatterySupport = TRUE;

      //
      // Graphic, Video, Diaplay
      //
      DataSize = sizeof (EFI_GUID);
      PcdSetPtr (PcdBmpImageGuid, &DataSize, &gBswCrImageGuid);

    //
    // Power
    //
    PlatformInfoHob.BoardSvidConfig = BSW_I2C_PMIC_CONFIG;

    //
    // Communication
    //
      PcdSet8 (PcdNfcConnection, 1);

      //
      // GPIO
      //
      BraswellCrBoardGpioConfigure();
      PcdSet64 (PcdGpioInitFunc, (UINT64)(UINTN)BraswellCRGpioLateInit);

      //
      // Expose EFI_PLATFORM_INFO_HOB.
      //
    DataSize = sizeof (EFI_PLATFORM_INFO_HOB);
      PcdSetPtr (PcdPlatformInfo, &DataSize, &PlatformInfoHob);

      Status = PeiServicesInstallPpi (&mBraswellCRDetectedPpi);

    DEBUG ((EFI_D_INFO,  "I'm BrasWell CR \n\n"));
    DEBUG ((EFI_D_INFO,  "PlatformInfoHob->MemCfgID= 0x%x\n", PlatformInfoHob.MemCfgID));
    DEBUG ((EFI_D_INFO,  "BoardID detected = %d\n", BoardId));
    DEBUG ((EFI_D_INFO,  "PlatformInfoHob->FABID  = 0x%x\n", FabId ));
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
BraswellCRInitConstructor (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;

  Status = PeiServicesNotifyPpi (&mBraswellCRDetectionStartNotifyList);

  return Status;
}

