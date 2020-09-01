# Log System

## Description

Logging is very important for developing embedded products. This is especially true for connection-based wireless products because the use of breakpoints will probably result in dropped connections, whereas issues can be easily addressed by walking through the log.

This example introduces a simple implementation of logging on EFR32 based devices. The key point of the example code is to introduce a way to classify the log into different levels, where each level of log has its own identifier. Let’s start with 2 design sketches, which demonstrate outputting the log via RTT and VCOM(UART).

## Features

### Logging Interface

The example code supports both [SEGGER's Real Time Transfer (RTT)](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/) and Serial Terminal via UART (VCOM).

### Fields in the Logging

By default, each piece of logging contains below information.

- Time - it's the relative time from device boots, if it's fed by the wall clock information, then the time will become the absolute time.
- Location - the file and line information where the logging located will be recorded into the logging.
- Level - it supports 7 levels logging, identified by color. See below picture and table.


The images above show different levels of logging in different colors. This example introduces 5 logging levels, as shown below.

| Type                   | color  |
| ---------------------- | ------ |
| Error (highest level)  | Red    |
| Warning                | Yellow |
| Information            | Blue   |
| Debug                  | Green  |
| Verbose (lowest level) | White  |

## Setting up

To be able to test the logger module do as follows:

1. Create a new _SoC-Empty_ example project in Simplicity Studio.

2. Copy the attached _app.c_, _log.c_, _log.h_ files into your project (overwriting existing _app.c_).

3. `LOG_PORT`, defined in _log.h_, can be set to **SEGGER_JLINK_VIEWER** or **PORT_VCOM** to determine if you want to send debug messages via DEBUG port or COM port (UART).

   - **SEGGER_JLINK_VIEWER** – the log will be out from J-Link RTT.
   - **PORT_VCOM**
     - If `HAL_VCOM_ENABLE` is enabled in _hal-config.h_, the log will be out from VCOM via USB. This is the default configuration in a _SOC - Empty_ project.
     - If `HAL_VCOM_ENABLE` is disabled, the log will be out from the UART TX pin directly which can be accessed on the expansion header of the WSTK

4. If you use the COM port (UART) for logging, copy _VCOM_RTT.h_ into your project. If you use SEGGER_JLINK_VIEWER, copy \*SEGGER_RTT\*.\*\* files into your project.

5. Define `LOG_LEVEL` in _log.h_. The definition of `LOG_LEVEL` determines which level of logging should be output to the terminal. As you can see from the table 1, error has the highest level while verbose has the lowest level. For example, If the `LOG_LEVEL` is defined as information level, then error, warning and information log will be sent to terminal, the debug and verbose log which have lower level than information will not be sent to the terminal. See figure below, which shows the log as information level without modifying anything from the first figure, using RTT.
   ![Log with Information Level](images/log_info_lvl.png)

6. Build and flash the project to your device

## Usage

1. On your PC open a terminal program and connect to the chosen port (e.g. you can use TeraTerm to connect via VCOM and RTT Viewer to connect via DEBUG port).

2. Reset your device and observe the log messages.

3. You may also open a Bluetooth connection to see more logs.

You can add new logs to your project. The following are 5 corresponding functions to send the log. The input parameter of these five functions is the same as standard printf();.

- `LOGE()` – Send ERROR log.
- `LOGW()` – Send WARNING log.
- `LOGI()` – Send INFORMATION log.
- `LOGD()` – Send DEBUG log.
- `LOGV()` – Send VERBOSE log.

Use these functions in your code to print different level logs.

## Source

- [app.c](source/app.c)
- [log.c](source/log.c)
- [log.h](source/log.h)
- [VCOM_RTT.h](source/VCOM_RTT.h)
- [SEGGER_RTT.c](source/SEGGER_RTT.c)
- [SEGGER_RTT.h](source/SEGGER_RTT.h)
- [SEGGER_RTT_Conf.h](source/SEGGER_RTT_Conf.h)
- [SEGGER_RTT_printf.c](source/SEGGER_RTT_printf.c)
