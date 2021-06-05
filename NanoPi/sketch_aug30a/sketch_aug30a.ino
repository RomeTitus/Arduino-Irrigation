/*
  Arduino Slave for Raspberry Pi Master
  i2c_slave_ard.ino
  Connects to Raspberry Pi via I2C
  
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/

// Include the Wire library for I2C
#include <Wire.h>

// LED on pin 13
const int ledPin = 13; 
String MessageFromPi = "";
void setup() {
   Serial.begin(9600);
   Serial.println("Debugger On");
  // Join I2C bus as slave with address 04
  Wire.begin(0x04);
  
  // Call receiveEvent when data received                
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  // Setup pin 13 as output and turn LED off
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

// Function that executes whenever data is received from master
void receiveEvent(int howMany) {
  Serial.println("Massage Recoived");
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    
    Serial.print(c);
    MessageFromPi = MessageFromPi + String(c);
  }
  
  Serial.println("\nArduino got: " + String(MessageFromPi));

  
  if(MessageFromPi.equals("on")){
  digitalWrite(ledPin, HIGH);
  }
  else if(MessageFromPi.equals("off")){
  digitalWrite(ledPin, LOW);
  }
  
  Serial.write("\n");
  
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  //Wire.print(MessageFromPi); // respond with message of 6 bytes
  Wire.print(String(MessageFromPi));
  MessageFromPi = "";
  // as expected by master
}
void loop() {
  delay(100);
}
