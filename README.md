# CreativeBuild-Keybow
## Usage
1. Have a working Raspberry Pi Pico C SDK setup.
2. Clone/download the repository. Don't forget to run git submodule update --init.
3. Ensure that there is a symbolic link to pico_sdk_import.cmake from the Pico C SDK. (If a symbolic link isn't an option, just copy the file.)
4. Create a build directory, cd to it, and run cmake ../. (Or your build system equivalents.)
5. Build by running make. (Or your build system equivalent.)
6. Reset (or plug in) the Pico holding down the BOOTSEL and copy CreativeBuild_Keybow.uf2 to the Pico's USB mass storage device.
7. You should have a keyboard device now. Shorting the GP18 -> GP21 pin to ground will trigger a keypress of keystrokes.
## Reference: https://github.com/guruthree/pico-superkey-board#usage
