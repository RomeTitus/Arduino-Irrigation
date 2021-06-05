/*
  Arduino Slave for Raspberry Pi Master
  i2c_slave_ard.ino
  Connects to Raspberry Pi via I2C
  
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/

// Include the Wire library for I2C
#include <Wire.h>

String MessageFromPi = "";
String MessageForPi = "0";
void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // Join I2C bus as slave with address 04
  Wire.begin(0x04);
  
  // Call receiveEvent when data received                
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

// Function that executes whenever data is received from master
void receiveEvent(int byteCount) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    MessageFromPi = MessageFromPi + String(c);
  }
    if(byteCount == 1){
    return;
  }
  //Serial.println("Massage Recoived");
  //Serial.println("Arduino got: " + String(MessageFromPi));

  String pinNumber = getValue(MessageFromPi, ',', 0);
  int sensorVal;
  uint8_t  uint8Pin  = atoi (pinNumber.substring(1, 3).c_str ());
  if (getValue(MessageFromPi, ',', 1).equals("read")) {

      if (getPinType(pinNumber) == 0) {
        //Analog
        MessageForPi = analogRead(uint8Pin);
      } else {
        MessageForPi = digitalRead(uint8Pin);
        //Digital
      }
      //Serial.println("Read Value was: " + String(MessageForPi));
      //delay(500);
  } else {
      MessageForPi = "";
      int output = 0;
      if (getValue(MessageFromPi, ',', 2).equals("Off")) {
        output = 1;
      }
      if (getPinType(pinNumber) == 0) {
        //Analog
        pinMode(uint8Pin, OUTPUT);
        //Serial.println("Turning Analog " + String(output) + "Using Pin: " + String(uint8Pin));
        analogWrite(uint8Pin, output);
      } else {
        //Serial.println("Turning Digital " + String(output) + "Using Pin: " + String(uint8Pin));
        pinMode(uint8Pin, OUTPUT);
        digitalWrite(uint8Pin, output);
      }  
    }
  MessageFromPi = "";
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  //Wire.print(MessageFromPi); // respond with message of 6 bytes
  Wire.print(String(MessageForPi));
  MessageForPi = "";
  // as expected by master
}
void loop() {
  delay(100);
}

int getPinType(String str) {

  // Length (with one extra character for the null terminator)
  int str_len = str.length() + 1;

  // Prepare the character array (the buffer)
  char char_array[str_len];

  // Copy it over
  str.toCharArray(char_array, str_len);
  if (char_array[0] == 'A') {
    return 0;
  }
  return 1;
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length();

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
