# AGENTS.md

## Project context

This repository contains firmware test programs for the Infineon TLE9855 EvalKit.

The target hardware is:

* Infineon TLE9855 EvalKit
* TLE9855QX MCU
* Arm Cortex-M0-based embedded power MCU
* Onboard SEGGER J-Link debugger
* Debug/flashing through J-Link / J-Trace Cortex using SWD
* Development/build environment on the EVB Windows computer using Keil µVision5
* Peripheral configuration using Infineon MCU Config Wizard / Config Wizard for Embedded Power ICs

The current intended workflow is:

1. Develop/edit source code on Mac using Codex.
2. Commit and push changes to GitHub.
3. Pull changes on the EVB Windows computer.
4. Open the Keil `.uvprojx` project.
5. Build using Keil µVision’s Build/Rebuild button.
6. Flash using Keil’s Load/F8 through J-Link/SWD.
7. Press Reset or run/debug the board.

Do not assume that the Mac build system is currently authoritative. The authoritative build is currently Keil µVision on the EVB computer.

## Hardware/toolchain setup

The EVB computer has:

* Keil µVision5
* Infineon MCU Config Wizard / Config Wizard for Embedded Power ICs
* Infineon TLE985x SDK / Keil device pack
* SEGGER J-Link driver/tools
* TLE9855 EvalKit connected over USB/J-Link/SWD

Keil should be configured with:

* Device: `Infineon -> TLE98xx Series -> TLE985x Series -> TLE9855QX`
* Debugger: `J-Link / J-Trace Cortex`
* Interface: `SWD`
* SWD connection confirmed when an IDCODE is visible in J-Link settings

The Infineon example flow uses:

* `Device -> Startup`
* `Device -> ConfigWizard`
* SDK components such as `TIMER2x`
* Config Wizard settings for Timer2 and PORT
* Keil Build
* Keil Load/F8 to flash
* Reset button on the EvalKit to run the loaded firmware

## Repository workflow

The repository is shared between two machines:

### Mac / Codex computer

Use this computer for source code editing.

Typical workflow:

```bash
git pull
# edit code with Codex
git status
git add .
git commit -m "Describe change"
git push
```

Do not try to build the final MCU firmware on Mac unless a standalone Makefile/CMake build is explicitly added and verified.

### EVB / Keil computer

Use this computer for the real build, flash, and hardware test.

Typical workflow:

```bash
git pull
```

Then:

1. Open the Keil `.uvprojx` project.
2. Build or Rebuild in Keil.
3. Flash using Load/F8.
4. Press Reset or enter Debug mode and Run.
5. Test on the TLE9855 EvalKit.
6. If Keil or Config Wizard changed project/config files, commit and push those changes.

## Expected repository structure

Preferred structure:

```text
tle9855-firmware/
├── AGENTS.md
├── README.md
├── .gitignore
├── keil/
│   ├── TLE9855_test_programs.uvprojx
│   ├── TLE9855_test_programs.uvoptx
│   └── RTE/
├── src/
│   ├── main.c
│   ├── app_timer.c
│   ├── app_gpio.c
│   ├── app_adc.c
│   └── app_register_logger.c
├── include/
│   ├── app_timer.h
│   ├── app_gpio.h
│   ├── app_adc.h
│   └── app_register_logger.h
├── tools/
│   ├── capture_serial.py
│   └── flash_jlink.jlink
└── docs/
    └── notes.md
```

A simpler early-stage structure is also acceptable:

```text
tle9855-firmware/
├── README.md
├── .gitignore
├── TLE9855_test_programs.uvprojx
├── TLE9855_test_programs.uvoptx
├── RTE/
└── main.c
```

If the Keil project is moved into `keil/`, source files can still live outside it. Keil can reference them through relative paths such as:

```text
../src/main.c
../include/app_gpio.h
```

The Keil Project pane is not a normal folder browser. It only shows files included in the Keil project. To add files from `../src/`, right-click a source group and use “Add Existing Files to Group.”

## Git tracking rules

Track these files:

```text
*.uvprojx
*.uvoptx
RTE/
src/
include/
tools/
docs/
README.md
AGENTS.md
.gitignore
Config Wizard-generated source/config files required to rebuild the project
```

Do not track generated build outputs:

```text
Objects/
Listings/
*.o
*.d
*.crf
*.htm
*.lnp
*.map
*.lst
*.dep
*.iex
*.axf
*.elf
*.hex
*.bin
*.bak
*.scvd
*.dbgconf
*.uvguix.*
.DS_Store
Thumbs.db
.vscode/
```

Recommended `.gitignore`:

```gitignore
# Keil build outputs
Objects/
Listings/
*.o
*.d
*.crf
*.htm
*.lnp
*.map
*.lst
*.dep
*.iex
*.axf
*.elf
*.hex
*.bin

# Keil temporary/user-specific files
*.bak
*.uvguix.*
*.scvd
*.dbgconf

# OS/editor files
.DS_Store
Thumbs.db
.vscode/

# Python tools
__pycache__/
*.pyc
.venv/
```

