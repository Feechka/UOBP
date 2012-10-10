/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2012 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version. Please see the file LICENSE-GPL for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

/* FreedomScientific/braille.c - Braille display library
 * Freedom Scientific's Focus and PacMate series
 * Author: Dave Mielke <dave@mielke.cc>
 */

#include "prologue.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "parse.h"
#include "timing.h"

#define BRLSTAT ST_AlvaStyle
#define BRL_HAVE_PACKET_IO
#include "brl_driver.h"
#include "brldefs-fs.h"

BEGIN_KEY_NAME_TABLE(common)
  KEY_NAME_ENTRY(FS_KEY_LeftAdvance, "LeftAdvance"),
  KEY_NAME_ENTRY(FS_KEY_RightAdvance, "RightAdvance"),
  KEY_NAME_ENTRY(FS_KEY_LeftGdf, "LeftGdf"),
  KEY_NAME_ENTRY(FS_KEY_RightGdf, "RightGdf"),

  KEY_NAME_ENTRY(FS_KEY_LeftWheel, "LeftWheelPress"),
  KEY_NAME_ENTRY(FS_KEY_RightWheel, "RightWheelPress"),

  KEY_NAME_ENTRY(FS_KEY_WHEEL+0, "LeftWheelUp"),
  KEY_NAME_ENTRY(FS_KEY_WHEEL+1, "LeftWheelDown"),
  KEY_NAME_ENTRY(FS_KEY_WHEEL+2, "RightWheelDown"),
  KEY_NAME_ENTRY(FS_KEY_WHEEL+3, "RightWheelUp"),

  KEY_SET_ENTRY(FS_SET_RoutingKeys, "RoutingKey"),
  KEY_SET_ENTRY(FS_SET_NavrowKeys, "NavrowKey"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(focus)
  KEY_NAME_ENTRY(FS_KEY_Dot1, "Dot1"),
  KEY_NAME_ENTRY(FS_KEY_Dot2, "Dot2"),
  KEY_NAME_ENTRY(FS_KEY_Dot3, "Dot3"),
  KEY_NAME_ENTRY(FS_KEY_Dot4, "Dot4"),
  KEY_NAME_ENTRY(FS_KEY_Dot5, "Dot5"),
  KEY_NAME_ENTRY(FS_KEY_Dot6, "Dot6"),
  KEY_NAME_ENTRY(FS_KEY_Dot7, "Dot7"),
  KEY_NAME_ENTRY(FS_KEY_Dot8, "Dot8"),

  KEY_NAME_ENTRY(FS_KEY_Space, "Space"),
  KEY_NAME_ENTRY(FS_KEY_LeftShift, "LeftShift"),
  KEY_NAME_ENTRY(FS_KEY_RightShift, "RightShift"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(bumpers)
  KEY_NAME_ENTRY(FS_KEY_LeftBumperUp, "LeftBumperUp"),
  KEY_NAME_ENTRY(FS_KEY_LeftBumperDown, "LeftBumperDown"),
  KEY_NAME_ENTRY(FS_KEY_RightBumperUp, "RightBumperUp"),
  KEY_NAME_ENTRY(FS_KEY_RightBumperDown, "RightBumperDown"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(rockers)
  KEY_NAME_ENTRY(FS_KEY_LeftRockerUp, "LeftRockerUp"),
  KEY_NAME_ENTRY(FS_KEY_LeftRockerDown, "LeftRockerDown"),
  KEY_NAME_ENTRY(FS_KEY_RightRockerUp, "RightRockerUp"),
  KEY_NAME_ENTRY(FS_KEY_RightRockerDown, "RightRockerDown"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(pacmate)
  KEY_NAME_ENTRY(FS_KEY_HOT+0, "Hot1"),
  KEY_NAME_ENTRY(FS_KEY_HOT+1, "Hot2"),
  KEY_NAME_ENTRY(FS_KEY_HOT+2, "Hot3"),
  KEY_NAME_ENTRY(FS_KEY_HOT+3, "Hot4"),
  KEY_NAME_ENTRY(FS_KEY_HOT+4, "Hot5"),
  KEY_NAME_ENTRY(FS_KEY_HOT+5, "Hot6"),
  KEY_NAME_ENTRY(FS_KEY_HOT+6, "Hot7"),
  KEY_NAME_ENTRY(FS_KEY_HOT+7, "Hot8"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLES(focus_basic)
  KEY_NAME_TABLE(common),
  KEY_NAME_TABLE(focus),
END_KEY_NAME_TABLES

BEGIN_KEY_NAME_TABLES(focus_large)
  KEY_NAME_TABLE(common),
  KEY_NAME_TABLE(focus),
  KEY_NAME_TABLE(bumpers),
END_KEY_NAME_TABLES

BEGIN_KEY_NAME_TABLES(focus_small)
  KEY_NAME_TABLE(common),
  KEY_NAME_TABLE(focus),
  KEY_NAME_TABLE(rockers),
END_KEY_NAME_TABLES

BEGIN_KEY_NAME_TABLES(pacmate)
  KEY_NAME_TABLE(common),
  KEY_NAME_TABLE(pacmate),
END_KEY_NAME_TABLES

DEFINE_KEY_TABLE(focus_basic)
DEFINE_KEY_TABLE(focus_large)
DEFINE_KEY_TABLE(focus_small)
DEFINE_KEY_TABLE(pacmate)

BEGIN_KEY_TABLE_LIST
  &KEY_TABLE_DEFINITION(focus_basic),
  &KEY_TABLE_DEFINITION(focus_large),
  &KEY_TABLE_DEFINITION(focus_small),
  &KEY_TABLE_DEFINITION(pacmate),
END_KEY_TABLE_LIST

typedef struct {
  int (*openPort) (char **parameters, const char *device);
  void (*closePort) (void);
  int (*awaitInput) (int milliseconds);
  int (*readBytes) (void *buffer, int length);
  int (*writePacket) (const void *buffer, int length);
} InputOutputOperations;

static const InputOutputOperations *io;
static int outputPayloadLimit;

static const int serialBaud = 57600;
static int serialCharactersPerSecond;

#include "io_serial.h"
static SerialDevice *serialDevice = NULL;

static int
openSerialPort (char **parameters, const char *device) {
  if ((serialDevice = serialOpenDevice(device))) {
    if (serialRestartDevice(serialDevice, serialBaud)) return 1;

    serialCloseDevice(serialDevice);
    serialDevice = NULL;
  }
  return 0;
}

static void
closeSerialPort (void) {
  if (serialDevice) {
    serialCloseDevice(serialDevice);
    serialDevice = NULL;
  }
}

static int
awaitSerialInput (int milliseconds) {
  return serialAwaitInput(serialDevice, milliseconds);
}

static int
readSerialBytes (void *buffer, int length) {
  return serialReadData(serialDevice, buffer, length, 0, 0);
}

static int
writeSerialPacket (const void *buffer, int length) {
  return serialWriteData(serialDevice, buffer, length);
}

static const InputOutputOperations serialOperations = {
  openSerialPort, closeSerialPort,
  awaitSerialInput, readSerialBytes, writeSerialPacket
};

#include "io_usb.h"

static UsbChannel *usbChannel = NULL;

static int
openUsbPort (char **parameters, const char *device) {
  static const UsbChannelDefinition definitions[] = {
    { /* Focus 1 */
      .vendor=0X0F4E, .product=0X0100,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=2, .outputEndpoint=1
    }
    ,
    { /* PAC Mate */
      .vendor=0X0F4E, .product=0X0111,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=2, .outputEndpoint=1
    }
    ,
    { /* Focus 2 */
      .vendor=0X0F4E, .product=0X0112,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=2, .outputEndpoint=1
    }
    ,
    { /* Focus Blue */
      .vendor=0X0F4E, .product=0X0114,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=2, .outputEndpoint=1
    }
    ,
    { .vendor=0 }
  };

  if ((usbChannel = usbFindChannel(definitions, (void *)device))) {
    return 1;
  }
  return 0;
}

static void
closeUsbPort (void) {
  if (usbChannel) {
    usbCloseChannel(usbChannel);
    usbChannel = NULL;
  }
}

static int
awaitUsbInput (int milliseconds) {
  return usbAwaitInput(usbChannel->device, usbChannel->definition.inputEndpoint, milliseconds);
}

static int
readUsbBytes (void *buffer, int length) {
  int count = usbReapInput(usbChannel->device, usbChannel->definition.inputEndpoint, buffer, length, 0, 0);
  if (count == -1)
    if (errno == EAGAIN)
      count = 0;
  return count;
}

static int
writeUsbPacket (const void *buffer, int length) {
  return usbWriteEndpoint(usbChannel->device, usbChannel->definition.outputEndpoint, buffer, length, 1000);
}

static const InputOutputOperations usbOperations = {
  openUsbPort, closeUsbPort,
  awaitUsbInput, readUsbBytes, writeUsbPacket
};

#include "io_bluetooth.h"

static BluetoothConnection *bluetoothConnection = NULL;

static int
openBluetoothPort (char **parameters, const char *device) {
  return (bluetoothConnection = bthOpenConnection(device, 1, 0)) != NULL;
}

static void
closeBluetoothPort (void) {
  if (bluetoothConnection) {
    bthCloseConnection(bluetoothConnection);
    bluetoothConnection = NULL;
  }
}

static int
awaitBluetoothInput (int milliseconds) {
  return bthAwaitInput(bluetoothConnection, milliseconds);
}

static int
readBluetoothBytes (void *buffer, int length) {
  return bthReadData(bluetoothConnection, buffer, length, 0, 0);
}

static int
writeBluetoothPacket (const void *buffer, int length) {
  return bthWriteData(bluetoothConnection, buffer, length);
}

static const InputOutputOperations bluetoothOperations = {
  openBluetoothPort, closeBluetoothPort,
  awaitBluetoothInput, readBluetoothBytes, writeBluetoothPacket
};

typedef enum {
  PKT_QUERY  = 0X00, /* host->unit: request device information */
  PKT_ACK    = 0X01, /* unit->host: acknowledge packet receipt */
  PKT_NAK    = 0X02, /* unit->host: negative acknowledge, report error */
  PKT_KEY    = 0X03, /* unit->host: key event */
  PKT_BUTTON = 0X04, /* unit->host: routing button event */
  PKT_WHEEL  = 0X05, /* unit->host: whiz wheel event */
  PKT_HVADJ  = 0X08, /* host->unit: set braille display voltage */
  PKT_BEEP   = 0X09, /* host->unit: sound short beep */
  PKT_CONFIG = 0X0F, /* host->unit: configure device options */
  PKT_INFO   = 0X80, /* unit->host: response to query packet */
  PKT_WRITE  = 0X81, /* host->unit: write to braille display */
  PKT_EXTKEY = 0X82  /* unit->host: extended keys event */
} PacketType;

typedef enum {
  PKT_ERR_TIMEOUT   = 0X30, /* no data received from host for a while */
  PKT_ERR_CHECKSUM  = 0X31, /* incorrect checksum */
  PKT_ERR_TYPE      = 0X32, /* unsupported packet type */
  PKT_ERR_PARAMETER = 0X33, /* invalid parameter */
  PKT_ERR_SIZE      = 0X34, /* write size too large */
  PKT_ERR_POSITION  = 0X35, /* write position too large */
  PKT_ERR_OVERRUN   = 0X36, /* message queue overflow */
  PKT_ERR_POWER     = 0X37, /* insufficient USB power */
  PKT_ERR_SPI       = 0X38  /* timeout on SPI bus */
} PacketError;

typedef enum {
  PKT_EXT_HVADJ    = 0X08, /* error in varibraille packet */
  PKT_EXT_BEEP     = 0X09, /* error in beep packet */
  PKT_EXT_CLEAR    = 0X31, /* error in ClearMsgBuf function */
  PKT_EXT_LOOP     = 0X32, /* timing loop in ParseCommands function */
  PKT_EXT_TYPE     = 0X33, /* unknown packet type in ParseCommands function */
  PKT_EXT_CMDWRITE = 0X34, /* error in CmdWrite function */
  PKT_EXT_UPDATE   = 0X7E, /* error in update packet */
  PKT_EXT_DIAG     = 0X7F, /* error in diag packet */
  PKT_EXT_QUERY    = 0X80, /* error in query packet */
  PKT_EXT_WRITE    = 0X81  /* error in write packet */
} PacketExtended;

typedef enum {
  OPT_EXTKEY = 0X01  /* send extended key events */
} UnitOption;

typedef struct {
  unsigned char type;
  unsigned char arg1;
  unsigned char arg2;
  unsigned char arg3;
} PacketHeader;

typedef struct {
  PacketHeader header;
  union {
    unsigned char bytes[0X100];
    struct {
      char manufacturer[24];
      char model[16];
      char firmware[8];
    } info;
    struct {
      unsigned char bytes[4];
    } extkey;
  } payload;
} Packet;

typedef struct {
  const KeyTableDefinition *keyTableDefinition;
  signed char hotkeysRow;
} ModelTypeEntry;

typedef enum {
  MOD_TYPE_Focus,
  MOD_TYPE_Pacmate
} ModelType;

static const ModelTypeEntry modelTypeTable[] = {
  [MOD_TYPE_Focus] = {
    .keyTableDefinition = &KEY_TABLE_DEFINITION(focus_basic),
    .hotkeysRow = -1
  }
  ,
  [MOD_TYPE_Pacmate] = {
    .keyTableDefinition = &KEY_TABLE_DEFINITION(pacmate),
    .hotkeysRow = 1
  }
};

typedef struct {
  const char *identifier;
  const DotsTable *dotsTable;
  unsigned char cellCount;
  unsigned char type;
} ModelEntry;

static const DotsTable dotsTable_Focus1 = {0X01, 0X02, 0X04, 0X10, 0X20, 0X40, 0X08, 0X80};

static const ModelEntry modelTable[] = {
  { .identifier = "Focus 40",
    .dotsTable = &dotsTable_ISO11548_1,
    .cellCount = 40,
    .type = MOD_TYPE_Focus
  }
  ,
  { .identifier = "Focus 44",
    .dotsTable = &dotsTable_Focus1,
    .cellCount = 44,
    .type = MOD_TYPE_Focus
  }
  ,
  { .identifier = "Focus 70",
    .dotsTable = &dotsTable_Focus1,
    .cellCount = 70,
    .type = MOD_TYPE_Focus
  }
  ,
  { .identifier = "Focus 80",
    .dotsTable = &dotsTable_ISO11548_1,
    .cellCount = 80,
    .type = MOD_TYPE_Focus
  }
  ,
  { .identifier = "Focus 84",
    .dotsTable = &dotsTable_Focus1,
    .cellCount = 84,
    .type = MOD_TYPE_Focus
  }
  ,
  { .identifier = "pm display 20",
    .dotsTable = &dotsTable_ISO11548_1,
    .cellCount = 20,
    .type = MOD_TYPE_Pacmate
  }
  ,
  { .identifier = "pm display 40",
    .dotsTable = &dotsTable_ISO11548_1,
    .cellCount = 40,
    .type = MOD_TYPE_Pacmate
  }
  ,
  { .identifier = NULL }
};

static const ModelEntry *model;
static const KeyTableDefinition *keyTableDefinition;

static unsigned char outputBuffer[84];

static int writeFirst;
static int writeLast;
static int writingFirst;
static int writingLast;

typedef void (*AcknowledgementHandler) (int ok);
static AcknowledgementHandler acknowledgementHandler;
static TimeValue acknowledgementTime;
static int acknowledgementsMissing;
static unsigned char configFlags;
static int firmnessSetting;

static union {
  Packet packet;
  unsigned char bytes[sizeof(Packet)];
} inputBuffer;
static int inputCount;

static uint64_t oldKeys;

static int
writePacket (
  BrailleDisplay *brl,
  unsigned char type,
  unsigned char arg1,
  unsigned char arg2,
  unsigned char arg3,
  const unsigned char *data
) {
  Packet packet;
  int size = sizeof(packet.header);
  unsigned char checksum = 0;

  checksum -= (packet.header.type = type);
  checksum -= (packet.header.arg1 = arg1);
  checksum -= (packet.header.arg2 = arg2);
  checksum -= (packet.header.arg3 = arg3);

  if (data) {
    unsigned char length = packet.header.arg1;
    int index;

    for (index=0; index<length; index+=1)
      checksum -= (packet.payload.bytes[index] = data[index]);

    packet.payload.bytes[length] = checksum;
    size += length + 1;
  }

  logOutputPacket(&packet, size);
  {
    int result = io->writePacket(&packet, size);
    if (result != -1) brl->writeDelay += (size * 1000 / serialCharactersPerSecond) + 1;
    return result;
  }
}

static void
logNegativeAcknowledgement (const Packet *packet) {
  const char *problem;
  const char *component;

  switch (packet->header.arg1) {
    default:
      problem = "unknown problem";
      break;
    case PKT_ERR_TIMEOUT:
      problem = "timeout during packet transmission";
      break;
    case PKT_ERR_CHECKSUM:
      problem = "incorrect checksum";
      break;
    case PKT_ERR_TYPE:
      problem = "unknown packet type";
      break;
    case PKT_ERR_PARAMETER:
      problem = "invalid parameter value";
      break;
    case PKT_ERR_SIZE:
      problem = "write size too large";
      break;
    case PKT_ERR_POSITION:
      problem = "write start too large";
      break;
    case PKT_ERR_OVERRUN:
      problem = "message FIFO overflow";
      break;
    case PKT_ERR_POWER:
      problem = "insufficient USB power";
      break;
    case PKT_ERR_SPI:
      problem = "SPI bus timeout";
      break;
  }

  switch (packet->header.arg2) {
    default:
      component = "unknown component";
      break;
    case PKT_EXT_HVADJ:
      component = "VariBraille packet";
      break;
    case PKT_EXT_BEEP:
      component = "beep packet";
      break;
    case PKT_EXT_CLEAR:
      component = "ClearMsgBuf function";
      break;
    case PKT_EXT_LOOP:
      component = "timing loop of ParseCommands function";
      break;
    case PKT_EXT_TYPE:
      component = "ParseCommands function";
      break;
    case PKT_EXT_CMDWRITE:
      component = "CmdWrite function";
      break;
    case PKT_EXT_UPDATE:
      component = "update packet";
      break;
    case PKT_EXT_DIAG:
      component = "diag packet";
      break;
    case PKT_EXT_QUERY:
      component = "query packet";
      break;
    case PKT_EXT_WRITE:
      component = "write packet";
      break;
  }

  logMessage(LOG_WARNING, "Negative Acknowledgement: [%02X] %s in [%02X] %s",
             packet->header.arg1, problem,
             packet->header.arg2, component);
}

static void
handleConfigAcknowledgement (int ok) {
  configFlags = 0;
}

static void
handleFirmnessAcknowledgement (int ok) {
  firmnessSetting = -1;
}

static void
handleWriteAcknowledgement (int ok) {
  if (!ok) {
    if ((writeFirst == -1) || (writingFirst < writeFirst)) writeFirst = writingFirst;
    if ((writeLast == -1) || (writingLast > writeLast)) writeLast = writingLast;
  }
}

static void
setAcknowledgementHandler (AcknowledgementHandler handler) {
  acknowledgementHandler = handler;
  getCurrentTime(&acknowledgementTime);
}

static int
writeRequest (BrailleDisplay *brl) {
  if (acknowledgementHandler) return 1;

  if (configFlags) {
    if (writePacket(brl, PKT_CONFIG, configFlags, 0, 0, NULL) == -1) return 0;
    setAcknowledgementHandler(handleConfigAcknowledgement);
    return 1;
  }

  if (firmnessSetting >= 0) {
    if (writePacket(brl, PKT_HVADJ, firmnessSetting, 0, 0, NULL) == -1) return 0;
    setAcknowledgementHandler(handleFirmnessAcknowledgement);
    return 1;
  }

  if (writeLast != -1) {
    unsigned int count = writeLast + 1 - writeFirst;
    unsigned char buffer[count];
    int truncate = count > outputPayloadLimit;

    if (truncate) count = outputPayloadLimit;
    translateOutputCells(buffer, &outputBuffer[writeFirst], count);
    if (writePacket(brl, PKT_WRITE, count, writeFirst, 0, buffer) == -1) return 0;

    setAcknowledgementHandler(handleWriteAcknowledgement);
    writingFirst = writeFirst;

    if (truncate) {
      writingLast = (writeFirst += count) - 1;
    } else {
      writingLast = writeLast;
      writeFirst = -1;
      writeLast = -1;
    }

    return 1;
  }

  return 1;
}

static void
updateCells (
  BrailleDisplay *brl,
  const unsigned char *cells,
  unsigned char count,
  unsigned char offset
) {
  unsigned int from;
  unsigned int to;

  if (cellsHaveChanged(&outputBuffer[offset], cells, count, &from, &to, NULL)) {
    int first = from + offset;
    int last = to + offset - 1;

    if ((writeFirst == -1) || (first < writeFirst)) writeFirst = first;
    if (last > writeLast) writeLast = last;
  }
}

static int
readPacket (BrailleDisplay *brl, Packet *packet) {
  while (1) {
    int size = sizeof(PacketHeader);
    int hasPayload = 0;

    if (inputCount >= sizeof(PacketHeader)) {
      if (inputBuffer.packet.header.type & 0X80) {
        hasPayload = 1;
        size += inputBuffer.packet.header.arg1 + 1;
      }
    }

    if (size <= inputCount) {
      logInputPacket(inputBuffer.bytes, size);

      if (hasPayload) {
        unsigned char checksum = 0;
        int index;

        for (index=0; index<size; index+=1) checksum -= inputBuffer.bytes[index];
        if (checksum) logMessage(LOG_WARNING, "Input packet checksum error.");
      }

      memcpy(packet, &inputBuffer, size);
      memmove(&inputBuffer.bytes[0], &inputBuffer.bytes[size],
              inputCount -= size);
      return size;
    }

  retry:
    {
      int count = io->readBytes(&inputBuffer.bytes[inputCount], size-inputCount);

      if (count < 1) {
        if (count == -1) {
          logSystemError("read");
        } else if ((count == 0) && (inputCount > 0)) {
          if (io->awaitInput(1000)) goto retry;
          if (errno != EAGAIN) count = -1;
          logPartialPacket(inputBuffer.bytes, inputCount);
          inputCount = 0;
        }

        return count;
      }
      acknowledgementsMissing = 0;

      if (!inputCount) {
        static const unsigned char packets[] = {
          PKT_ACK, PKT_NAK,
          PKT_KEY, PKT_EXTKEY, PKT_BUTTON, PKT_WHEEL,
          PKT_INFO
        };
        int first;

        for (first=0; first<count; first+=1)
          if (memchr(packets, inputBuffer.bytes[first], sizeof(packets)))
            break;

        if (first) {
          logDiscardedBytes(inputBuffer.bytes, first);
          memmove(&inputBuffer.bytes[0], &inputBuffer.bytes[first], count-=first);
        }
      }

      inputCount += count;
    }
  }
}

static int
getPacket (BrailleDisplay *brl, Packet *packet) {
  while (1) {
    int count = readPacket(brl, packet);

    if (count > 0) {
      switch (packet->header.type) {
        {
          int ok;

        case PKT_NAK:
          logNegativeAcknowledgement(packet);

          if (!acknowledgementHandler) {
            logMessage(LOG_WARNING, "Unexpected NAK.");
            continue;
          }

          switch (packet->header.arg1) {
            case PKT_ERR_TIMEOUT: {
              int originalLimit = outputPayloadLimit;

              if (outputPayloadLimit > model->cellCount)
                outputPayloadLimit = model->cellCount;

              if (outputPayloadLimit > 1)
                outputPayloadLimit--;

              if (outputPayloadLimit != originalLimit)
                logMessage(LOG_WARNING, "Maximum payload length reduced from %d to %d.",
                           originalLimit, outputPayloadLimit);
              break;
            }
          }

        handleNegativeAcknowledgement:
          ok = 0;
          goto handleAcknowledgement;

        case PKT_ACK:
          if (!acknowledgementHandler) {
            logMessage(LOG_WARNING, "Unexpected ACK.");
            continue;
          }

          ok = 1;
        handleAcknowledgement:
          acknowledgementHandler(ok);
          acknowledgementHandler = NULL;
          if (writeRequest(brl)) continue;

          count = -1;
          break;
        }
      }
    } else if ((count == 0) && acknowledgementHandler &&
               (millisecondsSince(&acknowledgementTime) > 500)) {
      if (++acknowledgementsMissing < 5) {
        logMessage(LOG_WARNING, "Missing ACK; assuming NAK.");
        goto handleNegativeAcknowledgement;
      }

      logMessage(LOG_WARNING, "Too many missing ACKs.");
      count = -1;
    }

    return count;
  }
}

static int
setFirmness (BrailleDisplay *brl, BrailleFirmness setting) {
  firmnessSetting = setting * 0XFF / BRL_FIRMNESS_MAXIMUM;
  return writeRequest(brl);
}

static int
brl_construct (BrailleDisplay *brl, char **parameters, const char *device) {
  if (isSerialDevice(&device)) {
    io = &serialOperations;
  } else if (isUsbDevice(&device)) {
    io = &usbOperations;
  } else if (isBluetoothDevice(&device)) {
    io = &bluetoothOperations;
  } else {
    unsupportedDevice(device);
    return 0;
  }

  inputCount = 0;
  outputPayloadLimit = 0XFF;
  serialCharactersPerSecond = serialBaud / 10;

  if (io->openPort(parameters, device)) {
    int tries = 0;

    while (io->awaitInput(10)) {
      Packet packet;
      int count = readPacket(brl, &packet);
      if (count == -1) goto failure;
    }
    if (errno != EAGAIN) goto failure;

    while (writePacket(brl, PKT_QUERY, 0, 0, 0, NULL) > 0) {
      int acknowledged = 0;
      model = NULL;

      while (io->awaitInput(100)) {
        Packet response;
        int count = readPacket(brl, &response);
        if (count == -1) goto failure;

        switch (response.header.type) {
          case PKT_INFO:
            logMessage(LOG_DEBUG, "Manufacturer: %s", response.payload.info.manufacturer);
            logMessage(LOG_DEBUG, "Model: %s", response.payload.info.model);
            logMessage(LOG_DEBUG, "Firmware: %s", response.payload.info.firmware);

            model = modelTable;
            while (model->identifier) {
              if (strcmp(response.payload.info.model, model->identifier) == 0) break;
              model += 1;
            }

            if (!model->identifier) {
              static ModelEntry generic;
              model = &generic;
              logMessage(LOG_WARNING, "Detected unknown model: %s", response.payload.info.model);

              memset(&generic, 0, sizeof(generic));
              generic.identifier = "Generic";
              generic.cellCount = 20;
              generic.dotsTable = &dotsTable_ISO11548_1;
              generic.type = MOD_TYPE_Pacmate;

              {
                typedef struct {
                  const char *identifier;
                  const DotsTable *dotsTable;
                } ExceptionEntry;
                static const ExceptionEntry exceptionTable[] = {
                  {"Focus", &dotsTable_Focus1},
                  {NULL   , NULL         }
                };
                const ExceptionEntry *exception = exceptionTable;
                while (exception->identifier) {
                  if (strncmp(response.payload.info.model, exception->identifier, strlen(exception->identifier)) == 0) {
                    generic.dotsTable = exception->dotsTable;
                    break;
                  }
                  exception++;
                }
              }

              {
                const char *word = strrchr(response.payload.info.model, ' ');
                if (word) {
                  int size;
                  if (isInteger(&size, ++word)) {
                    static char identifier[sizeof(response.payload.info.model)];
                    generic.cellCount = size;
                    snprintf(identifier, sizeof(identifier), "%s %d",
                             generic.identifier, generic.cellCount);
                    generic.identifier = identifier;
                  }
                }
              }
            }

            if (model) {
              keyTableDefinition = modelTypeTable[model->type].keyTableDefinition;
              makeOutputTable(model->dotsTable[0]);
              memset(outputBuffer, 0, model->cellCount);
              writeFirst = 0;
              writeLast = model->cellCount - 1;

              acknowledgementHandler = NULL;
              acknowledgementsMissing = 0;
              configFlags = 0;
              firmnessSetting = -1;

              if (model->type == MOD_TYPE_Focus) {
                unsigned char firmwareVersion = response.payload.info.firmware[0] - '0';

                if (firmwareVersion >= 3) {
                  configFlags |= 0X02;

                  if (model->cellCount < 80) {
                    keyTableDefinition = &KEY_TABLE_DEFINITION(focus_small);
                  } else {
                    keyTableDefinition = &KEY_TABLE_DEFINITION(focus_large);
                  }
                }
              }

              oldKeys = 0;

              logMessage(LOG_INFO, "Detected %s: cells=%d, firmware=%s",
                         model->identifier,
                         model->cellCount,
                         response.payload.info.firmware);
            }
            break;

          case PKT_ACK:
            acknowledged = 1;
            break;

          case PKT_NAK:
            logNegativeAcknowledgement(&response);
          default:
            acknowledged = 0;
            model = NULL;
            break;
        }

        if (acknowledged && model) {
          brl->textColumns = model->cellCount;
          brl->textRows = 1;

          brl->keyBindings = keyTableDefinition->bindings;
          brl->keyNameTables = keyTableDefinition->names;

          brl->setFirmness = setFirmness;

          if (!writeRequest(brl)) break;
          return 1;
        }
      }
      if (errno != EAGAIN) break;

      if (++tries == 3) break;
    }

  failure:
    io->closePort();
  }
  return 0;
}

static void
brl_destruct (BrailleDisplay *brl) {
  io->closePort();
}

static int
brl_writeWindow (BrailleDisplay *brl, const wchar_t *text) {
  updateCells(brl, brl->buffer, model->cellCount, 0);
  return writeRequest(brl);
}

static void
updateKeys (uint64_t newKeys, unsigned char keyBase, unsigned char keyCount) {
  const FS_KeySet set = FS_SET_NavigationKeys;
  FS_NavigationKey key = keyBase;

  FS_NavigationKey pressKeys[keyCount];
  unsigned int pressCount = 0;

  uint64_t keyBit = 0X1 << keyBase;
  newKeys <<= keyBase;
  newKeys |= oldKeys & ~(((0X1 << keyCount) - 1) << keyBase);

  while (oldKeys != newKeys) {
    uint64_t oldKey = oldKeys & keyBit;
    uint64_t newKey = newKeys & keyBit;

    if (oldKey && !newKey) {
      enqueueKeyEvent(set, key, 0);
      oldKeys &= ~keyBit;
    } else if (newKey && !oldKey) {
      pressKeys[pressCount++] = key;
      oldKeys |= keyBit;
    }

    keyBit <<= 1;
    key += 1;
  }

  while (pressCount) enqueueKeyEvent(set, pressKeys[--pressCount], 1);
}

static int
brl_readCommand (BrailleDisplay *brl, KeyTableCommandContext context) {
  Packet packet;
  int count;

  while ((count = getPacket(brl, &packet))) {
    if (count == -1) return BRL_CMD_RESTARTBRL;

    switch (packet.header.type) {
      default:
        break;

      case PKT_KEY: {
        uint64_t newKeys = packet.header.arg1 |
                           (packet.header.arg2 << 8) |
                           (packet.header.arg3 << 16);

        updateKeys(newKeys, 0, 24);
        continue;
      }

      case PKT_EXTKEY: {
        uint64_t newKeys = packet.payload.extkey.bytes[0];

        updateKeys(newKeys, 24, 8);
        continue;
      }

      case PKT_BUTTON: {
        unsigned char key = packet.header.arg1;
        unsigned char press = (packet.header.arg2 & 0X01) != 0;
        unsigned char set = packet.header.arg3;

        if (set == modelTypeTable[model->type].hotkeysRow) {
          static const unsigned char keys[] = {
            FS_KEY_LeftGdf,
            FS_KEY_HOT+0, FS_KEY_HOT+1, FS_KEY_HOT+2, FS_KEY_HOT+3,
            FS_KEY_HOT+4, FS_KEY_HOT+5, FS_KEY_HOT+6, FS_KEY_HOT+7,
            FS_KEY_RightGdf
          };
          static const unsigned char keyCount = ARRAY_COUNT(keys);
          const unsigned char base = (model->cellCount - keyCount) / 2;

          if (key < base) {
            key = FS_KEY_LeftAdvance;
          } else if ((key -= base) >= keyCount) {
            key = FS_KEY_RightAdvance;
          } else {
            key = keys[key];
          }

          set = FS_SET_NavigationKeys;
        } else {
          set += 1;
        }

        enqueueKeyEvent(set, key, press);
        continue;
      }

      case PKT_WHEEL: {
        const FS_KeySet set = FS_SET_NavigationKeys;
        const FS_NavigationKey key = FS_KEY_WHEEL + ((packet.header.arg1 >> 3) & 0X7);
        unsigned int count = packet.header.arg1 & 0X7;

        while (count) {
          enqueueKeyEvent(set, key, 1);
          enqueueKeyEvent(set, key, 0);
          count -= 1;
        }

        continue;
      }
    }

    logMessage(LOG_WARNING, "unsupported packet: %02X %02X %02X %02X",
               packet.header.type,
               packet.header.arg1,
               packet.header.arg2,
               packet.header.arg3);
  }

  return EOF;
}

static ssize_t
brl_readPacket (BrailleDisplay *brl, void *buffer, size_t length) {
  Packet packet;
  int count = readPacket(brl, &packet);
  if (count > 0) {
    if (count > sizeof(packet.header)) count--;
    if (length < count) {
      logMessage(LOG_WARNING, "Input packet buffer too small: %d < %d", (int)length, count);
      count = length;
    }
    memcpy(buffer, &packet, count);
  }
  return count;
}

static ssize_t
brl_writePacket (BrailleDisplay *brl, const void *packet, size_t length) {
  const unsigned char *bytes = packet;
  int size = 4;
  if (length >= size) {
    int hasPayload = 0;
    if (bytes[0] & 0X80) {
      size += bytes[1];
      hasPayload = 1;
    }
    if (length >= size) {
      if (length > size)
        logMessage(LOG_WARNING, "Output packet buffer larger than necessary: %d > %d",
                   (int)length, size);
      return writePacket(brl, bytes[0], bytes[1], bytes[2], bytes[3],
                         (hasPayload? &bytes[4]: NULL));
    }
  }
  logMessage(LOG_WARNING, "Output packet buffer too small: %d < %d", (int)length, size);
  return 0;
}

static int
brl_reset (BrailleDisplay *brl) {
  return 0;
}
