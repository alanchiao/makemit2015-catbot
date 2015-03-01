#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "/Users/amruthvenkatraman/Downloads/everything.h"


#define MAX_BUF 1
typedef enum {ZEROBYTES, ONEBYTE, TWOBYTES } state;
int fd;
char buf[MAX_BUF];
state currState = ZEROBYTES;
char leftMotorSpeed;
char rightMotorSpeed;
char ALIGNMENT_NUMBER = 255;
Adafruit_DCMotor *leftMotor;
Adafruit_DCMotor *rightMotor;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
int thresh = 1000;
unsigned long timeout = 1000;

boolean isOn = true;
boolean wasAboveThresh = true;
unsigned long flipTime = 0;


void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:  
  //sensorSetup();
  controllerSetup(); 
}

void sensorSetup()
{
  Serial.println("Light Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  Serial.println("");
  
  pinMode(13, OUTPUT);
}

void controllerSetup(){
    
  // put your setup code here, to run once:
  // Open up  a pipe
  sleep(3);
  Serial.print("!!!");
  char * myfifo = "/home/root/makemit2015-catbot/controller";
  /* open, read, and display the message from the FIFO */
  Serial.print("About to read");
  fd = open(myfifo, O_RDONLY);
  Serial.print("before making motor shield object\n");
  Adafruit_MotorShield AFMS = Adafruit_MotorShield();
  leftMotor = AFMS.getMotor(1);
  rightMotor = AFMS.getMotor(3);
  AFMS.begin();
  leftMotor->setSpeed(100);
  rightMotor->setSpeed(100);
  Serial.println("ABOUT TO CALL MOTOR RUN IN SETUP:");
  
  while (true) {
    //sensorLoop();
    controllerLoop();
    delay(2000);
  }
  
  Serial.println("before afms begin");
  Serial.println((int) leftMotor);
}

void loop() {
  //sensorLoop();
  
  delay(1000);
  controllerLoop();
}

void sensorLoop() {
    unsigned long currTime = millis();
  
  if (currTime - flipTime < timeout) {
    return;
  }
  
  int lux = getLux();
  Serial.println(lux);
  if (lux >= thresh) {
    if (!wasAboveThresh) {
      isOn = !isOn;
      flipTime = currTime;
      if (isOn) {
        Serial.println("ON");
        digitalWrite(13, HIGH);
      } else {
        Serial.println("OFF");
        digitalWrite(13, LOW);
      }
      wasAboveThresh = true;
    }
  }
  else {
    wasAboveThresh = false;
  }
}

void controllerLoop() {
  
  // put your main code here, to run repeatedly: 
  // Read from the pipe (3 bytes)
  // We wait until we see a 0 byte
  Serial.println("Hello");
//  Serial.println(currState);
  Serial.print(buf[0]);
  Serial.print("\n");
  Serial.print(ALIGNMENT_NUMBER);
  Serial.print("\n");
  int bytesRead = read(fd, buf, MAX_BUF);
  if (bytesRead != 0) {
   leftMotorSpeed = 150;
    rightMotorSpeed = 150;
   leftMotor -> setSpeed(leftMotorSpeed);
    rightMotor -> setSpeed(rightMotorSpeed);
   leftMotor -> run(FORWARD);
    rightMotor -> run(FORWARD);
   delay(3000);
  leftMotor -> setSpeed(0);
   rightMotor -> setSpeed(0);
    leftMotor -> run(FORWARD);
   rightMotor -> run(FORWARD);
 }
  /*
  int converted = buf[0];
  Serial.println(converted);
  if (bytesRead == 0) {return;}

  if (currState == ZEROBYTES) {
    if (converted == ALIGNMENT_NUMBER) {
      currState = TWOBYTES;
    }
  }
  else if (currState == TWOBYTES) {
    if (converted != ALIGNMENT_NUMBER) {
      leftMotorSpeed = converted;
      currState = ONEBYTE;
    }
    // else it was a new frame so expect 2 more bytes
  }
  else {
    if (converted != ALIGNMENT_NUMBER) {
      rightMotorSpeed = converted;
      
      leftMotor->setSpeed(100);
      rightMotor->setSpeed(100);

      Serial.print(leftMotorSpeed);
      Serial.print("\t");
      Serial.print(rightMotorSpeed);
      Serial.print("\n");
      // do motor stuff
      Serial.print("WE SET THE LEFT MOTOR SPEED!\n");
      
      Serial.print("WE SET THE RIGHT MOTOR SPEED!\n");
      currState = ZEROBYTES;
      leftMotor->run(FORWARD);
      rightMotor->run(FORWARD);
    }
    else {
      currState = TWOBYTES;
    }
  } 
  
  /*
  Serial.println((int) leftMotor);
  leftMotor->setSpeed(100);
  Serial.println((int) leftMotor);
  rightMotor->setSpeed(100);
  Serial.println((int) leftMotor);
  leftMotor->run(FORWARD);
  Serial.println((int) leftMotor);
  rightMotor->run(FORWARD);
  Serial.println((int) leftMotor);
  delay(1000);
  Serial.println("running");
  */
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(100);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

int getLux(void)
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    //Serial.print(event.light); Serial.println(" lux");
    return event.light;
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    //Serial.println("Either zero or overload");
    return 0;
  }
  //delay(10);
}
