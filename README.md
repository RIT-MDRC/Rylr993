# Project Name: Rylr993

## Author(s)
- Clinten Hopkins (cmh3586@rit.edu)

## Maintainer(s)
- Clinten Hopkins (cmh3586@rit.edu)

## Description

The `Rylr993` library provides a C++ interface for communicating with RYLR998 LoRa modules using the Arduino framework. It facilitates sending AT commands to configure the module, transmit messages, and receive acknowledgments, all while abstracting away lower-level serial communication.

This library includes:
- A setup routine for ensuring the LoRa module is in the correct mode.
- Methods for sending commands and parsing responses.
- Reliable message transmission with acknowledgment handling.
- Message parsing and debug logging (conditionally compiled with `DEBUG`).

### Key Features:
- **Modular Setup**: Automatically configures the LoRa module's operating mode, band, and parameters.
- **Command Abstractions**: Wraps AT commands in convenient methods.
- **Address Management**: Set and retrieve module address.
- **Acknowledgment Support**: Waits for remote acknowledgment to ensure message delivery.
- **Debug Macros**: Lightweight debugging through conditional compilation.

### Usage

To use the library, instantiate the `Rylr998` class by passing a hardware serial port and an address string:
```cpp
Rylr998 lora(Serial1, "0012");
```

Send a message with automatic retries until an ACK is received:
```cpp
lora.sendWithAck("0013", "Hello!");
```

Receive and parse incoming messages:
```cpp
if (Serial1.available()) {
  String raw = Serial1.readStringUntil('\n');
  String data = lora.parseMessage(raw);
}
```

## Releases

- **v1.0.0** - Initial release with full command interface, acknowledgment support, and basic setup routine.