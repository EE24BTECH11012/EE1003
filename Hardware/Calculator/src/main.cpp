#include <LiquidCrystal.h>
#include <math.h>

// Initialize the LCD with the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Button matrix setup - 4 rows, 5 columns
const int rowPins[4] = {A0, A1, A2, A3}; // Rows connected to analog pins
const int colPins[5] = {8, 9, 10, 6, 7}; // Columns connected to digital pins

// Button layout in the matrix
// Primary functions (no modifiers)
const char* buttonLabels[4][5] = {
  {"7", "8", "9", "+", "DEL"},
  {"4", "5", "6", "-", "("},
  {"1", "2", "3", "*", ")"},
  {"0", ".", "=", "/", "C"}
};

// Alpha modifier functions
const char* alphaFunctions[4][5] = {
  {"sin", "cos", "tan", "^", "BS"},
  {"log", "ln", "e^", "√", "("},
  {"π", "x²", "x³", "1/x", ")"},
  {"EXP", "ANS", "M+", "M-", "MR"}
};

// Shift modifier functions
const char* shiftFunctions[4][5] = {
  {"asin", "acos", "atan", "y^x", "CLR"},
  {"10^", "e", "abs", "cbrt", "["},
  {"deg", "rad", "mod", "fact", "]"},
  {"HEX", "DEC", "BIN", "OCT", "MC"}
};

// Variables for calculator operation
String currentNumber = "";
String expression = "";
float firstOperand = 0;
float secondOperand = 0;
float result = 0;
float memoryValue = 0;
float ansValue = 0;
char operation = ' ';
bool newCalculation = true;
bool operationPressed = false;
bool alphaMode = false;
bool shiftMode = false;
String currentMode = ""; // For display
String displayExpression = ""; // Added to show the current expression on display
String currentFunction = ""; // Added to track current function being used

// Expression parsing state
int parenLevel = 0;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Calculator Ready");
  delay(1000);
  lcd.clear();
  
  // Initialize row pins as inputs with pullup resistors
  for (int r = 0; r < 4; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }
  
  // Initialize column pins as outputs
  for (int c = 0; c < 5; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH); // Set HIGH initially (inactive)
  }
}

void loop() {
  // Scan the button matrix
  for (int c = 0; c < 5; c++) {
    // Set current column to LOW (active)
    digitalWrite(colPins[c], LOW);
    
    // Check each row
    for (int r = 0; r < 4; r++) {
      // Check if button is pressed (LOW due to pull-up)
      if (digitalRead(rowPins[r]) == LOW) {
        String button = "";
        
        // Handle Alpha and Shift special cases
        if (r == 1 && c == 4) { // Alpha button (in place of "(" key)
          alphaMode = !alphaMode;
          // Turn off shift mode if alpha is pressed
          if (alphaMode) {
            shiftMode = false;
            currentMode = "ALPHA";
          } else {
            currentMode = "";
          }
          updateModeDisplay();
          delay(200);
        } 
        else if (r == 2 && c == 4) { // Shift button (in place of ")" key)
          shiftMode = !shiftMode;
          // Turn off alpha mode if shift is pressed
          if (shiftMode) {
            alphaMode = false;
            currentMode = "SHIFT";
          } else {
            currentMode = "";
          }
          updateModeDisplay();
          delay(200);
        }
        else {
          // Get the button function based on current mode
          if (alphaMode) {
            button = alphaFunctions[r][c];
            // Show which function was selected before processing
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Alpha: " + button);
            delay(500); // Brief delay to show the function
            
            alphaMode = false; // Reset after one use
            currentMode = "";
          } else if (shiftMode) {
            button = shiftFunctions[r][c];
            // Show which function was selected before processing
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Shift: " + button);
            delay(500); // Brief delay to show the function
            
            shiftMode = false; // Reset after one use
            currentMode = "";
          } else {
            button = buttonLabels[r][c];
          }
          
          processButton(button);
          updateModeDisplay();
          delay(200);
        }
        
        // Wait for button release
        while (digitalRead(rowPins[r]) == LOW) {
          delay(10);
        }
      }
    }
    
    // Set column back to HIGH (inactive)
    digitalWrite(colPins[c], HIGH);
  }
}

