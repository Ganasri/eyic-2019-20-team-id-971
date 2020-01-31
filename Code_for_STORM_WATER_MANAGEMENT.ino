#define TRIGGER D4   //trigger is defined as port D4 of NODEMCU
#define ECHO    D5   //echo is defined as port D4 of NODEMCU
#define PULSE_PIN D2  //gpio4   //gpio13
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "3y9MSjEnD2DnzFIu-xBoF6F1g6aNzHba";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Manikanta"; //user name for hotsopt or wifi
char pass[] = "manikanta007"; // password
//declared the type of variables we used in the code

volatile long pulseCount=0;
float calibrationFactor = 4.5;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;

unsigned long oldTime;

void ICACHE_RAM_ATTR pulseCounter()
{
  pulseCount++;
}
// initialising the pins and values
void setup()
{
  pinMode(D7,OUTPUT);
  pinMode(D3,OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
 pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PULSE_PIN, INPUT);
//  pinMode(PULSE_PIN, INPUT_PULLUP);
//  digitalWrite(PULSE_PIN, HIGH);   ????
  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
}
// the code to interface flowsensor, ultrasonicsensor, dc motor to nodemcu.
void loop()
{
  // the below is the code for ultrasonic sensor.
  long duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1; // logic to know the distance.
  Blynk.virtualWrite(V0,distance);// to know the height of water level in mobile through blynk app.
   
  
  
  Serial.print(distance);
  Serial.println("Centimeter:");
   delay(200);
  Blynk.run();
   // the following is the code for flow sensor.
   if((millis() - oldTime) > 1000)    // Only process counters once per second.
   {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;    
    totalLitres = totalMilliLitres * 0.001;
    unsigned int frac;  
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    Blynk.virtualWrite(V1,flowRate);
    // comment the below code if there is no need of quantity of liquid flow
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalLitres);
    Serial.println("L");// comment till here.
    
    pulseCount = 0;

    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
    Blynk.run();
    if(flowRate==0) // when there is no flow i.e flowrate=0 the motor starts rotating 
    // the motor to rotate in clokwise direction
    analogWrite(D7,0);
    analogWrite(D3,170);
    delay(4000);
     // the motor to rotate in anticlokwise direction i.e to comeback initial position
    analogWrite(D7,170);
    analogWrite(D3,0);

    delay(4000);
    
    
  }


}
