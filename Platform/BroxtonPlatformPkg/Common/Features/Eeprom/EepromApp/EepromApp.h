/** @file

  This application will read/write/verify/dump the EERPOM data.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License which accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/

#ifndef _EEPROM_APP_H_
#define _EEPROM_APP_H_

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/EepromLib.h>
#include <Library/FileHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>

// Defines
#define   APP_VERSION          L"1.00.00"
#define   APP_COPYRIGHT        L"2017 - 2018"
#define   DIVIDING_LINE        L"+----------------------------------------------------+------------------+\n"

#define   MD5DigestSize        0x10
#define   SHA1DigestSize       0x14
#define   SHA256DigestSize     0x20
#define   SHA384DigestSize     0x30
#define   SHA512DigestSize     0x40

// Structures
typedef struct {
  BOOLEAN    HelpFlag;
  BOOLEAN    AddressFlag;
  UINT8      AddressValue;
  BOOLEAN    BusFlag;
  UINT8      BusValue;
  BOOLEAN    CheckFlag;
  UINT8      CheckLibraryIndex;
  UINT8     *CheckFile;
  CHAR16    *CheckFileName;
  UINT32     CheckFileSize;
  BOOLEAN    DumpHumanFlag;
  UINT8      DumpHumanLibraryIndex;
  UINT8     *DumpHumanFile;
  CHAR16    *DumpHumanFileName;
  UINT32     DumpHumanFileSize;
  BOOLEAN    DumpRawFlag;
  UINT8      DumpRawLibraryIndex;
  UINT8     *DumpRawFile;
  CHAR16    *DumpRawFileName;
  UINT32     DumpRawFileSize;
  BOOLEAN    EraseFlag;
  UINT8      EraseLibraryIndex;
  BOOLEAN    ReadFlag;
  UINT8      ReadLibraryIndex;
  UINT8     *ReadFile;
  CHAR16    *ReadFileName;
  UINT32     ReadFileSize;
  BOOLEAN    ScanFlag;
  BOOLEAN    VerboseFlag;
  BOOLEAN    VerifyFlag;
  UINT8      VerifyLibraryIndex;
  UINT8     *VerifyFile;
  CHAR16    *VerifyFileName;
  UINT32     VerifyFileSize;
  BOOLEAN    WriteFlag;
  UINT8      WriteLibraryIndex;
  UINT8     *WriteFile;
  CHAR16    *WriteFileName;
  UINT32     WriteFileSize;
} PROGRAM_INFO;

// Headers
SHELL_STATUS
BufferToMemory (
  IN  UINT8     *Buffer,
  IN  UINT32     BufferSize,
  OUT UINT8    **MemoryBuffer,
  OUT UINT32    *MemoryBufferSize
  );

SHELL_STATUS
CheckForInvalidOptions (
  IN PROGRAM_INFO    *ProgramInfo
  );

SHELL_STATUS
CheckOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

SHELL_STATUS
DumpHumanOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

VOID
DumpParagraph (
  IN   VOID   *Ptr,
  IN   UINTN   Count
  );

VOID
DumpProgramInfo (
  IN PROGRAM_INFO    *ProgramInfo
  );

SHELL_STATUS
DumpRawOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

SHELL_STATUS
EraseOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

UINTN
GetHexOrDecFromString (
  CHAR16 *Arg
  );

VOID
ParseParameters (
  IN UINTN            Argc,
  IN CHAR16         **Argv,
  IN PROGRAM_INFO    *ProgramInfo
  );

VOID
PrintChar (
  IN UINT8    Count,
  IN CHAR16   *Char
);

VOID
PrintHelp (VOID);

EFI_STATUS
ReadFileData (
  IN  CHAR16    *FileName,
  OUT UINT8    **Buffer,
  OUT UINT32    *BufferSize
  );

SHELL_STATUS
ReadOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

SHELL_STATUS
ScanOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

CHAR16 *
ToLower (
  IN OUT  CHAR16 *Str
  );

SHELL_STATUS
VerifyOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

EFI_STATUS
WriteFileData (
  IN  CHAR16   *FileName,
  IN  UINT8    *Buffer,
  IN  UINTN     BufferSize
  );

SHELL_STATUS
WriteOption (
  IN PROGRAM_INFO    *ProgramInfo
  );

#endif  // _EEPROM_APP_H_