void processButton(String buttonValue) {
  // Store the current function for display purposes
  currentFunction = buttonValue;
  
  // Clear and reset
  if (buttonValue == "C" || buttonValue == "CLR") {
    clearCalculator();
    return;
  }
  
  // Delete/Backspace
  if (buttonValue == "DEL" || buttonValue == "BS") {
    if (currentNumber.length() > 0) {
      currentNumber = currentNumber.substring(0, currentNumber.length() - 1);
      updateDisplay();
    }
    return;
  }
  
  // Memory functions
  if (buttonValue == "M+") {
    if (currentNumber != "") {
      memoryValue += currentNumber.toFloat();
      lcd.setCursor(0, 0);
      lcd.print("M+: " + currentNumber);
      lcd.setCursor(0, 1);
      lcd.print("Memory = " + String(memoryValue));
      delay(800);
      updateDisplay();
    }
    return;
  }
  
  if (buttonValue == "M-") {
    if (currentNumber != "") {
      memoryValue -= currentNumber.toFloat();
      lcd.setCursor(0, 0);
      lcd.print("M-: " + currentNumber);
      lcd.setCursor(0, 1);
      lcd.print("Memory = " + String(memoryValue));
      delay(800);
      updateDisplay();
    }
    return;
  }
  
  if (buttonValue == "MR") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Memory Recall:");
    lcd.setCursor(0, 1);
    lcd.print(String(memoryValue));
    delay(800);
    currentNumber = String(memoryValue);
    updateDisplay();
    return;
  }
  
  if (buttonValue == "MC") {
    memoryValue = 0;
    lcd.setCursor(0, 0);
    lcd.print("Memory Cleared");
    delay(800);
    updateDisplay();
    return;
  }
  
  // Recall last answer
  if (buttonValue == "ANS") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Answer Recall:");
    lcd.setCursor(0, 1);
    lcd.print(String(ansValue));
    delay(800);
    currentNumber = String(ansValue);
    updateDisplay();
    return;
  }
  
  // Scientific functions
  if (buttonValue == "sin" || buttonValue == "cos" || buttonValue == "tan" || 
      buttonValue == "log" || buttonValue == "ln" || buttonValue == "√" ||
      buttonValue == "asin" || buttonValue == "acos" || buttonValue == "atan" ||
      buttonValue == "10^" || buttonValue == "cbrt" || buttonValue == "abs" ||
      buttonValue == "e^") {
    
    if (currentNumber != "") {
      float value = currentNumber.toFloat();
      
      // Display the function and input before calculating
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(buttonValue + "(" + currentNumber + ")");
      
      // Trigonometric functions
      if (buttonValue == "sin") {
        result = sin(value * PI / 180);
        // result = calculate_scientific_functions(1, value) ;
      }
      else if (buttonValue == "cos") {
        result = cos(value * PI / 180);
      }
      else if (buttonValue == "tan") {
        result = tan(value * PI / 180);
      }
      else if (buttonValue == "asin") {
        if (value >= -1 && value <= 1) {
          result = asin(value) * 180 / PI;
        } else {
          displayError("Domain Error");
          return;
        }
      }
      else if (buttonValue == "acos") {
        if (value >= -1 && value <= 1) {
          result = acos(value) * 180 / PI;
        } else {
          displayError("Domain Error");
          return;
        }
      }
      else if (buttonValue == "atan") {
        result = atan(value) * 180 / PI;
      }
      
      // Logarithmic functions
      else if (buttonValue == "log") {
        if (value > 0) {
          result = log10(value);
        } else {
          displayError("Log Error");
          return;
        }
      }
      else if (buttonValue == "ln") {
        if (value > 0) {
          result = log(value);
        } else {
          displayError("Log Error");
          return;
        }
      }
      else if (buttonValue == "10^") {
        result = pow(10, value);
      }
      else if (buttonValue == "e^") {
        result = exp(value);
      }
      
      // Root functions
      else if (buttonValue == "√") {
        if (value >= 0) {
          result = sqrt(value);
        } else {
          displayError("Root Error");
          return;
        }
      }
      else if (buttonValue == "cbrt") {
        result = cbrt(value);
      }
      
      // Other functions
      else if (buttonValue == "abs") {
        result = abs(value);
      }
      
      // Display the result after a short delay
      delay(800);
      lcd.setCursor(0, 1);
      lcd.print("= " + String(result));
      delay(1200);
      
      // Set up for next calculation
      currentNumber = String(result);
      ansValue = result;
      newCalculation = true;
      displayExpression = ""; // Reset display expression
      updateDisplay();
    } else {
      // If no number entered, show error message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(buttonValue + "(??)");
      lcd.setCursor(0, 1);
      lcd.print("Enter a number first");
      delay(1200);
      updateDisplay();
    }
    return;
  }
  
  // Constants
  if (buttonValue == "π") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Constant: π");
    lcd.setCursor(0, 1);
    lcd.print(String(PI));
    delay(800);
    currentNumber = String(PI);
    updateDisplay();
    return;
  }
  
  if (buttonValue == "e") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Constant: e");
    lcd.setCursor(0, 1);
    lcd.print(String(exp(1.0)));
    delay(800);
    currentNumber = String(exp(1.0));
    updateDisplay();
    return;
  }
  
  // Powers
  if (buttonValue == "x²") {
    if (currentNumber != "") {
      float value = currentNumber.toFloat();
      
      // Display the operation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(currentNumber + "^2");
      
      result = value * value;
      
      // Display the result after a short delay
      delay(800);
      lcd.setCursor(0, 1);
      lcd.print("= " + String(result));
      delay(1200);
      
      currentNumber = String(result);
      ansValue = result;
      newCalculation = true;
      displayExpression = ""; // Reset display expression
      updateDisplay();
    }
    return;
  }
  
  if (buttonValue == "x³") {
    if (currentNumber != "") {
      float value = currentNumber.toFloat();
      
      // Display the operation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(currentNumber + "^3");
      
      result = value * value * value;
      
      // Display the result after a short delay
      delay(800);
      lcd.setCursor(0, 1);
      lcd.print("= " + String(result));
      delay(1200);
      
      currentNumber = String(result);
      ansValue = result;
      newCalculation = true;
      displayExpression = ""; // Reset display expression
      updateDisplay();
    }
    return;
  }
  
  // Reciprocal
  if (buttonValue == "1/x") {
    if (currentNumber != "") {
      float value = currentNumber.toFloat();
      if (value != 0) {
        // Display the operation
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1/" + currentNumber);
        
        result = 1.0 / value;
        
        // Display the result after a short delay
        delay(800);
        lcd.setCursor(0, 1);
        lcd.print("= " + String(result));
        delay(1200);
        
        currentNumber = String(result);
        ansValue = result;
        newCalculation = true;
        displayExpression = ""; // Reset display expression
        updateDisplay();
      } else {
        displayError("Divide by Zero");
      }
    }
    return;
  }
  
  // Number base conversions
  if (buttonValue == "HEX" || buttonValue == "DEC" || buttonValue == "BIN" || buttonValue == "OCT") {
    if (currentNumber != "") {
      long value = currentNumber.toFloat();
      String result = "";
      
      // Display the operation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(currentNumber + " to " + buttonValue);
      
      if (buttonValue == "HEX") {
        char buffer[20];
        sprintf(buffer, "%lX", value);
        result = String(buffer);
      } else if (buttonValue == "BIN") {
        // Simple binary conversion
        long temp = value;
        if (temp < 0) temp = -temp; // Handle only positive for now
        
        if (temp == 0) result = "0";
        while (temp > 0) {
          result = String(temp % 2) + result;
          temp /= 2;
        }
        if (value < 0) result = "-" + result;
      } else if (buttonValue == "OCT") {
        char buffer[20];
        sprintf(buffer, "%lo", value);
        result = String(buffer);
      } else { // DEC is already the default
        result = String(value);
      }
      
      // Display the result after a short delay
      delay(800);
      lcd.setCursor(0, 1);
      lcd.print(result);
      delay(2000);
      updateDisplay();
    }
    return;
  }
  
  // Parentheses
  if (buttonValue == "(" || buttonValue == "[") {
    // Add a multiplication sign if there's a number before the parenthesis
    if (currentNumber != "") {
      processArithmeticOperation("*");
    }
    
    expression += "(";
    displayExpression += "("; // Add to display expression
    parenLevel++;
    
    // Update display to show the parenthesis
    lcd.clear();
    lcd.setCursor(0, 0);
    if (displayExpression.length() > 16) {
      lcd.print(displayExpression.substring(displayExpression.length() - 16));
    } else {
      lcd.print(displayExpression);
    }
    return;
  }
  
  if (buttonValue == ")" || buttonValue == "]") {
    if (parenLevel > 0) {
      if (currentNumber != "") {
        expression += currentNumber + ")";
        displayExpression += currentNumber + ")"; // Add to display expression
        currentNumber = "";
      } else {
        expression += ")";
        displayExpression += ")"; // Add to display expression
      }
      
      parenLevel--;
      
      // Update display
      lcd.clear();
      lcd.setCursor(0, 0);
      if (displayExpression.length() > 16) {
        lcd.print(displayExpression.substring(displayExpression.length() - 16));
      } else {
        lcd.print(displayExpression);
      }
    }
    return;
  }
  
  // Numeric buttons and decimal point
  if (isDigit(buttonValue[0]) || buttonValue == ".") {
    // Clear display if starting a new calculation
    if (newCalculation) {
      currentNumber = "";
      displayExpression = ""; // Reset display expression for new calculation
      newCalculation = false;
    }
    
    if (operationPressed) {
      operationPressed = false;
      currentNumber = ""; // Start new number entry after operation
    }
    
    // Add digit to current number if there's room
    if (currentNumber.length() < 16) {
      // For decimal point, check if it's already present
      if (buttonValue == "." && currentNumber.indexOf('.') != -1) {
        return; // Don't add multiple decimal points
      }
      
      // Initialize with 0 if starting with decimal point
      if (buttonValue == "." && currentNumber == "") {
        currentNumber = "0";
      }
      
      currentNumber += buttonValue;
      updateDisplay();
    }
  }
  // Operation buttons
  else if (buttonValue == "+" || buttonValue == "-" || buttonValue == "*" || buttonValue == "/" ||
           buttonValue == "^" || buttonValue == "y^x" || buttonValue == "mod" || buttonValue == "fact") {
    processArithmeticOperation(buttonValue);
  }
  // Equals button
  else if (buttonValue == "=") {
    // If we have an expression with parentheses
    if (expression != "" && currentNumber != "") {
      expression += currentNumber;
      
      // Basic evaluation (Note: This is a simplified approach. Proper expression
      // evaluation would need a parser and stack implementation)
      // For now, we'll just display the expression and result for demo purposes
      
      lcd.clear();
      lcd.setCursor(0, 0);
      if (expression.length() > 16) {
        lcd.print(expression.substring(expression.length() - 16));
      } else {
        lcd.print(expression);
      }
      
      // Here you would add real expression evaluation
      // For demo, we'll just use the simple operation already stored
      if (operation != ' ' && secondOperand == 0) {
        secondOperand = currentNumber.toFloat();
        calculateResult();
      }
      
      expression = "";
      displayExpression = ""; // Reset display expression
      parenLevel = 0;
      return;
    }
    
    // Standard operation
    if (currentNumber != "" && operation != ' ') {
      secondOperand = currentNumber.toFloat();
      calculateResult();
    }
  }
}

