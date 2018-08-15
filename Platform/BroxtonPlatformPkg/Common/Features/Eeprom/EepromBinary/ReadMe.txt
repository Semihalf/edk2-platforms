## @file
#  Python Tools.
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution. The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

==========================================================================================
                                   Eeprom Current Status
==========================================================================================

1. PYTHON SCRIPTS:
     CreateKeyPair.py   - creates the private/public key-pairs.
     ConvertPemToBin.py - converts a private key PEM file into a public key binary file.
     eeprom.py          - creates a binary file using two configuration files (definition
                          file and Project file) Examples are provided. This binary file
                          describes the EEPROM in which it will reside. The description
                          include the eeproms bus#, device address, page size, capacity
                          among others.
     HashBinary.py      - fills out the $PromSig structure, if it exists, and adds the
                          hash and signature if requested.

2. UEFI TOOL: EepromApp.efi is then used to flash the created binary from step 1. into
   EEPROM from the UEFI shell.

3. For MB3, a default eeprom binary file has been included in the UEFI BIOS.

4. You can get the ASL compiler from https://www.acpica.org/downloads It should go in the
   IASL folder.

5. You can get OpenSSL from https://wiki.openssl.org/index.php/Binaries It should go in the
   openssl folder.


==========================================================================================
                                   CreateKeyPair python Tool
==========================================================================================
CreateKeyPair.py:
- This tool creates a private/public key-pair, including the binary form of the public key.
  private-key.pem, public-key.pem, & public-key.bin will be created if no FilePrefix is
  specified. Otherwise, FilePrefix-private.pem, FilePrefix-public.pem, &
  FilePrefix-public.bin will be created.

usage: CreateKeyPair.py OpenSsl DestinationPath [FilePrefix]

Example:
  CreateKeyPair.py ..\openssl\openssl.exe RSA-keys


==========================================================================================
                                   ConvertPemToBin python Tool
==========================================================================================
ConvertPemToBin.py:
- This tool converts a private key PEM file into a public key binary file.

usage: ConvertPemToBin.py OpenSsl PrivateKeyPemFileName PublicKeyBinFileName

Example:
  ConvertPemToBin.py ..\openssl\openssl.exe RSA-keys\MB3-private.pem RSA-keys\MB3-public.bin


==========================================================================================
                                   Eeprom python Tool
==========================================================================================
Eeprom.py:
- This tool converts information in a definition and project configuration files into
  Self Describing Board binary file format that can be written into an EERPOM.

- Generates a header file containing all Self Describing Board structures as defined in
  the definition file passed as a parameter.

usage: eeprom.py [-h] [-d DEFINITIONS] [-p PROJECT] [-b BINARY]
                 [--header HEADERFILE] [--verbose] [--version]

optional arguments:
  -h, --help            show this help message and exit
  -d DEFINITIONS, --deff DEFINITIONS
                        Provides a path to the definition file for the image.
  -p PROJECT, --project PROJECT
                        Provides a path to the project configuration file.
  -b BINARY, --binary BINARY
                        Output binary file name.
  --header HEADERFILE   Provides a file name for a C header file to be
                        generated.
  --verbose             Enables verbose output for debugging.
  --version             Displays the version information and exits the
                        program.

Example:
  eeprom.py -d MB3-Defs.cfg -p MB3-Prj.cfg -b MB3-Eeprom.bin --header EepromStruct.h


==========================================================================================
                                   HashBinary python Tool
==========================================================================================
HashBinary.py:
- This tool adds the hash to the EEPROM $PromSig structure if it exists.

usage: HashBinary.py OpenSsl BinaryFileName [PrivateKeyPemFileName]

Example:
  HashBinary.py ..\openssl\openssl.exe MB3-Eeprom.bin RSA-keys\MB3-private.pem


===========================================================================================
                                   UEFI EEPROM tool
