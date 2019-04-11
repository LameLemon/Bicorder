#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Button.cpp"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define ARRAYSIZE 10
#define DOWN_PIN 3
#define UP_PIN 4
#define SELECT_PIN 5
#define ON_OFF_PIN 9
#define ON_LED 13
#define BACKLIGHT 12
#define LED_1 A1
#define LED_2 A2
#define LED_3 A3

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 4800;

int color = 0;
int in_color = 0;

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
int prev_r = 0;

Button down_button(DOWN_PIN);
Button up_button(UP_PIN);
Button select_button(SELECT_PIN);

Adafruit_LiquidCrystal lcd(4);

Adafruit_BMP280 bme;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

unsigned long previousMillis = 0;
const long interval = 500;
int currentLED = 0;

byte block[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte therm_top[8] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01010,
  B01110,
  B01110,
  B01110
};
byte therm_bot[8] = {
  B01110,
  B01110,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

void setup() {
    Serial.begin(115200);
    ss.begin(GPSBaud);
    lcd.begin(16, 2);

    // for (int i =0; i< ARRAYSIZE; i++) Serial.println(results[i]);


    if (!bme.begin()) {  
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1);
    }

    pinMode(ON_OFF_PIN, INPUT_PULLUP);
    pinMode(ON_LED, OUTPUT);
    pinMode(BACKLIGHT, OUTPUT);

    boot_seq();
    drawMenu();
    pinMode(LED_1, INPUT); 
    pinMode(LED_2, INPUT);
    pinMode(LED_3, INPUT);

}

void loop() {
    analogWrite(6, color);
    checkButtons();

    // Update selectItem every 1.5 seconds
    if(in_item && (millis() - lastUpdate > 1500))
        selectItem();
    

    while (ss.available() > 0)
        if (gps.encode(ss.read())){
            location();
            datetime();
        }
    
    int r = digitalRead(ON_OFF_PIN);
    // r = HIGH;
    if (!r) {
        digitalWrite(ON_LED, HIGH);
        digitalWrite(BACKLIGHT, LOW);
        digitalWrite(BACKLIGHT, LOW);
        digitalWrite(BACKLIGHT, LOW);
        if (prev_r != r)
            drawMenu();
    } else {
        digitalWrite(ON_LED, LOW);
        digitalWrite(BACKLIGHT, HIGH);
        digitalWrite(BACKLIGHT, HIGH);
        digitalWrite(BACKLIGHT, HIGH);
        if((millis() - lastUpdate > 1500))
            displayDateTime();
        if (prev_r != r)
            displayDateTime();
    }
    prev_r = r;

    currentLED = 1;

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        // Serial.println(currentLED);
        switch (currentLED)
        {
            case 0:
                set_pins(LED_2, LED_1);
                break;
            case 1:
                set_pins(LED_1, LED_2);
                break;
            case 2:
                set_pins(LED_3, LED_2);
                break;
            case 3:
                set_pins(LED_2, LED_3);
                break;
            case 4:
                set_pins(LED_3, LED_1);
                break;
            case 5:
                set_pins(LED_1, LED_3);
                break;
            default:
                break;
        }
        if (currentLED == 6)
            currentLED = 0;
        else
            currentLED++;
    }
}

void set_pins(int high_pin, int low_pin) {
   led_off();
   pinMode(high_pin, OUTPUT);
   pinMode(low_pin, OUTPUT);
   digitalWrite(high_pin, HIGH);
   digitalWrite(low_pin,LOW);
}

void led_off() {
   pinMode(LED_1, INPUT); 
   pinMode(LED_2, INPUT);
   pinMode(LED_3, INPUT);
   digitalWrite(LED_1, LOW);
   digitalWrite(LED_2, LOW);
   digitalWrite(LED_3, LOW);
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

void displayDateTime(){
    lastUpdate = millis();

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
}

// Check if buttons are pressed and call perform respective actions
void checkButtons() {
    down_button.check();
    up_button.check();
    select_button.check();
    // Serial.println(up_button.state());
    int down_button_state = down_button.state();
    int up_button_state = up_button.state();

    if (down_button_state == HIGH && scroll < 5 && in_color == 0) {
        scroll++;
        Serial.print("scroll: ");
        Serial.println(scroll);
        drawMenu();
    }
    if (up_button_state == HIGH && scroll > 0 && in_color == 0) {
        scroll--;
        Serial.print("scroll`: ");
        Serial.println(scroll);
        drawMenu();
    }
    if (select_button.state() == HIGH) {
        Serial.print("in_item: ");
        Serial.print(in_item);
        if (in_item) {
            in_color = 0;
            in_item = 0;
            drawMenu();
        } else {
            selectItem();
            in_item = 1;
        }
        Serial.println(in_item);
    }

    if (down_button_state == HIGH && in_color == 1) {
        if (color > 10) {
            color = color - 10;
        } else {
            color = 0;
            lcd.setCursor(0, 1);
            lcd.print("min value");
        }
        Serial.print("color: ");
        Serial.println(color);
    }
    if (up_button_state == HIGH && in_color == 1) {
        if (color > 250) {
            color = 250;
            lcd.setCursor(0, 1);
            lcd.print("max value");
        } else {
            color = color + 10;
        }
        Serial.print("color: ");
        Serial.println(color);
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
            lcd.setCursor(0, 1);
            lcd.print(bme.readTemperature()* 9/5 + 32 );
            lcd.print(" *F");
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
            in_color = 1;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Color: ");
            lcd.print(color);
            if (color > 250) {
                color = 250;
                lcd.setCursor(0, 1);
                lcd.print("max value");
            }
            if (color < 0) {
                color = 0;
                lcd.setCursor(0, 1);
                lcd.print("min value");
            }
            break;
        default:
            break;
  }
}

void boot_seq(){
    lcd.clear();
    lcd.createChar(0, block);
    for (int i = 0; i < 16; i++) {
        lcd.setCursor(i, 0);
        delay(100);
        lcd.write(0);
    }
    for (int i = 0; i < 16; i++) {
        lcd.setCursor(i, 1);
        delay(100);
        lcd.write(0);
    }
    delay(100);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("BOOT");
    lcd.setCursor(4, 1);
    lcd.print("COMPLETE");
    // lcd.clear();
    // lcd.setCursor(6, 0);
    // lcd.print("GAMEBOY");
    // lcd.setCursor(4, 1);
    // lcd.print("OwO");
    delay(1000);
}