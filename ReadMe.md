# **EDK II Minimized firmware for Intel(R) platforms**

## Features
* The Minimized Kabylake provides the minimal feature of the Kabylake BIOS.
* The Minimized Purley provides the minimal feature of the Purley BIOS.

## **Windows Build Instructions**

### Pre-requisites

* GIT client: Available from https://git-scm.com/downloads
* Microsoft Visual Studio.
  - Visual Studio 2015 recommended and is used in the examples below.
* ASL compiler: Available from http://www.acpica.org
  - Install into ```C:\ASL``` to match default tools_def.txt configuration.
* NASM assembler:  Available from: http://www.nasm.us/
  - Install into ```C:\NASM``` to match default tools_def.txt configuration.

### Download the required component

1. Create a new directory for an EDK II WORKSPACE.

2. Download below repository to this WORKSPACE:

edk2 repository
	``git clone https://github.com/tianocore/edk2.git``

edk2-platforms repository
	``git clone https://github.com/tianocore/edk2-platforms.git -b devel-MinPlatform``

edk2-non-osi repository
	``git clone https://github.com/tianocore/edk2-non-osi.git -b devel-MinPlatform``

FSP repository
	``git clone https://github.com/IntelFsp/FSP.git -b Kabylake``

### Build

For MinKabylake
1. Open command window, goto the workspace dir, e.g. c:\Kabylake.
2. Type "cd edk2-platforms\Platform\Intel\KabylakeOpenBoardPkg\KabylakeRvp3".
3. Type "GitEdk2MinKabylake.bat" to setup GIT environment.
4. Type "prep" and make prebuild finish for debug build, "prep r" for rlease build.
5. Type "bld" to build kabylake BIOS image.

For MinPurley
1. Open command window, goto the workspace dir, e.g. c:\Purley.
2. Type "cd edk2-platforms\Platform\Intel\PurleyOpenBoardPkg\BoardMtOlympus".
3. Type "GitEdk2MinMtOlympus.bat" to setup GIT environment.
4. Type "bld" to build Purley BIOS image, "bld release" for release build.

The user can run FITC tool to build full image for flash (BIOS + ME + DESC). The user can also burn the BIOS image to highest part of flash region directly.
The validated version of iasl compiler that can build MinPurley is 20180629. Older version may generate ACPI build errors. 

### Known limitations

For MinKabylake
1. This firmware project has only been validated on the Intel KabylakeRVP3 board.
2. This firmware project has only been validated booting to Microsoft Windows 10 x64 with AHCI mode and Integrated Graphic Device.
3. This firmware project has only been validated using the Microsoft Visual Studio 2015 compiler.

For MinPurley
1. This firmware project has only been validated on the Microsoft MtOlympus board.
2. This firmware project has only been validated booting to Microsoft Windows Server 2016 with NVME on M.2 slot.
3. This firmware project has only been validated using the Microsoft Visual Studio 2015 compiler.

