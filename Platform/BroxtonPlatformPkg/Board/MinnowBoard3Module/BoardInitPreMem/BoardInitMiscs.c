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

UPDATE_FSPM_UPD_FUNC               mMb3MUpdateFspmUpdPtr            = Mb3MUpdateFspmUpd;
DRAM_CREATE_POLICY_DEFAULTS_FUNC   mMb3MDramCreatePolicyDefaultsPtr = Mb3MDramCreatePolicyDefaults;
UPDATE_PCIE_CONFIG_FUNC            mMb3MUpdatePcieConfigPtr         = Mb3MUpdatePcieConfig;

//
// Minnow Board Module, Fab A swizzling
//
UINT8 ChSwizzle_MB3Ma[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS] = {
  {0x00,0x06,0x04,0x05,0x01,0x03,0x02,0x07,0x08,0x09,0x0B,0x0F,0x0A,0x0D,0x0C,0x0E,0x1C,0x18,0x1A,0x1B,0x1D,0x1E,0x1F,0x19,0x12,0x13,0x14,0x11,0x10,0x16,0x17,0x15}, // Channel 0
  {0x0E,0x0F,0x0A,0x0B,0x08,0x0D,0x0C,0x09,0x07,0x04,0x05,0x00,0x01,0x03,0x02,0x06,0x12,0x15,0x14,0x17,0x10,0x13,0x11,0x16,0x1D,0x1C,0x1F,0x1B,0x1A,0x19,0x18,0x1E}, // Channel 1
  {0x0B,0x0D,0x0C,0x0F,0x09,0x08,0x0A,0x0E,0x05,0x06,0x03,0x07,0x00,0x01,0x02,0x04,0x17,0x11,0x10,0x13,0x14,0x16,0x15,0x12,0x19,0x1B,0x1A,0x18,0x1C,0x1D,0x1E,0x1F}, // Channel 2
  {0x07,0x01,0x02,0x03,0x04,0x05,0x06,0x00,0x0E,0x0D,0x0B,0x0C,0x0A,0x08,0x0F,0x09,0x1E,0x18,0x1C,0x1D,0x1A,0x19,0x1B,0x1F,0x14,0x11,0x12,0x16,0x13,0x15,0x17,0x10}  // Channel 3
};

//
// Minnow Board Module, Fab C swizzling
//
UINT8 ChSwizzle_MB3Mc[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS] = {
  {0x0F,0x0A,0x09,0x08,0x0E,0x0C,0x0D,0x0B,0x01,0x05,0x06,0x04,0x02,0x03,0x00,0x07,0x1C,0x18,0x1A,0x1B,0x1D,0x1E,0x1F,0x19,0x12,0x13,0x14,0x11,0x10,0x16,0x17,0x15}, // Channel 0
  {0x0E,0x0F,0x0A,0x0B,0x08,0x0D,0x0C,0x09,0x07,0x04,0x05,0x00,0x01,0x03,0x02,0x06,0x1C,0x18,0x1B,0x1A,0x1F,0x1E,0x1D,0x19,0x15,0x16,0x14,0x17,0x10,0x12,0x11,0x13}, // Channel 1
  {0x07,0x00,0x02,0x03,0x06,0x05,0x04,0x01,0x0C,0x0B,0x0F,0x09,0x0A,0x0D,0x0E,0x08,0x17,0x11,0x10,0x13,0x14,0x16,0x15,0x12,0x19,0x1B,0x1A,0x18,0x1C,0x1D,0x1E,0x1F}, // Channel 2
  {0x07,0x01,0x02,0x03,0x04,0x05,0x06,0x00,0x0E,0x0D,0x0B,0x0C,0x0A,0x08,0x0F,0x09,0x10,0x15,0x13,0x11,0x14,0x16,0x17,0x12,0x1A,0x1F,0x1C,0x1B,0x1D,0x19,0x18,0x1E}  // Channel 3
};

BOARD_CHANNEL_INFO gMB3MChannelInfo[] = {
   // DeviceWidth               DramDensity               Option                    RankEnable                DescString
   // Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x01, 0x01, 0x00, 0x00}, "LPDDR4  8Gbit 2 channels"}, // #1 - MT53B256M32D1  8Gbit 2 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x01, 0x01, 0x01, 0x01}, "LPDDR4  8Gbit 4 channels"}, // #2 - MT53B256M32D1  8Gbit 4 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x03, 0x03, 0x00, 0x00}, "LPDDR4 16Gbit 2 channels"}, // #3 - MT53B512M32D2 16Gbit 2 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x03, 0x03, 0x03, 0x03}, "LPDDR4 16Gbit 4 channels"}, // #4 - MT53B512M32D2 16Gbit 4 channels
};

