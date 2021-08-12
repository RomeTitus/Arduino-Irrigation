#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <LiquidCrystal.h>
String forwarding[5];
String helpText = "Help Commands:\nREPEAT [01,02] [03,04]\tThere can be up to 5 diffrent repeater setup";
const int nssPin = 10, resetPin = 6, irqPin = 2;          // LoRa radio chip select, LoRa radio reset, must be a hardware interrupt pin  [2,3]
String outgoing;              // outgoing message
//const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
const int rs = 8, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
int spreadingFactor = 12;

byte msgCount = 0; 
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

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
      
     // if(readString.substring(0,6).equalsIgnoreCase("repeat")){
     //   setForwardingKeys(readString.substring(6));
     //}else if(readString.substring(0,1) == "?" || readString.substring(0,4).equalsIgnoreCase("help")){
     //   Serial.println(helpText);
     //}
     //else{
        //Serial.println("success" + "RSSI: " + String(LoRa.packetRssi()) + "Snr: " + String(LoRa.packetSnr()));
     //   sendMessage(readString);
     //}
     
    //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    sendMessage(readString);                       // wait for a second
    //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
     
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

  //String repeatedMessage = repeatRequired(incoming);
  //if(repeatedMessage != incoming){
  //  delay(50);
  //  sendMessage(repeatedMessage);
  //  repeatedMessage = "LoRa Repeated: " + repeatedMessage + "←";
  //}
  Serial.println(incoming + "RSSI: " + String(LoRa.packetRssi()) + "Snr: " + String(LoRa.packetSnr()));
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(incoming);
  lcd.setCursor(0,1);
  lcd.print("RSSI" + String(LoRa.packetRssi()) + "Snr" + String(LoRa.packetSnr()));
  
}

/*
String repeatRequired(String outgoingMessage){
  String key = outgoingMessage.substring(5,7);
  String NewKey = "";
  for (int i = 0; i <= 5; i++) {
    String forwardKeys = forwarding[i];
    int keyIndex = forwardKeys.indexOf(key);
    if(keyIndex >= 0){
      i = 6;
      int splitIndex = forwardKeys.indexOf(',');
      if(keyIndex<=splitIndex){  
        NewKey = forwardKeys.substring(splitIndex+1);
      }else{
        NewKey = forwardKeys.substring(0, splitIndex);
      }
    }
  }
  
  Serial.println("KEY: " + key);
  if(NewKey != ""){    
    outgoingMessage.setCharAt(5, NewKey.charAt(0));
    outgoingMessage.setCharAt(6, NewKey.charAt(1));
  }
  return outgoingMessage;
}

void setForwardingKeys(String repeat){
    repeat.replace(" ", "");
    //Clear the Array
    for (int i = 0; i <= 5; i++) {
      forwarding[i] = "";
    }
    
    char char_array[0];
    // Copy it over 
    repeat.toCharArray(char_array, repeat.length() + 1);
    String repeatIds = "";
    int index = 0;
     for (int i = 0; i <= strlen(char_array); i++) {
      if(char_array[i] != '[' && char_array[i] != ']'){
        repeatIds += char_array[i];
      }else if(char_array[i] == ']' && repeatIds.length()>0){
        forwarding[index] = repeatIds;
        repeatIds = "";
        index += 1;
      }  
    }
    if(index == 0){
      Serial.println("404 Incorrect Parameters : " + repeat);
    }else{
      Serial.println("OK");
    }
}
*/
