#ifndef RYLR993_H
#define RYLR993_H

#include <Arduino.h>

#define SUCCESS_MESSAGE "OK";
#define REMOTE_ACK "ACK"

#define LORA_BAUD 9600

#ifdef DEBUG
	#define DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
	#define DEBUG_PRINTF(...) { Serial.printf(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
	#define DEBUG_PRINTF(...) {}
#endif

struct CommandResponse {
  String message;
  bool success;
};

class Rylr998 {
  public:
    // Constructor takes a reference to the hardware serial port (e.g., Serial1)
    Rylr998(HardwareSerial &port, String address);
    
    // The the current address.
    String getAddress();
    
    // Send an AT command (optionally with an additional parameter) and return the response string.
    CommandResponse getCommandResponse(String command);
    
    // Send a command and check if the response equals SUCCESS_RESPONSE.
    bool sendCommand(String command);
    
    // Send a reset command and check if the response equals SUCCESS_RESPONSE.
    bool sendResetCommand();
    
    // Send a message to a recipient address.
    bool sendLoRaMessage(String recipient, String data);
    
    // Parse an incoming message formatted as "+RCV=addr,port,data,..." and return the data payload.
    String parseMessage(String msg);
    
    // Wait for a remote ACK (delivered as a SEND message that arrives as a +RCV message)
    // within the specified timeout (in milliseconds).
    bool waitForRemoteAck(unsigned long timeout);
    
    // Send a command and wait for a remote ACK.
    // If either the local module’s response isn’t SUCCESS_RESPONSE or a remote ACK isn’t received,
    // it will retry until the ACK is received.
    bool sendWithAck(String recipient, String data, unsigned long ackTimeout = 500);
    
  private:
    HardwareSerial *serialPort;

    CommandResponse messageToCommandResponse(String message);
};

#endif
