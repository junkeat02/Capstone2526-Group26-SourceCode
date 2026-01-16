#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OledDisplay {
private:
  Adafruit_SSD1306 display;
  
  // Screen dimensions
  static const int SCREEN_WIDTH = 128;
  static const int SCREEN_HEIGHT = 64;
  static const int YELLOW_HEIGHT = 16;
  static const int BLUE_HEIGHT = 48;
  
  // Screen timing
  unsigned long lastScreenUpdate;
  static const unsigned long SCREEN_INTERVAL = 2000;
  
  // Fall flash variables
  bool fallFlashActive;
  unsigned long fallFlashStart;
  int fallFlashStep;
  static const unsigned long FALL_FLASH_INTERVAL = 200;
  static const int FALL_FLASH_TOTAL = 10;
  
  // Scroll variables
  int scrollX;
  bool isScrolling;
  const char* scrollText;
  int textWidth;
  
  // Date and time storage
  char currentDate[20];
  char currentTime[10];
  
  // Helper functions
  void showDateTime(const char* date, const char* timeStr);
  void walkFig(int yOffset);
  void feetFig(int yOffset);
  void centerYellowBlue(const char* txt, int txtSize = 2);
  void centerBlue(const char* line1, const char* line2, int txtSize, bool invert = false, int gap = 5);
  
public:
  OledDisplay();
  
  // Initialization
  bool begin(uint8_t address = 0x3C);
  
  // Update date and time from NTP
  void updateDateTime();
  
  // Screen display functions
  void showBootScreen(const char* groupName);
  void showStepScreen(int stepCount);
  void showHeartScreen(int bpm, int spo2);
  void showFallAlert();
  void showScrollReminder(const char* message = "TIME TO MOVE UP!");
  
  // Fall detection flash
  void startFallFlash();
  void updateFallFlash();
  bool isFallFlashing();
  void stopFallFlash();
  
  // Scroll control
  void startScroll(const char* message = "TIME TO MOVE UP!");
  void updateScroll();
  bool isCurrentlyScrolling();
  void stopScroll();
  
  // Screen update timing
  bool shouldUpdateScreen();
  void resetScreenTimer();
};

#endif