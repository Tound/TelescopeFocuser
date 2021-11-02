#include "Tilt_Compass.h"

void TiltCompass::init(){
  compass.init();
  compass.setCalibration(-1081, 658, -40, 1398, -2200, 0);
  Serial.println("Compass intialised");

  if (!accel.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  accel.setAccelerometerRange(MPU6050_RANGE_8_G);
  accel.setGyroRange(MPU6050_RANGE_500_DEG);
  accel.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("Tilt Compass intialised");
}

char TiltCompass::getTemp(){
  return temp.temperature;
}

int TiltCompass::getElevation(){
  return a.acceleration.y * RAD_TO_DEG;
}


void TiltCompass::tiltCompensate(char dir[3]){
  compass.read();
  accel.getEvent(&a, &g, &temp);
  
  int x,y,z;
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  
  float pitch = a.acceleration.x;
  float roll = a.acceleration.y;
  float _x = x*cos(pitch) + y*sin(roll)*sin(pitch) - z*cos(roll)*sin(pitch);
  float _y = y*cos(roll) + z*sin(roll);


  Serial.print("X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.print(y);
  Serial.print(", Z: ");
  Serial.println(z);
  Serial.print("NEW X: ");
  Serial.print(_x);
  Serial.print(", NEW Y: ");
  Serial.println(_y);

  Serial.print("Pitch: ");
  Serial.print(pitch*RAD2DEG);
  Serial.print(", Roll: ");
  Serial.println(roll*RAD2DEG);

  
  // Calculate the new tilt compensated heading
  byte heading = atan2(_y, _x) * RAD_TO_DEG;
  //Serial.println(heading);
  //Serial.print("\n\n\n");
  compass.getDirection(dir, heading);
}
