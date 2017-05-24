#include <SPI.h>
#include <DS1302.h>
#include <DS1307.h>
#include "JeeLib.h"
#include <LiquidCrystal.h>
#include <SD.h>
File file,file1,myFile;
#include <EEPROM.h>
LiquidCrystal lcd(9,8,5,4,3,2);
DS1302 rtc(A2,A1,A0);
const int sensorPin = A4;    // pin that the sensor is attached to
const int ledPin = 9;        // pin that the LED is attached to
// variables:
float s1 = 0.0;         // the sensor value
int sensorMin = 1024;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value
int addr = 0;
int address = 0;
byte value;
ISR(WDT_vect){Sleepy::watchdogEvent();}

byte newChar1[8] = {
  B01000,
  B10100,
  B01000,
  B00011,
  B00100,
  B00100,
  B00011,
  B00000
};
void setup()
{
  //TCCR2B = TCCR2B & 0b11111000 | 0x04;
  //SPI.begin();
  //SPI.setClockDivider(105);
  //CLKPR=10000110;
  analogReference(INTERNAL);
  rtc.halt(false);
  rtc.writeProtect(false);
  
  pinMode(10, OUTPUT);
  lcd.begin(16,2);
  lcd.createChar(0, newChar1);
  lcd.clear();
  
  pinMode(A3, OUTPUT);  
  pinMode(0,INPUT_PULLUP);
  pinMode(1,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);

  lcd.setCursor(4,0);
  lcd.print("MINI PROJECT");
  digitalWrite(A3,LOW);
  delay(2000);
  SD.begin(10);
  lcd.setCursor(8,0);
  lcd.clear();
  
  while(1)
  {
    file=SD.open("tempdata.csv",FILE_WRITE);
    file.println(" , , , "); 
    String name1 = " ,//,//, ";
    file.println(name1);
    file.println();
    file.println(",TEMP ,DATA LOGGER ,");
    file.close();
    break;
  }

}

void mode(int *);
int modify(int ,int );
void run();

void set_time();
int modify_t_d(int ,unsigned char ,unsigned char );
int modify_i_c(int , int ,int );
void set_interval(int ,int ,int ,int );
void setdate();
void cal(int ,int ,int ,int );

unsigned char j2=0,j3=0,j4=0,j5=1;
unsigned char c2=1,c3=0,c4=0,c5=0;
char flag=0,f=0,i1=0;