EFI_STATUS
EFIAPI
Mb3MUpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  )
{
  EFI_PEI_HOB_POINTERS               Hob;
  EFI_PLATFORM_INFO_HOB             *PlatformInfo = NULL;
  DRAM_POLICY_PPI                   *DramPolicy;
  EFI_STATUS                         Status;
  MRC_PARAMS_SAVE_RESTORE           *MrcNvData;
  BOOT_VARIABLE_NV_DATA             *BootVariableNvData;
  MRC_PARAMS_SAVE_RESTORE           *MrcParamsHob;
  BOOT_VARIABLE_NV_DATA             *BootVariableNvDataHob;
  SYSTEM_CONFIGURATION               SystemConfiguration;
  UINTN                              VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;
  UINT32                             HwconfStraps;
  UINT8                              MemoryType;
  UINT8                             *MemoryData;
  UINT32                             MemoryDataSize;
  UINT16                             SpdSlotFlag;

  //
  // Check for EEPROM memory data
  //
  MemoryData     = NULL;
  MemoryDataSize = 0;
  SpdSlotFlag    = 0;
  Status = EepromGetMemoryData (NULL, &SpdSlotFlag, &MemoryData, &MemoryDataSize);
  DEBUG ((DEBUG_INFO, "**** MB3 Module - EepromGetMemoryData() --> %r\n", Status));
  if (EFI_ERROR (Status) || (MemoryDataSize < sizeof (DRAM_POLICY_PPI)) || (SpdSlotFlag != 0x000F)) {
    //
    // Didn't find valid memory data. Clear the pointers.
    //
    MemoryData     = NULL;
    MemoryDataSize = 0;
    SpdSlotFlag    = 0;
  }

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gDramPolicyPpiGuid,
                             0,
                             NULL,
                             (VOID **) &DramPolicy
                             );
  if (EFI_ERROR (Status)) {
    DramPolicy = NULL;
  } else {
    if (((VOID *) (UINT32) DramPolicy->MrcTrainingDataPtr != 0) &&
        ((VOID *) (UINT32) DramPolicy->MrcBootDataPtr     != 0)) {
      DEBUG ((DEBUG_INFO, "%a() - NvsBufferPtr\n", __FUNCTION__));
      MrcNvData          = (MRC_PARAMS_SAVE_RESTORE *) AllocateZeroPool (sizeof (MRC_PARAMS_SAVE_RESTORE));
      BootVariableNvData = (BOOT_VARIABLE_NV_DATA *) AllocateZeroPool (sizeof (BOOT_VARIABLE_NV_DATA));

      MrcParamsHob          = (MRC_PARAMS_SAVE_RESTORE *) ((UINT32) DramPolicy->MrcTrainingDataPtr);
      BootVariableNvDataHob = (BOOT_VARIABLE_NV_DATA *) ((UINT32) DramPolicy->MrcBootDataPtr);

      CopyMem (MrcNvData,          MrcParamsHob,          sizeof (MRC_PARAMS_SAVE_RESTORE));
      CopyMem (BootVariableNvData, BootVariableNvDataHob, sizeof (BOOT_VARIABLE_NV_DATA));
      FspUpdRgn->FspmArchUpd.NvsBufferPtr        = (VOID *) (UINT32) MrcNvData;
      FspUpdRgn->FspmConfig.VariableNvsBufferPtr = (VOID *) (UINT32) BootVariableNvData;
    }
  }

  //
  // Overrides for MinnowBoard3Module from Platfrom4 profile
  //
  //   Description    | DualRank | RankEnable | DeviceWidth | DramDenisty | SoC   | Channel
  //  ================|==========|============|=============|=============|=======|=========
  //   MT53B256M32D1  | 0x01     | 0x01       | 0x01  x16   | 0x02   8Gb  | E3930 | Ch0
  //   MT53B512M32D2  | 0x01     | 0x03       | 0x01  x16   | 0x02   8Gb  | E3950 | Ch0&1
  //   MT53B1024M32D4 | 0x01     | 0x03       | 0x00  x8    | 0x04  16Gb  |       |
  //

  //
  // Get HWCONF straps
  //
  HwconfStraps = Minnow3ModuleGetHwconfStraps ();
  DEBUG ((DEBUG_INFO, "**** MB3 Module - HWCONF straps = 0x%08X\n", HwconfStraps));

  //
  // Translate into Memory Type
  //
  MemoryType = (UINT8) ((HwconfStraps & MB3M_HWCONF_MEMORY_MASK) >> MB3M_HWCONF_MEMORY);
  if (MemoryType == 0) {
    DEBUG ((DEBUG_INFO, "**** MB3 Module - SPD based memory init requested, but converted into Memory Profile type #4!\n"));
    MemoryType = 4; // LPDDR4 16Gbit 4 channels
  }
  if (MemoryType == 5) {
    if (MemoryData == NULL) {
      DEBUG ((DEBUG_INFO, "**** MB3 Module - EEPROM based memory init requested, but converted into Memory Profile type #4!\n"));
      MemoryType = 4; // LPDDR4 16Gbit 4 channels
    } else {
      DEBUG ((DEBUG_INFO, "**** MB3 Module - EEPROM based memory init requested and being used.\n"));
      DramPolicy = (DRAM_POLICY_PPI *) MemoryData;
    }
  }
  MemoryType--; // Zero base it for use as index into array

  if (DramPolicy == NULL) {
    //
    // Status is still valid from LocatePpi(gDramPolicyPpiGuid)
    //
    DEBUG ((DEBUG_INFO, "%a() - LocatePpi(gDramPolicyPpiGuid) returned %r\n", __FUNCTION__, Status));
  } else {
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

    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_RankEnable),    &DramPolicy->ChDrp,     sizeof (DramPolicy->ChDrp));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), &DramPolicy->ChSwizzle, sizeof (DramPolicy->ChSwizzle));
  }

  DEBUG ((DEBUG_INFO, "%a() - gEfiPlatformInfoGuid\n", __FUNCTION__));
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  if (MemoryType != 4) {
    //
    // Common items
    //
    FspUpdRgn->FspmConfig.Package                 = 0x01;
    FspUpdRgn->FspmConfig.Profile                 = 0x09; // LPDDR4_1600_14_15_15
    //FspUpdRgn->FspmConfig.Profile                 = 0x0B; // LPDDR4_2400_24_22_22
    FspUpdRgn->FspmConfig.MemoryDown              = 0x01;
    FspUpdRgn->FspmConfig.DualRankSupportEnable   = 0x01;

    //
    // Memory Type specific items
    //
    if (MemoryType < (sizeof (gMB3MChannelInfo) / sizeof (gMB3MChannelInfo[0]))) {
      DEBUG ((DEBUG_INFO, "**** MB3 Module - %a detected!\n", gMB3MChannelInfo[MemoryType].DescString));

      // DDR0CH0
      FspUpdRgn->FspmConfig.Ch0_RankEnable        = gMB3MChannelInfo[MemoryType].RankEnable[0];
      FspUpdRgn->FspmConfig.Ch0_DeviceWidth       = gMB3MChannelInfo[MemoryType].DeviceWidth[0];
      FspUpdRgn->FspmConfig.Ch0_DramDensity       = gMB3MChannelInfo[MemoryType].DramDensity[0];
      FspUpdRgn->FspmConfig.Ch0_Option            = gMB3MChannelInfo[MemoryType].Option[0];

      // DDR0CH1
      FspUpdRgn->FspmConfig.Ch1_RankEnable        = gMB3MChannelInfo[MemoryType].RankEnable[1];
      FspUpdRgn->FspmConfig.Ch1_DeviceWidth       = gMB3MChannelInfo[MemoryType].DeviceWidth[1];
      FspUpdRgn->FspmConfig.Ch1_DramDensity       = gMB3MChannelInfo[MemoryType].DramDensity[1];
      FspUpdRgn->FspmConfig.Ch1_Option            = gMB3MChannelInfo[MemoryType].Option[1];

      // DDR1CH0
      FspUpdRgn->FspmConfig.Ch2_RankEnable        = gMB3MChannelInfo[MemoryType].RankEnable[2];
      FspUpdRgn->FspmConfig.Ch2_DeviceWidth       = gMB3MChannelInfo[MemoryType].DeviceWidth[2];
      FspUpdRgn->FspmConfig.Ch2_DramDensity       = gMB3MChannelInfo[MemoryType].DramDensity[2];
      FspUpdRgn->FspmConfig.Ch2_Option            = gMB3MChannelInfo[MemoryType].Option[2];

      // DDR1CH1
      FspUpdRgn->FspmConfig.Ch3_RankEnable        = gMB3MChannelInfo[MemoryType].RankEnable[3];
      FspUpdRgn->FspmConfig.Ch3_DeviceWidth       = gMB3MChannelInfo[MemoryType].DeviceWidth[3];
      FspUpdRgn->FspmConfig.Ch3_DramDensity       = gMB3MChannelInfo[MemoryType].DramDensity[3];
      FspUpdRgn->FspmConfig.Ch3_Option            = gMB3MChannelInfo[MemoryType].Option[3];
    } else {
      DEBUG ((DEBUG_INFO, "**** MB3 Module - Memory Type 0x%02X is out of range!\n", MemoryType));
    }

    //
    // Swizzling
    //
    if ((PcdGet8 (PcdFabId) == FAB_ID_A) && (ChSwizzle_MB3Ma != NULL)) {
      CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), ChSwizzle_MB3Ma[0], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch1_Bit_swizzling), ChSwizzle_MB3Ma[1], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch2_Bit_swizzling), ChSwizzle_MB3Ma[2], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch3_Bit_swizzling), ChSwizzle_MB3Ma[3], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
    }
    if ((PcdGet8 (PcdFabId) == FAB_ID_C) && (ChSwizzle_MB3Mc != NULL)) {
      CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), ChSwizzle_MB3Mc[0], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch1_Bit_swizzling), ChSwizzle_MB3Mc[1], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch2_Bit_swizzling), ChSwizzle_MB3Mc[2], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
      CopyMem (&(FspUpdRgn->FspmConfig.Ch3_Bit_swizzling), ChSwizzle_MB3Mc[3], DRAM_POLICY_NUMBER_BITS * sizeof (UINT8));
    }
  }

  //
  // Update DramPolicy and then dump it so it can be copied into the EEPROM image
  //
  DramPolicy->Package                          = FspUpdRgn->FspmConfig.Package;
  DramPolicy->Profile                          = FspUpdRgn->FspmConfig.Profile;
  DramPolicy->MemoryDown                       = FspUpdRgn->FspmConfig.MemoryDown;
  DramPolicy->DDR3LPageSize                    = FspUpdRgn->FspmConfig.DDR3LPageSize;
  DramPolicy->DDR3LASR                         = FspUpdRgn->FspmConfig.DDR3LASR;
  DramPolicy->SystemMemorySizeLimit            = FspUpdRgn->FspmConfig.MemorySizeLimit;
  DramPolicy->SpdAddress[0]                    = FspUpdRgn->FspmConfig.DIMM0SPDAddress;
  DramPolicy->SpdAddress[1]                    = FspUpdRgn->FspmConfig.DIMM1SPDAddress;
  DramPolicy->DDR3LPageSize                    = FspUpdRgn->FspmConfig.DDR3LPageSize;
  DramPolicy->DDR3LASR                         = FspUpdRgn->FspmConfig.DDR3LASR;
  DramPolicy->HighMemMaxVal                    = FspUpdRgn->FspmConfig.HighMemoryMaxValue;
  DramPolicy->LowMemMaxVal                     = FspUpdRgn->FspmConfig.LowMemoryMaxValue;
  DramPolicy->DisableFastBoot                  = FspUpdRgn->FspmConfig.DisableFastBoot;
  DramPolicy->RmtMode                          = FspUpdRgn->FspmConfig.RmtMode;
  DramPolicy->RmtCheckRun                      = FspUpdRgn->FspmConfig.RmtCheckRun;
  DramPolicy->RmtMarginCheckScaleHighThreshold = FspUpdRgn->FspmConfig.RmtMarginCheckScaleHighThreshold;
  DramPolicy->MsgLevelMask                     = FspUpdRgn->FspmConfig.MsgLevelMask;
  DramPolicy->ChannelHashMask                  = FspUpdRgn->FspmConfig.ChannelHashMask;
  DramPolicy->SliceHashMask                    = FspUpdRgn->FspmConfig.SliceHashMask;
  DramPolicy->ChannelsSlicesEnabled            = FspUpdRgn->FspmConfig.ChannelsSlicesEnable;
  DramPolicy->ScramblerSupport                 = FspUpdRgn->FspmConfig.ScramblerSupport;
  DramPolicy->InterleavedMode                  = FspUpdRgn->FspmConfig.InterleavedMode;
  DramPolicy->MinRefRate2xEnabled              = FspUpdRgn->FspmConfig.MinRefRate2xEnable;
  DramPolicy->DualRankSupportEnabled           = FspUpdRgn->FspmConfig.DualRankSupportEnable;
  CopyMem (&DramPolicy->ChDrp,     &(FspUpdRgn->FspmConfig.Ch0_RankEnable),    sizeof (DramPolicy->ChDrp));
  CopyMem (&DramPolicy->ChSwizzle, &(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), sizeof (DramPolicy->ChSwizzle));
  // Print header marker
  DEBUG ((DEBUG_INFO, "\n\nDRAM_POLICY_PPI dump for EEPROM programming [0x%08x]:\n", sizeof (DRAM_POLICY_PPI)));
  EepromDumpParagraph (DEBUG_INFO, DramPolicy, sizeof (DRAM_POLICY_PPI));
  // Print footer marker
  DEBUG ((DEBUG_INFO, "\n\n"));

  //
  // Disable NPK based on DciEn
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
        FspUpdRgn->FspmConfig.NpkEn = 0; // Disable
      } else if (SystemConfiguration.DciAutoDetect == 1) {
        FspUpdRgn->FspmConfig.NpkEn = 3; // Auto
      } else {
        FspUpdRgn->FspmConfig.NpkEn = 1; // Enable
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
Mb3MDramCreatePolicyDefaults (
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

  DEBUG ((EFI_D_INFO, "*** Minnow Board 3 Module DramCreatePolicyDefaults\n"));
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
      DramConfig = &(DramConfigData->PlatformDram4);
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

  //
  // DRP
  //
  if (DrpPtr != NULL) {
    CopyMem (DramPolicy->ChDrp, DrpPtr, sizeof (DramPolicy->ChDrp));
  }

  //
  // Swizzling
  //
  if (ChSwizlePtr != NULL) {
    CopyMem (DramPolicy->ChSwizzle, ChSwizlePtr, sizeof (DramPolicy->ChSwizzle));
  }

  if (ReadSetupVars) {
    if (SystemConfiguration.Max2G == 0) {
      DramPolicy->SystemMemorySizeLimit = 0x800;
    }
  }

  DramPolicy->MrcTrainingDataPtr = (EFI_PHYSICAL_ADDRESS) *MrcTrainingDataAddr;
  DramPolicy->MrcBootDataPtr     = (EFI_PHYSICAL_ADDRESS) *MrcBootDataAddr;

  *DramPolicyPpi = DramPolicy;

  return EFI_SUCCESS;
}

/**
  MB3MUpdatePcieConfig updates the PCIe config block for platform specific items.

  @param[in] PciePreMemConfig         The pointer to the PCIe premem config instance

  @retval     EFI_SUCCESS             The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
Mb3MUpdatePcieConfig (
  IN  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig
  )
{
  //
  // Minnow Board v3 Module
  //
  PciePreMemConfig->RootPort[0].Perst = W_PMU_PLTRST_B; // D20:F0 - PCIe-A
  PciePreMemConfig->RootPort[1].Perst = W_PMU_PLTRST_B; // D20:F1 - PCIe-B
  PciePreMemConfig->RootPort[2].Perst = W_PMU_PLTRST_B; // D19:F0 - PCIe-C
  PciePreMemConfig->RootPort[3].Perst = W_PMU_PLTRST_B; // D19:F1 - PCIe-D
  PciePreMemConfig->RootPort[4].Perst = W_PMU_PLTRST_B; // D19:F2 - LAN
  PciePreMemConfig->RootPort[5].Perst = W_PMU_PLTRST_B; // D19:F3 - Empty
  PciePreMemConfig->RootPort[0].Clock = W_GPIO_210;     // D20:F0 - PCIe-A
  PciePreMemConfig->RootPort[1].Clock = W_GPIO_211;     // D20:F1 - PCIe-B
  PciePreMemConfig->RootPort[2].Clock = W_GPIO_212;     // D19:F0 - PCIe-C
  PciePreMemConfig->RootPort[3].Clock = 0;              // D19:F1 - PCIe-D
  PciePreMemConfig->RootPort[4].Clock = W_GPIO_209;     // D19:F2 - LAN
  PciePreMemConfig->RootPort[5].Clock = 0;              // D19:F3 - Empty

  return EFI_SUCCESS;
}