===========================================================================================
EepromApp.efi

  EepromApp.efi is a tool to read/write/dump the EERPOM data.

  Usage:
    EepromApp.efi command [library] [filename] [switches]

  Commands:
    -c  # filename   Check security of the image.
    -d  # filename   Dumps the image in raw format.
    -dh # filename   Dumps the image in human readable format.
    -e  #            Erase that image.
    -h               Dumps this help info.
    -r  # filename   Reads the image from the library index into the file.
                     - This reads the entire EEPROM part, not just the image.
    -s               Scans all of the buses looking for images.
                     - Can be substituted for specifying -b & -a if there is already an
                       EEPROM image present.
    -v  # filename   Verifies the image in library index with the file.
    -w  # filename   Writes the image from the file to the library index.

  Library:
    0 - Null library
    1 - EEPROM library
    2 - FV library
    3 - Memory library

  Switches:
    -a  #            Sets the I2C slave address (7-bit). (Only valid for EEPROM library.)
    -b  #            Sets the I2C bus number. (Only valid for EEPROM library.)
    --verbose        Turns on verbosity.

  Notes:
    - Numbers can be represented as hex (0x##) or decimal (##).
    - If specifying the EEPROM library, then either -b & -a or -s must be present on the
      command line as well.
    - If dumping a file, then specify the NULL library.

  Examples:
    EepromApp.efi -b  6 -a 0x53 -d 1            - Dump the EEPROM image in raw format from
                                                  I2C #6, 0x53.
    EepromApp.efi -r  3 Test.bin                - Write the memory image to Test.bin.
    EepromApp.efi -s -w  1 Test.bin             - Write Test.bin to the EERPOM last found
                                                  during the scan.
    EepromApp.efi -b  4 -a 0x5F -w 1 Test.bin   - Write Test.bin to the EERPOM on I2C #4,
                                                  0x5F.
    EepromApp.efi -dh 0 Test.bin                - Dump Test.bin in human readable form.
    EepromApp.efi -s -c 1                       - Scan the EEPROM buses for images and
                                                  check the security of the last one found.


===========================================================================================
                                   UEFI EEPROM PCDs
===========================================================================================
  [PcdsDynamic,PcdsDynamicEx]
    ## Used to store the EEPROM memory buffer pointer
    gPlatformModuleTokenSpaceGuid.PcdEepromMemoryPointer|0|UINT64|0xEEEE0000
    ## Used to store the EEPROM memory buffer size
    gPlatformModuleTokenSpaceGuid.PcdEepromMemorySize|0|UINT32|0xEEEE0001
    ## Used to store the EEPROM data library valid flags
    gPlatformModuleTokenSpaceGuid.PcdEepromLibraryValid|{0x00, 0x00, 0x00, 0x00}|VOID*|0xEEEE0002
    ## Flag to indicate that a HOB exists with EEPROM_MEMORY data
    gPlatformModuleTokenSpaceGuid.PcdEepromMemoryHobPresent|FALSE|BOOLEAN|0xEEEE0003
    ## Pointer to the Part head link
    gPlatformModuleTokenSpaceGuid.PcdEepromPartsHeadLink|0|UINT64|0xEEEE0004
    ## Pointer to the Parts table
    gPlatformModuleTokenSpaceGuid.PcdEepromParts|0|UINT64|0xEEEE0005
    ## Flag to tell if EEPROM Map is in memory
    gPlatformModuleTokenSpaceGuid.PcdEepromInMemoryFlag|0|BOOLEAN|0xEEEE0006
    ## Flag to tell if EEPROM Map is in HOB
    gPlatformModuleTokenSpaceGuid.PcdEepromMapHobValid|0|BOOLEAN|0xEEEE0007

  [PcdsFixedAtBuild]
    ## I2C bus the master EEPROM is hanging on
    gPlatformModuleTokenSpaceGuid.PcdEepromBus|0x06|UINT8|0xEEEE2000
    ## 7-bit address of the master EEPROM
    gPlatformModuleTokenSpaceGuid.PcdEepromAddress|0x50|UINT8|0xEEEE2001
    ## Priority order of EEPROM data libraries
    ## 00 - Null; 01 - EEPROM; 02 - FV; 03 - Memory; FF - End of list
    ## Memory should be first
    gPlatformModuleTokenSpaceGuid.PcdEepromAutoPriority|{0x03, 0x01, 0x02, 0x00, 0xFF}|VOID*|0xEEEE2002
    ## Public key file GUID - 5D8A38A3-FBBD-4077-8105-11170C2AF54D
    gPlatformModuleTokenSpaceGuid.PcdEepromPublicKeyFile|{0xA3, 0x38, 0x8A, 0x5D, 0xBD, 0xFB, 0x77, 0x40, 0x81, 0x05, 0x11, 0x17, 0x0C, 0x2A, 0xF5, 0x4D}|VOID*|0xEEEE2003
    ## FV EEPROM Image file GUID - BFBD3DAC-01EB-4FEB-A9DE-BCC9D1BA5531
    gPlatformModuleTokenSpaceGuid.PcdEepromFvImageFile|{0xAC, 0x3D, 0xBD, 0xBF, 0xEB, 0x01, 0xEB, 0x4F, 0xA9, 0xDE, 0xBC, 0xC9, 0xD1, 0xBA, 0x55, 0x31}|VOID*|0xEEEE2004
    ## GPIO PAD whitelist                           |    END OF ARRAY    |
    gPlatformModuleTokenSpaceGuid.PcdGpioWhiteList|{0xFF, 0xFF, 0xFF, 0xFF}|VOID*|0xEEEE2005


===========================================================================================
                                   UEFI EEPROM Libraries
===========================================================================================
  EepromLib.h
    UINTN
    EFIAPI
    DisplayStackPointer (
      IN   CHAR8    *Function,
      IN   UINTN     LineNumber
      );

    EFI_STATUS
    EFIAPI
    EraseEeprom (
      IN       UINT8     LibraryIndex
      );

    EFI_STATUS
    EFIAPI
    GetEepromStructure (
      IN       UINT8      LibraryIndex,
      IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
      IN OUT   UINT8    **Buffer,
      IN OUT   UINT32    *Size
      );

    UINT32
    EFIAPI
    GetImageSize (
      IN       UINT8      LibraryIndex
      );

    EFI_STATUS
    EFIAPI
    GetNextEepromStructure (
      IN       UINT8      LibraryIndex,
      IN OUT   UINT32    *Index,
      IN OUT   UINT8    **Buffer,
      IN OUT   UINT32    *Size
      );

    UINT8
    EFIAPI
    GetValidEepromLibrary (
      IN       BOOLEAN   CopyToMemory,
      IN       BOOLEAN   MemoryInitialized
      );

    BOOLEAN
    EFIAPI
    InPeiPhase (VOID);

    EFI_STATUS
    EFIAPI
    ValidateEeprom (
      IN       UINT8     LibraryIndex
      );

    //
    // Desc:        Registers the raw data libraries
    // Variables:   None
    // Return:      EFI_SUCCESS
    //
    EFI_STATUS
    EFIAPI
    EepromInitConstructor (VOID);

  EepromDataLib.h
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
      );

    //
    // Desc:        Creates a new memory pool.
    // Variables:   Size           Size of the pool requested
    // Return:      Pointer        Pointer the new pool
    //
    VOID*
    EFIAPI
    EepromAllocatePool (
      IN  UINTN   Size
      );

    EFI_STATUS
    EFIAPI
    EepromDataLibNemToMemory (VOID);

    //
    // Desc:        Frees a memory pool.
    // Variables:   Pointer        Pointer to the beginning of the pool to be freed
    // Return:      Pointer        NULL
    //
    VOID*
    EFIAPI
    EepromFreePool (
      IN  VOID  *Pointer
      );

    //
    // Desc:        Reads from the EEPROM and copies to the passed in buffer.
    // Variables:   LibraryIndex   Determines which raw data library to use
    //              Offset         Start copying from the offset
    //              Size           Size of the buffer and the number of bytes to copy
    //                             - If set to 0, then return size of EEPROM binary
    //              Buffer         Storage buffer for the copied data from the EEPROM
    //              FunctionInfo   Pointer to function specific data
    // Return:      EFI_SUCCESS             Data copied successfully
    //              EFI_UNSUPPORTED         This function is not supported
    //              EFI_INVALID_PARAMETER   One of the parameters is invalid
    //              EFI_NOT_READY           Called before all necessary library available
    //              EFI_DEVICE_ERROR        Communication error with device
    //
    EFI_STATUS
    EFIAPI
    ReadEeprom (
      IN       UINT8     LibraryIndex,
      IN       UINT32    Offset,
      IN OUT   UINT32   *Size,
      IN OUT   UINT8    *Buffer,
      IN OUT   VOID     *FunctionInfo
      );

    //
    // Desc:        Writes to the EEPROM and copies to the passed in buffer.
    // Variables:   LibraryIndex   Determines which raw data library to use
    //              Offset         Start copying from the offset
    //              Size           Size of the buffer and the number of bytes to copy
    //                             - If set to 0, then return size of EEPROM binary
    //              Buffer         Data to be copied to the EEPROM
    //              FunctionInfo   Pointer to function specific data
    // Return:      EFI_SUCCESS             Data copied successfully
    //              EFI_UNSUPPORTED         This function is not supported
    //              EFI_INVALID_PARAMETER   One of the parameters is invalid
    //              EFI_NOT_READY           Called before all necessary library available
    //              EFI_DEVICE_ERROR        Communication error with device
    //
    EFI_STATUS
    EFIAPI
    WriteEeprom (
      IN       UINT8     LibraryIndex,
      IN       UINT32    Offset,
      IN OUT   UINT32   *Size,
      IN OUT   UINT8    *Buffer,
      IN OUT   VOID     *FunctionInfo
      );


