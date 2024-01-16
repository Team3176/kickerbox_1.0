#include <Servo.h>
#include <LiquidCrystal.h>
Servo motorController;

#define MOTOR_NEUTRAL  90.0
float mcOutput = MOTOR_NEUTRAL;
int sliderPort = A1;
int rawSlider = 512;
float percentMotor = 0;
float deadBandLow = 492;
float deadBandHigh =532;
float floatSlider = 0;
int clearEstop = 1;
int seteStop = 1;
int pauseToggle = 1;
enum mode{
  estop=0,
  runnin=1,
  paused=2
};
  mode curMode = estop;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
long lastTime = 0;
bool pauseDebounceState = 0;


void setup()
{
  Serial.begin(9600);
  motorController.attach (11,1000,2000);
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  lcd.print("LCD BOOT"); // print a simple message
  pinMode(3, INPUT);
  pinMode(12, INPUT);
  pinMode(2,INPUT);
  digitalWrite(3,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(2,HIGH);
}



void loop()
 { 
 
  
    //inputs
  rawSlider = analogRead(sliderPort);
  clearEstop = digitalRead(3);
  seteStop = digitalRead(12);
 /*
 //Debounce the pause toggle
    if(clearEstop==0){
      //if button not pressed
      lastTime = millis();
      pauseToggle = 0;
      pauseDebounceState = 0;
    }else if(clearEstop == 1){
      //Button is pressed
      if(millis()-lastTime < 2000){
            pauseToggle = 0;
        }else{
            //one shot pause
            if(pauseDebounceState==0){
              pauseToggle = 1;
              pauseDebounceState = 1;
            }else{
              pauseToggle = 0;
            }
        }
      }

*/
    


  //******processing*******
  //+Processing Slider Into Motor Percent
  floatSlider = rawSlider;
  if (rawSlider < deadBandLow){
    //If Slider is in the negative range
    percentMotor = floatSlider*100.0/492.0 -100.0;
    
  }else if(rawSlider > deadBandHigh){
    // If Slider is in the high range
    percentMotor = floatSlider * 100.0 /491.0 -108.4;
    
  }else{
    //If Slider is in the deadband range
    percentMotor= 0;
  }
   //-Processing Slider Into Motor Percent
   //+Proccessing Motor percent into motor outputs+//
 switch(curMode){
  case estop:
    //+Perform Case Actions+//
      //set the  motor output to zero
    if(rawSlider<deadBandHigh && rawSlider>deadBandLow && clearEstop == 0){
      //Then set mode = running
   curMode = runnin;
    }else {
          mcOutput = MOTOR_NEUTRAL;
        }
      
   
     
   
   
   
  break;
  case runnin:
    //+Perform Case Actions
        //set the motor output to pctMotor
      //+Evaluate Transition
      //if estop is pressed then set mode=estop
         if (seteStop == 0){
               curMode = estop;
         }else {
           if (pauseToggle == 0){
             curMode = paused;
           }else {
            //mcOutput is passed to the servo clas which takes a value from 0 to 180 deg with 0-90 being reverse, 90 being stopped, and 90-180 being forwared
            //Note that "servo" expepects a 180 deg servo, and we are instead using it for a motor controller. This equation scales accordingly. 
             mcOutput = 0.9*percentMotor+90;
               
           }
         }
               
               
break;
  case paused:
    //+Perform Case Actions
    //+Evaluate Transition
      //if estop is pressed then set mode=estop
    if (seteStop == 0){
          curMode = estop;
    }else {
      if(pauseToggle == 0) {
      curMode = runnin;
      }else {
        mcOutput = MOTOR_NEUTRAL;
      }
    }
   
 
    break;
  default:
    //+Perform Case Actions
      //set motor output to zero
        mcOutput = MOTOR_NEUTRAL;
    //+Evaluate Transition
      //set mode = estop
        curMode = estop;
  break;
  }
  //-Proccessing Motor percent into motor outputs-//
  //outputs

  motorController.write(mcOutput);
  
//Write LCD Line 1
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Sld:");
  lcd.setCursor(4,0);
  lcd.print(rawSlider);
  lcd.setCursor(7,0);
  lcd.print("Pct");
  lcd.setCursor(10 ,0);
  lcd.print(percentMotor);
  
  //Write LCD Line 2

  switch(curMode){
    case estop:
    lcd.setCursor(0,1);
      if(percentMotor != 0 ){
         lcd.print("ESTP,SldToCenter");
      }else
        lcd.print("ESTP,Press Reset ");
    break;
    case runnin: 
      lcd.setCursor(0,1);
      lcd.print("RUNNING         ");
    break;
    case paused:
      lcd.setCursor(0,1);
      lcd.print("PAUS:PrsRstToRun");
    break;

    default:
      lcd.setCursor(0,1);
      lcd.print("something broke ");
    break;

  }

 
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
  
