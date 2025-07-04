//LCD
#include <LiquidCrystal.h>
const int rs = 2, e = 3,  db4 = 4, db5 = 5, db6 = 6, db7 = 7;//can connect db0-3 for faster connection if needed
LiquidCrystal lcd(rs, e, db4, db5, db6, db7);
const int lcdColumns = 16;
const int lcdRows = 2;
//Max/Min
//First value is max, second is min.
//Set max to low and min to high values, so that first check will definatly update values.
int maxMinTemp[2]={-999,999};
int maxMinMoist[2]={-999,999};
int maxMinLight[2]={-999,999};
//Serial1 comunication
const byte tx = 14;
const byte rx = 13;
byte infReq =4; //For storing which data will be requested.
byte liveTemp=0;  //For storing values received over Serial1.
byte liveLight=0;
byte liveMoist=0;
//efficiency testing
int t1,t2; //For storing time in ms, to calculating run times.
//keypad
#include <Keypad.h>
const byte ROWS=3;
const byte COLS=3;
char hexaKeys [ROWS] [COLS]={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
};
byte rowPins[ROWS] = {9,10,11}; 
byte columnPins[COLS] = {A4,A5,A6};
byte keyNum;   //For storing which keypad number is pressed.
byte menu = 0; //For storing page of menu that user is on.
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, columnPins, ROWS, COLS);


void setup() {
 //LCD
 lcd.begin(lcdColumns,lcdRows);
 //tx/rx
 pinMode(tx, OUTPUT);
 pinMode(rx, INPUT);
 Serial.begin(9600);
 Serial1.begin(9600);
 //max/min
 pinMode(A1,INPUT);
 attachInterrupt(digitalPinToInterrupt(A1), maxMinReset, HIGH); //Resets max and min values when button is pressed.
}

void loop() {
  t1 =millis();//Efficiency testing, runtime start.

  Serial.print("Information request =");
  Serial.println(infReq);
  byte msg[2]={0,0};  //first byte is data ID, second is value

  Serial1.write(infReq);    //Send information request to other board
  delay(300);
  
  if(Serial1.available() > 1){ //Checks if ID and value bytes have both been recieved.
    delay(50);
    Serial1.readBytes(msg,2); //read ID and Value bytes.

    //Temperature
    if(msg[0] == 4){      //If ID corresponds to temperature
      liveTemp = msg[1];  //Store transmitted value as temperature. 
      Serial.print("LiveTemp=");
      Serial.println(liveTemp);
      if (liveTemp > maxMinTemp[0]){ //If new value > max
        maxMinTemp[0] = liveTemp; //Update max temperature.
      } 
      if (liveTemp < maxMinTemp[1]){ //If new value < min
        maxMinTemp[1] = liveTemp; //Update min temperature.
      }
      infReq =5;  //Request moisture
    }

    //Moisture
    else if(msg[0]==5){   //If ID corresponds to moisture
      liveMoist = msg[1]; //Store transmitted value as moisture. 
      Serial.print("LiveMoisture=");
      Serial.println(liveMoist);
      if (liveMoist > maxMinMoist[0]){ //If new value > max
        maxMinMoist[0] = liveMoist; //Update max moisture.
      } 
      if (liveMoist < maxMinMoist[1]){ //If new value < min
        maxMinMoist[1] = liveMoist; //Update min moisture.
      }
      infReq =6; //Request light
    }

    //Light
    else if(msg[0]==6){   //If ID corresponds to light
      liveLight = msg[1]; //Store transmitted value as light.
      Serial.print("Live light=");
      Serial.println(liveLight);
      if (liveLight > maxMinLight[0]){ //If new value > max
        maxMinLight[0] = liveLight; //Update max light.
      } 
      if (liveLight < maxMinLight[1]){ //If new value < min
        maxMinLight[1] = liveLight; //Update min light.
      }
      infReq =4; //Request temperature
    }      
  }

  //Display Menus on LCD
  //Numbered options can be selected using keypad.
  switch(menu){
    case 1://Temp
    lcd.clear();
    lcd.print("1:Live");
    lcd.setCursor(0,1);
    lcd.print("2:Max/Min");
    break;

    case 2://Light
    lcd.clear();
    lcd.print("1:Live");
    lcd.setCursor(0,1);
    lcd.print("2:Max/Min");
    break;

    case 3://Moisture
    lcd.clear();
    lcd.print("1:Live");
    lcd.setCursor(0,1);
    lcd.print("2:Max/Min");
    break;

    case 4://Live temperature
    lcd.clear();
    lcd.print("Temperature is:");
    lcd.setCursor(0,1);
    lcd.print(liveTemp);
    lcd.print((char)223);//degree symbol
    lcd.print("C");
    break;

    case 5://Live light
    lcd.clear();
    lcd.print("Light is:");
    lcd.setCursor(0,1);
    lcd.print(liveLight);
    lcd.print("%");
    break;

    case 6://Live moisture
    lcd.clear();
    lcd.print("Moisture is:");
    lcd.setCursor(0,1);
    lcd.print(liveMoist);
    lcd.print("%");
    break;

    case 7://Max/min temperature
    lcd.clear();
    lcd.print("Max Temp: ");
    lcd.print(maxMinTemp[0]);
    lcd.print((char)223);//degree symbol
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Min Temp: ");
    lcd.print(maxMinTemp[1]);
    lcd.print((char)223);//degree symbol
    lcd.print("C");
    break;

    case 8://Max/min light
    lcd.clear();
    lcd.print("Max Light: ");
    lcd.print(maxMinLight[0]);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("Min Light: ");
    lcd.print(maxMinLight[1]);
    lcd.print("%");
    break;

    case 9://Max/min moisture
    lcd.clear();
    lcd.print("Max Moist: ");
    lcd.print(maxMinMoist[0]);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("Min Moist: ");
    lcd.print(maxMinMoist[1]);
    lcd.print("%");
    break;
    
    default://Main
    lcd.clear();
    lcd.print("1:Temp   2:Light");
    lcd.setCursor(0,1);
    lcd.print("3:Moisture");
    menu=0; //To insure user input directs to the correct menu
  }

  //Keypad
  keyNum =0;
  char key = customKeypad.getKey(); 
  if(key != NO_KEY){   //Check if key is pressed.
    keyNum = key-48; //ASCII to number convertion. (0=48,1=49 etc.)
  }

  //Menu update
  if (menu != 0){keyNum = keyNum*3;}//multiply by number of options to avoid ambiguity, e.g. 3+1=4 and 2+2=4.
  menu += keyNum;//Updates which page of the menu will be shown on LCD based on keypad input.

  //Efficiency testing
  t2=millis(); //Runtime end.
  Serial.print(t2-t1);// Calculate runtime.
  Serial.println("ms");
  //Efficiency calculations before final delay(), to record only delays caused during running of loop code.
  delay(500);
}

void maxMinReset(){ //Reset max and min when button is clicked.
  maxMinTemp[0]=-999;
  maxMinTemp[1]=999;
  maxMinMoist[0]=-999;
  maxMinMoist[1]=999;
  maxMinLight[0]=-999;
  maxMinLight[1]=999;
  Serial.println("MAX/MIN RESET");
}
