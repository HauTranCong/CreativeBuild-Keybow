// Include library for key and mouse
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "includes/usb.h"
#include "includes/Adafruit_USBD_CDC-stub.h"
#include "Adafruit_TinyUSB_Arduino/src/Adafruit_TinyUSB.h"
#include "TinyUSB_Mouse_and_Keyboard/TinyUSB_Mouse_and_Keyboard.h"
// Inlude library for ws2812
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "includes/WS2812.pio.h"
#include "includes/ws2812.h"
// Debounce delay (ms)
#define DEBOUNCE_DELAY 100
// GPIO pin the keyswitch is on
#define PIN_LENGTH 4
#define KEY_MAP_ROWS 3
#define KEY_MAP_COLUMNS 4
// Pin mouse(encoder)
#define CLK 17
#define DT 16
#define SW 12
bool isInterp = false;
// Variables for detacting encoder change
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
static int effectState = 0;

const int switch_pin[PIN_LENGTH] = {18, 19, 20, 21};
uint8_t keyMap[KEY_MAP_ROWS][KEY_MAP_COLUMNS] = {
    {0x80, 0x80, 0x80, 0x80},
    {0x63, 0x76, 0x81, 0xB3},
    {0x00, 0x00, 0xB3, 0x00},
};

// Class to contain key
class KEY
{
private:
  // Attribute
  int _key;
  uint8_t _funcKey1;
  uint8_t _funcKey2;
  uint8_t _funcKey3;

public:
  // Contructor Funcion
  //  KEY(int, uint8_t, uint8_t);
  //  KEY(int, uint8_t, uint8_t, uint8_t);
  // Method: set information of key via publics funcion
  void set_key(int);
  void set_keyFunc1(uint8_t);
  void set_keyFunc2(uint8_t);
  void set_keyFunc3(uint8_t);
  // Method: when key press and release
  void key_press();
  void key_release();
  // Method: get information of key via publics funcion
  int get_key();
  uint8_t inline get_keyFunc1();
  uint8_t inline get_keyFunc2();
  uint8_t inline get_keyFunc3();
};

// Declaration Prototype, pass object key to funcion
void key_funcion(KEY, bool &, bool &, uint32_t, uint32_t &, int);
void set_keyfuncion(KEY *, const int, const int);
void mouse_encoder();
void core1_entry();
void sw_callback(uint gpio, uint32_t events);
void all_effect(ws2812 &object, int state, int dir);
static bool loopTask(repeating_timer_t *rt);

// Declaration ws2812 object
ws2812 backGroundLed(11, false, 0.1, 9, pio0, 0);
ws2812 buttonLed(25, false, 0.1, 4, pio0, 1, 255, 230, 230);

// Pointer to member funcion
typedef void (ws2812::*ledEffect)(uint);
#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))
ledEffect effect[] = {
    &ws2812::pattern_snakes,
    &ws2812::pattern_random,
    &ws2812::pattern_sparkle,
    &ws2812::pattern_greys};

// Adafruit TinyUSB instance
extern Adafruit_USBD_Device USBDevice;

int main()
{
  // bi_decl(bi_program_description("A single superkey keyboard"));
  // bi_decl(bi_program_feature("USB HID Device"));
  stdio_init_all();
  stdio_usb_init();

  USBDevice.begin(); // Initialise Adafruit TinyUSB

  // launch core 1 with core1_entry funcion callback
  multicore_launch_core1(core1_entry);

  // Timer for regularly processing USB events
  // struct repeating_timer timer;
  // add_repeating_timer_ms(10, loopTask, NULL, &timer);

  // Initialise a mouse
  Mouse.begin();
  gpio_set_dir(CLK, GPIO_IN); // CLK in input mode
  gpio_set_dir(DT, GPIO_IN);  // DT in input mode
  gpio_set_dir(SW, GPIO_IN);
  gpio_pull_up(SW);
  gpio_set_irq_enabled_with_callback(SW, 0x04, 1, sw_callback); // callback sw_callback when sw press

  // Initialise a keyboard (code will wait here to be plugged in)
  Keyboard.begin();
  // Initialise keymap and keyfuncion per switch
  KEY SWITCH[4];
  set_keyfuncion(SWITCH, KEY_MAP_COLUMNS, KEY_MAP_ROWS); // Read columns above to below
  // Initise GPIO pin as input with pull-up
  for (size_t i = 0; i < PIN_LENGTH; i++)
  {
    SWITCH[i].set_key(switch_pin[i]);
    gpio_init(SWITCH[i].get_key());
    gpio_set_dir(SWITCH[i].get_key(), GPIO_IN);
    gpio_pull_up(SWITCH[i].get_key());
  }

  // Read the initial state of CLK_encoder
  lastStateCLK = gpio_get(CLK);
  // Variables for detecting key press
  bool lastState = true; // pulled up by default
  uint32_t lastTime = to_ms_since_boot(get_absolute_time());

  buttonLed.setAll(255, 230, 230); // Set all led of buttonLed object with white color

  while (1)
  {
    int32_t now = to_ms_since_boot(get_absolute_time());
    for (size_t i = 0; i < PIN_LENGTH; i++)
    { // Scan 4 switch with for loop
      bool state = gpio_get(SWITCH[i].get_key());
      key_funcion(SWITCH[i], state, lastState, now, lastTime, i);
      mouse_encoder();
    }
  }
}

