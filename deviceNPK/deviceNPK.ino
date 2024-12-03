#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

// RO to pin 8 & DI to pin 9 when using AltSoftSerial
#define RE 6
#define DE 7

const byte nitro[] =          {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] =           {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] =           {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
byte val1, val2, val3;

// Define sensor pins
#define DHTPIN 12      // Pin for DHT11
#define DHTTYPE DHT11  // DHT 11 sensor type
#define SOIL_PIN A0    // Analog pin for soil moisture sensor
#define ONE_WIRE_BUS 11 // Pin for DS18B20 sensor

int temperatureDS18B20, humidity, temperatureDHT, soilMoisture;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

// Initialize sensors
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
AltSoftSerial mod;
SoftwareSerial mySerial(2, 3);

void setup() {
  Serial.begin(9600);
  dht.begin();
  sensors.begin();
  mod.begin(4800);
  mySerial.begin(9600);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // put RS-485 into receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
}

void loop() {

  Serial.print("   Nitrogen: ");
  val1 = nitrogen();
  Serial.print(" = ");
  Serial.print(val1);
  Serial.println(" mg/kg");
  delay(250);

  Serial.print("Phosphorous: ");
  val2 = phosphorous();
  Serial.print(" = ");
  Serial.print(val2);
  Serial.println(" mg/kg");
  delay(250);

  Serial.print("  Potassium: ");
  val3 = potassium();
  Serial.print(" = ");
  Serial.print(val3);
  Serial.println(" mg/kg");
  Serial.println();

  temp18b20Read();
  dhtRead();
  soilRead();


  if ((millis() - lastTime) > timerDelay) {
    sendData();
    lastTime = millis();
  }


}

void temp18b20Read() {
  // Reading temperature from DS18B20
  sensors.requestTemperatures();
  temperatureDS18B20 = sensors.getTempCByIndex(0);

  // Print DS18B20 temperature
  Serial.print("Temperature (DS18B20): ");
  Serial.print(temperatureDS18B20);
  Serial.println(" °C");
}
void dhtRead() {
  // Reading data from DHT11
  humidity = dht.readHumidity();
  temperatureDHT = dht.readTemperature();

  // Print DHT11 values
  Serial.print("Humidity (DHT11): ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Temperature (DHT11): ");
  Serial.print(temperatureDHT);
  Serial.println(" °C");
}
void soilRead() {
  // Reading soil moisture sensor and mapping to 0-100
  soilMoisture = analogRead(SOIL_PIN);
  soilMoisture = map(soilMoisture, 0, 1023, 100, 0);

  // Print soil moisture level
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.println(" %");
}
byte nitrogen() {
  // clear the receive buffer
  mod.flushInput();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  // write out the message
  for (uint8_t i = 0; i < sizeof(nitro); i++ ) mod.write( nitro[i] );

  // wait for the transmission to complete
  mod.flush();

  // switch RS-485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // crude delay to allow response bytes to be received!
  delay(100);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}

byte phosphorous() {
  mod.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  for (uint8_t i = 0; i < sizeof(phos); i++ ) mod.write( phos[i] );
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(100);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}

byte potassium() {
  mod.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  for (uint8_t i = 0; i < sizeof(pota); i++ ) mod.write( pota[i] );
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(100);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}
void sendData() {
  String sendDataESP32 = "22;" + (String)temperatureDS18B20 + ";" + humidity + ";" + temperatureDHT + ";" + soilMoisture + ";" + val1 + ";" + val2 + ";" + val3 + "";
  mySerial.print(sendDataESP32);
  Serial.print("ESP DATA :  ");
  Serial.println(sendDataESP32);
}
