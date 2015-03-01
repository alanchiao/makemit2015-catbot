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

void setup() {
  // put your setup code here, to run once:
  // Open up  a pipe
  char * myfifo = "../controller";
  /* open, read, and display the message from the FIFO */
  fd = open(myfifo, O_RDONLY);
}

void loop() { 
  int bytesRead = read(fd, buf, MAX_BUF);
  if (bytesRead == 0) { return; }
  //printf("Bytes read: %d\n", bytesRead);
  int converted = buf[0];
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
      // do motor stuff
      printf("Left Motor Speed: %d\n", leftMotorSpeed);
      printf("Right Motor Speed: %d\n", rightMotorSpeed);
      currState = ZEROBYTES;
    }
    else {
      currState = TWOBYTES;
    }
  }
    
  //printf("Received: %s\n", buf);
}

void main() {
    setup();
    while(1){loop();}
}