## Keil project file behavior

Keil stores the project source list and build configuration in the `.uvprojx` file. If a file is added to or removed from Source Group 1, Git will usually show a diff in the `.uvprojx`.

Keil may also update `.uvoptx` for user/debug/project options.

The `*.uvguix.*` files are user-specific GUI/session files and should usually be ignored.

If unwanted Keil metadata changes appear, use:

```bash
git status
git diff
git restore keil/TLE9855_test_programs.uvprojx
git restore keil/TLE9855_test_programs.uvoptx
```

If the project files are at the repo root instead of inside `keil/`, adjust the paths accordingly:

```bash
git restore TLE9855_test_programs.uvprojx
git restore TLE9855_test_programs.uvoptx
```

To remove untracked generated files:

```bash
git clean -nd
git clean -fd
```

Only use `git clean -fd` after confirming the preview is safe.

## Source file organization

The intended pattern is:

* `main.c` contains the only `main()` function.
* Other `.c` files contain demo/app functions.
* Header files in `include/` declare those functions.
* Keil compiles every `.c` file included in the project into object files.
* The linker combines all object files into one firmware image.

Avoid multiple `main()` functions. Do not create separate demo files that each define `main()` unless only one of them is included in the Keil build at a time.

Preferred pattern:

```c
// main.c
#include "tle_device.h"
#include "app_timer.h"
#include "app_gpio.h"
#include "app_adc.h"
#include "app_register_logger.h"

#define DEMO_LED_BLINK        1
#define DEMO_GPIO_BUTTON      0
#define DEMO_ADC_TEST         0
#define DEMO_REGISTER_LOGGER  0

int main(void)
{
#if DEMO_LED_BLINK
    run_led_blink_demo();
#elif DEMO_GPIO_BUTTON
    run_gpio_button_demo();
#elif DEMO_ADC_TEST
    run_adc_test_demo();
#elif DEMO_REGISTER_LOGGER
    run_register_logger_demo();
#endif

    while (1)
    {
    }
}
```

Example module:

```c
// app_gpio.c
#include "tle_device.h"
#include "app_gpio.h"

void run_gpio_button_demo(void)
{
    while (1)
    {
        // GPIO demo logic
    }
}
```

Example header:

```c
// app_gpio.h
#ifndef APP_GPIO_H
#define APP_GPIO_H

void run_gpio_button_demo(void);

#endif
```

## Manage Run-Time Environment

Keil’s Manage Run-Time Environment selects which software components/drivers are included in the project.

Examples:

```text
Device -> Startup
Device -> ConfigWizard
SDK -> TIMER2x
SDK -> PORT
SDK -> ADC
SDK -> UART/LIN-related components
```

Selecting a peripheral here does not automatically make the hardware run. It only includes the relevant driver/support files and APIs in the project.

For example:

```text
Selecting SDK -> TIMER2x
= include Timer2x driver/API support
```

The peripheral still needs to be configured in Config Wizard and started/used in C code.

## MCU Config Wizard

Infineon MCU Config Wizard edits/generates the configuration files used by the firmware.

Config Wizard is used for settings such as:

```text
PORT pin mode
GPIO input/output
pull-up/pull-down
timer reload values
timer interrupts
callback names
ADC channels
UART/LIN settings
clock/system settings
watchdog/startup-related settings
```

For example, the basic Timer2 demo uses:

```text
Timer2 reload = 1000 us
Timer2 overflow interrupt = enabled
Timer2 callback = task_1ms
P0.2 = output
```

Then C code starts Timer2:

```c
TIMER2_Start();
```

and the generated interrupt code calls:

```c
void task_1ms(void)
{
    // user code
}
```

After changing Config Wizard settings on the EVB computer:

```bash
git status
git diff
```

Commit the generated config/project files if the change is intentional.

## Baseline firmware examples

### 1 ms Timer2 callback baseline

The basic idea:

```c
#include "tle_device.h"

volatile uint32_t g_ms_ticks = 0;
volatile uint8_t g_500ms_flag = 0;

void task_1ms(void)
{
    static uint16_t count = 0;

    g_ms_ticks++;
    count++;

    if (count >= 500)
    {
        count = 0;
        g_500ms_flag = 1;
    }
}

int main(void)
{
    TIMER2_Start();

    while (1)
    {
        if (g_500ms_flag)
        {
            g_500ms_flag = 0;
            PORT_ChangePin(PORT_0, PIN_2);
        }
    }
}
```

Conceptual flow:

```text
main()
-> TIMER2_Start()
-> Timer2 hardware counts
-> Timer2 overflows every 1 ms
-> Timer2 overflow interrupt fires
-> interrupt handler calls task_1ms()
-> task_1ms sets flags/counters
-> main loop handles slower app logic
```

Interrupt callbacks should stay short. Prefer setting flags/counters in the interrupt and doing heavier logic in the main loop.

### Other simple demos

Useful next programs:

```text
GPIO LED pattern
GPIO button input controlling LED
GPIO button debounce
ADC read into global variable
ADC value controls LED
UART/serial hello-world logging
Register snapshot variables
Register dump over UART or RTT
PWM duty-cycle sweep
Watchdog reset test
LIN transmit/receive test
```

