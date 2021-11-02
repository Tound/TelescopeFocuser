/* TELESCOPE FOCUSER HEADER */
#pragma once

#ifndef TELESCOPEFOCUSER_H
#define TELESCOPEFOCUSER_H

#include <QMC5883LCompass.h>
#include <Adafruit_MPU6050.h>


class TiltCompass{
  public:
    TiltCompass(QMC5883LCompass compass, Adafruit_MPU6050 accel);
    char tiltCompensate();
    byte getTemp();
    byte getElevation();
    sensors_event_t a, g, temp;
    QMC5883LCompass getCompass();
    void setCompass(QMC5883LCompass compass);
    Adafruit_MPU6050 getAccel();
    void setAccel(Adafruit_MPU6050 accel);
    
  private:
    QMC5883LCompass compass;
    Adafruit_MPU6050 accel;
};

void TiltCompass::TiltCompass(QMC5883LCompass compass, Adafruit_MPU6050 accel){
  compass = compass;
  accel = accel;
}

byte TiltCompass::getTemp(){

}

byte TiltCompass::getElevation(){
  

}


char TiltCompass::tiltCompensate(){
  compass.read();
  accel.getEvent(&a, &g, &temp);
  
  int x,y,z;
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  
  byte pitch = g.gyro.y;
  byte roll = g.gyro.x;
  byte _x = x*cos(pitch) + y*sin(roll)*sin(pitch) – z*cos(roll)*sin(pitch);
  byte _y = y*cos(roll) + z*sin(roll);
  
  // Calculate the new tilt compensated heading
  byte heading = atan2(_y, _x) * RAD2DEG;
  char dir[3];
  compass.getDirection(dir, heading); 
  return dir;
}


#endif