void processArithmeticOperation(String op) {
  // Store operand and operation
  if (currentNumber != "") {
    // If we're already building an expression with parentheses
    if (expression != "") {
      expression += currentNumber + op[0];
      displayExpression += currentNumber + op[0]; // Add to display expression
      currentNumber = "";
      
      lcd.clear();
      lcd.setCursor(0, 0);
      if (displayExpression.length() > 16) {
        lcd.print(displayExpression.substring(displayExpression.length() - 16));
      } else {
        lcd.print(displayExpression);
      }
      return;
    }
    
    firstOperand = currentNumber.toFloat();
    
    // Handle factorial specifically
    if (op == "fact") {
      int n = (int)firstOperand;
      if (n < 0) {
        displayError("Fact Error");
        return;
      }
      
      // Display the operation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String(n) + "!");
      
      float fact = 1;
      for (int i = 2; i <= n; i++) {
        fact *= i;
      }
      
      // Display the result after a short delay
      delay(800);
      lcd.setCursor(0, 1);
      lcd.print("= " + String(fact));
      delay(1200);
      
      currentNumber = String(fact);
      ansValue = fact;
      newCalculation = true;
      displayExpression = ""; // Reset display expression
      updateDisplay();
      return;
    }
    
    // For standard operations
    if (op == "^" || op == "y^x") {
      operation = '^';
    } else if (op == "mod") {
      operation = '%';
    } else {
      operation = op[0];
    }
    
    // Update the display expression with the current operation
    displayExpression = currentNumber + " " + operation + " ";
    
    operationPressed = true;
    newCalculation = false; // Changed to false to maintain display through next number
    
    // Update display to show operation
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(displayExpression);
  } else if (op == "-") {
    // Allow negative numbers if current number is empty
    currentNumber = "-";
    updateDisplay();
  }
}