===========================================================================================
                                   UEFI EEPROM Samples
===========================================================================================
Adding a VBT to the EEPROM:
1. In MB3-Prj.cfg
   A. In the [Structures] section, uncomment "Struct_6 = VideoInfo"
   B. Uncomment the [VideoInfo] section
   C. Point the from_file() to the desired VBT

Adding a Logo to the EEPROM:
1. In MB3-Prj.cfg
   A. In the [Structures] section, uncomment "Struct_7 = LogoInfo"
   B. Uncomment the [LogoInfo] section
   C. Point the from_file() to the desired Logo BMP file
   D. Only BMP files are allowed.

Adding an I2C device to the EEPROM:
1. In EepromSsdt0.asl
   A. Uncomment the desired I2C bus example
   B. ASL I2C busses are 1 based while the LPSS busses are 0 based
2. In I2C_Bus2_TestDevice.asl
   A. Make sure you have the I2C address correct for your design

Adding a SPI device to the EEPROM:
1. In EepromSsdt0.asl
   A. Uncomment the desired SPI bus example
   B. ASL SPI busses are 1 based while the LPSS busses are 0 based
2. In SPI?_CS0_TestDevice.asl
   A. Make sure you have the CS selected correctly for your design


===========================================================================================
                   UEFI EEPROM STRUCTURE DEFINITION Version: 1.00
