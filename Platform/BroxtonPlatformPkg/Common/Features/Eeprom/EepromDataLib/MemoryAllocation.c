/** @file
  Common EEPROM memory allocation code.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataLib.h"

#include <Library/MemoryAllocationLib.h>

UINTN
EFIAPI
DisplayStackPointer (
  IN   CHAR8    *Function,
  IN   UINTN     LineNumber
  )
{
  UINT8   *Temp;

  Temp = AllocatePool (1);
  if (mEepromDataLibDebugFlag) {
    DEBUG ((DEBUG_INFO, "%a (#%4d) - INFO: FreeBottom = %08x\n", __FUNCTION__, __LINE__, Temp));
  }

  return (UINTN) Temp;
}

//
// Desc:        Copies the contents of an existing memory pool into a new memory pool of equal or greater size.
// Variables:   Size           Size of the pool to copy existing pool into
//              SourcePointer  Pointer to the source buffer to copy
// Return:      Pointer        Pointer to your copy of the pool
//
VOID*
EFIAPI
EepromAllocateCopyPool (
  IN  UINTN    Size,
  IN  VOID    *SourcePointer
  )
{
  return AllocateCopyPool (Size, SourcePointer);
}

//
// Desc:        Creates a new memory pool.
// Variables:   Size           Size of the pool requested
// Return:      Pointer        Pointer the new pool
//
VOID*
EFIAPI
EepromAllocatePool (
  IN  UINTN   Size
  )
{
  return AllocateZeroPool (Size);
}

//
// Desc:        Frees a memory pool.
// Variables:   Pointer        Pointer to the beginning of the pool to be freed
// Return:      Pointer        NULL
//
VOID*
EFIAPI
EepromFreePool (
  IN  VOID  *Pointer
  )
{
  if (Pointer != NULL) {
    FreePool (Pointer);
  }

  return NULL;
}

