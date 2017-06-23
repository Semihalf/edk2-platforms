# **EDK II Minimized firmware for Intel(R)Kabylake platforms**

##Features
The Minimized Kabylake provides the minimal feature of the Kabylake BIOS.

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

edk2-BaseTools-win32 repository
	``git clone https://github.com/tianocore/edk2-BaseTools-win32.git``

edk2-platforms repository
	``git clone https://github.com/tianocore/edk2-platforms.git -b devel-MinPlatform``

edk2-non-osi repository
	``git clone https://github.com/tianocore/edk2-non-osi.git -b devel-MinPlatform``

FSP repository
	``git clone https://github.com/IntelFsp/FSP.git -b KabylakeFsp0001``

### Build

1. Open command window, goto the workspace dir, e.g. c:\Kabylake.
2. Type "cd KabylakeOpenBoardPkg\KabylakeRvp3".
3. Type "GitEdk2MinKabylake.bat" to setup GIT environment.
4. Type "prep" and make prebuild finish for debug build, "prep r" for rlease build.
5. Type "bld" to build kabylake BIOS image.

The user can run FITC tool to build full image for flash (BIOS + ME + DESC). The user can also burn the BIOS image to highest part of flash region directly.

##Known limitations
1. This firmware project has only been validated on the Intel KabylakeRVP3 board.
2. This firmware project has only been validated booting to Microsoft Windows 10 x64 with AHCI mode and Integrated Graphic Device.
3. This firmware project has only been validated using the Microsoft Visual Studio 2015 compiler.

