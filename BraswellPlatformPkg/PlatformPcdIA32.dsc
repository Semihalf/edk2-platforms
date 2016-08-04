## @file
#  Platform description.
#
#  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

################################################################################
#
# FSP Configuration
#
################################################################################
[PcdsFixedAtBuild.common]
  gFspWrapperTokenSpaceGuid.PcdTemporaryRamSize|0x00020000

[PcdsFixedAtBuild.IA32]
  !if $(PERFORMANCE_ENABLE) == TRUE
    gFspWrapperTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x1600
  !else
    gFspWrapperTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x2600
  !endif
  gFspWrapperTokenSpaceGuid.PcdCpuMicrocodePatchAddress|0xFFC60000
  gFspWrapperTokenSpaceGuid.PcdCpuMicrocodePatchRegionSize|0x00023000
  gFspWrapperTokenSpaceGuid.PcdFlashMicroCodeOffset|0x60
  gFspWrapperTokenSpaceGuid.PcdFlashCodeCacheAddress|0xFFC00000
  gFspWrapperTokenSpaceGuid.PcdFlashCodeCacheSize|0x00400000
  gFspWrapperTokenSpaceGuid.PcdFlashFvFspBase|0xFFF20000
  gFspWrapperTokenSpaceGuid.PcdFlashFvFspSize|0x0009E000
  gFspWrapperTokenSpaceGuid.PcdMaxUpdRegionSize|0x300

[PcdsPatchableInModule.common]
  gFspWrapperTokenSpaceGuid.PcdFlashFvFspBase|0xFFF80000
  gFspWrapperTokenSpaceGuid.PcdFlashFvSecondFspBase|0x00000000
  gFspWrapperTokenSpaceGuid.PcdFlashFvFspSize|0x00048000

##################################################################################
#
# Debug
#
##################################################################################
[PcdsFeatureFlag.common]
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseDataHub|FALSE
  !if $(TARGET) == RELEASE
    gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
  !endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE

[PcdsFixedAtBuild.common]
  !if $(TARGET) == RELEASE
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x3
  !else
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  !endif

[PcdsFixedAtBuild.IA32]
!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x3
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2E
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
!endif

[PcdsPatchableInModule.common]
  # UART 16550
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x3F8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE

  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80380546

##################################################################################
#
# Security
#
##################################################################################
[PcdsFixedAtBuild.common]
!if $(SECURE_BOOT_ENABLE) == TRUE
  gEfiCpuTokenSpaceGuid.PcdCpuSmmStackSize|0x20000

  ## Pcd for OptionRom.
  #  Image verification policy settings:
  #  ALWAYS_EXECUTE                         0x00000000
  #  NEVER_EXECUTE                          0x00000001
  #  ALLOW_EXECUTE_ON_SECURITY_VIOLATION    0x00000002
  #  DEFER_EXECUTE_ON_SECURITY_VIOLATION    0x00000003
  #  DENY_EXECUTE_ON_SECURITY_VIOLATION     0x00000004
  #  QUERY_USER_ON_SECURITY_VIOLATION       0x00000005
  gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy | 0x00000004
!endif

[PcdsDynamicExDefault.common.DEFAULT]
!if $(TPM_ENABLED) == TRUE
  ## Put Ptt guid here: e.g. { 0xf9c6a62f, 0xc60f, 0x4b44, { 0xa6, 0x29, 0xed, 0x3d, 0x40, 0xae, 0xfa, 0x5f } }
  ## TPM1.2 { 0x8b01e5b6, 0x4f19, 0x46e8, { 0xab, 0x93, 0x1c, 0x53, 0x67, 0x1b, 0x90, 0xcc } }
  ## TPM2.0Dtpm { 0x286bf25a, 0xc2c3, 0x408c, { 0xb3, 0xb4, 0x25, 0xe6, 0x75, 0x8b, 0x73, 0x17 } }
  ## TPM2.0Ptt { 0x72cd3a7b, 0xfea5, 0x4f5e, { 0x91, 0x65, 0x4d, 0xd1, 0x21, 0x87, 0xbb, 0x13 } }
  #gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0xb6, 0xe5, 0x01, 0x8b, 0x19, 0x4f, 0xe8, 0x46, 0xab, 0x93, 0x1c, 0x53, 0x67, 0x1b, 0x90, 0xcc}
  #gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0x7b, 0x3a, 0xcd, 0x72, 0xA5, 0xFE, 0x5e, 0x4f, 0x91, 0x65, 0x4d, 0xd1, 0x21, 0x87, 0xbb, 0x13}
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0x5a, 0xf2, 0x6b, 0x28, 0xc3, 0xc2, 0x8c, 0x40, 0xb3, 0xb4, 0x25, 0xe6, 0x75, 0x8b, 0x73, 0x17}
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmAutoDetection|FALSE

  gEfiSecurityPkgTokenSpaceGuid.PcdTrEEAvailableEventLogs|0x00000001
  gEfiSecurityPkgTokenSpaceGuid.PcdTrEEHashAlgorithmBitmap|0
  gEfiSecurityPkgTokenSpaceGuid.PcdTrEESupportedEventLogs|0xFFFFFFFF
  gEfiSecurityPkgTokenSpaceGuid.PcdTrEEProtocolVersion|0x0001

  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2InitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2SelfTestPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2ScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmScrtmPolicy|1
