# Robot Library

This collection of C files provides headers an utility routines for the [Pololu 3pi+ 2040 robot](https://www.pololu.com/docs/0J86).  These are mostly drawn from the [C library](https://github.com/pololu/pololu-3pi-2040-robot/tree/master/c) in the [pololu-3pi-2040-robot](https://github.com/pololu/pololu-3pi-2040-robot) repository by [DavidEGrayson](https://github.com/DavidEGrayson).  Some code is also borrowed from the [pololu-3pi-plus-2040-arduino-library](https://github.com/adamgreen/pololu-3pi-plus-2040-arduino-library) by [adamgreen](https://github.com/adamgreen).

These files are used in Lingua Franca examples provided in the src directory of this
repository.  To use these library routines in your Lingua Franca program, it is necessary to include the following directives in your target declaration:

```
target C {
  platform: "RP2040", 
  threading: false,
  files: ["../../robot-lib/"],                      // Needed to find imu.h
  cmake-include: ["../../robot-lib/robot-lib.txt"]  // Needed to find imu.h
}
```

The `../../` may need to be adjusted depending on where your `.lf` file is relative to the `robot-lib` directory.
