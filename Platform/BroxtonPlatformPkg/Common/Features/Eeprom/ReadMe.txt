## @file
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
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

==========================================================================================
                                   EEPROM PCDs
==========================================================================================
[PcdsFixedAtBuild]
  ## I2C bus the master EEPROM is hanging on
  gPlatformModuleTokenSpaceGuid.PcdEepromBus|0x06|UINT8|0xEEEE2000
  ## 7-bit address of the master EEPROM
  gPlatformModuleTokenSpaceGuid.PcdEepromAddress|0x53|UINT8|0xEEEE2001
  ## Priority order of EEPROM data libraries
  ## 00 - Null; 01 - EEPROM; 02 - FV; 03 - Memory; FF - End of list
  ## Memory should be first
  gPlatformModuleTokenSpaceGuid.PcdEepromAutoPriority|{0x03, 0x01, 0x02, 0x00, 0xFF}|VOID*|0xEEEE2002
  ## Public key file GUID - 5D8A38A3-FBBD-4077-8105-11170C2AF54D
  gPlatformModuleTokenSpaceGuid.PcdEepromPublicKeyFile|{0xA3, 0x38, 0x8A, 0x5D, 0xBD, 0xFB, 0x77, 0x40, 0x81, 0x05, 0x11, 0x17, 0x0C, 0x2A, 0xF5, 0x4D}|VOID*|0xEEEE2003
  ## FV EEPROM Image file GUID - BFBD3DAC-01EB-4FEB-A9DE-BCC9D1BA5531
  gPlatformModuleTokenSpaceGuid.PcdEepromFvImageFile|{0xAC, 0x3D, 0xBD, 0xBF, 0xEB, 0x01, 0xEB, 0x4F, 0xA9, 0xDE, 0xBC, 0xC9, 0xD1, 0xBA, 0x55, 0x31}|VOID*|0xEEEE2004

[PcdsDynamic, PcdsDynamicEx]
  ## Used to store the EEPROM memory buffer pointer
  gPlatformModuleTokenSpaceGuid.PcdEepromMemoryPointer|0|UINT64|0xEEEE0000
  ## Used to store the EEPROM memory buffer size
  gPlatformModuleTokenSpaceGuid.PcdEepromMemorySize|0|UINT32|0xEEEE0001
  ## Used to store the EEPROM data library valid flags
  gPlatformModuleTokenSpaceGuid.PcdEepromLibraryValid|{0x00, 0x00, 0x00, 0x00}|VOID*|0xEEEE0002
  ## Flag to indicate that a HOB exists with EEPROM_MEMORY data
  gPlatformModuleTokenSpaceGuid.PcdEepromMemoryHobPresent|FALSE|BOOLEAN|0xEEEE0003
  ## Pointer to the Part head link
  gPlatformModuleTokenSpaceGuid.PcdEepromPartsHeadLink|0|UINT64|0xEEEE0004
  ## Pointer to the Parts table
  gPlatformModuleTokenSpaceGuid.PcdEepromParts|0|UINT64|0xEEEE0005
  ## Flag to tell if EEPROM Map is in memory
  gPlatformModuleTokenSpaceGuid.PcdEepromInMemoryFlag|0|BOOLEAN|0xEEEE0006
  ## Flag to tell if EEPROM Map is in HOB
  gPlatformModuleTokenSpaceGuid.PcdEepromMapHobValid|0|BOOLEAN|0xEEEE0007


==========================================================================================
                                   EEPROM GUIDs
==========================================================================================
[Guids]
  # GUID for EEPROM variables - {EE96CA33-5F59-4594-9869-07F79AA3C06F}
  gEepromVariableGuid = { 0xEE96CA33, 0x5F59, 0x4594, { 0x98, 0x69, 0x07, 0xF7, 0x9A, 0xA3, 0xC0, 0x6F }}


==========================================================================================
                                   Generating EEPROM binary
==========================================================================================
 Use the GenerateBinary.py script as a guide on how to create the EEPROM binary.

