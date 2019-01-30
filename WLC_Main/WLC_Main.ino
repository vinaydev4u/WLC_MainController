
// include the library code:
#include <LiquidCrystal.h>
//EEPROM is to store values offline even when device is power off
#include <EEPROM.h>
#include <stdio.h>

#include "ConfigureLib.h"
#include "TransferI2C_WLC.h"

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

//Global Variables and Objects
ConfigureLib *m_pConfigureLib;
int  TanksSelected = 0;
bool IsConfiguration = false;
bool EnableDebug = true;
bool SumpMotorExists  = false;
bool BoreMotorExists = false;

//Levels of Each tank in percentage
float leve0 = 0; //zero level or 5%
float leve20 = 0;//20 % of the tank height
float leve50 = 0;//50% of tank height
float leve80 = 0;//80% of tank height
float leve100 = 0;//50% of tank height

/*
Bar Graph Logic To display Tank filled status
*/

// To Create Characters for Bar Graph
byte NoLevel[8] = {
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000
};

byte Leve20[8] = {
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b11111,
 0b11111
};

byte Leve50[8] = {
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b11111,
 0b11111,
 0b11111,
 0b11111
};

byte Leve80[8] = {
 0b00000,
 0b00000,
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111
};

byte Leve100[8] = {
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111,
 0b11111
};

//Constant address for EEPROM
const int MaxDataAddress = 100;
//Consecetive address location of Tank details to store in EEPROM
const int DataSetAddress = 0;
int DataAddress = 1;

//Constant Global Variables
const int  MaxTanksSupported = 4;//Numbers
const int MaxTankHeight = 100; // inches
const int MaxTickCount  = 5;
const int PrimaryTankNo = 1;
//Error reading for valus in inches
const int ErrorReading = 1000;


//Basic setup
void setup() {

   // put your setup code here, to run once:

    //Initialization
    InitializeLCD();
  
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


    //Read EEPROM to check data exists
    if(EEPROM.read(DataSetAddress) == 1)
    {
        ReadConfigDataFromEEPROM(); 
    }
    else
    {
      //Configuration Setup for each tank with parameters required
      SetupConfiguration();
    }
}

void loop() {

  //Check for reset pin to reset the configration settings
     if(digitalRead(keypadResetPin) == true)
     {
        if(EnableDebug)
          Serial.println("Reset Pin pressed");
       
        EEPROM.write(DataSetAddress,0);
        
         for(int i = 1; i<= MaxDataAddress ; i++)
            EEPROM.write(i,0);

        DataAddress = 1;
    
        //Configuration Setup for each tank with parameters required
        SetupConfiguration();
     }
     

//   delay(400);
//   if(digitalRead(keypadOKPin) == false)
//   {
//        LogSerial(false,logFunc,false,String("OK Key pressed"));
//   }
//   else if(digitalRead(keypadUpPin) == false)
//   {
//        LogSerial(false,logFunc,false,String("Up Key pressed"));
//   }
//   else if(digitalRead(keypadDownPin) == false)
//   {
//        LogSerial(false,logFunc,false,String("Down Key pressed"));
//   }
//   else if(digitalRead(keypadResetPin) == false)
//   {
//        LogSerial(false,logFunc,false,String("Reset Key pressed"));
//   }
//   else if(digitalRead(keypadRefillPin) == false)
//   {
//        LogSerial(false,logFunc,false,String("Refill Key pressed"));
//   }
//
//   //Test LED Indicators
//    digitalWrite(LED3, HIGH);
//    delay(100);
//    digitalWrite(LED3, LOW);
//
//    //Test Relays
//    digitalWrite(Relay1, HIGH);
//    delay(500);
//    digitalWrite(Relay1, LOW);
//
//    digitalWrite(Relay2, HIGH);
//    delay(1000);
//    digitalWrite(Relay2, LOW);
//    
//    digitalWrite(Relay3, HIGH);
//    delay(1000);
//    digitalWrite(Relay3, LOW);
//    
//  //Test  Buzzer
//    digitalWrite(buzzerPin, HIGH);
//    delay(400);
//    digitalWrite(buzzerPin, LOW);
  
}

//Initialize LCD with welcome message
void InitializeLCD()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.clear();
    
    //Welcome Message
    lcd.setCursor(0,0);
    lcd.print("Welcome MyTools");
    lcd.setCursor(3,1); 
    lcd.print("Controller");
    delay(500);

    //Bar Graph Initialization
    lcd.createChar(0, NoLevel);
    lcd.createChar(1, Leve20);
    lcd.createChar(2, Leve50);
    lcd.createChar(3, Leve80);
    lcd.createChar(4, Leve100);
}

