# CAN bus analyzer

This is a very simple terminal app for reversing CAN bus traffic.
It displays all received CAN frames and overwrites previous entries when the same identifier appears again.
You probably should use [SavvyCAN](https://github.com/collin80/SavvyCAN) instead - an open-source, feature-rich GUI application.

## Communication protocol

I use simple, custom protocol. Every message consists of:

1. Magic bytes `AB CD` indicating start of the frame.
2. 4 bytes of identifier. Although standard IDs are 11 bits long, some systems (like my Volvo) use 29-bit IDs, so 2 bytes are not sufficient.
3. Flags byte: the LSB indicates the RTR bit; the second LSB indicates whether the ID is a 29-bit extended ID (1) or a standard 11-bit ID (0).
4. DLC byte.
5. Payload consisting of DLC bytes.

## CAN bus sniffer

This app works by talking to another device that translates CAN bus to UART (or different transport method, but currently only UART is implemented).
For testing, you can use the `MockTransporter` which simulates can by sampling random ids and messages, but
if you want to listen to CAN, you can use my [CAN sniffer](https://github.com/JakubZojdzik/can-sniffer) which is written for ESP32 connected
to a CAN bus transceiver, I use [SN65HVD230](https://botland.com.pl/magistrala-can/6960-modul-z-interfejsem-can-sn65hvd230-waveshare-3945-5904422300715.html).


## Building

To build to project, run:

```sh
cmake -B build && cmake --build build
```

It will generate two binary files: `./build/can_analyzer` and `./build/can_sender`

## Can analyzer

It uses ncurses to draw the interface. Use following to interact:

```
$ can_analyzer --help
CAN Analyzer
Nawigation:
  UP    - k, up arrow
  DOWN  - j, down arrow
  BEGIN - g
  END   - G
Other:
  clear - c
  copy  - y
  exit  - q
```

## Can sender

There is separate binary compiled for sending CAN frames to UART. It uses exactly the same protocol.

```
can_sender --help
Usage: ./build/can_sender <device> <id> <data_hex>
  device   - UART device path
  id       - frame id in hex
  data_hex - payload in hex
```
