/*
  LiquidCrystal Library - Autoscroll
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch demonstrates the use of the autoscroll()
 and noAutoscroll() functions to make new text scroll or not.
 
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/LiquidCrystalAutoscroll

 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int num_facts = 2;
char* lisp_facts[] = {
  "Lisp was initially used for control algorithms of Egyptian Pyramids, but it was later gave up as \"too ancient\".",
  "Initially Hamurabi codex predicted punishment of a week of Lisp coding for murder. Later it was decided it is too cruel, so death penalty was invented instead.",
  "Lol, Lisp"
};

void lisp_facts_setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
}

int ctr = 0;

void lisp_facts_loop() {
  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  // print from 0 to 9:
  lcd.print("Fact ");
  lcd.print(ctr+1);
  // set the cursor to (16,1):
  delay(1000);
  lcd.clear();
  // set the display to automatically scroll:
  // print from 0 to 9:
  char* lisp_fact = lisp_facts[ctr];
  int chr = 0;
  while(lisp_fact[++chr]);
  int max_chars = 14;
  int since_last_break = 0;
  for (int end_idx = 0; end_idx <chr; ++end_idx) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fact ");
    lcd.print(ctr+1);
    lcd.setCursor(0,1);
    for (int j=end_idx-max_chars; j<=end_idx; ++j) {
      lcd.print(( j<0 ? ' ' : lisp_fact[j]));
    }
    if (lisp_fact[end_idx] == ' ' || lisp_fact[end_idx] == '.') {
      delay(200+50*since_last_break);
      since_last_break = 0;
    } else {
      delay(100);
      ++since_last_break;
    }
  }
  delay(1000);
  ctr = (ctr+1)%num_facts;
  // turn off automatic scrolling
  lcd.noAutoscroll();
  
  // clear screen for the next loop:
  lcd.clear();
}
