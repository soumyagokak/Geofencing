// Created by: Electro freaks
//College :JAIN COLLEGE OF ENGINEERING AND RESEARCH, BELAGAVI, KARNATAKA.
#include <SoftwareSerial.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd1(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

SoftwareSerial sim800L(2,3); 
String buff;
//--------------------------------------------------------------
#define xPin A1
#define yPin A2
#define zPin A3
//--------------------------------------------------------------

byte updateflag;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 150;
double angle;
boolean impact_detected = false;

unsigned long time1;

void setup()
{
  
  Serial.begin(9600);
  time1 = micros(); 
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  lcd1.begin(16,2);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);

  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  // Serial.begin(115200);
  
  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);

  Serial.println("Initializing...");
  
  sim800L.println("AT");
  waitForResponse ();

  sim800L.println("ATE1");
  waitForResponse();

  sim800L.println("AT+CMGF=1");
  waitForResponse();

  sim800L.println("AT+CNMI=1,2,0,0,0");
  waitForResponse();
}


void loop()
{

  if (micros() - time1 > 1999) Impact();
  if(updateflag > 0) 
  {
    updateflag=0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude:"); Serial.println(magnitude);
    lcd1.setCursor(0, 0);
    lcd1.print("Impact detected!!");
    lcd1.setCursor(0,1);
    lcd1.print("Magnitude: ");
    lcd1.print(magnitude);
    delay(5000);
    lcd1.clear();
    digitalWrite(5,LOW);

    make_call();

    impact_detected = true;
  }
}






void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  int oldx = xaxis; //store previous axis readings for comparison
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  
  
  vibration--; 
  
  if(vibration < 0) vibration = 0;                                

  
  if(vibration > 0) return;
  //--------------------------------------------------------------
  deltx = xaxis - oldx;                                           
  delty = yaxis - oldy;
  deltz = zaxis - oldz;
  
  
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  
  if (magnitude >= sensitivity) //impact detected
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
  }
 
  else
  {
    magnitude=0;
  }
  
}

void make_call()
{
  sim800L.println("ATD+918105909664;");
  waitForResponse();
}

void waitForResponse()
{
  delay(1000);
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  sim800L.read();
}
