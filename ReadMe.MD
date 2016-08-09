      Sample UEFI Firmware for the N-series Intel(R) Pentium(R) Processor
      and Intel(R) Celeron(R) Processor Families (formerly Braswell)

The N-series Intel(R) Pentium(R) processor and Intel(R) Celeron(R) processor families
(formerly Braswell) use a multicore System on a Chip (SoC) based on a new
microarchitecture and are manufactured on Intel’s industry leading tri-gate
14nm process. http://ark.intel.com/products/codename/66094/Braswell#@Embedded 

This sample UEFI firmware is designed for N-series Intel Pentium processor and
Intel Celeron processor families, and has been verified on the Intel Cherry Hill
BSP. This sample codebase uses the Intel UDK2015 core and Intel(R) Firmware Support
Package (Intel(R) FSP) to demonstrate building a full UEFI solution by combining
open source EDK II with Intel(R) FSP binaries.

This code combines elements from github, intel.com/fsp and firmware.intel.com

===============================================================================
                            HOW TO CREATE A FULL SOURCE TREE
===============================================================================
1) Create a new folder (directory) on the root of your local development machine
   for use as your work space (for example "C:\MyWorkspace").
   
   NOTE: Some code paths are very long, and placing the working directory too
         deep below the ROOT directory may cause the path to be longer than the 
         operating system or file system's maximum path length.

2) Enter "C:\MyWorkspace", then run following git command to get the
   pentium-celeron-n-udk2015 code branch:
   git clone https://github.com/tianocore/edk2-platforms.git -b pentium-celeron-n-udk2015
  
   Once the download completes, all EDK II packages and files will be found
   under the "C:\MyWorkspace\edk2-platforms" directory.

3) Get the BaseTools binary:
   a. git clone https://github.com/tianocore/edk2-BaseTools-win32.git
   b. git checkout ea691aec89b06aa83474100df1de000a875b4ea0
   c. Put BaseTools Binary "edk2-BaseTools-win32" into MyWorkspace\edk2-platforms\BaseTools\Bin”, 
   d. Change folder name from "C:\MyWorkspace\edk2-platforms\BaseTools\Bin\edk2-BaseTools-win32”
      to "C:\MyWorkspace\edk2-platforms\BaseTools\Bin\win32”

4) Download the 'Intel(R) Firmware Support Package for Intel(R) Pentium(R) and Celeron(R)
   Processor N3000 Product Families and Intel(R) Atom™ x5-E8000 Processor'
   (BSW_MR1) from http://intel.com/fsp
   
5) Run the Intel(R) FSP installer and copy the following files into the Workspace:
   a. Default install directory: "C:\Program Files (x86)\Braswell FSP Kit MR1\FSP" 
   b. Copy FspUpdVpd.h into the "C:\MyWorkspace\edk2-platforms\ChvFspBinPkg\Include" directory.
   c. Create the "FspBinary" sub-directory under the "C:\MyWorkspace\edk2-platforms\ChvFspBinPkg"
      directory, if it does not exist ("C:\MyWorkspace\edk2-platforms\ChvFspBinPkg\FspBinary").
   d. Copy BSWFSP.fd into the "C:\MyWorkspace\edk2-platforms\ChvFspBinPkg\FspBinary" directory.
   
6) Download the 'BraswellPlatformPkg' package from firmware.intel.com:
   https://firmware.intel.com/projects/braswell-uefi
   NOTE: The directory structure of BraswellPlatformPkg changed between the 2016-06-15
   and 2016-08-05 releases. Check your BraswellPlatformPkg git hash to determine which 
   version to download.
   If BraswellPlatformPkg hash is f71cf3bf29aa216bc72f703815a7f96fb2ca0592(2016-08-05 release) or
   after this hash, please download this link:
   https://firmware.intel.com/sites/default/files/BraswellPlatformPkg_2016-08-05.zip
   If BraswellPlatformPkg hash is 471674c0aa4350c48e5e9e8f2d8962883f582275(2016-06-15 release), 
   please download this link:
   https://firmware.intel.com/sites/default/files/BraswellPlatformPkg_2016-06-15.zip

   

