# Template for the Lingua Franca RP2040 target platform
This repo is a template for [Lingua Franca](https://www.lf-lang.org/) projects using the bare metal RP2040 target platform such as found on the Raspberry Pi Pico board and the [Pololu 3pi+ 2040 robot](https://www.pololu.com/docs/0J86). Currently the repo supports MacOS, Linux, and Windows through [WSL](https://learn.microsoft.com/en-us/windows/wsl/install).
To support RP2040-based boards, the repo uses the [Pico SDK](https://github.com/raspberrypi/pico-sdk/tree/master/src) as a dependency which includes a light set of headers, libraries and a build system.

## Setup
This template uses nix to manage toolchains and other applications. Install [nix](https://nixos.org/download.html) first for your preferred platform. Make note of the installation type since a **multi-user** install will require sudo permissions and will interact with a system-wide `/nix/store`. After installation, run the following in the shell to enable the experimental nix flakes feature.

``` bash
mkdir -p ~/.config/nix
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```

To launch the lf-pico shell environment, run the following in the root of the lf-pico repository. The launched shell will include the various required toolchains and applications needed for development.

```bash
nix develop
```

## Building
Lingua Franca applications are code generated into the target language. To both code generate and build application binaries one can either use lfc or lingo. Lingo ultimately uses lfc as a backend but provides an additional experimental interface for managing multiple application binaries builds.

### LFC
LFC is the main lingua franca compiler used for code generation. Install a nightly version of the [tool chain](https://github.com/lf-lang/lingua-franca/releases/tag/nightly) since RP2040 support is not in an official release.

After installation, run ``lfc`` in the root directory on the application of your choice.
``` bash
lfc src/Blink.lf
```
An application binary will be populated in the ``/bin`` directory with the same name as the source file. The source code for the application after code generation will be in the ``/src-gen`` folder.

## Flashing
Before flashing the binary to your rp2040 based board, the board must be placed into ``BOOTSEL`` mode. On a [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) this can be entered by holding the ``RESET`` button while connecting the board to the host device.

The [picotool](https://github.com/raspberrypi/picotool) application installed in the nix shell is an easy way to interact with boards.
With the application you can check what programs are currently flashed and can directly load program binaries. Run ``picotool help`` for more information on its capabilities.


Run the following to flash an application binary on to your board.
``` shell
picotool load -x bin/Blink.elf
```

## Serial
Standard IO output can be redirected to be hosted across either a usb connection or pins connected to a uart peripheral. This option is specified by modifying the `platform` target option. Use the board field to specify the `<board_name>:"uart" "usb"`, where `uart` will redirect the stdio signal to the hardware pins for the particular board. Board options can be found [here](https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards)

```
target C {
    platform: {
        name: "RP2040",
        board: "pico:uart"
    },
    ...
}
```

Once flashed, open a minicom session using the following command and replace the COM string with the platform specific COM Port string. The following is a MacOS example.

```shell
minicom -b 115200 -o -D /dev/cu.usbmodem1234
```

Exit minicom with `CTRL-A` and `X`

## Debugging
To debug applications for RP2040 based boards, there exists a nice tool called [picoprobe](https://github.com/raspberrypi/picoprobe). This applications allows a Raspberry Pi Pico or RP2040 board to act as a [cmsis-dap](https://arm-software.github.io/CMSIS_5/DAP/html/index.html) device which interacts with the target device cortex cores through a [serial wire debug](https://wiki.segger.com/SWD)(swd) pin interface connection.

To get started, you'll need a secondary RP2040 based board and will need to flash the picoprobe [binaries](https://github.com/raspberrypi/picoprobe/releases/tag/picoprobe-cmsis-v1.02)linked here. The page contains pre-built firmware packages that can be flashed using picotool or copied to a `bootsel` mounted board. 

### Wiring
Once the **probe** device is prepared, wire it up to the **target** device as follows. The following is an example of a pico to pico connection and the pin numbers will differ from board to board.

```
Probe GND -> Target GND
Probe GP2 -> Target SWCLK
Probe GP3 -> Target SWDIO
Probe GP4 (UART1 TX) -> Target GP1 (UART0 RX)
Probe GP5 (UART1 RX) -> Target GP0 (UART0 TX)
```

*UART0* is the default uart peripheral used for stdio when uart is enabled for stdio in cmake. The target board uart is passed through the probe and can be accessed as usual using a serial port communication program on the host device connected to the probe.

### OpenOCD
[Open On-Chip Debugger](https://openocd.org/) is a program that runs on host machines called a `debug translator` It understands the swd protocol and is able to communicate with the probe device while exposing a local debug server for `GDB` to attach to.

After wiring, run the following command to flash a test binary of your choice
```
openocd -f interface/cmsis-dap.cfg -c "adapter speed 5000" -f target/rp2040.cfg -c "program bin/HelloPico.elf verify reset exit"
```
The above will specify the 
- probe type: `cmsis-dap`
- the target type: `rp2040`
- commands: the `-c` flag will directly run open ocd commands used to configure the flash operation. 
	- `adapter speed 5000` makes the transaction faster
	- `program <binary>.elf` specifies the `elf` binary to load into flash memory. These binaries specify the areas of where different parts of the program are loaded and the sizes.
	- `verify` reads the flash and checks against the binary
	- `reset` places the mcu in a clean initial state
	- `exit` disconnects openocd and the program will start to run on the board

### GDB
The gnu debugger is an open source program for stepping through application code. Here we use the remote target feature to connect to the exposed debug server provided by openocd. 

Make sure the intended program to be debugged on the **target** device has an accessible `.elf` binary that was built using the `Debug` option. To specify this property in an LF program, add the following to the program header:

```lf
target C {
    platform: {
        name: "RP2040",
        board: ...
    },
    build-type: "Debug"
    ...
}
```

First start openocd using the following command

```bash
openocd -f interface/cmsis-dap.cfg -c "adapter speed 5000" -f target/rp2040.cfg -s tcl
```

In a separate terminal window, run the following GDB session providing the elf binary. Since this binary was built using the `Debug` directive, it will include a symbol table that will be used for setting up breakpoints in gdb.

```bash
gdb <binary>.elf
```
Once the GDB environment is opened, connect to the debug server using the following. Each core exposes its own port but `core0` which runs the main thread exposes `3333`.

```bash
(gdb) target extended-remote localhost:3333
```

From this point onwards normal gdb functionality such as breakpoints, stack traces and register analysis can be accessed through various gdb commands.

## Emulator
To run basic smoke tests and monitor GPIO, UART and other supported peripherals, a nodejs based emulator is provided in this repo. During the nix shell setup for the repo, the node modules in the ``/test`` directory are installed. 

These are the currently supported peripherals the emulator has integration with. More information can be found [here](https://docs.wokwi.com/parts/wokwi-pi-pico)
By default, the emulator uses **hex** binaries which are generated by both build options. Any hex files that are in need of testing must be placed in the ``/target/hex/`` directory. This will automatically be done by *lingo*.

Run the following from the ``/test/`` directory. It will run an emulator instance for each hex file in the ``hex`` directory in parallel and report results as plain text files.
``` bash
cd test/
npm start
```
The text framework source code is available and can be easily extended. Currently, a test is set to report a **FAILING** status if it does not terminate within 10 seconds but this condition can be altered.

