

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);


int prevFreqByte = 0;
volatile int freqByte;
int freqByte1;
int freqByte2;
volatile int settings[] = {0,0,0,894,0};
volatile byte settingLevel = 0;
volatile byte increment = 1;
byte dot = B10100101;
byte prevVol = 0;
byte upLimit = 0;
byte downLimit = 0;
unsigned long startMillis;
char BTInfo[] = "Bluetooth-nimi: SANWU Audio   K*ynnist* laite uudelleen tehd*ksesi uuden laiteparin.                ";
int maxVals[] = {1,5,2,1080,99};
int minVals[] = {0,0,0,875,0};
byte rollOver[] = {1,1,1,0,0};
int prevSetting = 0;
unsigned long backlightTimer = 0;
byte lcdState = 1;
int prevFreq = 0;
byte trash;
int receByte;
int nextReceByte;
//int batteryVal;
//int supplyVal;
//int supplyConnected;
unsigned long lastUpdate = 0;
bool timeOutLoop = false;


const int button = 10;
const int encoderA = 2;
const int encoderB = 9;
const int incr = 7;
const int upDown = 6;
const int cs = 8;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();

  pinMode(encoderB,INPUT_PULLUP);
  pinMode(encoderA,INPUT_PULLUP);
  pinMode(button,INPUT_PULLUP);
  pinMode(incr,OUTPUT);
  pinMode(upDown,OUTPUT);
  pinMode(cs,OUTPUT);
  digitalWrite(cs,LOW);
  digitalWrite(upDown,LOW);
  digitalWrite(incr,LOW);

    digitalWrite(upDown,LOW);
   delayMicroseconds(50); 
  for(int x = 0; x != 100; x++){
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);      
  }
  
  attachInterrupt(digitalPinToInterrupt(encoderA),aFall,FALLING);

 menu();
}

void loop() {
 
}

void aFall(){
  if(digitalRead(encoderB)){
    settings[settingLevel] += increment;
  }
  else settings[settingLevel] -= increment;
  while(!(digitalRead(encoderA))){}
}

void buttonCheck(){
do{
  /*
if(lastUpdate+1000 < millis()){
batteryVal = 0;
supplyVal = 0;
supplyConnected = 0;
  
for(int x = 0; x != 10; x++){
  batteryVal += analogRead(A2);
delay(5);
supplyVal += analogRead(A1);
delay(5);
supplyConnected += analogRead(A3);
delay(5);
}
batteryVal = batteryVal/10;
supplyVal = supplyVal/10;
supplyConnected = supplyConnected/10;
lastUpdate = millis();
}*/


  if(settings[settingLevel] != prevSetting){
    backlightTimer = millis();
    prevSetting = settings[settingLevel];
    if(lcdState != 1){
    lcd.display();
    lcd.setBacklight(HIGH);   
    lcdState = 1;
    timeOutLoop = false;
    switch(settingLevel){
      case 0:
       menu();
      case 1:
       FMradio();
      case 2:
       BTspeaker();
      case 3:
       setFreq();
      case 4:
       setVolume();
       }
    }
  }
  if(backlightTimer+40000 < millis()){
    timeOutLoop = true;
      /*if(supplyConnected > 100 && supplyVal < 100){
        if(batteryVal >= 790 && lcdState != 2){
          lcd.clear();
           lcd.display();
           lcd.setBacklight(HIGH);  
          lcd.print("Lataus valmis.");
          lcd.noCursor();
          lcdState = 2; 
        }
        else if(batteryVal < 790 && lcdState != 3){
          lcd.clear();
          lcd.display();
          lcd.setBacklight(HIGH);  
          lcd.print("Ladataan...");
          lcd.noCursor();
          lcdState = 3; 
        }
      }
      else*/ if(lcdState != 0){
      lcd.setBacklight(LOW);
      lcd.noDisplay();
      lcdState = 0;   
      }
    

  }
  if(!digitalRead(button)){
    while(!digitalRead(button)){maxMinCheck();}
    buttonPressed(settingLevel,settings[settingLevel]); 
  }
}while(timeOutLoop); 
}

