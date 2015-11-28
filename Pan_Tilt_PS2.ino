/*
 
   -  SKETCH DESCRIPTION  - 
This sketch uses the PS2X by Bill Porter and default Servo libraries to use a PS2 controller to control pan 
and tilt servos. 

For full instructions and diagrams visit: http://nickkamm.com/control-pan-tilt-servos-using-a-ps2-controller-arduino/

Nick Kamm
2015-11-08
Rev 1.04

   -  WIRING  - 
PS2 CONTROLLER to ARDUINO: BLUE->12; BROWN->11; ORANGE->10; YELLOW->9; RED->3.3V; BLACK->GND; 
10kohm pull-up resistor from BROWN (data) to 5V (or 3.3V) 
PAN SERVO to ARDUINO:  SIGNAL -> 6; POS -> 5V; NEG -> GND  (it's good practice to power servos from non-arduino source)
TILT SERVO to ARDUINO: SIGNAL -> 5; POS -> 5V; NEG -> GND

  
    - TO DO  - 
      Add note about inverting y axis
      Note about removing serial output 

*/

//  - LIBRARIES -
#include <Servo.h> 
#include <PS2X_lib.h>

//  -  OBJECT DECLARATIONS  - 
Servo servo_pan;  // create servo object to control a pan servo 
Servo servo_tilt;  // create servo object to control tilt servo
PS2X ps2x; // create PS2 Controller Class

 // - GLOBAL VARIABLES -
int error = 0; 
byte type = 0;
byte vibrate = 0;
int pin_pan = 6; // assigns output pin to pan servo - should be a (~ PWM) pin
int pin_tilt = 5; // assigns output pin to tilt servo - should be a (~ PWM) pin
int SERVO_PAN_POS = 1533; //initial position, tweak as necessary
int SERVO_TILT_POS = 1725; //initial position, tweak as necessary
int panAdjust; // Pan increment adjustment value
int tiltAdjust; // Tilt increment adjustmenet value
int BOUNDUP = 145; // Upper analog stick threshhold, increase if servo jittery when joystick not in use
int BOUNDDOWN = 95; //Lower analog stick threshhold, decrease if servo jittery when joystick not in use
int writedelay = 15; // Delay that allows servo to find written position, adjust this proportional to xHI/xLO values if changed
/*
These variables control the adjustment amount, ie the speed of the servos, increase values to increase speed. 
When thumbstick is all the way in a direction the servo
 receives the high speed value, anything less is low speed.  
*/
int xHI = 10;
int xLO = 3;
int yHI = 10;
int yLO = 3;

void setup() {
  Serial.begin(57600);
  ServoStartup(); // run startup sequence, comment out if you don't want to run at startup
    error = ps2x.config_gamepad(9,11,10,12, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 String errorVal;
 errorVal=errorFunc(error); // sends error value to errorFunc() and returns string
 Serial.println(errorVal); //prints error msg to serial monitor
   
   type = ps2x.readType(); //checks type of controller and prints type to Serial Monitor
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
     }

// Outputs initial positions to serial monitor
   Serial.print("Initial Pan Position: "); 
   Serial.println(servo_pan.readMicroseconds());
   Serial.print("Initial Tilt Position: ");
   Serial.println(servo_tilt.readMicroseconds());
}

