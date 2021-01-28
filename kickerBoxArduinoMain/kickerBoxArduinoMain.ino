#include <Servo.h>

Servo motorController;

float mcOutput = 0;

int sliderPort = A0;

int rawSlider = 512;
float percentMotor = 0;
float deadBandLow = 492;
float deadBandHigh =532;
float floatSlider = 0;
int clearEstop = 0;
int seteStop = 0;
int pauseToggle = 0;

enum mode{
  estop=0,
  running=1,
  paused=2
};

mode curMode = estop;


void setup()
{
  Serial.begin(9600);
  motorController.attach(9,1000,2000);
   pinMode(2, INPUT);
    pinMode(4, INPUT);
    pinMode(3,INPUT);
    digitalWrite(2,HIGH);
    digitalWrite(4,HIGH);
    digitalWrite(3,HIGH);
}

void loop()
 { 
    //inputs
  
  rawSlider = analogRead(sliderPort);
  clearEstop = digitalRead(2);
  seteStop = digitalRead(4);
  pauseToggle = digitalRead(3);
  //******processing*******
  //+Processing Slider Into Motor Percent
  floatSlider = rawSlider;
  if (rawSlider < deadBandLow){
    //If Slider is in the negative range//
    percentMotor = floatSlider*100/492 -100;
    
  }else if(rawSlider > deadBandHigh){
    // If Slider is in the high range
    percentMotor = floatSlider * 100 /491 -108.4;
    
  }else{
    //If Slider is in the deadband range
    percentMotor= 0;
  }
    
  //-Processing Slider Into Motor Percent
  //+Processes motor percenet into motor outputs+//
 
  switch(curMode){
  case estop:
    //+Perform Case Actions+//
      //set the  motor output to zero
        mcOutput = 0;
    //+Evaluate Transition
      //If slider in DB, and button pressed
        if(rawSlider<deadBandHigh && rawSlider>deadBandLow && clearEstop == 0){
      //Then set mode = running
              curMode = running;
            }
  break;
  case running:
    //+Perform Case Actions
        //set the motor output to pctMotor
         mcOutput = .9*percentMotor+90;
      //+Evaluate Transition
      //if estop is pressed then set mode=estop
         if (seteStop == 1){
               curMode = estop;
             }
      //if paused is pressed then set mode=paused
      if (pauseToggle == 1){
            curMode = paused;
         }
  break;
  case paused:
    //+Perform Case Actions
      //set motor output to zero
        mcOutput = 0;
    //+Evaluate Transition
      //if estop is pressed then set mode=estop
    if (seteStop == 1){
          curMode = estop;
    }
      //if paused is pressed then set mode=running
    if (pauseToggle == 1){
      curMode = running;
    }
    break;
  default:
    //+Perform Case Actions
      //set motor output to zero
        mcOutput = 0;
    //+Evaluate Transition
      //set mode = estop
        curMode = estop;
  break;
  }
  
  //-Processes motor percenet into motor outputs-//
  
  //outputs
  motorController.write(mcOutput);
  
  
  Serial.print("Estop.off ");
  Serial.print(clearEstop);
  Serial.print(" Estop? ");
  Serial.print(seteStop);
  Serial.print(" Toggled ");
  Serial.print(pauseToggle);
  Serial.print(" Mode ");
  Serial.print(curMode);
  Serial.print(" The Slider is ");
  Serial.print(rawSlider);
  Serial.print(" The Motor is ");
  Serial.print(percentMotor);
  Serial.print(" The Angle Is "); 
  Serial.print(mcOutput);
  Serial.println(" ");
 }




  