void buttonPressed(byte level,int setting){
  switch(level){
    case 0:{
     switch(setting){
      case 0:{
      increment = 1;
      settingLevel = 1;
       FMradio();
      }
       break;
      case 1:{
      increment = 1;
      settingLevel = 2;
       BTspeaker();
      }
       break;
     }
    }
   case 1:{
    switch(setting){
      case 0:{
        searchDown();
      }
       break;
      case 1:{
       increment = 10;
       settingLevel = 3;
       setFreq();
      }
       break;
      case 2:{
       increment = 1;
       settingLevel = 3;
       setFreq();
      }
       break;

       case 3:{
        searchUp();
       }
       break;

       case 4:{
       increment = 1;
       settingLevel = 4;
       setVolume();
       }
       break;

       case 5:{
       increment = 1;
       settingLevel = 0;
       Wire.beginTransmission(0x60);
       Wire.write(B10101001);
       Wire.write(B11010101);
       Wire.write(0x1A);
       Wire.write(0x54);
       Wire.write(0x00);
       Wire.endTransmission();  
       menu();
       }
       break;
    }
   }
  case 2:{
    switch(setting){
      case 0:{
        BTspeaker();
      }
      case 1:{
       increment = 1;
       settingLevel = 0;
       menu();  
      }
      case 2:{
       increment = 1;
       settingLevel = 0;
       menu();         
      }
      
    }

       
  }
  break; 
  case 3:{
   increment = 1;
   settingLevel = 1;
   FMradio();
  }
   break;
  case 4:{
    digitalWrite(upDown,LOW);
   delayMicroseconds(50); 
  for(int x = 0; x != 100; x++){
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);      
  }
   digitalWrite(upDown,HIGH);
   delayMicroseconds(50);
   for(int x = 0; x != settings[4]; x++){
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);      
  }
   increment = 1;
   settingLevel = 1;
   digitalWrite(cs,HIGH);
   FMradio();
  }
   break;
    case 5:{
  increment = 1;
   settingLevel = 1;
   FMradio();
  }
  break; 
  }

}

void menu(){
  while(true){
    delay(100);
    maxMinCheck();
  switch(settings[0]){
    case 0:{
     lcd.clear();
     lcd.print("FM-radio       ");
     lcd.write(dot);
     lcd.setCursor(0,1);
     lcd.print("BT-Kaiutin");
     while(settings[0] == 0){buttonCheck();}
    }
     break;

    case 1:{
     lcd.clear();
     lcd.print("BT-Kaiutin     ");
     lcd.write(dot);     
     lcd.setCursor(0,1);
     lcd.print("FM-radio");
     while(settings[0] == 1){buttonCheck();}
    }
     break;
  }
 }
}

void FMradio(){
freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
  
while(true){
  delay(100);
  maxMinCheck();
  switch(settings[1]){
    case 0:{
     lcd.clear();
     lcd.print("< ");
     lcd.print((float)settings[3]/10); 
     if(settings[3] < 1000){
      lcd.print(" > ");
     }
     else lcd.print("> ");
     lcd.print("MHz");   
     lcd.setCursor(0,1);
     lcd.print("Voimakkuus: ");
     lcd.print(settings[4]);
     lcd.setCursor(0,0);
     lcd.cursor();
     while(settings[1] == 0){buttonCheck();}
    }
     break;
     
    case 1:{
     lcd.noCursor();
     lcd.clear();
     lcd.print("< ");
     lcd.print((float)settings[3]/10); 
     if(settings[3] < 1000){
      lcd.print(" > ");
     }
     else lcd.print("> ");
     lcd.print("MHz");   
     lcd.setCursor(0,1);
     lcd.print("Voimakkuus: ");
     lcd.print(settings[4]);
     if(settings[3] < 1000){
      lcd.setCursor(3,0);
     }
     else lcd.setCursor(4,0);
     lcd.cursor();
     while(settings[1] == 1){buttonCheck();}
    }
     break;
     
    case 2:{
     lcd.noCursor();
     lcd.clear();
     lcd.print("< ");
     lcd.print((float)settings[3]/10); 
     if(settings[3] < 1000){
      lcd.print(" > ");
     }
     else lcd.print("> ");
     lcd.print("MHz");   
     lcd.setCursor(0,1);
     lcd.print("Voimakkuus: ");
     lcd.print(settings[4]);
     if(settings[3] < 1000){
      lcd.setCursor(5,0);
     }
     else lcd.setCursor(6,0);
     lcd.cursor();
     while(settings[1] == 2){buttonCheck();}
    }
    break;

    case 3:{
     lcd.noCursor();
     lcd.clear();
     lcd.print("< ");
     lcd.print((float)settings[3]/10); 
     if(settings[3] < 1000){
      lcd.print(" > ");
     }
     else lcd.print("> ");
     lcd.print("MHz");   
     lcd.setCursor(0,1);
     lcd.print("Voimakkuus: ");
     lcd.print(settings[4]);
      lcd.setCursor(8,0);
     lcd.cursor();
      while(settings[1] == 3){buttonCheck();}
    }
    break;

    case 4:{
      lcd.noCursor();
      lcd.clear();
      lcd.print("Voimakkuus: ");
      lcd.print(settings[4]);  
      lcd.setCursor(15,0);
      lcd.write(dot);
      lcd.setCursor(0,1);    
      lcd.print("Takaisin");
      while(settings[1] == 4){buttonCheck();}
    }
    break;

    case 5:{
      lcd.noCursor();
      lcd.clear();
      lcd.print("Takaisin");
      lcd.setCursor(15,0);
      lcd.write(dot);
      lcd.setCursor(0,1);
      lcd.print("< ");
      lcd.print((float)settings[3]/10); 
     if(settings[3] < 1000){
      lcd.print(" > ");
     }
     else lcd.print("> ");
      lcd.print("MHz");
      while(settings[1] == 5){buttonCheck();}
    }
    }
 }
}

