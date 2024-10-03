//Code for an automated cornsnake tank control
//
//This code controls the heat lamp and the misting system. Temperature is read in Celsius.
//The Lamp and Mist pins are connected to a 250V relay module, so they can control mains power.
//The tempearture and humidity are read by 2 DHT sensors, one at the heat spot, another at the coldest spot.
//The temperature readings are displayed on an LCD screen.
//This code is designed to run 24 hours a day without a real time clock module. Because no real time clock module is used the arduino has to be plugged in
//at the beginning of the day. I want the day to start at 8:00am, so that's when I started the arduino.

//The DHT sensors are connected to pins 7 and 6, the relay control for the lamp is connected to pin 8 and the relay control for the misting system is connected
//to pin 9.

//load necessary libraries
#include <DHT22.h>;
#include "LiquidCrystal.h";
#include <Arduino.h>;

//Define the pins
#define DHTPIN1 7//One DHT sensor is connected to pin 7, the other to pin 6
#define DHTPIN2 6
#define DHTTYPE DHT22
#define Lamp 8
#define Mist 9 //The relay pins are connected to pin 8 (lamp) and 9 (misting system)
#define Power1 10 
#define Power2 13 //The DHT sensors are run off the arduino pins 10 & 13, so the power for the sensors can be restarted, if an error occurs. 
//This wiring is done, because I experienced some errors of the sensors after longer operation times and this way they can be restarted automatically,
//when this happens.

DHT22 dht1 (DHTPIN1);
DHT22 dht2 (DHTPIN2);
int counter = 0;//This sets up a loop counter. I use this counter as a proxy for a real clock.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);//The LCD panel is connected to pins 12,11,5,4,3,2.

void setup() {
  Serial.begin(115200);
  pinMode (8, OUTPUT);
  pinMode (9, OUTPUT);
  pinMode (10, OUTPUT);
  pinMode (13,OUTPUT);
  lcd.begin(16, 2);
  delay (2000);
}

long last_time = 0;//This sets up a time measurement, to see how long each loop takes.
long cont = 0; //used to avoid over-misting
long cont2=0; //used to avoid excessive light switching

void loop() {
  long now = millis ();//Another part of the loop time measurement.
  digitalWrite (Power1, HIGH);
  digitalWrite (Power2, HIGH);
  delay (4000);
  float hum1 = dht1.getHumidity(); //hum1 is the humidity reading of the sensor at the heat spot.
  float temp1 = dht1.getTemperature(); //temp1 is the temperature reading (in Celsius) at the heat spot.
  float hum2 = dht2.getHumidity(); //hum2 is the humidity reading at the cold spot.
  float temp2 = dht2.getTemperature(); //temp2 is the temperature reading (in Celsius) at the cold spot.
  if (dht1.getLastError() != dht1.OK){
    temp1= 0.0;
    hum1=0.0;
    lcd.clear();
    digitalWrite (Power1, LOW);
  } 
  if (dht2.getLastError() != dht2.OK){
    temp2= 0.0;
    hum2=0.0;
    lcd.clear();
    digitalWrite (Power2, LOW);
  } 

  Serial.print("H1:"); //The following lines (47-59) will print the current humidity and temperature readings to the Serial Monitor
  Serial.print(hum1); //The readings will be displayed like this: H1: x, T1: x, H2: x, T2:x. This will also allow you to monitor the
  Serial.print (','); //temperature and humidity readings in the Serial Plotter.
  Serial.print("T1:");
  Serial.print(temp1);
  Serial.print (',');
  Serial.print("H2:");
  Serial.print(hum2);
  Serial.print (',');
  Serial.print("T2:");
  Serial.print(temp2);
  Serial.print (',');
  Serial.println();
  if (cont>0){
    cont= cont-1;
  }
  if (cont2>0){
  cont2= cont2-1;
  }
  lcd.setCursor (0,0); //The following lines (60-69) print the current readings to the LCD screen. The readings will be displayed like this: 
  lcd.print (temp1,1); // temp1 C hum1 %  H
  lcd.print ("C   ");  // temp2 C hum1 %  C
  lcd.print (hum1,1);
  lcd.print ("%  H");
  lcd.setCursor(0, 1);
  lcd.print(temp2 ,1);
  lcd.print("C   ");
  lcd.print(hum2 ,1);
  lcd.print("%  C");
  counter = counter +1; //The loop counter.
  Serial.print(counter); //Print the current counter value.
  Serial.println("control");
  Serial.print(cont);
  Serial.println();
  Serial.print(cont2);
  Serial.println();
  //The temperature settings are defined here. Daytime is set to 13 hours, in that time about 3107 loops are done.
  //The temperature is set to minimum 20°C at the cold spot and minimum 25°C (the sensor is not directly under the lamp) at the heat spot during the day.
  //The minimum temperature at the heat spot at all times (also at night) is set to 18°C.
  //When the temperature falls below these values the heat lamp will be turned on.
  if(cont2==0){
    if ((temp1 > 25)&& (temp2 > 19)&&(counter < 3107)){digitalWrite (Lamp, HIGH); cont2=cont2+60;}
    else if ((temp2 > 18)&&(counter >=3107)){digitalWrite (Lamp, HIGH);cont2=cont2+60; }
    else {digitalWrite (Lamp, LOW);}
  }
  else{digitalWrite (Lamp, HIGH);}

  //The minimum humidity is set to 60%. If it drops below this the misting system is turned on.
  //hum1 is placed at the dryer end, hum2 at the humid end

  if ((hum1 < 55) && (hum1>0) && (hum2 < 65) && (cont == 0)){
     digitalWrite (Mist, LOW);
    cont = cont+16;}//This starts the control, so the misting system will wait 16 cycles (~2 minutes) to spray again.
      else {digitalWrite (Mist, HIGH);}
  //When ~24 hours (5736 loops) have passed the loop counter is set to 0, so the day (for the arduino) begins again.
  if (counter == 5736){
    counter = 0;
  }
  
  delay (11038); //This is important, so the misting system stays on for a few seconds and the humidity is not kept constant at ~60%.
  Serial.print("Time elapsed:");
  Serial.println(now-last_time);//These lines will print the loop time to the Serial Monitor. I used these values to define 13 hours for day time.
  //One loop takes 15064 milliseconds on my arduino. I took that value and calculated that ~3107 loops will be performed in 13 hours and ~5736 loops will be 
  //performed in 24 hours. These values can be found in the definitions for the daytime and for the duration of a day.
  last_time=now;
}