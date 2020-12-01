# libthermal

Library for thermal printers like the **QR701** or **Adafruit Mini Thermal Receipt Printer**.

A portion of this code was heavily inspired by
[mbedded.ninja's blog post](https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/)
about **Linux Serial Ports Using C/C++** and the
[Adafruit Thermal Printer Library](https://github.com/adafruit/Adafruit-Thermal-Printer-Library) for Arduino.

As this is my first real project
developed mainly in C, I am very much happy for feedback and help.

## Build

A Makefile is provided to compile the source code. The executable provides
an easy CLI for printing strings.

```bash
make
```

## Usage

You can use the CLI to print single strings:
```bash
./bin/thermal /dev/ttyS0 "Hello World!"
```
Currently, this only works with **QR701** printers (Baud set to `9600`).

### Emulating a Serial Port

```bash
socat PTY,link=./serial-port,raw,echo=1 -
```

## License

This project is licensed under the **BSD 3-Clause "New" or "Revised" License**:

    Copyright (c) 2020 Jonas Drotleff. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