void BTspeaker(){
while(true){
  delay(100);
  maxMinCheck();
  switch(settings[2]){
    case 0:{
     lcd.clear();
     lcd.print("BT-Kaiutin");
     while(settings[2] == 0){
        upLimit = 0;
        downLimit = 0;
        while(upLimit != sizeof(BTInfo)-1 && settings[2] == 0){
         lcd.setCursor(15-(upLimit-downLimit),1);
         for(byte var = 0; var < 16 && var <= upLimit; var++){
           if(BTInfo[var+downLimit] == '*'){
             lcd.write(B11100001); //ä
           }
           else lcd.print(BTInfo[var+downLimit]);
    
         }
         startMillis = millis();
         while(millis() < startMillis+400 && settings[2] == 0){
          buttonCheck();
         }
         if(upLimit >= 15){
           downLimit++;
         }
         upLimit++;
        }
     }
    }break;
     
    case 1:{
     lcd.clear();
     lcd.setCursor(0,1);
     lcd.print("Takaisin ");
     lcd.setCursor(15,1);
     lcd.write(dot);
     while(settings[2]== 1){
        upLimit = 0;
        downLimit = 0;
        while(upLimit != sizeof(BTInfo)-1&& settings[2] == 1){
         lcd.setCursor(15-(upLimit-downLimit),0);
         for(byte var = 0; var < 16 && var <= upLimit; var++){
           if(BTInfo[var+downLimit] == '*'){
             lcd.write(B11100001); //ä
           }
           else lcd.print(BTInfo[var+downLimit]);
    
         }
         startMillis = millis();
         while(millis() < startMillis+400  && settings[2] == 1){
          buttonCheck();
         }
         if(upLimit >= 15){
           downLimit++;
         }
         upLimit++;
        }      
     }
    }
     break;
    case 2:{
     lcd.clear();
     lcd.print("Takaisin ");
     lcd.setCursor(15,0);
     lcd.write(dot);
     lcd.setCursor(0,1);
     lcd.print("BT-Kaiutin");
     while(settings[2] == 2){buttonCheck();}
    }
     break;
     }
}
}

void setFreq(){
lcd.noCursor();
lcd.clear();
lcd.print("< ");
lcd.print((float)settings[3]/10); 
if(settings[3] < 1000){
lcd.print(" > ");
}
else lcd.print("> "); 
lcd.print("MHz");  
lcd.setCursor(0,1);
lcd.print("Voimakkuus: ");
lcd.print(settings[4]);

  
while(settingLevel == 3){  
startMillis = millis();
while(millis() < startMillis+300){
  buttonCheck();
}
maxMinCheck(); 
lcd.setCursor(2,0);
lcd.print((float)settings[3]/10);
if(settings[3] < 1000){
  lcd.setCursor(7,0);
lcd.print(" ");      
}
if(increment == 10){
if(settings[3] < 1000){
  lcd.setCursor(3,0);
}
else lcd.setCursor(4,0);
}
else if(settings[3] < 1000){
  lcd.setCursor(5,0);
}
else lcd.setCursor(6,0);
lcd.cursor();
if(prevFreq != settings[3]){ 
prevFreq = settings[3];
freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
}
buttonCheck();
}
}

