#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

// Analog 
#define IR_SENSOR1 A0
#define IR_SENSOR2 A1
#define ON_ESP     A2

// Digital 
#define NUM_PIXELS 60
#define NEO_PIXEL_LED 2
#define BUZZER 3
#define M2_DIR 4
#define M2_PMW 5
#define BT_RX 6
#define BT_TX 7
#define ENA 8
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12
#define ENB 13

// Initialize object variable 
SoftwareSerial BTserial(BT_RX, BT_TX); //  RX | TX 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIXEL_LED, NEO_GRB + NEO_KHZ800);

// Timing variables
unsigned long previousMotorMillis = 0;
unsigned long previousNeoPixelMillis = 0;
unsigned long previousRainbowMillis = 0;

// Intervals variables 
const long motorInterval = 20;      // Motor control interval in milliseconds
const long neoPixelInterval = 20;   // NeoPixel update interval in milliseconds
const long rainbowInterval = 20;    // Rainbow cycle interval in milliseconds

// Additional variables
int command;
int Speed = 255;
int Speedsec;
int buttonState = 0;
int lastButtonState = 0;
int Turnradius = 0;
int brakeTime = 45;
int brkonoff = 1;

int M2Speed = 0;
int counter = 0;
bool neoPixelActive = false;
bool rainbowActive = false;

void left() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  analogWrite(ENA, Speed); 
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void back() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void forwardleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speedsec);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forwardright() {
  analogWrite(ENA, Speedsec);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backright() {
  analogWrite(ENA, Speedsec);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speedsec);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void Stop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void brakeOn() {
  buttonState = command;
  if (buttonState != lastButtonState) {
    if (buttonState == 'S') {
      if (lastButtonState != buttonState) {
        analogWrite(ENA, 255);
        analogWrite(ENB, 255);
        delay(brakeTime);
        Stop();
      }
    }
    lastButtonState = buttonState;
  }
}

void brakeOff() {
  // Placeholder for brake off functionality
}

// Rainbow animation LED strip
void rainbowCycle() {
  static uint16_t j = 0;
  if (millis() - previousRainbowMillis >= rainbowInterval) {
    previousRainbowMillis = millis();
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i * 256 / pixels.numPixels() + j) & 255));
    }
    pixels.show();
    j++;
    if (j >= 256) j = 0;
  }
}

// change colours
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

// Function to set all NeoPixel LEDs to white
void setWhite() {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255)); // Set to white
  }
  pixels.show();
}

// Function to clear the LED strip
void clearStrip() {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); 
  }
  pixels.show();
}

// Function to check IR sensors and sound the buzzer
void checkIRSensors() {
  bool sensor1Triggered = analogRead(IR_SENSOR1) < 500; 
  bool sensor2Triggered = analogRead(IR_SENSOR2) < 500; 

  if (sensor1Triggered && sensor2Triggered) {
    tone(BUZZER, 500, 100);
  } else {
    noTone(BUZZER);
  }
}

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  pixels.begin();

  pinMode(M2_PMW, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(IR_SENSOR1, INPUT);
  pinMode(IR_SENSOR2, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ON_ESP, OUTPUT);  
}

void loop() {
  unsigned long currentMillis = millis(); 
  
  if (BTserial.available() > 0) {
    command = BTserial.read();  
    digitalWrite(M2_DIR, LOW);
    analogWrite(M2_PMW, M2Speed);

    switch (command) {
      case 'F':
        left();
        break;
      case 'B':
        right();
        break;
      case 'L':
        back();
        break;
      case 'R':
        forward();
        break;
      case 'G':
        backright();
        break;
      case 'I':
        backleft();
        break;
      case 'H':
        forwardright();
        break;
      case 'J':
        forwardleft();
        break;
      case '0':
        M2Speed = 0;
        break;
      case '1':
        M2Speed = 20;
        break;
      case '2':
        M2Speed = 45;
        break;
      case '3':
        M2Speed = 60;
        break;
      case '4':
        M2Speed = 80;
        break;
      case '5':
        M2Speed = 100;
        break;
      case '6':
        M2Speed = 120;
        break;
      case '7':
        M2Speed = 150;
        break;
      case '8':
        M2Speed = 200;
        break;
      case '9':
        M2Speed = 220;
        break;
      case 'q':
        M2Speed = 245;
        break;
      case 'W':
        neoPixelActive = true;
        setWhite();
        break;
      case 'w':
        neoPixelActive = false;
        clearStrip();
        break;
      case 'U':
        rainbowActive = true; 
        break;
      case 'u':
        rainbowActive = false; 
        clearStrip();
        break;
      case 'V':
        tone(BUZZER, 500, 100);
        break;
      case 'v':
        noTone(BUZZER);
        break; 
      case 'X':
        digitalWrite(ON_ESP, HIGH);
        break;  
      case 'x':
        digitalWrite(ON_ESP, LOW);
        break;
      case 'D':
        // Stop all actions
        Speed = 0;
        Stop();
        noTone(BUZZER);
        neoPixelActive = false;
        rainbowActive = false;
        clearStrip();
        break;
    }
  }

  if (rainbowActive) {
    rainbowCycle();
  }

  checkIRSensors(); // Check IR sensors in each loop iteration
}