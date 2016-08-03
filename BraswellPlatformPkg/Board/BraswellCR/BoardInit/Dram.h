/** @file
  DRAM setting for CherryView.

  This file includes package header files, library classes.

  Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

UINT8  mBSW_CR_SpdDataMemorySolderDown[] = {
  0x92,       //Byte 0
  0x12,
  0x0b,
  0x03,
  0x04,      //Byte 4
  0x11,
  0x02,
  0x02,
  0x03,
  0x52,
  0x01,
  0x08,
  0x0a,
  0x00,
  0xfe,
  0x00,
  0x69,
  0x78,
  0x69,
  0x3c,
  0x69,       //Byte20
  0x11,
  0x18,
  0x81,
  0x00,
  0x05,
  0x3c,
  0x3c,
  0x01,
  0x40,
  0x83,
  0x01,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,   //Byte40
  0,0,0,0,0,0,0,0,
  0x00,
  0x00,
  0x00,
  0x00,
  0x0f,
  0x11,
  0x22,
  0x00,
  0,0,0,0,0,0,0,0,    //Byte64
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0x00,               //Byte112
  0x00,
  0x00,
  0x00,
  0x00,
  0x80,
  0xad,
  0x01,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00           //Byte 215
};
