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
    Device (WLFC) {
        Name (_ADR, 0x1A)  // _ADR: Address
        Name (_HID, "1AEC8731")  // _HID: Hardware ID
        Name (_CID, "1AEC8731")  // _CID: Compatible ID
        Name (_DDN, "Wolfson WM8731 Audio Codec")  // _DDN: DOS Device Name
        Name (_UID, One)  // _UID: Unique ID

        Name (SBUF, ResourceTemplate () {
          I2CSerialBus (
            0x1A,               // SlaveAddress: bus address
            ControllerInitiated,// SlaveMode: default to ControllerInitiated
            400000,             // ConnectionSpeed: in Hz
            AddressingMode7Bit, // Addressing Mode: default to 7 bit
            "\\_SB.PCI0.I2C1",  // ResourceSource: I2C bus controller name
            ,                   // Descriptor Name: creates name for offset of resource descriptor
          )  // VendorData
          GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullNone, 0,"\\_SB.GPO0") {20} //  AUD_INT
        })

        Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
          Return (SBUF)
        }

        Method (_STA, 0, NotSerialized) { // _STA: Status
          Return (0x0F)
        }

        Method (_DIS, 0, NotSerialized) { // _DIS: Disable Device
        }
    }
}

