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

#include "prologue.h"

#include <string.h>
#include <errno.h>

#include "log.h"

#include "brl_driver.h"
#include "brldefs-hm.h"

BEGIN_KEY_NAME_TABLE(routing)
  KEY_SET_ENTRY(HM_SET_RoutingKeys, "RoutingKey"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(dots)
  KEY_NAME_ENTRY(HM_KEY_Dot1, "Dot1"),
  KEY_NAME_ENTRY(HM_KEY_Dot2, "Dot2"),
  KEY_NAME_ENTRY(HM_KEY_Dot3, "Dot3"),
  KEY_NAME_ENTRY(HM_KEY_Dot4, "Dot4"),
  KEY_NAME_ENTRY(HM_KEY_Dot5, "Dot5"),
  KEY_NAME_ENTRY(HM_KEY_Dot6, "Dot6"),
  KEY_NAME_ENTRY(HM_KEY_Dot7, "Dot7"),
  KEY_NAME_ENTRY(HM_KEY_Dot8, "Dot8"),
  KEY_NAME_ENTRY(HM_KEY_Space, "Space"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLE(brailleSense)
  KEY_NAME_ENTRY(HM_KEY_BS_F1, "F1"),
  KEY_NAME_ENTRY(HM_KEY_BS_F2, "F2"),
  KEY_NAME_ENTRY(HM_KEY_BS_F3, "F3"),
  KEY_NAME_ENTRY(HM_KEY_BS_F4, "F4"),

  KEY_NAME_ENTRY(HM_KEY_BS_Backward, "Backward"),
  KEY_NAME_ENTRY(HM_KEY_BS_Forward, "Forward"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLES(sense)
  KEY_NAME_TABLE(routing),
  KEY_NAME_TABLE(dots),
  KEY_NAME_TABLE(brailleSense),
END_KEY_NAME_TABLES

BEGIN_KEY_NAME_TABLE(syncBraille)
  KEY_NAME_ENTRY(HM_KEY_SB_LeftUp, "LeftUp"),
  KEY_NAME_ENTRY(HM_KEY_SB_LeftDown, "LeftDown"),
  KEY_NAME_ENTRY(HM_KEY_SB_RightUp, "RightUp"),
  KEY_NAME_ENTRY(HM_KEY_SB_RightDown, "RightDown"),
END_KEY_NAME_TABLE

BEGIN_KEY_NAME_TABLES(sync)
  KEY_NAME_TABLE(routing),
  KEY_NAME_TABLE(syncBraille),
END_KEY_NAME_TABLES

DEFINE_KEY_TABLE(sense)
DEFINE_KEY_TABLE(sync)

BEGIN_KEY_TABLE_LIST
  &KEY_TABLE_DEFINITION(sense),
  &KEY_TABLE_DEFINITION(sync),
END_KEY_TABLE_LIST

typedef struct {
  int (*openPort) (const char *device);
  int (*configurePort) (void);
  void (*closePort) ();
  int (*awaitInput) (int milliseconds);
  int (*readBytes) (unsigned char *buffer, int length, int wait);
  int (*writeBytes) (const unsigned char *buffer, int length);
} InputOutputOperations;
static const InputOutputOperations *io;

static unsigned int charactersPerSecond;

typedef enum {
  IPT_CURSOR  = 0X00,
  IPT_KEYS    = 0X01,
  IPT_CELLS   = 0X02
} InputPacketType;

typedef union {
  unsigned char bytes[10];

  struct {
    unsigned char start;
    unsigned char type;
    unsigned char count;
    unsigned char data;
    unsigned char reserved[4];
    unsigned char checksum;
    unsigned char end;
  } PACKED data;
} PACKED InputPacket;

static int
readByte (unsigned char *byte, int wait) {
  int count = io->readBytes(byte, 1, wait);
  if (count > 0) return 1;

  if (count == 0) errno = EAGAIN;
  return 0;
}

static int
readPacket (BrailleDisplay *brl, InputPacket *packet) {
  const int length = 10;
  int offset = 0;

  while (1) {
    unsigned char byte;

    {
      int started = offset > 0;
      if (!readByte(&byte, started)) {
        if (started) logPartialPacket(packet->bytes, offset);
        return 0;
      }
    }

    if (!offset) {
      if (byte != 0XFA) {
        logIgnoredByte(byte);
        continue;
      }
    }

    packet->bytes[offset++] = byte;
    if (offset == length) {
      if (byte == 0XFB) {
        {
          int checksum = -packet->data.checksum;

          {
            int i;
            for (i=0; i<offset; i+=1) checksum += packet->bytes[i];
          }

          if ((checksum & 0XFF) != packet->data.checksum) {
            logInputProblem("Incorrect Input Checksum", packet->bytes, offset);
          }
        }

        logInputPacket(packet->bytes, offset);
        return length;
      }

      {
        const unsigned char *start = memchr(packet->bytes+1, packet->bytes[0], offset-1);
        const unsigned char *end = packet->bytes + offset;
        if (!start) start = end;
        logDiscardedBytes(packet->bytes, start-packet->bytes);
        memmove(packet->bytes, start, (offset = end - start));
      }
    }
  }
}

static int
writePacket (
  BrailleDisplay *brl,
  unsigned char type, unsigned char mode,
  const unsigned char *data1, size_t length1,
  const unsigned char *data2, size_t length2
) {
  unsigned char packet[2 + 1 + 1 + 2 + length1 + 1 + 1 + 2 + length2 + 1 + 4 + 1 + 2];
  unsigned char *byte = packet;
  unsigned char *checksum;

  /* DS */
  *byte++ = type;
  *byte++ = type;

  /* M */
  *byte++ = mode;

  /* DS1 */
  *byte++ = 0XF0;

  /* Cnt1 */
  *byte++ = (length1 >> 0) & 0XFF;
  *byte++ = (length1 >> 8) & 0XFF;

  /* D1 */
  byte = mempcpy(byte, data1, length1);

  /* DE1 */
  *byte++ = 0XF1;

  /* DS2 */
  *byte++ = 0XF2;

  /* Cnt2 */
  *byte++ = (length2 >> 0) & 0XFF;
  *byte++ = (length2 >> 8) & 0XFF;

  /* D2 */
  byte = mempcpy(byte, data2, length2);

  /* DE2 */
  *byte++ = 0XF3;

  /* Reserved */
  {
    int count = 4;
    while (count--) *byte++ = 0;
  }

  /* Chk */
  *(checksum = byte++) = 0;

  /* DE */
  *byte++ = 0XFD;
  *byte++ = 0XFD;

  {
    unsigned char sum = 0;
    const unsigned char *ptr = packet;
    while (ptr != byte) sum += *ptr++;
    *checksum = sum;
  }

  {
    int size = byte - packet;
    logOutputPacket(packet, size);
    if (io->writeBytes(packet, size) == -1) return 0;
    brl->writeDelay += (size * 1000 / charactersPerSecond) + 1;
  }

  return 1;
}

typedef struct {
  const char *modelName;
  const KeyTableDefinition *keyTableDefinition;
  int (*getCellCount) (BrailleDisplay *brl, unsigned int *count);
} ProtocolOperations;
static const ProtocolOperations *protocol;

static int
getBrailleSenseCellCount (BrailleDisplay *brl, unsigned int *count) {
  *count = 32;
  return 1;
}

static const ProtocolOperations brailleSenseOperations = {
  "Braille Sense", &KEY_TABLE_DEFINITION(sense),
  getBrailleSenseCellCount
};

static int
getSyncBrailleCellCount (BrailleDisplay *brl, unsigned int *count) {
  static const unsigned char data[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  if (writePacket(brl, 0XFB, 0X01, data, sizeof(data), NULL, 0)) {
    InputPacket packet;

    while (io->awaitInput(1000)) {
      if (readPacket(brl, &packet)) {
        if (packet.data.type == IPT_CELLS) {
          *count = packet.data.data;
          return 1;
        }
      }
    }
  }

  return 0;
}

static const ProtocolOperations syncBrailleOperations = {
  "SyncBraille", &KEY_TABLE_DEFINITION(sync),
  getSyncBrailleCellCount
};

/* Serial IO */
#include "io_serial.h"

static SerialDevice *serialDevice = NULL;
#define SERIAL_BAUD 115200

static int
openSerialPort (const char *device) {
  if (!(serialDevice = serialOpenDevice(device))) return 0;
  protocol = &brailleSenseOperations;
  return 1;
}

static int
configureSerialPort (void) {
  if (!serialRestartDevice(serialDevice, SERIAL_BAUD)) return 0;
  return 1;
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
readSerialBytes (unsigned char *buffer, int count, int wait) {
  const int timeout = 100;
  return serialReadData(serialDevice, buffer, count,
                        (wait? timeout: 0), timeout);
}

static int
writeSerialBytes (const unsigned char *buffer, int length) {
  return serialWriteData(serialDevice, buffer, length);
}

static const InputOutputOperations serialOperations = {
  openSerialPort, configureSerialPort, closeSerialPort,
  awaitSerialInput, readSerialBytes, writeSerialBytes
};

/* USB IO */
#include "io_usb.h"

static UsbChannel *usbChannel = NULL;

static int
openUsbPort (const char *device) {
  static const UsbChannelDefinition definitions[] = {
    { /* Braille Sense */
      .vendor=0X045E, .product=0X930A,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=1, .outputEndpoint=2,
      .disableAutosuspend=1,
      .data=&brailleSenseOperations
    }
    ,
    { /* Sync Braille */
      .vendor=0X0403, .product=0X6001,
      .configuration=1, .interface=0, .alternative=0,
      .inputEndpoint=1, .outputEndpoint=2,
      .data=&syncBrailleOperations
    }
    ,
    { .vendor=0 }
  };

  if ((usbChannel = usbFindChannel(definitions, (void *)device))) {
    protocol = usbChannel->definition.data;
    return 1;
  }
  return 0;
}

static int
configureUsbPort (void) {
  return 1;
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
  return usbAwaitInput(usbChannel->device,
                       usbChannel->definition.inputEndpoint,
                       milliseconds);
}

static int
readUsbBytes (unsigned char *buffer, int length, int wait) {
  const int timeout = 100;
  int count = usbReapInput(usbChannel->device,
                           usbChannel->definition.inputEndpoint,
                           buffer, length,
                           (wait? timeout: 0), timeout);
  if (count != -1) return count;
  if (errno == EAGAIN) return 0;
  return -1;
}

static int
writeUsbBytes (const unsigned char *buffer, int length) {
  return usbWriteEndpoint(usbChannel->device,
                          usbChannel->definition.outputEndpoint,
                          buffer, length, 1000);
}

static const InputOutputOperations usbOperations = {
  openUsbPort, configureUsbPort, closeUsbPort,
  awaitUsbInput, readUsbBytes, writeUsbBytes
};

/* Bluetooth IO */
#include "io_bluetooth.h"

static BluetoothConnection *bluetoothConnection = NULL;

static int
openBluetoothPort (const char *device) {
  if (!(bluetoothConnection = bthOpenConnection(device, 4, 0))) return 0;
  protocol = &brailleSenseOperations;
  return 1;
}

static int
configureBluetoothPort (void) {
  return 1;
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
readBluetoothBytes (unsigned char *buffer, int length, int wait) {
  const int timeout = 100;
  return bthReadData(bluetoothConnection, buffer, length,
                     (wait? timeout: 0), timeout);
}

static int
writeBluetoothBytes (const unsigned char *buffer, int length) {
  int count = bthWriteData(bluetoothConnection, buffer, length);
  if (count != length) {
    if (count == -1) {
      logSystemError("Bluetooth write");
    } else {
      logMessage(LOG_WARNING, "Trunccated bluetooth write: %d < %d", count, length);
    }
  }
  return count;
}

static const InputOutputOperations bluetoothOperations = {
  openBluetoothPort, configureBluetoothPort, closeBluetoothPort,
  awaitBluetoothInput, readBluetoothBytes, writeBluetoothBytes
};

static unsigned char previousCells[40];

static int
writeCells (BrailleDisplay *brl) {
  const size_t count = brl->textColumns * brl->textRows;
  unsigned char cells[count];

  translateOutputCells(cells, previousCells, count);

  return writePacket(brl, 0XFC, 0X01, cells, count, NULL, 0);
}

static int
clearCells (BrailleDisplay *brl) {
  memset(previousCells, 0, brl->textColumns*brl->textRows);
  return writeCells(brl);
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

  if (io->openPort(device)) {
    if (io->configurePort()) {
      charactersPerSecond = SERIAL_BAUD / 10;
      logMessage(LOG_INFO, "detected: %s", protocol->modelName);

      if (protocol->getCellCount(brl, &brl->textColumns) ||
          protocol->getCellCount(brl, &brl->textColumns)) {
        brl->textRows = 1;
        brl->keyBindings = protocol->keyTableDefinition->bindings;
        brl->keyNameTables = protocol->keyTableDefinition->names;

        makeOutputTable(dotsTable_ISO11548_1);
  
        if (clearCells(brl)) return 1;
      }
    }

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
  size_t count = brl->textColumns * brl->textRows;

  if (cellsHaveChanged(previousCells, brl->buffer, count, NULL, NULL, NULL)) {
    if (!writeCells(brl)) return 0;
  }

  return 1;
}

static int
brl_readCommand (BrailleDisplay *brl, KeyTableCommandContext context) {
  InputPacket packet;
  int length;

  while ((length = readPacket(brl, &packet))) {
    switch (packet.data.type) {
      case IPT_CURSOR: {
        unsigned char key = packet.data.data;

        enqueueKeyEvent(HM_SET_RoutingKeys, key, 1);
        enqueueKeyEvent(HM_SET_RoutingKeys, key, 0);
        continue;
      }

      case IPT_KEYS: {
        uint16_t bits = packet.data.reserved[0] | (packet.data.reserved[1] << 8);
        uint16_t bit = 0X1;
        unsigned char key = 0;

        unsigned char keys[0X10];
        unsigned int keyCount = 0;

        while (++key <= 0X10) {
          if (bits & bit) {
            enqueueKeyEvent(HM_SET_NavigationKeys, key, 1);
            keys[keyCount++] = key;
          }

          bit <<= 1;
        }

        if (keyCount) {
          do {
            enqueueKeyEvent(HM_SET_NavigationKeys, keys[--keyCount], 0);
          } while (keyCount);

          continue;
        }

        break;
      }

      default:
        break;
    }

    logUnexpectedPacket(&packet, length);
  }
  if (errno != EAGAIN) return BRL_CMD_RESTARTBRL;

  return EOF;
}
