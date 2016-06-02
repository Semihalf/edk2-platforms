                          Intel Sample UEFI BIOS for 
      N-series Intel Pentium Processor and Intel Celeron Processor Families (formerly Braswell)

The N-series Intel Pentium processor and Intel Celeron processor families (formerly Braswell)are the
Intel Architecture (IA) SoC that integrates the next generation Intel processor core,
Graphics, Memory Controller, and I/O interfaces into a single system-on-chip solution.

This sample UEFI BIOS is designed for N-series Intel Pentium processor and Intel Celeron processor 
families(formerly Braswell), and has been verified on Intel CherryHill CRB(Customer Reference Board). 
It is based on Intel UDK2015 (EDKII) core and Intel Braswell FSP. It is a sample codebase to show how 
FSP + open source can be used to build a full solution.


================================================================================
                            HOW TO CREATE A FULL SOURCE TREE
================================================================================
1) Create a new folder (directory) on the root of your local hard drive (development machine)
   for use as your work space (for example "C:\MyWorkspace").
   
   NOTE: Some code paths are very long, therefore, placing the working directory too deep 
         from the ROOT directory may cause the path to be longer than Windows maximum allowed 
         path length.

2) Get pentium-celeron-n-udk2015 branch:
   git clone https://github.com/tianocore/edk2-platforms.git -b pentium-celeron-n-udk2015

3) Get BaseTools Binary:
   a. git clone https://github.com/tianocore/edk2-BaseTools-win32.git
   b. git checkout ea691aec89b06aa83474100df1de000a875b4ea0
   c. Put BaseTools Binary "edk2-BaseTools-win32" into MyWorkspace\edk2-platforms\BaseTools\Bin”, 
   d. Change folder name from  "C:\MyWorkspace\edk2-platforms\BaseTools\Bin\ edk2-BaseTools-win32” to "C:\MyWorkspace\edk2-platforms\BaseTools\Bin\win32”

4) Integrate other required binarys:
   a. Get Microcode of N-series Intel Pentium processor and Intel Celeron processor form your Intel Representive (TBD); 
      Put Microcode M01406C334A.mcb and M01406C2220.mcb into "C:\MyWorkspace\BraswellPlatformPkg\Microcode".
   b. Get SEC V2.0.0.2058 of N-series Intel Pentium processor and Intel Celeron processor form your Intel Representive (TBD); 
      Put SEC_Region.bin in "C:\MyWorkspace\BraswellPlatformPkg\Stitch\SEC\BSW\2.0.0.2058".
   c. Download FSP of Intel® Pentium® and Celeron® Processor N3000 Product Families and Intel® Atom™ x5-Z8300 
      Processor (formerly Braswell, Compliant with FSP v1.1 Specification) from http://www.intel.com/fsp;
      Put FspUpdVpd.h into "C:\MyWorkspace\ChvFspBinPkg\Include";
      Put BSWFSP.fd in "C:\MyWorkspace\ChvFspBinPkg\FspBinary";
   d. Integrate other binaries such as UNDI drivers. 


================================================================================
                            HOW TO BUILD (WINDOWS ENVIRONMENT)
================================================================================
Windows System Configuration:
  Microsoft Windows 8.1 Ultimate 64-bit*
  
1. Setup Build Environment
   1) Install C compiler (the default is Visual Studio .NET 2008) in the 
      build machine.
      
   2) Obtain and install version Python 2.6.4 from: https://www.python.org/download/releases/2.6.4/
      Make sure that a file with the extension of ".py" will be opened by Python.exe.
       
2. Extract Source Code
   1) Follow the instructions of "HOW TO CREATE A FULL SOURCE TREE"  to create a 
      full source tree.
      
   2) Follow the instructions found in the file "Patch-HOWTO.txt" located in your workspace
      (e.g. "C:\MyWorkspace\CryptoPkg\Library\OpensslLib\Patch-HOWTO.txt") to install the 
      Openssl source code.
      
3. Install the iasl compiler by downloading acpica-win-20141107.zip from the following location:
   "https://acpica.org/downloads/version-20141107" and place the unzipped content ("iasl.exe") into 
   the directory "C:\ASL" on your local hard drive (create the folder "C:\ASL" if it does not exist).

4a. Build Steps (32-bit)
   1) To build a 32-bit release version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\BraswellPlatformPkg" 
      Type the command "BuildBsw.bat /r /ia32 BRAS" to build a release version.
      
   2) To build a 32-bit debug version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\BraswellPlatformPkg" 
      Type command "BuildBsw.bat /d /ia32 BRAS" to build a debug version.
               
   3) After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive: 
      
      "C:\MyWorkspace\BraswellPlatformPkg\Stitch\"

4b. Build Steps (64-bit)
   1) To build a 64-bit release version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\BraswellPlatformPkg"
      Type the command "BuildBsw.bat /r BRAS" to build a release version.
      
   2) To build a 64-bit debug version
      Open a command prompt window (make certain you "Run as Administrator"). 
      Type the command: "cd C:\MyWorkspace\BraswellPlatformPkg" 
      Type command "BuildBsw.bat /d BRAS" to build a debug version.
               
   3) After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive: 
      
      "C:\MyWorkspace\BraswellPlatformPkg\Stitch\"
      
================================================================================
                           SUPPORTED OS
================================================================================ 
1. Yocto Revision	 X64 1.8
2. Win10 Revision	 X64 Build 10586  Enterprise
3. Ubuntu	15.10 X64 Desktop

================================================================================
                           DOCUMENTS AND RESOURCES
================================================================================ 
1. Open source platforms - EDKII using Intel FSP
http://firmware.intel.com/blog/open-source-platforms-edkii-using-intel-fsp     

2. Open Braswell UEFI Codebase - Design and Porting Guide 
https://github.com/mangguo321/Braswell/blob/master/Documents/Open_Braswell_Platform_Designing_Porting_Guide.pdf 

================================================================================
                                  MAINTAINERS
================================================================================
david.wei@intel.com
mang.guo@intel.com
tim.he@intel.com
vincent.zimmer@intel.com
mike.wu@intel.com 
shifeix.a.lu@intel.com

