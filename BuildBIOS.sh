#
# Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
# 
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

##**********************************************************************
## Function define
##**********************************************************************
function Usage () {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: BuildBios.sh Build_Flags [PlatformName]  Target_Flag"
  echo
  echo "       Build_Flags:                 /MN    MinnowBoard3 (default: MN)"
  echo "       Build_Flags:                 /BG    Benson Glacier Board"
  echo "       Build_Flags:                 /MX    MinnowBoard3 Module"
  echo "       Build_Flags:                 /LH    LeafHill CRB Board"
  echo "       Build_Flags:                 /UP    AAEON UP2 Board"
  echo "       Build_Flags:                 /A     Set FabId to A "
  echo "       Build_Flags:                 /B     Set FabId to B (default:  FAB_B)"
  echo "       Build_Flags:                 /C     Set FabId to C "
  echo "       Build_Flags:                 /D     Set FabId to D "
  echo "       Build_Flags:                 /L     Flash region access permission for host CPU"
  echo "       PlatformName [optional]:     Broxton  "                   
  echo "       Target_Flag:                 Release, Debug       "
  echo
  echo "Example: BuildBios.sh /LH /D Broxton Release"
  echo
  echo "Press any key......"
  read
  exit 0
}

##**********************************************************************
## Initial Setup
##**********************************************************************

Build_Flags=

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

if [ "$1" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  Usage
fi


## Build Flags
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MN" ]; then
      BoardId=MN
      Build_Flags="$Build_Flags /MN"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/BG" ]; then
      BoardId=BG
      Build_Flags="$Build_Flags /BG"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MX" ]; then
      BoardId=MX
      Build_Flags="$Build_Flags /MX"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/UP" ]; then
      BoardId=UP
      Build_Flags="$Build_Flags /UP"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/LH" ]; then
      BoardId=LH
      Build_Flags="$Build_Flags /LH"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/D" ]; then
      FabId=D
      Build_Flags="$Build_Flags /D"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      FabId=C
      Build_Flags="$Build_Flags /C"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/B" ]; then
      FabId=B
      Build_Flags="$Build_Flags /B"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/A" ]; then
      FabId=A
      Build_Flags="$Build_Flags /A"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags /L"
      shift
    else
      break
    fi
  done

if [ "$1" == "Broxton" ]; then
       shift
 else
   echo "No parameter for board : assume Broxton"  
fi

Target_Flag=Debug

if [ "$1" == "Debug" ]; then
  Target_Flag=Debug
elif [ "$1" == "Release" ]; then
  Target_Flag=Release
else
  echo "Error - Invalid Target"
  echo "  Please review the Help screen"
  Usage
fi

echo $Build_Flags
echo $Target_Flag

cd ..
export WORKSPACE=`pwd`
export CORE_PATH=$WORKSPACE/edk2
export CONF_PATH=$WORKSPACE/Conf
export PACKAGES_PATH=$WORKSPACE:$WORKSPACE/edk2:$WORKSPACE/edk2-platforms/:$WORKSPACE/edk2-platforms/Platform:$WORKSPACE/edk2-platforms/Platform/BroxtonPlatformPkg:$WORKSPACE/edk2-platforms/Silicon/BroxtonSoC:$WORKSPACE/edk2-platforms/Silicon:$WORKSPACE/edk2-platforms/Platform/BroxtonPlatformPkg/Common

make -C $CORE_PATH/BaseTools
source $CORE_PATH/edksetup.sh BaseTools

bash ./edk2-platforms/Platform/BroxtonPlatformPkg/BuildIFWI.sh $Build_Flags Broxton $Target_Flag

