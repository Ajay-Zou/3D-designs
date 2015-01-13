

/*
 Syringe Pump Programmer for USB Input:
 By Chase Smith
 
 This will give the user a serial prompt at the time of upload. 
 By following the serial instructions, the user can program te Syringe Pump, 
 and when they are done they can save the values to the EEPROM of the 
 microcontroller, unplug the controller, and plug it in later for further use, with
 the specific syringe and dose in the EEPROM. 
   However, if the user does not want to change old values they can 
 ignore the serial, or simply plug the microcontroller into a power outlet and wait until the 
 LED starts to blink slowly. Once the LED begins to blink, the user knows that the 
 syringe pump is set-up with the previous values, and can be fully utilized.
   Note that the interrupt that triggers the syringe pump is a RISING interrupt, meaning 
 it will go from LOW to HIGH.
 *** (EEPROM works with programmer set to USBtinyISP, and the Arduino UNO board)
 */

#include "avr/sleep.h"
#include "EEPROMEx.h"

//#include "Wire.h"
//permanent variables for syringe specs

//the syringe pump variables
int affirm=0;
int decline=0;
int timer=0;
float displace; //cm displaced by the servo per turn
float diameter;
int time;
char syringeMan;
char inString[12]; //used to gather input data and convert to float
int index=0; //used with the char array to gather input data
int counter=0; //used for switch cases(syringe pump)
int current = 0; //used for switch cases as well(Syringe pump)
float dose; //volume of syringe, volume of dose
float volume;
boolean done=false; //boolean for starting
boolean printout=true; //boolean for welcome printout 
boolean calibrated=false;//boolean for setup of motor
boolean configured=false; //boolean for syringe 
boolean isConn=false;//boolean for the system input
boolean runn=true;
int switchPin=0; //on the pcb this will be 1
int input=1; //on the pcb this will be 0
//pins and nonfunction related variables
//variables for the pins on arduino
int servo=6;// pin number of the servo // on the pcb will be pin 6
int led=5; // pin number of led // on pcb this will be pin 5
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //Wire.begin(1); //begin wire at default
  pinMode(servo,24);
  pinMode(3, INPUT); //establish interrupt pin as input
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  //setting up the servo and syringe specs

  if(calibrated==false){
    CalibrateServo();
    calibrated=true;
  }
  if(configured==false && calibrated==true){
    ConfigureSyringeSpecs();
    configured=true;
  }
}


