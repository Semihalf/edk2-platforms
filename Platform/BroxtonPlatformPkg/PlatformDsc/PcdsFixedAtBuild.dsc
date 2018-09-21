## @file
#  Platform Fixed At Build Pcd Description.
#
#  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution. The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  # PcdTemporaryRamBase + PcdTemporaryRamSize should equal top of CAR
  # Top of CAR (CarBase + CarSize) = 0xFF000000 on BXT
  !if $(PERFORMANCE_ENABLE) == TRUE
     gPlatformModuleTokenSpaceGuid.PcdTemporaryRamBase|0xFEFC0000
     gPlatformModuleTokenSpaceGuid.PcdTemporaryRamSize|0x00040000
  !else
     gPlatformModuleTokenSpaceGuid.PcdTemporaryRamBase|0xFEFD5100
     gPlatformModuleTokenSpaceGuid.PcdTemporaryRamSize|0x0002AF00
  !endif

  gPlatformModuleTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x2000

  !if $(INTEL_FPDT_ENABLE) == TRUE
     gEfiMdeModulePkgTokenSpaceGuid.PcdExtFpdtBootRecordPadSize|0x1000
  !endif

  !if $(SECURE_BOOT_ENABLE) == TRUE
     gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
  !else
     gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x5000
  !endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x00000800
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x400

  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdS3AcpiReservedMemorySize|0x1800000
  gEfiMdeModulePkgTokenSpaceGuid.PcdShadowPeimOnS3Boot|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeimPerFv|50
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPpiSupported|128

  !if $(S4_ENABLE) == TRUE
     gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|TRUE
  !else
     gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  !endif
  !if $(TARGET) == RELEASE
     gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
     gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x3
  !else
     gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
     gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  !endif
  !if $(PERFORMANCE_ENABLE) == TRUE
     gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
     !if $(INTEL_FPDT_ENABLE) == TRUE
        gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|120
     !else
        gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|60
     !endif
  !endif

  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdEbdaReservedMemorySize|0x10000
  gEfiMdeModulePkgTokenSpaceGuid.PcdLoadModuleAtFixAddressEnable|$(TOP_MEMORY_ADDRESS)
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0x$(PLATFORM_PCIEXPRESS_BASE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserSubtitleTextColor|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextColor|0x01
  gClientCommonModuleTokenSpaceGuid.PcdMaxConsoleDeviceNum|0x05

  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVbeEnable|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVgaEnable|TRUE

  gEfiBxtTokenSpaceGuid.PcdPmcGcrBaseAddress|0xFE043000

  !if $(SOURCE_DEBUG_ENABLE) == TRUE
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE
  !endif
  
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackSize|0x3000
  
  !if $(UP2_BOARD) == TRUE
    gPlatformModuleTokenSpaceGuid.PcdSerialIoUartNumber|0
  !else 
    gPlatformModuleTokenSpaceGuid.PcdSerialIoUartNumber|2
  !endif
  
  !if $(MINNOW3_MODULE_BOARD) == TRUE
    #
    # I2C bus the master EEPROM is hanging on
    gPlatformModuleTokenSpaceGuid.PcdEepromBus|0x08
    # 7-bit address of the master EEPROM
    gPlatformModuleTokenSpaceGuid.PcdEepromAddress|0x10
    # EEPROM GPIO Whitelist for MB3M (should think about shifting to SkuId specfic PCD to allow for other platforms).
    #                                                +-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
    #                                                |       SW_GPIO_206     |       SW_GPIO_166     |       SW_GPIO_167     |       SW_GPIO_168     |       SW_GPIO_169     |       SW_GPIO_170     |       SW_GPIO_171     |       SW_GPIO_172     |       SW_GPIO_173     |       SW_GPIO_174     |       SW_GPIO_175     |       SW_GPIO_176     |       SW_GPIO_177     |       SW_GPIO_178     |       SW_GPIO_186     |       SW_GPIO_183     |       SW_SMB_ALERTB   |       SW_SMB_CLK      |       SW_SMB_DATA     |       NW_GPIO_193     |       NW_GPIO_196     |       NW_GPIO_197     |       NW_GPIO_198     |       NW_GPIO_199     |       NW_GPIO_200     |       NW_GPIO_203     |       NW_GPIO_204     |       NW_PMC_SPI_FS1  |       NW_GPIO_84      |       NW_GPIO_84      |       NW_GPIO_85      |       NW_GPIO_86      |       NW_GPIO_87      |       NW_GPIO_88      |       NW_GPIO_97      |       NW_GPIO_98      |       NW_GPIO_99      |       NW_GPIO_100     |       NW_GPIO_103     |       NW_GPIO_104     |       NW_GPIO_105     |       NW_GPIO_106     |       NW_GPIO_109     |       NW_GPIO_110     |       NW_GPIO_112     |       NW_GPIO_113     |       N_GPIO_2        |       N_GPIO_3        |       N_GPIO_4        |       N_GPIO_6        |       N_GPIO_7        |       N_GPIO_13       |       N_GPIO_14       |       N_GPIO_21       |       N_GPIO_22       |       N_GPIO_23       |       N_GPIO_24       |       N_GPIO_25       |       N_GPIO_26       |       N_GPIO_38       |       N_GPIO_39       |       N_GPIO_40       |       N_GPIO_41       |       N_GPIO_42       |       N_GPIO_43       |       N_GPIO_46       |       N_GPIO_47       |       N_GPIO_48       |       N_GPIO_49       |       N_GPIO_64       |       N_GPIO_65       |    N_GPIO_66          |    N_GPIO_67          |       W_GPIO_126      |       W_GPIO_127      |       W_GPIO_132      |       W_GPIO_133      |       W_GPIO_134      |       W_GPIO_135      |       W_GPIO_136      |       W_GPIO_137      |       W_GPIO_146      |       W_GPIO_147      |       W_GPIO_148      |       W_GPIO_149      |       W_OSC_CLK_OUT_0 |     W_PMU_AC_PRESENT  |     W_PMU_BATLOW_B    |     W_PMU_PLTRST_B    |    W_PMU_SLP_S3_B     |  End of array marker  |
    gPlatformModuleTokenSpaceGuid.PcdGpioWhiteList | {0x08, 0x05, 0xC0, 0x00, 0x70, 0x05, 0xC0, 0x00, 0x78, 0x05, 0xC0, 0x00, 0x80, 0x05, 0xC0, 0x00, 0x88, 0x05, 0xC0, 0x00, 0x90, 0x05, 0xC0, 0x00, 0x98, 0x05, 0xC0, 0x00, 0xA0, 0x05, 0xC0, 0x00, 0xA8, 0x05, 0xC0, 0x00, 0xB0, 0x05, 0xC0, 0x00, 0xB8, 0x05, 0xC0, 0x00, 0xC0, 0x05, 0xC0, 0x00, 0xC8, 0x05, 0xC0, 0x00, 0xD0, 0x05, 0xC0, 0x00, 0xE0, 0x05, 0xC0, 0x00, 0xF0, 0x05, 0xC0, 0x00, 0xF8, 0x05, 0xC0, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x08, 0x06, 0xC0, 0x00, 0x30, 0x05, 0xC4, 0x00, 0x48, 0x05, 0xC4, 0x00, 0x50, 0x05, 0xC4, 0x00, 0x58, 0x05, 0xC4, 0x00, 0x60, 0x05, 0xC4, 0x00, 0x68, 0x05, 0xC4, 0x00, 0x80, 0x05, 0xC4, 0x00, 0x88, 0x05, 0xC4, 0x00, 0x98, 0x05, 0xC4, 0x00, 0x60, 0x06, 0xC4, 0x00, 0x60, 0x06, 0xC4, 0x00, 0x68, 0x06, 0xC4, 0x00, 0x70, 0x06, 0xC4, 0x00, 0x78, 0x06, 0xC4, 0x00, 0x80, 0x06, 0xC4, 0x00, 0xA8, 0x06, 0xC4, 0x00, 0xB0, 0x06, 0xC4, 0x00, 0xB8, 0x06, 0xC4, 0x00, 0xC0, 0x06, 0xC4, 0x00, 0xD8, 0x06, 0xC4, 0x00, 0xE8, 0x06, 0xC4, 0x00, 0xF0, 0x06, 0xC4, 0x00, 0xF8, 0x06, 0xC4, 0x00, 0x00, 0x07, 0xC4, 0x00, 0x08, 0x07, 0xC4, 0x00, 0x18, 0x07, 0xC4, 0x00, 0x20, 0x07, 0xC4, 0x00, 0x10, 0x05, 0xC5, 0x00, 0x18, 0x05, 0xC5, 0x00, 0x20, 0x05, 0xC5, 0x00, 0x30, 0x05, 0xC5, 0x00, 0x38, 0x05, 0xC5, 0x00, 0x68, 0x05, 0xC5, 0x00, 0x70, 0x05, 0xC5, 0x00, 0xA8, 0x05, 0xC5, 0x00, 0xB0, 0x05, 0xC5, 0x00, 0xB8, 0x05, 0xC5, 0x00, 0xC0, 0x05, 0xC5, 0x00, 0xC8, 0x05, 0xC5, 0x00, 0xD0, 0x05, 0xC5, 0x00, 0x30, 0x06, 0xC5, 0x00, 0x38, 0x06, 0xC5, 0x00, 0x40, 0x06, 0xC5, 0x00, 0x48, 0x06, 0xC5, 0x00, 0x50, 0x06, 0xC5, 0x00, 0x58, 0x06, 0xC5, 0x00, 0x70, 0x06, 0xC5, 0x00, 0x78, 0x06, 0xC5, 0x00, 0x80, 0x06, 0xC5, 0x00, 0x88, 0x06, 0xC5, 0x00, 0xA0, 0x06, 0xC5, 0x00, 0xA8, 0x06, 0xC5, 0x00, 0xB0, 0x06, 0xC5, 0x00, 0xB8, 0x06, 0xC5, 0x00, 0x10, 0x05, 0xC7, 0x00, 0x18, 0x05, 0xC7, 0x00, 0x40, 0x05, 0xC7, 0x00, 0x48, 0x05, 0xC7, 0x00, 0x50, 0x05, 0xC7, 0x00, 0x58, 0x05, 0xC7, 0x00, 0x60, 0x05, 0xC7, 0x00, 0x68, 0x05, 0xC7, 0x00, 0x80, 0x05, 0xC7, 0x00, 0x88, 0x05, 0xC7, 0x00, 0x90, 0x05, 0xC7, 0x00, 0x98, 0x05, 0xC7, 0x00, 0xF0, 0x05, 0xC7, 0x00, 0x18, 0x06, 0xC7, 0x00, 0x20, 0x06, 0xC7, 0x00, 0x28, 0x06, 0xC7, 0x00, 0x48, 0x06, 0xC7, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}
  !endif
