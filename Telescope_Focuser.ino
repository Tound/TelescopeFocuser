/*
Telescope Focuser Code
Written by Thomas Pound
Built to control an Arduino controlled stepper motor that is used
to control the focus of a telescope

V1.1
Last updated: 25/8/21
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <QMC5883LCompass.h>

#define COMPASS_ADDRESS 0xD

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

#define STEPPER_DIR 6
#define STEPPER_STEP 5
#define MS1 9
#define MS2 10
#define MS3 11
#define STEPPER_ENABLE 12

int stepsPerRevolution = 200;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QMC5883LCompass compass;

#define JOYSTICK_X 15
#define JOYSTICK_Y 14
#define JOYSTICK_SW 2

int jsXVal = 0;
int jsYVal = 0;
int jsSWVal = 0;

boolean stepperEnabled = false;

byte currentMicrostep = 0;

byte microSteps[5] = {
  b000, // Full step
  b100, // Half step
  b010, // Quarter step
  b110, // Eighth step
  b111  // Sixteeth step
};

int calculateElevation(int x, int y, int z){

  return elevation;
}

void updateScreen(void){
  // Print to screen
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text

  // Print elevation
  display.setCursor(10, 0);
  display.print(F("Elevation: "));
  display.print(F(" degs"));

  // Print  microstepping val
  display.setCursor(50, 0);
  display.print(F("uStep: "));
  display.print(F("1/");
  display.print(F(pow(2,currentMicrostep)));

  // Print stepper enable
  if(!stepperEnabled){
    display.setCursor(50, 20);
    display.print(F("STEPPER"));
    display.setCursor(50, 50);
    display.print(F("DISABLED"));
  }

  // Print direction
  display.print(F("Direction: "));
  display.print(F(myArray[0]));
  display.print(F(myArray[1]));
  display.print(F(myArray[2]));
  
  // Update the display
  display.display();
  delay(20);
}

void motorTurn(boolean direction){
  int interval = 2000;
  boolean continue = true;
  if(direction){
    digitalWrite(STEPPER_DIR, HIGH);
  }
  else{
    digitalWrite(STEPPER_DIR, LOW);
  }

  while(continue){ // While the JS is not center or the opposite direction
    jsYVal = analogRead(JOYSTICK_Y);
    interval = map(abs(533-jsYVal),0,533,0,5000);
    delayMicroseconds(interval);
    if(!((direction && jsYVal > 536) || (!direction && jsYVal < 530))){
      continue = false;
    }
  }
}

void initialiseScreen(void){
  // Start up screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Start Screen
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,0);
  display.println(F("TELESCOPE"));
  display.setCursor(10,16);
  display.println(F("FOCUSER"));
  display.display();
  delay(2000);
}

void setup() {
  Serial.begin(9600);
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);

  pinMode(STEPPER_ENABLE, OUTPUT);
  pinMode(STEPPER_STEP,OUTPUT);
  pinMode(STEPPER_DIR,OUTPUT);
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  pinMode(MS3,OUTPUT);

  // Initialise output values
  digitalWrite(STEPPER_ENABLE,LOW);
  digitalWrite(MS1,LOW);
  digitalWrite(MS2,LOW);
  digitalWrite(MS3,LOW);
  
  compass.init();
  compass.setCalibration(-1645, 1585, -2723, 841, -1545, 1363);
  
  initialiseScreen();
}

void loop() {
  Serial.println(F(""));
  jsXVal = analogRead(JOYSTICK_X);
  jsYVal = analogRead(JOYSTICK_Y);
  jsSWVal = digitalRead(JOYSTICK_SW);
  Serial.println(F(jsXVal));
  Serial.println(F(jsYVal));
  Serial.println(F(!jsSWVal));

  //Read compass
  compass.read();
  byte a = compass.getAzimuth();
  delay(20);

  char compassArray[3];
  compass.getDirection(compassArray, a);
  delay(20);

  // Read joystick
  // Joystick Button
  if(jsSWVal == HIGH){
    // Enable stepper (Active Low)
    stepperEnabled = !stepperEnabled;
    digitalWrite(STEPPER_ENABLE, stepperEnabled);
  }

  // Joystick X axis
  // Change microstepping
  if(jsXVal > 900){
    if(currentMicrostep != sizeof(microSteps)-1){
      currentMicrostep++;
      stepsPerRevolution = stepsPerRevolution*2;
      // Increase microstep
      digitalWrite(MS1,bitRead(microSteps[currentMicrostep], 2));
      digitalWrite(MS2,bitRead(microSteps[currentMicrostep], 1));
      digitalWrite(MS3,bitRead(microSteps[currentMicrostep], 0));
    }
  }

  else if(jsXVal < 100){
    if(currentMicrostep != 0){
      currentMicrostep--;
      stepsPerRevolution = stepsPerRevolution/2;
      // Decrease microstep
      digitalWrite(MS1,bitRead(microSteps[currentMicrostep], 2));
      digitalWrite(MS2,bitRead(microSteps[currentMicrostep], 1));
      digitalWrite(MS3,bitRead(microSteps[currentMicrostep], 0));
    }
  }

  // Focus in or out
  if(jsYVal > 536){
    // Turn Stepper CW
    motorTurn(true);
  }
  else if(jsYVal < 530){
    // Turn Stepper CCW
    motorTurn(false);
  }
  else{
    digitalWrite(STEPPER_STEP, 0);
  }

  updateScreen();
  
  delay(300); // Debounce/allow changes to take place
}