void loop() {
  
  if(error == 1) //skip loop if no controller found
  return; 
  
 else { //DualShock Controller
      ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed  
       
  // ------------------------------------- START:  Servo Control Code  --------------------------//
  int panX = ps2x.Analog(PSS_RX); //reads right analog stick X values to variable
  int tiltY = ps2x.Analog(PSS_RY); //reads right analog stick Y values to variable
  
 // Code for panning right
  if (panX > BOUNDUP && SERVO_PAN_POS < 2300){  
      servo_pan.attach(pin_pan); // attached servo_pan to output pin
      if (panX > 253){
        panAdjust = xHI; // speed set high when analog stick fully engaged
      }
      else{
        panAdjust = xLO; // speed set low when analog stick partially engaged
      }
      SERVO_PAN_POS = SERVO_PAN_POS + panAdjust;
      servo_pan.writeMicroseconds(SERVO_PAN_POS);
    delay (writedelay); // delay for movement allowance
  }
  // Code for panning left
  if (panX < BOUNDDOWN && SERVO_PAN_POS > 500){
    servo_pan.attach(pin_pan); // attach servo_pan to output pin
    if(panX < 2){
      panAdjust = xHI;
    }
      else{
        panAdjust = xLO;
      }
      SERVO_PAN_POS = SERVO_PAN_POS - panAdjust;
      servo_pan.writeMicroseconds(SERVO_PAN_POS);
     delay (writedelay); // delay for movement allowance
    }
    
    // Code for tilting up
    if (tiltY > BOUNDUP && SERVO_TILT_POS > 530){           
      servo_tilt.attach(pin_tilt); // attached servo_tilt to output pin
      if(tiltY > 253){
        tiltAdjust = yHI;
      }
      else {
        tiltAdjust = yLO;
      }
      SERVO_TILT_POS = SERVO_TILT_POS - tiltAdjust;
      servo_tilt.writeMicroseconds(SERVO_TILT_POS);
      delay (writedelay);
    }
      
       // Code for tilting down
    if (tiltY < BOUNDDOWN && SERVO_TILT_POS < 2180){
      servo_tilt.attach(pin_tilt); // attached servo_tilt to output pin
      if(tiltY < 2){
        tiltAdjust = yHI;
      }
      else {
        tiltAdjust = yLO;
      }
      SERVO_TILT_POS = SERVO_TILT_POS + tiltAdjust;
      servo_tilt.writeMicroseconds(SERVO_TILT_POS);
      delay (writedelay);
    }
    /*
    Code to print position values for troubleshooting, comment this out to increase servo speed and reliability
    if(panX > BOUNDUP || panX < BOUNDDOWN || tiltY > BOUNDUP || tiltY < BOUNDDOWN){ // prints positions whenever analog stick is not in centered position
     Serial.print("panAdjust: ");   
     Serial.print(panAdjust);
     Serial.print(", tiltAdjust: "); 
     Serial.println(tiltAdjust);
     Serial.print("panX: ");
     Serial.print(panX);
     Serial.print(", tiltY: ");
     Serial.println(tiltY);
     Serial.print("Pan Pos.: ");
     Serial.print(servo_pan.readMicroseconds());
     Serial.print(" deg, ");
      Serial.print(", Tilt Pos.: ");
     Serial.print(servo_tilt.readMicroseconds());
     Serial.println(" deg");
     Serial.println(" --------------------------------- ");
      }
      */
    
    if (panX < BOUNDUP && panX > BOUNDDOWN){ 
     servo_pan.detach(); // turns off servo when not moving to avoid searching noise on low cost hobby servos
     delay (5);
  }
    if(tiltY < BOUNDUP && tiltY > BOUNDDOWN){
     servo_tilt.detach(); // turns off servo when not moving to avoid searching noise
     delay (5);
  }
  
  //--------------------------------------END: Servo Control Code ----------------------------//
  

   } // end controller check else statement
} // end void loop()



String errorFunc(int x){ // controller error che
String y;
 if(x == 0){
  y = "Found Controller, configured successful."
  " Try out all the buttons, X will vibrate the controller, faster as you press harder"
  " holding L1 or R1 will print out the analog stick values. ";
 }
   
  else if(x == 1)
  y = "No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips";
   
  else if(x == 2)
  y = "Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips";
   
  else if(x == 3)
  y = "Controller refusing to enter Pressures mode, may not support it. ";
  
   return y;
 }
 
 void ServoStartup(){
   int delay1=450;
  servo_pan.writeMicroseconds(SERVO_PAN_POS); //set initial servo position if desired, 
  servo_tilt.writeMicroseconds(SERVO_TILT_POS); //set initial servo position if desired
  servo_pan.attach(pin_pan);  //attaches servo_pan to output pin
  servo_tilt.attach(pin_tilt); //attaches servo_tilt to output pin
  delay(delay1); 
  servo_pan.write(150);
  delay(delay1);
  servo_pan.write(30);
  delay(600);
  servo_pan.writeMicroseconds(SERVO_PAN_POS);
  delay(delay1);
  servo_tilt.write(140);
  delay(150);
  servo_tilt.writeMicroseconds(SERVO_TILT_POS);
  delay(150);
  servo_tilt.write(140);
  delay(150);
  servo_tilt.writeMicroseconds(SERVO_TILT_POS);
  delay(delay1); // time for the servo to travel to center before detaching 
  servo_pan.detach(); // detach servos when not in use to avoid search noise
  servo_tilt.detach();
   delay(10);
 }