Recommended learning order:

```text
1. GPIO LED output
2. GPIO button input
3. Timer-based scheduler
4. ADC read
5. UART or RTT logging
6. Register snapshots/logging
7. PWM output
8. Watchdog
9. LIN
```

## Runtime output/logging

The MCU cannot directly write a `.txt` file on the PC. It needs a communication path.

Options:

```text
Keil Watch window
J-Link RTT
UART serial output
PC script reading registers through J-Link/SWD
Semihosting/debug printf
```

Recommended order:

```text
1. Watch variables/registers in Keil
2. J-Link RTT for debug-style logs through J-Link
3. UART serial logging for normal terminal/text-file output
4. PC-side J-Link scripts for direct register reads
```

For text files, the PC captures the data:

```text
MCU sends data over UART/RTT
-> PC terminal/script receives it
-> PC writes log.txt or log.csv
```

Example PC-side serial capture idea:

```python
import serial

ser = serial.Serial("COM5", 115200, timeout=1)

with open("mcu_log.txt", "w") as f:
    while True:
        line = ser.readline().decode(errors="ignore")
        if line:
            print(line, end="")
            f.write(line)
            f.flush()
```

## Build behavior

Keil compiles each included `.c` file into an object file, then links all object files into one firmware image.

Example:

```text
main.c              -> main.o
app_gpio.c          -> app_gpio.o
app_timer.c         -> app_timer.o
app_register_logger.c -> app_register_logger.o

all .o files + startup + SDK drivers + config objects
-> final firmware image
```

Keil outputs are usually placed in:

```text
Objects/
Listings/
```

Typical final outputs:

```text
*.axf
*.hex
*.map
```

Do not manually configure output folders unless necessary. Do not commit build outputs.

## Build and flash steps on EVB computer

1. Pull latest code:

```bash
git pull
```

2. Open the Keil `.uvprojx`.

3. Check device target:

```text
TLE9855QX
```

4. Check Manage Run-Time Environment for required components.

5. Open Config Wizard if peripheral settings need changes:

```text
Tools -> Config Wizard V2
```

6. Save Config Wizard changes.

7. Build:

```text
Build Target / F7
```

or use Rebuild for a clean build.

8. Connect EvalKit:

```text
USB/J-Link
VBAT/GND power if required by setup
```

9. Check debugger:

```text
Options for Target -> Debug
Use: J-Link / J-Trace Cortex
Interface: SWD
IDCODE visible
```

10. Flash:

```text
Load / F8
```

11. Reset/run the board.

12. If project/config files changed intentionally:

```bash
git status
git add .
git commit -m "Update firmware/config"
git push
```

## Mac standalone build status

A Mac standalone build is not the primary workflow right now.

To build on Mac, this repo would need to include or reference:

```text
ARM Cortex-M compiler
startup/vector table file
GCC linker script
Infineon SDK source and headers
CMSIS/device headers
Config Wizard-generated files
correct include paths
correct compiler macros
correct Cortex-M0 flags
Makefile or CMakeLists.txt
```

This is possible, but not the current authoritative flow.

Current rule:

```text
Mac edits code.
EVB computer builds and flashes using Keil.
```

Only add Mac Makefile/CMake build support after the Keil-managed build is stable and the required startup/linker/SDK files are identified.

## Agent instructions

When modifying this repo:

1. Prefer small, clear changes.
2. Do not remove Keil project files.
3. Do not commit build outputs.
4. Do not add a second `main()` unless specifically requested.
5. Keep hardware-specific code readable and beginner-friendly.
6. Use module files under `src/` and headers under `include/`.
7. If adding a new `.c` file, mention that it must also be added to the Keil project Source Group unless the Keil project is also updated.
8. If moving files, preserve relative paths expected by Keil or update `.uvprojx`.
9. Avoid editing generated Config Wizard files manually unless the user explicitly requests it.
10. If a Config Wizard change is needed, tell the user to make it on the EVB computer through Config Wizard, then commit the changed generated files.
11. Assume the final hardware test happens on the TLE9855 EvalKit, not on Mac.
12. Do not assume terminal output exists unless UART, RTT, semihosting, or a J-Link script has been set up.
13. Keep explanations direct and concise.
14. When providing C code, use simple embedded C and avoid unnecessary abstraction.
15. Be careful with interrupts: keep interrupt callbacks short and use volatile flags for communication with `main()`.

## Current mental model

```text
GitHub repo
-> shared source/project/config state

Mac
-> edit code with Codex
-> push commits

EVB Windows computer
-> pull commits
-> Keil µVision builds firmware
-> J-Link/SWD flashes firmware
-> TLE9855 EvalKit runs firmware

MCU Config Wizard
-> edits generated peripheral configuration files

Manage Run-Time Environment
-> selects which Keil/SDK software components are included

main.c
-> selects/runs the active demo/application

src/*.c
-> implementation modules

include/*.h
-> module interfaces
```