void loop()
{
  while(1)
  {
    file=SD.open("tempdata.csv",FILE_WRITE);
    if(file)
    {
      file.println("  ,  ,  ,  ");
      String head = "  Date , Time, Temp *C,";
      file.println(head);
      file.println(" , , , ");
      file.close();  
    }
    break;
  }

  run();
  
  if(digitalRead(0)==0)
  {
    while(1)
    {   
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("RUN");
      lcd.setCursor(10,0);
      lcd.print("Press");
      lcd.setCursor(12,1);
      lcd.print("ENT");
      delay(500);
      while(digitalRead(0)==1)
      {
        if(digitalRead(1)==0)
          run();
      }

      lcd.clear();
      lcd.setCursor(0,0);
      delay(500);
      lcd.print("Set Time");      
      set_time();
      
      lcd.clear();
      lcd.setCursor(0,0);
      delay(500);
      lcd.print("Set Date");      

      setdate();
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Print");
      lcd.setCursor(0,1);
      lcd.print("Interval:");

      delay(500);
      while(digitalRead(0)==1)
      {
        lcd.setCursor(0,1);
        set_interval(j2,j3,j4,j5);
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CAL");
      lcd.setCursor(10,0);
      lcd.print("Press");
      lcd.setCursor(12,1);
      lcd.print("ENT");

      delay(500);
      while(digitalRead(0)==1)
      {
        if(digitalRead(1)==0)
        {
          lcd.clear();
          cal(c2,c3,c4,c5);
        }
      }
    }
  }
}
int i2=0,s2;
void run()
{
  digitalWrite(A3,HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp");

  j2=EEPROM.read(0);
  j3=EEPROM.read(1);
  j4=EEPROM.read(2);
  j5=EEPROM.read(3);

  c2=EEPROM.read(4);
  c3=EEPROM.read(5);
  c4=EEPROM.read(6);
  c5=EEPROM.read(7);

  int d,w,v,count=0;
  //digitalWrite(A3,HIGH);
  while(digitalRead(0)==1)
  {
    lcd.setCursor(0,1);
    s1=analogRead(A4);

    //lcd.print(((c2*1000+c3*100+c4*10+c5)*s1*0.001075*100)/1000);
    v=((c2*1000+c3*100+c4*10+c5)*s1*0.001075*100)/1000;
    w=((c2*1000+c3*100+c4*10+c5)*s1*0.001075*1000)/1000;
    d=(w%10);
    lcd.print(v);lcd.print(".");lcd.print(d);
    lcd.setCursor(5,1);
    lcd.write(byte(0));
    int s3=analogRead(A5);
    /*lcd.setCursor(4,0);
    lcd.print(s3);*/
    if(count==2)
    {
      lcd.setCursor(6,0);
      lcd.print("   ");
    }
    else if(s3<59 && count==3)
    {
      lcd.setCursor(6,0);
      lcd.print("B.L");
      count=0;
    }  
    else if(s3>=59)
    count=0;
    lcd.setCursor(11, 0);
    //FORMAT_LITTLEENDIAN
    lcd.print(rtc.getTime1Str(FORMAT_LITTLEENDIAN));
    
    lcd.setCursor(8, 1);
    lcd.print(rtc.getDateStr(FORMAT_SHORT));
    
    s2=(j2*1000+j3*100+j4*10+j5)*1.94;
    //if(i2==((j2*1000+j3*100+j4*10+j5)*1.81-1))
    //if(i2==((j2*1000+j3*100+j4*10+j5)*2)-28)
    //if(i2==((j2*1000+j3*100+j4*10+j5)*2)-3)
    if(i2==s2-1)
     digitalWrite(A3,LOW);
 
    if(i2==s2)
    //if(i2==(((j2*1000+j3*100+j4*10+j5)*2)-27))
    //if(i2==(((j2*1000+j3*100+j4*10+j5)*2)-2)) 
    {
      
      SD.begin(10);
       //SPI.setClockDivider(105);
       String data = String(rtc.getDateStr()) + "," + String(rtc.getTime1Str()) + "," + String(v) + '.' + String(d);
       file=SD.open("tempdata.csv",FILE_WRITE);
        if(file)
        {
          file.println(data);
          file.close();  
        }
        //file.close();
        i2=0;
       digitalWrite(A3,HIGH);
    }
    i2=i2+1;
    count=count+1;
    Sleepy::loseSomeTime(998);
    
    if(digitalRead(0)==0)
      break;
  } 
}
void set_time()
{
  lcd.setCursor(0,1);
  lcd.print(rtc.getTime1Str());
  int h=00,m=00,s=00;
  unsigned char f1=0;
  Time t;
  t=rtc.getTime1();
  while(1)
  {
  if(digitalRead(1)==0)
  {
   lcd.setCursor(0,1); 
   lcd.print("00:00:00");
  while(digitalRead(0)==1)
  {
    delay(500);
    lcd.setCursor(1,1);
    lcd.blink();
    if(f1==0)
    h=modify_t_d(0,0,24);
    else
    h=modify_t_d(h,0,24);
    rtc.setTime(h,m,s);
    if(digitalRead(0)==0)
     break;
    
    lcd.noBlink();
    delay(500);
    lcd.setCursor(4,1);
    lcd.blink();
    if(f1==0)
    m=modify_t_d(0,3,60);
    else
    m=modify_t_d(m,3,60);
    rtc.setTime(h,m,s);

    if(digitalRead(0)==0)
     break;
   
    lcd.noBlink();
    delay(500);
    lcd.setCursor(7,1);
    lcd.blink();
    if(f1==0)
    s=modify_t_d(0,6,60);
    else
    s=modify_t_d(s,6,60);
    rtc.setTime(h,m,s);

    if(digitalRead(0)==0)
     break;
 
    lcd.noBlink();
    delay(190);
    f1=f1+1;
    
  }
  }
  else if(digitalRead(0)==0)
  break;
  }
  lcd.noBlink();
  f1=0;
}

void setdate()
{
 
  Time t;
  t=rtc.getTime1();
  
  lcd.setCursor(0,1);
  lcd.print(rtc.getDateStr(FORMAT_SHORT));
  int d,mo,y;
  unsigned char f2=0;
  while(1)
  {
  if(digitalRead(1)==0)
  {
   lcd.setCursor(0,1); 
   lcd.print("00:00:00");
  while(digitalRead(0)==1)
  {
    delay(500);
    lcd.setCursor(1,1);
    lcd.blink();
    if(f2==0)
    d=modify_t_d(0,0,32);
    else
    d=modify_t_d(d,0,32);
    rtc.setDate(d,mo,2000+y);
    if(digitalRead(0)==0)
     break;
   
    lcd.noBlink();
    delay(190);
    lcd.setCursor(4,1);
    lcd.blink();
    if(f2==0)
    mo=modify_t_d(0,3,13);
    else
    mo=modify_t_d(mo,3,13);
    rtc.setDate(d,mo,2000+y);
    if(digitalRead(0)==0)
     break;

    lcd.noBlink();
    delay(500);
    lcd.setCursor(7,1);
    lcd.blink();
    if(f2==0)
    y=modify_t_d(0,6,100);
    else
    y=modify_t_d(y,6,100);
    rtc.setDate(d,mo,2000+y);
    if(digitalRead(0)==0)
     break;

    lcd.noBlink();
    delay(500);
    f2=f2+1;
  }
  }
  else if(digitalRead(0)==0)
   break;
  }
  lcd.noBlink();
  f2=0;
}
void set_interval(int j2,int j3,int j4,int j5)
{ 
  int a;
  j2=EEPROM.read(0);
  j3=EEPROM.read(1);
  j4=EEPROM.read(2);
  j5=EEPROM.read(3);  
  lcd.setCursor(0,1);
  lcd.print("Interval:");
  lcd.setCursor(14,1);
  lcd.print("s");

  lcd.setCursor(9,1);
  lcd.print(j2);
  lcd.setCursor(10,1);
  lcd.print(j3);
  lcd.setCursor(11,1);
  lcd.print(j4);
  lcd.setCursor(12,1);
  lcd.print(j5);

  while(digitalRead(0)==1)
  { 
    if(digitalRead(0)==0)
      break;
    delay(250); 
    while(digitalRead(1)==1)
    { 

      j2=EEPROM.read(0);
      j3=EEPROM.read(1);
      j4=EEPROM.read(2);
      j5=EEPROM.read(3);  

      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(9,0);
      lcd.print("v");

      a=modify_i_c(0,j2,9);
      j2=a;

      EEPROM.write(0,j2);
      EEPROM.write(1,j3);
      EEPROM.write(2,j4);
      EEPROM.write(3,j5);

      lcd.setCursor(9,1);
      lcd.print(j2);
      lcd.setCursor(10,1);
      lcd.print(j3);
      lcd.setCursor(11,1);
      lcd.print(j4);
      lcd.setCursor(12,1);
      lcd.print(j5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500);
    while(digitalRead(1)==1)
    {

      j2=EEPROM.read(0);
      j3=EEPROM.read(1);
      j4=EEPROM.read(2);
      j5=EEPROM.read(3);  

      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(10,0);
      lcd.print("v");

      a=modify_i_c(0,j3,10);
      j3=a;

      lcd.setCursor(9,1);
      lcd.print(j2);
      lcd.setCursor(10,1);
      lcd.print(j3);
      lcd.setCursor(11,1);
      lcd.print(j4);
      lcd.setCursor(12,1);
      lcd.print(j5);

      EEPROM.write(0,j2);
      EEPROM.write(1,j3);
      EEPROM.write(2,j4);
      EEPROM.write(3,j5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500); 
    while(digitalRead(1)==1)
    {

      j2=EEPROM.read(0);
      j3=EEPROM.read(1);
      j4=EEPROM.read(2);
      j5=EEPROM.read(3);  

      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(11,0);
      lcd.print("v");

      a=modify_i_c(0,j4,11);
      j4=a; 

      lcd.setCursor(9,1);
      lcd.print(j2);
      lcd.setCursor(10,1);
      lcd.print(j3);
      lcd.setCursor(11,1);
      lcd.print(j4);
      lcd.setCursor(12,1);
      lcd.print(j5);

      EEPROM.write(0,j2);
      EEPROM.write(1,j3);
      EEPROM.write(2,j4);
      EEPROM.write(3,j5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500);
    while(digitalRead(1)==1)
    {

      j2=EEPROM.read(0);
      j3=EEPROM.read(1);
      j4=EEPROM.read(2);
      j5=EEPROM.read(3);  

      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(12,0);
      lcd.print("v");

      a=modify_i_c(0,j5,12);
      j5=a;

      EEPROM.write(0,j2);
      EEPROM.write(1,j3);
      EEPROM.write(2,j4);
      EEPROM.write(3,j5);

      lcd.setCursor(9,1);
      lcd.print(j2);
      lcd.setCursor(10,1);
      lcd.print(j3);
      lcd.setCursor(11,1);
      lcd.print(j4);
      lcd.setCursor(12,1);
      lcd.print(j5);

      if(digitalRead(0)==0)
        break;
  }



    if(digitalRead(0)==0)
      break;
  }
}     
int modify_t_d(int a,unsigned char n,unsigned char L)
{

  while(digitalRead(1)==1)
  {
    if(digitalRead(6)==0)  
    {
      if(a<1)
        a=L;
      a=a-1;
    delay(200);
    if (a <= 9)
      {
        lcd.setCursor(n,1); 
        lcd.print("0");
        lcd.setCursor(n+1,1);
        lcd.print(a);
      }

     else
      {
            lcd.setCursor(n,1);
            lcd.print(a);
      }
    } 
    else if(digitalRead(7)==0)
    {
      if(a>L-2)
        a=-1; 
      a=a+1;
      delay(200);
    if (a <= 9)
      {
        lcd.setCursor(n,1); 
        lcd.print("0");
        lcd.setCursor(n+1,1);
        lcd.print(a);
      }

     else
      {
            lcd.setCursor(n,1);
            lcd.print(a);
      }
    }
    else if(digitalRead(0)==0)
      break; 
  }
  return(a);
}

void cal(int c2,int c3,int c4,int c5)
{ 
  int c;
  c2=EEPROM.read(4);
  c3=EEPROM.read(5);
  c4=EEPROM.read(6);
  c5=EEPROM.read(7);  
  lcd.setCursor(0,1);
  lcd.print("Corr fac:");
  lcd.setCursor(10,1);
  lcd.print("."); 
  lcd.setCursor(9,1);
  lcd.print(c2);
  lcd.setCursor(11,1);
  lcd.print(c3);
  lcd.setCursor(12,1);
  lcd.print(c4);
  lcd.setCursor(13,1);
  lcd.print(c5);

  while(digitalRead(0)==1)
  { 
    if(digitalRead(0)==0)
      break;
    delay(500); 
    while(digitalRead(1)==1)
    { 

      c2=EEPROM.read(4);
      c3=EEPROM.read(5);
      c4=EEPROM.read(6);
      c5=EEPROM.read(7);  

      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(9,0);
      lcd.print("v");

      c=modify_i_c(1,c2,9);
      c2=c;

      EEPROM.write(4,c2);
      EEPROM.write(5,c3);
      EEPROM.write(6,c4);
      EEPROM.write(7,c5);

      lcd.setCursor(9,1);
      lcd.print(c2);
      lcd.setCursor(11,1);
      lcd.print(c3);
      lcd.setCursor(12,1);
      lcd.print(c4);
      lcd.setCursor(13,1);
      lcd.print(c5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500);
    while(digitalRead(1)==1)
    {

      c2=EEPROM.read(4);
      c3=EEPROM.read(5);
      c4=EEPROM.read(6);
      c5=EEPROM.read(7);  

      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(11,0);
      lcd.print("v");

      c=modify_i_c(1,c3,11);
      c3=c;

      lcd.setCursor(9,1);
      lcd.print(c2);
      lcd.setCursor(11,1);
      lcd.print(c3);
      lcd.setCursor(12,1);
      lcd.print(c4);
      lcd.setCursor(13,1);
      lcd.print(c5);

      EEPROM.write(4,c2);
      EEPROM.write(5,c3);
      EEPROM.write(6,c4);
      EEPROM.write(7,c5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500); 
    while(digitalRead(1)==1)
    {
      c2=EEPROM.read(4);
      c3=EEPROM.read(5);
      c4=EEPROM.read(6);
      c5=EEPROM.read(7);  

      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(12,0);
      lcd.print("v");

      c=modify_i_c(1,c4,12);
      c4=c; 

      lcd.setCursor(9,1);
      lcd.print(c2);
      lcd.setCursor(11,1);
      lcd.print(c3);
      lcd.setCursor(12,1);
      lcd.print(c4);
      lcd.setCursor(13,1);
      lcd.print(c5);

      EEPROM.write(4,c2);
      EEPROM.write(5,c3);
      EEPROM.write(6,c4);
      EEPROM.write(7,c5);

      if(digitalRead(0)==0)
        break;
    }
    if(digitalRead(0)==0)
      break;
    delay(500);
    while(digitalRead(1)==1)
    {

      c2=EEPROM.read(4);
      c3=EEPROM.read(5);
      c4=EEPROM.read(6);
      c5=EEPROM.read(7);  

      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(13,0);
      lcd.print("v");

      c=modify_i_c(1,c5,13);
      c5=c;

      EEPROM.write(4,c2);
      EEPROM.write(5,c3);
      EEPROM.write(6,c4);
      EEPROM.write(7,c5);

      lcd.setCursor(9,1);
      lcd.print(c2);
      lcd.setCursor(11,1);
      lcd.print(c3);
      lcd.setCursor(12,1);
      lcd.print(c4);
      lcd.setCursor(13,1);
      lcd.print(c5);

      if(digitalRead(0)==0)
        break;
    }



    if(digitalRead(0)==0)
      break;
  }
}     
int modify_i_c(int p, int a,int n)
{
  if (p == 1)
  {
    lcd.setCursor(0,1);
    lcd.print("Corr fac:");
    lcd.setCursor(10,1);
    lcd.print("."); 
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("Interval:");
    lcd.setCursor(14,1);
    lcd.print("s");
  }
  while(digitalRead(1)==1)
  {
    if(digitalRead(6)==0)  
    {
      if(a<1)
        a=10;
      a=a-1;
      delay(200);
      lcd.setCursor(n,1); 
      lcd.print(a);
    } 
    else if(digitalRead(7)==0)
    {
      if(a>8)
        a=-1; 
      a=a+1;
      delay(200);
      lcd.setCursor(n,1); 
      lcd.print(a);
    }
    else if(digitalRead(0)==0)
      break; 
  }
  lcd.print("         ");
  return(a);
}
