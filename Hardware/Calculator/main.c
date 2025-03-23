#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

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

const char keypad_normal[4][5] PROGMEM = {
    {'7', '8', '9', '+', 'D'},
    {'4', '5', '6', '-', 'S'},
    {'1', '2', '3', '*', 'A'},
    {'0', '.', '=', '/', 'C'}
};

const char* const keypad_alpha[4][5] PROGMEM  = {
    {"sin", "cos", "tan", "^", "BS"},
    {"log", "ln", "e^", "√", "("},
    {"π", "x²", "x³", "1/x", ")"},
    {"EXP", "ANS", "M+", "M-", "MR"}
};

const char* const shiftFunctions[4][5] PROGMEM = {
    {"asin", "acos", "atan", "y^x", "CLR"},
    {"10^", "e", "abs", "cbrt", "["},
    {"deg", "rad", "mod", "fact", "]"},
    {"HEX", "DEC", "BIN", "OCT", "MC"}
};


// Global variables
int shift_mode = 0;
int alpha_mode = 0;
double memory = 0.0;
double last_result = 0.0;
char buffer[16];  // Global buffer for string operations

// Function prototypes
void lcd_init(void);
void lcd_command(unsigned char cmd);
void lcd_char(unsigned char data);
void lcd_string(char *str);
void lcd_clear(void);
void keypad_init(void);
char keypad_scan(void);
void evaluate_expression(char *expression);
double perform_operation(double a, double b, char op);
double parse_number(char **expr);
void handle_shift_mode(char *key, char *expression, int *expr_index);
void handle_alpha_mode(char *key, char *expression, int *expr_index);
void handle_special_functions(char *key, char *expression, int *expr_index);
double factorial(int n);

#define MAX_EXPRESSION_LENGTH 50

