// Define LCD pins
#define RS 12      // Register Select pin
#define EN 11      // Enable pin
#define D4 5       // Data pin 4
#define D5 4       // Data pin 5
#define D6 3       // Data pin 6
#define D7 2       // Data pin 7

// Define keypad pins
#define ROW1 A0    
#define ROW2 A1    
#define ROW3 A2     
#define ROW4 A3     

#define COL1 8     // D8
#define COL2 9     // D9
#define COL3 10    // D10
#define COL4 6     // D6
#define COL5 7     // D7

// Define macros for pin manipulation
#define SetBit(PORT, BIT) ((PORT) |= (1 << (BIT)))
#define ClearBit(PORT, BIT) ((PORT) &= ~(1 << (BIT)))
#define ReadBit(PORT, BIT) ((PORT) & (1 << (BIT)))

// Convert Arduino pin numbers to AVR port and pin
#define RS_PORT PORTB
#define RS_PIN 4    // Arduino pin 12 is PB4
#define EN_PORT PORTB
#define EN_PIN 3    // Arduino pin 11 is PB3
#define D4_PORT PORTD
#define D4_PIN 5    // Arduino pin 5 is PD5
#define D5_PORT PORTD
#define D5_PIN 4    // Arduino pin 4 is PD4
#define D6_PORT PORTD
#define D6_PIN 3    // Arduino pin 3 is PD3
#define D7_PORT PORTD
#define D7_PIN 2    // Arduino pin 2 is PD2

// Define direction registers
#define RS_DDR DDRB
#define EN_DDR DDRB
#define D4_DDR DDRD
#define D5_DDR DDRD
#define D6_DDR DDRD
#define D7_DDR DDRD

// Keypad row port and pin definitions
#define ROW1_PORT PORTC
#define ROW1_PIN 0     // A0 is PC0
#define ROW2_PORT PORTC
#define ROW2_PIN 1     // A1 is PC1
#define ROW3_PORT PORTC
#define ROW3_PIN 2     // A2 is PC2
#define ROW4_PORT PORTC
#define ROW4_PIN 3     // A3 is PC3

// Keypad row direction registers
#define ROW1_DDR DDRC
#define ROW2_DDR DDRC
#define ROW3_DDR DDRC
#define ROW4_DDR DDRC

// Keypad column port and pin definitions
#define COL1_PORT PORTB
#define COL1_PIN 0     // D8 is PB0
#define COL2_PORT PORTB
#define COL2_PIN 1     // D9 is PB1
#define COL3_PORT PORTB
#define COL3_PIN 2     // D10 is PB2
#define COL4_PORT PORTD
#define COL4_PIN 6     // D6 is PD6
#define COL5_PORT PORTD
#define COL5_PIN 7     // D7 is PD7

// Keypad column direction registers
#define COL1_DDR DDRB
#define COL2_DDR DDRB
#define COL3_DDR DDRB
#define COL4_DDR DDRD
#define COL5_DDR DDRD

// Keypad column input registers
#define COL1_PIN_REG PINB
#define COL2_PIN_REG PINB
#define COL3_PIN_REG PINB
#define COL4_PIN_REG PIND
#define COL5_PIN_REG PIND

// LCD commands
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_4BIT_2LINE 0x28
#define LCD_SET_CURSOR 0x80


// Function to write a string to buffer and update index
void write_string_to_buffer(const char *src, char *dest, int *index) {
    int i = 0;
    while (src[i] != '\0' && (*index + i) < MAX_EXPRESSION_LENGTH - 1) {
        dest[*index + i] = src[i];
        i++;
    }
    *index += i;
    dest[*index] = '\0';
}

char keypad_scan(void) {
    // Variable to store the pressed key
    char pressed_key = 0;
    
    // Iterate through each row
    for (int r = 0; r < 4; r++) {
        // Set the current row to LOW, keep others HIGH
        if (r == 0) ClearBit(ROW1_PORT, ROW1_PIN); else SetBit(ROW1_PORT, ROW1_PIN);
        if (r == 1) ClearBit(ROW2_PORT, ROW2_PIN); else SetBit(ROW2_PORT, ROW2_PIN);
        if (r == 2) ClearBit(ROW3_PORT, ROW3_PIN); else SetBit(ROW3_PORT, ROW3_PIN);
        if (r == 3) ClearBit(ROW4_PORT, ROW4_PIN); else SetBit(ROW4_PORT, ROW4_PIN);
        
        // Small delay to allow signals to stabilize
        _delay_us(10);
        
        // Check each column for the current row
        if (!(COL1_PIN_REG & (1 << COL1_PIN))) {
            pressed_key = pgm_read_byte(&keypad_normal[r][0]);
        }
        if (!(COL2_PIN_REG & (1 << COL2_PIN))) {
            pressed_key = pgm_read_byte(&keypad_normal[r][1]);
        }
        if (!(COL3_PIN_REG & (1 << COL3_PIN))) {
            pressed_key = pgm_read_byte(&keypad_normal[r][2]);
        }
        if (!(COL4_PIN_REG & (1 << COL4_PIN))) {
            pressed_key = pgm_read_byte(&keypad_normal[r][3]);
        }
        if (!(COL5_PIN_REG & (1 << COL5_PIN))) {
            pressed_key = pgm_read_byte(&keypad_normal[r][4]);
        }
        
        // If a key was pressed, restore row states and return the key
        if (pressed_key != 0) {
            // Restore all rows to HIGH
            SetBit(ROW1_PORT, ROW1_PIN);
            SetBit(ROW2_PORT, ROW2_PIN);
            SetBit(ROW3_PORT, ROW3_PIN);
            SetBit(ROW4_PORT, ROW4_PIN);
            
            return pressed_key;
        }
    }
    
    // No key was pressed, restore row states
    SetBit(ROW1_PORT, ROW1_PIN);
    SetBit(ROW2_PORT, ROW2_PIN);
    SetBit(ROW3_PORT, ROW3_PIN);
    SetBit(ROW4_PORT, ROW4_PIN);
    
    return 0;
}