!endif

[PcdsDynamicDefault.common.DEFAULT]
 !if $(TPM_ENABLED) == TRUE
  gClientCommonModuleTokenSpaceGuid.PcdTpm2HierarchyChangeAuthPlatform|1
  gClientCommonModuleTokenSpaceGuid.PcdTpm2HierarchyControlPlatform|1
  gClientCommonModuleTokenSpaceGuid.PcdTpm2HierarchyControlEndorsement|1
  gClientCommonModuleTokenSpaceGuid.PcdTpm2HierarchyControlOwner|1
  gClientCommonModuleTokenSpaceGuid.PcdTpm2ChangeEps|0
  gClientCommonModuleTokenSpaceGuid.PcdTpm2ChangePps|0
  gClientCommonModuleTokenSpaceGuid.PcdTpm2Clear|0
!endif

##################################################################################
#
# SMBIOS
#
##################################################################################
[PcdsPatchableInModule.common]
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSBiosVersion|"Cherryview Platform BIOS"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSSystemUuid|{0xa5, 0x00, 0x02, 0x88, 0x64, 0x62, 0x45, 0x24, 0x98, 0x6a, 0x9b, 0x77, 0x37, 0xe3, 0x15, 0xcf}
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSSystemManufacturer|"Intel Corporation"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSSystemVersion|"0.1"|VOID* |0x40
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSSystemSerialNumber|"112233445566"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSBoardManufacturer|"Intel Corp."|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSBoardVersion|"FAB"|VOID* |0x20
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSBoardSerialNumber|"1"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSChassisManufacturer|"Intel Corporation"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSChassisVersion|"0.1"|VOID* |0x20
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSChassisSerialNumber|"serial"|VOID* |0x80
  gPlatformModuleTokenSpaceGuid.PcdSMBIOSChassisAssetTag|"Asset Tag"|VOID* |0x20

##################################################################################
#
# Console & HII
#
##################################################################################
[PcdsDynamicExDefault.common.DEFAULT]
  ## This PCD defines the video horizontal resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|0
  ## This PCD defines the video vertical resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|0

  ## This PCD defines the Console output column and the default value is 25 according to UEFI spec.
  #  This PCD could be set to 0 then console output could be at max column and max row.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|31|UINT32|0x40000006
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|31
  ## This PCD defines the Console output row and the default value is 80 according to UEFI spec.
  #  This PCD could be set to 0 then console output could be at max column and max row.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|100|UINT32|0x40000007
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|100

  ## The PCD is used to specify the video horizontal resolution of text setup.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|800|UINT32|0x50000001
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|800
  ## The PCD is used to specify the video vertical resolution of text setup.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|600|UINT32|0x50000002
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|600
  ## The PCD is used to specify the console output column of text setup.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|100|UINT32|0x50000003
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|100
  ## The PCD is used to specify the console output column of text setup.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|31|UINT32|0x50000004
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|31

  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVbeEnable|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVgaEnable|TRUE

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserSubtitleTextColor|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextColor|0x01

[PcdsDynamicHii.common.DEFAULT]

  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|L"ConOutConfig"|gClientCommonModuleTokenSpaceGuid|0x0|100
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|L"ConOutConfig"|gClientCommonModuleTokenSpaceGuid|0x4|31

[PcdsDynamicExDefault.common.DEFAULT]
  gEfiCHVTokenSpaceGuid.PcdBmpImageGuid |{0x87, 0x8A, 0xC2, 0xCC, 0x53, 0x43, 0x46, 0xF2, 0xB5, 0x63, 0x51, 0xF8, 0x9D, 0xAF, 0x56, 0xBA}

[PcdsPatchableInModule.common]
  gPlatformModuleTokenSpaceGuid.PcdCustomizedVbtFile|{0xFF, 0xFF, 0xFF, 0xFF} |4  # Reserve n bytes for customized vbt

