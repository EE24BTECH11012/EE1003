#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// BCD Output Pins
#define A PD2
#define B PD3
#define C PD4
#define D PD5

// Common Display Pins
#define H1 PD6
#define H2 PD7
#define M1 PB0
#define M2 PB1
#define S1 PB2
#define S2 PB3

// Button Pins
#define SET_HOUR PC1
#define SET_MIN PC2
#define SET_SEC PC0
#define RESET_BTN PC3  // Reset button pin
#define ALARM_BTN PC4  // Alarm setting button

// Buzzer pin
#define BUZZER_PIN PB4   // Buzzer output pin

// Global BCD digits for the clock
volatile uint8_t h1 = 0;  // tens of hours
volatile uint8_t h2 = 0;  // ones of hours
volatile uint8_t m1 = 0;  // tens of minutes
volatile uint8_t m2 = 0;  // ones of minutes
volatile uint8_t s1 = 0;  // tens of seconds
volatile uint8_t s2 = 0;  // ones of seconds

// Alarm time storage
volatile uint8_t alarm_h1 = 0;  // tens of hours for alarm
volatile uint8_t alarm_h2 = 0;  // ones of hours for alarm
volatile uint8_t alarm_m1 = 0;  // tens of minutes for alarm
volatile uint8_t alarm_m2 = 0;  // ones of minutes for alarm
volatile uint8_t alarm_enabled = 0;  // alarm on/off status
volatile uint8_t alarm_setting_mode = 0; // 0=off, 1=hours, 2=minutes
volatile uint8_t alarm_triggered = 0;    // Tracks if alarm is currently sounding

// Time direction (0 = forward, 1 = backward)
volatile uint8_t time_direction = 0;

// Multiplexing control variables
uint8_t current_digit = 0;
volatile uint32_t millis_count = 0;
const uint8_t mux_interval = 2; // 2ms per digit refresh

// Timekeeping control
volatile uint32_t last_second = 0;

// Button debouncing
volatile uint32_t last_button_check = 0;
const uint16_t debounce_interval = 200;

// Reset button long-press detection
volatile uint32_t reset_press_start = 0;
volatile uint8_t reset_button_state = 0;
const uint32_t reset_time_threshold = 1500;     // 1.5 seconds for normal reset
const uint32_t backward_time_threshold = 10000; // 10 seconds for backward time

// Initialize timer for millisecond counting
void init_timer0() {
    // Set Timer0 to CTC mode with prescaler 64
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS01) | (1 << CS00);
    
    // Set compare value for 1ms at 16MHz
    OCR0A = 249;
    
    // Enable Timer0 compare match interrupt
    TIMSK0 |= (1 << OCIE0A);
    
    // Enable global interrupts
    sei();
}

// Timer0 interrupt service routine - increment millisecond counter
ISR(TIMER0_COMPA_vect) {
    millis_count++;
}

// Get milliseconds elapsed
uint32_t millis() {
    uint32_t ms;
    cli();  // Disable interrupts for atomic read
    ms = millis_count;
    sei();  // Re-enable interrupts
    return ms;
}

/*
  bcdIncrement()
  Increments a BCD digit by one using bit‐wise operations.
  If the digit equals max, it rolls over to 0.
*/
uint8_t bcdIncrement(uint8_t bcd, uint8_t max) {
    if (bcd == max) {
        return 0;
    }
    uint8_t d0 = bcd & 0x01;
    uint8_t d1 = (bcd >> 1) & 0x01;
    uint8_t d2 = (bcd >> 2) & 0x01;
    uint8_t d3 = (bcd >> 3) & 0x01;
    
    uint8_t n0 = !d0;
    uint8_t c0 = d0;
    uint8_t n1 = d1 ^ c0;
    uint8_t c1 = d1 & c0;
    uint8_t n2 = d2 ^ c1;
    uint8_t c2 = d2 & c1;
    uint8_t n3 = d3 ^ c2;
    
    uint8_t result = (n3 << 3) | (n2 << 2) | (n1 << 1) | n0;
    return result;
}

