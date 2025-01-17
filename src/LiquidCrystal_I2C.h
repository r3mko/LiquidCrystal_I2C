#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include <Arduino.h>
#include <inttypes.h>
#include <Print.h> 
#include <Wire.h>

// Commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// Flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100 // Enable bit
#define Rw B00000010 // Read/Write bit
#define Rs B00000001 // Register select bit

#define LCD_SDA 255
#define LCD_SCL 255

class LiquidCrystal_I2C : public Print {
public:
  LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_sda = LCD_SDA, uint8_t lcd_scl = LCD_SCL, uint8_t lcd_dots = LCD_5x8DOTS);
  void init();
  void oled_init();
  void begin(uint8_t cols, uint8_t rows, uint8_t dotsize = LCD_5x8DOTS );
  void setRowOffsets(int row1, int row2, int row3, int row4);
  void clear();
  void home();
  void setCursor(uint8_t col, uint8_t row); 
  void noDisplay();
  void display();
  void noCursor();
  void cursor();
  void noBlink();
  void blink();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll(); 
  void createChar(uint8_t location, uint8_t charmap[]);
  void createChar(uint8_t location, const char *charmap); // Example: const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000}; 
  void noBacklight();
  void backlight();
  void command(uint8_t value);
  virtual size_t write(uint8_t value);

/* Compatibility API function aliases */

void display_on(); // Alias for display()
void display_off(); // Alias for noDisplay()
void cursor_on(); // Alias for cursor()
void cursor_off(); // Alias for noCursor()
void blink_on(); // Alias for blink()
void blink_off(); // Alias for noBlink()
void load_custom_character(uint8_t char_num, uint8_t *rows); // Alias for createChar()
void setBacklight(uint8_t new_val); // Alias for backlight() and nobacklight()

private:
  void init_priv();
  void send(uint8_t value, uint8_t mode);
  void write4bits(uint8_t value);
  void expanderWrite(uint8_t _data);
  void pulseEnable(uint8_t _data);
  uint8_t _addr;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _sdaPin;
  uint8_t _sclPin;
  uint8_t _dots;
  uint8_t _backlightval;
  bool _oled = false;
  uint8_t _displayfunction;
  uint8_t _numlines;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _row_offsets[4];
};

#endif