/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.I2C1) {
  Device (ANCH) {
    Name (_ADR, 0x6B)
    Name (_HID, "BQ258900")           // _HID: Hardware ID
    Name (_CID, "BQ258900")           // _CID: Compatible ID
    Name (_DDN, "Charger Controller") // _DDN: DOS Device Name
    Name (RBUF, ResourceTemplate () {
      I2CSerialBus(
        0x6B,               // SlaveAddress: bus address
        ,                   // SlaveMode: default to ControllerInitiated
        400000,             // ConnectionSpeed: in Hz
        ,                   // Addressing Mode: default to 7 bit
        "\\_SB.PCI0.I2C1",  // ResourceSource: I2C bus controller name
        ,                   // Descriptor Name: creates name for offset of resource descriptor
      )  // VendorData
      GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPO0") {19} //  CHARGER_INT
      GpioIo (Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO0", 0x00, ResourceConsumer, ,) {15}
    })

    Method (_CRS, 0, NotSerialized) { // _CRS: Current Resource Settings
      Return (RBUF) /* \_SB_.PCI0.I2C1.ANCH.RBUF */
    }

    Method (_STA, 0, NotSerialized) { // _STA: Status
      If (LEqual(OSSL, One))          // Linux
        {Return (0x0F)}
      Else
        {Return (Zero)}
    }
  }
}

