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
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses.common]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  DxeSmmDriverEntryPoint|IntelFrameworkPkg/Library/DxeSmmDriverEntryPoint/DxeSmmDriverEntryPoint.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf

  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  GenericBdsLib|$(PLATFORM_PACKAGE)/Common/Override/IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf


  #
  # Generic Modules
  #
!if $(USB_ENABLE) == TRUE
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
!endif
!if $(SCSI_ENABLE) == TRUE
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
!endif
!if $(NETWORK_ENABLE) == TRUE
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
!endif
!if $(S3_ENABLE) == TRUE
  S3Lib|IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
!endif
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
!if $(CAPSULE_ENABLE) == TRUE
  CapsuleLib|IntelFrameworkModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
!else
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!endif
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf

  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf

  #
  # CPU
  #
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf

  #
  # SMM
  #
  SmmLib|MdePkg/Library/SmmLibNull/SmmLibNull.inf

  #
  # Misc
  #
!if $(TARGET) == RELEASE
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
!endif
  PlatformHookLib|BraswellPlatformPkg/Common/Library/PlatformHookLib/PlatformHookLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
!if $(TPM_ENABLED) == TRUE
  TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
  Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf

!endif
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf

  #
  # CryptLib
  #
!if $(TPM_ENABLED) == TRUE
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
!endif


  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf

  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
!if ($(HYBRID_ENABLE) == TRUE)|| ($(FTPM_ENABLE) == TRUE)
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
!endif
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  TrEEPhysicalPresenceLib|SecurityPkg/Library/DxeTrEEPhysicalPresenceLib/DxeTrEEPhysicalPresenceLib.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  TrEEPpVendorLib|SecurityPkg/Library/TrEEPpVendorLibNull/TrEEPpVendorLibNull.inf



  # FSP lib
  FspApiLib|IntelFspWrapperPkg/Library/BaseFspApiLib/BaseFspApiLib.inf

  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf

[LibraryClasses.IA32]

  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/BaseAcpiTimerLib.inf

[LibraryClasses.IA32.SEC]
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/BaseAcpiTimerLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SecPeiCpuExceptionHandlerLib.inf  # RP_BDW_CorePkgDifference
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf


[LibraryClasses.IA32.PEIM, LibraryClasses.IA32.PEI_CORE]
  #
  # PEI phase common
  #
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf

!if $(TPM_ENABLED) == TRUE
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterPei.inf
!endif
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/BaseAcpiTimerLib.inf

!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
!endif

!if $(TARGET) == RELEASE
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
!else
  DebugLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!endif
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf

HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf

[LibraryClasses.X64]
  #
  # DXE phase common
  #
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf

  TcgPhysicalPresenceLib|SecurityPkg/Library/DxeTcgPhysicalPresenceLib/DxeTcgPhysicalPresenceLib.inf

!if $(TPM_ENABLED) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
!endif

  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf

  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf

!if $(TPM_ENABLED) == TRUE
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
!endif
  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf

  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf  # RP_BDW_CorePkgDifference
CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf  # RP_BDW_CorePkgDifference
  CustomizedDisplayLib|MdeModulePkg\Library\CustomizedDisplayLib\CustomizedDisplayLib.inf

[LibraryClasses.X64.PEIM]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf


[LibraryClasses.X64.DXE_DRIVER]
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif

[LibraryClasses.X64.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
!endif
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf  # RP_BDW_CorePkgDifference

[LibraryClasses.X64.DXE_SMM_DRIVER]
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
!endif
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf

  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf  # RP_BDW_CorePkgDifference

[LibraryClasses.X64.SMM_CORE]
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf

!if $(TPM_ENABLED) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
!endif
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
!endif

[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!if $(TPM_ENABLED) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!endif

[LibraryClasses.X64.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf


[Components.IA32]



  MdeModulePkg/Core/Pei/PeiMain.inf {
!if $(TARGET) == DEBUG
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2E
!endif
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
  }
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf

!if $(TPM_ENABLED) == TRUE
  SecurityPkg/Tcg/PhysicalPresencePei/PhysicalPresencePei.inf
  SecurityPkg/Tcg/TcgPei/TcgPei.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
}
!endif

  MdeModulePkg/Universal/PCD/Pei/Pcd.inf

  UefiCpuPkg/CpuIoPei/CpuIoPei.inf
  UefiCpuPkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf

!if $(CAPSULE_ENABLE) == TRUE
  MdeModulePkg/Universal/CapsulePei/CapsulePei.inf
!endif
  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
    NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }

!if ($(TPM_ENABLED) == TRUE)
   SecurityPkg/Tcg/TrEEConfig/TrEEConfigPei.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }

   SecurityPkg/Tcg/TrEEPei/TrEEPei.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
!if $(FTPM_ENABLE) == TRUE
      NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
!endif
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }

!if $(FTPM_ENABLE) == TRUE
  ChvRefCodePkg/Txe/fTPM/Pei/fTPMInitPei.inf
!endif

!endif

!if $(ACPI50_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTablePei/FirmwarePerformancePei.inf{
    <LibraryClasses>
      TimerLib|PerformancePkg/Library/TscTimerLib/PeiTscTimerLib.inf
  }

!endif



[Components.X64]
  #
  # EDK II Related Platform codes
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }
  IntelFrameworkModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xF0000043
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  }

  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }



  IntelFrameworkModulePkg/Universal/CpuIoDxe/CpuIoDxe.inf
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf

