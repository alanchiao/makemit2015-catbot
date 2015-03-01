#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#define MAX_BUF 1
typedef enum {TWOBYTES, ONEBYTE, ZEROBYTES} state;
int fd;
char buf[MAX_BUF];
state currState;
char leftMotorSpeed;
char rightMotorSpeed;
char ALIGNMENT_NUMBER = 255;
Adafruit_DCMotor *leftMotor;
Adafruit_DCMotor *rightMotor;

void setup() {
  // put your setup code here, to run once:
  // Open up  a pipe
  
  char * myfifo = "../controller";
  /* open, read, and display the message from the FIFO */
  fd = open(myfifo, O_RDONLY);
  Adafruit_MotorShield AFMS = Adafruit_MotorShield();
  leftMotor = AFMS.getMotor(1);
  rightMotor = AFMS.getMotor(2);
  AFMS.begin();
}

void loop() {
  // put your main code here, to run repeatedly: 
  // Read from the pipe (3 bytes)
  // We wait until we see a 0 byte
  
  int bytesRead = read(fd, buf, MAX_BUF);
  if (bytesRead == 0) {return;}

  if (currState == ZEROBYTES) {
    if (buf[0] == ALIGNMENT_NUMBER) {
      currState = TWOBYTES;
    }
  }
  else if (currState == TWOBYTES) {
    if (buf[0] != ALIGNMENT_NUMBER) {
      leftMotorSpeed = buf[0];
      currState = ONEBYTE;
    }
    // else it was a new frame so expect 2 more bytes
  }
  else {
    if (buf[0] != ALIGNMENT_NUMBER) {
      rightMotorSpeed = buf[0];
      // do motor stuff
      leftMotor->setSpeed(leftMotorSpeed);
      rightMotor->setSpeed(rightMotorSpeed);
      currState = ZEROBYTES;
    }
    else {
      currState = TWOBYTES;
    }
  }
    
  printf("Received: %s\n", buf);
}
