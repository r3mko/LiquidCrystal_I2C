#include "LiquidCrystal_I2C.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note: however, that resetting the Arduino doesn't reset the LCD,
// so we can't assume that its in that state when a sketch starts
// (and the LiquidCrystal constructor is called).

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_sda, uint8_t lcd_scl, uint8_t lcd_dots) {
    _addr = lcd_addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _sdaPin = lcd_sda;
    _sclPin = lcd_scl;
    _dots = lcd_dots;
    _backlightval = LCD_BACKLIGHT;
}

void LiquidCrystal_I2C::init() {
    init_priv();
}

void LiquidCrystal_I2C::oled_init() {
    _oled = true;
    init_priv();
}

void LiquidCrystal_I2C::init_priv() {
    if ((_sdaPin != LCD_SDA) && (_sclPin != LCD_SCL)) {
        Wire.begin(_sdaPin, _sclPin);
    }
    else {
        Wire.begin();
    }
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(_cols, _rows, _dots);
}

void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t rows, uint8_t dotsize) {
    if (rows > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = rows;

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

    // For some 1 line displays you can select a 10 pixel high font
    if ((dotsize != LCD_5x8DOTS) && (rows == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // See page 45/46 figure 23/24 for the initialization specification.
    // According to the hitachi HD44780 datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50ms
    delay(50); 
  
    // Now we pull both RS and R/W low to begin commands
    expanderWrite(_backlightval); // reset expander (Bit 8 = 1)
    delay(500);

    // Put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46
    
    // We start in 8bit mode
    // try to set to 4 bit mode
    write4bits(0x03 << 4);
    delayMicroseconds(4200); // wait min 4.1ms
   
    // Second try
    write4bits(0x03 << 4);
    delayMicroseconds(110); // wait min 100us
   
    // Third time's the charm!
    write4bits(0x03 << 4); 
   
    // Finally, set to 4-bit interface
    write4bits(0x02 << 4); 

    // Set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);  
    
    // Turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    
    // Clear it
    clear();
    
    // Initialize to default text direction (for roman languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    
    // Set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);
    
    home();  
}

void LiquidCrystal_I2C::setRowOffsets(int row0, int row1, int row2, int row3) {
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
}

/* High level commands, for the user! */

void LiquidCrystal_I2C::clear() {
    command(LCD_CLEARDISPLAY); // Clear display, set cursor position to zero
    delayMicroseconds(2000); // This command takes a long time!
    if (_oled) {
        setCursor(0, 0);
    }
}

void LiquidCrystal_I2C::home() {
    command(LCD_RETURNHOME); // Set cursor position to zero
    delayMicroseconds(2000); // This command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);

    if (row >= max_lines) {
        row = max_lines - 1; // we count rows starting with 0
    }
    if (row >= _numlines) {
        row = _numlines - 1; // We count rows starting with 0
    }

    command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal_I2C::scrollDisplayRight(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // We only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        write(charmap[i]);
    }
}

// Create Char with PROGMEM input
void LiquidCrystal_I2C::createChar(uint8_t location, const char *charmap) {
    location &= 0x7; // We only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        write(pgm_read_byte_near(charmap++));
    }
}

// Turn the (optional) backlight on/off
void LiquidCrystal_I2C::noBacklight(void) {
    _backlightval=LCD_NOBACKLIGHT;
    expanderWrite(0);
}

void LiquidCrystal_I2C::backlight(void) {
    _backlightval=LCD_BACKLIGHT;
    expanderWrite(0);
}

/* Mid level commands, for sending command or data */

inline void LiquidCrystal_I2C::command(uint8_t value) {
    send(value, 0);
}

inline size_t LiquidCrystal_I2C::write(uint8_t value) {
    send(value, Rs);
    return 1;
}

/* Low level data pushing commands */

// Write either command or data
void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
    uint8_t highnib=value&0xf0;
    uint8_t lownib=(value<<4)&0xf0;
    write4bits((highnib)|mode);
    write4bits((lownib)|mode); 
}

void LiquidCrystal_I2C::write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void LiquidCrystal_I2C::expanderWrite(uint8_t _data) {                                        
    Wire.beginTransmission(_addr);
    Wire.write((int)(_data) | _backlightval);
    Wire.endTransmission();   
}

void LiquidCrystal_I2C::pulseEnable(uint8_t _data) {
    expanderWrite(_data | En); // En high
    delayMicroseconds(1); // Enable pulse must be >450ns
    
    expanderWrite(_data & ~En); // En low
    delayMicroseconds(42); // Commands need > 37us to settle
}

// Alias functions
void LiquidCrystal_I2C::cursor_on() {
    cursor();
}

void LiquidCrystal_I2C::cursor_off() {
    noCursor();
}

void LiquidCrystal_I2C::blink_on() {
    blink();
}

void LiquidCrystal_I2C::blink_off() {
    noBlink();
}

void LiquidCrystal_I2C::load_custom_character(uint8_t char_num, uint8_t *rows) {
    createChar(char_num, rows);
}

void LiquidCrystal_I2C::setBacklight(uint8_t new_val) {
    if(new_val) {
        backlight(); // Turn backlight on
    } else {
        noBacklight(); // Turn backlight off
    }
}

// Unsupported API functions
void LiquidCrystal_I2C::off() {}
void LiquidCrystal_I2C::on() {}
void LiquidCrystal_I2C::setDelay(int cmdDelay, int charDelay) {}
uint8_t LiquidCrystal_I2C::status() {return 0;}
uint8_t LiquidCrystal_I2C::keypad() {return 0;}
uint8_t LiquidCrystal_I2C::init_bargraph(uint8_t graphtype) {return 0;}
void LiquidCrystal_I2C::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end) {}
void LiquidCrystal_I2C::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_row_end) {}
void LiquidCrystal_I2C::setContrast(uint8_t new_val) {}