7) Unzip the 'BraswellPlatformPkg' package and move the contents into the Workspace:
   a. Copy the "BraswellPlatformPkg" sub-directory over the existing directory
      in the Workspace ("C:\MyWorkspace\edk2-platforms\BraswellPlatformPkg"). This will add
      files to the "Microcode", "Stitch" and "UNDI" sub-directories.

8) Follow the instructions found in the "Patch-HOWTO.txt" file located in the Workspace
   (e.g. "C:\MyWorkspace\edk2-platforms\CryptoPkg\Library\OpensslLib\Patch-HOWTO.txt")
   to install the Openssl source code.

================================================================================
                            HOW TO BUILD (WINDOWS ENVIRONMENT)
================================================================================
OS Configuration:
  Microsoft Windows 8.1 Ultimate 64-bit*
  
1. Setup Build Environment
   1) Install a C compiler (Visual Studio .NET 2008, 2010 or 2013) on the 
      build machine.
      
   2) Install version Python 2.6.4 from: https://www.python.org/download/releases/2.6.4/
      Make sure that a file with the extension of ".py" will be opened by Python.exe.
      
   3) Install the iasl compiler by downloading acpica-win-20141107.zip from the
      following location: "https://acpica.org/downloads/version-20141107" and
      place the unzipped content ("iasl.exe") into the "C:\ASL" directory on your
      local drive (create the folder "C:\ASL" if it does not exist).
       
2. Follow the instructions of "HOW TO CREATE A FULL SOURCE TREE" to create a 
   full source tree.
           
3a. Build Steps (32-bit)
   1) To build a 32-bit release version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\edk2-platforms\BraswellPlatformPkg" 
      Type the command "BuildBsw.bat /vs08 /r /ia32 BRAS" to build a release version.
      (/vs08 = Visual Studio 2008; /vs10 = Visual Studio 2010; /vs13 = Visual Studio 2013) 
      
   2) To build a 32-bit debug version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\edk2-platforms\BraswellPlatformPkg" 
      Type command "BuildBsw.bat /vs08 /d /ia32 BRAS" to build a debug version.
      (/vs08 = Visual Studio 2008; /vs10 = Visual Studio 2010; /vs13 = Visual Studio 2013) 
               
   3) After the build successfully completes, the 8MB firmware binary image
      will be located in "C:\MyWorkspace\edk2-platforms\Build\"  

3b. Build Steps (64-bit)
   1) To build a 64-bit release version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\edk2-platforms\BraswellPlatformPkg"
      Type the command "BuildBsw.bat /vs08 /r BRAS" to build a release version.
      (/vs08 = Visual Studio 2008; /vs10 = Visual Studio 2010; /vs13 = Visual Studio 2013) 
      
   2) To build a 64-bit debug version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\edk2-platforms\BraswellPlatformPkg" 
      Type command "BuildBsw.bat /vs08 /d BRAS" to build a debug version.
      (/vs08 = Visual Studio 2008; /vs10 = Visual Studio 2010; /vs13 = Visual Studio 2013) 
               
   3) After the build successfully completes, the 8MB firmware binary image
      will be located in "C:\MyWorkspace\edk2-platforms\Build\"      
      
================================================================================
                           VALIDATED OS
================================================================================ 
1. Yocto Revision 1.8 (x64)
2. Microsoft Windows 10 Enterprise, Build 10240 (x64 [64-bit] and x86 [32-bit])
3. Ubuntu 15.10 Desktop (x64)

================================================================================
                           DOCUMENTS AND RESOURCES
================================================================================ 
1. Open source platforms - EDKII using Intel FSP
http://firmware.intel.com/blog/open-source-platforms-edkii-using-intel-fsp     

2. Open Braswell UEFI Codebase - Design and Porting Guide 
https://github.com/mangguo321/Braswell/blob/master/Documents/Open_Braswell_Platform_Designing_Porting_Guide.pdf 

3. Yocto BSP for Intel N3700, N3000, N3050, N3150 Mobile and Embedded Processors
https://www.yoctoproject.org/downloads/bsps/fido18/cherry-hill-bsp-braswell

================================================================================
                                  KNOWN ISSUES
================================================================================

1. Linux build environments are not currently supported.

================================================================================
                                  MAINTAINERS
================================================================================
david.wei@intel.com
mang.guo@intel.com
tim.he@intel.com
vincent.zimmer@intel.com
mike.wu@intel.com 
shifeix.a.lu@intel.com

