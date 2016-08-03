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
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = BraswellPlatformPkg
  PLATFORM_GUID                       = 465B0A0B-7AC1-443b-8F67-7B8DEC145F90
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005

  #
  # Set platform specific package/folder name, same as passed from PREBUILD script.
  # PLATFORM_PACKAGE would be the same as PLATFORM_NAME as well as package build folder
  # DEFINE only takes effect at R9 DSC and FDF.
  #
  DEFINE      PLATFORM_PACKAGE                = BraswellPlatformPkg
  DEFINE      PLATFORM2_PACKAGE               = BraswellPlatformPkg
  DEFINE      PLATFORM_RC_PACKAGE             = ChvRefCodePkg
  DEFINE      CLIENT_COMMON_PACKAGE           = ClientCommonPkg
  DEFINE      PLATFORM_AMI_CSM_PACKAGE        = Vlv2DeviceAmiCsmPkg
  DEFINE      PLATFORM_INTEL_RESTRICT_PACKAGE = ChvTbltDeviceMiscPkg
  OUTPUT_DIRECTORY                    = Build/$(PLATFORM_PACKAGE)
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT
  VPD_TOOL_GUID                       = 8C3D856A-9BE6-468E-850A-24F7A8D38E08

  DEFINE CPU_ARCH                  = ValleyView2
  DEFINE PROJECT_SC_FAMILY         = IntelPch

  DEFINE PROJECT_SC_ROOT           = ../$(PLATFORM_RC_PACKAGE)/CherryViewSoc/SouthCluster
  DEFINE PROJECT_VLV_ROOT          = ../$(PLATFORM_RC_PACKAGE)/CherryViewSoc/NorthCluster
  DEFINE CHIPSET_MEMORY_CONTROLLER = ValleyView2

  !include $(WORKSPACE)/BraswellPlatformPkg/AutoPlatformCFG.txt
  !include $(WORKSPACE)/BraswellPlatformPkg/PlatformFeatureConfig.dsc

!if $(ENABLE_FV_BIN)
  FLASH_DEFINITION                    = $(WORKSPACE)/BraswellPlatformPkg/PlatformFvBinPkg.fdf
!else
  FLASH_DEFINITION                    = $(WORKSPACE)/BraswellPlatformPkg/PlatformPkg.fdf
!endif
  FIX_LOAD_TOP_MEMORY_ADDRESS         = 0x0
  DEFINE   TOP_MEMORY_ADDRESS         = 0x0

  #
  # Do not use 0x prefix, pass prefix 0x or postfix h through macro for C or ASM
  #
  DEFINE   PLATFORM_PCIEXPRESS_BASE   = 0E0000000

  DEFINE PCH_INIT_ROOT          = ChvRefCodePkg/CherryViewSoc/SouthCluster/PchInit
  DEFINE DXE_ARCHITECTURE       = X64
  DEFINE EDK_DXE_ARCHITECTURE   = X64

  !if $(ENABLE_FAST_BOOT)
    DEFINE PERFORMANCE_ENABLE = TRUE
  !endif

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
!include PlatformPcdX64.dsc

################################################################################
#
# EDK2 Core
#
################################################################################
!include Include/Build/Edk2CorePkgX64.dsc

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses.common]

  FlashDeviceLib|$(PLATFORM_PACKAGE)/Common/Library/FlashDeviceLib/FlashDeviceLib.inf
  #
  # Framework
  #