[PcdsFeatureFlag.common]
gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserGrayOutTextStatement|TRUE

##################################################################################
#
# Variable
#
##################################################################################
[PcdsFeatureFlag.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|FALSE

[PcdsFixedAtBuild.common]
!if $(SECURE_BOOT_ENABLE) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x22000
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x7000
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x00000800
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x400

[PcdsPatchableInModule.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|0

##################################################################################
#
# Boot
#
##################################################################################
[PcdsFeatureFlag.common]
  # IA32 and X64 select
   gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|FALSE

  # Recovery
  gEfiMdeModulePkgTokenSpaceGuid.PcdRecoveryOnIdeDisk|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdRecoveryOnFatFloppyDisk|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdRecoveryOnDataCD|FALSE

  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE

[PcdsPatchableInModule.common]
  ## This PCD specifies whether to use the optimized timing for best PS2 detection performance.
  #  Note this PCD could be set to TRUE for best boot performance and set to FALSE for best device compatibility.
  #gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdFastPS2Detection|TRUE|BOOLEAN|0x3000000b
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdFastPS2Detection|TRUE

  !if $(ENABLE_FAST_BOOT)
    gPlatformModuleTokenSpaceGuid.PcdEnableFastBoot|TRUE
  !else
    gPlatformModuleTokenSpaceGuid.PcdEnableFastBoot|FALSE
  !endif

[PcdsDynamicExHii.common.DEFAULT]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|5 # Variable: L"Timeout"
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"HwErrRecSupport"
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBootState|L"BootState"|gEfiBootStateGuid|0x0|TRUE

[PcdsDynamicExDefault.common.DEFAULT]
  gPlatformModuleTokenSpaceGuid.PcdBootToFirmwareUserInterface|FALSE

##################################################################################
#
# ACPI
#
##################################################################################
[PcdsFixedAtBuild.common]
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdS3AcpiReservedMemorySize|0x200000
[PcdsDynamicExDefault.common.DEFAULT]
  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0

[PcdsFeatureFlag.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFixedAtBuild.common]
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBusNumber|0
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciDeviceNumber|31
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciFunctionNumber|0
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciEnableRegisterOffset|0x40
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoBarEnableMask|0x02
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBarRegisterOffset|0x40
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddress|0x400
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiPm1TmrOffset|0x08
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddressMask|0xFF80

[PcdsPatchableInModule.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"Intel "
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x20202020324B4445

##################################################################################
#
# SMM
#
##################################################################################
[PcdsFeatureFlag.common]
  gEfiCpuTokenSpaceGuid.PcdCpuSmmBlockStartupThisAp|TRUE
  gEfiCpuTokenSpaceGuid.PcdCpuSmmProfileEnable|TRUE


[PcdsFeatureFlag.X64]
  gEfiCpuTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE

[PcdsFixedAtBuild.common]
  gEfiCpuTokenSpaceGuid.PcdCpuSmmApSyncTimeout|1000

##################################################################################
#
# Misc
#
##################################################################################
[PcdsFeatureFlag.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFrameworkCompatibilitySupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreImageLoaderSearchTeSectionFirst|FALSE
  ## This PCD specifies whether PS2 keyboard does a extended verification during start.
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPs2KbdExtendedVerification|FALSE
  ## This PCD specifies whether PS2 mouse does a extended verification during start.
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPs2MouseExtendedVerification|FALSE

[PcdsFixedAtBuild.common]
  gChvRefCodePkgTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x10000
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxFvSupported|7
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeimPerFv|50
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPpiSupported|128
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE

!if $(PERFORMANCE_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
!else
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
!endif

  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdEbdaReservedMemorySize|0x10000
  gEfiMdeModulePkgTokenSpaceGuid.PcdLoadModuleAtFixAddressEnable|$(TOP_MEMORY_ADDRESS)
  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0

[PcdsPatchableInModule.common]
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0x$(PLATFORM_PCIEXPRESS_BASE)
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdLegacyBiosCacheLegacyRegion|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|0x00010000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Intel Corporation"|VOID* |0x80
  gEfiEdkIIPlatformTokenSpaceGuid.PcdSsidSvid|0x12348086

[PcdsDynamicExDefault.common.DEFAULT]
  gEfiEdkIIPlatformTokenSpaceGuid.PcdSystemConfiguration|{0x0}|VOID*|0x3A0
  gEfiEdkIIPlatformTokenSpaceGuid.PcdPlatformInfo|{0x0}|VOID*|0x100








