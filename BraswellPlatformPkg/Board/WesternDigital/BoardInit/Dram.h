/** @file
  Board Init driver.

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


//        Input_Struct.tCL = 11;                                  //< actual CL /
//        Input_Struct.tRP_tRCD = 11;                             //< TRP and tRCD in dram clk/
//        Input_Struct.tWR = 12;// MIN =15ns                      //< in dram clk  /
//        Input_Struct.tWTR = 6;// MIN = greater of 4CK or 7.5ns  //< in dram clk  /
//        Input_Struct.tRRD = 6;// MIN = greater of 4CK or 7.5ns  //< in dram clk  /
//        Input_Struct.tRTP = 6;// MIN = greater of 4CK or 7.5ns; //< in dram clk  /
//        Input_Struct.tFAW = 32;
//
// TBD: Need update to CH version
//
UINT8  mWD_CR_SpdDataMemorySolderDown[] = {
  0x92,       // Byte 0
  0x12,       // Byte 1
  0x0b,       // Byte 2 (Key Byte / DRAM Device Type) DDR3 SDRAM
  0x03,       // Byte 3 (Key Byte / Module Type)   SO-DIMM
  0x05,       // Byte 4 (8Banks, 8Gb)
  0x23,       // Byte 5 (SDRAM Addressing) 00 100 011 = 0x23
  0x02,       // Byte 6
  0x02,       // Byte 7: Module Organization    Bit [5, 4, 3] : 000 = 1 Rank    Bit [2, 1, 0] :010 = SDRAM Device Width 16 bits
  0x03,       // Byte 8: Module Memory Bus Width Bit [2, 1, 0] : 011 = 64 bits
  0x52,       // Byte 9: Fine Timebase (FTB) Dividend / Divisor
  0x01,       // Byte 10: Medium Timebase (MTB) Dividend
  0x08,       // Byte 11: Medium Timebase (MTB) Divisor
  0x0a,       // Byte 12: SDRAM Minimum Cycle Time (tCKmin)
  0x00,       // Byte 13: Reserved
  0xfe,       // Byte 14: CAS Latencies Supported, Least Significant Byte (tCL)
  0x00,       // Byte 15: CAS Latencies Supported, Most Significant Byte
  0x69,       // Byte 16: Minimum CAS Latency Time (tAAmin)
  0x78,       // Byte 17: Minimum Write Recovery Time (tWRmin)
  0x69,       // Byte 18: Minimum RAS# to CAS# Delay Time (tRCDmin)
  0x3c,       // Byte 19: Minimum Row Active to Row Active Delay Time (tRRDmin)
  0x69,       // Byte 20: Minimum Row Precharge Delay Time (tRPmin)
  0x11,       // Byte 21: Upper Nibbles for tRAS and tRC
  0x18,       // Byte 22: Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte = 34
  0x81,       // Byte 23: Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
  0x00,       // Byte 24: Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
  0x05,       // Byte 25: Minimum Refresh Recovery Delay Time (tRFCmin), Most Significant Byte
  0x3c,       // Byte 26: Minimum Internal Write to Read Command Delay Time (tWTRmin)
  0x3c,       // Byte 27: Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
  0x01,       // Byte 28: Upper Nibble for tFAW
  0x40,       // Byte 29: Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
  0x83,       // Byte 30: SDRAM Optional Features
  0x01,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,   //Byte40
  0,0,0,0,0,0,0,0,
  0x00,
  0x00,
  0x00,
  0x00,
  0x0f,  // Byte 60:
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



