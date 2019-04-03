#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Button.cpp"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define ARRAYSIZE 10
#define DOWN_PIN 4
#define UP_PIN 3
#define SELECT_PIN 5

String results[ARRAYSIZE] = { "Location", "Temperature", "Pressure", "Altitude", "Colors" };
int scroll = 0;     // location on menu
int in_item = 0;    // true if in selectItem()
unsigned long lastUpdate = 0;   // update selectItem()

Button down_button(DOWN_PIN);
Button up_button(UP_PIN);
Button select_button(SELECT_PIN);

Adafruit_LiquidCrystal lcd(4);

Adafruit_BMP280 bme;

void setup() {
    Serial.begin(115200);
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
    

}

// Check if buttons are pressed and call perform respective actions
void checkButtons() {
    down_button.check();
    up_button.check();
    select_button.check();

    if (down_button.state() == HIGH && scroll < 4) {
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
            lcd.setCursor(0, 0);
            lcd.print("Lon: 12345");
            lcd.setCursor(0, 1);
            lcd.print("Lat: 122345");
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
            lcd.print("UNDER ");
            lcd.setCursor(0, 1);
            lcd.print("CONSTRUCTION");
            break;
        default:
            break;
  }
}