// Function to initialize the LCD
void lcd_init(void) {
    // Set data pins as output
    SetBit(RS_DDR, RS_PIN);
    SetBit(EN_DDR, EN_PIN);
    SetBit(D4_DDR, D4_PIN);
    SetBit(D5_DDR, D5_PIN);
    SetBit(D6_DDR, D6_PIN);
    SetBit(D7_DDR, D7_PIN);
    
    // Wait for LCD to initialize
    _delay_ms(50);
    
    // 4-bit initialization sequence
    // Set to 4-bit mode
    ClearBit(RS_PORT, RS_PIN);  // RS = 0 for command
    
    // Function set - 8-bit mode first (3 times)
    ClearBit(D4_PORT, D4_PIN);
    ClearBit(D5_PORT, D5_PIN);
    SetBit(D6_PORT, D6_PIN);
    SetBit(D7_PORT, D7_PIN);
    
    // Pulse enable
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(5);
    
    // Repeat again
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(1);
    
    // Repeat third time
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(1);
    
    // Now set to 4-bit mode
    ClearBit(D7_PORT, D7_PIN);
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(1);
    
    // Now we can use lcd_command function
    // Set 4-bit interface, 2 line display, 5x8 font
    lcd_command(LCD_FUNCTION_4BIT_2LINE);
    // Display on, cursor on, blinking off
    lcd_command(LCD_DISPLAY_ON);
    // Entry mode set: increment cursor, no display shift
    lcd_command(LCD_ENTRY_MODE);
    // Clear display
    lcd_command(LCD_CLEAR);
    // Return home
    lcd_command(LCD_HOME);
    
    _delay_ms(2);
}

// Function to send a command to the LCD
void lcd_command(unsigned char cmd) {
    // RS = 0 for command
    ClearBit(RS_PORT, RS_PIN);
    
    // Send high nibble
    (cmd & 0x80) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (cmd & 0x40) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (cmd & 0x20) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (cmd & 0x10) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Pulse enable
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    _delay_us(200);
    
    // Send low nibble
    (cmd & 0x08) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (cmd & 0x04) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (cmd & 0x02) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (cmd & 0x01) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Pulse enable
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    // Commands need more time
    if (cmd == LCD_CLEAR || cmd == LCD_HOME) {
        _delay_ms(2);
    } else {
        _delay_us(50);
    }
}

// Function to send data (character) to the LCD
void lcd_char(unsigned char data) {
    // RS = 1 for data
    SetBit(RS_PORT, RS_PIN);
    
    // Send high nibble
    (data & 0x80) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (data & 0x40) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (data & 0x20) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (data & 0x10) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Pulse enable
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    _delay_us(200);
    
    // Send low nibble
    (data & 0x08) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (data & 0x04) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (data & 0x02) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (data & 0x01) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Pulse enable
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    _delay_us(50);
}

// Function to display a string on the LCD
void lcd_string(const char *str) {
    while (*str) {
        lcd_char(*str++);
    }
}

// Function to display a string from program memory
void lcd_string_P(const char *str) {
    char c;
    while ((c = pgm_read_byte(str++))) {
        lcd_char(c);
    }
}

// Function to clear the LCD
void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
}

// Function to initialize the keypad
void keypad_init(void) {
    // Set row pins as outputs
    SetBit(ROW1_DDR, ROW1_PIN);
    SetBit(ROW2_DDR, ROW2_PIN);
    SetBit(ROW3_DDR, ROW3_PIN);
    SetBit(ROW4_DDR, ROW4_PIN);
    
    // Set row pins HIGH initially
    SetBit(ROW1_PORT, ROW1_PIN);
    SetBit(ROW2_PORT, ROW2_PIN);
    SetBit(ROW3_PORT, ROW3_PIN);
    SetBit(ROW4_PORT, ROW4_PIN);
    
    // Set column pins as inputs with pull-ups
    ClearBit(COL1_DDR, COL1_PIN);
    ClearBit(COL2_DDR, COL2_PIN);
    ClearBit(COL3_DDR, COL3_PIN);
    ClearBit(COL4_DDR, COL4_PIN);
    ClearBit(COL5_DDR, COL5_PIN);
    
    SetBit(COL1_PORT, COL1_PIN);  // Enable pull-up
    SetBit(COL2_PORT, COL2_PIN);  // Enable pull-up
    SetBit(COL3_PORT, COL3_PIN);  // Enable pull-up
    SetBit(COL4_PORT, COL4_PIN);  // Enable pull-up
    SetBit(COL5_PORT, COL5_PIN);  // Enable pull-up
}
