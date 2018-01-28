import processing.serial.*;
Serial myPort;
boolean pressed = false;
int startedX;
int startedY;
int zeit = 0;

void setup() {
  size(500, 500);
  printArray(Serial.list());
  boolean worked = false;
  while (!worked)
  try {
    myPort = new Serial(this, Serial.list()[1], 115200);
    worked = true;
  }
  catch(ArrayIndexOutOfBoundsException e) {
    println("Please connect the Sender");
    delay(1000);
  }
  line(250, 0, 250, 500);
  line(0, 250, 500, 250);
}
void draw() {
}

void mousePressed() {
  if (mouseButton == LEFT) {
    if (!pressed) {
      pressed = true;
      println("asf");
      startedX = mouseX;
      startedY = mouseY;
    }
  }
}

void keyPressed() {
  if (key == 'r') {
    myPort.write("restart \n");
  }
  if (key == 's') {
    myPort.write("startPos \n");
  }
  if (key == 'e') {
    myPort.write("endPos \n");
  }
  if (key == 'g') {
    myPort.write("goToStart \n");
  }
  if (key == '+') {
    zeit += 1;
    println(zeit);
    myPort.write("setTime, "+zeit+" \n");
  }
  if (key == '-') {
    zeit -= 1;
    println(zeit);
    myPort.write("setTime, "+zeit+" \n");
  }
  if (key == ' ') {
    myPort.write("start \n");
  }
  if (key == CODED) {
    int delX = 0;
    int delY = 0;
    if (keyCode == UP) {
      delY = 1000;
    } else if (keyCode == DOWN) {
      delY = -1000;
    }
    if (keyCode == RIGHT) {
      delX = 100;
    } else if (keyCode == LEFT) {
      delX = -100;
    }
    println(delX+" , "+delY);
    myPort.write(delX+","+delY+" \n");
  }
  
}