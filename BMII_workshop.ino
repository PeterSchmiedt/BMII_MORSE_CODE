#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

#define ONE_LINE 16
#define BUTTON A0
#define BUTTON_TIME 200
#define PAUSE_TIME 500
#define LED_PIN A1
#define BLINK_INTERVAL 100
#define PAUSE_INTERVAL 2
#define PAUSE_MUL 3
#define DEBOUNCE_DELAY 50
#define MORSE_NO 36

int buttonState = 1;
int lastButtonState = 1;
int reading = 1;
int pauseTime = 0;
int currentPauseTime = 0;
int buttonTime = 0;
int currentButtonTime = 0;
bool registerBlink = false;
int currentBlinkTime = 0;
int previousBlinkTime = 0;
int blinkCount = 0;
int lastDebounceTime = 0;
int displayCharPos = 0;
int displayLinePos = 0;

char alphabet[MORSE_NO] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String morse[MORSE_NO] = {
  ".-",
  "-...",
  "-.-.",
  "-..",
  ".",
  "..-.",
  "--.",
  "....",
  "..",
  ".---",
  "-.-",
  ".-..",
  "--",
  "-.",
  "---",
  ".--.",
  "--.-",
  ".-.",
  "...",
  "-",
  "..-",
  "...-",
  ".--",
  "-..-",
  "-.--",
  "--..",
  "-----",
  ".----",
  "..---",
  "...--",
  "....-",
  ".....",
  "-....",
  "--...",
  "---..",
  "----."
};

String sb = "";


//-------------------------------------------------------------------------------------------------------------------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  pinMode(BUTTON, INPUT_PULLUP);


  lcd.begin(ONE_LINE, 2);

  // ------- Quick 3 blinks of backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight();
  //--------

  lcd.setCursor(0, 0);
  lcd.print("MORSE CODE");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("TRANSLATOR");
  delay(1000);


  lcd.clear();
  lcd.setCursor(0, 0);
}

//-------------------------------------------------------------------------------------------------------
void loop() {

  blinkLedService();

  lastButtonState = reading;
  reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }


  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {

    if (reading != buttonState) {
      buttonState = reading;
      //ak nastane zmena stavu
      if (buttonState == 0) {
        //pressed
        //start counting pressed
        buttonTime = millis();
        blinkCount = 0;
      } else {
        //released
        //start counting pause
        pauseTime = millis();

        //evaluate dash/dot
        currentButtonTime = millis();
        if ((currentButtonTime - buttonTime) < BUTTON_TIME) {
          //lcd.write(".");
          sb += '.';
        } else {
          //lcd.write("-");
          sb += '-';
        }
      }
    } else {
      if (buttonState == 0) {
        //pressed
      } else {
        //released
        currentPauseTime = millis();
        if ((currentPauseTime - pauseTime) > PAUSE_TIME - PAUSE_INTERVAL && (currentPauseTime - pauseTime) < PAUSE_TIME + PAUSE_INTERVAL) {
          //letter delimiter
          blinkLed();
        } else if ((currentPauseTime - pauseTime) > PAUSE_MUL * PAUSE_TIME - PAUSE_INTERVAL && (currentPauseTime - pauseTime) < PAUSE_MUL * PAUSE_TIME + PAUSE_INTERVAL) {
          //word delimiter
          blinkLed();
        }
      }
    }
  }
}

void blinkLed() {
  registerBlink = true;
  digitalWrite(LED_PIN, LOW);
  previousBlinkTime = millis();
}

void blinkLedService() {
  if (registerBlink == true) {
    currentBlinkTime = millis();
    if ((currentBlinkTime - previousBlinkTime) > BLINK_INTERVAL) {
      digitalWrite(LED_PIN, HIGH);

      blinkCount++;
      if (blinkCount == 1) {

        //lcd.print(sb);
        printLetter();//-------------------------------------------
        sb = "";

      } else if (blinkCount == 2) {
        lcd.write(" ");
        displayCharPos++;
        checkDisplay();
      }

      registerBlink = false;
    }
  }
}

void printLetter() {

  for (int i = 0; i <= MORSE_NO; i++) {
    if (sb == morse[i]) {
      lcd.write(alphabet[i]);
      displayCharPos++;
      checkDisplay();
    }
  }
}

void checkDisplay() {
  if(displayCharPos >= 16) {
    displayCharPos = 0;
    displayLinePos++;
    displayLinePos %= 2;
    lcd.setCursor(displayCharPos, displayLinePos);
    lcd.write("                ");
    lcd.setCursor(displayCharPos, displayLinePos);
  }
}













