

// include the library code:
#include "ConfigureLib.h"


LiquidCrystal *m_pLCD;

#define MaxTankCount  5 

 Tank *m_pTank[MaxTankCount];

//Tank Class
Tank::Tank(String tankName,int no,bool primary,int height1,int height2)
{
  
    TankName = tankName;
    TankNo = no;
    IsPrimaryTank = primary;
    BottomToFillHeight = height1;
    FillToSensorHeight = height2;

    Height = BottomToFillHeight + FillToSensorHeight;
}

int Tank::GetTankNo()
{
  return TankNo;
}

String Tank::GetTankName()
{
   return TankName;
}

bool Tank::IsPrimary()
{
  return IsPrimaryTank;
}

int Tank::GetBottomToFillHeight()
{
  return BottomToFillHeight;
}

int Tank::GetFillToSensorHeight()
{
  return FillToSensorHeight;
}

int Tank::GetTankHeight()
{
  return Height;
}

void Tank::SetTankFilledHeight(float value)
{
  FilledHeight = value;
}
/*******************************************************************************************************************************************************************************/

//Configuration Class
ConfigureLib::ConfigureLib(int tanks,LiquidCrystal *lcd)
{
  //Inititalize the number of sensors going to configure

    if(tanks > 0)
    {
      MaxTanks = tanks;
    }
    m_pLCD = lcd;
}

//Set up each sensor data
bool ConfigureLib::SetupConfiguration()
{
  
  String message1;
  String message2;
  
  message1 = "Initialize . . ";
  DisplayLCDMessage(0,0,message1);
  
  message1 = "Tanks Count ";
  message2 = MaxTanks;
  
  //Display count in 7th position of second row
  DisplayLCDMessage(0,0,message1,7,1,message2);
     
  return true;
}

//Display message to LCD
void ConfigureLib::DisplayLCDMessage(int c1 = 0,int r1 = 0,String messageRow1 = "",int c2 = 0,int r2 = 0,String messageRow2 = "")
{
   m_pLCD->clear();
   m_pLCD->setCursor(c1,r1);
   m_pLCD->print(messageRow1);
   m_pLCD->setCursor(c2,r2);
   m_pLCD->print(messageRow2);
    
   delay(1000);
}

//Add Tank Details
bool ConfigureLib::AddTankDetails(String tankName,int no,bool primary,int height1,int height2)
{
  bool result = false;

  if(tankName != "")
  {
    m_pTank[no] = new Tank(tankName,no,primary,height1,height2);
    result = true;
  }
     
  return result;
}

//Show Tank Details
void ConfigureLib::DisplayTankDetails(int no)
{
  if(m_pTank[no])
  {
      String message1;
      String message2;
      
      char tempMsg1[LCD_CHAR_LENGTH];  
      sprintf(tempMsg1, " B2FHt:%d", m_pTank[no]->GetBottomToFillHeight()); // send data to the buffer
       
      message1 = m_pTank[no]->GetTankName() + tempMsg1;
      
      char tempMsg2[LCD_CHAR_LENGTH];
      
      if(m_pTank[no]->IsPrimary())
        sprintf(tempMsg2, "F2SHt:%d,Prmry:Y", m_pTank[no]->GetFillToSensorHeight()); // send data to the buffer
      else
        sprintf(tempMsg2, "F2Ht:%d,Prmry:N",m_pTank[no]->GetFillToSensorHeight()); // send data to the buffer
      
      message2 = tempMsg2;
      
      DisplayLCDMessage(0,0,message1,0,1,message2);
  }
  
}

//Get tank primary details
 bool ConfigureLib::IsTankPrimary(int tankNo)
 {
  if(m_pTank[tankNo])
     return  m_pTank[tankNo]->IsPrimary();

  return false;
 }

//Get Tank fill height
 int ConfigureLib::GetTankFillHeight(int tankNo)
{
   int height = 0;
   
   if(m_pTank[tankNo])
     height = m_pTank[tankNo]->GetBottomToFillHeight();

  return height;
}

int ConfigureLib::GetFilltoSensorHeight(int tankNo)
{
  int height = 0;
   
   if(m_pTank[tankNo])
     height = m_pTank[tankNo]->GetFillToSensorHeight();

  return height;  
}

void ConfigureLib::SetTankFilledHeight(int tankNo,float value)
{
  if(m_pTank[tankNo])
     m_pTank[tankNo]->SetTankFilledHeight(value);
}