int main(void) {
    char expression[MAX_EXPRESSION_LENGTH] = "";
    int expr_index = 0;

    lcd_init();
    keypad_init();

    lcd_string("Welcome !");
    _delay_ms(2000);
    lcd_clear();

    lcd_string("Enter expression:");
    lcd_command(LCD_SET_CURSOR | 0x40);  // Move to second line

    while(1) {
        char key = keypad_scan();

        if (key != 0) {
            if (key == 'S') {
                shift_mode = !shift_mode;
                lcd_clear();
                lcd_string(shift_mode ? "Shift mode ON" : "Shift mode OFF");
                _delay_ms(1000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'A') {
                alpha_mode = !alpha_mode;
                lcd_clear();
                lcd_string(alpha_mode ? "Alpha mode ON" : "Alpha mode OFF");
                _delay_ms(1000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'C') {
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                expr_index = 0;
                expression[expr_index] = '\0';
            } else if (key == 'D' && expr_index > 0) {
                expr_index--;
                expression[expr_index] = '\0';
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string("                ");  // Clear second line
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == '=') {
                expression[expr_index] = '\0';
                lcd_clear();
                lcd_string("Result:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                evaluate_expression(expression);
                expr_index = 0;
                expression[expr_index] = '\0';
                _delay_ms(3000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
            } else if (expr_index < MAX_EXPRESSION_LENGTH - 1) {
                handle_special_functions(&key, expression, &expr_index);
                if (key != 0) {  // Only add the key if it wasn't handled by a special function
                    expression[expr_index++] = key;
                    expression[expr_index] = '\0';
                    lcd_char(key);
                }
            }
            _delay_ms(300);  // Debounce
        }
    }

    return 0;
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

void handle_shift_mode(char *key, char *expression, int *expr_index) {
    // First, find which key was pressed in the normal keypad
    int row = -1, col = -1;
    
    for (int r = 0; r < 4 && row == -1; r++) {
        for (int c = 0; c < 5; c++) {
            if (pgm_read_byte(&keypad_normal[r][c]) == *key) {
                row = r;
                col = c;
                break;
            }
        }
    }
    
    // If found, handle the shift function
    if (row != -1 && col != -1) {
        const char* function = (const char*)pgm_read_word(&shiftFunctions[row][col]);
        
        // Handle the function based on its name
        if (strcmp_P(function, PSTR("asin")) == 0) {
            strcpy(buffer, "asin(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("acos")) == 0) {
            strcpy(buffer, "acos(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("atan")) == 0) {
            strcpy(buffer, "atan(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("y^x")) == 0) {
            *key = '^';
        } else if (strcmp_P(function, PSTR("CLR")) == 0) {
            lcd_clear();
            lcd_string("Enter expression:");
            lcd_command(LCD_SET_CURSOR | 0x40);
            *expr_index = 0;
            expression[*expr_index] = '\0';
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("10^")) == 0) {
            strcpy(buffer, "10^");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("e")) == 0) {
            strcpy(buffer, "2.71828");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("abs")) == 0) {
            strcpy(buffer, "abs(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("cbrt")) == 0) {
            strcpy(buffer, "cbrt(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("[")) == 0) {
            *key = '[';
        } else if (strcmp_P(function, PSTR("deg")) == 0) {
            strcpy(buffer, "deg(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("rad")) == 0) {
            strcpy(buffer, "rad(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("mod")) == 0) {
            *key = '%';
        } else if (strcmp_P(function, PSTR("fact")) == 0) {
            strcpy(buffer, "fact(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("]")) == 0) {
            *key = ']';
        } else if (strcmp_P(function, PSTR("HEX")) == 0 || 
                  strcmp_P(function, PSTR("DEC")) == 0 || 
                  strcmp_P(function, PSTR("BIN")) == 0 || 
                  strcmp_P(function, PSTR("OCT")) == 0) {
            // These would change the display mode, not implemented in this example
            *key = 0; // Don't add anything to expression
        } else if (strcmp_P(function, PSTR("MC")) == 0) {
            // Memory clear function
            memory = 0.0;
            lcd_clear();
            lcd_string("Memory cleared");
            _delay_ms(1000);
            lcd_clear();
            lcd_string("Enter expression:");
            lcd_command(LCD_SET_CURSOR | 0x40);
            lcd_string(expression);
            *key = 0; // Don't add anything to expression
        }
    }
    
    // Turn off shift mode after handling a key
    shift_mode = 0;
}

void handle_alpha_mode(char *key, char *expression, int *expr_index) {
    // First, find which key was pressed in the normal keypad
    int row = -1, col = -1;
    
    for (int r = 0; r < 4 && row == -1; r++) {
        for (int c = 0; c < 5; c++) {
            if (pgm_read_byte(&keypad_normal[r][c]) == *key) {
                row = r;
                col = c;
                break;
            }
        }
    }
    
    // If found, handle the alpha function
    if (row != -1 && col != -1) {
        const char* function = (const char*)pgm_read_word(&keypad_alpha[row][col]);
        
        // Handle the function based on its name
        if (strcmp_P(function, PSTR("sin")) == 0) {
            strcpy(buffer, "sin(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("cos")) == 0) {
            strcpy(buffer, "cos(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("tan")) == 0) {
            strcpy(buffer, "tan(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("^")) == 0) {
            *key = '^';
        } else if (strcmp_P(function, PSTR("BS")) == 0) {
            // Backspace functionality - handled in main
            *key = 'D'; // Use the existing delete key behavior
        } else if (strcmp_P(function, PSTR("log")) == 0) {
            strcpy(buffer, "log(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("ln")) == 0) {
            strcpy(buffer, "ln(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("e^")) == 0) {
            strcpy(buffer, "e^(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("√")) == 0) {
            strcpy(buffer, "sqrt(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("(")) == 0) {
            *key = '(';
        } else if (strcmp_P(function, PSTR("π")) == 0) {
            strcpy(buffer, "3.14159");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("x²")) == 0) {
            strcpy(buffer, "^2");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("x³")) == 0) {
            strcpy(buffer, "^3");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("1/x")) == 0) {
            strcpy(buffer, "1/(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR(")")) == 0) {
            *key = ')';
        } else if (strcmp_P(function, PSTR("EXP")) == 0) {
            *key = 'E';
        } else if (strcmp_P(function, PSTR("ANS")) == 0) {
            // Add last result to expression
            dtostrf(last_result, 8, 2, buffer);
            // Trim trailing zeros and decimal point if whole number
            char *p = buffer + strlen(buffer) - 1;
            while (*p == '0' && p > buffer) {
                *p-- = '\0';
            }
            if (*p == '.') *p = '\0';
            
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("M+")) == 0) {
            // Add expression to memory
            // Evaluate current expression and add to memory
            char temp_expr[MAX_EXPRESSION_LENGTH];
            strcpy(temp_expr, expression);
            float result;
            // Simplified evaluation for memory operations
            result = atof(temp_expr);
            memory += result;
            
            lcd_clear();
            lcd_string("Added to memory");
            _delay_ms(1000);
            lcd_clear();
            lcd_string("Enter expression:");
            lcd_command(LCD_SET_CURSOR | 0x40);
            lcd_string(expression);
            *key = 0; // Don't add anything to expression
        } else if (strcmp_P(function, PSTR("M-")) == 0) {
            // Subtract expression from memory
            // Evaluate current expression and subtract from memory
            char temp_expr[MAX_EXPRESSION_LENGTH];
            strcpy(temp_expr, expression);
            float result;
            // Simplified evaluation for memory operations
            result = atof(temp_expr);
            memory -= result;
            
            lcd_clear();
            lcd_string("Subtracted from mem");
            _delay_ms(1000);
            lcd_clear();
            lcd_string("Enter expression:");
            lcd_command(LCD_SET_CURSOR | 0x40);
            lcd_string(expression);
            *key = 0; // Don't add anything to expression
        } else if (strcmp_P(function, PSTR("MR")) == 0) {
            // Recall memory value
            dtostrf(memory, 8, 2, buffer);
            // Trim trailing zeros and decimal point if whole number
            char *p = buffer + strlen(buffer) - 1;
            while (*p == '0' && p > buffer) {
                *p-- = '\0';
            }
            if (*p == '.') *p = '\0';
            
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        }
    }
    
    // Turn off alpha mode after handling a key
    alpha_mode = 0;
}

void handle_special_functions(char *key, char *expression, int *expr_index) {
    // Check if we're in SHIFT or ALPHA mode
    if (shift_mode) {
        handle_shift_mode(key, expression, expr_index);
        return;
    } else if (alpha_mode) {
        handle_alpha_mode(key, expression, expr_index);
        return;
    }
    
    // For normal mode, handle some basic special functions
    switch (*key) {
        case 's':
            strcpy(buffer, "sin(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'c':
            strcpy(buffer, "cos(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 't':
            strcpy(buffer, "tan(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'p':
            strcpy(buffer, "3.14159");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'e':
            strcpy(buffer, "2.71828");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'l':
            strcpy(buffer, "log(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'r':
            strcpy(buffer, "sqrt(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
    }
}

double factorial(int n) {
    if (n <= 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Modify the evaluate_expression function to handle more functions
void evaluate_expression(char *expression) {
    // Implementation of BODMAS rule with added functions
    char *expr = expression;
    double result = 0;
    
    // Simple parser for basic expressions
    // In a real calculator, you'd want a more sophisticated parser
    
    // For this example, using a simplified approach
    // This handles basic operations and a few functions
    
    // Check for special cases first
    if (strncmp(expr, "sin(", 4) == 0) {
        expr += 4;  // Skip "sin("
        float angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = sin(angle * M_PI / 180.0);  // Convert to radians
    } else if (strncmp(expr, "cos(", 4) == 0) {
        expr += 4;  // Skip "cos("
        float angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = cos(angle * M_PI / 180.0);  // Convert to radians
    } else if (strncmp(expr, "tan(", 4) == 0) {
        expr += 4;  // Skip "tan("
        float angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = tan(angle * M_PI / 180.0);  // Convert to radians
    } else if (strncmp(expr, "asin(", 5) == 0) {
        expr += 5;  // Skip "asin("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = asin(value) * 180.0 / M_PI;  // Convert to degrees
    } else if (strncmp(expr, "acos(", 5) == 0) {
        expr += 5;  // Skip "acos("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = acos(value) * 180.0 / M_PI;  // Convert to degrees
    } else if (strncmp(expr, "atan(", 5) == 0) {
        expr += 5;  // Skip "atan("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = atan(value) * 180.0 / M_PI;  // Convert to degrees
    } else if (strncmp(expr, "sqrt(", 5) == 0) {
        expr += 5;  // Skip "sqrt("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = sqrt(value);
    } else if (strncmp(expr, "log(", 4) == 0) {
        expr += 4;  // Skip "log("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = log10(value);
    } else if (strncmp(expr, "ln(", 3) == 0) {
        expr += 3;  // Skip "ln("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = log(value);
    } else if (strncmp(expr, "e^(", 3) == 0) {
        expr += 3;  // Skip "e^("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = exp(value);
    } else if (strncmp(expr, "cbrt(", 5) == 0) {
        expr += 5;  // Skip "cbrt("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = cbrt(value);
    } else if (strncmp(expr, "abs(", 4) == 0) {
        expr += 4;  // Skip "abs("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = fabs(value);
    } else if (strncmp(expr, "fact(", 5) == 0) {
        expr += 5;  // Skip "fact("
        int value = atoi(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = factorial(value);
    } else if (strncmp(expr, "deg(", 4) == 0) {
        expr += 4;  // Skip "deg("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = value * 180.0 / M_PI;  // Convert radians to degrees
    } else if (strncmp(expr, "rad(", 4) == 0) {
        expr += 4;  // Skip "rad("
        float value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = value * M_PI / 180.0;  // Convert degrees to radians
    } else {
        // Parse basic arithmetic expression
        double a = parse_number(&expr);
        
        while (*expr) {
            char op = *expr++;
            
            // Skip whitespace
            while (*expr && isspace(*expr)) {
                expr++;
            }
            
            double b = parse_number(&expr);
            a = perform_operation(a, b, op);
        }
        
        result = a;
    }
    
    // Store the result for ANS function
    last_result = result;
    
    // Format and display the result
    dtostrf(result, 8, 4, buffer);
    
    // Trim trailing zeros and decimal point if whole number
    char *p = buffer + strlen(buffer) - 1;
    while (*p == '0' && p > buffer) {
        *p-- = '\0';
    }
    if (*p == '.') *p = '\0';
    
    lcd_string(buffer);
}

double parse_number(char **expr) {
    double num = 0.0;
    int decimals = 0;
    int decimal_places = 0;
    int negative = 0;
    
    // Skip whitespace
    while (**expr && isspace(**expr)) {
        (*expr)++;
    }
    
    // Check for negative number
    if (**expr == '-') {
        negative = 1;
        (*expr)++;
    }
    
    // Parse integer part
    while (**expr && isdigit(**expr)) {
        num = num * 10.0 + (**expr - '0');
        (*expr)++;
    }
    
    // Parse decimal part
    if (**expr == '.') {
        (*expr)++;
        while (**expr && isdigit(**expr)) {
            num = num * 10.0 + (**expr - '0');
            decimal_places++;
            (*expr)++;
        }
        decimals = 1;
    }
    
    // Apply decimal point
    if (decimals) {
        while (decimal_places > 0) {
            num /= 10.0;
            decimal_places--;
        }
    }
    
    // Apply sign
    if (negative) {
        num = -num;
    }
    
    // Handle scientific notation
    if (**expr == 'E' || **expr == 'e') {
        (*expr)++;
        
        // Get exponent sign
        int exp_negative = 0;
        if (**expr == '-') {
            exp_negative = 1;
            (*expr)++;
        } else if (**expr == '+') {
            (*expr)++;
        }
        
        // Parse exponent
        int exponent = 0;
        while (**expr && isdigit(**expr)) {
            exponent = exponent * 10 + (**expr - '0');
            (*expr)++;
        }
        
        // Apply exponent
        if (exp_negative) {
            while (exponent > 0) {
                num /= 10.0;
                exponent--;
            }
        } else {
            while (exponent > 0) {
                num *= 10.0;
                exponent--;
            }
        }
    }
    
    return num;
}

double perform_operation(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b);
        case '%': return fmod(a, b);
        default: return b;  // For unknown operations, just return the second operand
    }
}

void lcd_init(void) {
    // Set LCD pins as output
    SetBit(RS_DDR, RS_PIN);
    SetBit(EN_DDR, EN_PIN);
    SetBit(D4_DDR, D4_PIN);
    SetBit(D5_DDR, D5_PIN);
    SetBit(D6_DDR, D6_PIN);
    SetBit(D7_DDR, D7_PIN);
    
    // Wait for LCD to power up
    _delay_ms(50);
    
    // Initialize in 4-bit mode
    // First send 0x03 three times (recommended by datasheet)
    ClearBit(RS_PORT, RS_PIN);  // RS = 0 for command
    
    // Send 0x03 first time
    ClearBit(D4_PORT, D4_PIN);
    ClearBit(D5_PORT, D5_PIN);
    SetBit(D6_PORT, D6_PIN);
    SetBit(D7_PORT, D7_PIN);
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(5);
    
    // Send 0x03 second time
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_us(100);
    
    // Send 0x03 third time
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_us(100);
    
    // Now set to 4-bit interface
    ClearBit(D7_PORT, D7_PIN);  // D7 = 0
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_us(100);
    
    // Now we're in 4-bit mode, send function set command
    lcd_command(LCD_FUNCTION_4BIT_2LINE);
    lcd_command(LCD_DISPLAY_ON);
    lcd_command(LCD_CLEAR);
    lcd_command(LCD_ENTRY_MODE);
}

void lcd_command(unsigned char cmd) {
    // RS = 0 for command
    ClearBit(RS_PORT, RS_PIN);
    
    // Send high nibble
    (cmd & 0x80) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (cmd & 0x40) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (cmd & 0x20) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (cmd & 0x10) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    // Wait a bit
    _delay_us(100);
    
    // Send low nibble
    (cmd & 0x08) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (cmd & 0x04) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (cmd & 0x02) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (cmd & 0x01) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    // Wait for command to execute
    _delay_ms(2);
}

void lcd_char(unsigned char data) {
    // RS = 1 for data
    SetBit(RS_PORT, RS_PIN);
    
    // Send high nibble
    (data & 0x80) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (data & 0x40) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (data & 0x20) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (data & 0x10) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    // Wait a bit
    _delay_us(100);
    
    // Send low nibble
    (data & 0x08) ? SetBit(D7_PORT, D7_PIN) : ClearBit(D7_PORT, D7_PIN);
    (data & 0x04) ? SetBit(D6_PORT, D6_PIN) : ClearBit(D6_PORT, D6_PIN);
    (data & 0x02) ? SetBit(D5_PORT, D5_PIN) : ClearBit(D5_PORT, D5_PIN);
    (data & 0x01) ? SetBit(D4_PORT, D4_PIN) : ClearBit(D4_PORT, D4_PIN);
    
    // Enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    
    // Wait for character to be displayed
    _delay_ms(1);
}

void lcd_string(char *str) {
    while (*str) {
        lcd_char(*str++);
    }
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
    _delay_ms(2);  // Clear command takes longer
}

void keypad_init(void) {
    // Set row pins as output
    SetBit(ROW1_DDR, ROW1_PIN);
    SetBit(ROW2_DDR, ROW2_PIN);
    SetBit(ROW3_DDR, ROW3_PIN);
    SetBit(ROW4_DDR, ROW4_PIN);
    
    // Set row pins HIGH
    SetBit(ROW1_PORT, ROW1_PIN);
    SetBit(ROW2_PORT, ROW2_PIN);
    SetBit(ROW3_PORT, ROW3_PIN);
    SetBit(ROW4_PORT, ROW4_PIN);
    
    // Set column pins as input with pull-up
    ClearBit(COL1_DDR, COL1_PIN);
    ClearBit(COL2_DDR, COL2_PIN);
    ClearBit(COL3_DDR, COL3_PIN);
    ClearBit(COL4_DDR, COL4_PIN);
    ClearBit(COL5_DDR, COL5_PIN);
    
    // Enable pull-up resistors on column pins
    SetBit(COL1_PORT, COL1_PIN);
    SetBit(COL2_PORT, COL2_PIN);
    SetBit(COL3_PORT, COL3_PIN);
    SetBit(COL4_PORT, COL4_PIN);
    SetBit(COL5_PORT, COL5_PIN);
}
