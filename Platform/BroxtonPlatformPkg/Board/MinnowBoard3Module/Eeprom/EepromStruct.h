#ifndef _EEPROM_LAYOUT_H_
#define _EEPROM_LAYOUT_H_

#pragma pack(1)

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


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   acpitbl[0];
}ACPI_TABLE;


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


typedef struct {
    CHAR8   maplabel[16];
    UINT32  length;
    UINT32  offset;
    UINT8   address;
    CHAR8   reserved[7];
}EEPROM_MAP_RECORD;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   gpiodata[0];
}GPIO_DATA_HEADER;


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


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   hdacodec[0];
}HDA_CODEC;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    UINT16  spdslot;
    CHAR8   reserved[14];
//  UINT8   spddata[0];
}MEMORY_DATA;


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


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    UINT16  hashtype;
    CHAR8   reserved[14];
//  UINT8   eepromsig[0];
}SIGNATURE_DATA;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   ucodedata[0];
}MICROCODE;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   videodata[0];
}VIDEO_DATA;


typedef struct {
    CHAR8   signature[8];
    UINT16  vermajor;
    UINT16  verminor;
    UINT32  length;
    CHAR8   reserved[16];
//  UINT8   logodata[0];
}LOGO_DATA;

#pragma pack()
#endif

