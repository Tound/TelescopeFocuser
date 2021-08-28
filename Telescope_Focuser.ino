/*
Telescope Focuser Code
Written by Thomas Pound
Built to control an Arduino controlled stepper motor that is used
to control the focus of a telescope

V1.2
Last updated: 28/8/21
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
char compassArray[3];

#define JOYSTICK_X 15
#define JOYSTICK_Y 14
#define JOYSTICK_SW 2 // Attach interrupt

int jsXVal = 0;   // Joystick X Axis
int jsYVal = 0;   // Joystick Y Axis
int jsSWVal = 0;  // Joystick Switch Value

int jsXMid = 512;
int jsYMid = 512;

boolean stepperEnabled = false;

int currentMicrostep = 0;  // Current microstep mode (e.g. 0,1,2,3,4)

byte microSteps[5] = {
  0b000, // Full step
  0b100, // Half step
  0b010, // Quarter step
  0b110, // Eighth step
  0b111  // Sixteeth step
};

/* Update the Adafruit OLED display */
void updateScreen(void){
  // Print to screen
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text

  // Print elevation
  display.setCursor(2, 2);
  display.print(F("Elevation: "));
  //display.print(calculateElevation());
  display.setCursor(5, 12);
  display.print(F("+90"));
  display.print(F(" degs"));

  // Print  microstepping val
  display.setCursor(70, 2);
  display.print(F("uStep: "));
  display.print(int(round(pow(2,currentMicrostep))));

  // Print stepper enable
  if(!stepperEnabled){
    display.setCursor(75, 15);
    display.print(F("STEPPER"));
    display.setCursor(73, 25);
    display.print(F("DISABLED"));
  }

  // Print direction
  display.setCursor(2, 25);
  display.print(F("Dir: "));
  display.print(compassArray);
  
  // Update the display
  display.display();
  delay(20);
}

void motorTurn(boolean direction){
  int interval = 2000;
  boolean cont = true;
  if(direction){
    digitalWrite(STEPPER_DIR, HIGH);
  }
  else{
    digitalWrite(STEPPER_DIR, LOW);
  }

  while(cont && stepperEnabled){ // While the JS is not center or the opposite direction
    jsYVal = analogRead(JOYSTICK_Y);
    interval = map(abs(jsYMid-jsYVal),0,jsYMid,2000,500); // Calculate interval between stepper steps
    Serial.print("Interval: "); Serial.println(interval);
    // Complete 1 step of motor
    digitalWrite(STEPPER_STEP, HIGH);
    delayMicroseconds(interval);
    digitalWrite(STEPPER_STEP, LOW);
    delayMicroseconds(interval);

    // Check if JS is back in center or changed direction
    if(!((direction && jsYVal > jsYMid) || (!direction && jsYVal < jsYMid))){
      cont = false;
    }
  }
}

char calculateElevation(void){//int x, int y, int z){
  char elevation[3] = {'+','9','0'};
  return elevation;
}

/* On the JS button press, enable/disable the motor */
void motorState(void){
  // Joystick Button
  // Enable stepper (Active Low)
  stepperEnabled = !stepperEnabled;
  digitalWrite(STEPPER_ENABLE, !stepperEnabled);
  delayMicroseconds(10000);
}

/* Set the microstep pin */
void setMicrostep(void){
  digitalWrite(MS1,bitRead(microSteps[currentMicrostep], 2));
  digitalWrite(MS2,bitRead(microSteps[currentMicrostep], 1));
  digitalWrite(MS3,bitRead(microSteps[currentMicrostep], 0));
}

/* Initialise the Adafruit OLED Screen */
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
  display.println(F("TELESCOPE\n  FOCUSER"));
  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
}

/*
 * MAIN CODE
 * INC. SETUP AND LOOP
 */

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

  attachInterrupt(digitalPinToInterrupt(JOYSTICK_SW), motorState, LOW);

  // Initialise output values
  digitalWrite(STEPPER_ENABLE,LOW);
  digitalWrite(MS1,LOW);
  digitalWrite(MS2,LOW);
  digitalWrite(MS3,LOW);

  Serial.println("Pins set");
  
  compass.init();
  compass.setCalibration(-1645, 1585, -2723, 841, -1545, 1363);

  Serial.println("Compass intialised");
  initialiseScreen();
  Serial.println("Screen initialised\n=============");
  //calibrateJS();
  jsXMid = analogRead(JOYSTICK_X);
  jsYMid = analogRead(JOYSTICK_Y);
}

void loop() {
  // Read joystick values
  jsXVal = analogRead(JOYSTICK_X);
  jsYVal = analogRead(JOYSTICK_Y);
  jsSWVal = digitalRead(JOYSTICK_SW);

  Serial.println(F("Joystick Values"));
  Serial.print(F("X: "));Serial.println(jsXVal);
  Serial.print(F("Y: "));Serial.println(jsYVal);
  Serial.print(F("SW: "));Serial.println(!jsSWVal);

  // Read compass
  compass.read();
  byte a = compass.getAzimuth();
  delay(20);

  compass.getDirection(compassArray, a);
  delay(20);

  // Compare joystick values

  // Joystick X axis
  // Change microstepping
  if(jsXVal > (1.5 * jsXMid)){
    if(currentMicrostep != sizeof(microSteps)-1){
      currentMicrostep++;
      stepsPerRevolution = stepsPerRevolution*2;
      // Increase microstep
      setMicrostep();
    }
  }

  else if(jsXVal < (0.5 * jsXMid)){
    if(currentMicrostep != 0){
      currentMicrostep--;
      stepsPerRevolution = stepsPerRevolution/2;
      // Decrease microstep
      setMicrostep();
    }
  }

  // Focus in or out
  if(jsYVal > (jsYMid + 10)){
    motorTurn(true);     // Turn Stepper CW
  }
  else if(jsYVal < (jsYMid - 10)){
    motorTurn(false);     // Turn Stepper CCW
  }
  else{
    digitalWrite(STEPPER_STEP, 0);
  }

  updateScreen();
  
  delay(100); // Debounce/allow changes to take place
}
