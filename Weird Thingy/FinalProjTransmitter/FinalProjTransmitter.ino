#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

const int PulseWire = 0;
int Threshold = 550; //Need to Calibrate Threshold
PulseSensorPlayground pulseSensor;
int bpmThreshold = 90; //Need to Calibrate bpm Threshold
int laserPin =8;

#include <SPI.h>
#include <RH_RF95.h>
#define RF95_FREQ 915.0
RH_RF95 rf95;

int switchPin = 3;
int buttonPin = 2;

void setup() {
  Serial.begin(9600);
  
  pinMode(switchPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(laserPin, OUTPUT);
  //Pulse Sensor Setup
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(13);
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    Serial.println("Pulse Sensor Initialized!");  
  }
  //LoRa Setup
  while (!rf95.init()) {
    Serial.println("LoRa Failed ...");
    while (1);
  }
  Serial.println("LoRa Initialized!");
  //LoRa setFreq
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  Serial.println("");
  Serial.println("");
  
  //Transmitter Power
  rf95.setTxPower(23, false);
}



int16_t packetnum = 0;

/*
System = (____, ____)
Manual = (1,1) - take pic, or (1,0) - don't take pic
Auto = (0, 1) if bpm high, (0,0) if low
*/

void loop() {

  while(checkSwitch()==true){
    manual();
  }
  while(checkSwitch()==false){
    automated();
  }
  delay(20);
}

void manual(){
  if(buttonPressed()==true){
    Serial.println("(1,1)");
    digitalWrite(laserPin, HIGH);
  }
  else{
    Serial.println("(1,0)");
    digitalWrite(laserPin, LOW);
  }
}

void automated(){
  digitalWrite(laserPin, LOW);
  if(bpmCalc()>=bpmThreshold){
    Serial.println("(0,1)");
   }
  if(bpmCalc()<=bpmThreshold){
    Serial.println("(0,0)");
  }
}

bool checkSwitch(){
  if (digitalRead(switchPin)==HIGH){
    return true;
  }
  else{
    return false;
  }
}

int bpmCalc(){
  return pulseSensor.getBeatsPerMinute();
}

bool buttonPressed(){
  if(digitalRead(buttonPin)==HIGH){
    return true;
  }
  else{
    return false;
  }
}