void calculateResult() {
  // Perform calculation
  switch (operation) {
    case '+':
      result = firstOperand + secondOperand;
      break;
    case '-':
      result = firstOperand - secondOperand;
      break;
    case '*':
      result = firstOperand * secondOperand;
      break;
    case '/':
      if (secondOperand != 0) {
        result = firstOperand / secondOperand;
      } else {
        displayError("Divide by Zero");
        return;
      }
      break;
    case '^':
      result = pow(firstOperand, secondOperand);
      break;
    case '%':
      result = (int)firstOperand % (int)secondOperand;
      break;
  }
  
  // Display result
  lcd.clear();
  lcd.setCursor(0, 0);
  String calcDisplay = String(firstOperand) + " " + operation + " " + String(secondOperand);
  
  if (calcDisplay.length() > 16) {
    lcd.print(calcDisplay.substring(0, 16));
  } else {
    lcd.print(calcDisplay);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("= " + String(result));
  
  // Set up for next calculation
  currentNumber = String(result);
  ansValue = result;
  operation = ' ';
  newCalculation = true;
  displayExpression = ""; // Reset display expression
}

void clearCalculator() {
  currentNumber = "";
  expression = "";
  displayExpression = ""; // Clear display expression
  firstOperand = 0;
  secondOperand = 0;
  result = 0;
  operation = ' ';
  newCalculation = true;
  operationPressed = false;
  parenLevel = 0;
  
  lcd.clear();
  lcd.print("Ready");
  delay(500);
  lcd.clear();
  
  updateModeDisplay();
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  // Display mode and current expression/number
  String displayLine = "";
  if (currentMode != "") {
    displayLine += "[" + currentMode + "] ";
  }
  
  // Show current operation if applicable
  if (displayExpression != "" && currentNumber != "") {
    // Display the expression and current number on separate lines
    displayLine += displayExpression;
    lcd.print(displayLine.length() > 16 ? displayLine.substring(displayLine.length() - 16) : displayLine);
    
    lcd.setCursor(0, 1);
    lcd.print(currentNumber);
  } else if (displayExpression != "") {
    // Display just the expression if no current number
    displayLine += displayExpression;
    lcd.print(displayLine.length() > 16 ? displayLine.substring(displayLine.length() - 16) : displayLine);
  } else {
    // Display just the current number
    displayLine += currentNumber;
    lcd.print(displayLine);
  }
}

void updateModeDisplay() {
  updateDisplay(); // Use the same display function to ensure consistency
}

void displayError(String errorMsg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error:");
  lcd.setCursor(0, 1);
  lcd.print(errorMsg);
  delay(1500);
  clearCalculator();
}
