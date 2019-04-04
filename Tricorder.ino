#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Button.cpp"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define ARRAYSIZE 10
#define DOWN_PIN 4
#define UP_PIN 3
#define SELECT_PIN 5
static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 4800;

String results[ARRAYSIZE] = { "Location", "Temperature", "Pressure", "Altitude", "Time", "Colors" };
int scroll = 0;     // location on menu
int in_item = 0;    // true if in selectItem()
unsigned long lastUpdate = 0;   // update selectItem()
struct coord {
    float lat;
    float lon;
} c;
struct date {
    int month;
    int day;
    int year;
    int hour;
    int minute;
    int second;
    int centisecond; 
} d;

Button down_button(DOWN_PIN);
Button up_button(UP_PIN);
Button select_button(SELECT_PIN);

Adafruit_LiquidCrystal lcd(4);

Adafruit_BMP280 bme;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);


void setup() {
    Serial.begin(115200);
    ss.begin(GPSBaud);
    lcd.begin(16, 2);

    // for (int i =0; i< ARRAYSIZE; i++) Serial.println(results[i]);

    drawMenu();

    if (!bme.begin()) {  
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1);
    }
}

void loop() {
    checkButtons();

    // Update selectItem every 1.5 seconds
    if(in_item && (millis() - lastUpdate > 1500))
        selectItem();
    

    while (ss.available() > 0)
        if (gps.encode(ss.read())){
            location();
            datetime();
        }
}

void location() {
    if (gps.location.isValid()){
        c.lat = gps.location.lat();
        c.lon = gps.location.lng();
    } else {
        c.lat = 42.952005;
        c.lon = -78.823872;
    }
}

void datetime(){
    if (gps.date.isValid()) {
        d.month = gps.date.month();
        d.day = gps.date.day();
        d.year = gps.date.year();
    } else {
        d.month = 06;
        d.day = 06;
        d.year = 2020;
    }

    if (gps.time.isValid()){
        d.hour = gps.time.hour();
        d.minute = gps.time.minute();
        d.second = gps.time.second();
        d.centisecond = gps.time.centisecond();
    } else {
        d.hour = 24;
        d.minute = 24;
        d.second = 24;
        d.centisecond = 24;
    }
}
void displayInfo() {
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

// Check if buttons are pressed and call perform respective actions
void checkButtons() {
    down_button.check();
    up_button.check();
    select_button.check();

    if (down_button.state() == HIGH && scroll < 5) {
        scroll++;
        Serial.print("scroll: ");
        Serial.println(scroll);
        drawMenu();
    }
    if (up_button.state() == HIGH && scroll > 0) {
        scroll--;
        Serial.print("scroll: ");
        Serial.println(scroll);
        drawMenu();
    }
    if (select_button.state() == HIGH) {
        Serial.print("in_item: ");
        Serial.print(in_item);
        if (in_item) {
            in_item = 0;
            drawMenu();
        } else {
            selectItem();
            in_item = 1;
        }
        Serial.println(in_item);
    }
}

// Draw manu based on scroll position
void drawMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("> " + results[scroll]);
    lcd.setCursor(0, 1);
    lcd.print(results[scroll + 1]);
}

// Call individual item pages from menu
void selectItem() {
    lastUpdate = millis();
    switch (scroll) {
        case 0:
            lcd.clear();
            Serial.println(c.lon);
            lcd.setCursor(0, 0);
            lcd.print("Lat: " + String(c.lat));    
            lcd.setCursor(0, 1);
            lcd.print("Lon: " + String(c.lon));        
            break;
        case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(bme.readTemperature());
            lcd.print(" *C");
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(bme.readPressure());
            lcd.print(" Pa");
            break;
        case 3:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(bme.readAltitude(1013.25));
            lcd.print(" m");
            break;
        case 4:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(d.month);
            lcd.print("/");
            lcd.print(d.day);
            lcd.print("/");
            lcd.print(d.year);
            lcd.setCursor(0, 1);
            lcd.print(d.hour);
            lcd.print(":");
            lcd.print(d.minute);
            lcd.print(":");
            lcd.print(d.second);
            break;
        case 5:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("UNDER ");
            lcd.setCursor(0, 1);
            lcd.print("CONSTRUCTION");
            break;
        default:
            break;
  }
}
