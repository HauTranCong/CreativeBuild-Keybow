#ifndef WS2812_H
#define WS2812_H

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

class ws2812
{
private:
    int PIN_TX_;
    bool RGBW_;
    float BRIGHTNESS_;
    int LEDS_;
    PIO pio_;
    int sm_;
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;

public:
    ws2812(int PIN_TX, bool RGBW, float BRIGHTNESS, int LEDS, PIO pio, int sm);
    ws2812(int PIN_TX, bool RGBW, float BRIGHTNESS, int LEDS, PIO pio, int sm, uint8_t r, uint8_t g, uint8_t b);

    void ws2812_init();
    inline void put_pixel(uint32_t pixel_grb);
    inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
    inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b, float BRIGHTNESS);
    void setAll(uint8_t r, uint8_t g, uint8_t b);
    void setOnePixel(int pixel);

    void blinky_led(int len, uint8_t r, uint8_t g, uint8_t b, int delay_ms);
    void pattern_snakes(uint t);
    void pattern_random(uint t);
    void pattern_sparkle(uint t);
    void pattern_greys(uint t);
    void pattern_blink(uint t);
};

#endif
