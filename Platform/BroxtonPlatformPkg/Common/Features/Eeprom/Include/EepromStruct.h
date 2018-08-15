/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_LAYOUT_H_
#define _EEPROM_LAYOUT_H_

#pragma pack(1)

typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
}GENERIC_HEADER;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    UINT32  structlength;
    UINT32  crc32;
    UINT32  crclength;
    UINT32  version;
    CHAR8   reserved[16];
}EEPROM_HEADER;

#define   EEPROM_HEADER_SIGNATURE   "$Eeprom$"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   acpitbl[0];
}ACPI_TABLE;

#define   EEPROM_ACPI_TABLE_SIGNATURE   "$AcpiTbl"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   manuname[16];
    CHAR8   brdname[16];
    CHAR8   brdserial[16];
    UINT32  boardid;
    UINT32  fabid;
    UINT32  ecid;
    UINT8   boardtype;
    CHAR8   reserved[19];
}BOARD_INFO_TABLE;

#define   EEPROM_BOARD_INFO_SIGNATURE   "$BrdInfo"
#define   EEPROM_BOARD_TYPE_REDBOX      0x00
#define   EEPROM_BOARD_TYPE_PLUGIN      0x01


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   partlabel[16];
    UINT32  blklength;
    UINT16  pagesize;
    UINT32  partsize;
    UINT8   busnumber;
    UINT8   master;
    UINT8   speed;
    CHAR8   reserved[3];
//  UINT8   mapdata[0];
}EEPROM_MAP;

#define   EEPROM_MAP_SIGNATURE   "$EeprMap"


typedef struct {
    CHAR8   maplabel[16];
    UINT32  length;
    UINT32  offset;
    UINT8   address;
    CHAR8   reserved[7];
}EEPROM_MAP_RECORD;

#define   EEPROM_SPEED_STANDARD   0x01
#define   EEPROM_SPEED_FAST       0x02
#define   EEPROM_SPEED_HIGH       0x03


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   gpiodata[0];
}GPIO_DATA_HEADER;

#define   EEPROM_GPIO_SIGNATURE   "$GpioDat"


typedef struct {
    CHAR8   gpiolabel[16];
    UINT32  length;
    UINT32  offset;
    UINT32  anddata;
    UINT32  ordata;
    UINT8   datasize;
    UINT8   datatype;
    CHAR8   reserved[14];
}GPIO_DATA_RECORD;

#define   EEPROM_GPIO_TYPE_IO           0x00
#define   EEPROM_GPIO_TYPE_MMIO         0x01
#define   EEPROM_GPIO_TYPE_PCI          0x02
#define   EEPROM_GPIO_TYPE_PCIE         0x03
#define   EEPROM_GPIO_TYPE_PAD_OFFSET   0x04


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   hdacodec[0];
}HDA_CODEC;

#define   EEPROM_HDA_CODEC_SIGNATURE   "$HdCodec"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    UINT16  spdslot;
    CHAR8   reserved[14];
//  UINT8   spddata[0];
}MEMORY_DATA;

#define   EEPROM_MEMORY_DATA_SIGNATURE   "$MemCnfg"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   nicid[8];
    CHAR8   macaddr[6];
    UINT16  nicnum;
    CHAR8   reserved[16];
//  UINT8   nicdata[0];
}NIC_INFO;

#define   EEPROM_NIC_INFO_SIGNATURE   "$MacInfo"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    UINT16  hashtype;
    CHAR8   reserved[14];
//  UINT8   eepromsig[0];
}SIGNATURE_DATA;

#define   EEPROM_SIGNATURE_SIGNATURE     "$PromSig"
#define   EEPROM_SIGNATURE_TYPE_NONE     0x0000
#define   EEPROM_SIGNATURE_TYPE_MD5      0x0001
#define   EEPROM_SIGNATURE_TYPE_SHA1     0x0002
#define   EEPROM_SIGNATURE_TYPE_SHA256   0x0003
#define   EEPROM_SIGNATURE_TYPE_SHA384   0x0004
#define   EEPROM_SIGNATURE_TYPE_SHA512   0x0005
#define   EEPROM_SIGNATURE_TYPE_SIGNED   0x8000


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   ucodedata[0];
}MICROCODE;

#define   EEPROM_MICROCODE_SIGNATURE   "$uCode$"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   videodata[0];
}VIDEO_DATA;

#define   EEPROM_VIDEO_DATA_SIGNATURE   "$Video$"


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   logodata[0];
}LOGO_DATA;

#define   EEPROM_LOGO_DATA_SIGNATURE   "$Logo$"

#pragma pack()
#endif