!if $(S3_ENABLE) == TRUE
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
!else
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
!endif
  S3IoLib|MdePkg/Library/BaseS3IoLib/BaseS3IoLib.inf
  S3PciLib|MdePkg/Library/BaseS3PciLib/BaseS3PciLib.inf

  #
  # CPU
  #
  CpuConfigLib|BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/Library/CpuConfigLib/CpuConfigLib.inf
  CpuIA32Lib|$(PLATFORM_PACKAGE)/Common/Library/CpuIA32Lib/CpuIA32Lib.inf
  StallSmmLib|$(PLATFORM_PACKAGE)/Common/Library/StallSmmLib/StallSmmLib.inf

  #
  # ICH
  #
  SmbusLib|ChvRefCodePkg/CherryViewSoc/SouthCluster/Library/SmbusLib/SmbusLib.inf
  SmmIoLib|$(PLATFORM_PACKAGE)/Common/Library/SmmIo/SmmIoLib.inf
  ResetSystemLib|$(PLATFORM_PACKAGE)/Common/Library/ResetSystemLib/ResetSystemLib.inf

  #
  # RC Common Library
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgCommonLib.dsc

  #
  # FSP platform sample
  #
  FspPlatformInfoLib|$(PLATFORM_PACKAGE)/Common/FspSupport/Library/FspPlatformInfoLib/FspPlatformInfoLib.inf
  FspPlatformSecLib|IntelFspWrapperPkg/Library/SecPeiFspPlatformSecLibSample/SecPeiFspPlatformSecLibSample.inf
  FspHobProcessLib|$(PLATFORM_PACKAGE)/Common/FspSupport/Library/PeiFspHobProcessLib/PeiFspHobProcessLib.inf
  BltLib|$(PLATFORM_PACKAGE)/Common/FspSupport/Library/FrameBufferBltLib/FrameBufferBltLib.inf

  RecoveryOemHookLib|$(PLATFORM_PACKAGE)/Common/Library/RecoveryOemHookLib/RecoveryOemHookLib.inf

[LibraryClasses.IA32]
  #
  # RC Pei Library
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgPeiLib.dsc

  GpioLib|ChvRefCodePkg/CherryViewSoc/SouthCluster/Library/GpioLib/GpioLib.inf

[LibraryClasses.IA32.SEC]



[LibraryClasses.IA32.PEIM, LibraryClasses.IA32.PEI_CORE]
  #
  # PEI phase common
  #
  BiosIdLib|BraswellPlatformPkg/Common/Library/BiosIdLib/BiosIdLib.inf
  I2CLibPei|ChvRefCodePkg/CherryViewSoc/SouthCluster/Library/I2CLibPei/I2CLibPei.inf
  MultiPlatformLib|$(PLATFORM_PACKAGE)/Common/Library/MultiPlatformLib/MultiPlatformLib.inf
  PeiKscLib|$(PLATFORM_PACKAGE)/Common/Library/Ksc/Pei/PeiKscLib.inf

[LibraryClasses.X64]

  BiosIdLib|BraswellPlatformPkg/Common/Library/BiosIdLib/BiosIdLib.inf
  DxeKscLib|$(PLATFORM_PACKAGE)/Common/Library/Ksc/Dxe/DxeKscLib.inf
  EfiRegTableLib|$(PLATFORM_PACKAGE)/Common/Library/EfiRegTableLib/EfiRegTableLib.inf

  #
  # RC Dxe Library
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgDxeLib.dsc


[LibraryClasses.X64.PEIM]


[LibraryClasses.X64.DXE_DRIVER]


[LibraryClasses.X64.DXE_CORE]


[LibraryClasses.X64.DXE_SMM_DRIVER]

  SmmCpuPlatformHookLib|BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf

  #
  # RC Smm Library
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgSmmLib.dsc
  SmmIoLib|$(PLATFORM_RC_PACKAGE)/CherryViewSoc/CPU/SampleCode/Library/SmmIo/SmmIo.inf
  SmmKscLib|$(PLATFORM_RC_PACKAGE)/CherryViewSoc/CPU/SampleCode/Library/Ksc/Smm/SmmKscLib.inf
  DTSHookLib|$(PLATFORM_RC_PACKAGE)/CherryViewSoc/CPU/SampleCode/Library/DTSHookLib/Smm/DTSHookLib.inf

