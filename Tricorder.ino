#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Button.cpp"

#define ARRAYSIZE 10
#define DOWN_PIN 4
#define UP_PIN 3
#define SELECT_PIN 5

String results[ARRAYSIZE] = { "Location", "Temperature", "Pressure", "Altitude", "Colors" };
int scroll = 0;

Button down_button(DOWN_PIN);
Button up_button(UP_PIN);

Adafruit_LiquidCrystal lcd(4);

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);

    // for (int i =0; i< ARRAYSIZE; i++) Serial.println(results[i]);

    drawMenu();
}

void loop() {
  checkButtons();


}

void checkButtons() {
    down_button.check();
    up_button.check();
    if (down_button.state() == HIGH && scroll < 4){
        scroll++;
        drawMenu();
    }
    if (up_button.state() == HIGH && scroll > 0) {
        scroll--;
        drawMenu();
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
            lcd.print("22 *C");
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("102700 Pa");
            break;
        case 3:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("0.770 m");
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
