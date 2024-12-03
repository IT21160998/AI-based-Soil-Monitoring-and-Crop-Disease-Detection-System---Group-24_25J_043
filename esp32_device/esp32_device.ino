#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>

#define OLED_Address 0x3C
Adafruit_SSD1306 display(128, 64);


#define RXD2 19
#define TXD2 18

String incoming, dumy;
String temp = "0";
String water_temp = "0";
String humy = "0";
String soil = "0";
String N = "0";
String P = "0";
String K = "0";
int a1;
String  Latitude = "6.42";
String  Longitude = "80.82";

HardwareSerial Sender(1);
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  Sender.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  display.clearDisplay();

}

void loop() {


  while (Sender.available() > 0)
    if (gps.encode(Sender.read()))
      displayInfo();

  if (Serial2.available() > 0) {
    incoming = Serial2.readString();
    Serial.print("Data : ");
    Serial.println(incoming);

    a1 = incoming.indexOf(';');

    if (a1 == 2) {
      a1 = incoming.indexOf(';');
      dumy = incoming.substring(0, a1);
      Serial.print("dumy Data: ");
      Serial.println(dumy);

      int a2 = incoming.indexOf(';', a1 + 1);
      water_temp = incoming.substring(a1 + 1, a2);
      Serial.print("Water Temperature: ");
      Serial.println(water_temp);

      int a3 = incoming.indexOf(';', a2 + 1);
      humy = incoming.substring(a2 + 1, a3);
      Serial.print("Humidity: ");
      Serial.println(humy);

      int a4 = incoming.indexOf(';', a3 + 1);
      temp = incoming.substring(a3 + 1, a4);
      Serial.print("Temperature: ");
      Serial.println(temp);

      int a5 = incoming.indexOf(';', a4 + 1);
      soil = incoming.substring(a4 + 1, a5);
      Serial.print("soil : ");
      Serial.println(soil);

      int a6 = incoming.indexOf(';', a5 + 1);
      N = incoming.substring(a5 + 1, a6);
      Serial.print("Nitrogen: ");
      Serial.println(N);

      int a7 = incoming.indexOf(';', a6 + 1);
      P = incoming.substring(a6 + 1, a7);
      Serial.print("Phosphorous: ");
      Serial.println(P);

      int a8 = incoming.indexOf(';', a7 + 1);
      K = incoming.substring(a7 + 1, a8);
      Serial.print("Potassium: ");
      Serial.println(K);
      Display();
    }
  }
}
void displayInfo() {
  if (gps.location.isValid()) {
    Latitude = gps.location.lat();
    Longitude = gps.location.lng();

    Serial.print(F("Latitude: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(F("Longitude: "));
    Serial.println(gps.location.lng(), 6);

  } else {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
void Display() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 0);
  display.println("NPK Analizer");
  display.setCursor(0, 20);
  display.print("Temperature : ");
  display.println(temp);
  display.print("Soil Moisture : ");
  display.print(soil);
  display.println(" %");
  display.print("Nitrogen : ");
  display.println(N);
  display.print("Phosphorous : ");
  display.println(P);
  display.print("Potassium : ");
  display.println(K);
  display.display();
}