//puts the arduino to sleep unless the correct input pin is pulsed from high to low, it then wakes up
//and runs the servo
void loop(){
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  interrupts();
  attachInterrupt(input, wakeUp, RISING); //the interrupt is actually attached to pin 2
  //attachInterrupt(switchPin, stopPump, LOW); //interrupt will cause the motor to stop if switch hits low
  sleep_mode();
  run();
  /*only use this code when the audio jack is being used as the interrupt for the pump
  /*if(runn!=false){
    run();
  }
  if(run==false){
    detachInterrupt(input);
    attachInterrupt(switchPin, startPump, RISING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    runn=true;
  }*/
}
//function that wakes the servo up
void wakeUp(){
  sleep_disable();
  analogWrite(led, HIGH);
  
}
//stops the pump when the motor goes too far activated by a switch pin
void stopPump(){
  sleep_disable();
  blinkLed();
  runn=false;
}
//starts the pump after the syringe has been replaced
void startPump() {
  sleep_disable();
}
//runs the servo the amount needed
void run(){
  Serial.println("Running");
  time=EEPROM.readInt(5);
  analogWrite(led, LOW);
  analogWrite(servo, 24);
  delay(time);
  analogWrite(servo, 0);
  Serial.println("Done!");
  delay(35);
}
//blinks the led
void blinkLed(){
  int i;
  for(i=0; i<3; i++){
  digitalWrite(led, HIGH);
  delay(50);
  digitalWrite(led, LOW);
  delay(50);
  } 
} 
//does the math to get the time
void calculateTime(float dia, float dos, float dis){
  //find area of the circle created by the syringe
  float circle=3.14159265359*dia*dia/4;
  //find amount to press in(cm)
  float nDisplace=dose/circle;
  float temp=nDisplace/dis; //putting height needed over cm traveled per degree
  time= (int)(temp*(1700)); //converting degrees to time
}
//gets the syringe from a manufacturers name and volume
void findSyringe(char name, int vol){
  if(name=='B' || name=='b'){
    if(vol==1){
      diameter=0.4699;
      Serial.println("This is a B-D 1ml syringe.");
    }
    if(vol==3){
      diameter=0.8585;
      Serial.println("This is a B-D 3ml syringe.");
    }
    if(vol==5){
      diameter=1.199;
      Serial.println("This is a B-D 5ml syringe.");
    }
    if(vol==10){
      diameter=1.443;
      Serial.println("This is a B-D l0ml syringe.");
    }
    if(vol==20){
      diameter=1.905;
      Serial.println("This is a B-D 20ml syringe.");
    }
    if(vol==30){
      diameter=2.159;
      Serial.println("This is a B-D 30ml syringe.");
    }
    if(vol==60){
      diameter=2.659;
      Serial.println("This is a B-D 60ml syringe.");
    }
  }
  if(name=='M' || name== 'm'){
    if(vol==1){
      diameter=0.574;
      Serial.println("This is a Monoject 1ml syringe.");
    }
    if(vol==3){
      diameter=0.8941;
      Serial.println("This is a Monoject 3ml syringe.");
    }
    if(vol==5){
      diameter=1.27;
      Serial.println("This is a Monoject 5ml syringe.");
    }
    if(vol==10){
      diameter=1.572;
      Serial.println("This is a Monoject l0ml syringe.");
    }
    if(vol==20){
      diameter=2.012;
      Serial.println("This is a Monoject 20ml syringe.");
    }
    if(vol==35){
      diameter=2.352;
      Serial.println("This is a Monoject 35ml syringe.");
    }
    if(vol==60){
      diameter=2.664;
      Serial.println("This is a Monoject 60ml syringe.");
    }
    if(vol==140){
      diameter=3.80;
      Serial.println("This is a Monoject 140ml syringe.");
    }
  }  
}
//configures the Syringe specs using volume and manufacturer's name(only uses first letter)
void ConfigureSyringeSpecs() {
  affirm=0;
  decline=0;
  int inChar;
  current=0;
  index=0;
  // send an intro: and reset all values from previous use(index, done, current) and turn off with boolean printout
  while(printout!=false){ 
    Serial.println("Do you want to use a new syringe?(Y/N)");
    delay(25);
    current=0;
    index=0;
    isConn=false;
    printout=false;   
  }
  while(isConn!=true){
    if(Serial.available()>0){
      isConn=true;
      done=false;
      break;
    }
    if(timer >= 600){
      time=EEPROM.readInt(5); 
      Serial.println(time);
      delay(25);
      configured=true;
      done=true;
      isConn=true;
      break;
    }     
    delay(100);
    timer++;
  }
  //reads the users input on the serial moniter and saves as the values for volume, diameter, dose, circle, nDisplace
  while(done!=true && isConn==true){
    // Read serial input:
    if(Serial.available() > 0) {
      inChar = Serial.read();
      //adds any digit or '.' to a char arraylist instring
      if (isDigit(inChar) || inChar=='.' || isprint(inChar)) {
        // convert the incoming byte to a char 
        // and add it to the string:
        inString[index] = (char)inChar;
        index++;
      }
      if(inChar=='Y' || inChar=='y'){
        switch(affirm) {
        case 0:
          Serial.println("Input the new syringe specifications:");
          Serial.println();
          Serial.println("Please enter the volume of the syringe(ml): "); 
          decline++;//have to add one to decline to make sure answers are for the correct question
          current=0;
          break;
        case 1:
          Serial.println("The syringe is correct!");
          delay(500);
          Serial.print("Do you wish to change the dose from : ");
          Serial.print(EEPROM.readFloat(1));
          Serial.println("? (Y/N)");
          decline=3;
          break;
        case 2:
          Serial.println("Please input the dose(ml): ");
          delay(50);
          decline=2;
          break;
        case 3:  
          Serial.println("Values correct!");
          EEPROM.updateInt(5, time);
          EEPROM.updateFloat(1, dose);
          Serial.println(EEPROM.readInt(5));
          delay(75);
          Serial.println("Begin testing!");
          delay(25);
          done=true;
          configured=true;
          break;
        }
        affirm++;
        index=0;
      }
      if(inChar=='n' || inChar=='N'){
        switch(decline) {
        case 0:
          Serial.println("Begin testing!");
          delay(50);
          time= EEPROM.readInt(5);
          Serial.println(time);
          delay(25);
          index=0;
          configured=true;
          done=true;
          break;
        case 1:
          break;
        case 2:
          Serial.println("Input the correct data for the syringe");
          Serial.println();
          Serial.println("Please input the volume(ml): ");
          current=0; //have to reset the process
          decline=0;
          index=0;
          break;
        case 3:
          Serial.println("The dose was not changed");
          Serial.println();
          calculateTime(diameter, EEPROM.readFloat(1), EEPROM.readFloat(3));
          EEPROM.updateInt(5,time);
          Serial.println("Begin Testing!");
          delay(25);
          done=true;
          affirm=0;
          decline=0;
          configured=true;
          index=0;
          break;
        case 4:
          Serial.println("Input the correct dose: ");
          current=5;
          index=0;
          break;
        }
        decline++;
      }
      // if you get a new line convert array to a float,
      // and save float as appropriate variable
      if (inChar == '\n' ) {
        switch (current) {
        case 0:
          break;
        case 1:
          //add null to end of array to make it work and save as volume
          inString[index]='\0';
          volume= atof(inString);
          Serial.print("Volume of Syringe: ");
          Serial.println(volume);
          Serial.println("Please input the syringe manufacturer: ");
          break;
        case 2:
           inString[index]='\0';
          syringeMan=inString[0];
          findSyringe(syringeMan, (int) volume);
          delay(100);
          Serial.println(diameter);
          Serial.println("Is this the correct syringe?(Y/N) ");
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          inString[index]='\0';
          dose= atof(inString);
          //calculate run time of the servo 
          calculateTime( diameter, dose, displace);
          // print the results and calculations to the screen
          Serial.print("Dose: ");
          Serial.println(dose);
          Serial.print("Run time per dose: ");
          Serial.println(time);
          Serial.println();
          Serial.println("Are these values correct?(Y/N)");
          break;
        case 6:
          break;
        }
        //every time the function goes through this increment current and set index to zero to build over the inString array for next variable and input
        current++;
        index=0;
      }
    }
  }
}
//calibrates the motor, to set the amount of distance displaced per degree  
void CalibrateServo(){
  affirm=0;
  decline=0;
  int inCharr;
  counter=0;
  isConn=false;
    Serial.println("Do you wish to calibrate the Syringe Pump Servo(Y/N)?");
  while(isConn!=true){
    if(Serial.available()>0){
      isConn=true;
      done=false;
      break;
    }
    if(timer==600){
      delay(25);
      dose=EEPROM.readFloat(1);
      time=EEPROM.readFloat(5);
      Serial.println(time);
      delay(25);
      Serial.println(displace);
      delay(25);
      blinkLed();
      configured=true;
      calibrated=true;
      done=true;
      isConn=true;
      break;
    } 
    delay(100);
    timer++;
  }
  while(done!=true){
    if(Serial.available()>0){ 
      inCharr=Serial.read();
      if (isDigit(inCharr) || inCharr=='.') {
        // convert the incoming byte to a char 
        // and add it to the string:
        inString[index] = (char)inCharr; 
        index++;
      }
      //used for the yes/no requests
      if(inCharr=='Y'|| inCharr=='y'){
        switch(affirm) {
        case 0:
          //calibrate the servo to the thread of the syringe
          Serial.println("It's time to calibrate the servo!");
          Serial.println("For maximum accuracy place a syringe");
          Serial.println("on the pump and measure the distance(cm)"); 
          Serial.print("displaced by 5"); 
          Serial.println(" revolutions. Press any key to continue."); 
          decline++;
          break;
        case 1:
          Serial.println("Saving...");
          Serial.println("The data was saved!");
          EEPROM.updateFloat( 3, displace);
          delay(100);
          done=true;
          break;
        }
        affirm++;
        index=0; 
      }
      if(inCharr=='n' || inCharr== 'N'){
        switch(decline) {
        case 0:
          Serial.println("Skipping servo calibration.");
          Serial.println();
          displace=EEPROM.readFloat(3);
          calibrated=true;
          done=true;
          break;
        case 1:
          Serial.println("The data was not saved.");
          done=true;
          break;
        }
        decline++;
        index=0;
      }
      if(inCharr=='\n'){
        switch (counter) {
        case 0:
          break;
        case 1:
          Serial.println("Servo Running, please do not disturb the pump.");
          analogWrite(servo, 24);
          delay((int)1700*5); //equal to about 10 rev
          analogWrite(servo, 0);
          Serial.println("How much distance(cm) was displaced by the servo?");
          break;
        case 2:  
          inString[index]='\0';
          displace = atof(inString)/5.00;
          Serial.println("The pump has now been calibrated.");
          Serial.println("Do you wish to save this value?(Y/N)");
          break;
        case 3:
          break;
        }
        counter++;
        index=0;
      }
    }
  }
}




