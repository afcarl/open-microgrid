#ifndef ASYNC_LCD_UI_H
#define ASYNC_LCD_UI_H



#include <LiquidCrystal.h>
#include "linked_list.h"
#include "abstract_ui.h"
#include "one_wire_keypad.h"



class AsyncLcdUi : public AbstractUi {
    LiquidCrystal lcd;
    int ctr;

  public:
    AsyncLcdUi();
    void setup();
    void display(const __FlashStringHelper* string_index, ...);
    void step();
    int get_binary(const __FlashStringHelper* short_text, OneWireKeypad* keypad);
    int get_integer(const __FlashStringHelper* short_text, OneWireKeypad* keypad);
};

#endif
