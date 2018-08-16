/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.I2C2)
{
    Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () {"clock-frequency", 100000},
        }
    })
}

Scope (\_SB.PCI0.I2C2)
{
    Device (ACCL) {
        Name (_HID, "PRP0001")
        Name (_DDN, "ADXL345 Three-Axis Digital Accelerometer")
        Name (_CRS, ResourceTemplate () {
            I2cSerialBus (
                0x53,                   // Bus address
                ControllerInitiated,    // Don't care
                400000,                 // Max speed of the chip
                AddressingMode7Bit,     // 7-bit addressing
                "\\_SB.PCI0.I2C2",      // I2C host controller
                0                       // Must be 0
            )
                GpioInt (
                    Edge,
                    ActiveHigh,
                    ExclusiveAndWake,
                    PullNone,
                    0x0000,
                    "\\_SB.GPO2",
                    0x00,
                    ResourceConsumer, ,
                )
                {   // Pin list
                    28                  // ISH_GPIO_6_LS
                }
        })

        Name (_DSD, Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"compatible", Package () {"adi,adxl34x"}},
            }
        })
    }
}

