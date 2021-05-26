#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h> 
#include <SoftwareSerial.h> 
#include <stdlib.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

//definisi pin LoRa
#define ss 6
#define rst 5
#define dio0 2
#define batpin A0
#define charger 7

String batstatus, batt;
float VBAT = 0 ;
SoftwareSerial gpsSerial(8,9);//rx,tx 
TinyGPSPlus tinyGPS;
SSD1306AsciiAvrI2c display;

void setup() {
  Serial.begin(115200);
  pinMode(batpin, INPUT);
  pinMode(charger, INPUT);
  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(System5x7);
  display.clear();
  display.set2X();
  display.setFont(Callibri11_bold);
  display.setCursor(10, 0);
  display.println("MPU LORA");
  display.setCursor(10, 30);
  display.println(" NODE 2");
  delay(2000);
  
  analogReference(INTERNAL);
  while (!Serial);
  Serial.println("LoRa Sender");

  //Atur LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  gpsSerial.begin(9600);
  Serial.println("GPS Initialized");
  Serial.println("Waiting for GPS.. .");
  LoRa.beginPacket();
  Serial.println(gpsSerial.read());
  LoRa.print("Waiting for GPS...");
  LoRa.endPacket();
  Serial.println("inisialisasi selesai");
  Serial.println("___________________________");
}
void loop() {
    printGPSInfo();
    baterai();
    dataOled();
    smartDelay(1000); 
}
void dataOled(){
    display.setFont(System5x7);
    display.set1X();
    display.setFont(Callibri11_bold);
    display.clear();
    display.setCursor(0, 20);
    display.print("Latitude :");
    display.setCursor(50, 20);
    display.println(tinyGPS.location.lat(),6);

    display.setCursor(0, 30);
    display.print("Longitud:");
    display.setCursor(50, 30);
    display.println(tinyGPS.location.lng(),6);

    display.setCursor(0, 40);
    display.print("Altitude :");
    display.setCursor(50, 40);
    display.println(tinyGPS.altitude.meters());

    display.setCursor(0, 50);
    display.println(batstatus);

    if (digitalRead(charger) == HIGH){
      display.setCursor(90, 50);
      display.println("Charger"); 
    }
}
void printGPSInfo() {
    LoRa.beginPacket();
    //kirim text *1,lat,long,altitude,baterai#\n
    LoRa.print("*1,");
    LoRa.print(tinyGPS.location.lat(),6);
    LoRa.print(",");
    LoRa.print(tinyGPS.location.lng(),6);
    LoRa.print(",");
    LoRa.print(tinyGPS.altitude.meters());
    LoRa.print(",");
    LoRa.print(batt);
    LoRa.println("#"); 
    LoRa.endPacket();
    
    Serial.print("Lat: "); Serial.println(tinyGPS.location.lat(),6);
    Serial.print("Long : "); Serial.println(tinyGPS.location.lng(),6);
    Serial.print("Altitude: "); Serial.println(tinyGPS.altitude.meters()); 
    Serial.println("___________________________");
}
void baterai(){
  //baterai
  float r1=26820.0;
  float r2=100000.0;
  //VBAT = ((((3.3f*analogRead(batpin))/1024)/ r2*(r1+r2)) );
  VBAT = ((3.3f*analogRead(batpin))/1024);
  batt = String(VBAT, 2);
  batstatus = "Battery  : " + String(VBAT, 2) + " V";
  Serial.println(batstatus);
}
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      tinyGPS.encode(gpsSerial.read());
  } while (millis() - start < ms);
}
