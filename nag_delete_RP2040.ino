#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>

#define    OUTPIN                 D10
#define    INPIN                  D0
#define    LEDPIN                 25
#define    POWER                  11
#define    RGBLED                 12
#define    NUMPIXELS              1

const long LOW_MIN              = random(9500, 10000);
const long LOW_MAX              = random(12000, 12500);
const long btnPressForMin       = random(200, 215);
const long btnPressForMax       = random(250, 265);

bool powerSwitch                = false;

unsigned long timeNow           = 0;
unsigned long delayBy           = 0;
unsigned long timeCapture       = 0;
bool buttonPressed              = false;
unsigned long buttonPressedAt   = 0;
long buttonPressTime            = 250;

int brightLED                   = 10;

Adafruit_NeoPixel powerLED(NUMPIXELS, RGBLED, NEO_GRB + NEO_KHZ800);
OneButton btn;


// Handler function for a single click:
static void pressBtnNow() {
  if (powerSwitch) {
    digitalWrite(OUTPIN, HIGH);
    buttonPressed = true;
    
    buttonPressTime = random(btnPressForMin, btnPressForMax);
    buttonPressedAt = millis();
    powerLED.setPixelColor(0, powerLED.Color(0, 0, brightLED));
    powerLED.show();
    Serial.println("Button Pressed!!");
  } else {
    digitalWrite(OUTPIN, LOW);
    Serial.println("Power is OFF. Do nothing!!!!");
  }
}

static void unPressBtnNow() {
  if (powerSwitch) {
    digitalWrite(OUTPIN, LOW);
    buttonPressed = false;
    delayBy = random(LOW_MIN, LOW_MAX);
    timeNow = millis();
    changeLED();
    Serial.println("Button Released!!");
  } else {
    digitalWrite(OUTPIN, LOW);
    Serial.println("Power is OFF. Do nothing!!!!");
  }
}

static void changeLED() {
  powerLED.clear();
  if (powerSwitch) {
    powerLED.setPixelColor(0, powerLED.Color(0, brightLED, 0));
  } else {
    powerLED.setPixelColor(0, powerLED.Color(brightLED, 0, 0));
  }
  powerLED.show();
  Serial.println("LED CHANGED!!");
}

static void changeBrightness() {
  brightLED  = brightLED + 60;
  if ( brightLED > 240 ) { brightLED = 10; } 
  Serial.println("Clicked Twice! LED Brightness changed!!");
  changeLED();
}

static void toggleOnOff() {
  powerSwitch = !powerSwitch;
  changeLED();
  Serial.println("LONG Clicked! Power Toggled!!");
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Setup Begins");

  powerLED.begin();

  pinMode(OUTPIN, OUTPUT);
  pinMode(INPIN, INPUT);    
  pinMode(LEDPIN, OUTPUT);
  pinMode(POWER, OUTPUT);
  
  digitalWrite(LEDPIN, LOW);
  digitalWrite(OUTPIN, LOW);
  digitalWrite(POWER, HIGH);

  
  timeNow = millis();
  delayBy = random(LOW_MIN, LOW_MAX);

  btn.setup(INPIN, INPUT_PULLUP, false);
  btn.attachClick(pressBtnNow);
  btn.attachDoubleClick(changeBrightness);
  btn.attachLongPressStart(toggleOnOff);
  btn.reset();

  changeLED();

  Serial.println("Setup Complete");

}

void loop() {

  btn.tick();

  if (millis() - timeNow > delayBy && !buttonPressed && powerSwitch) {
    pressBtnNow();
  }

  if (millis() - buttonPressedAt > buttonPressTime && buttonPressed & powerSwitch) {
    unPressBtnNow();
  }

}
