/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
    "EepromSsdt1.aml",
    "SSDT",
    1,
    "INTEL",
    "EprmSdt0",
    0x1000
    )
{
  External (ADOS, IntObj)
  External (OSSL, IntObj)
  External (VTKB, IntObj)
  External (\_SB.GPO0, DeviceObj)
  External (\_SB.GPO2, DeviceObj)
  External (\_SB.IPC1, DeviceObj)
  External (\_SB.IPC1.PMIC, DeviceObj)
  External (\_SB.IPC1.PMIC.AVBG, IntObj)
  External (\_SB.IPC1.PMIC.AVBL, IntObj)
  External (\_SB.IPC1.PMIC.GP3B, IntObj)
  External (\_SB.IPC1.PMIC.GP6B, IntObj)
  External (\_SB.IPC1.PMIC.VP3B, IntObj)
  External (\_SB.IPC1.PMIC.VP6B, IntObj)
  External (\_SB.PCI0, DeviceObj)
  External (\_SB.PCI0.I2C1, DeviceObj)
  External (\_SB.PCI0.I2C2, DeviceObj)
  External (\_SB.PCI0.I2C3, DeviceObj)
  External (\_SB.PCI0.SPI1, DeviceObj)
  External (\_SB.PCI0.SPI2, DeviceObj)

  // include ("Charger/Charger_BQ258900.asl")
  include ("I2C/Eeprom_M24M02.asl")

  // These are 6 examples of audio codecs
  // include ("Audio/Audio_codec_10EC5645.asl")
  // include ("Audio/Audio_codec_10EC5651.asl")
  // include ("Audio/Audio_codec_1AEC8731.asl")
  // include ("Audio/Audio_codec_INT34C1.asl")
  // include ("Audio/Audio_codec_INT34E0.asl")
  // include ("Audio/Audio_codec_DLGS7212.asl")

  // This is an example of how to add an I2C device and set the speed
  // include ("I2C/I2C_Bus2_TestDevice.asl")

  // This is an example of how to add a SPI device
  // include ("SPI/SPI1_CS0_TestDevice.asl")
  // include ("SPI/SPI2_CS0_TestDevice.asl")

  // This is an example of how to add a touch panel
  // include ("Touch/Touch_ATML1000.asl")
}