// Method::key
void KEY::set_key(int key)
{
  _key = key;
}
void KEY::set_keyFunc1(uint8_t funcKey1)
{
  _funcKey1 = funcKey1;
}
void KEY::set_keyFunc2(uint8_t funcKey2)
{
  _funcKey2 = funcKey2;
}
void KEY::set_keyFunc3(uint8_t funcKey3)
{
  _funcKey3 = funcKey3;
}

void KEY::key_press()
{
  Keyboard.press(_funcKey1);
  delay(10);
  Keyboard.press(_funcKey2);
  delay(10);
  Keyboard.press(_funcKey3);
  delay(10);
}
void KEY::key_release()
{
  Keyboard.releaseAll();
}

int inline KEY::get_key()
{
  return _key;
}
uint8_t inline KEY::get_keyFunc1()
{
  return _funcKey1;
}
uint8_t inline KEY::get_keyFunc2()
{
  return _funcKey2;
}
uint8_t inline KEY::get_keyFunc3()
{
  return _funcKey3;
}

void core1_entry()
{
  // Setup
  int dir = 1;

  while (1)
  {
    int t = 0;
    for (int i = 0; i < 1000; ++i)
    {
      all_effect(backGroundLed, effectState, t);
      // When sw pressed that led effect is change with effectState
      if (isInterp)
      {
        isInterp = false;
        break;
      }
      sleep_ms(10);
      t += dir;
    }
    dir *= -1;
  }
}

void key_funcion(KEY SWITCH, bool &state, bool &lastState, uint32_t now, uint32_t &lastTime, int whichKey)
{
  // Check GPIO pin, and if more than DEBOUNCE_DELAY ms have passed since
  // the key changed press release key depending on value (delay is for
  // debounce, ie to avoid rapid changes to switch value)
  if ((now - lastTime > DEBOUNCE_DELAY) && state != lastState)
  {
    if (state)
    {                       // The pin is pulled up by default, so the logic is backwards
      SWITCH.key_release(); // and true is released
    }
    else
    {
      SWITCH.key_press();
      buttonLed.setOnePixel(whichKey);
    }
    lastTime = now;
    lastState = state;
  }
}

void set_keyfuncion(KEY *SWITCH, const int colums, const int rows)
{
  for (size_t i = 0; i < KEY_MAP_COLUMNS; i++)
  {
    for (size_t j = 0; j < KEY_MAP_ROWS; j++)
    {
      if (j == 0)
        SWITCH[i].set_keyFunc1(keyMap[j][i]);
      if (j == 1)
        SWITCH[i].set_keyFunc2(keyMap[j][i]);
      if (j == 2)
        SWITCH[i].set_keyFunc3(keyMap[j][i]);
    }
  }
}

void mouse_encoder()
{
  // Read the current state of CLK
  currentStateCLK = gpio_get(CLK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
  {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (gpio_get(DT) != currentStateCLK)
    {
      // currentDir ="CCW";
      Mouse.move(0, 0, 1);
    }
    else
    {
      // Encoder is rotating CW so increment
      // currentDir ="CW";
      Mouse.move(0, 0, 255);
    }
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  // Read the button state
  int btnState = gpio_get(SW);

  // If we detect LOW signal, button is pressed
  if (btnState == false)
  {
    // if 50ms have passed since last LOW pulse, it means that the
    // button has been pressed, released and pressed again
    if (to_ms_since_boot(get_absolute_time()) - lastButtonPress > 50)
    {
      // Serial.println("Button pressed!");
      effectState += 1;
      if (effectState > 3)
      {
        effectState = 0;
      }
    }
    // Remember last button press event
    lastButtonPress = to_ms_since_boot(get_absolute_time());
  }
  // Put in a slight delay to help debounce the reading
  delay(1);
}

// This function is called by a timer to change the on-board LED to flash
// differently depending on USB state
// static bool loopTask(repeating_timer_t *rt){
//   led_blinking_task();
//   return true;
// }

void sw_callback(uint gpio, uint32_t events)
{
  isInterp = true;
}

void all_effect(ws2812 &object, int state, int dir)
{
  CALL_MEMBER_FN(object, effect[state])
  (dir);
}
