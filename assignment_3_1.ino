//Clap clap robot by group 6 
//c programming language

#include <LiquidCrystal.h>          //the liquid crystal library contains commands for printing to the display
#include <math.h> //using maths function for log10 used for int spacing in Number_of_Claps


//Motor Α
const int PWMA = 5; // enA is pin9 
const int AIN1 = 3; // in1 is pin8
const int AIN2 = 4; // in2 is pin7


// Motor Β
const int PWMB = A2; // enB is pin3
const int BIN1 = 2; // in3 is  pin5
const int BIN2 = A1; // in4 is pin4

// LCD pins
LiquidCrystal lcd(8,9,10,11,12,13);

//Distance sensor pins
const int echopin = 7; 
const int trigpin = 6; 

//sound senor pin
const int SoundSensor = A3; //sound sensor output is pin A3

// RGB pin
const int greenpin = A4;	//green led is pin A4
const int redpin = A5;	//red led is pin A5 
  
//OGO (22363533) VARAIABLES
//setting up structure to collect graph x and y values  
struct GRAPH{ unsigned long int System_Time; //x value is time 
              unsigned long int Db_lvl; // y value is sound sensor value
            }; 

unsigned long int *pDecibels;//creating pointer for sound sensor value
unsigned long int *pSysTime; //creating pointer for time at which sound sensor value is recorded
static unsigned long int dB_Time_Array[150][2]={}; //2D array to store values from GRAPH structure

const int lcd_allow = 2000;//allows for user to read lcd for 2 secs
const int rightleft_turn = 1000;//time to turn right or left and then robot should go forwards
const int too_close = 10;
//END OF OGO VARIABLES


void setup()
{
  Serial.begin(9600);	//setting up serial monitor
  
  //motor A setup
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  
  //motor B setup
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  
  //lcd setup
  lcd.begin(16, 2); 
  lcd.clear();
  
  //sound sensor setup
  pinMode(SoundSensor, INPUT);//sound sensor is taking in the values
  
  //Ultrasonic distance sensor setup
  pinMode(echopin, INPUT);//echopin is an input
  pinMode(trigpin, OUTPUT);//trigpin is an output
  
  //RGB setup all outputs
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);

}

//VOID LOOP ASSEMBLED BY OGO (22363533)
void loop()
{

  digitalWrite(PWMA, HIGH);	// Motor A and B are set High permenently
  digitalWrite(PWMB,HIGH); 
  
  int claps = Number_of_Claps();	//number of claps user makes

  //if claps is zero then void loop will restart and go back to the Number_of_Claps()
  if(claps > 0){
    
    //functi will decide want the number of claps will do
    How_Should_I_Move(claps);
    
    int run_time = 10000;	//10 seconds are allowed to run each commands from how i should clap
    long int timer = millis(); //intililizes timer
    long int prev_timer = timer;	//will take value of timer
    
    //while loop is to check distance sensor if value is less than 20 cm robot will stop
    while((timer - prev_timer) < run_time ){
      timer = millis();	//timer is recalled again
      if(claps != 2){//when robot is moving backward distance sensor in the front will not be used so wont stop even if object is in the way
      	int closedistance = Close_enough(); //calls Close_enough function
      	//if close_enough distance is less than 20 while loop will break
        if (closedistance < too_close){
          break;
     	}	
      }
    }
    Stop(100);	//once 10 secs are up robot will stop //we use the int 100 because we know if it leaves the while loop after 10 secs then an object was not that close
    delay(lcd_allow - 1000);//allowing user to look at lcd for stop warning for 1 secs less than the usual
    }
  }

