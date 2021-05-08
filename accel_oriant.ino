//RFD900 EELE 489R
//Acceleration and Oriantation code
//Michael Valentino-Manno
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_LSM303AGR_Mag.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(11111); //assign ID to sensor (must be unique for I2C)
Adafruit_LSM303AGR_Mag_Unified mag = Adafruit_LSM303AGR_Mag_Unified(22222);

int STATE = LOW; //initially LED is off
int count = 0;

const int chipSelect = 53; //to be used for SD card

void setup(void) {
#ifndef ESP8266
  while (!Serial) //(only keep this while if you plan on using the serial monitor)
    ;                          //Nothing happens when serial monitor is closed
#endif
  Serial.begin(9600);
  pinMode(30, OUTPUT); //LED pin 
  Serial.println("Accelerometer/Magnetometer Test");
  Serial.println("");

  if (!accel.begin()) {      //initialization 
    Serial.println("LSM303 not detected");
    while (1)
      ;
  }

  if (!mag.begin()) {
    
    Serial.println("LSM303AGR not detected");
    while (1)
      ;
  }

  accel.setRange(LSM303_RANGE_8G);   //set accel range (2, 4, 8, 16)
  Serial.println("Range set to: +-8G");
  

  accel.setMode(LSM303_MODE_NORMAL);   //set accel mode (normal, low power, high res)
  Serial.println("Mode set to: normal\n");
}

void loop(void) {

  if(count == 4){  //toggle led at different rates than data is measured
    STATE = !STATE; 
    digitalWrite(30, STATE);
    count = 0;
  }
  count++;
  
  sensors_event_t event;
  accel.getEvent(&event); //gets measurement
  
  double a_x = event.acceleration.x; //gets accel
  double a_y = event.acceleration.y;
  double a_z = event.acceleration.z;
  //Serial.println("New measurement\n");
  //Serial.print("X: " + (String)a_x + "  "); //accel in m/s^2
  //Serial.print("Y: " + (String)a_y + "  ");
  //Serial.println("Z: " + (String)a_z + "  m/s^2\n");


  double xg = (a_x)/9.80665; //calculates accel to g's
  double yg = (a_y)/9.80665;
  double zg = (a_z)/9.80665;

  //Serial.print("X: " + (String)xg + "  ");
  //Serial.print("Y: " + (String)yg + "  ");
  //Serial.println("Z: " + (String)zg + "  g's");
  //Serial.println("---------------------\n");


  double pitch = (180/M_PI) * atan2(xg, sqrt(sq(yg) + sq(zg))); //calculate pitch from accel values
  double roll = (180/M_PI) * atan2(yg, sqrt(sq(xg) + sq(zg))); //calculate roll from the accel values


  mag.getEvent(&event); //gets measurement
  delay(10);
  double magx = event.magnetic.x - 26.32;
  double magy = event.magnetic.y + 25.12;
  double magz = event.magnetic.z + 30.45;
  
  //Serial.println("Magnetometer measurements\n");
  //Serial.print("X: " + (String)magx + "  ");
  //Serial.print("Y: " + (String)magy + "  ");
  //Serial.println("Z: " + (String)magz + "  uT");
  //Serial.println("---------------------\n");

  Serial.println("magx = " + (String)magx + " | " + "magy = " + (String)magy  + " | " + "magz = " + (String)magz +"\n");
  Serial.println("sin(pitch) = " + (String)(sin(pitch)) + " | " + "sin(roll) = " + (String)(sin(roll))  + " | " + "cos(pitch) = " + (String)(cos(pitch)) + " | " + "cos(roll) = " + (String)(cos(roll)) +"\n");

  double pitchr = (pitch * M_PI) / 180; 
  double rollr = (roll * M_PI) / 180;

  double mag_norm = sqrt((magx*magx) + (magy*magy) + (magz*magz));
 // magx = magx / mag_norm;
 // magy = magy / mag_norm;
 // magz = magz / mag_norm; 

  //Serial.println("NORMALIZED: magx = " + (String)magx + " | " + "magy = " + (String)magy  + " | " + "magz = " + (String)magz +"\n");
  
  double a = magx * cos(pitchr) + sin(pitchr) * sin(rollr) * magy + magz * cos(rollr) * sin(pitchr);
  double b = magy * cos(rollr) - magz * sin(rollr);
  //double yaw = 180 * atan2(-magy,magx) / M_PI;

  Serial.println("a = " + (String)a + " | " + "b = " + (String)b +"\n");
  
  double yaw = 180 * atan2(-b,a) / M_PI;
  //  double yaw = 180* atan(zg / sqrt(xg * xg + zg * zg)) / M_PI;
  //double yaw = (180 * (atan2(event.magnetic.y - 26.4,event.magnetic.x - .75))) / M_PI;  //compass measurement  
  if(yaw < 0){
    yaw = 360 + yaw;
  }
  
  //Serial.println("Oriantation values\n");
  Serial.print("Pitch: " + (String)pitch + "  ");
  Serial.print("Roll: " + (String)roll + "  ");
  Serial.println("Yaw: " + (String)yaw + "  degrees");
  Serial.println("---------------------\n");

  delay(500);
}
