#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);
int GPSBaud = 9600;
int RXPin = 5;
int TXPin = 4;
// Create a TinyGPS++ object
TinyGPSPlus gps;
// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

int counter = 0;   

void setup() {
  Serial.begin(9600);
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  pinMode(7, INPUT);
  while (!Serial);
  Serial.println("LoRa Node");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(20);
}

void loop() {

   if (digitalRead(7) == HIGH) {
    Serial.println("Button is pressed");
    Serial.print("Sending packet: ");
    Serial.println(counter);
    // send packet
    LoRa.beginPacket();
    LoRa.print(gps.lat);
    LoRa.print(gps.lng)
    LoRa.print(counter);
    LoRa.endPacket();
    counter++;
    delay(5000);
    display.clearDisplay();
    display.display();
   }

  else{

    int packetSize = LoRa.parsePacket();
  
    if (packetSize) {
      // received a packet
      Serial.print("Received packet '");
      display.begin(SH1106_SWITCHCAPVCC, 0x3C);  
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println(gps.location.lat(), 6);
      display.println(gps.location.lng(), 6);
      display.display();
      delay(2000);
      // read packet
      while (LoRa.available()) {
        Serial.print((char)LoRa.read());
      }
      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());
      display.clearDisplay();
      display.display();

      LoRa.beginPacket();
      LoRa.print("hello ");
      LoRa.endPacket();
    }
    
    }
    
}
