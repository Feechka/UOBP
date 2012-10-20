/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 2012 by Timothy Hobbs and
 * Copyright (C) 1995-2011 by The BRLTTY Developers.
 *
 * BRLTTY and the FCHAD software comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version. Please see the file LICENSE-GPL for details.
 *
 * Web Page: http://mielke.cc/brltty/ and  http://brmlab.cz/user/timthelion
 *
 * This software is maintained by Timothy Hobbs <timothyhobbs@seznam.cz>.
 */
#ifndef BRLTTY
#include "uobp_braille.h"
#endif

/*Information passed to a frame handler or node initializer
 providing the values pertaining to a frame or subframe respectively.*/
struct FrameInfo{
  BrailleDisplay * brl;
  unsigned char  * info;
  uint16_t length;
  Capability * capabilities[NUM_CAPABILITIES];
  CapabilityState * capabilityStates;
  GioEndpoint * gioEndpoint;
};

/////////////////////////////////////////////////
///Frame Handlers////////////////////////////////
/////////////////////////////////////////////////
#define MAX_NUM_HANDLERS 5
#define NUM_FRAME_TYPES 3
#define MAX_NUM_FRAME_SUBTYPES 3

void initializeFrameHandlers(
 void (*frameHandlers[NUM_FRAME_TYPES]
                     [MAX_NUM_FRAME_SUBTYPES]
                     [MAX_NUM_HANDLERS])
  (FrameInfo frameInfo));

void * getFrameHandler (
 unsigned char frameType
 ,unsigned char frameSubType);

void handleFrame(
 uint16_t length
 ,unsigned char type
 ,unsigned char subType
 ,unsigned char * information
 ,void * uncastedFrameInfo);
//////////////////////////////////////////////////
///Event Handling/////////////////////////////////
//////////////////////////////////////////////////
void callFrameEventHandler(
 void (*handler[])(FrameInfo * frameInfo),
 FrameInfo * frameInfo);
#define ADD_HANDLER_SUCCESS 0
#define ADD_HANDLER_FAIL_MAX_NUM_HANDLERS_OVERFLOW 1
unsigned char addFrameEventHandler(
 void (*handler[])(FrameInfo * frameInfo)
 ,void (*addition)(FrameInfo * frameInfo));