!if $(DXE_ARCHITECTURE) == X64
!if $(CAPSULE_ENABLE) == TRUE
  MdeModulePkg/Universal/CapsulePei/CapsuleX64.inf {
    <LibraryClasses>
      CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  }
!endif
!endif

  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf  {
    <LibraryClasses>
!if $(TARGET) != RELEASE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf

  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf{
    <LibraryClasses>
!if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
!endif
!if ($(FTPM_ENABLE) == TRUE) || ($(TPM_ENABLED) == TRUE)
      NULL|SecurityPkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
!endif
  }

  MdeModulePkg/Universal/Metronome/Metronome.inf


  IntelFspWrapperPkg/FspNotifyDxe/FspNotifyDxe.inf

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf


  #
  # Authenticated variable modules
  #
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPcdLib/VarCheckPcdLib.inf
  }
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf  {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPcdLib/VarCheckPcdLib.inf
  }
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf{
  <LibraryClasses>
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
      AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
      AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPcdLib/VarCheckPcdLib.inf
}
  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf

  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf{
     <LibraryClasses>
          FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  }
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

!if $(DATAHUB_ENABLE) == TRUE
  IntelFrameworkModulePkg/Universal/DataHubDxe/DataHubDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|0
  }
!endif
  MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf

  PcAtChipsetPkg/HpetTimerDxe/HpetTimerDxe.inf

!if ($(TPM_ENABLED) == TRUE) || ($(FTPM_ENABLE) == TRUE)
  SecurityPkg/Tcg/MemoryOverwriteControl/TcgMor.inf
  SecurityPkg/Tcg/TrEEDxe/TrEEDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
  }
!endif

!if $(TPM_ENABLED) == TRUE
    SecurityPkg/Tcg/TrEESmm/TrEESmm.inf
    SecurityPkg/Tcg/TrEEConfig/TrEEConfigDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
!endif


  #
  # SMM
  #
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf


  #
  # ACPI
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  #
  # PCI
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

#
# ISA
#
#  WinbondSioPkg/Wpce791/Wpce791.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaIoDxe/IsaIoDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf
# IntelSiliconBasic/SerialDxe/SerialDxe.inf

#
# SDIO
#
!if $(ACPI50_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf {
    <LibraryClasses>
      TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf
  }
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf {
    <LibraryClasses>
      TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf
  }
  MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf
!endif

#
# IDE/SCSI/AHCI
#
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
!if $(SCSI_ENABLE) == TRUE
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
!endif
#
# Console
#
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  #
  # USB
  #
!if $(USB_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf {
    <BuildOptions>
      *_*_*_CC_FLAGS = $(C_PROJ_FLAGS) /Od /D EFI_DEBUG /GL-
  }
!endif

  #
  # SMBIOS
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  MdeModulePkg/Universal/SectionExtractionDxe/SectionExtractionDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
      NULL|SecurityPkg/Library/DxeRsa2048Sha256GuidedSectionExtractLib/DxeRsa2048Sha256GuidedSectionExtractLib.inf
  }

  #
  # Legacy Modules
  #
  PcAtChipsetPkg/8259InterruptControllerDxe/8259.inf

  #
  # Network Modules
  #
!if $(NETWORK_ENABLE) == TRUE
  !if $(NETWORK_ISCSI_ENABLE) == TRUE
    !if $(NETWORK_IP6_ENABLE) == TRUE
      NetworkPkg/IScsiDxe/IScsiDxe.inf
    !else
      MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
    !endif
  !endif
  !if $(NETWORK_VLAN_ENABLE) == TRUE
    MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  !endif
  !if $(CSM_ENABLE) == TRUE
    IntelFrameworkModulePkg/Csm/BiosThunk/Snp16Dxe/Snp16Dxe.inf
  !endif
!endif

!if $(NETWORK_ENABLE) == TRUE
  #
  # UEFI network modules
  #
    MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
    MdeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf

    MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
    MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
    MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
    MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
    MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
    MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf {
      <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
    }
    MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
    !if $(NETWORK_IP6_ENABLE) == TRUE
      NetworkPkg/Ip6Dxe/Ip6Dxe.inf
      NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
      NetworkPkg/IpSecDxe/IpSecDxe.inf
      NetworkPkg/TcpDxe/TcpDxe.inf
      NetworkPkg/Udp6Dxe/Udp6Dxe.inf
      NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
    !endif
    !if $(NETWORK_IP6_ENABLE) == TRUE
      NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
    !else
      MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
    !endif
!endif

!if $(CAPSULE_ENABLE) == TRUE
  # capsule related drivers and Application.
  IntelFrameworkModulePkg/Universal/FirmwareVolume/FwVolDxe/FwVolDxe.inf
  IntelFrameworkModulePkg/Universal/FirmwareVolume/UpdateDriverDxe/UpdateDriverDxe.inf
!endif

!if $(PERFORMANCE_ENABLE) == TRUE
  PerformancePkg/Dp_App/Dp.inf {
    <LibraryClasses>
      SortLib|ShellPkg/Library/UefiSortLib/UefiSortLib.inf
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      FileHandleLib|ShellPkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
      PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  }
!endif

