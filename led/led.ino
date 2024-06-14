#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  lcd.print("Hello, World!"); // Print a test message
}

void loop() {
    // Set the cursor to column 0, line 1 (second row)
  lcd.setCursor(0, 1);
  // Print the number of seconds since reset
  delay(5000);
}
