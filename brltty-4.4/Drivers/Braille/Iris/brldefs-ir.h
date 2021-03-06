/*
 * libbrlapi - A library providing access to braille terminals for applications.
 *
 * Copyright (C) 2002-2010 by The BRLTTY Developers.
 *
 * libbrlapi comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU Lesser General Public License, as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any
 * later version. Please see the file LICENSE-LGPL for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

#ifndef BRLTTY_INCLUDED_IR_BRLDEFS
#define BRLTTY_INCLUDED_IR_BRLDEFS

#define IR_DEFAULT_LATCH_DELAY 1000

#define IR_MAXWINDOWSIZE 40 /* Maximum size for braille window and visual display */
#define IR_DT_SMALL_WINDOWSIZE 32 /* Size of braille window on the Iris S */
#define IR_OPT_SERIALNUMBERRESPONSE_LENGTH 6
#define IR_OPT_SERIALNUMBERRESPONSE_NOWINDOWLENGTH 0xff

typedef enum {
  IR_DT_KB, /* Iris KB -> with PC-style keyboard */
  IR_DT_LARGE, /* Iris Large model, simply called Iris with no suffix */
  IR_DT_SMALL /* Iris S (for small) model */
} IrisDeviceType;

typedef enum {
  IR_IPT_BrailleKeys     = 'B',
  IR_IPT_LinearKeys      = 'C',
  IR_IPT_InteractiveKey  = 'I',
  IR_IPT_SerialNumberResponse    = 'S', 
  IR_IPT_XtKeyCode       = 'U',
  IR_IPT_XtKeyCodeRepeat = 'u',
  IR_IPT_VersionResponse = 'V'
} IrisInputPacketType;

typedef enum {
  IR_OPT_WriteBraille   = 'B',
  IR_OPT_WriteVisual    = 'L',
  IR_OPT_SerialNumberRequest = 'S',
  IR_OPT_VersionRequest = 'V'
} IrisOutputPacketType;

typedef enum {
  /* linear keys */
  IR_KEY_L1 = 0X00,
  IR_KEY_L2 = 0X01,
  IR_KEY_L3 = 0X02,
  IR_KEY_L4 = 0X03,
  IR_KEY_L5 = 0X05,
  IR_KEY_L6 = 0X06,
  IR_KEY_L7 = 0X07,
  IR_KEY_L8 = 0X08,

  /* braille keys */
  IR_KEY_Dot1      = 0X10,
  IR_KEY_Dot2      = 0X11,
  IR_KEY_Dot3      = 0X12,
  IR_KEY_Dot4      = 0X13,
  IR_KEY_Dot5      = 0X14,
  IR_KEY_Dot6      = 0X15,
  IR_KEY_Dot7      = 0X16,
  IR_KEY_Dot8      = 0X17,
  IR_KEY_Backspace = 0X18,
  IR_KEY_Space     = 0X19,

  /* special keys */
  IR_KEY_Menu = 0X20,
  IR_KEY_Z    = 0X21
} IR_NavigationKey;

typedef enum {
  IR_SET_NavigationKeys = 0,
  IR_SET_RoutingKeys,
  IR_SET_Xt,
  IR_SET_XtE0,
  IR_SET_XtE1
} IR_KeySet;

#endif /* BRLTTY_INCLUDED_IR_BRLDEFS */ 
