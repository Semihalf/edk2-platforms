/** @file
  Multiplatform initialization header file.
  This file includes package header files, library classes.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MINNOW_MODULE_MULTIPLATFORM_LIB_H_
#define _MINNOW_MODULE_MULTIPLATFORM_LIB_H_

#include "PlatformId.h"
#include <BoardFunctionsPei.h>
#include <Guid/SetupVariable.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiDxeSmmMmPciLib.h>
#include <SaRegs.h>
#include "MmrcData.h"


extern UPDATE_FSPM_UPD_FUNC             mMb3MUpdateFspmUpdPtr;
extern DRAM_CREATE_POLICY_DEFAULTS_FUNC mMb3MDramCreatePolicyDefaultsPtr;
extern UPDATE_PCIE_CONFIG_FUNC          mMb3MUpdatePcieConfigPtr;

typedef struct {
  UINT8    DeviceWidth[DRAM_POLICY_NUMBER_CHANNELS];
  UINT8    DramDensity[DRAM_POLICY_NUMBER_CHANNELS];
  UINT8    Option[DRAM_POLICY_NUMBER_CHANNELS];
  UINT8    RankEnable[DRAM_POLICY_NUMBER_CHANNELS];
  CHAR8    DescString[32];
} BOARD_CHANNEL_INFO;

EFI_STATUS
EFIAPI
Mb3MUpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  );

EFI_STATUS
EFIAPI
Mb3MDramCreatePolicyDefaults (
  IN  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi,
  OUT DRAM_POLICY_PPI                  **DramPolicyPpi,
  IN  IAFWDramConfig                   *DramConfigData,
  IN  UINTN                            *MrcTrainingDataAddr,
  IN  UINTN                            *MrcBootDataAddr,
  IN  UINT8                            BoardId
  );

EFI_STATUS
EFIAPI
Mb3MUpdatePcieConfig (
  IN  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig
  );

#endif

