#include "Rylr993.h"

#define OPMODE_UNSETABLE 665

String _address = "";

Rylr998::Rylr998(HardwareSerial &port, String address) {
  serialPort = &port;

  // Setup serial connection
  serialPort->begin(LORA_BAUD);
  while (!(*serialPort)) {
    ; // Wait for serial port to connect
  }

  // Go through setup routine
  CommandResponse opMode = getCommandResponse("OPMODE=?");
  if (opMode.message.equals("1")) {
    DEBUG_PRINTLN("Device already in proprietary mode.")
  } else {
    if (!sendCommand("OPMODE=1")) { 
      DEBUG_PRINTLN("Setting the OPMODE failed, exiting setup...");
      return;
    };
    if (!sendResetCommand()) { 
      DEBUG_PRINTLN("Resetting the device failed, exiting setup...");
      return;
    };
  }

  CommandResponse addr = getCommandResponse(String("ADDRESS=") + address);
  if (!addr.success) { 
    DEBUG_PRINTLN("Setting the address failed with the response: " + addr.message + ", exiting setup...");
    return;
  }
  _address = address;
  
  // if (!sendCommand("NETWORKID=5")) { 
  //   DEBUG_PRINTLN("Setting the network failed, exiting setup...");
  //   return;
  // };
  if (!sendCommand("BAND=915000000")) { 
    DEBUG_PRINTLN("Setting the band failed, exiting setup...");
    return;
  };
  if (!sendCommand("PARAMETER=9,7,1,12")) { 
    DEBUG_PRINTLN("Setting the parameters failed, exiting setup...");
    return;
  };
  DEBUG_PRINTLN("Setup successful!");
}

String getAddress() {
  return _address;
}

CommandResponse Rylr998::getCommandResponse(String command) {
  // Send the AT command
  serialPort->print(F("AT"));
  if (command.length() > 0) {
    serialPort->print(F("+"));
    serialPort->print(command);
  }
  serialPort->print(F("\r\n"));

  while (serialPort->available() <= 1) {
    ; // Spin until we get a response from the device.
  }

  return messageToCommandResponse(serialPort->readString());
}


bool Rylr998::sendCommand(String command) {
  CommandResponse response = getCommandResponse(command);
  return response.success;
}

bool Rylr998::sendResetCommand() {
  // Send the ATZ command
  serialPort->print(F("ATZ\r\n"));
  
  // Wait until a response is available.
  while (serialPort->available() <= 1) {
    ;
  }

  return messageToCommandResponse(serialPort->readString()).success;
}

bool Rylr998::sendLoRaMessage(String recipient, String data) {
  int payloadLength = data.length();
  String command = "SEND=" + recipient + "," + String(payloadLength) + "," + data;
  return sendCommand(command);
}

String Rylr998::parseMessage(String msg) {
  // Expect message format: "+RCV=addr,port,data,..."
  if (msg.startsWith("+RCV=")) {
    String payload = msg.substring(5); // Remove the "+RCV=" prefix.
    int firstComma = payload.indexOf(',');
    int secondComma = payload.indexOf(',', firstComma + 1);
    int thirdComma = payload.indexOf(',', secondComma + 1);
    if (firstComma == -1 || secondComma == -1 || thirdComma == -1) {
      return "";
    }
    String dataField = payload.substring(secondComma + 1, thirdComma);
    dataField.trim();
    return dataField;
  }
  return "";
}

bool Rylr998::waitForRemoteAck(unsigned long timeout) {
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (serialPort->available()) {
      String incoming = serialPort->readStringUntil('\n');
      incoming.trim();
      String parsed = parseMessage(incoming);
      if (parsed.equals(REMOTE_ACK)) {
        return true;
      }
    }
  }
  return false;
}

bool Rylr998::sendWithAck(String recipient, String data, unsigned long ackTimeout) {
  while (true) {
    // Send the command and verify local module response.
    if (!sendLoRaMessage(recipient, data)) {
      // Local +OK not received; try again.
      continue;
    }
    // Wait for the remote ACK.
    if (waitForRemoteAck(ackTimeout)) {
      return true;
    }
    // If ACK not received, the loop retries the command.
  }
}

CommandResponse Rylr998::messageToCommandResponse(String message) {
  message = message.trim();
  // Find the last '\r\n'
  int lastSplit = message.lastIndexOf("\r\n");

  String body, lastLine;
  if (lastSplit != -1) {
    body = message.substring(0, lastSplit).trim();
    lastLine = message.substring(lastSplit + 2).trim();
  } else {
    // No split found; assume whole thing is a single line
    body = "";
    lastLine = message;
  }

  CommandResponse result;
  result.message = body;
  result.success = lastLine.equals("OK");
  return result;
}
