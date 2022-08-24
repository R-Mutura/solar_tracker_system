

#if !defined(DEBUG_H)
#include <Arduino.h>
int myDEBUG = 1;
#define DEBUG_H

#if myDEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#else
#define debug(x) 
#define debugln(x) 
#endif 
#endif // DEBUG_H
