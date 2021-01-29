#include "TimerOne.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <GyverEncoder.h> 
#include <avr/eeprom.h>
/////////////////////////////////////////////////////////////////////
#define CLK 9
#define DT 8
#define SW 7
/////////////////////////////////////////////////////////////////////
#define LINES 4   // количество строк дисплея
/////////////////////////////////////////////////////////////////////
#define P1_Pin 6 
#define P2_Pin 5
#define P3_Pin 4 
#define P4_Pin 3
/////////////////////////////////////////////////////////////////////
#define SDA_DS  23
#define SCL_DS  24  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
LiquidCrystal_I2C lcd(0x27, 20, 4);// инициализация дисплея
Encoder encoder(CLK, DT, SW);
/////////////////////////////////////////////////////////////////////
int CountPresses;
int Counter;
float TempCounter;
int PressedPump;
float BandwidthP1;
float BandwidthP2;
float BandwidthP3;
float BandwidthP4;
float BandwidthP1Read;
float BandwidthP2Read;
float BandwidthP3Read;
float BandwidthP4Read;
float TempTime;
float x;//кол-во милилитров в колбе
int SubstanceQuantity;
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#define SETTINGS_AMOUNT_MAIN 3  // количество настроек
int8_t arrowPosMAIN = 0;
int8_t screenPosMAIN = 0; // номер "экрана"
const char *settingsNamesMAIN[]  = 
{
  "Start pumps",
  "Manual Control Mode",
  "Settings",
};
/////////////////////////////////////////////////////////////////////
#define SETTINGS_AMOUNT_RELOAD 2  // количество настроек
int8_t arrowPosRELOAD = 0;
int8_t screenPosRELOAD = 0; // номер "экрана"
const char *settingsNamesRELOAD[]  = 
{
  "ALL PUMPS",
  "SINGLE PUMP",
};
/////////////////////////////////////////////////////////////////////
#define SETTINGS_AMOUNT_PUMPS 4
int8_t arrowPosPUMPS = 0;
int8_t screenPosPUMPS = 0; // номер "экрана"
const char *settingsNamesPUMPS[]  = 
{
  "Pump 1",
  "Pump 2",
  "Pump 3",
  "Pump 4",
};
/////////////////////////////////////////////////////////////////////
#define SETTINGS_AMOUNT_SETTINGS 4
float settingsparam [SETTINGS_AMOUNT_SETTINGS];
int8_t arrowPosSETTINGS = 0;
int8_t screenPosSETTINGS = 0; // номер "экрана"
const char *settingsNamesSETTINGS[]  = 
{
  "Pump 1",
  "Pump 2",
  "Pump 3",
  "Pump 4",
};
/////////////////////////////////////////////////////////////////////
#define SETTINGS_AMOUNT_START 4
int startparam [SETTINGS_AMOUNT_START];
int8_t arrowPosSTART = 0;
int8_t screenPosSTART = 0; // номер "экрана"
const char *settingsNamesSTART[]  = 
{
  "Pump 1",
  "Pump 2",
  "Pump 3",
  "Pump 4",
};
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  attachInterrupt(0, isrCLK, CHANGE);    // прерывание на 2 пине! CLK у энка
  attachInterrupt(1, isrDT, CHANGE);    // прерывание на 3 пине! DT у энка
  pinMode(P1_Pin, OUTPUT);
  pinMode(P2_Pin, OUTPUT);
  pinMode(P3_Pin, OUTPUT);
  pinMode(P4_Pin, OUTPUT);
  encoder.setType(TYPE2);
  printGUIMAIN();
  Timer1.initialize(100000);//1000= 1мс
  Timer1.attachInterrupt(counter);
  BandwidthP1Read=eeprom_read_float(0);
  //BandwidthP2Read=eeprom_read_float(4);
  //BandwidthP3Read=eeprom_read_float(8);
  //BandwidthP4Read=eeprom_read_float(12);
  //Serial.println(BandwidthP1Read);
  //Serial.println(BandwidthP2Read);
  //Serial.println(BandwidthP3Read);
  //Serial.println(BandwidthP4Read);
}
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void loop()
{
//Serial.println(Time);
//Serial.println(Counter);
//Serial.println(BandwidthP1Read);
//Serial.println(BandwidthP1);
/////////////////////////////////////////////////////////////////////
  if(encoder.isSingle())
  {
    CountPresses=CountPresses+1;
    if (CountPresses==1)
    {
      if (arrowPosMAIN==0)
      {
        printGUISTART();
      }
      if (arrowPosMAIN==1)
      {
        printGUIRELOAD();
      }
      if (arrowPosMAIN==2)
      {
        printGUISETTINGS();
      }
    }
    if (CountPresses==2)
    {
      if (arrowPosRELOAD==1)
      {
        printGUIPUMPS();
      }
    }
  }
/////////////////////////////////////////////////////////////////////
  if (encoder.isDouble())
  {
    Zeroing();
  }
    if (arrowPosMAIN==0)
    {
      StartMode();
    }
    if (arrowPosMAIN==1)
    { 
      ManualMode();
    }
    if (arrowPosMAIN==2)
    {
      SettingsMode();
    }
/////////////////////////////////////////////////////////////////////
  Menuies();
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void isrCLK() 
  {
    encoder.tick();  // отработка в прерывании
  }
void isrDT() 
  {
    encoder.tick();  // отработка в прерывании
  }
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void printGUIMAIN() 
{
  lcd.clear();  
  screenPosMAIN = arrowPosMAIN / LINES;// ищем номер экрана (0..3 - 0, 4..7 - 1)
  for (byte i = 0; i < LINES; i++) 
  {  
    lcd.setCursor(0, i);// курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPosMAIN == LINES * screenPosMAIN + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPosMAIN + i == SETTINGS_AMOUNT_MAIN) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsNamesMAIN[LINES * screenPosMAIN + i]);
  }
}
/////////////////////////////////////////////////////////////////////
void printGUIRELOAD() 
{
  lcd.clear();  
  screenPosRELOAD= arrowPosRELOAD / LINES;// ищем номер экрана (0..3 - 0, 4..7 - 1)
  for (byte i = 0; i < LINES; i++) 
  {  
    lcd.setCursor(0, i);// курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPosRELOAD == LINES * screenPosRELOAD + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPosRELOAD + i == SETTINGS_AMOUNT_RELOAD) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsNamesRELOAD[LINES * screenPosRELOAD + i]);
  }
}
/////////////////////////////////////////////////////////////////////
void printGUIPUMPS()
{
  lcd.clear();  
  screenPosPUMPS= arrowPosPUMPS / LINES;// ищем номер экрана (0..3 - 0, 4..7 - 1)
  for (byte i = 0; i < LINES; i++) 
  {  
    lcd.setCursor(0, i);// курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPosPUMPS == LINES * screenPosPUMPS + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPosPUMPS + i == SETTINGS_AMOUNT_PUMPS) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsNamesPUMPS[LINES * screenPosPUMPS + i]);
  }  
}
/////////////////////////////////////////////////////////////////////
void printGUISETTINGS()
{
  lcd.clear();  
  screenPosSETTINGS= arrowPosSETTINGS / LINES;// ищем номер экрана (0..3 - 0, 4..7 - 1)
  for (byte i = 0; i < LINES; i++) 
  {  
    lcd.setCursor(0, i);// курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPosSETTINGS == LINES * screenPosSETTINGS + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPosSETTINGS + i == SETTINGS_AMOUNT_SETTINGS) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsNamesSETTINGS[LINES * screenPosSETTINGS + i]);
    lcd.print(": ");
    lcd.print(settingsparam[LINES * screenPosSETTINGS + i]);
    lcd.print(" ml/s");
  }  
}
/////////////////////////////////////////////////////////////////////
void printGUISTART()
{
  lcd.clear();  
  screenPosSTART= arrowPosSTART / LINES;// ищем номер экрана (0..3 - 0, 4..7 - 1)
  for (byte i = 0; i < LINES; i++) 
  {  
    lcd.setCursor(0, i);// курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPosSTART == LINES * screenPosSTART + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPosSTART + i == SETTINGS_AMOUNT_START) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsNamesSTART[LINES * screenPosSTART + i]);
    lcd.print(": ");
    lcd.print(startparam[LINES * screenPosSTART + i]);
    lcd.print(" ml");
  }  
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void EncoderMainMenu() 
{
  encoder.tick();// отработка энкодера
  if (encoder.isTurn()) 
   {
    int increment = 0;// локальная переменная направления
    if (encoder.isRight()) increment = -1;
    if (encoder.isLeft()) increment = 1;
    arrowPosMAIN += increment;// двигаем курсор  
    arrowPosMAIN = constrain(arrowPosMAIN, 0, SETTINGS_AMOUNT_MAIN - 1);// ограничиваем
    increment = 0;// обнуляем инкремент  
    printGUIMAIN();
  }
}
/////////////////////////////////////////////////////////////////////
void EncoderRealoadMenu() 
{
  encoder.tick();// отработка энкодера
  if (encoder.isTurn()) 
   {
    int increment = 0;// локальная переменная направления
    if (encoder.isRight()) increment = -1;
    if (encoder.isLeft()) increment = 1;
    arrowPosRELOAD += increment;// двигаем курсор  
    arrowPosRELOAD = constrain(arrowPosRELOAD, 0, SETTINGS_AMOUNT_RELOAD - 1);// ограничиваем
    increment = 0;// обнуляем инкремент  
    printGUIRELOAD();
  }
}
/////////////////////////////////////////////////////////////////////
void EncoderPumpsMenu()
{
  encoder.tick();// отработка энкодера
  if (encoder.isTurn()) 
   {
    int increment = 0;// локальная переменная направления
    if (encoder.isRight())increment = -1;
    if (encoder.isLeft()) increment = 1;
    arrowPosPUMPS += increment;// двигаем курсор  
    arrowPosPUMPS = constrain(arrowPosPUMPS, 0, SETTINGS_AMOUNT_PUMPS - 1);// ограничиваем
    increment = 0;// обнуляем инкремент
    printGUIPUMPS();
  }
}
/////////////////////////////////////////////////////////////////////
void EncoderSettingsMenu()
{
  encoder.tick();// отработка энкодера
  if (encoder.isTurn()) 
   {
    int increment = 0;// локальная переменная направления
    if (encoder.isRight())increment = -1;
    if (encoder.isLeft()) increment = 1;
    arrowPosSETTINGS += increment;// двигаем курсор  
    arrowPosSETTINGS = constrain(arrowPosSETTINGS, 0, SETTINGS_AMOUNT_SETTINGS - 1);// ограничиваем
    increment = 0;// обнуляем инкремент
    printGUISETTINGS();
  }  
}
/////////////////////////////////////////////////////////////////////
void EncoderStartMenu()
{
  encoder.tick();// отработка энкодера
  if (encoder.isTurn()) 
   {
    int increment = 0;// локальная переменная направления
    if (encoder.isRight())increment = -1;
    if (encoder.isLeft()) increment = 1;
    arrowPosSTART += increment;// двигаем курсор  
    arrowPosSTART = constrain(arrowPosSTART, 0, SETTINGS_AMOUNT_START - 1);// ограничиваем
    increment = 0;// обнуляем инкремент
    if (encoder.isRightH()) increment = -1;
    if (encoder.isLeftH()) increment = 1;
    startparam[arrowPosSTART] += increment;
    if ((startparam[arrowPosSTART])<(0)) startparam[arrowPosSTART]=0;
    printGUISTART();
  }
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void Zeroing()
{
    digitalWrite(P1_Pin, LOW);
    digitalWrite(P2_Pin, LOW);
    digitalWrite(P3_Pin, LOW);
    digitalWrite(P4_Pin, LOW);
    CountPresses=0;
    SubstanceQuantity=0;
    arrowPosRELOAD=0;
    arrowPosPUMPS=0;
    arrowPosMAIN=0;
    arrowPosSETTINGS;
    arrowPosSTART=0;
    PressedPump=0;
    TempCounter=0;
    Counter=0;//!!!!!!!!!!!!!!!!!!!!!!!!
    printGUIMAIN();
}
/////////////////////////////////////////////////////////////////////
void ZeroingStart()
{
  startparam[PressedPump-1]=0;
  PressedPump=0;
  CountPresses=1;
  SubstanceQuantity=0;
  TempCounter=0;
  Counter=0;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void counter()
{
  Counter++;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void ManualMode()
{
  if ((arrowPosPUMPS==0)&(CountPresses==3)&(arrowPosRELOAD==1)&(PressedPump==0))
  {
    PressedPump=1;
    digitalWrite(P1_Pin, HIGH);
  }
  if ((arrowPosPUMPS==1)&(CountPresses==3)&(arrowPosRELOAD==1)&(PressedPump==0))
  {
    PressedPump=2;
    digitalWrite(P2_Pin, HIGH);
  }
  if ((arrowPosPUMPS==2)&(CountPresses==3)&(arrowPosRELOAD==1)&(PressedPump==0))
  {
    PressedPump=3;
    digitalWrite(P3_Pin, HIGH);
  }
  if ((arrowPosPUMPS==3)&(CountPresses==3)&(arrowPosRELOAD==1)&(PressedPump==0))
  {
    PressedPump=4;
    digitalWrite(P4_Pin, HIGH);
  }
/////////////////////////////////////////////////////////////////////
   if ((arrowPosRELOAD==0)&(CountPresses==2)&(arrowPosMAIN==1)) 
   {
     if (digitalRead(P1_Pin)==HIGH)
     {
       digitalWrite(P1_Pin, LOW);
       digitalWrite(P2_Pin, LOW);
       digitalWrite(P3_Pin, LOW);
       digitalWrite(P4_Pin, LOW);
      }
     else if (digitalRead(P1_Pin)==LOW)
     {
       digitalWrite(P1_Pin, HIGH);
       digitalWrite(P2_Pin, HIGH);
       digitalWrite(P3_Pin, HIGH);
       digitalWrite(P4_Pin, HIGH);
     }
     CountPresses=1;  
   }
/////////////////////////////////////////////////////////////////////
  if ((PressedPump==1)&(CountPresses==4)&(arrowPosRELOAD==1))
  {
      digitalWrite(P1_Pin, LOW);
      PressedPump=0;
      CountPresses=2;
  }
  if ((PressedPump==2)&(CountPresses==4)&(arrowPosRELOAD==1))
  {
      digitalWrite(P2_Pin, LOW);
      PressedPump=0;
      CountPresses=2;
  }
  if ((PressedPump==3)&(CountPresses==4)&(arrowPosRELOAD==1))
  {
      digitalWrite(P3_Pin, LOW);
      PressedPump=0;
      CountPresses=2;
  }
  if ((PressedPump==4)&(CountPresses==4)&(arrowPosRELOAD==1))
  {
      digitalWrite(P4_Pin, LOW);
      PressedPump=0;
      CountPresses=2;
  }
}
/////////////////////////////////////////////////////////////////////
void SettingsMode()
{
  if ((arrowPosSETTINGS==0)&(CountPresses==2)&(arrowPosMAIN==2)&(PressedPump==0))
  {
    PressedPump=1;
    digitalWrite(P1_Pin, HIGH);
    TempCounter=Counter;
  }
  if ((arrowPosSETTINGS==1)&(CountPresses==2)&(arrowPosMAIN==2)&(PressedPump==0))
  {
    PressedPump=2;
    digitalWrite(P2_Pin, HIGH);
    TempCounter=Counter;
  }
  if ((arrowPosSETTINGS==2)&(CountPresses==2)&(arrowPosMAIN==2)&(PressedPump==0))
  {
    PressedPump=3;
    digitalWrite(P3_Pin, HIGH);
    TempCounter=Counter;
  }
  if ((arrowPosSETTINGS==3)&(CountPresses==2)&(arrowPosMAIN==2)&(PressedPump==0))
  {
    PressedPump=4;
    digitalWrite(P4_Pin, HIGH);
    TempCounter=Counter;
  }
/////////////////////////////////////////////////////////////////////
  x=100;// количество милилитров в колбе  
  if ((PressedPump==1)&(CountPresses==3)&(arrowPosMAIN==2))
  {
    digitalWrite(P1_Pin, LOW);
    BandwidthP1=x/(Counter-TempCounter)*0.1;//мл/ сек
    eeprom_write_float(0,BandwidthP1);//запарос пр запуске для правильнотси
    settingsparam[arrowPosSETTINGS]=BandwidthP1;
    PressedPump=0;
    CountPresses=1;
    printGUISETTINGS();
    Serial.println(BandwidthP1);
  }
  if ((PressedPump==2)&(CountPresses==3)&(arrowPosMAIN==2))
  {
    digitalWrite(P2_Pin, LOW);
    BandwidthP2=x/(Counter-TempCounter)*0.1;
    eeprom_write_float(4,BandwidthP2);//запарос пр запуске для правильнотси
    PressedPump=0;
    CountPresses=1;
    //Serial.println(BandwidthP2);
  }
  if ((PressedPump==3)&(CountPresses==3)&(arrowPosMAIN==2))
  {
    digitalWrite(P3_Pin, LOW);
    BandwidthP3=x/(Counter-TempCounter)*0.1;
    eeprom_write_float(8,BandwidthP3);//запарос пр запуске для правильнотси
    PressedPump=0;
    CountPresses=1;
    //Serial.println(BandwidthP3);
  }
  if ((PressedPump==4)&(CountPresses==3)&(arrowPosMAIN==2))
  {
    digitalWrite(P4_Pin, LOW);
    BandwidthP4=x/(Counter-TempCounter)*0.1;
    eeprom_write_float(12,BandwidthP4);//запарос пр запуске для правильнотси
    PressedPump=0;
    CountPresses=1;
    //Serial.println(BandwidthP4);
  }
}
/////////////////////////////////////////////////////////////////////  
void StartMode()
{
  //BandwidthP2Read=eeprom_read_float(4);
  //BandwidthP3Read=eeprom_read_float(8);
  //BandwidthP4Read=eeprom_read_float(12);
  if ((arrowPosSTART==0)&(CountPresses==2)&(arrowPosMAIN==0)&(PressedPump==0))
  {
    SubstanceQuantity=startparam[0];    
    BandwidthP1Read=eeprom_read_float(0);//производительность милилитров в секунду
    PressedPump=1;
    TempCounter=Counter*0.1;
    TempTime=(SubstanceQuantity/BandwidthP1Read);// время на которое нужно включить насос
    digitalWrite(P1_Pin, HIGH);
    //Serial.println(TempCounter+TempTime);
    //Serial.println(TempTime);
  }
  if ((arrowPosSTART==0)&(CountPresses==2)&(arrowPosMAIN==0)&(PressedPump==1))
  {
    //Serial.println(Counter*0.1);
    //delay(1000);
    if (Counter*0.1==(TempCounter+TempTime))
    {
      digitalWrite(P1_Pin, LOW);
      ZeroingStart();
    }
  }
}
/////////////////////////////////////////////////////////////////////  
/////////////////////////////////////////////////////////////////////
void Menuies()
{
  if (CountPresses==0)
  {
      EncoderMainMenu();    
  }
  if (CountPresses==1)
  {
    if (arrowPosMAIN==0)
    {
      EncoderStartMenu();
    }
    if (arrowPosMAIN==1)
    { 
      EncoderRealoadMenu();
    } 
    if ((arrowPosMAIN==2))
    {
      EncoderSettingsMenu();  
    }
    if (arrowPosMAIN==0)
    {
      EncoderStartMenu();  
    }
  }
  if ((CountPresses==2))
  {
    if (arrowPosRELOAD==1)
    {
      EncoderPumpsMenu();
    }
    if (arrowPosMAIN==2)
    {
      EncoderSettingsMenu();  
    }
  }
  if ((CountPresses==3))
  {
    if (arrowPosRELOAD==1)
    {
      EncoderPumpsMenu();
    }
  }
}
