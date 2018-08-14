/** @file
  Header file for the Platform ID code.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MINNOWBOARD_MODULE_PLATFORM_ID_H__
#define __MINNOWBOARD_MODULE_PLATFORM_ID_H__

#include <Guid/PlatformInfo.h>

//
// HWCONF defines. Low = off & high = on
//
#define HWCONF_COMx     BIT0
#define HWCONF_ECC      BIT1
//#define HWCONF_COMx     BIT2
//#define HWCONF_COMx     BIT3
#define HWCONF_USB3     BIT4
#define HWCONF_CAMERA   BIT5
#define HWCONF_MEMORY   6
#define HWCONF_MEMORY_MASK   (BIT6 | BIT7 | BIT8)

typedef struct {
  UINT32   CommAndOffset;
  UINT8    IOSTerm;
  UINT8    Term;
} PAD_ID_INFO;

typedef struct {
  UINT32   RawId;
  UINT8    TranslatedId;
  CHAR8    Description[32];
} TRANSLATE_ID_INFO;

EFI_STATUS
EFIAPI
Minnow3ModuleGetEmbeddedBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  );

UINT32
EFIAPI
Minnow3ModuleGetHwconfStraps (
  VOID
  );

#endif

