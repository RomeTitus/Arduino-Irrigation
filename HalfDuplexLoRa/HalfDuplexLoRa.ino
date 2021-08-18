//https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <LiquidCrystal.h>
const int nssPin = 10, resetPin = 6, irqPin = 2;          // LoRa radio chip select, LoRa radio reset, must be a hardware interrupt pin  [2,3]
String outgoing;              // outgoing message
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
int spreadingFactor = 12, txPower = 20;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("LoRa Duplex");
  lcd.begin(16, 2);
  lcd.print("LoRa Duplex");
  
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(nssPin, resetPin, irqPin);// set CS, reset, IRQ pin
  
  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("404 LoRa init failed. Check your connections.");
    lcd.setCursor(0,1);
    lcd.print("LoRa failed! :(");
    while (true);                       // if failed, do nothing
  }
  LoRa.setTxPower(txPower);
  LoRa.setSpreadingFactor(spreadingFactor);           // ranges from 6-12,default 7 see API docs
    lcd.setCursor(0,1);
    lcd.print("LoRa Spread: " + String(spreadingFactor));
    Serial.println("LoRa Spread: " + String(spreadingFactor));
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sent" + outgoing);
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(incoming);
  lcd.setCursor(0,1);
  lcd.print("RSSI" + String(LoRa.packetRssi()) + "Snr" + String(LoRa.packetSnr()));
  
}
