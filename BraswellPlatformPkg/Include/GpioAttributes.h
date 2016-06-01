/** @file
  This file provides the definitions of GPIO attributes

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _GPIO_ATTRIBUTES_H_
#define _GPIO_ATTRIBUTES_H_

#define GPIO_ATTRIBUTE_HIGH   1
#define GPIO_ATTRIBUTE_LOW    0
#define GPIO_ATTRIBUTE_NA     0xFF
//
//  Attribute: GPIO_USAGE
//
typedef enum {
  USAGE_GPIO = 0,
  USAGE_GPO = 1,
  USAGE_GPI = 2,
  USAGE_HIZ = 3,
  USAGE_Native = 4,
  USAGE_None = 0xFF
} GPIO_ATTRIBUTE_USAGE;

//
//  Attribute: INT_TRIGGER_TYPE
//
typedef enum {
  TRIGGER_Edge_High = 0,
  TRIGGER_Edge_Low = 1,
  TRIGGER_Edge_Both = 2,
  TRIGGER_Level_High = 3,
  TRIGGER_Level_Low = 4,
  TRIGGER_None = 0xFF
} GPIO_ATTRIBUTE_TRIGGER_TYPE;

//
//  Attribute: DRIVE_TYPE
//
typedef enum {
  DRIVE_Open_Drain = 0,
  DRIVE_Push_Pull = 1,
  DRIVE_None = 0xFF
} GPIO_ATTRIBUTE_DRIVE_TYPE;

//
//  Attribute: PULL_DIRECTION
//
typedef enum {
  DIRECTION_Pull_Up = 0,
  DIRECTION_Pull_Down = 1,
  DIRECTION_None = 0xFF
} GPIO_ATTRIBUTE_PULL_DIRECTION;

//
//  Attribute: PULL_STRENGTH
//
typedef enum {
  STRENGTH_1K = 0,
  STRENGTH_2K = 1,
  STRENGTH_5K = 2,
  STRENGTH_10K = 3,
  STRENGTH_20K = 4,
  STRENGTH_40K = 5,
  STRENGTH_None = 0xFF
} GPIO_ATTRIBUTE_PULL_STRENGTH;

//
//  Attribute: INT_TYPE
//
typedef enum {
  INT_Direct = 0,
  INT_Shared = 1,
  INT_SMI = 2,
  INT_SCI = 3,
  INT_None = 0xFF
} GPIO_ATTRIBUTE_INT_TYPE;

//
//  Attribute: SHARE_MODE
//
typedef enum {
  MODE_Shared = 0,
  MODE_Exclusive = 1,
  MODE_SharedAndWake = 2,
  MODE_ExclusiveAndWake = 3
} GPIO_ATTRIBUTE_SHARE_MODE;

//
//  Attribute: INVERT_RX_TX
//
typedef enum
{
  GLITCH_DISABLE = 0,
  GLITCH_EN_EdgeDetect,
  GLITCH_EN_RX_Data,
  GLITCH_EN_Edge_RX_Data,
} GPIO_ATTRIBUTE_GLITCH_CFG;

//
//  Attribute: INVERT_RX_TX
//
typedef enum
{
  GPIO_No_Inversion      = 0,
  GPIO_Inv_RX_Enable     = 0x1,
  GPIO_Inv_TX_Enable     = 0x2,
  GPIO_Inv_RX_TX_Enable  = 0x3,
  GPIO_Inv_RX_Data       = 0x4,
  GPIO_Inv_RX_Data_TX_Enable = 0x6,
  GPIO_Inv_TX_Data       = 0x8,
} GPIO_ATTRIBUTE_INVERT_RX_TX;
#endif
