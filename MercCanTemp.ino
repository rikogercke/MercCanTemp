#include <Arduino.h>
#include "mcp2515.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CANC_CS 10

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define SCREEN_ADDRESS 0x3C  ///0x3D for 128x64, 0x3C for 128x32

long enginetemp = 0;
long gearboxtemp = 0;

MCP2515* canC;
can_frame io_can_frame = { 0x00 };  // Reserve this in memory

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup() {
  // Init the OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // rotate
  display.setRotation(2);


  //StartupAnimation

  for (int i = 0; i < 100; i++) {
    displaytemp(String(i), String(i));
    delay(8);
  }

  display.invertDisplay(true);
  delay(400);
  display.invertDisplay(false);
  delay(400);
  display.invertDisplay(true);
  delay(400);
  display.invertDisplay(false);
  delay(400);


  // Init the CAN module
  canC = new MCP2515(CANC_CS);
  canC->reset();
  canC->setBitrate(CAN_500KBPS);

  // Set it as read only!
  canC->setListenOnlyMode();

  // Clear the OLED buffer
  display.clearDisplay();
  display.display();
}

void displaytemp(String temp1, String temp2) {
  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text

  display.setCursor(0, 0);  // Start at top-left corner
  display.println(F("M113            722.6"));

  String line;

  if ((temp1.length() + temp2.length()) == 2) {
    display.setTextSize(3);  // Draw 2X-scale text
    line = temp1 + "C   " + temp2 + "C";
  } else if ((temp1.length() + temp2.length()) == 3) {
    display.setTextSize(3);  // Draw 2X-scale text
    line = temp1 + "C  " + temp2 + "C";
  } else if ((temp1.length() + temp2.length()) == 4) {
    display.setTextSize(3);  // Draw 2X-scale text
    line = temp1 + "C " + temp2 + "C";
  } else if ((temp1.length() + temp2.length()) == 5) {
    display.setTextSize(2);  // Draw 2X-scale text
    line = temp1 + "C   " + temp2 + "C";
  } else if ((temp1.length() + temp2.length()) == 6) {
    display.setTextSize(2);  // Draw 2X-scale text
    line = temp1 + "C  " + temp2 + "C";
  }

  display.println(line);
  display.display();
}

void loop() {
  // Get new Messages on CAN C
  if (canC->readMessage(&io_can_frame) == MCP2515::ERROR_OK) {
    if (io_can_frame.can_id == 0x308) {  //Getting the 0x0308 Frame with engine temperature--offset 40 len 8
      if (io_can_frame.can_dlc == 8) {
        enginetemp = io_can_frame.data[5] - 40;
      }
    } else if (io_can_frame.can_id == 0x418) {  //Getting the 0x0418 Frame with gearbox temperature--offset 16 len 8
      if (io_can_frame.can_dlc == 8) {
        gearboxtemp = io_can_frame.data[2] - 50;
      }
    }
    displaytemp(String(enginetemp), String(gearboxtemp));
  }
}