#include <Arduino.h>
int DEBUG = 1;

#if !defined(DEBUG_H)
#define DEBUG_H

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#else
#define debug(x) 
#define debugln(x) 
#endif 
#endif // DEBUG_H
