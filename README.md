# LiquidCrystal_I2C

Library for LCD displays with the I2C interface.

Like its counterpart the LiquidCrystal library, this library allows an Arduino (like) board to control Liquid Crystal displays (LCDs) based on the Hitachi HD44780 (or a compatible) chipset, which is found on most text-based LCDs. The difference is that instead of using multiple digital pins, it communicates with the Arduino (like) via I2C with the help of an I2C expander (PCF8574) attached to the LCD.

## Methods

### **Constructor**

```c++
LiquidCrystal_I2C( uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_sda, uint8_t lcd_scl, uint8_t lcd_dots )
```

The constructor with the I2C address and the dimensions of the LCD (columns and rows). Optionally a big font (5x10 dots) and the SDA/SCL pins.

### **init()**

```c++
void init()
```

This method initializes the LCD.

### **clear()**

```c++
void clear()
```

This method clears the LCD and reset the cursor to the beginning.

### **print()**

```c++
size_t print( data, BASE )
```

This method writes data (char, byte, int, long, or string) to the LCD.
BASE: (optional) the base in which to print numbers: BIN for binary (base 2), DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

## Examples

This is a basic example of writing text to the LCD.

### Code

```c++
#include <LiquidCrystal_I2C.h>

// The LCD display is connected to D1/GPIO5 (SCL) and D2/GPIO4 (SDA)
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  lcd.init();
}

void loop() {
  lcd.clear();
  lcd.setCursor(2, 0);

  lcd.print("Hello world!");

  delay(1000);
}
```