===========================================================================================
Generic header structure
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$       "    | Marks the beginning of the structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x00000010    | Length in bytes of this structure

EEPROM header structure:
  - This structure MUST be first
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$Eeprom$"    | Marks the beginning of the EEPROM Header structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x00000020    | Length in bytes of this structure
 0x10  | 0x04   | StructLen | 0x00000000    | Length of all of the structures including this one.
 0x14  | 0x04   | Crc32     | 0x00000000    | CRC32 of all of the structures
 0x18  | 0x04   | CrcLength | 0x00000000    | Length in bytes of what is in the CRC32
 0x1C  | 0x04   | Version   | 0x00000000    | Version of the binary
 0x20  | 0x10   | Reserved  | 0x00          | Reserved for future info

 EEPROM Map structure
  - This structure MUST be right after the EEPROM Board Info structure
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$EeprMap"    | Marks the beginning of the EEPROM Map structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | PartLabel | ""            | Part Label
 0x20  | 0x04   | BlkLength | 0x0000????    | block length in bytes of this EEPROM device
 0x24  | 0x02   | PageSize  | 0x0000        | size of page in bytes of this EEPROM device
 0x26  | 0x04   | PartSize  | 0x0000        | Part size in bytes of this EEPROM device
 0x2A  | 0x01   | BusNumber | 0x00          | I2C Device Bus
 0x2B  | 0x01   | Master    | 0x00          | Set to 1 if EEPROM is master, 0 otherwise
 0x2C  | 0x01   | Speed     | 0x00          | The speed the EEPROM part should run at
       |        |           |               |  1 - 100KHz (Standard speed)
       |        |           |               |  2 - 400KHz (Fast speed)
       |        |           |               |  3 - 3.4MHz (High speed)
 0x2D  | 0x03   | Reserved  | 0x00          | Reserved for future info
 0x30  | ????   | MapData   | 0x00          | EEPROM Map records

    EEPROM Map record
   Offset | Length |   Label   | Default value | Description
   =======+========+===========+===============+==============================================
    0x00  | 0x10   | MapLabel  | ""            | String label for this Map record (If the
          |        |           |               |   string isn't 16 characters, then terminate
          |        |           |               |   with a "$").
    0x10  | 0x04   | Length    | 0x00000020    | Length of this block
    0x14  | 0x04   | Offset    | 0x00000000    | Offset into EEPROM address space
    0x18  | 0x01   | Address   | 0xA0          | I2C Device address, 7-bit
    0x19  | 0x07   | Reserved  | 0x0000        | Reserved for future info

