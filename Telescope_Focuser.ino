// Telescope Focusor Code
// Thomas Pound

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <QMC5883LCompass.h>

#define OLED_ADDRESS 0x3C
#define COMPASS_ADDRESS 0xD

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

#define STEPPER_DIR 6
#define STEPPER_STEP 5
const int steps_per_revolution = 200;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QMC5883LCompass compass;

#define JOYSTICK_X 15
#define JOYSTICK_Y 14
#define JOYSTICK_SW 2
#define STEPPER_DIR 5
#define STEPPER_PWM 6

int jsXVal = 0;
int jsYVal = 0;
int jsSWVal = false;
int pwm_val = 0;

int screen = 0;   // Screen
// Screen 1 - Overview
// Screen 2 - Compass vals?
// Screen 3 - Time?

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

  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,0);
  display.println("TELESCOPE");
  display.setCursor(10,16);
  display.println("FOCUSOR");
  display.display();
  delay(2000);
}

void setup() {
  Serial.begin(9600);
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);
  
  compass.init();
  compass.setCalibration(-1645, 1585, -2723, 841, -1545, 1363);
  
  initialiseScreen();
}

void loop() {
  Serial.println("");
  jsXVal = analogRead(JOYSTICK_X);
  jsYVal = analogRead(JOYSTICK_Y);
  jsSWVal = digitalRead(JOYSTICK_SW);
  Serial.println(jsXVal);
  Serial.println(jsYVal);
  Serial.println(!jsSWVal);

  //Read compass
  compass.read();
  byte a = compass.getAzimuth();
  delay(20);

  //Read joystick
  // Joystick Button
  if(jsSWVal == HIGH){
    // Switch mode
  }

  // Joystick X axis
  if(jsXVal > 504){
  
  }
  else if(jsXVal < 504){
  
  }

  // Focus in or out
  if(jsYVal > 533){
    digitalWrite(STEPPER_DIR, HIGH);
    analogWrite(STEPPER_PWM, pwm_val);
    // Turn Stepper CW
  }
  else if(jsYVal < 533){
    // Turn Stepper CCW
  }

  else{
    pwm_val = 0;
    analogWrite(STEPPER_PWM, pwm_val);
  }

  
  // Middlebutton of JS
  if(jsSWVal == 1){
    if(screen == 2){
      screen = 0;
    }else{
      screen++;
    }
  }

  // Print to screen
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(10, 0);
  display.print("X Val: ");
  display.println(jsXVal);

  display.setCursor(10, 10);
  display.print("Y Val: ");
  display.println(jsYVal);

  display.setCursor(10,20);
  char compassString = {};

  char myArray[3];
  compass.getDirection(myArray, a);
    
  display.print("Elevation: ");
  display.print(myArray[0]);
  display.print(myArray[1]);
  display.print(myArray[2]);
  
  display.display();
  
  delay(200);
}
