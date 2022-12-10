#Điều kiện về version
cmake_minimum_required(VERSION 3.17)

#Thêm SDK của raspberry pico
include(pico_sdk_import.cmake)

#Tên project và các ngôn ngữ được sử dụng trong project C, C++, Asm
project(CreativeBuild_Keybow C CXX ASM)
#Set các chuẩn của C, C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

#Định nghĩa sự liên quan giữa file thực thi và các file nguồn
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Print.cpp
    src/WS2812.cpp
    TinyUSB_Mouse_and_Keyboard/TinyUSB_Mouse_and_Keyboard.cpp
    Adafruit_TinyUSB_Arduino/src/arduino/hid/Adafruit_USBD_HID.cpp
    Adafruit_TinyUSB_Arduino/src/arduino/Adafruit_USBD_Device.cpp
    Adafruit_TinyUSB_Arduino/src/arduino/ports/rp2040/Adafruit_TinyUSB_rp2040.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_gpio
    pico_unique_id
    tinyusb_device
    tinyusb_board
)
pico_add_extra_outputs(${PROJECT_NAME})

add_definitions(-DUSE_TINYUSB)
add_definitions(-DFAMILY=rp2040)
add_definitions(-DUSB_PRODUCT="CreativeBuild.co")
add_definitions(-DARDUINO_ARCH_RP2040)
add_definitions(-DCFG_TUSB_CONFIG_FILE="includes/tusb_config.h")

pico_enable_stdio_usb(${PROJECT_NAME} ENABLED)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

include_directories(includes
    TinyUSB_Mouse_and_Keyboard
    Adafruit_TinyUSB_Arduino/src
)

# remove Adafruit tinyusb library code so that only the pico-sdk tinyusb code is used
# to restore, cd Adafruit_TinyUSB_Arduino/ && git checkout HEAD .
file(REMOVE Adafruit_TinyUSB_Arduino/src/tusb.c)
file(REMOVE Adafruit_TinyUSB_Arduino/src/tusb.h)
file(REMOVE Adafruit_TinyUSB_Arduino/src/tusb_option.h)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/osal/)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/portable/)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/device/)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/common/)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/class/)
file(REMOVE_RECURSE Adafruit_TinyUSB_Arduino/src/host/)