ACPI Table structure:
  - The current implementation only publishes SSDTs as(EprmSdt#)
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$AcpiTbl"    | Marks the beginning of the ACPI Table structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | AcpiTbl   | 0x00          | ACPI tables

Board info structure:
  - This structure MUST be right after the EEPROM Header structure
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$BrdInfo"    | Marks the beginning of the Board Info structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x00000060    | Length in bytes of this structure
 0x10  | 0x10   | ManuName  | "           " | Board manufacturer name (If the string isn't
       |        |           |               |   16 characters, then terminate with a "$").
 0x20  | 0x10   | BrdName   | "           " | Board name (If the string isn't 16 characters,
       |        |           |               |   then terminate with a "$").
 0x30  | 0x10   | BrdSerial | "           " | Board Serial number (If the string isn't
       |        |           |               |   16 characters, then terminate with a "$").
 0x40  | 0x04   | BoardId   | 0x00000000    | Board ID
 0x44  | 0x04   | FabId     | 0x00000000    | Fab ID
 0x48  | 0x04   | EcId      | 0x00000000    | EC ID
 0x4C  | 0x01   | BoardType | 0x00          | Type of board
       |        |           |               | - 0x00 = Main board, aka "RedBox"
       |        |           |               | - 0x01 = Plug-in board
 0x4D  | 0x13   | Reserved  | 0x00          | Reserved for future info

GPIO data structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$GpioDat"    | Marks the beginning of the GPIO data structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | GpioData  | 0x00          | GPIO data records

   GPIO data record
   Offset | Length |   Label   | Default value | Description
   =======+========+===========+===============+==============================================
    0x00  | 0x10   | GpioLabel | ""            | String label for this GPIO record (If the
          |        |           |               |   string isn't 16 characters, then terminate
          |        |           |               |   with a "$").
    0x10  | 0x04   | Length    | 0x00000030    | Length of this block
    0x14  | 0x04   | Offset    | 0x00000000    | Offset into GPIO address space, check
          |        |           |               | GPIO CONFIGURATION section for details
    0x18  | 0x04   | AndData   | 0x00000000    | Read the value from the Offset and do a logical
          |        |           |               |   AND using AndData
    0x1C  | 0x04   | OrData    | 0x00000000    | Do a logical OR against the value from AndData
          |        |           |               |   and write it back to Offset
    0x20  | 0x01   | DataSize  | 0x00          | Size of data in bytes
          |        |           |               | - 0x01 -  8-bits
          |        |           |               | - 0x02 - 16-bits
          |        |           |               | - 0x04 - 32-bits
    0x21  | 0x01   | DataType  | 0x00          | Type of access method for the data
          |        |           |               | - 0x00 = IO
          |        |           |               | - 0x01 = MMIO
          |        |           |               | - 0x02 = PCI
          |        |           |               | - 0x03 = PCIe
          |        |           |               | - 0x04 = PAD Offset
    0x22  | 0x0E   | Reserved  | 0x0000        | Reserved for future info

HDA Codec structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$HdCodec"    | Marks the beginning of the HDA Codec structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | HdaCodec  | 0x00          | HDA Codec verb table

Logo structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$Logo$"      | Marks the beginning of the Logo structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | LogoData  | 0x00          | Logo data

Memory SPD structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$MemCnfg"    | Marks the beginning of the memory SPD structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x02   | SpdSlot   | 0x0000        | Which slot this SPD data belongs to in bit flag format
       |        |           |               | - 0x0001 - This is for Slot 1 only
       |        |           |               | - 0x0005 - This is for Slot 1 & 3 only
 0x12  | 0x0E   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | SpdData   | 0x00          | SPD data for memory

NIC info structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$MacInfo"    | Marks the beginning of the NIC Info structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x08   | NicId     | ""            | String to describe this NIC
 0x18  | 0x06   | MacAddr   | 0x00000000000 | MAC Address
 0x1E  | 0x02   | NicNum    | 0x0000        | NIC number that this MAC belongs to
 0x20  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x30  | ????   | NicData   | 0x00          | This space is reserved for anything else that
       |        |           |               |   the NIC might need, but is not necessarily
       |        |           |               |   required to hold anything and may be 0 length

Signature structure
  - This structure is only to be appended to a final EEPROM image by a signing tool.
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$PromSig"    | Marks the beginning of the Signature Header structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x02   | HashType  | 0x0000        | Which hash type was used
       |        |           |               | - 0x0000 = None    - 0x00 byte hash
       |        |           |               | - 0x0001 = MD5     - 0x10 byte hash [signable]
       |        |           |               | - 0x0002 = SHA-1   - 0x14 byte hash [signable]
       |        |           |               | - 0x0003 = SHA-256 - 0x20 byte hash [signable]
       |        |           |               | - 0x0004 = SHA-384 - 0x30 byte hash
       |        |           |               | - 0x0005 = SHA-512 - 0x40 byte hash
       |        |           |               | - 0x8000 flag to indicate this is a signed hash
       |        |           |               |   - It is assumed that the BIOS knows how to verify
       |        |           |               |     the signed hash.
 0x12  | 0x0E   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | EepromSig | 0x00          | Hash, possibly signed, of all data structures.
       |        |           |               |   Includes this structure with the EepromSig zeroed
       |        |           |               |   out and CRC32 already calculated.

uCode data structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$uCode$"     | Marks the beginning of the uCode data structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | UcodeData | 0x00          | uCode data

Video structure:
Offset | Length |   Label   | Default value | Description
=======+========+===========+===============+==============================================
 0x00  | 0x08   | Signature | "$Video$"     | Marks the beginning of the Video structure
 0x08  | 0x02   | VerMajor  | 0x0001        | Version, major
 0x0A  | 0x02   | VerMinor  | 0x0000        | Version, minor
 0x0C  | 0x04   | Length    | 0x0000????    | Length in bytes of this structure
 0x10  | 0x10   | Reserved  | 0x00          | Reserved for future info
 0x20  | ????   | VideoData | 0x00          | Video data


===========================================================================================
                            GPIO CONFIGURATION
===========================================================================================
GPIO PAD programming info:
- pad_cfg_dw0 (PAD OFFSET + 0x00) "PAD_CONF0"
  - [31:30] - padrstcfg      - This register controls which reset is used to reset the specified Pad Register fields.
                               It is the SoC responsibilities to decide the number of configurable reset required
                               with an example below:
                               00 = Powergood (i.e. sticky reset).
                               01 = Deep GPIO Reset (i.e. host deep reset: host_deep_rst_b)
                               10 = GPIO Reset (i.e. host reset: host_rst_b)
                               11 = Reserved; tied to inactive (i.e. '1' for active low reset)
  - [29:29] - rxpadstsel     - Determine from which node the RX pad state for native function should be taken from.
                               This field only affects the pad state value being fanned out to native function(s)
                               and is not meaningful if the pad is in GPIO mode (i.e. Pad Mode = 0).
                               0 = Raw RX pad state directly from CFIO RX buffer.
                               1 = Internal RX pad state (subject to RXINV , hardware debouncer (if any) and
                                   PreGfRXSel settings)
  - [28:28] - rxrawl         - This bit determines if the selected pad state is being overridden to '1'. This field
                               only makes sense when the RX buffer is configured as an input in either GPIO Mode or
                               native function mode.
                               0 = No Override.
                               1 = RX drive 1 internally.
  - [27:27] - reserved       - Reserved
  - [26:25] - rxevcfg        - Determine if the internal RX pad state (synchronized, filtered vs non-filtered
                               version as determined by PreGfRXSel, and is further subject to RXInv) should be
                               passed on to the next logic stage as is, as a pulse, or level signal. This field
                               does not affect the received pad state (to GPIORXState or native functions) but how
                               the interrupt or wake triggering events should be delivered to the GPIO Community
                               Controller.
                               0h = Level
                               1h = Edge
                               2h = Drive '0'
                               3h = Reserved (implement as setting 0h)
  - [24:24] - pregfrxsel     - Determine if the synchronized version of the raw RX pad state should be subjected
                               to glitch filter or not.
                               0 = Select synchronized, non filtered RX pad state.
                               1 = Select synchronized, filtered RX pad state.
  - [23:23] - rxinv          - This bit determines if the selected pad state should go through the polarity
                               inversion stage.
                               0 = No inversion
                               1 = Inversion
  - [22:21] - rxtxencfg      - This controls the RX and TX buffer enables for the function selected by Pad Mode,
                               but is not applicable when Pad Mode is 0 (i.e. GPIO mode).
                               0 = Function defined in Pad Mode controls TX and RX Enables.
                               1 = Function controls TX Enable and RX Disabled with RX drive 0 internally.
                               2 = Function controls TX Enable and RX Disabled with RX drive 1 internally.
                               3 = Function controls TX Enabled and RX is always enabled.
  - [20:20] - gpiroutioxapic - Determine if the pad can be routed to cause peripheral IRQ when configured in GPIO
                               input mode.
                               0 = Routing does not cause peripheral IRQ.
                               1 = Routing can cause peripheral IRQ.
  - [19:19] - gpiroutsci     - Determine if the pad can be routed to cause SCI when configured in GPIO input mode.
                               0 = Routing does not cause SCI.
                               1 = Routing can cause SCI.
  - [18:18] - gpiroutsmi     - Determine if the pad can be routed to cause SMI when configured in GPIO input mode.
                               0 = Routing does not cause SMI.
                               1 = Routing can cause SMI.
  - [17:17] - gpiroutnmi     - Determine if the pad can be routed to cause NMI when configured in GPIO input mode.
                               0 = Routing does not cause NMI.
                               1 = Routing can cause NMI.
  - [16:13] - reserved       - Reserved
  - [12:10] - pmode          - This field determines whether the Pad is controlled by GPIO controller logic or one
                               of the native functions muxed onto the Pad.
                               0h = GPIO control the Pad.
                               1h = Function 1, if applicable, control the Pad.
                               ...
                               15 = Function 15 control the Pad.
  - [09:09] - gpiorxdis      - 0 = Enable the input buffer (active low enable) of the pad.
                               1 = Disable the input buffer of the pad.
  - [08:08] - gpiotxdis      - 0 = Enable the output buffer (active low enable) of the pad.
                               1 = Disable the output buffer of the pad.
  - [07:02] - reserved       - Reserved
  - [01:01] - gpiorxstate    - This is the current internal RX pad state after Glitch Filter logic stage and
                               hardware debouncer (if any) and is not affected by PMode and RXINV settings.
  - [00:00] - gpiotxstate    - 0 = Drive a level '0' to the TX output pad.
                               1 = Drive a level '1' to the TX output pad.

- pad_cdf_dw1 (PAD OFFSET + 0x04) "PAD_CONF1"
  - [31:18] - cfiopadcfg     - See CFIO HIP interface for details. Default to 0x0000
  - [17:14] - iosstate       - The I/O Standby State defines which state the pad should be parked in when the
                               I/O is in a standby state.
                                0 = Tx enabled driving last value driven, Rx enabled.
                                1 = Tx enabled driving 0, Rx disabled and Rx driving 0 back to its controller
                                    internally.
                                2 = Tx enabled driving 0, Rx disabled and Rx driving 1 back to its controller
                                    internally.
                                3 = Tx enabled driving 1, Rx disabled and Rx driving 0 back to its controller
                                    internally.
                                4 = Tx enabled driving 1, Rx disabled and Rx driving 1 back to its controller
                                    internally.
                                5 = Tx enabled driving 0, Rx enabled.
                                6 = Tx enabled driving 1, Rx enabled.
                                7 = Hi-Z, Rx driving 0 back to its controller internally.
                                8 = Hi-Z, Rx driving 1 back to its controller internally.
                                9 = Tx disabled, Rx enabled.
                               15 = IO-Standby is ignored for this pin (same as functional mode).
                               Others = Reserved
  - [13:10] - term           - The Pad Termination state defines the different weak pull-up and pull-down
                               settings that are supported by the buffer.
                               0 000: none
                               0 010: 5k wpd
                               0 100: 20k wpd
                               1 000: none
                               1 001: 1k wpu
                               1 011: 2k wpu
                               1 010: 5k wpu
                               1 100: 20k wpu
                               1 101: 1k & 2k wpu
                               1 111: (optional) Native controller selected by Pad Mode controls the Termination.
  - [09:08] - iosterm        - IOSTerm defines the behaviour of the termination logic when the IO Standby state
                               has been triggered.
                               0 = Same as functional mode (no change).
                               1 = Disable Pull-up and Pull-down (no on-die termination).
                               2 = Enable Pull-down.
                               3 = Enable Pull-up.
  - [07:00] - intsel         - Read-Only. The Interrupt Select defines which interrupt line driven from the
                               GPIO Controller toggles when an interrupt is detected on this pad.
