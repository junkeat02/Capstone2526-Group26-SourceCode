#include "oledDisplay.h"
#include <time.h>

OledDisplay::OledDisplay() :
  display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1),
  lastScreenUpdate(0),
  fallFlashActive(false),
  fallFlashStart(0),
  fallFlashStep(0),
  scrollX(SCREEN_WIDTH),
  isScrolling(false),
  scrollText("TIME TO MOVE UP!"),
  textWidth(0)
{
  memset(currentDate, 0, sizeof(currentDate));
  memset(currentTime, 0, sizeof(currentTime));
}

bool OledDisplay::begin(uint8_t address) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, address)) {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }
  display.clearDisplay();
  display.display();
  return true;
}

void OledDisplay::updateDateTime() {
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  
  strftime(currentDate, sizeof(currentDate), "%a, %d/%m", &timeinfo);
  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);
}

void OledDisplay::showDateTime(const char* date, const char* timeStr) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Date at top-left
  display.setCursor(0, 0);
  display.println(date);
  
  // Time at top-right
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(SCREEN_WIDTH - w, 0);
  display.println(timeStr);
}

void OledDisplay::walkFig(int yOffset) {
  int centerX = SCREEN_WIDTH / 2;
  
  // Head
  display.drawCircle(centerX, yOffset, 4, SSD1306_WHITE);
  // Body
  display.drawLine(centerX, yOffset + 4, centerX, yOffset + 14, SSD1306_WHITE);
  // Arms
  display.drawLine(centerX - 5, yOffset + 8, centerX + 5, yOffset + 8, SSD1306_WHITE);
  // Legs
  display.drawLine(centerX, yOffset + 14, centerX - 5, yOffset + 20, SSD1306_WHITE);
  display.drawLine(centerX, yOffset + 14, centerX + 5, yOffset + 20, SSD1306_WHITE);
}

void OledDisplay::feetFig(int yOffset) {
  int centerX = SCREEN_WIDTH / 2;
  
  // Left foot
  display.fillCircle(centerX - 5, yOffset, 2, SSD1306_WHITE);
  display.fillCircle(centerX - 2, yOffset + 2, 2, SSD1306_WHITE);
  // Right foot
  display.fillCircle(centerX + 5, yOffset, 2, SSD1306_WHITE);
  display.fillCircle(centerX + 2, yOffset + 2, 2, SSD1306_WHITE);
  
  display.fillRect(0, yOffset + 3, SCREEN_WIDTH, 2, SSD1306_BLACK);
}

void OledDisplay::centerYellowBlue(const char* txt, int txtSize) {
  int16_t x1, y1;
  uint16_t w, h;
  
  display.setTextSize(txtSize);
  display.setTextColor(SSD1306_WHITE);
  
  display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
  
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = YELLOW_HEIGHT + (BLUE_HEIGHT / 2) - (h / 2);
  
  display.setCursor(x, y);
  display.println(txt);
}

void OledDisplay::centerBlue(const char* line1, const char* line2, int txtSize, bool invert, int gap) {
  int16_t x1, y1;
  uint16_t w1, h1, w2, h2;
  
  display.setTextSize(txtSize);
  display.setTextColor(invert ? SSD1306_BLACK : SSD1306_WHITE);
  
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);
  display.getTextBounds(line2, 0, 0, &x1, &y1, &w2, &h2);
  
  uint16_t totalH = h1 + h2 + gap;
  int16_t startY = YELLOW_HEIGHT + (BLUE_HEIGHT - totalH) / 2;
  
  display.setCursor((SCREEN_WIDTH - w1) / 2, startY);
  display.println(line1);
  
  display.setCursor((SCREEN_WIDTH - w2) / 2, startY + h1 + gap);
  display.println(line2);
}

void OledDisplay::showBootScreen(const char* groupName) {
  display.clearDisplay();
  centerYellowBlue(groupName);
  display.display();
}

void OledDisplay::showStepScreen(int stepCount) {
  display.clearDisplay();
  
  // updateDateTime();
  // showDateTime(currentDate, currentTime);
  
  feetFig(12);
  
  int16_t x1, y1;
  uint16_t w1, h1, w2, h2;
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  display.getTextBounds("Steps", 0, 0, &x1, &y1, &w1, &h1);
  
  char stepStr[10];
  sprintf(stepStr, "%d", stepCount);
  display.getTextBounds(stepStr, 0, 0, &x1, &y1, &w2, &h2);
  
  int gap = 10;
  int16_t totalH = h1 + h2 + gap;
  int16_t startY = YELLOW_HEIGHT + (BLUE_HEIGHT - totalH) / 2;
  
  display.setCursor((SCREEN_WIDTH - w1) / 2, startY);
  display.println("Steps");
  
  display.setCursor((SCREEN_WIDTH - w2) / 2, startY + h1 + gap);
  display.println(stepStr);
  
  display.display();
}

