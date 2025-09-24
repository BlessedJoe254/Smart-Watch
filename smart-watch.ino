#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS3231 rtc;

String daysOfTheWeek[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // first upload only
}

void loop() {
  DateTime now = rtc.now();
  display.clearDisplay();

  // ----- TOP ROW ICONS -----
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Network icon (bars)
  int xNet = 2, yNet = 2;
  for(int i=0; i<4; i++){
    display.drawRect(xNet + i*4, yNet + 3 - i, 2, i+1, SSD1306_WHITE);
  }

  // Temperature (center top)
  float temp = rtc.getTemperature();
  String tempStr = String(temp,1) + "C";
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(tempStr, 0, 0, &x1, &y1, &w, &h);
  int xTemp = (SCREEN_WIDTH - w) / 2;
  display.setCursor(xTemp, 0);
  display.println(tempStr);

  // Battery icon (top-right)
  int batWidth = 14, batHeight = 6;
  int batLevel = map(now.minute(), 0, 59, 0, batWidth);
  int xBat = SCREEN_WIDTH - batWidth - 1, yBat = 0;
  display.drawRect(xBat, yBat, batWidth, batHeight, SSD1306_WHITE);
  display.fillRect(xBat, yBat, batLevel, batHeight, SSD1306_WHITE);

  // ----- TIME CENTERED (HH:MM:SS) -----
  int hour12 = now.hour() % 12;
  if(hour12 == 0) hour12 = 12;
  String timeStr = "";
  if(hour12 < 10) timeStr += "0";
  timeStr += String(hour12) + ":";
  if(now.minute() < 10) timeStr += "0";
  timeStr += String(now.minute()) + ":";
  if(now.second() < 10) timeStr += "0";
  timeStr += String(now.second());

  display.setTextSize(2);
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  int xTime = (SCREEN_WIDTH - w) / 2;
  int yTime = 10; // slightly lower than before
  display.setCursor(xTime, yTime);
  display.println(timeStr);

  // AM/PM next to time
  display.setTextSize(1);
  String ampm = now.hour() >= 12 ? "PM" : "AM";
  int xAMPM = xTime + w + 2;
  int yAMPM = yTime + 6;
  display.setCursor(xAMPM, yAMPM);
  display.println(ampm);

  // ----- DATE BELOW TIME -----
  int shortYear = now.year() % 100; // last two digits
  String dateStr = String(daysOfTheWeek[now.dayOfTheWeek()]) + " " +
                   String(now.day()) + "/" +
                   String(now.month()) + "/" +
                   String(shortYear);

  display.setTextSize(1); // smaller font
  display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
  int xDate = (SCREEN_WIDTH - w) / 2;
  int yDate = yTime + 16; // slightly closer to time but not overlapping
  display.setCursor(xDate, yDate);
  display.println(dateStr);

  display.display();
  delay(1000);
}
