# Template for the Lingua Franca RP2040 target platform
This repo is a template for [Lingua Franca](https://www.lf-lang.org/) (LF) projects using the bare metal RP2040 target platform such as found on the [Raspberry Pi Pico board](https://www.raspberrypi.com/products/raspberry-pi-pico/) and the [Pololu 3pi+ 2040 robot](https://www.pololu.com/docs/0J86).
This template is particularly well suited for the [LF Embedded Lab](https://www.lf-lang.org/lf-embedded-lab/) exercises.
Students should create a repository using this template and record their work within their repo.

The repo supports MacOS, Linux, and Windows through [WSL](https://learn.microsoft.com/en-us/windows/wsl/install).
To support RP2040-based boards, the repo uses the [Pico SDK](https://github.com/raspberrypi/pico-sdk/tree/master) as a submodule.
It also includes some code from the [pololu-3pi-2040-robot library](https://github.com/pololu/pololu-3pi-2040-robot/tree/master) by DavidEGrayson.

See the [getting started instructions](https://www.lf-lang.org/embedded-lab/GettingStarted.html) to get started.

## Getting started
- Make sure that REACTOR_UC_PATH points to the reactor-uc runtime.

1. Compile all example programs
```console
make test
```

2. Flash the blinky example to a connected Polulu robot
```console
run/flash.sh -m src/Blink.lf
```



