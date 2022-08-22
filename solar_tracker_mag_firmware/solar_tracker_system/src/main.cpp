#include <Arduino.h>
#include"magnetometer.h"
#include"debug.h"


/*********************************/
//PIN DIFINITIONS

#define red_led   16
#define green_led 17

/*************************/
int set_flag = 0;
void set_angle ();


void setup() {
  // put your setup code here, to run once:
     pinMode (red_led, OUTPUT  );
     pinMode (green_led, OUTPUT);
     initialize_mag();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void set_angle(){
  //select set button to enter this mode
   while(set_flag)
   {
        int compass_angle = get_angle();
        if (compass_angle == 0)
          {
            digitalWrite(green_led, HIGH);
            digitalWrite(red_led, LOW);
             debugln (NORTH FOUND!!);

            break;
          }
        else
            {
               digitalWrite(red_led, HIGH);
               digitalWrite(green_led, LOW);
               debugln (please locate the north);
            }
           
   }
}