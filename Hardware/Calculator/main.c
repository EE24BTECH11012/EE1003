#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

# define h 0.01
# define PI 3.14159265359
# define E 2.7182818
 

double rk4_1(double x, double y, double x0, double (*function)(double, double))
{
    double k1, k2, k3, k4;
    while ((x - x0 > 1e-5) || (x - x0 < -1e-5))
    {
        k1 = h*function(x, y);
        k2 = h*function(x + h/2, y + k1/2);
        k3 = h*function(x + h/2, y + k2/2);
        k4 = h*function(x + h, y + k3);
        y += (k1 + 2*k2 + 2*k3 + k4)/6;
        x += h;
    }
    return y;
}

double rk4_2(double x, double y1, double y2, double x0, 
              double (*f1)(double, double, double), 
              double (*f2)(double, double, double)) 
{
    double k1_1, k2_1, k3_1, k4_1;
    double k1_2, k2_2, k3_2, k4_2;
    while ((x - x0) > 1e-3 || (x - x0) < -1e-3) 
    {
        k1_1 = h * f1(x, y1, y2);
        k1_2 = h * f2(x, y1, y2);
        k2_1 = h * f1(x + h / 2, y1 + k1_1 / 2, y2 + k1_2 / 2);
        k2_2 = h * f2(x + h / 2, y1 + k1_1 / 2, y2 + k1_2 / 2);
        k3_1 = h * f1(x + h / 2, y1 + k2_1 / 2, y2 + k2_2 / 2);
        k3_2 = h * f2(x + h / 2, y1 + k2_1 / 2, y2 + k2_2 / 2);
        k4_1 = h * f1(x + h, y1 + k3_1, y2 + k3_2);
        k4_2 = h * f2(x + h, y1 + k3_1, y2 + k3_2);
        y1 += (k1_1 + 2*k2_1 + 2*k3_1 + k4_1) / 6;
        y2 += (k1_2 + 2*k2_2 + 2*k3_2 + k4_2) / 6;
        x += h;
    }
    return y1;  // Return the sine value at x = x0
}
double fast_inv_sqrt(double x){
    if(x <= 0) return 0;
    x = (float) x;
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return (double) y;
}