//Read configuration Data from EEPROM
void ReadConfigDataFromEEPROM()
{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Data Loading. . ");
      delay(500);

      if(EnableDebug)
        Serial.println("Data exists in EEPROM");

      //Read number of tanks in EEPROM at DataAddress = 1;
      TanksSelected =  EEPROM.read(DataAddress);

       if(EnableDebug)
       {
          Serial.print("Tanks Selected : ");
          Serial.println(TanksSelected);
       }
      
      //display user number of tanks selected
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(TanksSelected);
      lcd.println(" Tanks Selected");
      delay(1000);
      
      //Initialize Configuration Library
      m_pConfigureLib = new ConfigureLib(TanksSelected,&lcd);
  
      //Loop data address to retrive all tank details
      int address = DataAddress++;
      while( address <= MaxDataAddress)
      {
        //Read all tank details from EEPROM
        for(int tankCount = 1; tankCount <= TanksSelected; tankCount++)
        {
          int btmToFillHeight = 0;
          int fillToSensorHeight = 0;
          bool isPrimary = false;
          
          char tName[10] = "";

          if(tankCount == PrimaryTankNo)
            strcpy(tName,"Sump");
          else
            strcpy(tName,"Tank%d:");
          
          String tankName = FormatIntMessage(tName,tankCount);
 
          address++;
          isPrimary = EEPROM.read(address);
          
          address++;
          btmToFillHeight = EEPROM.read(address);
  
          address++;
          fillToSensorHeight = EEPROM.read(address);

          if(EnableDebug)
          {
            Serial.println(tankName);
            Serial.println(btmToFillHeight);
            Serial.println(fillToSensorHeight);
          }
          
          if(m_pConfigureLib)
          {    
             if(m_pConfigureLib->AddTankDetails(tankName,tankCount,isPrimary,btmToFillHeight,fillToSensorHeight))
             {
               if(EnableDebug)
                {
                    Serial.print("Added ");
                    Serial.println(tankName);
                }
             }
          }
        }

        break;// once all tank details are read just break the loop
      }

      //Display loaded data
      for(int i = 1; i <= TanksSelected; i++)
      {
        m_pConfigureLib->DisplayTankDetails(i);
        delay(1000);
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Initializing. . .");
      delay(500);
}


//Set up configuration by taking all tank height details
void SetupConfiguration()
{
    //Please Enter tanks to be configured
    lcd.clear();
    lcd.print("Number of Tanks:");
    delay(200);
    TanksSelected = GetUserInput(7,1,MaxTanksSupported);
    
    if(EnableDebug)
    {
      Serial.print("Tank Selected :");
      Serial.println(TanksSelected);
    }
    
    //Initialize Configuration Library
    m_pConfigureLib = new ConfigureLib(TanksSelected,&lcd);
    
    //display user number of tanks selected
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(TanksSelected);
    lcd.println(" Tanks Selected");
    delay(1000);

    if(EnableDebug)
      Serial.println("Configuration start for selected Tank count ");

    //Store all details at EEPROM 
    EEPROM.write(DataSetAddress,1); 
  
    //Wrie tank count
    EEPROM.write(DataAddress,TanksSelected); 

    //1. Change tank1 as sump by default and dont give acces to primary tank
    //2. Allow user to select only sump or has borewell
    //3. Allow peak hours tank fill option and set time
    //4. Set time not to switch on motor on unusual hours
    //5. Dry Run.
    
    // Take user input using loop for number of tanks selected
    for(int tankCount = 1; tankCount <= TanksSelected; tankCount++)
    {
      int btmToFillHeight = 0;
      int fillToSensorHeight = 0;
      bool isPrimary = false;
      
      char tName[10] = "";
      
      if(tankCount == PrimaryTankNo)
        strcpy(tName,"Sump");
      else
        strcpy(tName,"Tank%d:");
            
      String tankName = FormatIntMessage(tName,tankCount);
      String displayMsg = "";
      displayMsg  += tankName + "Ht in inch";

      if(EnableDebug)
        Serial.println(displayMsg);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(displayMsg);
      
      lcd.setCursor(0,1);
      lcd.print("B2F(inch):");
      delay(200);
      btmToFillHeight = GetUserInput(11,1,MaxTankHeight);

      lcd.setCursor(0,1);
      lcd.print("F2S(inch):");
      delay(200);
      fillToSensorHeight = GetUserInput(11,1,MaxTankHeight);

      if(EnableDebug)
      {
        Serial.print("btmToFillHeight : ");
        Serial.println(btmToFillHeight);
        Serial.print("fillToSensorHeight :");
        Serial.println(fillToSensorHeight);
      }

      //Get Yes or No boolean result
     /* lcd.clear();
      lcd.setCursor(0,0);
      displayMsg  = tankName + "Is Primary :";
      lcd.print(displayMsg);
      isPrimary = GetUserYesNoInput(7,1);
      delay(400);*/

     if(tankCount == PrimaryTankNo)
        isPrimary = true;
           
      if(m_pConfigureLib)
      {    
         if(m_pConfigureLib->AddTankDetails(tankName,tankCount,isPrimary,btmToFillHeight,fillToSensorHeight))
         {
           if(EnableDebug)
            {
              Serial.print("Added ");
              Serial.println(tankName);
            }
         }

        //Write Tank primary details
        DataAddress++;
        EEPROM.write(DataAddress,isPrimary); 


        //Write Tank btmToFillHeight details
        DataAddress++;
        EEPROM.write(DataAddress,btmToFillHeight); 

        //Write Tank fillToSensorHeight details
        DataAddress++;
        EEPROM.write(DataAddress,fillToSensorHeight);
             
      }
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Configuration");
    lcd.setCursor(0,1);
    lcd.print("Complete");
    delay(1500);

    if(EnableDebug)
      Serial.println("Configuration Complete!!");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please wait. . .");
    delay(500);

  //Display configured tank details to user
    for(int i = 1; i <= TanksSelected; i++)
    {
      m_pConfigureLib->DisplayTankDetails(i);
      delay(1500);
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Initializing. . .");
    delay(500);
}

//Get user input with respect to max value and display cursor in selected rows and column values
int GetUserInput(int col,int row,int maxValue)
{
   int result = 0;
   int digit = 0;

   //Clear LCD having digits
   lcd.setCursor(col,row);
   lcd.print(" ");
   lcd.setCursor(col+1,row);
   lcd.print(" ");

   int okKeyState =  digitalRead(keypadOKPin);

   if(EnableDebug)
      Serial.println(okKeyState);
  
   // Run while loop till user press enter key
   while(!okKeyState)
    {
        if(digitalRead(keypadOKPin) == true)
        {
          delay(500);

          if(EnableDebug)
            Serial.println("Break if OK key press");
            
          break;
        }
        //Take first input
        else if(digitalRead(keypadUpPin) == true)
        {
          if(++digit > maxValue)
            digit = maxValue;
            
          delay(400);
         
          lcd.setCursor(col,row);     
          lcd.print(digit);  
        }
        else if(digitalRead(keypadDownPin) == true)
        {
            if(--digit < 0)
              digit = 0;
                       
            delay(400); 

            //Make sure second digit is cleared as and when it swithches back to single digit
            lcd.setCursor(col+1,row);
            lcd.print(" ");

            lcd.setCursor(col,row);
            lcd.print(digit);  
        }   
    }

    result = digit;

    if(EnableDebug)
      Serial.println(result);
   
  return result;
}

//Get user input interm of Yes or No and returns boolean value
bool GetUserYesNoInput(int col,int row)
{

   bool result = false;
   char value = {'Y'};
   
   //Clear LCD having digits
   lcd.setCursor(col,row);
   lcd.print(" ");

   lcd.setCursor(col+1,row);
   lcd.print(" ");
   
   int okKeyState =  digitalRead(keypadOKPin);

   if(EnableDebug)
      Serial.println(okKeyState);
  
    // Run while loop till user press enter key    
    while(!okKeyState)
    {
       if(digitalRead(keypadOKPin) == true)
       {
         delay(600);

         if(EnableDebug)
            Serial.println("Break if OK key press");
            
          break;
       }
      //Take first input
      else if(digitalRead(keypadUpPin) == true)
      {    
          value = 'Y';
          lcd.setCursor(col,row);     
          lcd.print(value);
          result = true;
      }
      else if(digitalRead(keypadDownPin) == true)
      {
          value = 'N';   
          lcd.setCursor(col,row);
          lcd.print(value);
          result = false;
      }
    }

    return result;
}

//Formats integer messages
String FormatIntMessage(char* msg,int value)
{
  String message = "";
  
  char tempMsg[LCD_CHAR_LENGTH];
  sprintf(tempMsg, msg, value); // send data to the buffer
      message = tempMsg;

  return message;
}


//Set tank status w.r.t value as signal level
void ShowTankStatusInLCD(String message1,float val,float tankheight)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(message1);


    if(val > ErrorReading)
    {
      lcd.setCursor(0,1);
      lcd.print("Sensor Error !!");
      return;
    }
    else
    {  
      lcd.setCursor(0, 1);
      lcd.print("LOW");
      lcd.setCursor(11, 1);
      lcd.print("HIGH");
    }

/*
    Serial.print(leve0);
    Serial.print(leve20);
    Serial.print(leve50);
    Serial.print(leve80);
    Serial.print(leve100);
    Serial.print(val);
    */

    //Set value for level10
    if(val >= leve100 ) // water level reaching to empty
    {
      lcd.setCursor(3, 1); 
      lcd.write(byte(0));
      lcd.setCursor(4, 1); 
      lcd.write(byte(0));
      lcd.setCursor(5, 1); 
      lcd.write(byte(0));
      lcd.setCursor(6, 1);
      lcd.write(byte(0));
      lcd.setCursor(7, 1);
      lcd.write(byte(0));
      lcd.setCursor(8, 1);
      lcd.write(byte(0));
      lcd.setCursor(9, 1);
      lcd.write(byte(0));
      lcd.setCursor(10, 1);
      lcd.write(byte(0));
      lcd.setCursor(0,0);

    }
    else if(val <= leve100 && val > leve80 ) //Water level reaches near to 20%
    {
      delay(50);
      lcd.setCursor(3, 1); 
      lcd.write(byte(1));
      lcd.setCursor(4, 1); 
      lcd.write(byte(1));
      lcd.setCursor(5, 1); 
      lcd.write(byte(0));
      lcd.setCursor(6, 1);
      lcd.write(byte(0));
      lcd.setCursor(7, 1);
      lcd.write(byte(0));
      lcd.setCursor(8, 1);
      lcd.write(byte(0));
      lcd.setCursor(9, 1);
      lcd.write(byte(0));
      lcd.setCursor(10, 1);
      lcd.write(byte(0));
      lcd.setCursor(0,0);
    }
    else if(val <= leve80 && val> leve50 ) //Water level is getting close to 50% of the the tank
    {
      lcd.setCursor(3, 1); 
      lcd.write(byte(1));
      lcd.setCursor(4, 1); 
      lcd.write(byte(1));
      lcd.setCursor(5, 1); 
      lcd.write(byte(2));
      lcd.setCursor(6, 1); 
      lcd.write(byte(2));
      lcd.setCursor(7, 1);
      lcd.write(byte(0));
      lcd.setCursor(8, 1);
      lcd.write(byte(0));
      lcd.setCursor(9, 1);
      lcd.write(byte(0));
      lcd.setCursor(10, 1);
      lcd.write(byte(0));
    }
    else if(val <= leve50 && val > leve20 ) //water level nearing to level 50%
    { 
      lcd.setCursor(3, 1); 
      lcd.write(byte(1));
      lcd.setCursor(4, 1); 
      lcd.write(byte(1));
      lcd.setCursor(5, 1); 
      lcd.write(byte(2));
      lcd.setCursor(6, 1); 
      lcd.write(byte(2));
      lcd.setCursor(7, 1);
      lcd.write(byte(3));
      lcd.setCursor(8, 1);
      lcd.write(byte(3));
      lcd.setCursor(9, 1);
      lcd.write(byte(0));
      lcd.setCursor(10, 1);
      lcd.write(byte(0));
    }
   else if( val <= leve20  ) //water level greater than 20 is nothing more than 80% fill
    {
      lcd.setCursor(3, 1); 
      lcd.write(byte(1));
      lcd.setCursor(4, 1); 
      lcd.write(byte(1));
      lcd.setCursor(5, 1); 
      lcd.write(byte(2));
      lcd.setCursor(6, 1); 
      lcd.write(byte(2));
      lcd.setCursor(7, 1);
      lcd.write(byte(3));
      lcd.setCursor(8, 1);
      lcd.write(byte(3));
      lcd.setCursor(9, 1);
      lcd.write(byte(4));
      lcd.setCursor(10, 1);
      lcd.write(byte(4));
  }

  delay(1000);
  
}


//Log function
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