//OGOCHUKWU AMAEZE(22363533) FUCTION
int Number_of_Claps(){
  digitalWrite(redpin,HIGH); //to indicate to user that they can clap 
  digitalWrite(greenpin,LOW);
  
  int clap_registered = 0; //claps number recorded
  const int Threshold = 650; //sound sensor value threshold for a clap
  const int xcoord_time = 0; //value is for the dB_Time_Array postion 1
  const int ycoord_dBlvl = 1; //value is for the dB_Time_Array postion 2
  const int Maximum_Intensity = 1280; //sound sensor value max for a clap
  const int Cooldown = 10; //the amount of time need for each peak to relax
  const int TimeForCommand = 5050; //User is given 5 secs to input claps
  static int dB_Time_appender = 0; //appender for dB_Time_Array value is remebered after function is finished
  
  //setting up timer for recording
 unsigned long int RecordingTime = millis(); //setting up timer for clap input time
 unsigned long int prevRecordingTime = RecordingTime; //recording recordingTime value from before

   //setting up the LCD to tell user to give a command
   char notice[14] = "GIVE ME A NEW";
    LCD_display(0,0,true,notice,0); //lcd will clear and char will be printed
   char notice_prt2[] = "COMMAND";
    LCD_display(1,1,false,notice_prt2,0); //lcd will not clear and char will be printed
  
  //while loop will break once 5secs are up
  while(RecordingTime - prevRecordingTime <= TimeForCommand){
     unsigned long int ExternalResponse = analogRead(SoundSensor); //reading sound sensor
    
    //if sound sensor value is not over 650 and less than 850
    if(ExternalResponse >= Threshold && ExternalResponse <= Maximum_Intensity){//check if the sound sensor recording 
      // informing the user that their clap is registered (test)
      char value_rec[] = "clap recorded:";
      LCD_display(0,0,true,value_rec,0); //will clear lcd and print a char
      
     
      pDecibels = &ExternalResponse; //setting up pointer to point at address of the value recorded from sound sensor
      pSysTime = &RecordingTime; //setting up pointer to point at address of time at which the value from sound sensor was recorded
     	
      //dealing with the graphing of the sound sensor value
      struct GRAPH Graph_values; //setting up variable for the GRAPH structure
      
      Graph_values.System_Time = *pSysTime; //stuct variable x value is the pSysTime pointer is pointing at
      Graph_values.Db_lvl = *pDecibels; //stuct variable y value is the pDecibels pointer is pointing at
      
      //appending values that were recorded to array
      dB_Time_Array[dB_Time_appender][xcoord_time] = Graph_values.System_Time;
      dB_Time_Array[dB_Time_appender][ycoord_dBlvl] = Graph_values.Db_lvl;
      dB_Time_appender+=1; //once value is recorded value being recorded next will move to next pocket in the array
      
      //if array is full the pointer will restart to the first position
      if(dB_Time_appender == 149){
        dB_Time_appender = 0;
      }
 
      //This is to display graphing values on lcd to user
      int spacing = log10(Graph_values.System_Time)+1; //this will allow for the postion of the "," to line up with the time printed
      
      //for some reason if LCD_display function is used to print the 
      //Graph_values.System_Time and Graph_values.Db_lvl a bug will occur :(
      //so it must be done the long way
      lcd.setCursor(0,1);
      lcd.print(Graph_values.System_Time);//lcd wont clear and int will be printed 
      
      LCD_display(spacing,1,false,",",0); //lcd wont clear and char will print
      
      lcd.setCursor(spacing+1,1);
      lcd.print(Graph_values.Db_lvl);//lcd wont clear and int will be printed
      //end of printing values the long way
      
      //will print values in the serial monitor to plot on Matlab
      Serial_Plot(Graph_values.System_Time, Graph_values.Db_lvl);
      
      clap_registered+=1; //once clap is register the clap will be added
      delay(Cooldown);	//cooldown period for each sound sensor peak
    }
    //Calling timer again to check if while-loop statement is still true
    RecordingTime = millis();
  }
  //once while loop is finished the green light will indicate to the user that command will be fulfilled
  digitalWrite(redpin,LOW);
  digitalWrite(greenpin,HIGH);
  
  //will display to user the amount of claps the do
  char claps[] = "clap number: ";
  LCD_display(0,0,true,claps,0);//will clear lcd and print char 
  LCD_display(12,0,false,"0",clap_registered); //will not clear lcd and print an int
  delay(lcd_allow); //gives the user time to look at lcd display
  
  
  return clap_registered; //function will return the number of claps a player makes
}
//Mikolaj()
//function turn clap number into a command
void How_Should_I_Move(int num_clap){
  if(num_clap == 1){
    Forwards(); //1 clap will make the robot go forward
  }else if(num_clap == 2){
    Backwards(); //2 claps will make the robot go backwards
  }else if(num_clap == 3){
    Right(); //3 claps will make the robot go right
  }else if(num_clap>3){
    Left(); //the number of claps will make the robot go left
  }
}

