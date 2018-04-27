/** @file
  Timer instance for I2C Pei Library.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include "I2CAccess.h"
#include "I2CDelayPei.h"
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/Stall.h>