void OledDisplay::showHeartScreen(int bpm, int spo2) {
  display.clearDisplay();
  
  // updateDateTime();
  // showDateTime(currentDate, currentTime);
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  char hrStr[20];
  char spo2Str[20];
  sprintf(hrStr, "HR:%d BPM", bpm);
  sprintf(spo2Str, "SpO2:%d%%", spo2);
  
  int16_t hx, hy;
  uint16_t hw, hh;
  int16_t sx, sy;
  uint16_t sw, sh;
  
  display.getTextBounds(hrStr, 0, 0, &hx, &hy, &hw, &hh);
  display.getTextBounds(spo2Str, 0, 0, &sx, &sy, &sw, &sh);
  
  int gapBlue = 6;
  int totalTextHeight = hh + sh + gapBlue;
  int16_t startY = YELLOW_HEIGHT + (BLUE_HEIGHT - totalTextHeight) / 2;
  
  display.setCursor((SCREEN_WIDTH - hw) / 2, startY);
  display.println(hrStr);
  
  display.setCursor((SCREEN_WIDTH - sw) / 2, startY + hh + gapBlue);
  display.println(spo2Str);
  
  display.display();
}

void OledDisplay::startFallFlash() {
  fallFlashActive = true;
  fallFlashStart = millis();
  fallFlashStep = 0;
}

void OledDisplay::updateFallFlash() {
  if (!fallFlashActive) return;
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - fallFlashStart >= FALL_FLASH_INTERVAL) {
    fallFlashStart = currentMillis;
    display.clearDisplay();
    
    // Toggle the flash (Even steps show white background, Odd steps show black)
    if (fallFlashStep % 2 == 0) {
      display.fillRect(0, YELLOW_HEIGHT, SCREEN_WIDTH, BLUE_HEIGHT, SSD1306_WHITE);
      centerBlue("!! FALL !!", "HELP", 2, true, 8);
    } else {
      // Normal text on black background for the "blink" effect
      centerBlue("!! FALL !!", "HELP", 2, false, 8);
    }
    
    display.display();
    fallFlashStep++;
    
    // REMOVED: if (fallFlashStep >= FALL_FLASH_TOTAL) { fallFlashActive = false; }
    // Now it will flash forever until stopFallFlash() is called.
  }
}

void OledDisplay::showFallAlert() {
  display.clearDisplay();
  
  // updateDateTime();
  // showDateTime(currentDate, currentTime);
  
  display.fillRect(0, YELLOW_HEIGHT, SCREEN_WIDTH, BLUE_HEIGHT, SSD1306_WHITE);
  centerBlue("!! FALL !!", "HELP", 2, true, 8);
  
  display.display();
}

bool OledDisplay::isFallFlashing() {
  return fallFlashActive;
}

void OledDisplay::stopFallFlash() {
  fallFlashActive = false;
  fallFlashStep = 0;
}

void OledDisplay::startScroll(const char* message) {
  scrollText = message;
  isScrolling = true;
  scrollX = SCREEN_WIDTH;
  textWidth = 0;
}

void OledDisplay::updateScroll() {
  if (!isScrolling) return;
  
  display.setFont();
  display.setTextSize(1);
  
  if (textWidth == 0) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(scrollText, 0, 0, &x1, &y1, &w, &h);
    textWidth = w;
  }
  
  int y = YELLOW_HEIGHT + 12 + (BLUE_HEIGHT - 8) / 2;
  
  display.clearDisplay();
  // updateDateTime();
  // showDateTime(currentDate, currentTime);
  walkFig(22);
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(scrollX, y);
  display.println(scrollText);
  display.display();
  
  scrollX -= 1;
  
  if (scrollX < -textWidth) {
    scrollX = SCREEN_WIDTH;
    isScrolling = false;
  }
}

void OledDisplay::showScrollReminder(const char* message) {
  if (!isScrolling) {
    startScroll(message);
  }
  updateScroll();
}

bool OledDisplay::isCurrentlyScrolling() {
  return isScrolling;
}

void OledDisplay::stopScroll() {
  isScrolling = false;
  scrollX = SCREEN_WIDTH;
}

bool OledDisplay::shouldUpdateScreen() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastScreenUpdate >= SCREEN_INTERVAL) {
    lastScreenUpdate = currentMillis;
    return true;
  }
  return false;
}

void OledDisplay::resetScreenTimer() {
  lastScreenUpdate = millis();
}