void setVolume(){
  lcd.clear();
  lcd.print("Voimakkuus: ");
  lcd.print(settings[4]);
  lcd.setCursor(15,0);
  lcd.write(dot);
  lcd.setCursor(0,1);
  lcd.print("Takaisin ");
  digitalWrite(cs,LOW);
  digitalWrite(upDown,LOW);
delayMicroseconds(50); 



while(settingLevel == 4){  
   maxMinCheck(); 
  while(settings[4] != prevVol){
    delay(1);
  maxMinCheck();  
  if(settings[4] < prevVol){
    digitalWrite(upDown,LOW);
    delayMicroseconds(50);
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);
    prevVol--;
  }
    if(settings[4] > prevVol){
    digitalWrite(upDown,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);
    prevVol++;
  }
  if(settings[4] == 0){
  digitalWrite(upDown,LOW);
   delayMicroseconds(50); 
  for(int x = 0; x != 100; x++){
    digitalWrite(incr,HIGH);
    delayMicroseconds(50);
    digitalWrite(incr,LOW);
    delayMicroseconds(50);      
  }
  prevVol = 0;
  }
}
startMillis = millis();
while(millis() < startMillis+300){
  buttonCheck();
}
  maxMinCheck(); 
  lcd.setCursor(12,0);
  lcd.print(settings[4]);
  if(settings[4] < 10){
    lcd.setCursor(13,0);
    lcd.print("  ");
  }
  }
}

void searchUp(){
lcd.noCursor();
receByte =0;
settings[3] += 1;
while(receByte <= 160 && settings[3] < maxVals[3]){
 settings[3] += 1;  
freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
lcd.setCursor(2,0);
lcd.print((float)settings[3]/10);
delay(100);
 Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 receByte = Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 delay(50);
  Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 receByte += Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 receByte = receByte/2;
}

nextReceByte = 255;
while(receByte < nextReceByte && settings[3] < maxVals[3]){
  settings[3] += 1;
  freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
delay(100);
 Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 nextReceByte = Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 delay(50);
  Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 nextReceByte += Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 nextReceByte = nextReceByte/2;
}
settings[3] -= 1;
FMradio();
}

void searchDown(){
  lcd.noCursor();
receByte =0;
settings[3] -= 1; 
while(receByte <= 160 && settings[3] > minVals[3]){
 settings[3] -= 1;  
freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
lcd.setCursor(2,0);
lcd.print((float)settings[3]/10);
delay(100);
 Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 receByte = Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 delay(50);
  Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 receByte += Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 receByte = receByte/2;
}
nextReceByte = 255;
while(receByte < nextReceByte && settings[3] > minVals[3]){
  settings[3] -= 1;
  freqByte = (4*(100000*settings[3]+225000))/32768  ;
freqByte1 = freqByte >> 8;
freqByte2 = freqByte & 255;
Wire.beginTransmission(0x60);
Wire.write(freqByte1);
Wire.write(freqByte2);
Wire.write(0x72);
Wire.write(0x1E);
Wire.write(0x00);
Wire.endTransmission();
delay(100);
 Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 nextReceByte = Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 delay(50);
  Wire.requestFrom(0x60,5);
 trash = Wire.read();
 trash = Wire.read();
 trash = Wire.read();
 nextReceByte += Wire.read();
 while(Wire.available()){
  trash = Wire.read(); 
 }
 nextReceByte = nextReceByte/2;
}
settings[3] += 1;
FMradio();
}

void maxMinCheck(){
  if(settings[settingLevel] > maxVals[settingLevel]){
    if(rollOver[settingLevel]){
      settings[settingLevel] = minVals[settingLevel];
    }
    else settings[settingLevel] = maxVals[settingLevel];
  }
  if(settings[settingLevel] < minVals[settingLevel]){
    if(rollOver[settingLevel]){
      settings[settingLevel] = maxVals[settingLevel];
    }
    else settings[settingLevel] = minVals[settingLevel];
  }  
}