//Mikolaj()
void Left(){ //will make the robot go left
  char left[] = "I TURNED LEFT";
  LCD_display(2,0,true,left,0); //will clear the lcd and print a char
 digitalWrite(AIN1,HIGH);
 digitalWrite(AIN2,LOW);
 digitalWrite(BIN1,LOW);
 digitalWrite(BIN2,HIGH);
 delay(rightleft_turn);
 Forwards(); 
}

//Mikolaj()
void Right(){ //will make the robot go right
  char right[] = "I TURNED RIGHT"; //will clear the lcd and print a char
  LCD_display(1,0,true,right,0);
 digitalWrite(AIN1,LOW);
 digitalWrite(AIN2,HIGH);
 digitalWrite(BIN1,HIGH);
 digitalWrite(BIN2,LOW);
 delay(rightleft_turn);
 Forwards();
 
}
// Boshra(22309956)
void Forwards(){     // this will make the robot to move forward
  char forward[] = "I MOVED FORWARD";// will clear the lcd and prints the char
  LCD_display(0,0,true,forward,0);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  
}
// Boshra(22309956)
void Backwards(){    // This makes the robot move backward
  char backwards[] = "I MOVED BACKWARD";//clears the lcd and prints the char
  LCD_display(0,0,true,backwards,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,HIGH);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);

}
//Boshra(22309956)
void Stop(int distanced_stopped){    // This makes the robot stops when...
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
  
  delay(rightleft_turn - 1000);//a delay for thr previous lcd display
  char stop[] = "I STOPPED";//clears the lcd and prints the char
  LCD_display(3,0,true,stop,0);
  
  if(distanced_stopped < too_close){
    delay(lcd_allow - 1000);//delay for user to look at the last lcd display
    char stopdistance1[] = "distance from";
    LCD_display(0,0,true,stopdistance1,0);//clears lcd and print a char
    char stopdistance2[] = "object:";
    LCD_display(0,1,false,stopdistance2,0); //doesn't clear lcd and prints a char
    LCD_display(8,1,false,"0",distanced_stopped);//doesnt clear lcd and print an int
    LCD_display(9,1,false,"cm",0);//doesn't claer lcd and prints a char
    delay(lcd_allow); 
  }
}

//Mikolaj()
//function used to operate LCD given position, row, clear or not
//if word to be printed then int value set to 0 if int value not set int number to 0 then print int value.
void LCD_display(int space_postion, int rows,bool clear, char word[], int number)
{
  if(clear == true){//clear the lcd and reset cursor
   lcd.clear(); 
  }
  if (number != 0){//print an int
    lcd.setCursor(space_postion, rows);//set cursor at wanted space and position
    lcd.print(number,DEC);
  }
  else {//prints a char
    lcd.setCursor(space_postion, rows);
    lcd.print(word);
  }
}
// Boshra(22309956)
 // checks the distance of the object
// if the object is close gives warning through the LCD 
// and it stops if it is really  close
 int Close_enough(){
  int closes = getDistance(); // makes a vaiable be equal to
                              //the distance function
    
   while(closes == 0){  //if the distance is zero continue
    closes = getDistance(); 
   }
   
   int new_closes = closes; // makes a new variable for the 
                            //new distance
   if(new_closes <too_close){ //checks if the distance is close
     //prints warning onto the LCD
     char warning[]="WARNING AN"; 
     LCD_display(2,0,true,warning,0);
     char warning2[]="OBJECT AHEAD";
     LCD_display(1,1,false,warning2,0);
     Stop(new_closes);
   }
   return new_closes;
}
// Boshra(22309956)
void Serial_Plot(unsigned long int x, unsigned long int y){  // function to print the x and y values for matlab to plot the results
  Serial.println(x);
  Serial.println(y);
}
// Boshra(22309956)
//GET DISTANCE FUNCTION
//RETURNS THE DISTANCE MEASURED BY THE  DISTANCE SENSOR
int getDistance()
{
  float echoTime;                   //variable to store the time it takes for a ping to bounce off an object
  float calculatedDistance;         //variable to store the distance calculated from the echo time

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigpin, LOW);

  echoTime = pulseIn(echopin, HIGH);      //use the pulsein command to see how long it takes for the                                     //pulse to bounce back to the sensor
  calculatedDistance = (echoTime/29)/2;  //calculate the distance of the object that reflected the pulse (half the bounce time multiplied by the speed of sound) in centimeters 
  
  return calculatedDistance;              //send back the distance that was calculated
}