[LibraryClasses.X64.SMM_CORE]

[LibraryClasses.X64.UEFI_APPLICATION]

[Components.IA32]

  IntelFspWrapperPkg/FspWrapperSecCore/FspWrapperSecCore.inf {
  <LibraryClasses>
    FspPlatformInfoLib|$(PLATFORM_PACKAGE)/Common/FspSupport/Library/FspPlatformInfoLib/FspPlatformInfoLib.inf
    DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  <PcdsPatchableInModule>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xF0000043
  <PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  }

  IntelFspWrapperPkg/FspInitPei/FspInitPei.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }


  #
  # RC PEI Component
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgPei.dsc

  $(PLATFORM_PACKAGE)/Common/PlatformPei/PlatformPei.inf {
    <LibraryClasses>
       NULL|BraswellPlatformPkg\Board\BraswellCR\BoardInit\BoardInit.inf
       NULL|BraswellPlatformPkg\Board\BraswellCherryHill\BoardInit\BoardInit.inf
       NULL|BraswellPlatformPkg\Board\WesternDigital\BoardInit\BoardInit.inf
!if ($(FTPM_ENABLED) == TRUE)
      fTPMInitLib|ChvRefCodePkg/Txe/fTPM/Pei/fTPMInitPei.inf
      !endif
!if $(TARGET) == DEBUG
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf

!if $(ISH_ENABLE) == TRUE
      IshInitLib|ChvRefCodePkg/Txe/ISH/Pei/IshInit.inf
!endif
    <BuildOptions>
!if $(SeC_DEBUG_INFO_ENABLE) == TRUE
      *_*_IA32_CC_FLAGS      = /DSeC_DEBUG_INFO=1
!else
      *_*_IA32_CC_FLAGS      = /DSeC_DEBUG_INFO=0
!endif
!if $(FTPM_ENABLE)==TRUE
      *_*_IA32_CC_FLAGS = /D FTPM_ENABLE
!endif
!if $(ISH_ENABLE) == TRUE
      *_*_IA32_CC_FLAGS      = /DISH_ENABLE=1
!endif
  }


  $(PLATFORM_PACKAGE)/Common/PlatformPei/PostSilicon/PostSiliconInit.inf {
    <LibraryClasses>
!if $(TARGET) != RELEASE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }

  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/PiSmmCommunication/PiSmmCommunicationPei.inf

  #
  # Recovery Module
  #
  ChvRefCodePkg/CherryViewSoc/SouthCluster/Usb/Pei/PchUsb.inf
  MdeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
  MdeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  MdeModulePkg/Bus/Usb/UsbBotPei/UsbBotPei.inf

[Components.X64]

  BraswellPlatformPkg/Common/PcdConfigHook/DxePcdConfigHook.inf
  MdeModulePkg/Universal/Acpi/S3SaveStateDxe/S3SaveStateDxe.inf

  #
  # RC DXE Component
  #
  !include $(WORKSPACE)/$(PLATFORM_RC_PACKAGE)/ClientRefCodePkgDxe.dsc

  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/CpuInit/MpCpu.inf
  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/PciHostBridge/PciHostBridge.inf



  IntelFrameworkModulePkg/Universal/BdsDxe/BdsDxe.inf{
    <LibraryClasses>
     OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
     IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
     PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
     BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
     PlatformBdsLib|$(PLATFORM_PACKAGE)/Common/Library/PlatformBdsLib/PlatformBdsLib.inf
     Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
!if $(TPM_ENABLED) == TRUE
     NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
!endif
!if $(FTPM_ENABLE) == TRUE
     NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
!endif
}

  BraswellPlatformPkg/Common/Flash/SpiFlashParts/MX25/MX25.inf

