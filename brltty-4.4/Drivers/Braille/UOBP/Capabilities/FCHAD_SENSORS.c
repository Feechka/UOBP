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
#include "FCHAD_SENSORS.h"
/////////////////////////////////////////////////
///FCHAD Sensors/////////////////////////////////
/////////////////////////////////////////////////
void * initFCHADSensorsState(FrameInfo frameInfo){
  unsigned char * information = frameInfo.info;
  BrailleDisplay * brl = frameInfo.brl;
  FCHADSensorsState * thisState =
    (FCHADSensorsState *)
     malloc(sizeof(FCHADSensorsState));
  thisState->rows =
    (uint16_t)information[0]<<8;
  thisState->rows +=
    (uint16_t)information[1];
  thisState->cols =
    (uint16_t)information[2]<<8;
  thisState->cols +=
    (uint16_t)information[3];
  thisState->sensors =
    malloc(thisState->rows*thisState->cols);
  /* Despite the fact that the UOBP standard supports multiple nodes of a single capability,
  BRLTTY doesn't really support having multiple braille buffers.
  It is not meaningfull to add such support at this time.
  The case in which a device would have multiple nodes
  (say we could plug in a larger 2D sensor grid)
  is far away in the future.
  Here we just use the largest node as the "primary" node
  and let the nodes with smaller number of sensors be subsets of the "primary".*/
  if(thisState->rows>brl->textRows){
   brl->textRows    =thisState->rows;
   brl->resizeRequired  = 1;
  }
  if(thisState->cols>brl->textColumns){
   brl->textColumns =thisState->cols;
   brl->resizeRequired  = 1;
  }
  thisState->prevRow=0;
  thisState->prevCol=0;
  return thisState;
}

void freeFCHADSensorsState
 (FCHADSensorsState * thisSensorState){
  free(thisSensorState->sensors);
  free(thisSensorState);
}

void reactToSensorUp(FrameInfo * frameInfo){
  reactToSensorAction(frameInfo,SENSOR_UP);
}

void reactToSensorDown(FrameInfo * frameInfo){
  reactToSensorAction(frameInfo,SENSOR_DOWN);
}

void reactToSensorAction(FrameInfo * frameInfo,
                         unsigned char action){
  unsigned char * information = frameInfo->info;
  unsigned char node = information[0];
  if(!frameInfo->capabilityStates[4][node]){
   logMessage(LOG_WARNING,"Received frame event for uninitialized node %d of capability 4.",node);
   return;
  }
  CapabilityState * thisCapabilityNode =
   frameInfo->capabilityStates[4][node];

  FCHADSensorsState * myState =
    (FCHADSensorsState*)thisCapabilityNode->state;
  uint16_t row =
    (uint16_t)information[1]<<8;
  row +=
    (uint16_t)information[2];
  uint16_t col =
    (uint16_t)information[3]<<8;
  col +=
    (uint16_t)information[4];
  /*If the touch was inside the buffer
  (It will be outside only in the case of a coruption somewhere.*/
  if(row < myState->rows
           &&
     col < myState->cols){
   logMessage(LOG_INFO,"Sensor action %d cols %d",action,myState->cols);
   myState->sensors[row][col]=action;
   updateFCHADFromSensorValues
    (thisCapabilityNode
    ,node
    ,frameInfo);
  }
}

void updateFCHADFromSensorValues
 (CapabilityState * thisCapabilityNode
 ,unsigned char node
 ,FrameInfo * frameInfo){
  /* We want to always display the character marked by the top left corner
  (or top right in the case of a left handed user)
  of the area that is currently being touched.
  So first we figure out what the top left corner being touched is,
  and if it is different than the previous top left corner,
  we send a new signal to the device
  to tell it to display that new character.
  If nothing is being touched,
  than we send a signal telling the device to switch it's FCHAD Cell off.*/

 if(thisCapabilityNode){
  FCHADSensorsState * myState =
    (FCHADSensorsState*)thisCapabilityNode->state;
  uint16_t i,j;
  unsigned char found=0;
  j=0;/*Due to a warning...*/
  /*For each FCHAD_CELL node that is paired with this array of sensors,
  display the proper character to that node.*/
  #define FCHAD_CELL_CAPABILITY_ID 3
  unsigned char pair;
  for(pair=0;pair<thisCapabilityNode->numPairs;pair++)
  if(thisCapabilityNode->pairs[pair].capability
                        ==
                FCHAD_CELL_CAPABILITY_ID){
   FCHADCellState * pairState =
    (FCHADCellState *)frameInfo->capabilityStates
      [FCHAD_CELL_CAPABILITY_ID]
      [node]->state;
   if(pairState->cellHandedness==RIGHT_HANDED){
    for(i=0;i<myState->rows&&!found;i++)
     for(j=0;j<myState->cols&&!found;j++)
       if(myState->sensors[i][j])found=1;
    j--;i--;
   }else{/*LEFT_HANDED*/
    for(i=0;i<myState->rows&&!found;i++)
     for(j=myState->cols;j>-1&&!found;j--)
       if(myState->sensors[i][j])found=1;
    j++;i--;
   }
   unsigned char charToDisplay=0;
   if(found){
     charToDisplay=frameInfo->brl->buffer[i*j];
   }
   unsigned char information[3];
   information[0]=pair;
   information[1]=charToDisplay;
   /*Braille characters are only 8 bits,
   but some FCHAD Cells may have up to 16
   (so as to allow dpad like directional symbols.)
   We can safely ignore these potential extra dots.*/
   information[2]=0;
   sendFrame(3,1,1,information,frameInfo->gioEndpoint);
   #ifdef LOG_EVERYTHING
   logMessage(LOG_INFO,"#EVENT_LOG# displaying char:%c\n",
          charToDisplay);
   #endif
  }
 }
}

//////////////////////////////////////////////////
///Sensor logging/////////////////////////////////
//////////////////////////////////////////////////
#ifdef LOG_EVERYTHING
void logSensorDown(FrameInfo * frameInfo){
  logSensorAction(frameInfo,SENSOR_DOWN);
}

void logSensorUp(FrameInfo * frameInfo){
  logSensorAction(frameInfo,SENSOR_UP);
}

void logSensorAction(FrameInfo * frameInfo,
                     unsigned char action){
  unsigned char * information = frameInfo->info;
  uint16_t row =
    (uint16_t)information[1]<<8;
  row +=
    (uint16_t)information[2];
  uint16_t col =
    (uint16_t)information[3]<<8;
  col +=
    (uint16_t)information[4];
  //TODO log time as well!
  logMessage(LOG_INFO,"#EVENT_LOG# Sensor at row:%d col:%d ",row,col);
  if(action==SENSOR_DOWN)
   logMessage(LOG_INFO,"DOWN");
  if(action==SENSOR_UP)
   logMessage(LOG_INFO,"UP");
}
#endif