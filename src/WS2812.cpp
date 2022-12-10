#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "includes/WS2812.pio.h"
#include "includes/ws2812.h"

ws2812::ws2812(int PIN_TX, bool RGBW, float BRIGHTNESS, int LEDS, PIO pio, int sm)
{
  this->PIN_TX_ = PIN_TX;
  this->RGBW_ = RGBW;
  this->BRIGHTNESS_ = BRIGHTNESS;
  this->LEDS_ = LEDS;
  this->pio_ = pio;
  this->sm_ = sm;
  ws2812_init();
}

ws2812::ws2812(int PIN_TX, bool RGBW, float BRIGHTNESS, int LEDS, PIO pio, int sm, uint8_t r, uint8_t g, uint8_t b)
{
  this->PIN_TX_ = PIN_TX;
  this->RGBW_ = RGBW;
  this->BRIGHTNESS_ = BRIGHTNESS;
  this->LEDS_ = LEDS;
  this->pio_ = pio;
  this->sm_ = sm;
  ws2812_init();
}

void ws2812::ws2812_init()
{
  uint offset = pio_add_program(pio_, &ws2812_program);
  ws2812_program_init(pio_, sm_, offset, PIN_TX_, 800000, RGBW_);
}

void ws2812::put_pixel(uint32_t pixel_grb)
{
  pio_sm_put_blocking(pio_, sm_, pixel_grb << 8u);
}

uint32_t ws2812::urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
  // return 0x1000;
  // return 0x100000;
  return ((uint32_t)((uint8_t)(r * BRIGHTNESS_)) << 8) |
         ((uint32_t)((uint8_t)(g * BRIGHTNESS_)) << 16) |
         (uint32_t)((uint8_t)(b * BRIGHTNESS_));
}

uint32_t ws2812::urgb_u32(uint8_t r, uint8_t g, uint8_t b, float BRIGHTNESS)
{
  // return 0x1000;
  // return 0x100000;
  return ((uint32_t)((uint8_t)(r * BRIGHTNESS)) << 8) |
         ((uint32_t)((uint8_t)(g * BRIGHTNESS)) << 16) |
         (uint32_t)((uint8_t)(b * BRIGHTNESS));
}

void ws2812::setAll(uint8_t r, uint8_t g, uint8_t b)
{
  for (int i = 0; i < LEDS_; i++)
  {
    put_pixel(urgb_u32(r, g, b));
  }
}

void ws2812::setOnePixel(int led)
{
  switch (led)
  {
  case 0:
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(191, 0, 255, 0.2));
    break;
  case 1:
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(191, 0, 255, 0.2));
    put_pixel(urgb_u32(255, 230, 230));
    break;
  case 2:
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(191, 0, 255, 0.2));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    break;
  case 3:
    put_pixel(urgb_u32(191, 0, 255, 0.2));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    put_pixel(urgb_u32(255, 230, 230));
    break;
  }
}

void ws2812::blinky_led(int LEDS, uint8_t r, uint8_t g, uint8_t b, int delay_ms)
{
  for (int i = 0; i <= LEDS; i++)
  {
    put_pixel(urgb_u32(r, g, b));
  }
  sleep_ms(delay_ms);
  // Clear all pixels
  for (int i = 0; i <= LEDS; i++)
  {
    put_pixel(urgb_u32(0, 0, 0)); // Black or off
  }
  sleep_ms(delay_ms);
}

void ws2812::pattern_snakes(uint t)
{
  for (uint i = 0; i < LEDS_; ++i)
  {
    uint x = (i + (t >> 1)) % 64;
    if (x < 10)
      put_pixel(urgb_u32(0xff, 0, 0));
    else if (x >= 15 && x < 25)
      put_pixel(urgb_u32(0, 0xff, 0));
    else if (x >= 30 && x < 40)
      put_pixel(urgb_u32(0, 0, 0xff));
    else
      put_pixel(0);
  }
}

void ws2812::pattern_random(uint t)
{
  if (t % 8)
    return;
  for (int i = 0; i < LEDS_; ++i)
  {
    uint8_t a = rand();
    put_pixel(urgb_u32(rand(), rand(), rand()));
  }
}

void ws2812::pattern_sparkle(uint t)
{
  if (t % 8)
    return;
  for (int i = 0; i < LEDS_; ++i)
  {
    uint8_t a = rand() % 16 ? 0 : 0xff;
    put_pixel(urgb_u32(a, a, a));
  }
}

void ws2812::pattern_greys(uint t)
{
  int max = 100; // let's not draw too much current!
  t %= max;
  for (int i = 0; i < LEDS_; ++i)
  {
    uint8_t a = 2.575758 * t; // 0-255 if t max is 99
    put_pixel(urgb_u32(a, a, a));
    if (++t >= max)
      t = 0;
  }
}
