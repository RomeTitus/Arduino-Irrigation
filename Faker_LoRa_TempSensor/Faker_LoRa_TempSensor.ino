#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define tempSensor 3 //On digital pin 3
String header = "abbb", key = "13", controllerKey = "29";
OneWire oneWire_in(tempSensor);
DallasTemperature coolerRoomSensor(&oneWire_in);


const int nssPin = 10, resetPin = 6, irqPin = 2, spreadingFactor = 12;         // LoRa radio chip select, LoRa radio reset, must be a hardware interrupt pin  [2,3]
int interval = 2000;
float oldTemp1 = -99;
String outgoing;              // outgoing message
long lastSendTime = 0;        // last send time

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("LoRa Duplex");
  
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(nssPin, resetPin, irqPin);// set CS, reset, IRQ pin
  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("404 LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSpreadingFactor(spreadingFactor);           // ranges from 6-12,default 7 see API docs
  coolerRoomSensor.begin();
}
void loop() {
  // Run only when you receive data for serial port:
  
  String readString = "";
  
   while (Serial.available()) {
    delay(3);
    if (Serial.available() >0) {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
  }
  if (readString.length() == 0 && millis() - lastSendTime > interval) {
    coolerRoomSensor.requestTemperatures();
    float temp = coolerRoomSensor.getTempCByIndex(0);
    if(temp > oldTemp1 + 0.5 || temp < oldTemp1 -0.5){
      oldTemp1 = temp;
      readString = "►" + header +controllerKey +"8" +key + String(temp) + "◄";
      Serial.println(readString);
    }
    lastSendTime = millis();            // timestamp the message
    interval = random(4000) + 4000;    // 4-8 seconds
  }
  
   if (readString.length() >0) {
    sendMessage(readString);                       // wait for a second
   }

  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  //Lora.Write is not needed but we can use it to make sure all of the Packets arrived :) maybe to a hex total so we know packets is corrupt if one number/letter changes as the length will remain the same
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  // read packet header bytes:
  byte incomingLength = LoRa.read();    // incoming msg length
  String incoming = "";
  
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("404: message length does not match Header length");
    return;                             // skip rest of function
  }
  Serial.println(incoming + "RSSI: " + String(LoRa.packetRssi()) + "Snr: " + String(LoRa.packetSnr()));
  
}
