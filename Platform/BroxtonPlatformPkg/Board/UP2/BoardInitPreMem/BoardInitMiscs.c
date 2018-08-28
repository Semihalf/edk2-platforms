/** @file
  This file does Multiplatform initialization.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitMiscs.h"
#include "MmrcData.h"
#include "PlatformId.h"

UPDATE_FSPM_UPD_FUNC mUp2UpdateFspmUpdPtr = Up2UpdateFspmUpd;
DRAM_CREATE_POLICY_DEFAULTS_FUNC   mUp2DramCreatePolicyDefaultsPtr = Up2DramCreatePolicyDefaults;

UINT8 ChSwizzle_UP2[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS] = {
  {13,10,8,11,12,15,14,9,6,0,3,4,7,1,5,2,28,26,25,27,29,31,30,24,16,23,21,22,20,18,19,17}, // Channel 0   
  {0,7,4,5,6,2,3,1,8,15,13,11,10,9,14,12,23,17,19,18,20,21,22,16,28,26,29,31,24,25,30,27}, // Channel 1   
  {13,8,11,14,12,15,9,10,4,7,1,6,2,3,0,5,24,25,28,26,29,30,31,27,17,19,21,16,22,18,23,20}, // Channel 2
  {0,5,4,7,3,2,6,1,10,11,8,9,12,14,13,15,18,22,20,19,23,17,21,16,25,31,29,27,30,24,28,26}  // Channel 3
};



EFI_STATUS
EFIAPI
Up2UpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  )
{
  EFI_PEI_HOB_POINTERS           Hob;
  EFI_PLATFORM_INFO_HOB          *PlatformInfo = NULL;
  DRAM_POLICY_PPI                *DramPolicy;
  EFI_STATUS                     Status;
  MRC_PARAMS_SAVE_RESTORE        *MrcNvData;
  BOOT_VARIABLE_NV_DATA          *BootVariableNvData;
  MRC_PARAMS_SAVE_RESTORE        *MrcParamsHob;
  BOOT_VARIABLE_NV_DATA          *BootVariableNvDataHob;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
  UINT8                           DdrId;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gDramPolicyPpiGuid,
                             0,
                             NULL,
                             (VOID **) &DramPolicy
                             );

  if (!EFI_ERROR (Status)) {
    FspUpdRgn->FspmConfig.Package                           = DramPolicy->Package;
    FspUpdRgn->FspmConfig.Profile                           = DramPolicy->Profile;
    FspUpdRgn->FspmConfig.MemoryDown                        = DramPolicy->MemoryDown;
    FspUpdRgn->FspmConfig.DDR3LPageSize                     = DramPolicy->DDR3LPageSize;
    FspUpdRgn->FspmConfig.DDR3LASR                          = DramPolicy->DDR3LASR;
    FspUpdRgn->FspmConfig.MemorySizeLimit                   = DramPolicy->SystemMemorySizeLimit;
    FspUpdRgn->FspmConfig.DIMM0SPDAddress                   = DramPolicy->SpdAddress[0];
    FspUpdRgn->FspmConfig.DIMM1SPDAddress                   = DramPolicy->SpdAddress[1];
    FspUpdRgn->FspmConfig.DDR3LPageSize                     = DramPolicy->DDR3LPageSize;
    FspUpdRgn->FspmConfig.DDR3LASR                          = DramPolicy->DDR3LASR;
    FspUpdRgn->FspmConfig.HighMemoryMaxValue                = DramPolicy->HighMemMaxVal;
    FspUpdRgn->FspmConfig.LowMemoryMaxValue                 = DramPolicy->LowMemMaxVal;
    FspUpdRgn->FspmConfig.DisableFastBoot                   = DramPolicy->DisableFastBoot;
    FspUpdRgn->FspmConfig.RmtMode                           = DramPolicy->RmtMode;
    FspUpdRgn->FspmConfig.RmtCheckRun                       = DramPolicy->RmtCheckRun;
    FspUpdRgn->FspmConfig.RmtMarginCheckScaleHighThreshold  = DramPolicy->RmtMarginCheckScaleHighThreshold;
    FspUpdRgn->FspmConfig.MsgLevelMask                      = DramPolicy->MsgLevelMask;

    FspUpdRgn->FspmConfig.ChannelHashMask                   = DramPolicy->ChannelHashMask;
    FspUpdRgn->FspmConfig.SliceHashMask                     = DramPolicy->SliceHashMask;
    FspUpdRgn->FspmConfig.ChannelsSlicesEnable              = DramPolicy->ChannelsSlicesEnabled;
    FspUpdRgn->FspmConfig.ScramblerSupport                  = DramPolicy->ScramblerSupport;
    FspUpdRgn->FspmConfig.InterleavedMode                   = DramPolicy->InterleavedMode;
    FspUpdRgn->FspmConfig.MinRefRate2xEnable                = DramPolicy->MinRefRate2xEnabled;
    FspUpdRgn->FspmConfig.DualRankSupportEnable             = DramPolicy->DualRankSupportEnabled;

    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_RankEnable), &DramPolicy->ChDrp, sizeof (DramPolicy->ChDrp));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), &DramPolicy->ChSwizzle, sizeof (DramPolicy->ChSwizzle));

    if (((VOID *)(UINT32)DramPolicy->MrcTrainingDataPtr != 0) &&
        ((VOID *)(UINT32)DramPolicy->MrcBootDataPtr     != 0)) {
      DEBUG ((DEBUG_INFO, "UpdateFspmUpd - NvsBufferPtr\n"));
      MrcNvData          = (MRC_PARAMS_SAVE_RESTORE *) AllocateZeroPool (sizeof (MRC_PARAMS_SAVE_RESTORE));
      BootVariableNvData = (BOOT_VARIABLE_NV_DATA *) AllocateZeroPool (sizeof (BOOT_VARIABLE_NV_DATA));

      MrcParamsHob          = (MRC_PARAMS_SAVE_RESTORE*)((UINT32)DramPolicy->MrcTrainingDataPtr);
      BootVariableNvDataHob = (BOOT_VARIABLE_NV_DATA*)((UINT32)DramPolicy->MrcBootDataPtr);

      CopyMem(MrcNvData, MrcParamsHob, sizeof (MRC_PARAMS_SAVE_RESTORE));
      CopyMem(BootVariableNvData, BootVariableNvDataHob, sizeof (BOOT_VARIABLE_NV_DATA));
      FspUpdRgn->FspmArchUpd.NvsBufferPtr        = (VOID *)(UINT32)MrcNvData;
      FspUpdRgn->FspmConfig.VariableNvsBufferPtr = (VOID *)(UINT32)BootVariableNvData;
    }

  }

  DEBUG ((DEBUG_INFO, "UpdateFspmUpd - gEfiPlatformInfoGuid\n"));
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  DEBUG ((DEBUG_INFO, "***** Up2 - UpdateFspmUpd,BoardId = %d\n", PlatformInfo->BoardId));

  if (PlatformInfo->BoardId != BOARD_ID_UP2) {//MyBoardPorting
    //
    // ASSERT false if BoardId isn't UP2
    //
    ASSERT (FALSE);
  }

  
  FspUpdRgn->FspmConfig.Package         = 1;
  FspUpdRgn->FspmConfig.Profile         = 0xB; // 2400MHz
  FspUpdRgn->FspmConfig.MemoryDown      = 1;
  FspUpdRgn->FspmConfig.DDR3LPageSize   = 0;
  FspUpdRgn->FspmConfig.DDR3LASR        = 0;
  FspUpdRgn->FspmConfig.MemorySizeLimit = 0;
  FspUpdRgn->FspmConfig.DIMM0SPDAddress = 0;
  FspUpdRgn->FspmConfig.DIMM1SPDAddress = 0;
  FspUpdRgn->FspmConfig.DDR3LPageSize   = 0;
  FspUpdRgn->FspmConfig.DDR3LASR        = 0;
  FspUpdRgn->FspmConfig.ChannelHashMask       = 0;
  FspUpdRgn->FspmConfig.SliceHashMask         = 0;
  FspUpdRgn->FspmConfig.ChannelsSlicesEnable  = 0;
  FspUpdRgn->FspmConfig.ScramblerSupport      = 1;
  FspUpdRgn->FspmConfig.InterleavedMode       = 0;
  FspUpdRgn->FspmConfig.MinRefRate2xEnable    = 0;

  //
  // DDR_ID1    DDR_ID0  Memory
  // GPIO_215   GPIO_214
  //  0          0        2G
  //  0          1        4G
  //  1          0        8G
  //
  Status = Up2GetDdrId(PeiServices, &DdrId);
  
  if (DdrId == 0x00) { // 2GB, SK, Single Rank
  
    FspUpdRgn->FspmConfig.DualRankSupportEnable = 0;

    FspUpdRgn->FspmConfig.Ch0_RankEnable   = 1;  
    FspUpdRgn->FspmConfig.Ch0_DeviceWidth  = 1; 
    FspUpdRgn->FspmConfig.Ch0_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch0_Option       = 3;

    FspUpdRgn->FspmConfig.Ch1_RankEnable   = 1; 
    FspUpdRgn->FspmConfig.Ch1_DeviceWidth  = 1; // x16
    FspUpdRgn->FspmConfig.Ch1_DramDensity  = 2; // 8GB
    FspUpdRgn->FspmConfig.Ch1_Option       = 3;
 
    FspUpdRgn->FspmConfig.Ch2_RankEnable   = 0; // empty
    FspUpdRgn->FspmConfig.Ch2_DeviceWidth  = 1;
    FspUpdRgn->FspmConfig.Ch2_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch2_Option       = 3;

    FspUpdRgn->FspmConfig.Ch3_RankEnable   = 0; // empty
    FspUpdRgn->FspmConfig.Ch3_DeviceWidth  = 1;
    FspUpdRgn->FspmConfig.Ch3_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch3_Option       = 3;
    
  }else if (DdrId == 0x01) { // 4GB, SK, Single Rank
  
    FspUpdRgn->FspmConfig.DualRankSupportEnable = 0;

    FspUpdRgn->FspmConfig.Ch0_RankEnable   = 1; // 
    FspUpdRgn->FspmConfig.Ch0_DeviceWidth  = 1; 
    FspUpdRgn->FspmConfig.Ch0_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch0_Option       = 3;

    FspUpdRgn->FspmConfig.Ch1_RankEnable   = 1; 
    FspUpdRgn->FspmConfig.Ch1_DeviceWidth  = 1; // x16
    FspUpdRgn->FspmConfig.Ch1_DramDensity  = 2; // 8GB
    FspUpdRgn->FspmConfig.Ch1_Option       = 3;

    FspUpdRgn->FspmConfig.Ch2_RankEnable   = 1; // empty
    FspUpdRgn->FspmConfig.Ch2_DeviceWidth  = 1;
    FspUpdRgn->FspmConfig.Ch2_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch2_Option       = 3;

    FspUpdRgn->FspmConfig.Ch3_RankEnable   = 1;
    FspUpdRgn->FspmConfig.Ch3_DeviceWidth  = 1;
    FspUpdRgn->FspmConfig.Ch3_DramDensity  = 2;
    FspUpdRgn->FspmConfig.Ch3_Option       = 3;
    
  }else if (DdrId == 0x02) { // 8GB, Micron, Dual Rank

    //
    // Micron #MT53B512M32D2NP-062 AIT:C) from Platfrom4 profile
    //
    FspUpdRgn->FspmConfig.DualRankSupportEnable = 1;

    FspUpdRgn->FspmConfig.Ch0_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
    FspUpdRgn->FspmConfig.Ch0_DeviceWidth       = 0x01; // x16
    FspUpdRgn->FspmConfig.Ch0_DramDensity       = 0x02; // 8Gb
    FspUpdRgn->FspmConfig.Ch0_Option            = 0x03;

    FspUpdRgn->FspmConfig.Ch1_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
    FspUpdRgn->FspmConfig.Ch1_DeviceWidth       = 0x01; // x16
    FspUpdRgn->FspmConfig.Ch1_DramDensity       = 0x02; // 8Gb
    FspUpdRgn->FspmConfig.Ch1_Option            = 0x03;

    FspUpdRgn->FspmConfig.Ch2_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
    FspUpdRgn->FspmConfig.Ch2_DeviceWidth       = 0x01; // x16
    FspUpdRgn->FspmConfig.Ch2_DramDensity       = 0x02; // 8Gb
    FspUpdRgn->FspmConfig.Ch2_Option            = 0x03;

    FspUpdRgn->FspmConfig.Ch3_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
    FspUpdRgn->FspmConfig.Ch3_DeviceWidth       = 0x01; // x16
    FspUpdRgn->FspmConfig.Ch3_DramDensity       = 0x02; // 8Gb
    FspUpdRgn->FspmConfig.Ch3_Option            = 0x03;
  }

  //
  // Swizzling
  //
  if (ChSwizzle_UP2 != NULL) {
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), ChSwizzle_UP2[0], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch1_Bit_swizzling), ChSwizzle_UP2[1], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch2_Bit_swizzling), ChSwizzle_UP2[2], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch3_Bit_swizzling), ChSwizzle_UP2[3], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
  }


  //
  // Enable or disable NPK based on DciEn
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariablePpi);
  if (!EFI_ERROR (Status)) {
    VariableSize = sizeof (SYSTEM_CONFIGURATION);
    Status = VariablePpi->GetVariable (
                            VariablePpi,
                            PLATFORM_SETUP_VARIABLE_NAME,
                            &gEfiSetupVariableGuid,
                            NULL,
                            &VariableSize,
                            &SystemConfiguration
                            );
    if (!EFI_ERROR (Status)) {
      if (SystemConfiguration.DciEn == 0) {
        FspUpdRgn->FspmConfig.NpkEn = 0;
      } else if (SystemConfiguration.DciAutoDetect == 1) {
        FspUpdRgn->FspmConfig.NpkEn = 3;
      } else {
        FspUpdRgn->FspmConfig.NpkEn = 1;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  DramCreatePolicyDefaults creates the default setting of Dram Policy.

  @param[out] DramPolicyPpi           The pointer to get Dram Policy PPI instance

  @retval     EFI_SUCCESS             The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
Up2DramCreatePolicyDefaults (
  IN  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi,
  OUT DRAM_POLICY_PPI                  **DramPolicyPpi,
  IN  IAFWDramConfig                   *DramConfigData,
  IN  UINTN                            *MrcTrainingDataAddr,
  IN  UINTN                            *MrcBootDataAddr,
  IN  UINT8                            BoardId
  )
{
  DRAM_POLICY_PPI                     *DramPolicy;
  SYSTEM_CONFIGURATION                SystemConfiguration;
  UINTN                               VariableSize;
  EFI_STATUS                          Status;
  DRP_DRAM_POLICY                     *DrpPtr;
  UINT8                               (*ChSwizlePtr)[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS];
  PlatfromDramConf                    *DramConfig;
  BOOLEAN                             ReadSetupVars;
  EFI_PEI_HOB_POINTERS                Hob;

  DEBUG ((EFI_D_INFO, "*** Up2 DramCreatePolicyDefaults\n"));
  DramPolicy = (DRAM_POLICY_PPI *) AllocateZeroPool (sizeof (DRAM_POLICY_PPI));
  if (DramPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  ReadSetupVars = FALSE;
  DrpPtr        = NULL;
  ChSwizlePtr   = NULL;
  DramConfig    = NULL;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          PLATFORM_SETUP_VARIABLE_NAME,
                          &gEfiSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &SystemConfiguration
                          );

#if !(ONLY_USE_SMIP_DRAM_POLICY == 1)
  Status = EFI_UNSUPPORTED;
#endif

  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Using setup options data for DRAM policy\n"));
    ReadSetupVars = TRUE;
    DramPolicy->ChannelHashMask         = SystemConfiguration.ChannelHashMask;
    DramPolicy->SliceHashMask           = SystemConfiguration.SliceHashMask;
    DramPolicy->ChannelsSlicesEnabled   = SystemConfiguration.ChannelsSlicesEnabled;
    DramPolicy->ScramblerSupport        = SystemConfiguration.ScramblerSupport;
    DramPolicy->InterleavedMode         = SystemConfiguration.InterleavedMode;
    DramPolicy->MinRefRate2xEnabled     = SystemConfiguration.MinRefRate2xEnabled;
    DramPolicy->DualRankSupportEnabled  = SystemConfiguration.DualRankSupportEnabled;
  }

  DramConfig = &(DramConfigData->PlatformDram4);

  DEBUG ((EFI_D_INFO, "Using smip platform override: %d\n", DramConfigData->Platform_override));
  switch (DramConfigData->Platform_override) {
    case 0:
      DramConfig = &(DramConfigData->PlatformDram0);
      break;
    case 1:
      DramConfig = &(DramConfigData->PlatformDram1);
      break;
    case 2:
      DramConfig = &(DramConfigData->PlatformDram2);
      break;
    case 3:
      DramConfig = &(DramConfigData->PlatformDram3);
      break;
    case 4:
      DramConfig = &(DramConfigData->PlatformDram4);
      break;
    default:
      //
      // Do nothing if the override value does not exist. 0xFF is the
      // default Platform_override value when no override is selected
      //
      break;
    }

  DramPolicy->Package                            = DramConfig->Package;
  DramPolicy->Profile                            = DramConfig->Profile;
  DramPolicy->MemoryDown                         = DramConfig->MemoryDown;
  DramPolicy->DDR3LPageSize                      = DramConfig->DDR3LPageSize;
  DramPolicy->DDR3LASR                           = DramConfig->DDR3LASR;
  DramPolicy->SystemMemorySizeLimit              = DramConfig->MemorySizeLimit;
  DramPolicy->SpdAddress[0]                      = DramConfig->SpdAddress0;
  DramPolicy->SpdAddress[1]                      = DramConfig->SpdAddress1;
  DramPolicy->DDR3LPageSize                      = DramConfig->DDR3LPageSize;
  DramPolicy->DDR3LASR                           = DramConfig->DDR3LASR;
  DramPolicy->HighMemMaxVal                      = DramConfig->HighMemMaxVal;
  DramPolicy->LowMemMaxVal                       = DramConfig->LowMemMaxVal;
  DramPolicy->DisableFastBoot                    = DramConfig->DisableFastBoot;
  DramPolicy->RmtMode                            = DramConfig->RmtMode;
  DramPolicy->RmtCheckRun                        = DramConfig->RmtCheckRun;
  DramPolicy->RmtMarginCheckScaleHighThreshold   = DramConfig->RmtMarginCheckScaleHighThreshold;

  DramPolicy->MsgLevelMask                       = DramConfigData->Message_level_mask;
  DrpPtr                                         = (DRP_DRAM_POLICY *) (&(DramConfig->Ch0RankEnabled));
  ChSwizlePtr                                    = (UINT8(*)[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS]) (&(DramConfig->Ch0_Bit00_swizzling));

  if (!ReadSetupVars) {
    DEBUG ((EFI_D_INFO, "Using smip data for DRAM policy\n"));
    DramPolicy->ChannelHashMask         = DramConfig->ChannelHashMask;
    DramPolicy->SliceHashMask           = DramConfig->SliceHashMask;
    DramPolicy->ChannelsSlicesEnabled   = DramConfig->ChannelsSlicesEnabled;
    DramPolicy->ScramblerSupport        = DramConfig->ScramblerSupport;
    DramPolicy->InterleavedMode         = DramConfig->InterleavedMode;
    DramPolicy->MinRefRate2xEnabled     = DramConfig->MinRefRate2xEnabled;
    DramPolicy->DualRankSupportEnabled  = DramConfig->DualRankSupportEnabled;
  }

  if (DrpPtr != NULL) {
    CopyMem (DramPolicy->ChDrp, DrpPtr, sizeof (DramPolicy->ChDrp));
  }

  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          PLATFORM_SETUP_VARIABLE_NAME,
                          &gEfiSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &SystemConfiguration
                          );

  if (!EFI_ERROR (Status)) {
    if (SystemConfiguration.Max2G == 0) {
      DramPolicy->SystemMemorySizeLimit = 0x800;
    }
  }

  if (ChSwizlePtr != NULL) CopyMem (DramPolicy->ChSwizzle, ChSwizlePtr, sizeof (DramPolicy->ChSwizzle));

  DramPolicy->MrcTrainingDataPtr = (EFI_PHYSICAL_ADDRESS) *MrcTrainingDataAddr;
  DramPolicy->MrcBootDataPtr     = (EFI_PHYSICAL_ADDRESS) *MrcBootDataAddr;

  //
  // Get Platform Info HOB
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);


  DEBUG ((EFI_D_INFO, "UP2 has single rank memory\n"));
  DramPolicy->DualRankSupportEnabled = FALSE;

  *DramPolicyPpi = DramPolicy;

  return EFI_SUCCESS;
}

