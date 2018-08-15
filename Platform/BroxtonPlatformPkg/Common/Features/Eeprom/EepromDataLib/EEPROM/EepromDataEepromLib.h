/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_DATA_EEPROM_LIB_
#define _EEPROM_DATA_EEPROM_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/I2cLib.h>
#include <Library/EepromPlatformLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>

#include <EepromStruct.h>

//
// Defines
//
#define   BLOCK_LIST_SIGNATURE          SIGNATURE_32 ('B', 'k', 'L', 't')
#define   EEPROM_MAP_INFO_SIGNATURE     SIGNATURE_32 ('E', 'm', 'M', 'p')
#define   EEPROM_PART_INFO_SIGNATURE    SIGNATURE_32 ('E', 'm', 'P', 't')

#define   DEFAULT_PAGE_SIZE             0x10
#define   EEPROM_HOB_HEADER_SIGNATURE   SIGNATURE_32 ('e', 'H', 'B', 'e')
#define   EEPROM_HOB_HEADER_VERSION     0x00000001
#define   EEPROM_HOB_MAX_DATA_SIZE      (30 * 1024)

//
// Structures
//
typedef struct {
  UINT32       Signature;
  LIST_ENTRY   Link;
  UINT8        Address;
  UINT16       Offset;
  UINT32       Size;
} BLOCK_LIST;

typedef struct {
  UINT32       Signature;
  LIST_ENTRY   Link;
  UINT8        Address;
  UINT32       Offset;
} EEPROM_MAP_INFO;

typedef struct {
   UINT32    Signature;
   UINT32    Version;
   UINT16    BlockNumber;
   UINT16    BlockSize;
   UINT32    Reserved;
} EEPROM_MEMORY_HOB_HEADER;

typedef struct {
  UINT32             Signature;
  LIST_ENTRY         Link;
  UINT8              Address;
  UINT32             BlockLength;
  UINT8              BusNumber;
  BOOLEAN            Master;
  UINT16             PageSize;
  UINT32             PartSize;
  UINT8              PartSpeed;
  LIST_ENTRY         MapHeadLink;
} EEPROM_PART_INFO;

//
// Functions
//
EFI_STATUS
EFIAPI
CopyEepromMap (VOID);

EFI_STATUS
EFIAPI
CreateBlockList (
  IN       UINT32        Offset,
  IN       UINT32       *Size,
  IN       VOID         *FunctionInfo,
  IN OUT   BLOCK_LIST   *BlockListHead
  );

VOID
EFIAPI
DestroyPageList (
  IN  BLOCK_LIST  *BlockListHead
  );

VOID
EFIAPI
DumpEepromMap (VOID);

EFI_STATUS
EFIAPI
FillEepromMap (
  IN OUT   VOID      *FunctionInfo
  );

UINT8
EFIAPI
GetMapAddress (
   IN      UINT32    ReadOffset,
   OUT     UINT16   *BlockOffset,
   IN      VOID     *FunctionInfo
   );

EFI_STATUS
EFIAPI
I2cReadPages (
  IN       UINT8     Bus,
  IN       UINT8     Address,
  IN       UINT16    Offset,
  IN       UINT32    Size,
  IN OUT   UINT8    *Buffer
  );

EFI_STATUS
EFIAPI
I2cWritePages (
  IN       UINT8      Bus,
  IN       UINT8      Address,
  IN       UINT16     Offset,
  IN       UINT32     Size,
  IN OUT   UINT8     *Buffer
  );

EFI_STATUS
EFIAPI
LoadEepromMap (VOID);

EFI_STATUS
EFIAPI
ReadEepromBlockList (
  IN OUT   UINT8        *Buffer,
  IN       VOID         *FunctionInfo,
  IN       BLOCK_LIST   *BlockListHead
  );

EFI_STATUS
EFIAPI
SaveEepromMap (VOID);

EFI_STATUS
EFIAPI
ScanI2cBusForImages (
  IN       UINT8     I2cBus,
  IN OUT   UINT8   **Addresses,
  IN OUT   UINT8    *AddressesSize
  );

EEPROM_MAP_INFO *
EFIAPI
SearchEepromMapList (
  IN  EEPROM_PART_INFO   *EepromPartInfo,
  IN  UINT8               Address,
  IN  UINT32              Offset
  );

EEPROM_PART_INFO *
EFIAPI
SearchEepromPartList (
  IN  UINT8    Bus,
  IN  UINT8    Address
  );

EFI_STATUS
EFIAPI
WriteEepromBlockList (
  IN   UINT8        *Buffer,
  IN   VOID         *FunctionInfo,
  IN   BLOCK_LIST   *BlockListHead
  );

//
// CR macros
//
#define BLOCK_LIST_FROM_THIS(a) \
  CR (a, \
      BLOCK_LIST, \
      Link, \
      BLOCK_LIST_SIGNATURE \
      )

#define EEPROM_MAP_INFO_FROM_THIS(a) \
  CR (a, \
      EEPROM_MAP_INFO, \
      Link, \
      EEPROM_MAP_INFO_SIGNATURE \
      )

#define EEPROM_PART_INFO_FROM_THIS(a) \
  CR (a, \
      EEPROM_PART_INFO, \
      Link, \
      EEPROM_PART_INFO_SIGNATURE \
      )

#endif // _EEPROM_DATA_EEPROM_LIB_
