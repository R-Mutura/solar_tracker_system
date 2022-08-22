#include <Arduino.h>

#include "magnetometer.h"
#include <Wire.h>
#include <QMC5883LCompass.h>


QMC5883LCompass compass;

void initialize_mag()
{ 
    compass.init();
      
}

int get_angle(){
   //the azimuth a ngle is the angle away from the notrth
   //when pointing north, therfore, the azimuth angle is zero.
    // Read compass values
      compass.read();

      // Return Azimuth reading
     return(compass.getAzimuth());

}