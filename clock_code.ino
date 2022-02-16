   // include the library code:
#include <LiquidCrystal.h>

int hours = 16;
int minutes = 44;

int lB = 0;
int prevLB = 0;
int mB = 0;
int prevMB = 0;
int rB = 0;
int prevRB = 0;

int setHour = 16;
int setMinute = 45;

int setPosition = 0;

bool alarmSet = false;

bool clockMode = true; // true = clock, false = set alarm

bool alarmOn = false;

unsigned long onDuration = 0;
int holder = 0;
unsigned long pressStartTime = 0;
unsigned long cursorBlinkTime = 0;
int cursorBlinkInterval = 400;
bool cursorOn = true;

const int soundPin = 11;
const int motorPin = 9;
const int leftButton = 10;
const int middleButton = 12;
const int rightButton = 13;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3, 4, 8, 7, 6, 5);

byte clockChar[] = {
  0x00,
  0x00,
  0x0E,
  0x15,
  0x17,
  0x11,
  0x0E,
  0x00
};

void setup() {
  pinMode(leftButton, INPUT);
  pinMode(middleButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(A5, INPUT);

  pinMode(motorPin, OUTPUT);
  pinMode(soundPin, OUTPUT);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(0, clockChar);
  lcd.clear();

  Serial.begin(9600);
}

void loop() {
  if(millis() > cursorBlinkTime + cursorBlinkInterval) {
    cursorOn = !cursorOn;
    cursorBlinkTime = millis();
  }
  
  lB = digitalRead(leftButton);
  mB = digitalRead(middleButton);
  rB = digitalRead(rightButton);

  if(mB != prevMB) {
    if(mB == HIGH) {
      Serial.println("mB press");
      pressStartTime = millis();
    } else {
      if(millis() > pressStartTime + 1200) {
        clockMode = !clockMode;
        lcd.clear();
        Serial.println("mB long release");
      } else {
        if(clockMode == true) {
          alarmSet = !alarmSet;
          lcd.clear();
        } else {
          if(setPosition == 0) {
            setPosition = 1;  
          } else if(setPosition == 1) {
            setPosition = 0;
            alarmSet = true;
            clockMode = true;  
          }
        }
        Serial.println("mB short release");
      }
    }
  }
  prevMB = mB;

  if(lB != prevLB) {
    if(lB == HIGH) {
      Serial.println("lB press");
    } else {
      if(clockMode == false) {
        if(setPosition == 0) {
          setHour--;
          if(setHour < 0) {
            setHour = 23;
          }
        } else if(setPosition == 1) {
          setMinute--;
          if(setMinute < 0) {
            setMinute = 59;
          }
        }
      }
      Serial.println("lB short release");
    }
  }
  prevLB = lB;

  if(rB != prevRB) {
    if(rB == HIGH) {
      Serial.println("rB press");
    } else {
      if(clockMode == false) {
        if(setPosition == 0) {
          setHour++;
          if(setHour > 23) {
            setHour = 0;
          }
        } else if(setPosition == 1) {
          setMinute++;
          if(setMinute > 59) {
            setMinute = 0;
          }
        }
      }
      Serial.println("rB short release");
    }
  }
  prevRB = rB;

  lcd.setCursor(0, 0);

  onDuration = millis() / 1000;

  if(onDuration > holder + 59) {
    holder += 60;  
    minutes++;
  }
  if(minutes >= 60) {
    minutes = 0;
    hours++;
  }
  if (hours > 23) {
    hours = 0;
  }

  if(clockMode == true) {
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 0);
  
    // print hours
    if(hours < 10) {
      lcd.print(0);
    }
    lcd.print(hours);
  
    // print ":"
    lcd.print(":");
  
    //print minutes
    if(minutes < 10) {
      lcd.print(0);
    }
    lcd.print(minutes);
  
    lcd.print(":");
  
    if(onDuration - holder < 10) {
      lcd.print(0);
    }
    lcd.print(onDuration - holder);
    
    if(alarmSet == true) {
      lcd.setCursor(15, 0);
      lcd.write(byte(0));
      lcd.setCursor(0, 0);
    }

    //Check if alarm should start
    if(setHour == hours && setMinute == minutes && onDuration - holder == 0 && alarmSet == true) {
        alarmOn = true;
    }
    
    if(alarmOn == true) {
        Serial.println("ALARM");
        digitalWrite(motorPin, HIGH);
        tone(soundPin, 1450, 600);
        if(digitalRead(A5) == 0) {
          alarmOn = false;  
          alarmSet = false;
        }
    } else {
        digitalWrite(motorPin, LOW);  
    }
    
  } else {
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 0);
  
    // print hours
    if(setPosition == 0) {
      if(cursorOn == true) {
        if(setHour < 10) {
          lcd.print(0);
        }
        lcd.print(setHour);
      } else {
        lcd.print("  ");
      }
    } else {
      if(setHour < 10) {
          lcd.print(0);
        }
        lcd.print(setHour);
    }
  
    // print ":"
    lcd.print(":");
  
    //print minutes
    if(setPosition == 1) {
      if(cursorOn == true) {
        if(setMinute < 10) {
          lcd.print(0);
        }
        lcd.print(setMinute);
      } else {
        lcd.print("  ");
      }
    } else {
      if(setMinute < 10) {
          lcd.print(0);
        }
        lcd.print(setMinute);
    }
  }
}
