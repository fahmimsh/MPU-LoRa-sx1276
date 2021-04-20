#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define csPin 5
#define resetPin 14
#define irqPin 2

const char* ssid="fahmi";
const char* password = "bersiapsiapya";

String dataIn = "", dt[10];
boolean parsing=false, terima = false;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiClient client_;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.println("MPU LORA");
  display.setCursor(10, 30);
  display.println("GATEWAY");
  display.display();
  delay(2000);
  Serial.println("LoRa Gateway");
  LoRa.setPins(csPin, resetPin, irqPin);
  Serial.print("Inisialisasi LoRa .");
  if (!LoRa.begin(915E6)) {
    Serial.println(" [FAILED!]");
    while (true); 
  }
  Serial.println(" [OK!]");
  connectWifi();
  displayoled();
  LoRa.onReceive(onReceive);
  LoRa.disableInvertIQ();   //Mode Recheiver
  LoRa.receive();
  //LoRa Setting Komunikasi
//  LoRa.setTxPower(20);
//  LoRa.setSpreadingFactor(10);
//  LoRa.setSignalBandwidth(125E3);
//  //LoRa.setCodingRate4(8);
//  LoRa.setSyncWord(0xE3); //0xE3
//  //LoRa.enableCrc();   
}

void loop() {
  if(runSetiap(5000)){
    //LoRa.onReceive(onReceive);
    //Serial.println("upload data ke firebase");
  }
  if (terima == true){
    parsingdata();
  }
}
void parsingdata(){//*data1,data2,.....,data100#\n
    parsing = true;
    if(parsing){
      int j=0, i;
      dt[j]="";
      for(i=1;i<dataIn.length();i++){
       if ((dataIn[i] == '#') || (dataIn[i] == ',')){
        j++;
        dt[j]="";
      } else {
        dt[j] = dt[j] + dataIn[i];
      }
    }
    Serial.println("Node: " + dt[0]);
    Serial.println("Latitude: " + dt[1]);
    Serial.println("Longlitude : " + dt[2]);
    Serial.println("Altitude: " + dt[3]);
    Serial.println("Baterai: " + dt[4]);
    Serial.println("___________________________");
    displayoled();
    parsing=false;
    dataIn="";
    }
    terima = false;
}
void onReceive(int packetSize) {
  String Node = "";
  while (LoRa.available()) {
    Node = LoRa.readString();
    Serial.println("data masuk : " + Node);
    //Parsing Data
    terima = true; 
    dataIn += Node;
  }
}
void displayoled(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 0);
    display.println("MPU LORA");
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.print("Latitude :");
    display.setCursor(60, 20);
    display.println(dt[1]);

    display.setCursor(10, 30);
    display.print("Longitud:");
    display.setCursor(60, 30);
    display.println(dt[2]);

    display.setCursor(10, 40);
    display.print("Altitude :");
    display.setCursor(60, 40);
    display.println(dt[3]);

    display.setCursor(10, 50);
    display.print("Battery :");
    display.setCursor(80, 50);
    display.println(dt[4] + " V");
    display.display();
}
void connectWifi() {
    Serial.print("Inisialisasi WiFi");  
    WiFi.begin(ssid,password);
    Serial.print("Connecting. ");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("[OK!]");
    delay(500);
    Serial.println(WiFi.localIP());  
}
boolean runSetiap(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
