/* TELESCOPE FOCUSER HEADER */
#pragma once

#ifndef TILT_COMPASS_H
#define TILT_COMPASS_H

#include <QMC5883LCompass.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#define RAD2DEG 18/PI

class TiltCompass{
  public:
    //TiltCompass(QMC5883LCompass &compass, Adafruit_MPU6050 accel);
    void tiltCompensate(char dir[3]);
    void init();
    char getTemp();
    int getElevation();
    sensors_event_t a, g, temp;
    QMC5883LCompass getCompass();
    //void setCompass(QMC5883LCompass _compass);
    Adafruit_MPU6050 getAccel();
    //void setAccel(Adafruit_MPU6050 _accel);
    
  private:
    QMC5883LCompass compass;
    Adafruit_MPU6050 accel;
};

#endif
