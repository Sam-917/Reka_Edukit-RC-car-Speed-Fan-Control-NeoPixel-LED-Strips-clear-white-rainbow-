/**
Esp32 Dev Kit 

Functinalities : 
- Using Wifi for iOT purposes to update TDS (Total Disolved Water) values and temperature values 
- Using 16x2 LCD screen to display data 

Data display on BLYNK : 
- TDS (ppm)
- Temperature (°C)
- Water quality (%)
- EC 

Additional feature : 
- Web cam camera display using Esp32 CAM / AI Thinker Esp32 CAM
**/ 


#define BLYNK_TEMPLATE_ID "TMPL6t16JKD6g"
#define BLYNK_TEMPLATE_NAME "TDS"
#define BLYNK_AUTH_TOKEN "lkKu5evDs5B1s8gLgzXXD64KtuUETVkD"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "rahmanex5-2.4GHz";
char pass[] = "rahman1234";
char auth[] = BLYNK_AUTH_TOKEN;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

namespace pin {
  const byte tds_sensor = 32;  // Analog pin for TDS sensor ADC GPIO 32 
  const byte one_wire_bus = 4; // Dallas Temperature Sensor
  const byte servo1 = 14;      // Servo 1 control pin
  const byte servo2 = 27;      // Servo 2 control pin
}

namespace device {
  float aref = 5; // Vref, this is for 3.3v compatible controller boards, for arduino use 5.0v.
}

namespace sensor {
  float ec = 0;
  unsigned int tds = 0;
  float waterTemp = 0;
  float ecCalibration = 1;
}

OneWire oneWire(pin::one_wire_bus);
DallasTemperature dallasTemperature(&oneWire);
BlynkTimer timer;

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200); // Debugging on hardware Serial 0
  Blynk.begin(auth, ssid, pass);
  dallasTemperature.begin();
  lcd.init();
  lcd.backlight();

  // Set timer to call readTdsQuick every second (1000 ms)
  timer.setInterval(1000L, readTdsQuick);

  // Attach servos to their respective pins
  servo1.attach(pin::servo1);
  servo2.attach(pin::servo2);

  servo1.write(0);
  servo2.write(0);
}

// Blynk functions to control the servos
BLYNK_WRITE(V4) {
  int value = param.asInt();
  if (value == 1) {
    servo1.write(90);  // Turn Servo 1 to 90 degrees
  } else {
    servo1.write(0);   // Turn Servo 1 back to 0 degrees
  }
}

BLYNK_WRITE(V5) {
  int value = param.asInt();
  if (value == 1) {
    servo2.write(90);  // Turn Servo 2 to 90 degrees
  } else {
    servo2.write(0);   // Turn Servo 2 back to 0 degrees
  }
}

float calculateCleanlinessPercentage(float tdsValue) {
  float percentage;

  if (tdsValue <= 300) {
    percentage = 100;  // Very clean
  } else if (tdsValue >= 1200) {
    percentage = 0;  // Very poor
  } else {
    // Linearly interpolate between 100% (300 ppm) and 0% (1200 ppm)
    percentage = 100 - ((tdsValue - 300) / (1200 - 300)) * 100;
  }

  return percentage;
}

void readTdsQuick() {
  dallasTemperature.requestTemperatures();
  sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
  float rawEc = analogRead(pin::tds_sensor) * device::aref / 4096.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float temperatureCoefficient = 1.0 + 0.02 * (sensor::waterTemp - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  sensor::ec = (rawEc / temperatureCoefficient) * sensor::ecCalibration; // temperature and calibration compensation
  sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5; //convert voltage value to tds value

  Blynk.virtualWrite(V0, sensor::tds);
  Blynk.virtualWrite(V1, sensor::waterTemp);
  Blynk.virtualWrite(V4, calculateCleanlinessPercentage(sensor::tds));
  
  lcd.setCursor(0, 0);
  lcd.print("TDS: ");
  lcd.print(sensor::tds);
  lcd.print(" ppm");
  
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(sensor::waterTemp);
  lcd.print(" C");
}

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer
}
