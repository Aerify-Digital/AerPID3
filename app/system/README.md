# AerTiny Heartbeat Monitor - Failsafe System

Provides a heartbeat signal via ping-pong with ESP32 host controller.

## Detail

- Controls the element ENABLE line
  - Toggles allowance of element usage `on/off`
- Contains Attiny88 MCU for control CPU
  - 8 Mhz CPU, 8 kilobyte Storage, 512 bytes RAM
- Uses i2c Bus for communication
  - i2c Device Address: `0x2D`
- Accepts limited commands over i2c
  - Uses i2c Receive/Request events
- Provides debug toggle mode for host
  - _this is for programming the ESP32_
- Heartbeat State machine for ping-pong
  - Requires heartbeat signal over bus
  - Changes error state based on result from host
  - Uses basic command set for states

### Chipset Info

- [Attiny88](https://www.microchip.com/en-us/product/attiny88) - Datasheet
- [Arduino](https://www.arduino.cc/) - Application

---

## Setup and Install

The `Attiny88` chip on the `AerPID-Lite` board, should most likely already be flashed and programmed.
If it is not, please refer to the _Fuses_ section below.

### **Flashing** - Program Code

1. Compling of the source code requires usage of Arduino IDE
2. Compile the `ino` file to a `hex` blob for manual upload
3. Flashing of the Attiny88 requires an ISP programmer such as the [USPasp](https://www.fischl.de/usbasp/)
4. Connection to the Attiny88 is done using 6 pins located near the chip
   - An additional interface assembly board may be required to create a proper connection
5. Using [avrdude](https://github.com/avrdudes/avrdude), upload the `hex` blob to the chip
   - **Prog Command**: `avrdude -c usbasp -p t88 -U flash:w:aertiny_failsafe.ino.hex:i`

### **Flashing** - Chip Fuses

- When [new] chip Fuses are not set, the MCU will not respond at the normal buad rate
  - **_WARNING!!!_** ⚠ ☢
    - Improper usage of, or incorrect **fuse** values, _**will**_ brick your _chip_ and place it in a _**non-recoverable**_ state!
    - Recovery from _invalid_ fuse values is sometimes impossible. **Please proceed at own risk!**
  - Set the fuses for the clock divider and toggle the watchdog using [avrdude](https://github.com/avrdudes/avrdude):
    - **Fuse Command**: `avrdude -c usbasp -p t88 -B 5 -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m`
    - This will put the chip into 8mhz clock mode and enable the watchdog at 2.7 volts
  - You may use this [fuse calculator](https://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny88) to calculate fuses for additional desired setup

---

## LED Indicator Pins

These pins on the _Attiny88_ are each attached to a LED, and will show the status of the state machine result.

```cpp
// RGB Status Indicator
#define LED_A 17 // RED
#define LED_B 18 // GREEN
#define LED_C 19 // BLUE

// Error Indicator
#define LED_D 1 // RED
```

---

## Heartbeat Command Set

This enum holds the mapping of each state type to the corresponding byte value for the state.

```cpp
// Element Protect Op Codes enum types
typedef enum ELM_PROT_OP_CODE : uint8_t
{
  E_PROT_NONE = 0, // None state
  E_PROT_INIT = 1, // Initialization
  E_PROT_NEXT = 2, // Next state (goto)
  E_PROT_WAIT = 3, // Wait a moment
  E_PROT_OKAY = 4, // Result Okay
  E_PROT_RESET = 5, // Reset state trigger
  E_PROT_ERROR = 6, // Error active
  E_PROT_NACK = 7, // Back response
  E_PROT_ACK = 8, // Good resposne
  E_PROT_ERROR_NONE = 9, // None error case
  E_PROT_INIT_READY = 10, // Init is ready
  E_PROT_ERROR_NULL = 255, // Null error case
} _elm_prot_op_code_t;
```
