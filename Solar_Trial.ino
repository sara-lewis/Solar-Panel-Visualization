#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>

#include <LiquidCrystal.h>

// Variable definition 
int analogPin = 1; 
int returnedVal; 
double Realvoltage; 
double addedVals; 
double aveVolts; 
unsigned long timeRan = millis();
unsigned long lastCheck = 0; 
unsigned long iterations = 0; 
int seconds = 5; 
int voltsOverTime[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
unsigned long lastCheckDisplay = 0; 
int secondsDisplay = 2; 
int indexDisplay = 0; 
float numLights; 
int roundedLights; 

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8,8,8,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + 
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE, 
  NEO_GRB + NEO_KHZ800); 

// Setup function that tells the arduino how often to collect data 
void setup() {
  Serial.begin(9600); 
  lcd.begin(16,2); 
  matrix.setBrightness(0);  
  matrix.begin();
}

// Loop function that runs over and over again 
void loop() { 
  checkVolts(); 
}

// Function that gets voltage based on the pin number and averages the voltages of the past 10 seconds 
void checkVolts(){
  if((timeRan-lastCheck)/1000<seconds){
    getVolts(analogPin); 
  } else {
    calculateVolts(); 
    addToList(); 
    resetVars(); 
  }
  displayValue(); 
}

// Function that resets all the counting variables to zero after the whole process is completed once 
void resetVars(){
  returnedVal = 0; 
  Realvoltage = 0; 
  addedVals = 0; 
  aveVolts = 0; 
  iterations = 0; 
}

// Function that gets the voltage from the analog pin and incriments important variables 
void getVolts(int pinNumber){
  returnedVal = analogRead(pinNumber); 
  addedVals = addedVals + returnedVal; 
  iterations = iterations + 1; 
  timeRan = millis(); 
}

// Function that calculates voltage after time requirements are met 
void calculateVolts(){
     lastCheck = timeRan;  
     aveVolts = addedVals/iterations; 
     Realvoltage = (5*aveVolts)/1024; 
}

// Function that prints current voltage to LCD screen 
void displayValue(){
  lcd.setCursor(0,0); 
  if((timeRan-lastCheckDisplay)/1000==secondsDisplay){
    printToLCD(); 
    showLights(); 
    if(indexDisplay==23){
      indexDisplay = 0; 
    }else{
      indexDisplay = indexDisplay + 1; 
    }
    lastCheckDisplay = timeRan; 
  }
}

void printToLCD(){
    if(indexDisplay<10){
      lcd.print("Display  ");
    }else{
      lcd.print("Display ");
    }
    lcd.print(indexDisplay); 
    lcd.setCursor(0,1); 
    lcd.print("Voltage: "); 
    double printableVolts = (5*voltsOverTime[indexDisplay])/1024;
    lcd.print(printableVolts); 
    lcd.print(" V");
}

void showLights(){
  numLights = (64*voltsOverTime[indexDisplay])/715; 
  roundedLights = (int) numLights; 
  int rows = 0; 
  for(int i=0; i<8; i++){
    if((roundedLights-8)>0){
      roundedLights = roundedLights-8; 
      rows = rows + 1; 
    }
  }
  for(int y=0; y<rows; y++){
    for(int x=0; x<8; x++){
      matrix.drawPixel(x,y,matrix.Color(0,0,255)); 
      Serial.print(x); 
      Serial.print(y); 
      Serial.print(", "); 
    }
  }
  Serial.println(""); 
  for(int x=0; x<roundedLights; x++){
    matrix.drawPixel(x,rows,matrix.Color(0,255,255)); 
    Serial.print(x); 
    Serial.print(rows); 
    Serial.print(", "); 
  }
  matrix.show(); 
}

// Adds average voltage from the last incriment to an array that holds 24 values 
void addToList(){
  for(int i=1; i<24; i++){
    voltsOverTime[i-1] = voltsOverTime[i]; 
  }
  voltsOverTime[23] = aveVolts; 
  Serial.println(""); 
  for(int i=0; i<24; i++){
    //Serial.print(voltsOverTime[i]); 
    //Serial.print(","); 
  }
}
