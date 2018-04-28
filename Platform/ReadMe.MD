# ﻿Intel Atom(R) Processor E3900 Series Platforms

This codebase is designed for Intel Atom(R) Processor E3900 Series platforms
(https://www.intel.com/content/www/us/en/embedded/products/apollo-lake/overview.html) (formerly Apollo Lake).

## **Download the sources**

### Download official released sources:

 Please refer to the release notes to download official released sources.

 Get release notes from https://firmware.intel.com/projects/minnowboard3

### Download the latest sources:

(Official releases sources is recommanded as it is passed stability testing)
```
1. Create a new directory for use as your WORKSPACE (e.g. "C:\MyWorkspace").

2. Download below repository and required files to this WORKSPACE:

edk2 repository
    git clone -b vUDK2018 https://github.com/tianocore/edk2.git

edk2-platforms repository
    git clone https://github.com/tianocore/edk2-platforms.git -b devel-IntelAtomProcessorE3900

Intel(R) Firmware Support Package(Intel(R) FSP)
   1.git clone -b ApolloLake https://github.com/IntelFsp/FSP.git
   2.cd FSP
   3.git checkout a57c66616e78b471515a1e1862796bf61d3824d8
   4.Copy the ApolloLakeFspBinPkg to the folder
     "C:\MyWorkspace\edk2-platforms\Silicon\BroxtonSoC\BroxtonFspPkg".

3. Download UEFI UNDI driver:
   1.Install UEFI UNDI driver for Intel i210 Ethernet Controller
   Download PREBOOT.EXE Version 21.1 from the "Intel® Ethernet Connections 
   Boot Utility, Preboot Images, and EFI Drivers" page.
     https://downloadcenter.intel.com/download/19186/Intel-Ethernet-Connections-Boot-Utility-Preboot-Images-and-EFI-Drivers
   Windows user:
   a. Install PREBOOT.EXE into the default folder (C:\Intel21.1).
   b. Copy the UEFI x64 PCI-E gigabit driver (C:\Intel21.1\APPS\EFI\EFIx64\E7320X3.EFI) 
      to the platform package directory below, create the folder if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\UNDI\I210PcieUndiDxe"
   Linux user:
   a. Use command "unzip PREBOOT.EXE" to extract UEFI x64 PCI-E gigabit driver 
      (preboot/APPS/EFI/EFIx64/E7320X3.EFI)
   b. Copy E7320X3.EFI to platform package directory below, create the folder
      if it does not exist:
      "~/src/MyWorkspace/edk2-platforms/Platform/BroxtonPlatformPkg/Common/Binaries/UNDI/I210PcieUndiDxe"

   2.Install UNDI driver for AX88179 USB-to-LAN adapter
   a. Download AX88179_178A_UEFI_V2.7.0.zip from http://www.asix.com.tw/FrootAttach/driver/AX88179_178A_UEFI_V2.7.0.zip.
   b. Unzip AX88179_178A_UEFI_V2.7.0.zip to get AX88179_178A_UEFI_V2.7.0_X64.efi and AX88179_178A_UEFI_V2.7.0_IA32.efi.
   C. Copy AX88179_178A_UEFI_V2.7.0_X64.efi and AX88179_178A_UEFI_V2.7.0_IA32.efi to below directory, create the folder
      if it does not exist:
      Windows user: "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\UNDI\AX88179\"
      Linux user: "~/src/MyWorkspace/edk2-platforms/Platform/BroxtonPlatformPkg/Common/Binaries/UNDI/AX88179/"

4. Follow the instructions found in the "OpenSSL-HOWTO.txt" file located in the Workspace
   (e.g. "C:\MyWorkspace\edk2-platforms\Core\CryptoPkg\Library\OpensslLib\OpenSSL-HOWTO.txt")
   to install the OpenSSL* source code and enable the CryptoPkg module.   

5. Download MinnowBoard 3 Binary Object Modules
   a. Download the "MinnowBoard_v3-0.69-Binary.Objects.zip" from the project page:
      https://firmware.intel.com/projects/minnowboard3

   b. Unzip and copy the two folders (MinnowBoard3, MinnowBoard3Next) into
      the directory below, and create the folder if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Board\"
```

## **Windows Build Instructions**

### Pre-requisites

* Microsoft Visual Studio.
  - Install C compiler (Visual Studio .NET* VS2013/VS2015) on the build machine.
* ASL compiler: Available from http://www.acpica.org
  - Unzip and copy ```iasl.exe``` into ```C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Iasl```.
* NASM assembler:  Available from: http://www.nasm.us/
  - Unzip the 32-bit & 64-bit versions of nasm.exe to the directories listed below
  ```
  C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win32
  C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win64
  ```
* Python 2.7.10:  Available from: https://www.python.org/downloads/release/python-2710/
  - Install Python and make sure that a file with the extension of ".py" will be opened by Python.exe.

### Build

1. Open command window, goto the workspace dir, e.g. c:\MyWorkspace.
2. Type "cd edk2-platforms".
3. Build

      Examples:      
       To build release version 32-bit BIOS for Minnowboard 3 FAB B with VS2013,
         BuildBIOS.bat /vs13 /B /IA32 Broxton Release
            
       To build release version 64-bit BIOS for Minnowboard 3 FAB B with VS2013,
         BuildBIOS.bat /vs13 /B /x64 Broxton Release

## **Linux Build Instructions**

### Pre-requisites

* Linux System Configuration.
  - Ubuntu 16.04 64 bit
  - GCC version 5 or above
* Use below command to install required packages:
    ```
      # sudo apt-get update
      # sudo apt-get install nasm 
      # sudo apt-get install iasl
      # sudo apt-get install build-essential
      # sudo apt-get install uuid-dev
	```

### Build

1. Open a command prompt, goto the platform package "MyWorkspace/edk2-platforms".
2. Build
   - Type "./BuildBIOS.sh /B Broxton Release" to build a release version.
   - Type "./BuildBIOS.sh /B Broxton Debug" to build a debug version.
   
## **Related Materials**

### Projects Supported by this codebase

   (1) MinnowBoard 3 open hardware project:
  
   http://minnowboard.org/ -- Platform Information.

   https://firmware.intel.com/projects/minnowboard3 -- Release notes, pre-built release/debug binaries, build instructions, and binary objects required to compile firmware.