/*
  bcdDecrement()
  Decrements a BCD digit by one using bit‐wise operations.
  If the digit equals 0, it rolls over to max.
*/
uint8_t bcdDecrement(uint8_t bcd, uint8_t max) {
    if (bcd == 0) {
        return max;
    }
    
    uint8_t d0 = bcd & 0x01;
    uint8_t d1 = (bcd >> 1) & 0x01;
    uint8_t d2 = (bcd >> 2) & 0x01;
    uint8_t d3 = (bcd >> 3) & 0x01;
    
    uint8_t b0 = !d0;
    uint8_t b1 = d1;
    uint8_t b2 = d2;
    uint8_t b3 = d3;
    
    if (b0) {
        b1 = !b1;
        if (b1) {
            b2 = !b2;
            if (b2) {
                b3 = !b3;
            }
        }
    }
    
    uint8_t result = (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
    return result;
}

// Initialize buzzer pin
void init_buzzer() {
    // Configure buzzer pin as output
    DDRB |= (1 << BUZZER_PIN);
    // Ensure buzzer is off initially
    PORTB &= ~(1 << BUZZER_PIN);
}

// Activate buzzer
void buzzer_on() {
    PORTB |= (1 << BUZZER_PIN);
}

// Deactivate buzzer
void buzzer_off() {
    PORTB &= ~(1 << BUZZER_PIN);
}

/*
  reset_time()
  Resets all time digits to zero.
*/
void reset_time() {
    cli();  // Disable interrupts for atomic operation
    
    // Reset all time digits to zero
    h1 = 0;
    h2 = 0;
    m1 = 0;
    m2 = 0;
    s1 = 0;
    s2 = 0;
    
    // Reset second counter to prevent immediate increment
    last_second = millis();
    
    sei();  // Re-enable interrupts
}

/*
  toggle_time_direction()
  Toggles between forward and backward time.
*/
void toggle_time_direction() {
    cli();  // Disable interrupts
    time_direction = !time_direction;  // Toggle direction
    sei();  // Re-enable interrupts
}

/*
  updateSeconds()
  Updates seconds based on current time direction.
*/
void updateSeconds() {
    if (time_direction == 0) {
        // Forward time
        if (s2 == 9) {
            s2 = 0;
            if (s1 == 5) {
                s1 = 0;
            } else {
                s1++;
            }
        } else {
            s2++;
        }
    } else {
        // Backward time
        if (s2 == 0) {
            s2 = 9;
            if (s1 == 0) {
                s1 = 5;
            } else {
                s1--;
            }
        } else {
            s2--;
        }
    }
}

/*
  updateMinutes()
  Updates minutes based on current time direction.
*/
void updateMinutes() {
    if (time_direction == 0) {
        // Forward time
        if (m2 == 9) {
            m2 = 0;
            if (m1 == 5) {
                m1 = 0;
            } else {
                m1++;
            }
        } else {
            m2++;
        }
    } else {
        // Backward time
        if (m2 == 0) {
            m2 = 9;
            if (m1 == 0) {
                m1 = 5;
            } else {
                m1--;
            }
        } else {
            m2--;
        }
    }
}

/*
  updateHours()
  Updates hours based on current time direction.
*/
void updateHours() {
    if (time_direction == 0) {
        // Forward time
        if (h1 == 2 && h2 == 3) {
            h1 = 0;
            h2 = 0;
        } else if (h2 == 9) {
            h2 = 0;
            h1++;
        } else {
            h2++;
        }
    } else {
        // Backward time
        if (h1 == 0 && h2 == 0) {
            h1 = 2;
            h2 = 3;
        } else if (h2 == 0) {
            if (h1 > 0) {
                h1--;
                h2 = 9;
            }
        } else {
            h2--;
        }
    }
}

/*
  updateTime()
  Updates the time every second based on time direction.
*/
void updateTime() {
    if (millis() - last_second >= 1000) {
        last_second += 1000;
        
        updateSeconds();
        if ((time_direction == 0 && s1 == 0 && s2 == 0) || 
            (time_direction == 1 && s1 == 5 && s2 == 9)) {
            
            updateMinutes();
            if ((time_direction == 0 && m1 == 0 && m2 == 0) || 
                (time_direction == 1 && m1 == 5 && m2 == 9)) {
                
                updateHours();
            }
        }
    }
}

/*
  check_alarm()
  Checks if current time matches alarm time and triggers buzzer.
*/
void check_alarm() {
    if (alarm_enabled && !alarm_triggered) {
        // Compare current time with alarm time
        if (h1 == alarm_h1 && h2 == alarm_h2 && 
            m1 == alarm_m1 && m2 == alarm_m2 && 
            s1 == 0 && s2 == 0) {
            
            // Alarm matches! Start alarm
            alarm_triggered = 1;
            buzzer_on();
        }
    }
    
    // If alarm is triggered, handle it
    if (alarm_triggered) {
        static uint32_t alarm_start_time = 0;
        
        // Initialize alarm start time
        if (alarm_start_time == 0) {
            alarm_start_time = millis();
        }
        
        // Check if any button is pressed to stop alarm
        if (!(PINC & ((1 << SET_HOUR) | (1 << SET_MIN) | 
                       (1 << SET_SEC) | (1 << ALARM_BTN) | (1 << RESET_BTN)))) {
            // Button pressed, stop alarm
            buzzer_off();
            alarm_triggered = 0;
            alarm_start_time = 0;
        }
        
        // Check if 30 seconds elapsed (automatic turn off)
        if (millis() - alarm_start_time > 30000) {
            buzzer_off();
            alarm_triggered = 0;
            alarm_start_time = 0;
        }
    }
}

/*
  set_alarm()
  Handles alarm setting mode and button presses.
*/
void set_alarm() {
    static uint32_t button_hold_time = 0;
    static uint8_t button_state = 0;
    
    // Check if alarm button is pressed
    if (!(PINC & (1 << ALARM_BTN))) {
        if (button_state == 0) {
            button_hold_time = millis();
            button_state = 1;
        } 
        else if (button_state == 1 && (millis() - button_hold_time > 1500)) {
            // Long press toggles alarm on/off
            alarm_enabled = !alarm_enabled;
            
            // Visual feedback for alarm toggle
            for (uint8_t i = 0; i < 3; i++) {
                PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
                _delay_ms(100);
                PORTD |= ((alarm_enabled ? (1 << A) : 0) | 
                          (alarm_enabled ? (1 << D) : 0));
                _delay_ms(100);
            }
            
            button_state = 2;
        }
    } 
    else {
        if (button_state == 1 && (millis() - button_hold_time < 1500)) {
            // Short press cycles through setting modes
            alarm_setting_mode = (alarm_setting_mode + 1) % 3;
        }
        button_state = 0;
    }
    
    // In setting mode, use hour/minute buttons to adjust alarm time
    if (alarm_setting_mode > 0 && millis() - last_button_check > debounce_interval) {
        if (alarm_setting_mode == 1) {  // Hours setting
            if (!(PINC & (1 << SET_HOUR))) {
                // Increment hours
                if (alarm_h1 == 2 && alarm_h2 == 3) {
                    alarm_h1 = 0;
                    alarm_h2 = 0;
                } else if (alarm_h2 == 9) {
                    alarm_h2 = 0;
                    alarm_h1++;
                } else {
                    alarm_h2++;
                }
                last_button_check = millis();
            }
        } 
        else if (alarm_setting_mode == 2) {  // Minutes setting
            if (!(PINC & (1 << SET_MIN))) {
                // Increment minutes
                if (alarm_m2 == 9) {
                    alarm_m2 = 0;
                    if (alarm_m1 == 5) {
                        alarm_m1 = 0;
                    } else {
                        alarm_m1++;
                    }
                } else {
                    alarm_m2++;
                }
                last_button_check = millis();
            }
        }
    }
}

    static uint32_t button_hold_time = 0;
    
    // Check if alarm button is pressed
    if (!(PINC & (1 << ALARM_BTN))) {
        if (button_hold_time == 0) {
            button_hold_time = millis();
        } 
        else if (millis() - button_hold_time > 1500) {
            // Long press toggles alarm on/off
            alarm_enabled = !alarm_enabled;
            
            // Visual feedback for alarm toggle
            for (uint8_t i = 0; i < 3; i++) {
                PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
                _delay_ms(100);
                PORTD |= ((alarm_enabled ? (1 << A) : 0) | 
                          (alarm_enabled ? (1 << D) : 0));
                _delay_ms(100);
            }
            
            button_hold_time = millis();
        }
    } 
    else if (button_hold_time > 0) {
        // Short press cycles through setting modes
        if (millis() - button_hold_time < 1500) {
            alarm_setting_mode = (alarm_setting_mode + 1) % 3;
        }
        button_hold_time = 0;
    }
    
    // In setting mode, use hour/minute buttons to adjust alarm time
    if (alarm_setting_mode > 0 && millis() - last_button_check > debounce_interval) {
        if (alarm_setting_mode == 1) {  // Hours setting
            if (!(PINC & (1 << SET_HOUR))) {
                // Increment hours
                if (alarm_h1 == 2 && alarm_h2 == 3) {
                    alarm_h1 = 0;
                    alarm_h2 = 0;
                } else if (alarm_h2 == 9) {
                    alarm_h2 = 0;
                    alarm_h1++;
                } else {
                    alarm_h2++;
                }
                last_button_check = millis();
            }
        } 
        else if (alarm_setting_mode == 2) {  // Minutes setting
            if (!(PINC & (1 << SET_MIN))) {
                // Increment minutes
                if (alarm_m2 == 9) {
                    alarm_m2 = 0;
                    if (alarm_m1 == 5) {
                        alarm_m1 = 0;
                    } else {
                        alarm_m1++;
                    }
                } else {
                    alarm_m2++;
                }
                last_button_check = millis();
            }
        }
    }

/*
  check_reset_button()
  Checks for reset button press and handles long press detection.
*/
void check_reset_button() {
    // Read button state (active low)
    uint8_t button_pressed = !(PINC & (1 << RESET_BTN));
    
    if (button_pressed) {
        // Button is pressed
        if (reset_button_state == 0) {
            // First detection of press
            reset_press_start = millis();
            reset_button_state = 1;
        } else if (reset_button_state == 1) {
            // Continuing to hold
            uint32_t press_duration = millis() - reset_press_start;
            
            // Check for backward time threshold (10 seconds)
            if (press_duration >= backward_time_threshold) {
                toggle_time_direction();
                
                // Visual feedback for direction change
                for (uint8_t i = 0; i < 5; i++) {
                    // Flash all segments
                    PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
                    _delay_ms(100);
                    
                    if (time_direction) {
                        // Show backward pattern (opposite segments)
                        PORTD |= ((1 << A) | (1 << C));
                    } else {
                        // Show forward pattern (all segments)
                        PORTD |= ((1 << A) | (1 << B) | (1 << C) | (1 << D));
                    }
                    _delay_ms(100);
                }
                
                reset_button_state = 3;  // Mark as completed
            }
            // Check for reset threshold (1.5 seconds)
            else if (press_duration >= reset_time_threshold && reset_button_state == 1) {
                reset_time();
                
                // Visual feedback for reset
                for (uint8_t i = 0; i < 3; i++) {
                    PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
                    _delay_ms(100);
                    PORTD |= ((1 << A) | (1 << B) | (1 << C) | (1 << D));
                    _delay_ms(100);
                }
                
                reset_button_state = 2;  // Mark as reset completed
            }
        }
    } else {
        // Button is released
        reset_button_state = 0;
    }
}

/*
  set_time()
  Handles button presses for time setting.
*/
void set_time() {
    if (millis() - last_button_check > debounce_interval) {
        // Only process time setting if not in alarm setting mode
        if (alarm_setting_mode == 0) {
            // Hours button
            if (!(PINC & (1 << SET_HOUR))) {
                updateHours();
                last_button_check = millis();
            }
            // Minutes button
            if (!(PINC & (1 << SET_MIN))) {
                updateMinutes();
                last_button_check = millis();
            }
            // Seconds button
            if (!(PINC & (1 << SET_SEC))) {
                updateSeconds();
                last_button_check = millis();
            }
        }
    }
}

/*
  displayDigit()
  Sends a 4-bit BCD digit to the output pins and activates the common pin.
*/
void displayDigit(uint8_t digit, uint8_t position) {
    // Clear all common pins first
    PORTD &= ~((1 << H1) | (1 << H2));
    PORTB &= ~((1 << M1) | (1 << M2) | (1 << S1) | (1 << S2));
    
    // Blank display if digit is 0x0F (used for blinking)
    if (digit != 0x0F) {
        // Set BCD value
        PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
        PORTD |= ((digit & 0x01) << A) |
                 ((digit & 0x02) << (B-1)) |
                 ((digit & 0x04) << (C-2)) |
                 ((digit & 0x08) << (D-3));
    } else {
        // Turn off all segments
        PORTD &= ~((1 << A) | (1 << B) | (1 << C) | (1 << D));
    }
    
    // Activate the correct common pin
    switch (position) {
        case 0: // H1
            PORTD |= (1 << H1);
            break;
        case 1: // H2
            PORTD |= (1 << H2);
            break;
        case 2: // M1
            PORTB |= (1 << M1);
            break;
        case 3: // M2
            PORTB |= (1 << M2);
            break;
        case 4: // S1
            PORTB |= (1 << S1);
            break;
        case 5: // S2
            PORTB |= (1 << S2);
            break;
    }
    
    _delay_ms(mux_interval);
}

/*
  multiplexDisplay()
  Cycles through the 6 digits based on current mode.
*/
void multiplexDisplay() {
    uint8_t digits[6];
    
    if (alarm_setting_mode == 0) {
        // Normal time display
        digits[0] = h1;
        digits[1] = h2;
        digits[2] = m1;
        digits[3] = m2;
        digits[4] = s1;
        digits[5] = s2;
        
        // If alarm is enabled, blink the colon (S1) periodically
        if (alarm_enabled && (millis() % 2000 < 1000)) {
            digits[4] = 0x0F;  // Blank S1 to blink the alarm indicator
        }
    } 
    else if (alarm_setting_mode == 1) {
        // Alarm hour setting - flash hours
        digits[0] = (millis() % 800 < 400) ? alarm_h1 : 0x0F;
        digits[1] = (millis() % 800 < 400) ? alarm_h2 : 0x0F;
        digits[2] = alarm_m1;
        digits[3] = alarm_m2;
        digits[4] = 0;
        digits[5] = 0;
    } 
    else {
        // Alarm minute setting - flash minutes
        digits[0] = alarm_h1;
        digits[1] = alarm_h2;
        digits[2] = (millis() % 800 < 400) ? alarm_m1 : 0x0F;
        digits[3] = (millis() % 800 < 400) ? alarm_m2 : 0x0F;
        digits[4] = 0;
        digits[5] = 0;
    }
    
    // Display current digit and advance to next
    displayDigit(digits[current_digit], current_digit);
    current_digit = (current_digit + 1) % 6;
}

void setup() {
    // Configure data pins as outputs
    DDRD |= (1 << A) | (1 << B) | (1 << C) | (1 << D) | (1 << H1) | (1 << H2);
    DDRB |= (1 << M1) | (1 << M2) | (1 << S1) | (1 << S2) | (1 << BUZZER_PIN);
    
    // Configure button pins as inputs with pull-ups
    DDRC &= ~((1 << SET_HOUR) | (1 << SET_MIN) | (1 << SET_SEC) | (1 << RESET_BTN) | (1 << ALARM_BTN));
    PORTC |= (1 << SET_HOUR) | (1 << SET_MIN) | (1 << SET_SEC) | (1 << RESET_BTN) | (1 << ALARM_BTN);
    
    // Initialize timer for millisecond counting
    init_timer0();
    
    // Ensure buzzer is off initially
    PORTB &= ~(1 << BUZZER_PIN);
    
    // Reset all values
    last_second = millis();
    last_button_check = millis();
}

int main(void) {
    setup();
    
    while (1) {
        set_time();
        set_alarm();
        check_reset_button();
        check_alarm();
        updateTime();
        multiplexDisplay();
    }
    
    return 0;
}