#include <Arduino.h>
#include"magnetometer.h"
#include"debug.h"
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include<ezTime.h>
#include<math.h>
#include <AccelStepper.h>
#include <EEPROM.h>

#define EEPROM_SIZE 1

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Define pin connections
const int dirPin = 19;
const int stepPin = 18;

int angular_pos = 0;

#define motorInterfaceType 1

AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const float latitude = 0; //latitude at the equator.
                        //going above the equator the latitude increases positively
                          //going below the equatoe, the latitude decreases towards the negative
const int hour_angle_change = 15; //degrees per jour change
const double pi = 3.14159265358979323846;

/*********************************/
//PIN DIFINITIONS

#define red_led   16
#define green_led 17
#define channel_A 12
#define channel_B 13
#define set_pin 35
//encoder type: https://eu.mouser.com/ProductDetail/Broadcom-Avago/HEDS-5505I06?qs=RuhU64sK2%252Bs%2FooH6iJf%2FsQ%3D%3D
/*************************/
int set_flag = 0;
int encoder_pulses = 0;

// time keeping variables
long current_time = 0;
long minute_counts = 0;
long minute_length = 60000;

float get_solar_altitude();
void set_angle ();

void IRAM_ATTR encoder(){
      if(!digitalRead(channel_B)){
        //IF A is falling and B is low then incement the count
          encoder_pulses++;
      }
     else if(digitalRead(channel_B)) {
        //a is falling and and B is high then decrement the count
         encoder_pulses--;
     }
}

void setup() {
  // put your setup code here, to run once:
     Serial.begin(115200);
     pinMode (red_led, OUTPUT  );
     pinMode (green_led, OUTPUT);

     initialize_mag();
     
     EEPROM.begin(EEPROM_SIZE);
     //read the content of eeprom
     angular_pos = EEPROM.read(0);
    if (! rtc.begin()) {
          Serial.println("Couldn't find RTC do not proceed");
          while (1);
     }
    if (!rtc.isrunning()) {
       Serial.println("RTC lost power, lets set the time!");

      }
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


      /*   WiFi.mode(WIFI_STA)	;
         Timezone myTZ;
          myTZ.setLocation(F("Pacific/Auckland")); //set time zone which you are in
        if(!WiFi.begin(ssid , password)){
          debugln("NO WIFI CONNECTION");
        }
      else{
        waitForSync();

        Serial.println();
	      Serial.println("UTC:             " + UTC.dateTime());
        
       
        Serial.print(F("New Zealand:     "));
	      Serial.println(myTZ.dateTime());
      	
      }*/ // TODO: INTEGRATE NTP SERVER TO SET TIME OF THE RTC ACCURATELY WITH LOCATION USING ezTIME

      attachInterrupt(channel_A, encoder, FALLING);
      /********stepper motor setting************/
      	myStepper.setMaxSpeed(1000);
        myStepper.setAcceleration(50);
      	myStepper.setSpeed(200);

      current_time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
    if(!digitalRead(set_pin)){
      set_flag = 1;
       set_angle();
    }
    else{
      set_flag = 0;
    }
  
    if(millis() - current_time > minute_length)//if the millis reach i minute count
    {
      minute_counts++;
      current_time = millis();
    }

    if (minute_counts>= 60)//if we get to 1hr then proceed to do this
    {

        //TODO : READ ENCODER TO GET THE ANGULAR POSITION OF THE STEPPER
         int current_angle = EEPROM.read(0);
         debug("current angle = ");
         debugln(current_angle);

        // TODO: GET THE NEW ANGLE
        float new_angle = get_solar_altitude(); //this is the new angle to move to
        debug("New angle = ");
        debugln(new_angle);
        //TODO: MOVE THE STEPPER MOTOR TO THE NEW ANGLE
        myStepper.moveTo(long (new_angle/0.225));//the variable in move to is absolute from 0  -- 0.225 is the degrees moved per step -- step resulution = 1/8(1600steps/rev)
        myStepper.run();
        debug("MOVED TO = ");
        debugln((new_angle/0.225));
        minute_counts  =  0;
        angular_pos = (encoder_pulses*0.3515625); //get the current abgle as read by the encoder
        debug("New anglar position = ");
        debugln(angular_pos);
        //save to eeprom
        EEPROM.write(0 , angular_pos);
        EEPROM.commit();
        //NEW ANGLE SAVED TO EEPROM
    }
}

/**************************************************************************/
void set_angle(){
  //select set button to enter this mode
   while(set_flag)
   {
        int compass_angle = get_angle();//obtain the zineth angle of the compass
        if (compass_angle == 0)//check that the angle is zero
          {
            /*while in set mode reposition the solar pannel(stepper motor to the zero position and press the boot button 
            once to set the position to zero.*/
            
            //ensure that the stepper motor is zero
            encoder_pulses = 0;//reset the stepper angle to zero
                                 //reset myStepper angle to zero as well.
            myStepper.setCurrentPosition(0); //set the current position of the stepper motor to zero
           
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
               break;
            }
           
   }
}


/**************************************************************************/

/**************************************************************************/
int get_days (int month, int day){
    /*GET NUMBER OF DAYS SINCE THE BEGINNING OF THE YEAR*/
   switch(month){
       case 1: {
           return day;
           break;
       }
       switch(month){
       case 2: {
           return (31+ day);
           break;
       }

       case 3: {
           return (31 + 28.25+ day);
           break;
       }
       case 4: {
           return (31 + 28.25 + 31 + day);
           break;
       }
       case 5: {
           return (31 +28.25+31+30+ day);
           break;
       }
       case 6: {
           return (31 +28.25+31+30+31+ day);
           break;
       }
       case 7: {
           return (31 +28.25+31+30+31+30+ day);
           break;
       }
       case 8: {
           return (31 +28.25+31+30+31+30+31+ day);
           break;
       }
       case 9: {
           return (31 +28.25+31+30+31+30+31+31+ day);
           break;
       }
       case 10: {
           return (31 +28.25+31+30+31+30+31+31+30+ day);
           break;
       }
       case 11: {
           return (31 +28.25+31+30+31+30+31+31+30+31+ day);
           break;
       }
       case 12: {
           return (31 +28.25+31+30+31+30+31+31+30+31+30+ day);
           break;
       }
   }
}
}
/**************************************************************************/
float declination_angle(){
     DateTime now = rtc.now();   

    int days_ellapsed = get_days(now.month() , now.day()); //this is the number of days since the beginning of the year
    float angle = 0;
    float y= cos((360/365)*( pi/180)); //cosing of 360/365 in radians
    y = (23.45* (y*(180/pi))); //convert angle Y to degrees the add it to 23.45
    angle = y*(days_ellapsed + 10);
    return angle;//return declination angle in degrees
}

float hour_angle(){
       DateTime now = rtc.now(); 
       int hour = now.hour();
       int minutes = now.minute();

       hour = hour+(minutes/60);
       float hour_angle = (hour - 12.00);
      
 return (hour_angle * hour_angle_change); //return the hour angle of the sun

}

float get_solar_altitude(){

     int y = sin(latitude *pi/180 );
     y = y*sin(declination_angle()* (pi/180));
     int x = cos(latitude *pi/180 ) *(cos(hour_angle()) * pi/180 );//we get int x in radians
    
     float angle = x+y;//add the x and y values in radians
     // the sin inverse if angle we get
    angle = asin(angle);
    //conver to degre 
    angle = (angle *  180/pi) ;
    return angle;
    }

