import processing.serial.*;
Serial myPort;
boolean pressed = false;
int startedX;
int startedY;

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
  if (mousePressed && mouseButton == LEFT) {
    /*int mX = mouseX - startedX;
     int mY = mouseY - startedY;*/
    int mX = mouseX - 250;
    int mY = mouseY - 250;
    pressed = true;
    int delay1 = 0;
    int delay2 = 0;
    int rot1   = -2560;
    if (mY < 0)
      rot1 = -rot1;
    int rot2   = -200*16;
    if (mX < 0)
      rot2 = -rot2;

    if ((mY/10) != 0 && mouseY <= 500 && mouseY >= 0)
      delay1 = 1000/abs((mY/10));
    else
      rot1 = 0;

    if ((mX/10) != 0 && mouseX <= 500 && mouseX >= 0)
      delay2 = 160000/abs((mX/10));
    else
      rot2 = 0;

    println(delay1+","+delay2+","+rot1+","+rot2);
    myPort.write(delay1+" , "+delay2+" , "+rot1+" , "+rot2+" \n");
    delay(100);
  } else
    pressed = false;
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
  if (key == '1') {
    myPort.write("justRotateX \n");
  }
  if (key == '2') {
    myPort.write("normalX \n");
  }
  if (key == 'r') {
    myPort.write("restart \n");
  }
}