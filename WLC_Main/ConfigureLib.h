#ifndef configure
#define configure


//Check Aurdion library

#if(ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "wProgram.h"
#endif

#include <LiquidCrystal.h>
#define LCD_CHAR_LENGTH 18

//Class Tank to keep all tank details
class Tank
{
  public:
      Tank(String tankName,int no,bool primary,int height1,int height2);
      int GetTankNo();
      int GetTankHeight();
      bool IsPrimary();
      String GetTankName();
      int GetBottomToFillHeight();
      int GetFillToSensorHeight();
      void SetTankFilledHeight(float value);
      void SetTankDistance(float distance);
      bool IsTankFilled();
     
  private:
      //Store Tank details in class
      String TankName; // Tank Name
      int    TankNo; //Tank No details
      bool   IsPrimaryTank; //Nothing but Sump
      int    Height; //In inches
      int    BottomToFillHeight;
      int    FillToSensorHeight;
      float  FilledHeight;
      float  Distance;
 };


//Configuration class
class ConfigureLib
{

  public:
      //Constructor
      ConfigureLib(int sensors,LiquidCrystal *lcd);
      
      //Variables
      int MaxTanks;
      
      //Methods
      //SetupConfiguration
      bool SetupConfiguration();
      
      // c1 = Column 1 of first row of LCD
      //r1 = First row
      //c2 = Second column of LCD
      //r2 = second row of LCD
      // MessageRow1 = First row message
      // Message Row2 = Second row Message
      void DisplayLCDMessage(int c1 = 0,int r1 = 0,String messageRow1 = "",int c2 = 0,int r2 = 0,String messageRow2 = "");
      
      // void SetNumberOfTanks(int maxTanks);
      bool AddTankDetails(String tankName,int no,bool primary,int height1,int height2);

      //Display Tankd Details
      void DisplayTankDetails(int no);

      bool IsTankPrimary(int tankNo);

      int GetTankFillHeight(int tankNo);

      int GetFilltoSensorHeight(int tankNo);

      void SetTankFilledHeight(int tankNo,float value);

      void SetTankDistance(int tankNo, float tankDistance);
      
      bool IsOverHeadTank1Filled();

      bool IsOverHeadTank2Filled();

      bool IsSumpFilled();
      
};

#endif
