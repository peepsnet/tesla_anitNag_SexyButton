#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>

#define    OUTPIN                 D10
#define    INPIN                  D0
#define    LEDPIN                 25
#define    POWER                  11
#define    RGBLED                 12
#define    NUMPIXELS              1

long delayLowMin;
long delayLowMax;
long btnPressForMin;
long btnPressForMax;
long x2XClickDelayMin;
long x2XClickDelayMax;
long doubleClickDly;

bool powerSwitch                = false;

unsigned long timeNow           = 0;
unsigned long delayBy           = 0;
unsigned long timeCapture       = 0;
bool buttonPressed              = false;
bool doubleClicked              = 0;
int clicksLeft                  = 0;

unsigned long buttonPressedAt   = 0;
long buttonPressTime            = 175;


int brightLED                   = 10;

Adafruit_NeoPixel powerLED(NUMPIXELS, RGBLED, NEO_GRB + NEO_KHZ800);
OneButton btn;

static void randomize() {
  delayLowMin                      = random(9500, 10000);
  delayLowMax                      = random(12000, 12500);
  delayBy                          = random(delayLowMin, delayLowMax);

  btnPressForMin               = random(175, 200);
  btnPressForMax               = random(235, 250);
  buttonPressTime              = random(btnPressForMin, btnPressForMax);
  x2XClickDelayMin             = random(90, 110);
  x2XClickDelayMax             = random(120, 140);
  doubleClickDly               = random(x2XClickDelayMin, x2XClickDelayMax);
}

// Handler function for a single click:
static void pressBtnNow() {
  if (powerSwitch) {
    randomize();
    digitalWrite(OUTPIN, HIGH);
    buttonPressed = true;
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
    randomize();
    digitalWrite(OUTPIN, LOW);
    buttonPressed = false;
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
  !powerSwitch ? unPressBtnNow() : void();
  randomize();
  changeLED();
  doubleClicked = true;
  clicksLeft = 2;
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
  randomize();

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

  if (millis() - timeNow > delayBy && !buttonPressed && !doubleClicked && powerSwitch) {
    pressBtnNow();
  }

  if (millis() - buttonPressedAt > buttonPressTime && buttonPressed && !doubleClicked && powerSwitch) {
    unPressBtnNow();
  }

  if (millis() - buttonPressedAt > buttonPressTime && !buttonPressed && doubleClicked && clicksLeft > 0 && powerSwitch) {
    pressBtnNow();
  }

  if (millis() - buttonPressedAt > doubleClickDly && buttonPressed && doubleClicked && powerSwitch) {
    clicksLeft--;
    unPressBtnNow();
    if (clicksLeft == 0) {
      doubleClicked = false;
    }
  }

}
