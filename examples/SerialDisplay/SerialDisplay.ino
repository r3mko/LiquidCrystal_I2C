#include <LiquidCrystal_I2C.h>

// The LCD display is connected to D1/GPIO5 (SCL) and D2/GPIO4 (SDA)
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  lcd.init();
}

void loop() {
  // When characters arrive over the serial port...
  if (Serial.available()) {
    // Wait a bit for the entire message to arrive
    delay(100);
    // Clear the screen
    lcd.clear();
    // Read all the available characters
    while (Serial.available() > 0) {
      // Display each character to the LCD
      lcd.write(Serial.read());
    }
  }
}