/*
Flie: knappsats.ino
author: Joel Oscarsson
Date: 2023-11-29
Description: This program includes a screen and a keypad used to create a code  lock.
The user is able to change the code.
 */

#include <Keypad.h>
#include "U8glib.h"
#include <Wire.h>

// OLED display configuration
U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

// Password-related constants and variables
const int Password_Length = 4;
String Data;
String Master = "1212";
int lockOutput = 11;
byte data_count = 0;
char customKey;

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Lock-related variables and constants
unsigned long lockTime = 0;
const unsigned long lockDuration = 5000;
bool isLocked = true;

void setup() {
  Serial.begin(9600);
  pinMode(lockOutput, OUTPUT);
  digitalWrite(lockOutput, LOW);
  updateOLED("Locked");
  Wire.begin();
  oled.setFont(u8g_font_profont12);
  oled.begin();
}

// Main program loop
void loop() {
  customKey = customKeypad.getKey();

  if (customKey) {
    Data += customKey;
    data_count++;
    updateOLEDWithVisualFeedback(Data);
  }

  if (data_count == Password_Length) {
    if (Data == Master) {
      if (isLocked) {
        unlock();
        updateOLED("Unlocked");
        delay(1000);
        updateOLED("To change code: ####");
        delay(2000);
      } else {
        lock();
      }
    } else if (!isLocked && Data == "####") {
      changeCode();
    } else {
      updateOLED("Incorrect");
      delay(1000);
      if (isLocked) {
        lock();
      }
    }

    clearData();
    delay(1000);
    updateOLEDWithVisualFeedback("");
  }
}

// Function to update the OLED display with visual feedback
void updateOLEDWithVisualFeedback(String input) {
  oled.firstPage();
  do {
    int x = (oled.getWidth() - oled.getStrWidth(input.c_str())) / 2;
    int y = oled.getHeight() / 2;

    // Display initial four lines or password feedback
    String displayText = "";
    if (input.length() == 0) {
      for (int i = 0; i < Password_Length; i++) {
        
      }
    } else {
      for (int i = 0; i < Password_Length; i++) {
        if (i < input.length()) {
          displayText += "* ";
        } else {
          displayText += "- ";
        }
      }
    }

    oled.drawStr(x, y, displayText.c_str());
  } while (oled.nextPage());
}


// Function to unlock the system
void unlock() {
  digitalWrite(lockOutput, HIGH);
  isLocked = false;
}

// Function to lock the system
void lock() {
  digitalWrite(lockOutput, LOW);
  isLocked = true;
}

// Function to change the password
void changeCode() {
  updateOLED("Enter new code");
  delay(1000);
  clearData();

  while (data_count < Password_Length) {
    customKey = customKeypad.getKey();
    if (customKey) {
      Data += customKey;
      data_count++;
      updateOLEDWithVisualFeedback(Data);
      delay(500);
    }
  }

  Master = Data;
  updateOLED("Code changed");
  delay(2000);
  lock();
}

// Function to clear stored data
void clearData() {
  data_count = 0;
  Data = "";
}

// Function to update the OLED display
void updateOLED(String input) {
  oled.firstPage();
  do {
    int x = (oled.getWidth() - oled.getStrWidth(input.c_str())) / 2;
    int y = oled.getHeight() / 2;
    oled.drawStr(x, y, input.c_str());
  } while (oled.nextPage());
}
