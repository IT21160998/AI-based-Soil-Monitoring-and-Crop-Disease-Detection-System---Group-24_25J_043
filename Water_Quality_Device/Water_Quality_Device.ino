#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define turbidity 39
#define potPin 36
#define DS18B20PIN 13
#define OLED_Address 0x3C
#define TdsSensorPin 34
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

OneWire oneWire(DS18B20PIN);
DallasTemperature sensor(&oneWire);

Adafruit_SSD1306 display(128, 64);

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation

int valur_t, Value, temp;
float ph;

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}
void setup() {
  Serial.begin(9600);
  sensor.begin();
  pinMode(turbidity, INPUT);
  pinMode(potPin, INPUT);
  pinMode(TdsSensorPin, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  display.clearDisplay();

}

void loop() {
  Display();
  turbidityData();
  phRead();
  rd18b20();
  tdsRead();

}
void Display() {
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 0);
  display.println("Water Analizer");
  display.setCursor(0, 17);
  display.print("Turbidity : ");
  display.print(valur_t);
  display.println(" %");
  display.print("water Temp : ");
  display.print(temp);
  display.println(" *C");
  display.print("pH : ");
  display.println(ph);
  display.print("TDS : ");
  display.print(tdsValue);
  display.println(" ppm");
  display.display();
}
void turbidityData() {
  valur_t = analogRead(turbidity);
  Serial.print("Turbidity= ");
  valur_t = map(valur_t, 0, 4095, 0, 100);
  Serial.print(valur_t);
  Serial.println(" %");
}
void phRead() {
  Value = analogRead(potPin);
  Serial.print(Value);
  Serial.print(" | ");
  float voltage = Value * (3.3 / 4095.0);
  voltage = voltage - 0.12;
  ph = (3.3 * voltage);
  Serial.println(ph);
}
void rd18b20() {
  sensor.requestTemperatures();
  Serial.print("Temperature is: ");
  Serial.println(sensor.getTempCByIndex(0));
  temp = sensor.getTempCByIndex(0);
}
void tdsRead() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) { //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;

      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
      //temperature compensation
      float compensationVoltage = averageVoltage / compensationCoefficient;

      //convert voltage value to tds value
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue, 0);
      Serial.println("ppm");
    }
  }
}