double compute_scientific_functions(double value, int mode)
{
    // Function to calculate e^x
    if (mode == 1)
    {
        double function(double x, double y) { return y; }
        return rk4_1(0, 1, value, function);
    }
    // Function to calculate sin(x)
    else if (mode == 2)
    {
        double f1(double x, double y1, double y2) { return y2; }
        double f2(double x, double y1, double y2) { return -y1; }
        if (value == 0) { return 0; }
        else if (value > 0) { return rk4_2(0, 0, 1, value, f1, f2); }  // Fixed initial condition y2 = 1
        else if (value < 0) { return -1*rk4_2(0, 0, 1, -value, f1, f2); }
    }
    // Function to calculate cos(x)
    else if (mode == 3)
    {
        double f1(double x, double y1, double y2) { return y2; }
        double f2(double x, double y1, double y2) { return -y1; }
        if (value == 0) { return 1; }
        else if (value > 0) { return rk4_2(0, 1, 0, value, f1, f2); }
        else if (value < 0) { return rk4_2(0, 1, 0, -value, f1, f2); }
    }
    // Function to calculate tan(x)
    else if (mode == 4)
    {
        double cosval = compute_scientific_functions(value, 3);
        if (cosval == 0) { 
            printf("Infinity"); 
            return 0;  // Added return value for error case
        }
        else { 
            return compute_scientific_functions(value, 2)/cosval; 
        }
    }
    // Function to calculate ln(x)
else if (mode == 5)
{
    if (value <= 0) return 0.0;
    if (value < 1) return -compute_scientific_functions(1/value, 5);
    
    double x0 = 1.0, y = 0.0;
    int steps = (int)((value - x0) / h);
    
    for (int i = 0; i < steps; i++) {
        double k1 = h / x0;
        double k2 = h / (x0 + h/2);
        double k3 = h / (x0 + h/2);
        double k4 = h / (x0 + h);
        
        y += (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += h;
    }
    
    return y;
}
// Function to calculate log10(x)
else if (mode == 6)
{
    return 1/(2.3025) * compute_scientific_functions(value, 5) ;
}
    // Function to calculate sqrt(x)
    else if (mode == 7)
    {
        double function(double x, double y) { return 1/(2*y); }
        if (value < 0) { 
            printf("#"); 
            return 0;  // Added return value for error case
        }
        else if (value == 0) { return 0; }
        else { 
            // For sqrt, initial condition should start with a known point
            // e.g., sqrt(1) = 1, then integrate to the target value
            return rk4_1(1, 1, value, function);
        }
    }
    // Function to calculate sin⁻¹(x) (arcsin)
    if (mode == 8)
    {
       if (value < -1 || value > 1) return 0;

    double x0 = 0.0, y = 0.0;
    int steps = value >= 0 ? (int) (value / h): (int) (-value / h); 
    double step_dir = (value >= 0) ? 1 : -1;

    for (int i = 0; i < steps; i++) {
        double k1 = h * fast_inv_sqrt(1 - x0*x0);
        double k2 = h * fast_inv_sqrt(1 - (x0 + step_dir*h/2)*(x0 + step_dir*h/2));
        double k3 = h * fast_inv_sqrt(1 - (x0 + step_dir*h/2)*(x0 + step_dir*h/2));
        double k4 = h * fast_inv_sqrt(1 - (x0 + step_dir*h)*(x0 + step_dir*h));

        y += step_dir * (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += step_dir * h;
    }

    return y;
    }
    // Function to calculate acos(x)
    if ( mode == 9 )
    {
          return PI/2 - compute_scientific_functions(value, 8) ;
    }
    // Arctangent function tan-1(x)
    if (mode == 10 )
    {
        double function(double x, double y) { return 1.0 / (1 + x*x ) ; }
        return rk4_1(0, 0, value, function);
    }
    // Function to calculate \cbrt{x}
    if ( mode == 11 )
    {
          if (value == 0.0) return 0.0;
    
          int sign = (value > 0) ? 1 : -1;
          value = ( value > 0 ) ? value : -value ;
    
          // Initial guess - a good approximation speeds up convergence
    double guess = value / 3.0;
    
    // Newton's method iteration
    // Formula: x_n+1 = x_n - f(x_n)/f'(x_n)
    // For cube root: f(y) = y^3 - x, f'(y) = 3y^2
    // So iteration becomes: y = y - (y^3 - x)/(3y^2) = y - (y - x/y^2)/3
    
    for (int i = 0; i < 5; i++) {  // Usually converges in 3-5 iterations
        double y_cubed = guess * guess * guess;
        guess = guess - (y_cubed - value) / (3 * guess * guess);
    }
    
    return sign * guess;
    }
    
    return 0;  // Default return for invalid mode
}



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
char buffer[32];  // Increased buffer size for string operations

// Function prototypes
void lcd_init(void);
void lcd_command(unsigned char cmd);
void lcd_char(unsigned char data);
void lcd_string(const char *str);
void lcd_string_P(const char *str);
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
void write_string_to_buffer(const char *src, char *dest, int *index);

#define MAX_EXPRESSION_LENGTH 64  // Increased for complex expressions

int main(void) {
    char expression[MAX_EXPRESSION_LENGTH] = "";
    int expr_index = 0;

    lcd_init();
    keypad_init();

    lcd_string_P(PSTR("Welcome !"));
    _delay_ms(2000);
    lcd_clear();

    // lcd_string_P(PSTR("Enter expression:"));
    lcd_command(LCD_SET_CURSOR | 0x40);  // Move to second line

    while(1) {
        char key = keypad_scan();

        if (key != 0) {
            if (key == 'S') {
                shift_mode = !shift_mode;
                lcd_clear();
                lcd_string_P(shift_mode ? PSTR("Shift ON") : PSTR("Shift OFF"));
                _delay_ms(1000);
                lcd_clear();
                // lcd_string_P(PSTR("Enter expression:"));
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'A') {
                alpha_mode = !alpha_mode;
                lcd_clear();
                lcd_string_P(alpha_mode ? PSTR("Alpha mode ON") : PSTR("Alpha mode OFF"));
                _delay_ms(1000);
                lcd_clear();
                lcd_string_P(PSTR("Enter expression:"));
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'C') {
                lcd_clear();
                lcd_string_P(PSTR("Enter expression:"));
                lcd_command(LCD_SET_CURSOR | 0x40);
                expr_index = 0;
                expression[expr_index] = '\0';
            } else if (key == 'D' && expr_index > 0) {
                expr_index--;
                expression[expr_index] = '\0';
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string_P(PSTR("                "));  // Clear second line
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == '=') {
                expression[expr_index] = '\0';
                lcd_clear();
                lcd_string_P(PSTR("Result:"));
                lcd_command(LCD_SET_CURSOR | 0x40);
                evaluate_expression(expression);
                expr_index = 0;
                expression[expr_index] = '\0';
                _delay_ms(3000);
                lcd_clear();
                lcd_string_P(PSTR("Enter expression:"));
                lcd_command(LCD_SET_CURSOR | 0x40);
            } else if (expr_index < MAX_EXPRESSION_LENGTH - 1) {
                if (shift_mode) {
                    handle_shift_mode(&key, expression, &expr_index);
                } else if (alpha_mode) {
                    handle_alpha_mode(&key, expression, &expr_index);
                } else {
                    handle_special_functions(&key, expression, &expr_index);
                }
                
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
            strcpy_P(buffer, PSTR("asin("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("acos")) == 0) {
            strcpy_P(buffer, PSTR("acos("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("atan")) == 0) {
            strcpy_P(buffer, PSTR("atan("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("y^x")) == 0) {
            *key = '^';
        } else if (strcmp_P(function, PSTR("CLR")) == 0) {
            lcd_clear();
            lcd_string_P(PSTR("Enter expression:"));
            lcd_command(LCD_SET_CURSOR | 0x40);
            *expr_index = 0;
            expression[*expr_index] = '\0';
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("10^")) == 0) {
            strcpy_P(buffer, PSTR("10^"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("e")) == 0) {
            strcpy_P(buffer, PSTR("2.7182818"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("abs")) == 0) {
            strcpy_P(buffer, PSTR("abs("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("cbrt")) == 0) {
            strcpy_P(buffer, PSTR("cbrt("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("[")) == 0) {
            *key = '[';
        } else if (strcmp_P(function, PSTR("deg")) == 0) {
            strcpy_P(buffer, PSTR("deg("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("rad")) == 0) {
            strcpy_P(buffer, PSTR("rad("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("mod")) == 0) {
            *key = '%';
        } else if (strcmp_P(function, PSTR("fact")) == 0) {
            strcpy_P(buffer, PSTR("fact("));
            write_string_to_buffer(buffer, expression, expr_index);
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
            lcd_string_P(PSTR("Memory cleared"));
            _delay_ms(1000);
            lcd_clear();
            lcd_string_P(PSTR("Enter expression:"));
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
            strcpy_P(buffer, PSTR("sin("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("cos")) == 0) {
            strcpy_P(buffer, PSTR("cos("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("tan")) == 0) {
            strcpy_P(buffer, PSTR("tan("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("^")) == 0) {
            *key = '^';
        } else if (strcmp_P(function, PSTR("BS")) == 0) {
            // Backspace functionality - handled in main
            *key = 'D'; // Use the existing delete key behavior
        } else if (strcmp_P(function, PSTR("log")) == 0) {
            strcpy_P(buffer, PSTR("log("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("ln")) == 0) {
            strcpy_P(buffer, PSTR("ln("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("e^")) == 0) {
            strcpy_P(buffer, PSTR("e^("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("√")) == 0) {
            strcpy_P(buffer, PSTR("sqrt("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("(")) == 0) {
            *key = '(';
        } else if (strcmp_P(function, PSTR("π")) == 0) {
            strcpy_P(buffer, PSTR("3.14159"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("x²")) == 0) {
            strcpy_P(buffer, PSTR("^2"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("x³")) == 0) {
            strcpy_P(buffer, PSTR("^3"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        } else if (strcmp_P(function, PSTR("1/x")) == 0) {
            strcpy_P(buffer, PSTR("1/("));
            write_string_to_buffer(buffer, expression, expr_index);
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
            
            write_string_to_buffer(buffer, expression, expr_index);
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
            lcd_string_P(PSTR("Added to memory"));
            _delay_ms(1000);
            lcd_clear();
            lcd_string_P(PSTR("Enter expression:"));
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
            lcd_string_P(PSTR("Subtracted from mem"));
            _delay_ms(1000);
            lcd_clear();
            lcd_string_P(PSTR("Enter expression:"));
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
            
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
        }
    }
    
    // Turn off alpha mode after handling a key
    alpha_mode = 0;
}

void handle_special_functions(char *key, char *expression, int *expr_index) {
    // Handle some basic special functions for normal mode
    switch (*key) {
        case 's':
            strcpy_P(buffer, PSTR("sin("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'c':
            strcpy_P(buffer, PSTR("cos("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 't':
            strcpy_P(buffer, PSTR("tan("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'p':
            strcpy_P(buffer, PSTR("3.14159"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'e':
            strcpy_P(buffer, PSTR("2.71828"));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'l':
            strcpy_P(buffer, PSTR("log("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
        case 'r':
            strcpy_P(buffer, PSTR("sqrt("));
            write_string_to_buffer(buffer, expression, expr_index);
            lcd_string(buffer);
            *key = 0; // Don't add the key to expression
            break;
    }
}
// Function to calculate factorial
double factorial(int n) {
    if (n < 0) return NAN;  // Factorial undefined for negative numbers
    if (n <= 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Function to calculate power (x^y)
double power(float base, int exp) {
    double result = 1.0;

    // If the exponent is negative, take the reciprocal of the base
    if (exp < 0) {
        base = 1.0 / base;
        exp = -exp;
    }

    // Multiply the base 'exp' times
    for (int i = 0; i < exp; i++) {
        result *= base;
    }

    return result;
}

// Improved evaluate_expression function with better function handling
void evaluate_expression(char *expression) {
    // Implementation of BODMAS rule with added functions
    char *expr = expression;
    double result = 0;
    
    // Check for special cases first
    if (strncmp(expr, "sin(", 4) == 0) {
        expr += 4;  // Skip "sin("
        double angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = sin(angle * M_PI / 180.0);  // Convert to radians
        result = compute_scientific_functions(angle*PI/180.0, 2) ;
    } else if (strncmp(expr, "cos(", 4) == 0) {
        expr += 4;  // Skip "cos("
        double angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = cos(angle * M_PI / 180.0);  // Convert to radians
        result = compute_scientific_functions(angle*PI/180.0, 3) ;
    } else if (strncmp(expr, "tan(", 4) == 0) {
        expr += 4;  // Skip "tan("
        double angle = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = tan(angle * M_PI / 180.0);  // Convert to radians
        result = compute_scientific_functions(angle*PI/180.0, 4) ;
    } else if (strncmp(expr, "log(", 4) == 0) {
        expr += 4;  // Skip "log("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = log10(value);
        result = compute_scientific_functions(value, 6) ;
    } else if (strncmp(expr, "ln(", 3) == 0) {
        expr += 3;  // Skip "ln("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = log(value);
        result = compute_scientific_functions(value, 5) ;
    } else if (strncmp(expr, "sqrt(", 5) == 0) {
        expr += 5;  // Skip "sqrt("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = sqrt(value);
        result = compute_scientific_functions(value, 7) ;
    } else if (strncmp(expr, "asin(", 5) == 0) {
        expr += 5;  // Skip "asin("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = asin(value) * 180.0 / M_PI;  // Convert to degrees
        result = compute_scientific_functions(value, 8)*180/PI ;
    } else if (strncmp(expr, "acos(", 5) == 0) {
        expr += 5;  // Skip "acos("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = acos(value) * 180.0 / M_PI;  // Convert to degrees
        result = compute_scientific_functions(value, 9)*180/PI ;
    } else if (strncmp(expr, "atan(", 5) == 0) {
        expr += 5;  // Skip "atan("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = atan(value) * 180.0 / M_PI;  // Convert to degrees
        result = compute_scientific_functions(value, 10)*180/PI ;
    } else if (strncmp(expr, "e^(", 3) == 0) {
        expr += 3;  // Skip "e^("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = power(E, value);
        // result = compute_scientific_functions(value, 1) ;
    } else if (strncmp(expr, "10^", 3) == 0) {
        expr += 3;  // Skip "10^"
        double value = atof(expr);
        result = power(10.0, value);
    } else if (strncmp(expr, "abs(", 4) == 0) {
        expr += 4;  // Skip "abs("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = fabs(value);
        result = ( value > 0 ) ? value : -value ;
    } else if (strncmp(expr, "cbrt(", 5) == 0) {
        expr += 5;  // Skip "cbrt("
        double value = atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = cbrt(value);
        result = compute_scientific_functions(value, 11) ;
    } else if (strncmp(expr, "fact(", 5) == 0) {
        expr += 5;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = factorial(value);
    }  else if (strncmp(expr, "^3", 2) == 0) {
        expr += 2;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = factorial(value);
        result = power(value, 3) ;
    } else if (strncmp(expr, "^2", 2) == 0) {
        expr += 2;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = factorial(value);
        result = power(value, 2) ;
        }
        else if (strncmp(expr, "deg(", 4) == 0) {
        expr += 4;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = factorial(value);
        // result = power(value, 2) ;
        result = value*180/PI ;
        }
        else if (strncmp(expr, "rad(", 4) == 0) {
        expr += 4;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        // result = factorial(value);
        result = value*PI/180 ;
        }
        else if (strncmp(expr, "fact(", 5) == 0) {
        expr += 5;  // Skip "fact("
        int value = (int)atof(expr);
        // Find closing parenthesis
        while (*expr && *expr != ')') expr++;
        if (*expr == ')') expr++;
        
        result = factorial(value);
        }
    else {
        // For basic arithmetic expressions
        char op = '+'; // Start with addition
        double current_value = 0;
        
        while (*expr) {
            // Skip whitespace
            while (*expr && isspace(*expr)) expr++;
            
            // Parse number
            if (isdigit(*expr) || *expr == '.') {
                current_value = atof(expr);
                // Move to end of number
                while (*expr && (isdigit(*expr) || *expr == '.')) expr++;
            }
            
            // Perform operation
            if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/' || *expr == '^' || *expr == '%' || *expr == '\0') {
                switch (op) {
                    case '+':
                        result += current_value;
                        break;
                    case '-':
                        result -= current_value;
                        break;
                    case '*':
                        result *= current_value;
                        break;
                    case '/':
                        if (current_value != 0) {
                            result /= current_value;
                        } else {
                            lcd_string_P(PSTR("Div by zero error"));
                            return;
                        }
                        break;
                    case '^':
                        // result = pow(result, current_value);
                        result = power(result, current_value) ;
                        break;
                    case '%':
                        if (current_value != 0) {
                            result = (int)result % (int) current_value ; }
                        else {
                            lcd_string_P(PSTR("Mod by zero error"));
                            return;
                        }
                        break;
                }
                
                if (*expr) {
                    op = *expr;
                    expr++;
                }
            } else {
                // Skip any unrecognized character
                expr++;
            }
        }
    }
    
    // Store as last result
    last_result = result;
    
    // Format the result for display
    if (isnan(result)) {
        lcd_string_P(PSTR("Error"));
    } else if (isinf(result)) {
        lcd_string_P(PSTR("Infinity"));
    } else {
        dtostrf(result, 8, 4, buffer);
        
        // Trim trailing zeros and decimal point if whole number
        char *p = buffer + strlen(buffer) - 1;
        while (*p == '0' && p > buffer) {
            *p-- = '\0';
        }
        if (*p == '.') *p = '\0';
        
        lcd_string(buffer);
    }
}

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



