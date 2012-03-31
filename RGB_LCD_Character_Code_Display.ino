// --------------------------------------------------------------------------------
// An Arduino sketch to display the available characters on the Adafruit RGB LCD shield or
// Adafruit Negative RGB LCD shield.  Up/down buttons cycle through backlight colors.
// Left/right buttons cycle through the characters 16 at a time.
//
// MIT license.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
//      ******************************************************
//      Designed for the Adafruit RGB LCD Shield Kit
//      http://www.adafruit.com/products/716
//      or
//      Adafruit Negative RGB LCD Shield Kit
//      http://www.adafruit.com/products/714
//      ******************************************************
//
//
// --------------------------------------------------------------------------------
// Dependencies
// --------------------------------------------------------------------------------
// Adafruit Industries's RGB 16x2 LCD Shield library:
//       https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library
// Adafruit Industries's MCP23017 I2C Port Expander library:
//       https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
// --------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

//! The LCD display object.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//! Offset of current char page.
uint8_t char_offset;

//! The current backlight color.
uint8_t backlight_color;

//! Init the LCD and variables.
void setup() {
  backlight_color = 0x7;
  char_offset = 0;

  // LCD has 16 columns & 2 rows
  lcd.begin(16, 2);
  lcd.setBacklight(backlight_color);
  
  draw_screen();
}

//! Main loop of execution.
void loop() {
  uint8_t clicked_buttons = read_clicked_buttons();

  if (clicked_buttons & BUTTON_LEFT) {
    // Decrement character offset, wrapping if needed.
    char_offset = (char_offset > 0x0) ? char_offset - 1 : 0xF;
    draw_screen();
  } else if (clicked_buttons & BUTTON_RIGHT) {
    // Increment character offset, wrapping if needed.
    char_offset = (char_offset < 0xF) ? char_offset + 1 : 0x0;
    draw_screen();
  } else if (clicked_buttons & BUTTON_UP) {
    // Increment backlight color, wrapping if needed.
    backlight_color = (backlight_color < 0x7) ? backlight_color + 1 : 0x0;
    lcd.setBacklight(backlight_color);
  } else if (clicked_buttons & BUTTON_DOWN) {
    // Decrement backlight color, wrapping if needed.
    backlight_color = (backlight_color > 0x0) ? backlight_color - 1 : 0x7;
    lcd.setBacklight(backlight_color);
  }

  delay(10);
}

//! Return a bitmask of clicked buttons.
/*!
  Examine the bitmask of buttons which are currently pressed and compare against
  the bitmask of which buttons were pressed last time the function was called.
  If a button transitions from pressed to released, return it in the bitmask.

  \return the bitmask of clicked buttons
*/
uint8_t read_clicked_buttons() {
  // Static variables are intialized once and hold value between calls to the function.
  static uint8_t last_buttons = 0;
  
  // Read which buttons are currently on.
  uint8_t current_buttons = lcd.readButtons();
  
  // Calculate which buttons changed state since the last read.
  uint8_t changed_buttons = last_buttons ^ current_buttons;
  
  // Calculate which buttons are currently off.
  uint8_t off_buttons = ~current_buttons;
  
  // Calculate which buttons transitioned from on to off between reads.
  uint8_t clicked_buttons = changed_buttons & off_buttons;
  
  // Store current buttons for next call.
  last_buttons = current_buttons;
  
  return clicked_buttons;
}

//! Draw column headers and the current characters to the LCD.
/*!
  
*/
void draw_screen() {
  // Disable the cursor display.
  lcd.noCursor();
  
  // Move to the first row, first column.
  lcd.setCursor(0, 0);
  
  // Draw the column headers directly from flash memory.
  lcd.print(F("0123456789ABCDEF"));

  // Move to the second row, first column.
  lcd.setCursor(0, 1);
  
  // Loop over current characters and draw them to the LCD.
  for (uint8_t i=0; i<=0xF; i++) {
    lcd.write((char_offset << 4) | i);
  }
  
  // Move the cursor to the first row, the column of
  // the current character offset.
  lcd.setCursor(char_offset, 0);
  
  // Enable the cursor.
  lcd.cursor();
}
