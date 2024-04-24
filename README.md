# Oscilloscope using a Raspberry Pi Pico User manual

If you want to build this project yourself, this file details all the steps.

# Equipment

A Raspberry Pi Pico:
<https://www.raspberrypi.com/products/raspberry-pi-pico/>

The Waveshare Pico LCD 2: <https://www.waveshare.com/wiki/Pico-LCD-2>

2 10K potentiometers:
<https://coolcomponents.co.uk/products/rotary-potentiometer-10k-ohm-linear>

Input jack :
<https://thepihut.com/products/vertical-breadboard-friendly-3-5mm-mono-headphone-jack>

Breadboard(s):
<https://thepihut.com/products/raspberry-pi-breadboard-half-size>

Dupont wires:
<https://thepihut.com/products/thepihuts-jumper-bumper-pack-120pcs-dupont-wire>

# Environment set-up

This method <span style="color: red">only works on Windows</span>. This
project is written in C, which needs a toolchain, including a compiler.
Before starting make sure [Visual Studio
Code](https://code.visualstudio.com/download) is installed on your
machine. This link
<https://www.raspberrypi.com/news/raspberry-pi-pico-windows-installer/>
will take you to the Raspberry website, where you can download the
installer. When you run the .exe file, it will download all necessary
resources and set up a Visual Code environment. It will contain all the
libraries to exploit the Pico’s functionalities - like the ADC, or the
interrupts.

The next thing is to download the Waveshare folder
<https://files.waveshare.com/upload/2/28/Pico_code.7z>. You will need
[7-zip](https://www.7-zip.org/download.html) to decompress it. This is
the folder from which you will work. First set `PICO_SDK_PATH` as an
environment variable, and set it to where your SDK folder is; usually,
the directory link is C:/Program File/Raspberry Pi/Pico SDK
v1.5.1/pico-sdk. Then copy the `pico_sdk_import.cmake` file from the
`pico-examples` folder in the c subfolder of the Waveshare folder.

# Code and compilation

From GitHub download the `main.c` file
<https://github.com/toniozi/raspberry-pi-pico-oscilloscope>, whose
contents you can copy in your own `main.c` file. You also need to copy
the contents of `CMakeLists.txt` in your own file. In your Windows start
menu, open ’Pico - Visual Code’, this will open the `pico-examples`
folder. Now open the folder in your explorer, and copy the `.vscode`
folder. If it doesn’t appear, it could mean you don’t have hidden files
enabled, thus turn that option on. Paste the folder in your `c`
subfolder from the Waveshare folder. From VSCode, open the c subfolder
from Waveshare. A dropdown menu should appear to allow you to choose
your compiler; choose `Pico ARM GCC`.

To compile the code on Visual Code, press the `CMake` icon in the
’Extensions’ bar, after that to compile you can press the ’Build’ button
next to the line that should say `main` or `main.elf`. That’s your code
compiled and built. Go to the `build` subfolder of the Waveshare folder
and locate the `main.uf2` file. To send the file to the Raspberry Pi
Pico, press on the `BOOTSEL` button on the Pico **while** inserting the
USB cable into it. It will appear on your computer as a hard drive, in
which you can drag the `main.uf2` file. As soon as it is transferred,
the Pico will start running your code. Well done!

# Physical connections

On one bread board put the Pico and two potentiometers. Using this
[diagram](https://www.waveshare.com/w/A6Y79bcq/Kdy80nYY.php?f=Pico-LCD-2-details-inter.jpg&width=800), connect all the 19 appropriate
ports from the RPP to the screen using the Dupont wires. Connect the
input jack ’hot’ pin (small and not close to the long pin) to the ADC 0
port. Connect the first and second potentiometers’ middle pins to -
respectively - the ADC 1 and 2 ports. The left pins can be ’joined’ and
connected to the `3V3(OUT)` port. The ground from the 2 potentiometers
and input jack (long pin) can be joined and connected to any ground pin
on the Pico.