!if $(SMM_VARIABLE_ENABLE) == TRUE
  BraswellPlatformPkg/Common/Flash/SpiDeviceDxe/SpiDeviceSmm.inf
  BraswellPlatformPkg/Common/Flash/SpiDeviceDxe/SpiDeviceSmmDxe.inf
  BraswellPlatformPkg/Common/Flash/FvbRuntimeDxe/FvbSmm.inf
  BraswellPlatformPkg/Common/Flash/FvbRuntimeDxe/FvbSmmDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf
!else
  BraswellPlatformPkg/Common/SpiDeviceDxe/SpiDeviceDxe.inf
  BraswellPlatformPkg/Common/FvbRuntimeDxe/FvbRuntimeDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
!endif

  $(PLATFORM_PACKAGE)/Common/Setup/PlatformSetupDxe.inf

  #
  # EDK II Related Platform codes
  #
  $(PLATFORM_PACKAGE)/Common/Acpi/AcpiSmm/AcpiSmm.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xF0000043
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x3F
  }

  $(PLATFORM_PACKAGE)/Common/PlatformSmm/PlatformSmm.inf

  $(PLATFORM_PACKAGE)/Common/PlatformDxe/PlatformDxe.inf {
    <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
!if $(TPM_ENABLED) == TRUE
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
!endif
!if $(FTPM_ENABLE) == TRUE
      NULL|ChvRefCodePkg/Txe/Library/Tpm2DeviceLibSeC/Tpm2InstanceLibFtpm.inf
!endif
    <BuildOptions>
     !if $(HYBRID_ENABLE) == TRUE
       *_*_X64_CC_FLAGS = /D _PSSD_FIX_=1
     !else
       *_*_X64_CC_FLAGS = /D _PSSD_FIX_=0
     !endif
     !if $(EMMC_DRIVER_ENABLE) == TRUE
       *_*_X64_CC_FLAGS = /D EMMC_ENABLE=1
     !else
       *_*_X64_CC_FLAGS = /D EMMC_ENABLE=0
     !endif
     !if $(FSP_BOOT_ENABLE) == TRUE
       *_*_X64_CC_FLAGS       = -U FSP_FLAG
       *_*_IA32_CC_FLAGS      = -U FSP_FLAG
     !endif
  }

  $(PLATFORM_PACKAGE)/Common/Feature/PciPlatform/PciPlatform.inf

!if $(GOP_DRIVER_ENABLE) == TRUE
  $(PLATFORM_PACKAGE)/Common/FspSupport/GraphicsOutputDxe/GraphicsOutputDxe.inf  {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  }
!endif


  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x3F
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  }

  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/PiSmmCommunication/PiSmmCommunicationSmm.inf

  #
  # ACPI
  #
   MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xF0000043
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  }

  $(PLATFORM_PACKAGE)/Common/Acpi/AcpiPlatform/AcpiPlatform.inf {
    <BuildOptions>
       *_*_X64_CC_FLAGS = /D SUPPORT_64BIT_D=1
  }
  $(PLATFORM_PACKAGE)/Board/BraswellCR/Acpi/Acpi.inf
  $(PLATFORM_PACKAGE)/Board/BraswellCherryHill/Acpi/Acpi.inf


  #
  # ISA
  #
  BraswellPlatformPkg/Common/Silicon/WinbondSio/Wpce791/Wpce791.inf


  BraswellPlatformPkg/Common/Silicon/IntelSiliconBasic/SerialDxe/SerialDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SerialPortLib|BraswellPlatformPkg/Common/Library/PlatformBaseSerialPortLib16550/BaseSerialPortLib16550.inf
  }

  #
  # SMBIOS
  #
  $(PLATFORM_PACKAGE)/Common/Feature/SmBiosMiscDxe/SmbiosMiscDxe.inf

  #
  # CPU/FW Microde
  #
  $(PLATFORM2_PACKAGE)/Common/Microcode/MicrocodeUpdatesFsp.inf


######################################################################
#
# Define Build Options both for EDK and EDKII drivers.
#
######################################################################
  !include Include/Build/BuildOptions.dsc

