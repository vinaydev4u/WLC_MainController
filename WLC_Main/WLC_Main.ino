
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//Pins for Keypad Operation
const int keypadUpPin = A0;
const int keypadDownPin = A1;
const int keypadOKPin = A2;
const int keypadRefillPin = 9;
const int keypadResetPin = 12;

//Buzzer if any of the level goes down
const int buzzerPin = A3;

//LED Pins
//const int LED1 = 6;
//const int LED2 = 7;
const int LED3 = 8;

//Relay Pins
const int Relay1 = 11;
const int Relay2 = 10;
const int Relay3 = 13;

//Configuration or Reset Setting PIN

const String logFunc = "Test";

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");

   //Buzer pin
    pinMode(buzzerPin, OUTPUT);

    //Keypad Pins
    pinMode(keypadUpPin, INPUT);
    pinMode(keypadDownPin, INPUT);
    pinMode(keypadResetPin, INPUT);
    pinMode(keypadOKPin, INPUT);
    pinMode(keypadRefillPin, INPUT);

    //LED Display
     //pinMode(LED1, OUTPUT);
     //pinMode(LED2, OUTPUT);
     pinMode(LED3, OUTPUT);

    //Relay Pins
     pinMode(Relay1, OUTPUT);
     pinMode(Relay2, OUTPUT);
     pinMode(Relay3, OUTPUT);

  //Logging
    Serial.begin(9600); // Starts the serial communication
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
   lcd.setCursor(0, 1);
  // print the number of seconds since reset:
    lcd.print(millis() / 1000);

   delay(400);
   if(digitalRead(keypadOKPin) == false)
   {
        LogSerial(false,logFunc,false,String("OK Key pressed"));
   }
   else if(digitalRead(keypadUpPin) == false)
   {
        LogSerial(false,logFunc,false,String("Up Key pressed"));
   }
   else if(digitalRead(keypadDownPin) == false)
   {
        LogSerial(false,logFunc,false,String("Down Key pressed"));
   }
   else if(digitalRead(keypadResetPin) == false)
   {
        LogSerial(false,logFunc,false,String("Reset Key pressed"));
   }
   else if(digitalRead(keypadRefillPin) == false)
   {
        LogSerial(false,logFunc,false,String("Refill Key pressed"));
   }

   //Test LED Indicators
    digitalWrite(LED3, HIGH);
    delay(100);
    digitalWrite(LED3, LOW);

    //Test Relays
    digitalWrite(Relay1, HIGH);
    delay(500);
    digitalWrite(Relay1, LOW);

    digitalWrite(Relay2, HIGH);
    delay(1000);
    digitalWrite(Relay2, LOW);
    
    digitalWrite(Relay3, HIGH);
    delay(1000);
    digitalWrite(Relay3, LOW);
    
  //Test  Buzzer
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
  
}

void LogSerial(bool nextLine,String function,bool flow,String msg)
{
   if(true)
   {
      if(!flow)
        msg = function + " : " + msg;
      
      if(nextLine)
        Serial.println(msg);
       else
        Serial.print(msg);
   }
}
