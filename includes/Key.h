#ifndef KEY_H
#define KEY_H

#include <stdio.h> // for size_t

class key
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

#endif